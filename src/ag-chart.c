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

#include "ag-chart.h"

struct _AgChartPrivate {
    gchar *name;
    gchar *country;
    gchar *city;
    gchar *save_buffer;
};

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

G_DEFINE_TYPE(AgChart, ag_chart, GSWE_TYPE_MOMENT);

#define GET_PRIVATE(instance) (G_TYPE_INSTANCE_GET_PRIVATE((instance), AG_TYPE_CHART, AgChartPrivate))
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

    g_type_class_add_private(klass, sizeof(AgChartPrivate));

    gobject_class->set_property = ag_chart_set_property;
    gobject_class->get_property = ag_chart_get_property;
    gobject_class->finalize     = ag_chart_finalize;

    g_object_class_install_property(gobject_class, PROP_NAME, g_param_spec_string("name", "Chart name", "Name of the person on this chart", NULL, G_PARAM_READWRITE));
    g_object_class_install_property(gobject_class, PROP_COUNTRY, g_param_spec_string("country", "Country name", "Name of the country of birth", NULL, G_PARAM_READWRITE));
    g_object_class_install_property(gobject_class, PROP_CITY, g_param_spec_string("city", "City name", "Name of the city of birth", NULL, G_PARAM_READWRITE));
}

static void
ag_chart_init(AgChart *chart)
{
    chart->priv              = GET_PRIVATE(chart);
    chart->priv->name        = NULL;
    chart->priv->country     = NULL;
    chart->priv->city        = NULL;
    chart->priv->save_buffer = NULL;
}

static void
ag_chart_set_property(GObject *gobject, guint prop_id, const GValue *value, GParamSpec *param_spec)
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
ag_chart_get_property(GObject *gobject, guint prop_id, GValue *value, GParamSpec *param_spec)
{
    switch (prop_id) {
        case PROP_NAME:
            g_value_set_string(value, AG_CHART(gobject)->priv->name);

            break;

        case PROP_COUNTRY:
            g_value_set_string(value, AG_CHART(gobject)->priv->country);

            break;

        case PROP_CITY:
            g_value_set_string(value, AG_CHART(gobject)->priv->city);

            break;

    }
}

static void
ag_chart_finalize(GObject *gobject)
{
    AgChart *chart = AG_CHART(gobject);

    if (chart->priv->name != NULL) {
        g_free(chart->priv->name);
    }

    if (chart->priv->country != NULL) {
        g_free(chart->priv->country);
    }

    if (chart->priv->city != NULL) {
        g_free(chart->priv->city);
    }

    if (chart->priv->save_buffer != NULL) {
        g_free(chart->priv->save_buffer);
    }
}

AgChart *
ag_chart_new_full(GsweTimestamp *timestamp, gdouble longitude, gdouble latitude, gdouble altitude, GsweHouseSystem house_system)
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

    gswe_moment_add_all_planets(GSWE_MOMENT(chart));

    return chart;
}

void
ag_chart_set_name(AgChart *chart, const gchar *name)
{
    if (chart->priv->name != NULL) {
        g_free(chart->priv->name);
    }

    chart->priv->name = g_strdup(name);
}

gchar *
ag_chart_get_name(AgChart *chart)
{
    return g_strdup(chart->priv->name);
}

void
ag_chart_set_country(AgChart *chart, const gchar *country)
{
    if (chart->priv->country != NULL) {
        g_free(chart->priv->country);
    }

    chart->priv->country = g_strdup(country);
}

gchar *
ag_chart_get_country(AgChart *chart)
{
    return g_strdup(chart->priv->country);
}

void
ag_chart_set_city(AgChart *chart, const gchar *city)
{
    if (chart->priv->city != NULL) {
        g_free(chart->priv->city);
    }

    chart->priv->city = g_strdup(city);
}

gchar *
ag_chart_get_city(AgChart *chart)
{
    return g_strdup(chart->priv->city);
}

