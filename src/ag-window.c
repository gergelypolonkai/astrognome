#include <string.h>
#include <libgd/gd.h>

#include "ag-app.h"
#include "ag-window.h"

struct _AgWindowPrivate {
    GtkWidget *grid;
    GtkWidget *header_bar;
    GtkWidget *notebook;
    GtkBuilder *builder;

    gint tab_chart;
    gint tab_aspects;
    gint tab_points;
};

G_DEFINE_TYPE(AgWindow, ag_window, GTK_TYPE_APPLICATION_WINDOW);

#define GET_PRIVATE(instance) (G_TYPE_INSTANCE_GET_PRIVATE((instance), AG_TYPE_WINDOW, AgWindowPrivate))

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
set_tab_cb(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    AgWindow *window = AG_WINDOW(user_data);
    const gchar *target = g_variant_get_string(parameter, NULL);
    gint target_tab = 0;

    g_action_change_state(G_ACTION(action), parameter);

    if (strcmp(target, "chart") == 0) {
        target_tab = window->priv->tab_chart;
    } else if (strcmp(target, "aspects") == 0) {
        target_tab = window->priv->tab_aspects;
    } else if (strcmp(target, "points") == 0) {
        target_tab = window->priv->tab_points;
    } else {
        g_warning("Unknown tab!");

        return;
    }

    gtk_notebook_set_current_page(GTK_NOTEBOOK(window->priv->notebook), target_tab);
}

static void
change_tab_cb(GSimpleAction *action, GVariant *state, gpointer user_data)
{
    g_simple_action_set_state(action, state);
}

static GActionEntry win_entries[] = {
    { "close",      close_cb,     NULL, NULL,      NULL },
    { "gear-menu",  gear_menu_cb, NULL, "false",   NULL },
    { "tab-change", set_tab_cb,   "s",  "'chart'", change_tab_cb },
};

static void
ag_window_init(AgWindow *window)
{
    AgWindowPrivate *priv;
    GtkAccelGroup *accel_group;
    GError *err = NULL;

    window->priv = priv = GET_PRIVATE(window);

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

static void
window_populate(AgWindow *window)
{
    AgWindowPrivate *priv = window->priv;
    GtkWidget *menu_button;
    GObject *menu;
    GtkWidget *placeholder;

    priv->header_bar = gd_header_bar_new();
    menu_button = gd_header_menu_button_new();
    gd_header_button_set_symbolic_icon_name(GD_HEADER_BUTTON(menu_button), "emblem-system-symbolic");
    gtk_actionable_set_action_name(GTK_ACTIONABLE(menu_button), "win.gear-menu");

    gd_header_bar_pack_end(GD_HEADER_BAR(priv->header_bar), menu_button);

    gtk_grid_attach(GTK_GRID(priv->grid), priv->header_bar, 0, 0, 1, 1);

    menu = gtk_builder_get_object(priv->builder, "window-menu");
    gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(menu_button), G_MENU_MODEL(menu));

    priv->notebook = gtk_notebook_new();
    gtk_grid_attach(GTK_GRID(priv->grid), priv->notebook, 0, 1, 1, 1);

    placeholder = gtk_label_new("PLACEHOLDER FOR THE CHART WEBKIT");
    priv->tab_chart = gtk_notebook_append_page(GTK_NOTEBOOK(priv->notebook), placeholder, NULL);

    placeholder = gtk_label_new("PLACEHOLDER FOR THE ASPECTS TABLE");
    priv->tab_aspects = gtk_notebook_append_page(GTK_NOTEBOOK(priv->notebook), placeholder, NULL);

    placeholder = gtk_label_new("PLACEHOLDER FOR THE POINTS TABLES");
    priv->tab_points = gtk_notebook_append_page(GTK_NOTEBOOK(priv->notebook), placeholder, NULL);

    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(priv->notebook), FALSE);

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

