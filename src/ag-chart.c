#include <errno.h>
#include <gio/gio.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/tree.h>
#include <libxml/xinclude.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <swe-glib.h>
#include <locale.h>
#include <math.h>
#include <string.h>

#include "ag-db.h"
#include "ag-chart.h"
#include "placidus.h"

typedef struct _AgChartPrivate {
    gchar *name;
    gchar *country;
    gchar *city;
    gchar *save_buffer;
    GList *planet_list;
    gchar *note;
} AgChartPrivate;

enum {
    PROP_0,
    PROP_NAME,
    PROP_COUNTRY,
    PROP_CITY
};

typedef enum {
    XML_CONVERT_STRING,
    XML_CONVERT_DOUBLE,
    XML_CONVERT_INT
} XmlConvertType;

G_DEFINE_QUARK(ag_chart_error_quark, ag_chart_error);

G_DEFINE_TYPE_WITH_PRIVATE(AgChart, ag_chart, GSWE_TYPE_MOMENT);

#define ag_g_variant_unref(v) \
    if ((v) != NULL) { \
        g_variant_unref((v)); \
    }

static void ag_chart_set_property(GObject      *gobject,
                                  guint        prop_id,
                                  const GValue *value,
                                  GParamSpec   *param_spec);
static void ag_chart_get_property(GObject    *gobject,
                                  guint      prop_id,
                                  GValue     *value,
                                  GParamSpec *param_spec);
static void ag_chart_finalize(GObject *gobject);

static void
ag_chart_class_init(AgChartClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = ag_chart_set_property;
    gobject_class->get_property = ag_chart_get_property;
    gobject_class->finalize     = ag_chart_finalize;

    g_object_class_install_property(
            gobject_class,
            PROP_NAME,
            g_param_spec_string(
                    "name",
                    "Chart name",
                    "Name of the person on this chart",
                    NULL,
                    G_PARAM_READWRITE
                )
        );
    g_object_class_install_property(
            gobject_class,
            PROP_COUNTRY,
            g_param_spec_string(
                    "country",
                    "Country name",
                    "Name of the country of birth",
                    NULL,
                    G_PARAM_READWRITE
                )
        );
    g_object_class_install_property(
            gobject_class,
            PROP_CITY,
            g_param_spec_string(
                    "city",
                    "City name",
                    "Name of the city of birth",
                    NULL,
                    G_PARAM_READWRITE
                )
        );
}

static void
ag_chart_init(AgChart *chart)
{
    AgChartPrivate *priv = ag_chart_get_instance_private(chart);

    priv->name        = NULL;
    priv->country     = NULL;
    priv->city        = NULL;
    priv->save_buffer = NULL;
    priv->planet_list = NULL;
}

static void
ag_chart_set_property(GObject      *gobject,
                      guint        prop_id,
                      const GValue *value,
                      GParamSpec   *param_spec)
{
    switch (prop_id) {
        case PROP_NAME:
            ag_chart_set_name(AG_CHART(gobject), g_value_get_string(value));

            break;

        case PROP_COUNTRY:
            ag_chart_set_country(AG_CHART(gobject), g_value_get_string(value));

            break;

        case PROP_CITY:
            ag_chart_set_city(AG_CHART(gobject), g_value_get_string(value));

            break;
    }
}

static void
ag_chart_get_property(GObject    *gobject,
                      guint      prop_id,
                      GValue     *value,
                      GParamSpec *param_spec)
{
    AgChartPrivate *priv = ag_chart_get_instance_private(AG_CHART(gobject));

    switch (prop_id) {
        case PROP_NAME:
            g_value_set_string(value, priv->name);

            break;

        case PROP_COUNTRY:
            g_value_set_string(value, priv->country);

            break;

        case PROP_CITY:
            g_value_set_string(value, priv->city);

            break;

    }
}

static void
ag_chart_finalize(GObject *gobject)
{
    AgChart        *chart = AG_CHART(gobject);
    AgChartPrivate *priv  = ag_chart_get_instance_private(chart);

    if (priv->name != NULL) {
        g_free(priv->name);
    }

    if (priv->country != NULL) {
        g_free(priv->country);
    }

    if (priv->city != NULL) {
        g_free(priv->city);
    }

    if (priv->save_buffer != NULL) {
        g_free(priv->save_buffer);
    }
}

