#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <libxml/xmlversion.h>
#include <libxml/parser.h>
#include <libxslt/xslt.h>
#include <libxslt/transform.h>
#include <libexslt/exslt.h>

#include <libgd/gd.h>

#include <swe-glib.h>

#include "ag-app.h"
#include "ag-window.h"

#define UI_FILE PKGDATADIR "/astrognome.ui"

GtkBuilder *builder;
static gboolean option_version,
                option_quit,
                option_new_window;
GtkFileFilter *filter_all = NULL,
              *filter_chart = NULL;

const char *moonStateName[] = {
    "New Moon",
    "Waxing Crescent Moon",
    "Waxing Half Moon",
    "Waxing Gibbous Moon",
    "Full Moon",
    "Waning Gibbous Moon",
    "Waning Half Moon",
    "Waning Crescent Moon",
    "Dark Moon"
};

static void
run_action(AgApp *app, gboolean is_remote)
{
    if (option_new_window) {
        if (is_remote) {
            ag_app_new_window(app);
        }
    } else if (option_quit) {
        ag_app_quit(app);
    } else if (is_remote) { // Keep this option the last one!
        ag_app_raise(app);
    }
}

static void
application_activate_cb(AgApp *app, gpointer user_data)
{
    ag_app_new_window(app);
    run_action(app, FALSE);
}

void
init_filters(void)
{
    filter_all = gtk_file_filter_new();
    gtk_file_filter_set_name(filter_all, _("All files"));
    gtk_file_filter_add_pattern(filter_all, "*");
    g_object_ref_sink(filter_all);

    filter_chart = gtk_file_filter_new();
    gtk_file_filter_set_name(filter_chart, _("Astrognome charts"));
    gtk_file_filter_add_pattern(filter_chart, "*.agc");
    g_object_ref_sink(filter_chart);
}

int
main(int argc, char *argv[])
{
    gint status;
    AgApp *app;
    GError *err = NULL;

    GOptionEntry options[] = {
        { "new-window", 'n', 0, G_OPTION_ARG_NONE, &option_new_window, N_("Opens a new Astrognome window"), NULL },
        { "version",    'v', 0, G_OPTION_ARG_NONE, &option_version,    N_("Display version and exit"),      NULL },
        { "quit",       'q', 0, G_OPTION_ARG_NONE, &option_quit,       N_("Quit any running Astrognome"),   NULL },
        { NULL }
    };

#ifdef ENABLE_NLS
    bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    textdomain(GETTEXT_PACKAGE);
#endif

    LIBXML_TEST_VERSION;
    xmlSubstituteEntitiesDefault(1);
    xmlLoadExtDtdDefaultValue = 1;
    xsltInit();
    xsltSetXIncludeDefault(1);
    exsltRegisterAll();
    gswe_init();

    option_version = FALSE,
    option_quit = FALSE,
    option_new_window = FALSE;

    if (!gtk_init_with_args(&argc, &argv, _("[FILE…]"), options, GETTEXT_PACKAGE, &err)) {
        g_printerr("%s\n", err->message);

        return EXIT_FAILURE;
    }

    if (option_version) {
        g_print("%s\n", PACKAGE_STRING);

        return EXIT_SUCCESS;
    }

    init_filters();

    app = ag_app_new();
    g_signal_connect(app, "activate", G_CALLBACK(application_activate_cb), NULL);
    g_application_set_default(G_APPLICATION(app));

    if (!g_application_register(G_APPLICATION(app), NULL, &err)) {
        g_printerr("Couldn't register Astrognome instance: '%s'\n", (err) ? err->message : "");
        g_object_unref(app);

        return EXIT_FAILURE;
    }

    if (g_application_get_is_remote(G_APPLICATION(app))) {
        run_action(app, TRUE);
        g_object_unref(app);

        return EXIT_SUCCESS;
    }

    status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);

    return status;
}

