#include <string.h>
#include <glib/gi18n.h>
#include <libgd/gd.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

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

    GsweTimestamp *timestamp;
    AgChart *chart;
    gchar *uri;
};

G_DEFINE_TYPE(AgWindow, ag_window, GTK_TYPE_APPLICATION_WINDOW);

#define GET_PRIVATE(instance) (G_TYPE_INSTANCE_GET_PRIVATE((instance), AG_TYPE_WINDOW, AgWindowPrivate))

static void recalculate_chart(AgWindow *window);

static void
gear_menu_cb(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    GVariant *state;

    state = g_action_get_state(G_ACTION(action));
    g_action_change_state(G_ACTION(action), g_variant_new_boolean(!g_variant_get_boolean(state)));

    g_variant_unref(state);
}

static void
close_cb(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    AgWindow *window = user_data;

    // TODO: Save unsaved changes!
    gtk_widget_destroy(GTK_WIDGET(window));
}

static void
save_cb(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    xmlDocPtr doc = NULL;
    xmlNodePtr root_node = NULL,
               data_node = NULL,
               place_node = NULL,
               time_node = NULL;
    gchar *value;
    GsweCoordinates *coordinates;
    GsweTimestamp *timestamp;
    AgWindow *window = user_data;

    recalculate_chart(window);

    doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "chartinfo");
    xmlDocSetRootElement(doc, root_node);

    // Begin <data> node
    data_node = xmlNewChild(root_node, NULL, BAD_CAST "data", NULL);

    value = ag_chart_get_name(window->priv->chart);
    xmlNewChild(data_node, NULL, BAD_CAST "name", BAD_CAST value);
    g_free(value);

    // Begin <place> node
    place_node = xmlNewChild(data_node, NULL, BAD_CAST "place", NULL);

    value = ag_chart_get_country(window->priv->chart);
    xmlNewChild(place_node, NULL, BAD_CAST "country", BAD_CAST value);
    g_free(value);

    value = ag_chart_get_city(window->priv->chart);
    xmlNewChild(place_node, NULL, BAD_CAST "city", BAD_CAST value);
    g_free(value);

    coordinates = gswe_moment_get_coordinates(GSWE_MOMENT(window->priv->chart));

    value = g_malloc0(12);
    g_ascii_dtostr(value, 12, coordinates->longitude);
    xmlNewChild(place_node, NULL, BAD_CAST "longitude", BAD_CAST value);
    g_free(value);

    value = g_malloc0(12);
    g_ascii_dtostr(value, 12, coordinates->latitude);
    xmlNewChild(place_node, NULL, BAD_CAST "latitude", BAD_CAST value);
    g_free(value);

    value = g_malloc0(12);
    g_ascii_dtostr(value, 12, coordinates->altitude);
    xmlNewChild(place_node, NULL, BAD_CAST "altitude", BAD_CAST value);
    g_free(value);

    g_free(coordinates);

    // Begin <time> node
    time_node = xmlNewChild(root_node, NULL, BAD_CAST "time", NULL);

    timestamp = gswe_moment_get_timestamp(GSWE_MOMENT(window->priv->chart));

    value = g_malloc0(10);
    g_ascii_dtostr(value, 10, gswe_timestamp_get_gregorian_year(timestamp, NULL));
    xmlNewChild(time_node, NULL, BAD_CAST "year", BAD_CAST value);
    g_free(value);

    value = g_malloc0(3);
    g_ascii_dtostr(value, 3, gswe_timestamp_get_gregorian_month(timestamp, NULL));
    xmlNewChild(time_node, NULL, BAD_CAST "month", BAD_CAST value);
    g_free(value);

    value = g_malloc0(3);
    g_ascii_dtostr(value, 3, gswe_timestamp_get_gregorian_day(timestamp, NULL));
    xmlNewChild(time_node, NULL, BAD_CAST "year", BAD_CAST value);
    g_free(value);

    value = g_malloc0(3);
    g_ascii_dtostr(value, 3, gswe_timestamp_get_gregorian_hour(timestamp, NULL));
    xmlNewChild(time_node, NULL, BAD_CAST "hour", BAD_CAST value);
    g_free(value);

    value = g_malloc0(3);
    g_ascii_dtostr(value, 3, gswe_timestamp_get_gregorian_minute(timestamp, NULL));
    xmlNewChild(time_node, NULL, BAD_CAST "minute", BAD_CAST value);
    g_free(value);

    value = g_malloc0(3);
    g_ascii_dtostr(value, 3, gswe_timestamp_get_gregorian_second(timestamp, NULL));
    xmlNewChild(time_node, NULL, BAD_CAST "year", BAD_CAST value);
    g_free(value);

    value = g_malloc0(7);
    g_ascii_dtostr(value, 7, gswe_timestamp_get_gregorian_timezone(timestamp));
    xmlNewChild(time_node, NULL, BAD_CAST "timezone", BAD_CAST value);
    g_free(value);

    xmlSaveFormatFileEnc("-", doc, "UTF-8", 1);
    xmlFreeDoc(doc);
}

