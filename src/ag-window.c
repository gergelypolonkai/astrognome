#include <string.h>
#include <glib/gi18n.h>
#include <libgd/gd.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <webkit/webkit.h>

#include <swe-glib.h>

#include "ag-app.h"
#include "ag-window.h"
#include "ag-chart.h"

struct _AgWindowPrivate {
    GtkWidget *grid;
    GtkWidget *header_bar;
    GtkWidget *stack;
    GtkWidget *stack_switcher;
    GtkWidget *name;
    GtkWidget *north_lat;
    GtkWidget *south_lat;
    GtkWidget *latitude;
    GtkWidget *east_long;
    GtkWidget *west_long;
    GtkWidget *longitude;
    GtkWidget *year;
    GtkWidget *month;
    GtkWidget *day;
    GtkWidget *hour;
    GtkWidget *minute;
    GtkWidget *second;
    GtkBuilder *builder;

    GtkWidget *tab_chart;
    GtkWidget *tab_aspects;
    GtkWidget *tab_points;
    GtkWidget *tab_edit;
    GtkWidget *current_tab;

    AgChart *chart;
    gchar *uri;
};

G_DEFINE_QUARK(ag-window-error-quark, ag_window_error);

G_DEFINE_TYPE(AgWindow, ag_window, GTK_TYPE_APPLICATION_WINDOW);

#define GET_PRIVATE(instance) (G_TYPE_INSTANCE_GET_PRIVATE((instance), AG_TYPE_WINDOW, AgWindowPrivate))

static void recalculate_chart(AgWindow *window);

static void
ag_window_gear_menu_action(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    GVariant *state;

    state = g_action_get_state(G_ACTION(action));
    g_action_change_state(G_ACTION(action), g_variant_new_boolean(!g_variant_get_boolean(state)));

    g_variant_unref(state);
}

static void
ag_window_close_action(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    AgWindow *window = user_data;

    // TODO: Save unsaved changes!
    gtk_widget_destroy(GTK_WIDGET(window));
}

static void
ag_window_save_as(AgWindow *window, GError **err)
{
    gchar *name,
          *file_name;
    GtkWidget *fs;
    gint response;

    recalculate_chart(window);

    // We should never enter here, but who knows...
    if (window->priv->chart == NULL) {
        g_set_error(err, AG_WINDOW_ERROR, AG_WINDOW_ERROR_EMPTY_CHART, "Chart is empty");

        return;
    }

    name = ag_chart_get_name(window->priv->chart);

    if ((name == NULL) || (*name == 0)) {
        g_free(name);
        g_set_error(err, AG_WINDOW_ERROR, AG_WINDOW_ERROR_NO_NAME, "No name specified");

        return;
    }

    file_name = g_strdup_printf("%s.agc", name);
    g_free(name);

    fs = gtk_file_chooser_dialog_new(_("Save Chart"),
            GTK_WINDOW(window),
            GTK_FILE_CHOOSER_ACTION_SAVE,
            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
            GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
            NULL);
    gtk_dialog_set_default_response(GTK_DIALOG(fs), GTK_RESPONSE_ACCEPT);
    gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(fs), FALSE);
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(fs), TRUE);
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(fs), file_name);
    g_free(file_name);

    response = gtk_dialog_run(GTK_DIALOG(fs));
    gtk_widget_hide(fs);

    if (response == GTK_RESPONSE_ACCEPT) {
        GFile *file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(fs));

        ag_chart_save_to_file(window->priv->chart, file, err);
    }

    gtk_widget_destroy(fs);
}

static void
ag_window_save_action(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    AgWindow *window = AG_WINDOW(user_data);
    GError *err = NULL;
    gchar *uri;

    recalculate_chart(window);
    uri = ag_window_get_uri(window);

    if (uri != NULL) {
        GFile *file = g_file_new_for_uri(uri);
        g_free(uri);

        ag_chart_save_to_file(window->priv->chart, file, &err);
    } else {
        ag_window_save_as(window, &err);
    }

    // TODO: Check err!
}

static void
ag_window_save_as_action(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    AgWindow *window = AG_WINDOW(user_data);
    GError *err = NULL;

    recalculate_chart(window);
    ag_window_save_as(window, &err);

    // TODO: Check err!
}

void
ag_window_redraw_chart(AgWindow *window)
{
    GError *err = NULL;
    gchar *svg_content;

    svg_content = ag_chart_create_svg(window->priv->chart, &err);

    if (svg_content == NULL) {
        g_warning("%s", err->message);
    } else {
        webkit_web_view_load_string(WEBKIT_WEB_VIEW(window->priv->tab_chart), svg_content, "image/svg+xml", "UTF-8", "file://");
        g_free(svg_content);
    }
}