void
ag_chart_add_planets(AgChart *chart)
{
    AgChartPrivate *priv = ag_chart_get_instance_private(chart);

    gswe_moment_add_planet(GSWE_MOMENT(chart), GSWE_PLANET_CHARIKLO, NULL);
    priv->planet_list = g_list_prepend(
            priv->planet_list,
            GINT_TO_POINTER(GSWE_PLANET_CHARIKLO)
        );

    gswe_moment_add_planet(GSWE_MOMENT(chart), GSWE_PLANET_VESTA, NULL);
    priv->planet_list = g_list_prepend(
            priv->planet_list,
            GINT_TO_POINTER(GSWE_PLANET_VESTA)
        );

    gswe_moment_add_planet(GSWE_MOMENT(chart), GSWE_PLANET_JUNO, NULL);
    priv->planet_list = g_list_prepend(
            priv->planet_list,
            GINT_TO_POINTER(GSWE_PLANET_JUNO)
        );

    gswe_moment_add_planet(GSWE_MOMENT(chart), GSWE_PLANET_PALLAS, NULL);
    priv->planet_list = g_list_prepend(
            priv->planet_list,
            GINT_TO_POINTER(GSWE_PLANET_PALLAS)
        );

    gswe_moment_add_planet(GSWE_MOMENT(chart), GSWE_PLANET_CERES, NULL);
    priv->planet_list = g_list_prepend(
            priv->planet_list,
            GINT_TO_POINTER(GSWE_PLANET_CERES)
        );

    gswe_moment_add_planet(GSWE_MOMENT(chart), GSWE_PLANET_NESSUS, NULL);
    priv->planet_list = g_list_prepend(
            priv->planet_list,
            GINT_TO_POINTER(GSWE_PLANET_NESSUS)
        );

    gswe_moment_add_planet(GSWE_MOMENT(chart), GSWE_PLANET_PHOLUS, NULL);
    priv->planet_list = g_list_prepend(
            priv->planet_list,
            GINT_TO_POINTER(GSWE_PLANET_PHOLUS)
        );

    gswe_moment_add_planet(GSWE_MOMENT(chart), GSWE_PLANET_CHIRON, NULL);
    priv->planet_list = g_list_prepend(
            priv->planet_list,
            GINT_TO_POINTER(GSWE_PLANET_CHIRON)
        );

    gswe_moment_add_planet(GSWE_MOMENT(chart), GSWE_PLANET_MOON_APOGEE, NULL);
    priv->planet_list = g_list_prepend(
            priv->planet_list,
            GINT_TO_POINTER(GSWE_PLANET_MOON_APOGEE)
        );

    gswe_moment_add_planet(GSWE_MOMENT(chart), GSWE_PLANET_MOON_NODE, NULL);
    priv->planet_list = g_list_prepend(
            priv->planet_list,
            GINT_TO_POINTER(GSWE_PLANET_MOON_NODE)
        );

    gswe_moment_add_planet(GSWE_MOMENT(chart), GSWE_PLANET_PLUTO, NULL);
    priv->planet_list = g_list_prepend(
            priv->planet_list,
            GINT_TO_POINTER(GSWE_PLANET_PLUTO)
        );

    gswe_moment_add_planet(GSWE_MOMENT(chart), GSWE_PLANET_NEPTUNE, NULL);
    priv->planet_list = g_list_prepend(
            priv->planet_list,
            GINT_TO_POINTER(GSWE_PLANET_NEPTUNE)
        );

    gswe_moment_add_planet(GSWE_MOMENT(chart), GSWE_PLANET_URANUS, NULL);
    priv->planet_list = g_list_prepend(
            priv->planet_list,
            GINT_TO_POINTER(GSWE_PLANET_URANUS)
        );

    gswe_moment_add_planet(GSWE_MOMENT(chart), GSWE_PLANET_SATURN, NULL);
    priv->planet_list = g_list_prepend(
            priv->planet_list,
            GINT_TO_POINTER(GSWE_PLANET_SATURN)
        );

    gswe_moment_add_planet(GSWE_MOMENT(chart), GSWE_PLANET_JUPITER, NULL);
    priv->planet_list = g_list_prepend(
            priv->planet_list,
            GINT_TO_POINTER(GSWE_PLANET_JUPITER)
        );

    gswe_moment_add_planet(GSWE_MOMENT(chart), GSWE_PLANET_MARS, NULL);
    priv->planet_list = g_list_prepend(
            priv->planet_list,
            GINT_TO_POINTER(GSWE_PLANET_MARS)
        );

    gswe_moment_add_planet(GSWE_MOMENT(chart), GSWE_PLANET_VENUS, NULL);
    priv->planet_list = g_list_prepend(
            priv->planet_list,
            GINT_TO_POINTER(GSWE_PLANET_VENUS)
        );

    gswe_moment_add_planet(GSWE_MOMENT(chart), GSWE_PLANET_MERCURY, NULL);
    priv->planet_list = g_list_prepend(
            priv->planet_list,
            GINT_TO_POINTER(GSWE_PLANET_MERCURY)
        );

    gswe_moment_add_planet(GSWE_MOMENT(chart), GSWE_PLANET_MOON, NULL);
    priv->planet_list = g_list_prepend(
            priv->planet_list,
            GINT_TO_POINTER(GSWE_PLANET_MOON)
        );

    gswe_moment_add_planet(GSWE_MOMENT(chart), GSWE_PLANET_SUN, NULL);
    priv->planet_list = g_list_prepend(
            priv->planet_list,
            GINT_TO_POINTER(GSWE_PLANET_SUN)
        );

    gswe_moment_add_planet(GSWE_MOMENT(chart), GSWE_PLANET_VERTEX, NULL);
    priv->planet_list = g_list_prepend(
            priv->planet_list,
            GINT_TO_POINTER(GSWE_PLANET_VERTEX)
        );

    gswe_moment_add_planet(GSWE_MOMENT(chart), GSWE_PLANET_ASCENDANT, NULL);
    priv->planet_list = g_list_prepend(
            priv->planet_list,
            GINT_TO_POINTER(GSWE_PLANET_ASCENDANT)
        );

    gswe_moment_add_planet(GSWE_MOMENT(chart), GSWE_PLANET_MC, NULL);
    priv->planet_list = g_list_prepend(
            priv->planet_list,
            GINT_TO_POINTER(GSWE_PLANET_MC)
        );
}

AgChart *
ag_chart_new_full(GsweTimestamp   *timestamp,
                  gdouble         longitude,
                  gdouble         latitude,
                  gdouble         altitude,
                  GsweHouseSystem house_system)
{
    AgChart         *chart;
    GsweCoordinates *coords = g_new0(GsweCoordinates, 1);

    coords->longitude = longitude;
    coords->latitude  = latitude;
    coords->altitude  = altitude;

    chart = AG_CHART(g_object_new(AG_TYPE_CHART,
                                  "timestamp",    timestamp,
                                  "coordinates",  coords,
                                  "house-system", house_system,
                                  NULL));

    g_free(coords);

    ag_chart_add_planets(chart);

    return chart;
}

void
ag_chart_set_name(AgChart *chart, const gchar *name)
{
    AgChartPrivate *priv = ag_chart_get_instance_private(chart);

    if (priv->name != NULL) {
        g_free(priv->name);
    }

    priv->name = g_strdup(name);
}

gchar *
ag_chart_get_name(AgChart *chart)
{
    AgChartPrivate *priv = ag_chart_get_instance_private(chart);

    return g_strdup(priv->name);
}

void
ag_chart_set_country(AgChart *chart, const gchar *country)
{
    AgChartPrivate *priv = ag_chart_get_instance_private(chart);

    if (priv->country != NULL) {
        g_free(priv->country);
    }

    priv->country = g_strdup(country);
}

gchar *
ag_chart_get_country(AgChart *chart)
{
    AgChartPrivate *priv = ag_chart_get_instance_private(chart);

    return g_strdup(priv->country);
}

void
ag_chart_set_city(AgChart *chart, const gchar *city)
{
    AgChartPrivate *priv = ag_chart_get_instance_private(chart);

    if (priv->city != NULL) {
        g_free(priv->city);
    }

    priv->city = g_strdup(city);
}

gchar *
ag_chart_get_city(AgChart *chart)
{
    AgChartPrivate *priv = ag_chart_get_instance_private(chart);

    return g_strdup(priv->city);
}

/**
 * get_by_xpath:
 * @xpath_context: an XPath context
 * @uri: the name of the file currently being processed. Used in error messages
 *       only
 * @xpath: an XPath expression
 * @value_required: marks the value as required. Although the XML tags must be
 *                  present, some values (like country or city name) may be
 *                  omitted
 * @type: the type of the variable to return
 * @err: a GError
 *
 * Get the value of an XML tag via XPath.
 *
 * Returns: (transfer container): a GVariant with the requested value
 */
