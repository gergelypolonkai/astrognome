#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <libxml/xmlversion.h>
#include <libxml/parser.h>
#include <libxslt/xslt.h>
#include <libxslt/transform.h>
#include <libexslt/exslt.h>
#include <libxml/xmlreader.h>

#include <swe-glib.h>

#include "config.h"

#include "ag-app.h"
#include "ag-window.h"

#ifndef LIBXML_READER_ENABLED
#error "You need to have libxml2 with XmlReader enabled"
#endif

GtkBuilder    *builder;
GtkFileFilter *filter_all   = NULL;
GtkFileFilter *filter_chart = NULL;
GtkFileFilter *filter_hor   = NULL;
GtkFileFilter *filter_svg   = NULL;
GtkFileFilter *filter_jpg   = NULL;
GtkTreeModel  *country_list = NULL;
GtkTreeModel  *city_list    = NULL;
GHashTable    *xinclude_positions;
gsize         used_planets_count;

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

const GswePlanet used_planets[] = {
    GSWE_PLANET_MC,
    GSWE_PLANET_ASCENDANT,
    GSWE_PLANET_VERTEX,
    GSWE_PLANET_SUN,
    GSWE_PLANET_MOON,
    GSWE_PLANET_MERCURY,
    GSWE_PLANET_VENUS,
    GSWE_PLANET_MARS,
    GSWE_PLANET_JUPITER,
    GSWE_PLANET_SATURN,
    GSWE_PLANET_URANUS,
    GSWE_PLANET_NEPTUNE,
    GSWE_PLANET_PLUTO,
    GSWE_PLANET_MOON_NODE,
    GSWE_PLANET_MOON_APOGEE,
    GSWE_PLANET_CHIRON,
    GSWE_PLANET_PHOLUS,
    GSWE_PLANET_NESSUS,
    GSWE_PLANET_CERES,
    GSWE_PLANET_PALLAS,
    GSWE_PLANET_JUNO,
    GSWE_PLANET_VESTA,
    GSWE_PLANET_CHARIKLO
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

    filter_hor = gtk_file_filter_new();
    gtk_file_filter_set_name(filter_hor, _("Placidus charts"));
    gtk_file_filter_add_pattern(filter_hor, "*.hor");
    g_object_ref_sink(filter_hor);

    filter_svg = gtk_file_filter_new();
    gtk_file_filter_set_name(filter_svg, _("SVG image"));
    gtk_file_filter_add_pattern(filter_svg, "*.svg");
    g_object_ref_sink(filter_svg);

    filter_jpg = gtk_file_filter_new();
    gtk_file_filter_set_name(filter_jpg, _("JPEG image"));
    gtk_file_filter_add_pattern(filter_jpg, "*.jpg");
    gtk_file_filter_add_pattern(filter_jpg, "*.jpe");
    gtk_file_filter_add_pattern(filter_jpg, "*.jpeg");
    g_object_ref_sink(filter_jpg);
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

const gchar *
ag_house_system_id_to_nick(GsweHouseSystem house_system)
{
    GEnumClass *house_system_class;
    GEnumValue *enum_value;

    house_system_class = g_type_class_ref(GSWE_TYPE_HOUSE_SYSTEM);
    enum_value = g_enum_get_value(house_system_class, house_system);

    if (enum_value) {
        return enum_value->value_nick;
    }

    return NULL;
}

GsweHouseSystem
ag_house_system_nick_to_id(const gchar *nick)
{
    GEnumClass *house_system_class;
    GEnumValue *enum_value;

    house_system_class = g_type_class_ref(GSWE_TYPE_HOUSE_SYSTEM);
    enum_value = g_enum_get_value_by_nick(house_system_class, nick);

    if (enum_value) {
        return enum_value->value;
    }

    return GSWE_HOUSE_SYSTEM_NONE;
}

const gchar *
ag_planet_id_to_nick(GswePlanet planet)
{
    GEnumClass *planet_class;
    GEnumValue *enum_value;

    planet_class = g_type_class_ref(GSWE_TYPE_PLANET);
    enum_value = g_enum_get_value(planet_class, planet);

    if (enum_value) {
        return enum_value->value_nick;
    }

    return NULL;
}

GswePlanet
ag_planet_nick_to_id(const gchar *nick)
{
    GEnumClass *planet_class;
    GEnumValue *enum_value;

    planet_class = g_type_class_ref(GSWE_TYPE_PLANET);
    enum_value = g_enum_get_value_by_nick(planet_class, nick);

    if (enum_value) {
        return enum_value->value;
    }

    return GSWE_PLANET_NONE;
}

const gchar *
ag_aspect_id_to_nick(GsweAspect aspect)
{
    GEnumClass *aspect_class;
    GEnumValue *enum_value;

    aspect_class = g_type_class_ref(GSWE_TYPE_ASPECT);
    enum_value = g_enum_get_value(aspect_class, aspect);

    if (enum_value) {
        return enum_value->value_nick;
    }

    return NULL;
}

GsweAspect
ag_aspect_nick_to_id(const gchar *nick)
{
    GEnumClass *aspect_class;
    GEnumValue *enum_value;

    aspect_class = g_type_class_ref(GSWE_TYPE_ASPECT);
    enum_value = g_enum_get_value_by_nick(aspect_class, nick);

    if (enum_value) {
        return enum_value->value;
    }

    return GSWE_ASPECT_NONE;
}

const gchar *
ag_antiscion_axis_id_to_nick(GsweAntiscionAxis antiscion_axis)
{
    GEnumClass *antiscion_axis_class;
    GEnumValue *enum_value;

    antiscion_axis_class = g_type_class_ref(GSWE_TYPE_ANTISCION_AXIS);
    enum_value = g_enum_get_value(antiscion_axis_class, antiscion_axis);

    if (enum_value) {
        return enum_value->value_nick;
    }

    return NULL;
}

GsweAntiscionAxis
ag_antiscion_axis_nick_to_id(const gchar *nick)
{
    GEnumClass *antiscion_axis_class;
    GEnumValue *enum_value;

    antiscion_axis_class = g_type_class_ref(GSWE_TYPE_ANTISCION_AXIS);
    enum_value = g_enum_get_value_by_nick(antiscion_axis_class, nick);

    if (enum_value) {
        return enum_value->value;
    }

    return GSWE_ANTISCION_AXIS_NONE;
}

/**
 * ag_get_user_data_dir:
 *
 * Creates the astrognome data directory (~/.local/share/astrognome on XDG
 * compatible systems) if necessary, and returns a GFile handle to it.
 *
 * Returns: (transfer full): a #GFile handle to the application data directory
 *          that must be freed with g_object_unref().
 */
GFile *
ag_get_user_data_dir(void)
{
    GFile *user_data_dir = g_file_new_for_path(g_get_user_data_dir()),
          *ag_data_dir   = g_file_get_child(user_data_dir, "astrognome");

    g_clear_object(&user_data_dir);
    g_assert(ag_data_dir);

    if (!g_file_query_exists(ag_data_dir, NULL)) {
        gchar *path = g_file_get_path(ag_data_dir);

        if (g_mkdir_with_parents(path, 0700) != 0) {
            g_error(
                    "Data directory ‘%s’ does not exist and cannot be created.",
                    path
                );
        }

        g_free(path);
    }

    return ag_data_dir;
}

int
main(int argc, char *argv[])
{
    gint              status;
    AgApp             *app;
    xmlTextReaderPtr  reader;
    AstrognomeOptions options;
    GError            *err             = NULL;
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

    used_planets_count = sizeof(used_planets) / sizeof(GswePlanet);
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

    country_list = GTK_TREE_MODEL(gtk_list_store_new(
            AG_COUNTRY_COLCOUNT,
            G_TYPE_STRING,
            G_TYPE_STRING
        ));

    city_list = GTK_TREE_MODEL(gtk_list_store_new(
            AG_CITY_COLCOUNT,
            G_TYPE_STRING,
            G_TYPE_STRING,
            G_TYPE_DOUBLE,
            G_TYPE_DOUBLE,
            G_TYPE_DOUBLE,
            G_TYPE_DOUBLE,
            G_TYPE_DOUBLE
        ));

    reader = xmlReaderForFile(PKGDATADIR "/geodata.xml", NULL, 0);

    if (reader != NULL) {
        int ret;

        while ((ret = xmlTextReaderRead(reader)) == 1) {
            gchar        *name;
            GtkTreeIter  iter;

            name = (gchar *)xmlTextReaderConstName(reader);

            if (strcmp(name, "p") == 0) {
                gchar   *aname,
                        *acode,
                        *alat,
                        *alon,
                        *aalt,
                        *atzo,
                        *atzd,
                        *endptr;
                gdouble lat,
                        lon,
                        alt,
                        tzo,
                        tzd;

                aname = (gchar *)xmlTextReaderGetAttribute(
                        reader,
                        BAD_CAST "n"
                    );
                acode = (gchar *)xmlTextReaderGetAttribute(
                        reader,
                        BAD_CAST "c"
                    );
                alat = (gchar *)xmlTextReaderGetAttribute(
                        reader,
                        BAD_CAST "lat"
                    );
                alon = (gchar *)xmlTextReaderGetAttribute(
                        reader,
                        BAD_CAST "lon"
                    );
                aalt = (gchar *)xmlTextReaderGetAttribute(
                        reader,
                        BAD_CAST "alt"
                    );
                atzo = (gchar *)xmlTextReaderGetAttribute(
                        reader,
                        BAD_CAST "tzo"
                    );
                atzd = (gchar *)xmlTextReaderGetAttribute(
                        reader,
                        BAD_CAST "tzd"
                    );

                lat = g_ascii_strtod(alat, &endptr);
                lon = g_ascii_strtod(alon, &endptr);

                if (*aalt == '\0') {
                    alt = DEFAULT_ALTITUDE;
                } else {
                    alt = g_ascii_strtod(aalt, &endptr);
                }

                tzo = g_ascii_strtod(atzo, &endptr);
                tzd = g_ascii_strtod(atzd, &endptr);

                gtk_list_store_append(GTK_LIST_STORE(city_list), &iter);
                gtk_list_store_set(
                        GTK_LIST_STORE(city_list), &iter,
                        AG_CITY_COUNTRY, acode,
                        AG_CITY_NAME,    aname,
                        AG_CITY_LAT,     lat,
                        AG_CITY_LONG,    lon,
                        AG_CITY_ALT,     alt,
                        AG_CITY_TZO,     tzo,
                        AG_CITY_TZD,     tzd,
                        -1
                    );

                g_free(aname);
                g_free(acode);
                g_free(alat);
                g_free(alon);
                g_free(aalt);
                g_free(atzo);
                g_free(atzd);
            } else if (strcmp(name, "c") == 0) {
                gchar *aname,
                      *acode;

                aname = (gchar *)xmlTextReaderGetAttribute(
                        reader,
                        BAD_CAST "n"
                    );
                acode = (gchar *)xmlTextReaderGetAttribute(
                        reader,
                        BAD_CAST "c"
                    );

                gtk_list_store_append(GTK_LIST_STORE(country_list), &iter);
                gtk_list_store_set(
                        GTK_LIST_STORE(country_list), &iter,
                        AG_COUNTRY_CODE, acode,
                        AG_COUNTRY_NAME, aname,
                        -1
                    );

                g_free(aname);
                g_free(acode);
            }
        }

        xmlFreeTextReader(reader);

        if (ret != 0) {
            g_error("Parse error in geodata.xml!");
        }
    } else {
        g_error("Unable to open geodata.xml!");
    }

    status = g_application_run(G_APPLICATION(app), argc, argv);

    g_hash_table_destroy(xinclude_positions);
    g_object_unref(app);

    return status;
}