void
ag_window_update_from_chart(AgWindow *window)
{
    GsweTimestamp *timestamp;
    GsweCoordinates *coordinates;

    timestamp = gswe_moment_get_timestamp(GSWE_MOMENT(window->priv->chart));
    coordinates = gswe_moment_get_coordinates(GSWE_MOMENT(window->priv->chart));

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(window->priv->year), gswe_timestamp_get_gregorian_year(timestamp, NULL));
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(window->priv->month), gswe_timestamp_get_gregorian_month(timestamp, NULL));
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(window->priv->day), gswe_timestamp_get_gregorian_day(timestamp, NULL));
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(window->priv->hour), gswe_timestamp_get_gregorian_hour(timestamp, NULL));
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(window->priv->minute), gswe_timestamp_get_gregorian_minute(timestamp, NULL));
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(window->priv->second), gswe_timestamp_get_gregorian_second(timestamp, NULL));
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(window->priv->longitude), coordinates->longitude);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(window->priv->latitude), coordinates->latitude);
    gtk_entry_set_text(GTK_ENTRY(window->priv->name), ag_chart_get_name(window->priv->chart));

    g_free(coordinates);

    ag_window_redraw_chart(window);
}

static void
chart_changed(AgChart *chart, AgWindow *window)
{
    ag_window_redraw_chart(window);
}

static void
recalculate_chart(AgWindow *window)
{
    gint year,
         month,
         day,
         hour,
         minute,
         second;
    gdouble longitude,
            latitude;
    gboolean south,
             west;
    GsweTimestamp *timestamp;

    g_debug("Recalculating chart data");

    year = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(window->priv->year));
    month = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(window->priv->month));
    day = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(window->priv->day));
    hour = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(window->priv->hour));
    minute = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(window->priv->minute));
    second = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(window->priv->second));
    longitude = gtk_spin_button_get_value(GTK_SPIN_BUTTON(window->priv->longitude));
    latitude = gtk_spin_button_get_value(GTK_SPIN_BUTTON(window->priv->latitude));

    if ((south = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(window->priv->south_lat)))) {
        latitude = 0 - latitude;
    }

    if ((west = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(window->priv->west_long)))) {
        longitude = 0 - longitude;
    }

    // TODO: Set timezone according to the city selected!
    if (window->priv->chart == NULL) {
        timestamp = gswe_timestamp_new_from_gregorian_full(year, month, day, hour, minute, second, 0, 1.0);
        // TODO: make house system configurable
        window->priv->chart = ag_chart_new_full(timestamp, longitude, latitude, 380.0, GSWE_HOUSE_SYSTEM_PLACIDUS);
        g_signal_connect(window->priv->chart, "changed", G_CALLBACK(chart_changed), window);
    } else {
        timestamp = gswe_moment_get_timestamp(GSWE_MOMENT(window->priv->chart));
        gswe_timestamp_set_gregorian_full(timestamp, year, month, day, hour, minute, second, 0, 1.0, NULL);
    }

    ag_chart_set_name(window->priv->chart, gtk_entry_get_text(GTK_ENTRY(window->priv->name)));
    ag_window_redraw_chart(window);
}

static void
tab_changed_cb(GdStack *stack, GParamSpec *pspec, AgWindow *window)
{
    const gchar *active_tab_name = gd_stack_get_visible_child_name(stack);
    GtkWidget *active_tab;

    g_debug("Active tab changed: %s", active_tab_name);

    if (active_tab_name == NULL) {
        return;
    }

    active_tab = gd_stack_get_visible_child(stack);

    if (strcmp("chart", active_tab_name) == 0) {
        gtk_widget_set_size_request(active_tab, 600, 600);
    }

    // Note that priv->current_tab is actually the previously selected tab, not the real active one!
    if (window->priv->current_tab == window->priv->tab_edit) {
        recalculate_chart(window);
    }

    window->priv->current_tab = active_tab;
}

static void
ag_window_change_tab_action(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    AgWindow *window = user_data;
    const gchar *target_tab = g_variant_get_string(parameter, NULL);

    gd_stack_set_visible_child_name(GD_STACK(window->priv->stack), target_tab);
    g_action_change_state(G_ACTION(action), parameter);
}

static GActionEntry win_entries[] = {
    { "close",      ag_window_close_action,      NULL, NULL,      NULL },
    { "save",       ag_window_save_action,       NULL, NULL,      NULL },
    { "save-as",    ag_window_save_as_action,    NULL, NULL,      NULL },
    { "gear-menu",  ag_window_gear_menu_action,  NULL, "false",   NULL },
    { "change-tab", ag_window_change_tab_action, "s",  "'edit'",  NULL },
};