static GVariant *
get_by_xpath(xmlXPathContextPtr xpath_context,
             const gchar        *uri,
             const gchar        *xpath,
             gboolean           value_required,
             XmlConvertType     type,
             GError             **err)
{
    xmlXPathObjectPtr xpathObj;
    const gchar       *text;
    char              *endptr;
    GVariant          *ret = NULL;
    gdouble           d;
    gint              i;

    if ((xpathObj = xmlXPathEvalExpression(
                 (const xmlChar *)xpath,
                 xpath_context
             )) == NULL) {
        g_set_error(
                err,
                AG_CHART_ERROR, AG_CHART_ERROR_LIBXML,
                "File '%s' could not be parsed due to internal XML error.",
                uri
            );

        return NULL;
    }

    if (xpathObj->nodesetval == NULL) {
        g_debug("No such node '%s'", xpath);
        g_set_error(
                err,
                AG_CHART_ERROR, AG_CHART_ERROR_CORRUPT_FILE,
                "File '%s' doesn't look like a valid saved chart: "
                    "missing node: %s",
                uri, xpath
            );
        xmlXPathFreeObject(xpathObj);

        return NULL;
    }

    if (xpathObj->nodesetval->nodeNr > 1) {
        g_debug("Too many '%s' nodes", xpath);
        g_set_error(
                err,
                AG_CHART_ERROR, AG_CHART_ERROR_CORRUPT_FILE,
                "File '%s' doesn't look like a valid saved chart: "
                    "too many node: %s",
                uri, xpath
            );
        xmlXPathFreeObject(xpathObj);

        return NULL;
    }

    if (xpathObj->nodesetval->nodeNr == 0) {
        if (value_required) {
            g_debug("No '%s' nodes", xpath);
            g_set_error(
                    err,
                    AG_CHART_ERROR, AG_CHART_ERROR_CORRUPT_FILE,
                    "File '%s' doesn't look like a valid saved chart: "
                        "missing node: %s",
                    uri, xpath
                );
            xmlXPathFreeObject(xpathObj);

            return NULL;
        } else {
            GVariant *ret = NULL;

            switch (type) {
                case XML_CONVERT_STRING:
                    ret = g_variant_new("ms", NULL);

                    break;

                case XML_CONVERT_DOUBLE:
                    ret = g_variant_new("md", FALSE, 0);

                    break;

                case XML_CONVERT_INT:
                    ret = g_variant_new("mi", FALSE, 0);

                    break;
            }

            return ret;
        }
    }

    text = (const gchar *)xpathObj->nodesetval->nodeTab[0]->content;

    switch (type) {
        case XML_CONVERT_STRING:
            ret = g_variant_new("ms", text);

            break;

        case XML_CONVERT_DOUBLE:
            d = g_ascii_strtod(text, &endptr);

            if ((*endptr != 0) || (errno != 0)) {
                g_set_error(
                        err,
                        AG_CHART_ERROR, AG_CHART_ERROR_CORRUPT_FILE,
                        "File '%s' doesn't look like a valid saved chart: "
                            "Invalid value in node: %s",
                        uri, xpath
                    );
                ret = NULL;
            } else {
                ret = g_variant_new_double(d);
            }

            break;

        case XML_CONVERT_INT:
            i = strtol(text, &endptr, 10);

            if ((*endptr != 0) || (errno != 0)) {
                g_set_error(
                        err,
                        AG_CHART_ERROR, AG_CHART_ERROR_CORRUPT_FILE,
                        "File '%s' doesn't look like a valid saved chart: "
                            "Invalid value in node: %s",
                        uri, xpath
                    );
                ret = NULL;
            } else {
                ret = g_variant_new_int32(i);
            }

            break;

    }

    xmlXPathFreeObject(xpathObj);

    return ret;
}