/**
 * get_by_xpath:
 * @xpath_context: an XPath context
 * @uri: the name of the file currently being processed. Used in error messages only
 * @xpath: an XPath expression
 * @value_required: marks the value as required. Although the XML tags must be present, some values (like country or city name) may be omitted
 * @type: the type of the variable to return
 * @err: a GError
 *
 * Get the value of an XML tag via XPath.
 *
 * Returns: (transfer container): a GVariant with the requested value
 */
static GVariant *
get_by_xpath(xmlXPathContextPtr xpath_context, const gchar *uri, const gchar *xpath, gboolean value_required, XmlConvertType type, GError **err)
{
    xmlXPathObjectPtr xpathObj;
    const gchar       *text;
    char              *endptr;
    GVariant          *ret = NULL;
    gdouble           d;
    gint              i;

    if ((xpathObj = xmlXPathEvalExpression((const xmlChar *)xpath, xpath_context)) == NULL) {
        g_set_error(err, AG_CHART_ERROR, AG_CHART_ERROR_LIBXML, "File '%s' could not be parsed due to internal XML error.", uri);

        return NULL;
    }

    if (xpathObj->nodesetval == NULL) {
        g_debug("No such node '%s'", xpath);
        g_set_error(err, AG_CHART_ERROR, AG_CHART_ERROR_CORRUPT_FILE, "File '%s' doesn't look like a valid saved chart.", uri);
        xmlXPathFreeObject(xpathObj);

        return NULL;
    }

    if (xpathObj->nodesetval->nodeNr > 1) {
        g_debug("Too many '%s' nodes", xpath);
        g_set_error(err, AG_CHART_ERROR, AG_CHART_ERROR_CORRUPT_FILE, "File '%s' doesn't look like a valid saved chart.", uri);
        xmlXPathFreeObject(xpathObj);

        return NULL;
    }

    if (xpathObj->nodesetval->nodeNr == 0) {
        if (value_required) {
            g_debug("Too many '%s' nodes", xpath);
            g_set_error(err, AG_CHART_ERROR, AG_CHART_ERROR_CORRUPT_FILE, "File '%s' doesn't look like a valid saved chart.", uri);
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
                g_set_error(err, AG_CHART_ERROR, AG_CHART_ERROR_CORRUPT_FILE, "File '%s' doesn't look like a valid saved chart.", uri);
                ret = NULL;
            } else {
                ret = g_variant_new_double(d);
            }

            break;

        case XML_CONVERT_INT:
            i = strtol(text, &endptr, 10);

            if ((*endptr != 0) || (errno != 0)) {
                g_set_error(err, AG_CHART_ERROR, AG_CHART_ERROR_CORRUPT_FILE, "File '%s' doesn't look like a valid saved chart.", uri);
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
ag_chart_load_from_file(GFile *file, GError **err)
{
    AgChart            *chart = NULL;
    gchar              *uri;
    gchar              *xml = NULL;
    gchar              *name;
    gchar              *country_name;
    gchar              *city_name;
    gsize              length;
    xmlDocPtr          doc;
    xmlXPathContextPtr xpath_context;
    GVariant           *chart_name;
    GVariant           *country;
    GVariant           *city;
    GVariant           *longitude;
    GVariant           *latitude;
    GVariant           *altitude;
    GVariant           *year;
    GVariant           *month;
    GVariant           *day;
    GVariant           *hour;
    GVariant           *minute;
    GVariant           *second;
    GVariant           *timezone;
    GsweTimestamp      *timestamp;
    gboolean           found_error = FALSE;

    uri = g_file_get_uri(file);

    if (!g_file_load_contents(file, NULL, &xml, &length, NULL, err)) {
        g_free(uri);

        return NULL;
    }

    if ((doc = xmlReadMemory(xml, length, "chart.xml", NULL, 0)) == NULL) {
        g_set_error(err, AG_CHART_ERROR, AG_CHART_ERROR_CORRUPT_FILE, "File '%s' can not be read. Maybe it is corrupt, or not a save file at all", uri);
        g_free(xml);
        g_free(uri);

        return NULL;
    }

    if ((xpath_context = xmlXPathNewContext(doc)) == NULL) {
        g_set_error(err, AG_CHART_ERROR, AG_CHART_ERROR_LIBXML, "File '%s' could not be loaded due to internal LibXML error", uri);
        xmlFreeDoc(doc);
        g_free(xml);
        g_free(uri);

        return NULL;
    }

    if ((chart_name = get_by_xpath(xpath_context, uri, "/chartinfo/data/name/text()", TRUE, XML_CONVERT_STRING, err)) == NULL) {
        found_error = TRUE;
    }

    if ((country = get_by_xpath(xpath_context, uri, "/chartinfo/data/place/country/text()", FALSE, XML_CONVERT_STRING, err)) == NULL) {
        found_error = TRUE;
    }

    if ((city = get_by_xpath(xpath_context, uri, "/chartinfo/data/place/city/text()", FALSE, XML_CONVERT_STRING, err)) == NULL) {
        found_error = TRUE;
    }

    if ((longitude = get_by_xpath(xpath_context, uri, "/chartinfo/data/place/longitude/text()", TRUE, XML_CONVERT_DOUBLE, err)) == NULL) {
        found_error = TRUE;
    }

    if ((latitude = get_by_xpath(xpath_context, uri, "/chartinfo/data/place/latitude/text()", TRUE, XML_CONVERT_DOUBLE, err)) == NULL) {
        found_error = TRUE;
    }

    if ((altitude = get_by_xpath(xpath_context, uri, "/chartinfo/data/place/altitude/text()", TRUE, XML_CONVERT_DOUBLE, err)) == NULL) {
        found_error = TRUE;
    }

    if ((year = get_by_xpath(xpath_context, uri, "/chartinfo/data/time/year/text()", TRUE, XML_CONVERT_INT, err)) == NULL) {
        found_error = TRUE;
    }

    if ((month = get_by_xpath(xpath_context, uri, "/chartinfo/data/time/month/text()", TRUE, XML_CONVERT_INT, err)) == NULL) {
        found_error = TRUE;
    }

    if ((day = get_by_xpath(xpath_context, uri, "/chartinfo/data/time/day/text()", TRUE, XML_CONVERT_INT, err)) == NULL) {
        found_error = TRUE;
    }

    if ((hour = get_by_xpath(xpath_context, uri, "/chartinfo/data/time/hour/text()", TRUE, XML_CONVERT_INT, err)) == NULL) {
        found_error = TRUE;
    }

    if ((minute = get_by_xpath(xpath_context, uri, "/chartinfo/data/time/minute/text()", TRUE, XML_CONVERT_INT, err)) == NULL) {
        found_error = TRUE;
    }

    if ((second = get_by_xpath(xpath_context, uri, "/chartinfo/data/time/second/text()", TRUE, XML_CONVERT_INT, err)) == NULL) {
        found_error = TRUE;
    }

    if ((timezone = get_by_xpath(xpath_context, uri, "/chartinfo/data/time/timezone/text()", TRUE, XML_CONVERT_DOUBLE, err)) == NULL) {
        found_error = TRUE;
    }

    if (found_error) {
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

    // TODO: Make house system configurable (and saveable)
    chart = ag_chart_new_full(timestamp, g_variant_get_double(longitude), g_variant_get_double(latitude), g_variant_get_double(altitude), GSWE_HOUSE_SYSTEM_PLACIDUS);
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

    g_free(xml);
    g_free(uri);
    xmlXPathFreeContext(xpath_context);
    xmlFreeDoc(doc);

    return chart;
}

static xmlDocPtr
create_save_doc(AgChart *chart)
{
    xmlDocPtr  doc        = NULL;
    xmlNodePtr root_node  = NULL,
               data_node  = NULL,
               place_node = NULL,
               time_node  = NULL;
    gchar           *value;
    GsweCoordinates *coordinates;
    GsweTimestamp   *timestamp;

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
    g_ascii_dtostr(value, 10, gswe_timestamp_get_gregorian_year(timestamp, NULL));
    xmlNewChild(time_node, NULL, BAD_CAST "year", BAD_CAST value);
    g_free(value);

    value = g_malloc0(3);
    g_ascii_dtostr(value, 3, gswe_timestamp_get_gregorian_month(timestamp, NULL));
    xmlNewChild(time_node, NULL, BAD_CAST "month", BAD_CAST value);
    g_free(value);

    value = g_malloc0(3);
    g_ascii_dtostr(value, 3, gswe_timestamp_get_gregorian_day(timestamp, NULL));
    xmlNewChild(time_node, NULL, BAD_CAST "day", BAD_CAST value);
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
    xmlNewChild(time_node, NULL, BAD_CAST "second", BAD_CAST value);
    g_free(value);

    value = g_malloc0(7);
    g_ascii_dtostr(value, 7, gswe_timestamp_get_gregorian_timezone(timestamp));
    xmlNewChild(time_node, NULL, BAD_CAST "timezone", BAD_CAST value);
    g_free(value);

    return doc;
}

void
ag_chart_save_to_file(AgChart *chart, GFile *file, GError **err)
{
    xmlChar   *content = NULL;
    int       length;
    xmlDocPtr save_doc = create_save_doc(chart);

    xmlDocDumpFormatMemoryEnc(save_doc, &content, &length, "UTF-8", 1);

    g_file_replace_contents(file, (const gchar *)content, length, NULL, FALSE, G_FILE_CREATE_NONE, NULL, NULL, err);

    xmlFreeDoc(save_doc);
}

gchar *
ag_chart_create_svg(AgChart *chart, GError **err)
{
    xmlDocPtr         doc = create_save_doc(chart);
    xmlDocPtr         xslt_doc;
    xmlDocPtr         svg_doc;
    xmlNodePtr        root_node     = NULL;
    xmlNodePtr        ascmcs_node   = NULL;
    xmlNodePtr        houses_node   = NULL;
    xmlNodePtr        bodies_node   = NULL;
    xmlNodePtr        aspects_node  = NULL;
    xmlNodePtr        antiscia_node = NULL;
    xmlNodePtr        node          = NULL;
    gchar             *value;
    gchar             *stylesheet_path;
    gchar             *css_path;
    gchar             *save_content = NULL;
    gchar             *css_uri;
    gchar             *css_final_uri;
    gchar             **params;
    GList             *houses;
    GList             *house;
    GList             *planet;
    GList             *aspect;
    GList             *antiscion;
    GswePlanetData    *planet_data;
    GsweAspectData    *aspect_data;
    GEnumClass        *planets_class;
    GEnumClass        *aspects_class;
    GEnumClass        *antiscia_class;
    gint              save_length;
    GFile             *css_file;
    xsltStylesheetPtr xslt_proc;
    locale_t          current_locale;

    root_node = xmlDocGetRootElement(doc);

    // gswe_moment_get_house_cusps() also calculates ascmcs data, so call it this early
    houses = gswe_moment_get_house_cusps(GSWE_MOMENT(chart), NULL);

    // Begin <ascmcs> node
    g_debug("Generating theoretical points table");
    ascmcs_node = xmlNewChild(root_node, NULL, BAD_CAST "ascmcs", NULL);

    node = xmlNewChild(ascmcs_node, NULL, BAD_CAST "ascendant", NULL);

    planet_data = gswe_moment_get_planet(GSWE_MOMENT(chart), GSWE_PLANET_ASCENDANT, NULL);
    value       = g_malloc0(12);
    g_ascii_dtostr(value, 12, gswe_planet_data_get_position(planet_data));
    xmlNewProp(node, BAD_CAST "degree_ut", BAD_CAST value);
    g_free(value);

    node = xmlNewChild(ascmcs_node, NULL, BAD_CAST "mc", NULL);

    planet_data = gswe_moment_get_planet(GSWE_MOMENT(chart), GSWE_PLANET_MC, NULL);
    value       = g_malloc0(12);
    g_ascii_dtostr(value, 12, gswe_planet_data_get_position(planet_data));
    xmlNewProp(node, BAD_CAST "degree_ut", BAD_CAST value);
    g_free(value);

    node = xmlNewChild(ascmcs_node, NULL, BAD_CAST "vertex", NULL);

    planet_data = gswe_moment_get_planet(GSWE_MOMENT(chart), GSWE_PLANET_VERTEX, NULL);
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
        g_ascii_dtostr(value, 12, gswe_house_data_get_cusp_position(house_data));
        xmlNewProp(node, BAD_CAST "degree", BAD_CAST value);
        g_free(value);
    }

    // Begin <bodies> node
    g_debug("Generating bodies table");
    bodies_node = xmlNewChild(root_node, NULL, BAD_CAST "bodies", NULL);

    planets_class = g_type_class_ref(GSWE_TYPE_PLANET);

    for (planet = gswe_moment_get_all_planets(GSWE_MOMENT(chart)); planet; planet = g_list_next(planet)) {
        planet_data = planet->data;
        GEnumValue *enum_value;

        if (
            (gswe_planet_data_get_planet(planet_data) == GSWE_PLANET_ASCENDANT) ||
            (gswe_planet_data_get_planet(planet_data) == GSWE_PLANET_MC) ||
            (gswe_planet_data_get_planet(planet_data) == GSWE_PLANET_VERTEX)
           ) {
            continue;
        }

        node = xmlNewChild(bodies_node, NULL, BAD_CAST "body", NULL);

        enum_value = g_enum_get_value(G_ENUM_CLASS(planets_class), gswe_planet_data_get_planet(planet_data));
        xmlNewProp(node, BAD_CAST "name", BAD_CAST enum_value->value_name);

        value = g_malloc0(12);
        g_ascii_dtostr(value, 12, gswe_planet_data_get_position(planet_data));
        xmlNewProp(node, BAD_CAST "degree", BAD_CAST value);
        g_free(value);
    }

    // Begin <aspects> node
    g_debug("Generating aspects table");
    aspects_node = xmlNewChild(root_node, NULL, BAD_CAST "aspects", NULL);

    aspects_class = g_type_class_ref(GSWE_TYPE_ASPECT);

    for (aspect = gswe_moment_get_all_aspects(GSWE_MOMENT(chart)); aspect; aspect = g_list_next(aspect)) {
        GswePlanetData *planet_data;
        GEnumValue *enum_value;
        aspect_data = aspect->data;

        if (gswe_aspect_data_get_aspect(aspect_data) == GSWE_ASPECT_NONE) {
            continue;
        }

        node = xmlNewChild(aspects_node, NULL, BAD_CAST "aspect", NULL);

        planet_data = gswe_aspect_data_get_planet1(aspect_data);
        enum_value = g_enum_get_value(G_ENUM_CLASS(planets_class), gswe_planet_data_get_planet(planet_data));
        xmlNewProp(node, BAD_CAST "body1", BAD_CAST enum_value->value_name);
        gswe_planet_data_unref(planet_data);

        planet_data = gswe_aspect_data_get_planet2(aspect_data);
        enum_value = g_enum_get_value(G_ENUM_CLASS(planets_class), gswe_planet_data_get_planet(planet_data));
        xmlNewProp(node, BAD_CAST "body2", BAD_CAST enum_value->value_name);
        gswe_planet_data_unref(planet_data);

        enum_value = g_enum_get_value(G_ENUM_CLASS(aspects_class), gswe_aspect_data_get_aspect(aspect_data));
        xmlNewProp(node, BAD_CAST "type", BAD_CAST enum_value->value_name);
    }

    g_type_class_unref(aspects_class);

    // Begin <antiscia> node
    g_debug("Generating antiscia table");
    antiscia_node  = xmlNewChild(root_node, NULL, BAD_CAST "antiscia", NULL);
    antiscia_class = g_type_class_ref(GSWE_TYPE_ANTISCION_AXIS);

    for (antiscion = gswe_moment_get_all_antiscia(GSWE_MOMENT(chart)); antiscion; antiscion = g_list_next(antiscion)) {
        GswePlanetData *planet_data;
        GsweAntiscionData *antiscion_data = antiscion->data;
        GEnumValue        *enum_value;

        if (gswe_antiscion_data_get_axis(antiscion_data) == GSWE_ANTISCION_AXIS_NONE) {
            continue;
        }

        node = xmlNewChild(antiscia_node, NULL, BAD_CAST "antiscia", NULL);

        planet_data = gswe_antiscion_data_get_planet1(antiscion_data);
        enum_value = g_enum_get_value(G_ENUM_CLASS(planets_class), gswe_planet_data_get_planet(planet_data));
        xmlNewProp(node, BAD_CAST "body1", BAD_CAST enum_value->value_name);
        gswe_planet_data_unref(planet_data);

        planet_data = gswe_antiscion_data_get_planet2(antiscion_data);
        enum_value = g_enum_get_value(G_ENUM_CLASS(planets_class), gswe_planet_data_get_planet(planet_data));
        xmlNewProp(node, BAD_CAST "body2", BAD_CAST enum_value->value_name);
        gswe_planet_data_unref(planet_data);

        enum_value = g_enum_get_value(G_ENUM_CLASS(antiscia_class), gswe_antiscion_data_get_axis(antiscion_data));
        xmlNewProp(node, BAD_CAST "axis", BAD_CAST enum_value->value_name);
    }

    g_type_class_unref(planets_class);

    // Now, doc contains the generated XML tree

    css_path = g_strdup_printf("%s/%s", PKGDATADIR, "chart.css");
    g_debug("Using %s as a CSS stylesheet", css_path);
    css_file = g_file_new_for_path(css_path);
    css_uri  = g_file_get_uri(css_file);

    stylesheet_path = g_strdup_printf("%s/%s", PKGDATADIR, "chart.xsl");
    g_debug("Opening %s as a stylesheet", stylesheet_path);
    if ((xslt_doc = xmlReadFile(stylesheet_path, "UTF-8", 0)) == NULL) {
        g_set_error(err, AG_CHART_ERROR, AG_CHART_ERROR_CORRUPT_FILE, "File '%s' can not be parsed as a stylesheet file.", stylesheet_path);
        g_free(stylesheet_path);
        g_free(css_path);
        g_free(css_uri);
        g_object_unref(css_file);
        xmlFreeDoc(doc);

        return NULL;
    }

#if LIBXML_VERSION >= 20603
    xmlXIncludeProcessFlags(xslt_doc, XSLT_PARSE_OPTIONS);
#else
    xmlXIncludeProcess(xslt_doc);
#endif

    if ((xslt_proc = xsltParseStylesheetDoc(xslt_doc)) == NULL) {
        g_set_error(err, AG_CHART_ERROR, AG_CHART_ERROR_CORRUPT_FILE, "File '%s' can not be parsed as a stylesheet file.", stylesheet_path);
        g_free(stylesheet_path);
        g_free(css_path);
        g_free(css_uri);
        g_object_unref(css_file);
        xmlFreeDoc(xslt_doc);
        xmlFreeDoc(doc);

        return NULL;
    }

    css_final_uri = g_strdup_printf("'%s'", css_uri);
    g_free(css_uri);
    params    = g_new0(gchar *, 3);
    params[0] = "css_file";
    params[1] = css_final_uri;
    // libxml2 messes up the output, as it prints decimal floating point
    // numbers in a localized format. It is not good in locales that use a
    // character for decimal separator other than a dot. So let's just use the
    // C locale until the SVG is generated.
    current_locale = uselocale(newlocale(LC_ALL, "C", 0));
    svg_doc        = xsltApplyStylesheet(xslt_proc, doc, (const char **)params);
    uselocale(current_locale);
    g_free(stylesheet_path);
    g_free(css_path);
    g_object_unref(css_file);
    g_free(params);
    xsltFreeStylesheet(xslt_proc);
    xmlFreeDoc(doc);

    // Now, svg_doc contains the generated SVG file

    xmlDocDumpFormatMemoryEnc(svg_doc, (xmlChar **)&save_content, &save_length, "UTF-8", 1);
    xmlFreeDoc(svg_doc);

    return save_content;
}