static void
ag_window_init(AgWindow *window)
{
    AgWindowPrivate *priv;
    GtkAccelGroup *accel_group;
    GError *err = NULL;

    window->priv = priv = GET_PRIVATE(window);

    priv->chart = NULL;
    priv->uri = NULL;

    gtk_window_set_hide_titlebar_when_maximized(GTK_WINDOW(window), TRUE);

    priv->builder = gtk_builder_new();

    if (!gtk_builder_add_from_resource(priv->builder, "/eu/polonkai/gergely/astrognome/astrognome.ui", &err)) {
        g_error("Cannot add resource to builder: '%s'", (err) ? err->message : "unknown error");
        g_clear_error(&err);
    }

    priv->grid = gtk_grid_new();
    gtk_widget_show(priv->grid);

    gtk_container_add(GTK_CONTAINER(window), priv->grid);

    g_action_map_add_action_entries(G_ACTION_MAP(window), win_entries, G_N_ELEMENTS(win_entries), window);

    accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
}

static void
ag_window_class_init(AgWindowClass *klass)
{
    g_type_class_add_private(klass, sizeof(AgWindowPrivate));
}

static GtkWidget *
notebook_edit(AgWindow *window)
{
    GtkWidget *grid;
    GtkWidget *label;
    AgWindowPrivate *priv = window->priv;

    grid = gtk_grid_new();

    label = gtk_label_new(_("Name"));
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    priv->name = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), priv->name, 1, 0, 6, 1);

    label = gtk_label_new(_("Country"));
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);

    label = gtk_label_new(_("City"));
    gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);

    label = gtk_label_new(_("Latitude"));
    gtk_grid_attach(GTK_GRID(grid), label, 0, 3, 2, 1);

    priv->north_lat = gtk_radio_button_new_with_label(NULL, _("North"));
    gtk_grid_attach(GTK_GRID(grid), priv->north_lat, 0, 4, 1, 1);

    priv->south_lat = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(priv->north_lat), _("South"));
    gtk_grid_attach(GTK_GRID(grid), priv->south_lat, 1, 4, 1, 1);

    priv->latitude = gtk_spin_button_new_with_range(0.0, 90.0, 0.1);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(priv->latitude), 6);
    gtk_grid_attach(GTK_GRID(grid), priv->latitude, 0, 5, 2, 1);

    label = gtk_label_new(_("Longitude"));
    gtk_grid_attach(GTK_GRID(grid), label, 2, 3, 2, 1);

    priv->east_long = gtk_radio_button_new_with_label(NULL, _("East"));
    gtk_grid_attach(GTK_GRID(grid), priv->east_long, 2, 4, 1, 1);

    priv->west_long = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(priv->east_long), _("West"));
    gtk_grid_attach(GTK_GRID(grid), priv->west_long, 3, 4, 1, 1);

    priv->longitude = gtk_spin_button_new_with_range(0.0, 180.0, 0.1);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(priv->longitude), 6);
    gtk_grid_attach(GTK_GRID(grid), priv->longitude, 2, 5, 2, 1);

    label = gtk_label_new(_("Year"));
    gtk_grid_attach(GTK_GRID(grid), label, 4, 1, 1, 1);

    priv->year = gtk_spin_button_new_with_range(G_MININT, G_MAXINT, 1);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(priv->year), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(priv->year), 0);
    gtk_grid_attach(GTK_GRID(grid), priv->year, 4, 2, 1, 1);

    label = gtk_label_new(_("Month"));
    gtk_grid_attach(GTK_GRID(grid), label, 5, 1, 1, 1);

    priv->month = gtk_spin_button_new_with_range(1, 12, 1);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(priv->month), 0);
    gtk_grid_attach(GTK_GRID(grid), priv->month, 5, 2, 1, 1);

    label = gtk_label_new(_("Day"));
    gtk_grid_attach(GTK_GRID(grid), label, 6, 1, 1, 1);

    priv->day = gtk_spin_button_new_with_range(1, 31, 1);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(priv->day), 0);
    gtk_grid_attach(GTK_GRID(grid), priv->day, 6, 2, 1, 1);

    label = gtk_label_new(_("Hour"));
    gtk_grid_attach(GTK_GRID(grid), label, 4, 3, 1, 1);

    priv->hour = gtk_spin_button_new_with_range(0, 23, 1);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(priv->hour), 0);
    gtk_grid_attach(GTK_GRID(grid), priv->hour, 4, 4, 1, 1);

    label = gtk_label_new(_("Minute"));
    gtk_grid_attach(GTK_GRID(grid), label, 5, 3, 1, 1);

    priv->minute = gtk_spin_button_new_with_range(0, 59, 1);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(priv->minute), 0);
    gtk_grid_attach(GTK_GRID(grid), priv->minute, 5, 4, 1, 1);

    label = gtk_label_new(_("Second"));
    gtk_grid_attach(GTK_GRID(grid), label, 6, 3, 1, 1);

    priv->second = gtk_spin_button_new_with_range(0, 61, 1);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(priv->second), 0);
    gtk_grid_attach(GTK_GRID(grid), priv->second, 6, 4, 1, 1);

    gtk_widget_show_all(grid);

    return grid;
}