static void
save_as_cb(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
}

void
ag_window_redraw_chart(AgWindow *window)
{
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
    AgWindowPrivate *priv = window->priv;
    gint year   = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(priv->year)),
         month  = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(priv->month)),
         day    = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(priv->day)),
         hour   = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(priv->hour)),
         minute = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(priv->minute)),
         second = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(priv->second));
    gdouble longitude = gtk_spin_button_get_value(GTK_SPIN_BUTTON(priv->longitude)),
            latitude  = gtk_spin_button_get_value(GTK_SPIN_BUTTON(priv->latitude));
    gdouble south = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->south_lat)),
            west  = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->west_long));

    if (south) {
        latitude = 0 - latitude;
    }

    if (west) {
        longitude = 0 - longitude;
    }

    // TODO: Set timezone according to the city selected!
    if (priv->timestamp == NULL) {
        priv->timestamp = gswe_timestamp_new_from_gregorian_full(year, month, day, hour, minute, second, 0, 1.0);
    } else {
        gswe_timestamp_set_gregorian_full(priv->timestamp, year, month, day, hour, minute, second, 0, 1.0, NULL);
    }

    if (priv->chart == NULL) {
        // TODO: make house system configurable
        priv->chart = ag_chart_new_full(priv->timestamp, longitude, latitude, 380.0, GSWE_HOUSE_SYSTEM_PLACIDUS);
        ag_chart_set_name(priv->chart, gtk_entry_get_text(GTK_ENTRY(priv->name)));
        g_signal_connect(priv->chart, "changed", G_CALLBACK(chart_changed), window);
        ag_window_update_from_chart(window);
    } else {
        gswe_moment_set_timestamp(GSWE_MOMENT(priv->chart), priv->timestamp);
    }
}

static void
tab_changed_cb(GdStack *stack, GParamSpec *pspec, AgWindow *window)
{
    const gchar *active_tab_name = gd_stack_get_visible_child_name(stack);

    g_debug("Active tab changed: %s", active_tab_name);

    if (active_tab_name == NULL) {
        return;
    }

    // Note that priv->current_tab is actually the previously selected tab, not the real active one!
    if (window->priv->current_tab == window->priv->tab_edit) {
        g_debug("Recalculating chart data");
        recalculate_chart(window);
    }

    window->priv->current_tab = gd_stack_get_visible_child(stack);
}

static GActionEntry win_entries[] = {
    { "close",      close_cb,     NULL, NULL,      NULL },
    { "save",       save_cb,      NULL, NULL,      NULL },
    { "save-as",    save_as_cb,   NULL, NULL,      NULL },
    { "gear-menu",  gear_menu_cb, NULL, "false",   NULL },
};

static void
ag_window_init(AgWindow *window)
{
    AgWindowPrivate *priv;
    GtkAccelGroup *accel_group;
    GError *err = NULL;

    window->priv = priv = GET_PRIVATE(window);

    priv->timestamp = NULL;
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
    GtkWidget *menu_button;
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

    priv->tab_chart = gtk_label_new("PLACEHOLDER FOR THE CHART WEBKIT");
    gd_stack_add_titled(GD_STACK(priv->stack), priv->tab_chart, "chart", _("Chart"));

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