AgChart *
ag_chart_load_from_agc(GFile *file, GError **err)
{
    AgChart            *chart = NULL;
    gchar              *uri,
                       *xml = NULL,
                       *name,
                       *country_name,
                       *city_name,
                       *house_system_name,
                       *house_system_enum_name;
    gsize              length;
    xmlDocPtr          doc;
    xmlXPathContextPtr xpath_context;
    GVariant           *chart_name,
                       *country,
                       *city,
                       *longitude,
                       *latitude,
                       *altitude,
                       *year,
                       *month,
                       *day,
                       *hour,
                       *minute,
                       *second,
                       *timezone,
                       *note,
                       *house_system;
    GsweTimestamp      *timestamp;
    GEnumClass         *house_system_class;
    GEnumValue         *enum_value;
    gboolean           found_error = FALSE;

    uri = g_file_get_uri(file);

    if (!g_file_load_contents(file, NULL, &xml, &length, NULL, err)) {
        g_free(uri);

        return NULL;
    }

    if ((doc = xmlReadMemory(xml, length, "chart.xml", NULL, 0)) == NULL) {
        g_set_error(
                err,
                AG_CHART_ERROR, AG_CHART_ERROR_CORRUPT_FILE,
                "File '%s' can not be read. " \
                "Maybe it is corrupt, or not a save file at all",
                uri
            );
        g_free(xml);
        g_free(uri);

        return NULL;
    }

    if ((xpath_context = xmlXPathNewContext(doc)) == NULL) {
        g_set_error(
                err,
                AG_CHART_ERROR, AG_CHART_ERROR_LIBXML,
                "File '%s' could not be loaded due to internal LibXML error",
                uri
            );
        xmlFreeDoc(doc);
        g_free(xml);
        g_free(uri);

        return NULL;
    }

    if ((chart_name = get_by_xpath(
                 xpath_context,
                 uri,
                 "/chartinfo/data/name/text()",
                 TRUE,
                 XML_CONVERT_STRING,
                 err
             )) == NULL) {
        found_error = TRUE;
    }

    if ((country = get_by_xpath(
                 xpath_context,
                 uri,
                 "/chartinfo/data/place/country/text()",
                 FALSE,
                 XML_CONVERT_STRING,
                 err
             )) == NULL) {
        found_error = TRUE;
    }

    if ((city = get_by_xpath(
                 xpath_context,
                 uri,
                 "/chartinfo/data/place/city/text()",
                 FALSE,
                 XML_CONVERT_STRING,
                 err
             )) == NULL) {
        found_error = TRUE;
    }

    if ((longitude = get_by_xpath(
                 xpath_context,
                 uri,
                 "/chartinfo/data/place/longitude/text()",
                 TRUE,
                 XML_CONVERT_DOUBLE,
                 err
             )) == NULL) {
        found_error = TRUE;
    }

    if ((latitude = get_by_xpath(
                 xpath_context,
                 uri,
                 "/chartinfo/data/place/latitude/text()",
                 TRUE,
                 XML_CONVERT_DOUBLE,
                 err
             )) == NULL) {
        found_error = TRUE;
    }

    if ((altitude = get_by_xpath(
                 xpath_context,
                 uri,
                 "/chartinfo/data/place/altitude/text()",
                 TRUE,
                 XML_CONVERT_DOUBLE,
                 err
             )) == NULL) {
        found_error = TRUE;
    }

    if ((year = get_by_xpath(
                 xpath_context,
                 uri,
                 "/chartinfo/data/time/year/text()",
                 TRUE,
                 XML_CONVERT_INT,
                 err
             )) == NULL) {
        found_error = TRUE;
    }

    if ((month = get_by_xpath(
                 xpath_context,
                 uri,
                 "/chartinfo/data/time/month/text()",
                 TRUE,
                 XML_CONVERT_INT,
                 err
             )) == NULL) {
        found_error = TRUE;
    }

    if ((day = get_by_xpath(
                 xpath_context,
                 uri,
                 "/chartinfo/data/time/day/text()",
                 TRUE,
                 XML_CONVERT_INT,
                 err
             )) == NULL) {
        found_error = TRUE;
    }

    if ((hour = get_by_xpath(
                 xpath_context,
                 uri,
                 "/chartinfo/data/time/hour/text()",
                 TRUE,
                 XML_CONVERT_INT,
                 err
             )) == NULL) {
        found_error = TRUE;
    }

    if ((minute = get_by_xpath(
                 xpath_context,
                 uri,
                 "/chartinfo/data/time/minute/text()",
                 TRUE,
                 XML_CONVERT_INT,
                 err
             )) == NULL) {
        found_error = TRUE;
    }

    if ((second = get_by_xpath(
                 xpath_context,
                 uri,
                 "/chartinfo/data/time/second/text()",
                 TRUE,
                 XML_CONVERT_INT,
                 err
             )) == NULL) {
        found_error = TRUE;
    }

    if ((timezone = get_by_xpath(
                 xpath_context,
                 uri,
                 "/chartinfo/data/time/timezone/text()",
                 TRUE,
                 XML_CONVERT_DOUBLE,
                 err
             )) == NULL) {
        found_error = TRUE;
    }

    if ((house_system = get_by_xpath(
                 xpath_context,
                 uri,
                 "/chartinfo/data/housesystem/text()",
                 TRUE,
                 XML_CONVERT_STRING,
                 err
             )) == NULL) {
        found_error = TRUE;
    }

    note = get_by_xpath(
            xpath_context,
            uri,
            "/chartinfo/note/text()",
            FALSE,
            XML_CONVERT_STRING,
            NULL
        );

    if (found_error) {
        ag_g_variant_unref(house_system);
        ag_g_variant_unref(note);
        ag_g_variant_unref(chart_name);
        ag_g_variant_unref(country);
        ag_g_variant_unref(city);
        ag_g_variant_unref(longitude);
        ag_g_variant_unref(latitude);
        ag_g_variant_unref(altitude);
        ag_g_variant_unref(year);
        ag_g_variant_unref(month);
        ag_g_variant_unref(day);
        ag_g_variant_unref(hour);
        ag_g_variant_unref(minute);
        ag_g_variant_unref(second);
        ag_g_variant_unref(timezone);
        xmlFreeDoc(doc);
        g_free(xml);
        g_free(uri);

        return NULL;
    }

    timestamp = gswe_timestamp_new_from_gregorian_full(
            g_variant_get_int32(year),
            g_variant_get_int32(month),
            g_variant_get_int32(day),
            g_variant_get_int32(hour),
            g_variant_get_int32(minute),
            g_variant_get_int32(second),
            0,
            g_variant_get_double(timezone)
        );
    g_variant_unref(year);
    g_variant_unref(month);
    g_variant_unref(day);
    g_variant_unref(hour);
    g_variant_unref(minute);
    g_variant_unref(second);
    g_variant_unref(timezone);

    g_variant_get(house_system, "ms", &house_system_name);
    g_variant_unref(house_system);
    house_system_enum_name = g_utf8_strdown(house_system_name, -1);
    g_free(house_system_name);
    house_system_class = g_type_class_ref(GSWE_TYPE_HOUSE_SYSTEM);

    if ((enum_value = g_enum_get_value_by_nick(
                 G_ENUM_CLASS(house_system_class),
                 house_system_enum_name
             )) == NULL) {
        g_variant_unref(longitude);
        g_variant_unref(latitude);
        g_variant_unref(altitude);
        g_variant_unref(chart_name);
        g_variant_unref(country);
        g_variant_unref(city);
        ag_g_variant_unref(note);
        g_type_class_unref(house_system_class);
        g_free(house_system_enum_name);

        g_set_error(err,
                    AG_CHART_ERROR, AG_CHART_ERROR_CORRUPT_FILE,
                    "Unknown house system in save file"
               );

        return NULL;
    }

    chart = ag_chart_new_full(
            timestamp,
            g_variant_get_double(longitude),
            g_variant_get_double(latitude),
            g_variant_get_double(altitude),
            enum_value->value
        );
    g_type_class_unref(house_system_class);
    g_free(house_system_enum_name);
    g_variant_unref(longitude);
    g_variant_unref(latitude);
    g_variant_unref(altitude);

    g_variant_get(chart_name, "ms", &name);
    g_variant_unref(chart_name);
    ag_chart_set_name(chart, name);
    g_free(name);

    g_variant_get(country, "ms", &country_name);
    g_variant_unref(country);
    ag_chart_set_country(chart, country_name);
    g_free(country_name);

    g_variant_get(city, "ms", &city_name);
    g_variant_unref(city);
    ag_chart_set_city(chart, city_name);
    g_free(city_name);

    if (note) {
        gchar *note_text;

        g_variant_get(note, "ms", &note_text);
        g_variant_unref(note);
        ag_chart_set_note(chart, note_text);
    }

    g_free(xml);
    g_free(uri);
    xmlXPathFreeContext(xpath_context);
    xmlFreeDoc(doc);

    return chart;
}

