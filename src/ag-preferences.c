#include <gtk/gtk.h>
#include <swe-glib.h>

#include "ag-settings.h"
#include "ag-preferences.h"

static GtkWidget *prefs_dialog = NULL;

typedef struct _AgPreferencesPrivate {
    GtkCheckButton *maximized;
    GtkCheckButton *planet_chars;
    GtkCheckButton *aspect_chars;
    GtkWidget      *house_system;
    GtkListStore   *house_system_model;

    AgSettings *settings;
} AgPreferencesPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(AgPreferences, ag_preferences, GTK_TYPE_DIALOG);

static void
ag_preferences_finalize(GObject *gobject)
{
    AgPreferencesPrivate *priv;

    priv = ag_preferences_get_instance_private(AG_PREFERENCES(gobject));

    g_clear_object(&priv->settings);

    G_OBJECT_CLASS(ag_preferences_parent_class)->finalize(gobject);
}

static void
ag_preferences_response(GtkDialog *dlg, gint response_id)
{
    gtk_widget_destroy(GTK_WIDGET(dlg));
}

static void
ag_preferences_class_init(AgPreferencesClass *klass)
{
    GObjectClass   *object_class = G_OBJECT_CLASS(klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
    GtkDialogClass *dialog_class = GTK_DIALOG_CLASS(klass);

    object_class->finalize = ag_preferences_finalize;
    dialog_class->response = ag_preferences_response;

    gtk_widget_class_set_template_from_resource(
            widget_class,
            "/eu/polonkai/gergely/Astrognome/ui/ag-preferences.ui"
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgPreferences,
            maximized
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgPreferences,
            planet_chars
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgPreferences,
            aspect_chars
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgPreferences,
            house_system
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgPreferences,
            house_system_model
        );
}

static void
ag_preferences_add_house_system(GsweHouseSystemInfo  *house_system_info,
                                AgPreferencesPrivate *priv)
{
    GtkTreeIter iter;
    GEnumClass  *house_system_type = g_type_class_ref(GSWE_TYPE_HOUSE_SYSTEM);
    GEnumValue  *enum_value;

    enum_value = g_enum_get_value(
            house_system_type,
            gswe_house_system_info_get_house_system(house_system_info)
        );
    gtk_list_store_append(priv->house_system_model, &iter);
    gtk_list_store_set(
            priv->house_system_model, &iter,
            0, enum_value->value_nick,
            1, gswe_house_system_info_get_name(house_system_info),
            -1
        );
}

static void
ag_preferences_init(AgPreferences *prefs)
{
    GSettings            *settings_window,
                         *settings_main;
    GList                *house_system_list = gswe_all_house_systems();
    AgPreferencesPrivate *priv = ag_preferences_get_instance_private(prefs);
    GtkCellRenderer      *cell_renderer;

    gtk_widget_init_template(GTK_WIDGET(prefs));
    priv->settings = ag_settings_get();
    g_list_foreach(
            house_system_list,
            (GFunc)ag_preferences_add_house_system,
            priv
        );
    cell_renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(
            GTK_CELL_LAYOUT(priv->house_system),
            cell_renderer,
            TRUE
        );
    gtk_cell_layout_set_attributes(
            GTK_CELL_LAYOUT(priv->house_system),
            cell_renderer,
            "text", 1,
            NULL
        );

    settings_window = ag_settings_peek_window_settings(priv->settings);
    g_settings_bind(
            settings_window,
            "maximized",
            priv->maximized,
            "active",
            G_SETTINGS_BIND_DEFAULT
        );

    settings_main = ag_settings_peek_main_settings(priv->settings);
    g_settings_bind(
            settings_main,
            "planets-char",
            priv->planet_chars,
            "active",
            G_SETTINGS_BIND_DEFAULT
        );
    g_settings_bind(
            settings_main,
            "aspects-char",
            priv->aspect_chars,
            "active",
            G_SETTINGS_BIND_DEFAULT
        );
    g_settings_bind(
            settings_main,
            "default-house-system",
            priv->house_system,
            "active-id",
            G_SETTINGS_BIND_DEFAULT
        );
}

void
ag_preferences_show_dialog(GtkWindow *parent)
{
    g_return_if_fail(GTK_IS_WINDOW(parent));

    if (prefs_dialog == NULL) {
        prefs_dialog = GTK_WIDGET(g_object_new(AG_TYPE_PREFERENCES, NULL));
        g_signal_connect(
                prefs_dialog,
                "destroy",
                G_CALLBACK(gtk_widget_destroyed),
                &prefs_dialog
            );
    }

    if (parent != gtk_window_get_transient_for(GTK_WINDOW(prefs_dialog))) {
        gtk_window_set_transient_for(GTK_WINDOW(prefs_dialog), parent);
    }

    gtk_window_present(GTK_WINDOW(prefs_dialog));
}