static void
window_populate(AgWindow *window)
{
    AgWindowPrivate *priv = window->priv;
    GtkWidget *menu_button,
              *scroll;
    GObject *menu;

    priv->header_bar = gd_header_bar_new();
    menu_button = gd_header_menu_button_new();
    gd_header_button_set_symbolic_icon_name(GD_HEADER_BUTTON(menu_button), "emblem-system-symbolic");
    gtk_actionable_set_action_name(GTK_ACTIONABLE(menu_button), "win.gear-menu");

    gd_header_bar_pack_end(GD_HEADER_BAR(priv->header_bar), menu_button);

    gtk_grid_attach(GTK_GRID(priv->grid), priv->header_bar, 0, 0, 1, 1);

    menu = gtk_builder_get_object(priv->builder, "window-menu");
    gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(menu_button), G_MENU_MODEL(menu));

    priv->stack = gd_stack_new();
    gtk_grid_attach(GTK_GRID(priv->grid), priv->stack, 0, 1, 1, 1);
    g_signal_connect(priv->stack, "notify::visible-child", G_CALLBACK(tab_changed_cb), window);

    priv->stack_switcher = gd_stack_switcher_new();
    gd_stack_switcher_set_stack(GD_STACK_SWITCHER(priv->stack_switcher), GD_STACK(priv->stack));

    priv->tab_edit = notebook_edit(window);
    gd_stack_add_titled(GD_STACK(priv->stack), priv->tab_edit, "edit", _("Edit"));

    scroll = gtk_scrolled_window_new(NULL, NULL);
    g_object_set(scroll, "shadow-type", GTK_SHADOW_IN, NULL);
    gd_stack_add_titled(GD_STACK(priv->stack), scroll, "chart", _("Chart"));

    priv->tab_chart = webkit_web_view_new();
    gtk_container_add(GTK_CONTAINER(scroll), priv->tab_chart);
    webkit_web_view_load_string(WEBKIT_WEB_VIEW(priv->tab_chart), "<html><head><title>No Chart</title></head><body><h1>No Chart</h1><p>No chart is loaded. Create one on the edit view, or open one from the application menu!</p></body></html>", "text/html", "UTF-8", NULL);
    gtk_widget_set_size_request(priv->tab_chart, 600, 600);

    priv->tab_aspects = gtk_label_new("PLACEHOLDER FOR THE ASPECTS TABLE");
    gd_stack_add_titled(GD_STACK(priv->stack), priv->tab_aspects, "aspects", _("Aspects"));

    priv->tab_points = gtk_label_new("PLACEHOLDER FOR THE POINTS TABLES");
    gd_stack_add_titled(GD_STACK(priv->stack), priv->tab_points, "points", _("Points"));

    /* TODO: change to the Chart tab if we are opening an existing chart! */
    gd_stack_set_visible_child_name(GD_STACK(priv->stack), "edit");
    priv->current_tab = priv->tab_edit;

    gd_header_bar_set_custom_title(GD_HEADER_BAR(priv->header_bar), priv->stack_switcher);

    gtk_widget_show_all(priv->grid);
}

GtkWidget *
ag_window_new(AgApp *app)
{
    AgWindow *window;

    window = g_object_new(AG_TYPE_WINDOW, NULL);

    gtk_window_set_application(GTK_WINDOW(window), GTK_APPLICATION(app));

    window_populate(window);

    gtk_window_set_icon_name(GTK_WINDOW(window), "astrognome");

    return GTK_WIDGET(window);
}

void
ag_window_set_chart(AgWindow *window, AgChart *chart)
{
    if (window->priv->chart != NULL) {
        g_signal_handlers_disconnect_by_func(window->priv->chart, chart_changed, window);
        g_object_unref(window->priv->chart);
    }

    window->priv->chart = chart;
    g_signal_connect(window->priv->chart, "changed", G_CALLBACK(chart_changed), window);
    g_object_ref(chart);
}

AgChart *
ag_window_get_chart(AgWindow *window)
{
    return window->priv->chart;
}

void
ag_window_set_uri(AgWindow *window, const gchar *uri)
{
    if (window->priv->uri != NULL) {
        g_free(window->priv->uri);
    }

    window->priv->uri = g_strdup(uri);
}

gchar *
ag_window_get_uri(AgWindow *window)
{
    return g_strdup(window->priv->uri);
}