AgChart *ag_chart_load_from_placidus_file(GFile  *file,
                                          GError **err)
{
    gchar *hor_contents,
          *header,
          *name_buf,
          *name,
          *type_buf,
          *type,
          *city_buf,
          *city,
          *notes_buf,
          *notes,
          *comma,
          *country;
    gsize hor_length,
          name_len,
          type_len,
          city_len,
          notes_len;
    guint8 calendar,
           month,
           day,
           hour,
           minute,
           long_deg,
           long_min,
           long_hemi,
           lat_deg,
           lat_min,
           lat_hemi,
           zone_type,
           zone_hour,
           zone_minute,
           zone_sign,
           gender;
    guint16 year;
    gdouble second,
           swe_tz,
           real_long,
           real_lat;
    GsweTimestamp *timestamp;
    AgChart *chart;
    GError *local_err = NULL;

    if (!g_file_load_contents(
                file,
                NULL,
                &hor_contents,
                &hor_length,
                NULL,
                err)) {
        return NULL;
    }

    // A Placidus save file is at least 2176 bytes (3926 in case of
    // a Nostradamus save)
    if (hor_length < 2176) {
        g_set_error(
                err,
                AG_CHART_ERROR, AG_CHART_ERROR_INVALID_PLAC_FILE,
                "Invalid Placidus file."
            );

        return NULL;
    }

    header = g_malloc0(PLAC_HEADER_LEN);
    memcpy(header, hor_contents + PLAC_HEADER_POS, PLAC_HEADER_LEN);
    if (strncmp(
                "PLACIDUS v4.0 Horoscope File\x0d\x0a", header,
                PLAC_HEADER_LEN)) {
        g_free(header);
        g_set_error(
                err,
                AG_CHART_ERROR, AG_CHART_ERROR_INVALID_PLAC_FILE,
                "Invalid Placidus file."
            );

        return NULL;
    }

    g_free(header);

    name_buf = g_malloc0(PLAC_NAME_LEN + 1);
    memcpy(name_buf, hor_contents + PLAC_NAME_POS, PLAC_NAME_LEN);
    name = g_convert(
            name_buf, -1,
            "UTF-8", "LATIN2",
            NULL,
            &name_len,
            &local_err
        );
    g_free(name_buf);

    type_buf = g_malloc0(PLAC_TYPE_LEN + 1);
    memcpy(type_buf, hor_contents + PLAC_TYPE_POS, PLAC_TYPE_LEN);
    type = g_convert(
            type_buf, -1,
            "UTF-8", "LATIN2",
            NULL,
            &type_len,
            &local_err
        );
    g_free(type_buf);

    notes_buf = g_malloc0(PLAC_NOTES_LEN + 1);
    memcpy(notes_buf, hor_contents + PLAC_NOTES_POS, PLAC_NOTES_LEN);
    notes = g_convert(
            notes_buf, -1,
            "UTF-8", "LATIN2",
            NULL,
            &notes_len,
            &local_err
        );
    g_free(notes_buf);

    city_buf = g_malloc0(PLAC_CITY_LEN + 1);
    memcpy(city_buf, hor_contents + PLAC_CITY_POS, PLAC_CITY_LEN);
    city = g_convert(
            city_buf, -1,
            "UTF-8", "LATIN2",
            NULL,
            &city_len,
            &local_err
        );
    g_free(city_buf);

    memcpy(&calendar, hor_contents + PLAC_CALENDAR_POS, sizeof(guint8));
    memcpy(&year, hor_contents + PLAC_YEAR_POS, sizeof(guint16));
    year = GUINT16_FROM_LE(year);

    memcpy(&month, hor_contents + PLAC_MONTH_POS, sizeof(guint8));
    memcpy(&day, hor_contents + PLAC_DAY_POS, sizeof(guint8));
    memcpy(&hour, hor_contents + PLAC_HOUR_POS, sizeof(guint8));
    memcpy(&minute, hor_contents + PLAC_MINUTE_POS, sizeof(guint8));
    memcpy(&second, hor_contents + PLAC_SECOND_POS, sizeof(gdouble));
    memcpy(&long_deg, hor_contents + PLAC_LONGDEG_POS, sizeof(guint8));
    memcpy(&long_min, hor_contents + PLAC_LONGMIN_POS, sizeof(guint8));
    memcpy(&long_hemi, hor_contents + PLAC_LONGSIGN_POS, sizeof(guint8));
    memcpy(&lat_deg, hor_contents + PLAC_LATDEG_POS, sizeof(guint8));
    memcpy(&lat_min, hor_contents + PLAC_LATMIN_POS, sizeof(guint8));
    memcpy(&lat_hemi, hor_contents + PLAC_LATSIGN_POS, sizeof(guint8));
    memcpy(&zone_type, hor_contents + PLAC_ZONETYPE_POS, sizeof(guint8));
    memcpy(&zone_hour, hor_contents + PLAC_ZONEHOUR_POS, sizeof(guint8));
    memcpy(&zone_minute, hor_contents + PLAC_ZONEMIN_POS, sizeof(guint8));
    memcpy(&zone_sign, hor_contents + PLAC_ZONESIGN_POS, sizeof(guint8));
    memcpy(&gender, hor_contents + PLAC_GENDER_POS, sizeof(guint8));

    g_free(hor_contents);

    switch (zone_type) {
        // UTC
        case 0:
            swe_tz = 0.0;

            break;

        // Local mean time. It is unclear what it exactly means for Placidus,
        // so we don’t support it yet.
        case 1:
            g_set_error(
                    err,
                    AG_CHART_ERROR, AG_CHART_ERROR_UNSUPPORTED_PLAC_FILE,
                    "Local mean time Placidus charts are not supported yet."
                );

            g_free(name);
            g_free(type);
            g_free(city);
            g_free(notes);

            return NULL;

        // Zone time
        case 2:
            {
                GDateTime *utc,
                          *final;
                GTimeZone *zone;
                gchar *zone_string;

                utc = g_date_time_new_utc(
                        year, month, day,
                        hour, minute, second
                    );
                zone_string = g_strdup_printf(
                        "%c%02d:%02d",
                        (zone_sign == 0) ? '+' : '-',
                        zone_hour,
                        zone_minute
                    );
                zone = g_time_zone_new(zone_string);
                final = g_date_time_to_timezone(utc, zone);
                g_date_time_unref(utc);
                year = g_date_time_get_year(final);
                month = g_date_time_get_month(final);
                day = g_date_time_get_day_of_month(final);
                hour = g_date_time_get_hour(final);
                minute = g_date_time_get_minute(final);
                second = g_date_time_get_second(final);
                swe_tz = (gdouble)zone_hour + (gdouble)zone_minute / 60.0;
            }

            break;

        default:
            g_set_error(
                    err,
                    AG_CHART_ERROR, AG_CHART_ERROR_UNSUPPORTED_PLAC_FILE,
                    "Unknown time zone type."
                );

            g_free(name);
            g_free(type);
            g_free(city);
            g_free(notes);

            return NULL;
    }

    switch (calendar) {
        // Julian calendar
        case 0:
            g_set_error(
                    err,
                    AG_CHART_ERROR, AG_CHART_ERROR_UNSUPPORTED_PLAC_FILE,
                    "Julian calendar is not supported by Astrognome yet."
                );

            g_free(name);
            g_free(type);
            g_free(city);
            g_free(notes);

            return NULL;

        // Gregorian calendar
        case 1:
            break;

        default:
            g_set_error(
                    err,
                    AG_CHART_ERROR, AG_CHART_ERROR_UNSUPPORTED_PLAC_FILE,
                    "Unknown calendar type in Placidus chart."
                );

            g_free(name);
            g_free(type);
            g_free(city);
            g_free(notes);

            return NULL;
    }

    if (strncmp("radix", type, 5) != 0) {
            g_set_error(
                    err,
                    AG_CHART_ERROR, AG_CHART_ERROR_UNSUPPORTED_PLAC_FILE,
                    "Only radix charts are supported by Astrognome yet."
                );

            g_free(name);
            g_free(type);
            g_free(city);
            g_free(notes);

            return NULL;
    }

    if ((comma = strchr(city, ',')) != NULL) {
        *comma = 0;
        country = comma + 2;
    } else {
        country = g_strdup("");
    }

    real_long = (gdouble)long_deg + (gdouble)long_min / 60.0;
    real_lat  = (gdouble)lat_deg + (gdouble)lat_deg / 60.0;

    if (long_hemi == 1) {
        real_long = - real_long;
    }

    if (lat_hemi == 1) {
        real_lat = - real_lat;
    }

    if (zone_sign == 1) {
        swe_tz = - swe_tz;
    }

    timestamp = gswe_timestamp_new_from_gregorian_full(
            year, month, day,
            hour, minute, second, 0,
            swe_tz
        );

    chart = ag_chart_new_full(
            timestamp,
            real_long, real_lat, 280.0,
            GSWE_HOUSE_SYSTEM_PLACIDUS
        );

    ag_chart_set_name(chart, name);
    ag_chart_set_country(chart, country);
    ag_chart_set_city(chart, city);
    // TODO: implement gender
    ag_chart_set_note(chart, notes);

    return chart;
}

