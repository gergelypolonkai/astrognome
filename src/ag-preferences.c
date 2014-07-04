#include <gtk/gtk.h>

#include "ag-settings.h"
#include "ag-preferences.h"

static GtkWidget *prefs_dialog = NULL;

typedef struct _AgPreferencesPrivate {
    GtkCheckButton *maximized;
    GtkCheckButton *planet_chars;
    GtkCheckButton *aspect_chars;

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

    gtk_widget_class_set_template_from_resource(widget_class, "/eu/polonkai/gergely/Astrognome/ui/ag-preferences.ui");
    gtk_widget_class_bind_template_child_private(widget_class, AgPreferences, maximized);
    gtk_widget_class_bind_template_child_private(widget_class, AgPreferences, planet_chars);
    gtk_widget_class_bind_template_child_private(widget_class, AgPreferences, aspect_chars);
}

static void
ag_preferences_init(AgPreferences *prefs)
{
    AgPreferencesPrivate *priv;
    GSettings            *settings_window,
                         *settings_main;

    priv = ag_preferences_get_instance_private(prefs);
    gtk_widget_init_template(GTK_WIDGET(prefs));

    priv->settings = ag_settings_get();

    settings_window = ag_settings_peek_window_settings(priv->settings);
    g_settings_bind(settings_window, "maximized", priv->maximized, "active", G_SETTINGS_BIND_DEFAULT);

    settings_main = ag_settings_peek_main_settings(priv->settings);
    g_settings_bind(settings_main, "planets-char", priv->planet_chars, "active", G_SETTINGS_BIND_DEFAULT);
    g_settings_bind(settings_main, "aspects-char", priv->aspect_chars, "active", G_SETTINGS_BIND_DEFAULT);
}

void
ag_preferences_show_dialog(GtkWindow *parent)
{
    g_return_if_fail(GTK_IS_WINDOW(parent));

    if (prefs_dialog == NULL) {
        prefs_dialog = GTK_WIDGET(g_object_new(AG_TYPE_PREFERENCES, NULL));
        g_signal_connect(prefs_dialog, "destroy", G_CALLBACK(gtk_widget_destroyed), &prefs_dialog);
    }

    if (parent != gtk_window_get_transient_for(GTK_WINDOW(prefs_dialog))) {
        gtk_window_set_transient_for(GTK_WINDOW(prefs_dialog), parent);
    }

    gtk_window_present(GTK_WINDOW(prefs_dialog));
}
