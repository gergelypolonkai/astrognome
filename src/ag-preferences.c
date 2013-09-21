#include <glib.h>
#include <gtk/gtk.h>

#include "ag-settings.h"

typedef struct {
    GtkWidget  *dialog;
    AgSettings *settings;
} AgPreferences;

static AgPreferences *prefs;

static void
ag_preferences_init(void)
{
    GApplication *app;

    if (prefs) {
        return;
    }

    if ((app = g_application_get_default()) == NULL) {
        g_warning("Cannot launch preferences: No default application found");

        return;
    }

    prefs = g_new0(AgPreferences, 1);
    prefs->settings = ag_settings_get();
}

void
ag_preferences_show_dialog(void)
{
    ag_preferences_init();

    if (prefs->dialog != NULL) {
        gtk_window_present(GTK_WINDOW(prefs->dialog));

        return;
    }
}