AgChart *
ag_chart_new_from_db_save(AgDbSave *save_data, GError **err)
{
    GsweTimestamp   *timestamp;
    gchar           *house_system_enum_name;
    GTypeClass      *house_system_class;
    GEnumValue      *enum_value;
    GsweHouseSystem house_system;
    AgChart         *chart;

    if (save_data == NULL) {
        g_set_error(
                err,
                AG_CHART_ERROR, AG_CHART_ERROR_EMPTY_RECORD,
                "Invalid chart"
            );

        return NULL;
    }

    house_system_enum_name = g_utf8_strdown(save_data->house_system, -1);
    house_system_class = g_type_class_ref(GSWE_TYPE_HOUSE_SYSTEM);

    if ((enum_value = g_enum_get_value_by_nick(
                G_ENUM_CLASS(house_system_class),
                house_system_enum_name
            )) == NULL) {
        g_free(house_system_enum_name);
        g_set_error(
                err,
                AG_CHART_ERROR, AG_CHART_ERROR_INVALID_HOUSE_SYSTEM,
                "Invalid house system: '%s'",
                save_data->house_system
            );

        return NULL;
    }

    g_free(house_system_enum_name);

    house_system = enum_value->value;

    timestamp = gswe_timestamp_new_from_gregorian_full(
            save_data->year, save_data->month, save_data->day,
            save_data->hour, save_data->minute, save_data->second, 0,
            save_data->timezone
        );

    chart = ag_chart_new_full(
            timestamp,
            save_data->longitude,
            save_data->latitude,
            save_data->altitude,
            house_system
        );

    ag_chart_set_name(chart, save_data->name);
    ag_chart_set_country(chart, save_data->country);
    ag_chart_set_city(chart, save_data->city);
    ag_chart_set_note(chart, save_data->note);

    return chart;
}

static xmlDocPtr
create_save_doc(AgChart *chart)
{
    xmlDocPtr       doc        = NULL;
    xmlNodePtr      root_node  = NULL,
                    data_node  = NULL,
                    place_node = NULL,
                    time_node  = NULL;
    gchar           *value;
    GsweCoordinates *coordinates;
    GsweTimestamp   *timestamp;
    GEnumClass      *house_system_class;
    GEnumValue      *enum_value;

    doc       = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "chartinfo");
    xmlDocSetRootElement(doc, root_node);

    // Begin <data> node
    data_node = xmlNewChild(root_node, NULL, BAD_CAST "data", NULL);

    value = ag_chart_get_name(chart);
    xmlNewChild(data_node, NULL, BAD_CAST "name", BAD_CAST value);
    g_free(value);

    // Begin <place> node
    place_node = xmlNewChild(data_node, NULL, BAD_CAST "place", NULL);

    value = ag_chart_get_country(chart);
    xmlNewChild(place_node, NULL, BAD_CAST "country", BAD_CAST value);
    g_free(value);

    value = ag_chart_get_city(chart);
    xmlNewChild(place_node, NULL, BAD_CAST "city", BAD_CAST value);
    g_free(value);

    coordinates = gswe_moment_get_coordinates(GSWE_MOMENT(chart));

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
    time_node = xmlNewChild(data_node, NULL, BAD_CAST "time", NULL);

    timestamp = gswe_moment_get_timestamp(GSWE_MOMENT(chart));

    value = g_malloc0(10);
    g_ascii_dtostr(
            value, 10,
            gswe_timestamp_get_gregorian_year(timestamp, NULL)
        );
    xmlNewChild(time_node, NULL, BAD_CAST "year", BAD_CAST value);
    g_free(value);

    value = g_malloc0(3);
    g_ascii_dtostr(
            value, 3,
            gswe_timestamp_get_gregorian_month(timestamp, NULL)
        );
    xmlNewChild(time_node, NULL, BAD_CAST "month", BAD_CAST value);
    g_free(value);

    value = g_malloc0(3);
    g_ascii_dtostr(value, 3, gswe_timestamp_get_gregorian_day(timestamp, NULL));
    xmlNewChild(time_node, NULL, BAD_CAST "day", BAD_CAST value);
    g_free(value);

    value = g_malloc0(3);
    g_ascii_dtostr(
            value, 3,
            gswe_timestamp_get_gregorian_hour(timestamp, NULL)
        );
    xmlNewChild(time_node, NULL, BAD_CAST "hour", BAD_CAST value);
    g_free(value);

    value = g_malloc0(3);
    g_ascii_dtostr(
            value, 3,
            gswe_timestamp_get_gregorian_minute(timestamp, NULL)
        );
    xmlNewChild(time_node, NULL, BAD_CAST "minute", BAD_CAST value);
    g_free(value);

    value = g_malloc0(3);
    g_ascii_dtostr(
            value, 3,
            gswe_timestamp_get_gregorian_second(timestamp, NULL)
        );
    xmlNewChild(time_node, NULL, BAD_CAST "second", BAD_CAST value);
    g_free(value);

    value = g_malloc0(7);
    g_ascii_dtostr(value, 7, gswe_timestamp_get_gregorian_timezone(timestamp));
    xmlNewChild(time_node, NULL, BAD_CAST "timezone", BAD_CAST value);
    g_free(value);

    house_system_class = g_type_class_ref(GSWE_TYPE_HOUSE_SYSTEM);
    enum_value = g_enum_get_value(
            house_system_class,
            gswe_moment_get_house_system(GSWE_MOMENT(chart))
        );
    xmlNewChild(
            data_node,
            NULL,
            BAD_CAST "housesystem",
            BAD_CAST enum_value->value_nick
        );
    g_type_class_unref(house_system_class);

    if (ag_chart_get_note(chart)) {
        xmlNewChild(
                root_node,
                NULL,
                BAD_CAST "note",
                BAD_CAST ag_chart_get_note(chart)
            );
    }

    return doc;
}

void
ag_chart_save_to_file(AgChart *chart, GFile *file, GError **err)
{
    xmlChar   *content = NULL;
    int       length;
    xmlDocPtr save_doc = create_save_doc(chart);

    xmlDocDumpFormatMemoryEnc(save_doc, &content, &length, "UTF-8", 1);

    g_file_replace_contents(
            file,
            (const gchar *)content,
            length,
            NULL,
            FALSE,
            G_FILE_CREATE_NONE,
            NULL,
            NULL,
            err
        );

    xmlFreeDoc(save_doc);
}

gint
ag_chart_sort_planets_by_position(GswePlanetData *planet1,
                                  GswePlanetData *planet2)
{
    gdouble pos1,
            pos2;

    pos1 = gswe_planet_data_get_position(planet1);
    pos2 = gswe_planet_data_get_position(planet2);

    if (pos1 == pos2) {
        return 0;
    } else if (pos1 < pos2) {
        return -1;
    } else {
        return 1;
    }
}

