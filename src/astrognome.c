#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <libxml/xmlversion.h>
#include <libxml/parser.h>
#include <libxslt/xslt.h>
#include <libxslt/transform.h>
#include <libexslt/exslt.h>

#include <swe-glib.h>

#include "config.h"

#include "ag-app.h"
#include "ag-window.h"

GtkBuilder    *builder;
GtkFileFilter *filter_all   = NULL;
GtkFileFilter *filter_chart = NULL;
GHashTable    *xinclude_positions;

const char    *moonStateName[] = {
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

static int
xml_match_gresource(const char *uri)
{
    if ((uri != NULL) && (!strncmp("gres://", uri, 7))) {
        g_debug("Matched gres:// type link.");

        return 1;
    } else {
        return 0;
    }
}

static void *
xml_open_gresource(const gchar *uri)
{
    gchar  *path;
    GBytes *res_location;
    gsize  *position;

    if ((uri == NULL) || (strncmp("gres://", uri, 7))) {
        return NULL;
    }

    path = g_strdup_printf("/eu/polonkai/gergely/Astrognome/%s", uri + 7);
    g_debug("Opening gresource %s", path);

    res_location = g_resources_lookup_data(
            path,
            G_RESOURCE_LOOKUP_FLAGS_NONE,
            NULL
        );
    g_free(path);

    if ((position = g_hash_table_lookup(
                 xinclude_positions,
                 res_location
             )) == NULL) {
        g_hash_table_insert(xinclude_positions, res_location, g_new0(gsize, 1));
    } else {
        g_warning("Reopening gres:// link?");
        *position = 0;
    }

    return res_location;
}

static int
xml_close_gresource(void *context)
{
    if (context == NULL) {
        return -1;
    }

    g_debug("Closing gres:// link");

    g_hash_table_remove(xinclude_positions, context);
    g_bytes_unref((GBytes *)context);

    return 0;
}

static int
xml_read_gresource(void *context, char *buffer, int len)
{
    const gchar *data;
    gsize       max_length;
    GBytes      *data_holder = (GBytes *)context;
    gsize       *position;

    if ((context == NULL) || (buffer == NULL) || (len < 0)) {
        return -1;
    }

    data     = g_bytes_get_data(data_holder, &max_length);
    position = g_hash_table_lookup(xinclude_positions, data_holder);

    if (position == NULL) {
        g_warning("Trying to read non-opened gres:// link!");

        return -1;
    }

    if (*position >= max_length) {
        return 0;
    }

    if (len > max_length - *position) {
        len = max_length - *position;
    }

    memcpy(buffer, data + *position, len);
    g_debug("Read %d bytes", len);
    *position += len;

    return len;
}

int
main(int argc, char *argv[])
{
    gint              status;
    AgApp             *app;
    GError            *err = NULL;
    AstrognomeOptions options;

    GOptionEntry      option_entries[] = {
        {
                "new-window", 'n',
                0, G_OPTION_ARG_NONE,
                &(options.new_window),
                N_("Opens a new Astrognome window"),
                NULL
        },
        {
                "version",    'v',
                0, G_OPTION_ARG_NONE,
                &(options.version),
                N_("Display version and exit"),
                NULL
        },
        {
                "quit",       'q',
                0, G_OPTION_ARG_NONE,
                &(options.quit),
                N_("Quit any running Astrognome"),
                NULL
        },
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
    xmlRegisterInputCallbacks(
            xml_match_gresource,
            xml_open_gresource,
            xml_read_gresource,
            xml_close_gresource
        );
    xsltInit();
    xsltSetXIncludeDefault(1);
    exsltRegisterAll();
    gswe_init();
    xinclude_positions = g_hash_table_new_full(
            g_bytes_hash,
            g_bytes_equal,
            (GDestroyNotify)g_bytes_unref,
            (GDestroyNotify)g_free
        );

    memset(&options, 0, sizeof(AstrognomeOptions));

    if (!gtk_init_with_args(
                &argc, &argv,
                _("[FILE…]"
            ), option_entries, GETTEXT_PACKAGE, &err)) {
        g_printerr("%s\n", err->message);

        return EXIT_FAILURE;
    }

    if (options.version) {
        g_print("%s\n", PACKAGE_STRING);

        return EXIT_SUCCESS;
    }

    init_filters();

    app = ag_app_new();
    g_application_set_default(G_APPLICATION(app));

    if (!g_application_register(G_APPLICATION(app), NULL, &err)) {
        g_printerr(
                "Couldn't register Astrognome instance: '%s'\n",
                (err) ? err->message : ""
            );
        g_object_unref(app);

        return EXIT_FAILURE;
    }

    if (g_application_get_is_remote(G_APPLICATION(app))) {
        ag_app_run_action(app, TRUE, (const AstrognomeOptions *)&options);
        g_object_unref(app);

        return EXIT_SUCCESS;
    }

    status = g_application_run(G_APPLICATION(app), argc, argv);

    g_hash_table_destroy(xinclude_positions);
    g_object_unref(app);

    return status;
}