gchar *
ag_chart_create_svg(AgChart *chart, gsize *length, GError **err)
{
    xmlDocPtr         doc = create_save_doc(chart),
                      xslt_doc,
                      svg_doc;
    xmlNodePtr        root_node     = NULL,
                      ascmcs_node   = NULL,
                      houses_node   = NULL,
                      bodies_node   = NULL,
                      aspects_node  = NULL,
                      antiscia_node = NULL,
                      node          = NULL;
    gchar             *value,
                      *save_content = NULL;
    const gchar       *xslt_content;
    GList             *houses,
                      *house,
                      *planet,
                      *aspect,
                      *antiscion,
                      *sorted_planets;
    GswePlanetData    *planet_data;
    GsweAspectData    *aspect_data;
    GEnumClass        *planets_class,
                      *aspects_class,
                      *antiscia_class;
    gint              save_length;
    xsltStylesheetPtr xslt_proc;
    locale_t          current_locale;
    GBytes            *xslt_data;
    gsize             xslt_length;
    gdouble           asc_position,
                      prev_position;
    gboolean          first;
    guint             dist;

    root_node = xmlDocGetRootElement(doc);

    // gswe_moment_get_house_cusps() also calculates ascmcs data, so call it
    // this early
    houses = gswe_moment_get_house_cusps(GSWE_MOMENT(chart), NULL);

    // Begin <ascmcs> node
    g_debug("Generating theoretical points table");
    ascmcs_node = xmlNewChild(root_node, NULL, BAD_CAST "ascmcs", NULL);

    node = xmlNewChild(ascmcs_node, NULL, BAD_CAST "ascendant", NULL);

    planet_data = gswe_moment_get_planet(
            GSWE_MOMENT(chart),
            GSWE_PLANET_ASCENDANT,
            NULL
        );
    asc_position = gswe_planet_data_get_position(planet_data);
    value        = g_malloc0(12);
    g_ascii_dtostr(value, 12, asc_position);
    xmlNewProp(node, BAD_CAST "degree_ut", BAD_CAST value);
    g_free(value);

    node = xmlNewChild(ascmcs_node, NULL, BAD_CAST "mc", NULL);

    planet_data = gswe_moment_get_planet(
            GSWE_MOMENT(chart),
            GSWE_PLANET_MC,
            NULL
        );
    value       = g_malloc0(12);
    g_ascii_dtostr(value, 12, gswe_planet_data_get_position(planet_data));
    xmlNewProp(node, BAD_CAST "degree_ut", BAD_CAST value);
    g_free(value);

    node = xmlNewChild(ascmcs_node, NULL, BAD_CAST "vertex", NULL);

    planet_data = gswe_moment_get_planet(
            GSWE_MOMENT(chart),
            GSWE_PLANET_VERTEX,
            NULL
        );
    value       = g_malloc0(12);
    g_ascii_dtostr(value, 12, gswe_planet_data_get_position(planet_data));
    xmlNewProp(node, BAD_CAST "degree_ut", BAD_CAST value);
    g_free(value);

    // Begin <houses> node
    g_debug("Generating houses table");
    houses_node = xmlNewChild(root_node, NULL, BAD_CAST "houses", NULL);

    for (house = houses; house; house = g_list_next(house)) {
        GsweHouseData *house_data = house->data;

        node = xmlNewChild(houses_node, NULL, BAD_CAST "house", NULL);

        value = g_malloc0(3);
        g_ascii_dtostr(value, 3, gswe_house_data_get_house(house_data));
        xmlNewProp(node, BAD_CAST "number", BAD_CAST value);
        g_free(value);

        value = g_malloc0(12);
        g_ascii_dtostr(
                value, 12,
                gswe_house_data_get_cusp_position(house_data)
            );
        xmlNewProp(node, BAD_CAST "degree", BAD_CAST value);
        g_free(value);
    }

    // Begin <bodies> node
    g_debug("Generating bodies table");
    bodies_node = xmlNewChild(root_node, NULL, BAD_CAST "bodies", NULL);

    planets_class = g_type_class_ref(GSWE_TYPE_PLANET);
    sorted_planets = g_list_sort(
            g_list_copy(gswe_moment_get_all_planets(GSWE_MOMENT(chart))),
            (GCompareFunc)ag_chart_sort_planets_by_position
        );

    for (
                planet = sorted_planets,
                     dist = 0,
                     prev_position = -360.0,
                     first = TRUE;
                planet;
                planet = g_list_next(planet)
            ) {
        planet_data = planet->data;
        GEnumValue *enum_value;
        gdouble    position;

        if (
            (gswe_planet_data_get_planet(planet_data) == GSWE_PLANET_ASCENDANT)
            || (gswe_planet_data_get_planet(planet_data) == GSWE_PLANET_MC)
            || (gswe_planet_data_get_planet(planet_data) == GSWE_PLANET_VERTEX)
           ) {
            continue;
        }

        position = gswe_planet_data_get_position(planet_data);

        if (first) {
            dist = 0;
            first = FALSE;
        } else if (fabs(prev_position - position) < 5.0) {
            dist++;
        } else {
            dist = 0;
        }

        prev_position = position;

        node = xmlNewChild(bodies_node, NULL, BAD_CAST "body", NULL);

        enum_value = g_enum_get_value(
                G_ENUM_CLASS(planets_class),
                gswe_planet_data_get_planet(planet_data)
            );
        xmlNewProp(node, BAD_CAST "name", BAD_CAST enum_value->value_name);

        value = g_malloc0(12);
        g_ascii_dtostr(value, 12, gswe_planet_data_get_position(planet_data));
        xmlNewProp(node, BAD_CAST "degree", BAD_CAST value);
        g_free(value);

        value = g_strdup_printf("%d", dist);
        xmlNewProp(node, BAD_CAST "dist", BAD_CAST value);
        g_free(value);
    }

    // Begin <aspects> node
    g_debug("Generating aspects table");
    aspects_node = xmlNewChild(root_node, NULL, BAD_CAST "aspects", NULL);

    aspects_class = g_type_class_ref(GSWE_TYPE_ASPECT);

    for (
                aspect = gswe_moment_get_all_aspects(GSWE_MOMENT(chart));
                aspect;
                aspect = g_list_next(aspect)
            ) {
        GswePlanetData *planet_data;
        GEnumValue     *enum_value;

        aspect_data = aspect->data;

        if (gswe_aspect_data_get_aspect(aspect_data) == GSWE_ASPECT_NONE) {
            continue;
        }

        node = xmlNewChild(aspects_node, NULL, BAD_CAST "aspect", NULL);

        planet_data = gswe_aspect_data_get_planet1(aspect_data);
        enum_value  = g_enum_get_value(
                G_ENUM_CLASS(planets_class),
                gswe_planet_data_get_planet(planet_data)
            );
        xmlNewProp(node, BAD_CAST "body1", BAD_CAST enum_value->value_name);

        planet_data = gswe_aspect_data_get_planet2(aspect_data);
        enum_value  = g_enum_get_value(
                G_ENUM_CLASS(planets_class),
                gswe_planet_data_get_planet(planet_data)
            );
        xmlNewProp(node, BAD_CAST "body2", BAD_CAST enum_value->value_name);

        enum_value = g_enum_get_value(
                G_ENUM_CLASS(aspects_class),
                gswe_aspect_data_get_aspect(aspect_data)
            );
        xmlNewProp(node, BAD_CAST "type", BAD_CAST enum_value->value_name);
    }

    g_type_class_unref(aspects_class);

    // Begin <antiscia> node
    g_debug("Generating antiscia table");
    antiscia_node  = xmlNewChild(root_node, NULL, BAD_CAST "antiscia", NULL);
    antiscia_class = g_type_class_ref(GSWE_TYPE_ANTISCION_AXIS);

    for (
                antiscion = gswe_moment_get_all_antiscia(GSWE_MOMENT(chart));
                antiscion;
                antiscion = g_list_next(antiscion)
            ) {
        GswePlanetData    *planet_data;
        GsweAntiscionData *antiscion_data = antiscion->data;
        GEnumValue        *enum_value;

        if (gswe_antiscion_data_get_axis(
                    antiscion_data) == GSWE_ANTISCION_AXIS_NONE
               ) {
            continue;
        }

        node = xmlNewChild(antiscia_node, NULL, BAD_CAST "antiscia", NULL);

        planet_data = gswe_antiscion_data_get_planet1(antiscion_data);
        enum_value  = g_enum_get_value(
                G_ENUM_CLASS(planets_class),
                gswe_planet_data_get_planet(planet_data)
            );
        xmlNewProp(node, BAD_CAST "body1", BAD_CAST enum_value->value_name);

        planet_data = gswe_antiscion_data_get_planet2(antiscion_data);
        enum_value  = g_enum_get_value(
                G_ENUM_CLASS(planets_class),
                gswe_planet_data_get_planet(planet_data)
            );
        xmlNewProp(node, BAD_CAST "body2", BAD_CAST enum_value->value_name);

        enum_value = g_enum_get_value(
                G_ENUM_CLASS(antiscia_class),
                gswe_antiscion_data_get_axis(antiscion_data)
            );
        xmlNewProp(node, BAD_CAST "axis", BAD_CAST enum_value->value_name);
    }

    g_type_class_unref(planets_class);

    // Now, doc contains the generated XML tree

    xslt_data = g_resources_lookup_data(
            "/eu/polonkai/gergely/Astrognome/ui/chart-default.xsl",
            G_RESOURCE_LOOKUP_FLAGS_NONE,
            NULL
        );
    xslt_content = g_bytes_get_data(xslt_data, &xslt_length);

    if ((xslt_doc = xmlReadMemory(
                xslt_content,
                xslt_length,
                "file://" PKGDATADIR "/astrognome",
                "UTF-8",
                0
            )) == NULL) {
        g_set_error(
                err,
                AG_CHART_ERROR, AG_CHART_ERROR_CORRUPT_FILE,
                "Built in style sheet can not be parsed as a stylesheet file."
            );
        xmlFreeDoc(doc);

        return NULL;
    }

#if LIBXML_VERSION >= 20603
    xmlXIncludeProcessFlags(xslt_doc, XSLT_PARSE_OPTIONS);
#else
    xmlXIncludeProcess(xslt_doc);
#endif

    if ((xslt_proc = xsltParseStylesheetDoc(xslt_doc)) == NULL) {
        g_set_error(
                err,
                AG_CHART_ERROR, AG_CHART_ERROR_CORRUPT_FILE,
                "Built in style sheet can not be parsed as a stylesheet file."
            );
        xmlFreeDoc(xslt_doc);
        xmlFreeDoc(doc);

        return NULL;
    }

    // libxml2 messes up the output, as it prints decimal floating point
    // numbers in a localized format. It is not good in locales that use a
    // character for decimal separator other than a dot. So let's just use the
    // C locale until the SVG is generated.
    current_locale = uselocale(newlocale(LC_ALL, "C", 0));
    svg_doc        = xsltApplyStylesheet(xslt_proc, doc, NULL);
    uselocale(current_locale);
    xsltFreeStylesheet(xslt_proc);
    xmlFreeDoc(doc);

    // Now, svg_doc contains the generated SVG file

    xmlDocDumpFormatMemoryEnc(
            svg_doc,
            (xmlChar **)&save_content,
            &save_length,
            "UTF-8",
            1
        );
    xmlFreeDoc(svg_doc);

    if (length != NULL) {
        *length = save_length;
    }

    return save_content;
}

GList *
ag_chart_get_planets(AgChart *chart)
{
    AgChartPrivate *priv = ag_chart_get_instance_private(chart);

    return priv->planet_list;
}

void
ag_chart_export_svg_to_file(AgChart *chart, GFile *file, GError **err)
{
    gchar *svg;
    gsize length;

    if ((svg = ag_chart_create_svg(chart, &length, err)) == NULL) {
        return;
    }

    g_file_replace_contents(
            file,
            (const gchar *)svg,
            length,
            NULL,
            FALSE,
            G_FILE_CREATE_NONE,
            NULL,
            NULL,
            err
        );
}

void
ag_chart_set_note(AgChart *chart, const gchar *note)
{
    AgChartPrivate *priv = ag_chart_get_instance_private(chart);

    priv->note = g_strdup(note);
}

const gchar *ag_chart_get_note(AgChart *chart)
{
    AgChartPrivate *priv = ag_chart_get_instance_private(chart);

    return priv->note;
}

AgDbSave *
ag_chart_get_db_save(AgChart *chart, gint db_id)
{
    GsweCoordinates *coords;
    AgChartPrivate  *priv      = ag_chart_get_instance_private(chart);
    AgDbSave        *save_data = g_new0(AgDbSave, 1);
    GsweTimestamp   *timestamp = gswe_moment_get_timestamp(GSWE_MOMENT(chart));
    GEnumClass      *house_system_class;
    GEnumValue      *house_system_enum;

    save_data->db_id = db_id;

    save_data->name         = g_strdup(priv->name);
    save_data->country      = g_strdup(priv->country);
    save_data->city         = g_strdup(priv->city);
    coords                  = gswe_moment_get_coordinates(GSWE_MOMENT(chart));
    save_data->longitude    = coords->longitude;
    save_data->latitude     = coords->latitude;
    save_data->altitude     = coords->altitude;
    g_free(coords);
    save_data->year         = gswe_timestamp_get_gregorian_year(
            timestamp,
            NULL
        );
    save_data->month        = gswe_timestamp_get_gregorian_month(
            timestamp,
            NULL
        );
    save_data->day          = gswe_timestamp_get_gregorian_day(timestamp, NULL);
    save_data->hour         = gswe_timestamp_get_gregorian_hour(
            timestamp,
            NULL
        );
    save_data->minute       = gswe_timestamp_get_gregorian_minute(
            timestamp,
            NULL
        );
    save_data->second       = gswe_timestamp_get_gregorian_second(
            timestamp,
            NULL
        );
    save_data->timezone     = gswe_timestamp_get_gregorian_timezone(timestamp);
    house_system_class      = g_type_class_ref(GSWE_TYPE_HOUSE_SYSTEM);
    house_system_enum       = g_enum_get_value(
            house_system_class,
            gswe_moment_get_house_system(GSWE_MOMENT(chart))
        );
    save_data->house_system = g_strdup(house_system_enum->value_nick);
    g_type_class_unref(house_system_class);
    save_data->note         = g_strdup(priv->note);

    return save_data;
}
