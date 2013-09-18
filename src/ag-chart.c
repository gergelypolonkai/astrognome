#include <errno.h>
#include <gio/gio.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <swe-glib.h>

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

G_DEFINE_QUARK(ag-chart-error-quark, ag_chart_error);

G_DEFINE_TYPE(AgChart, ag_chart, GSWE_TYPE_MOMENT);

#define GET_PRIVATE(instance) (G_TYPE_INSTANCE_GET_PRIVATE((instance), AG_TYPE_CHART, AgChartPrivate))

static void ag_chart_set_property(GObject *gobject, guint prop_id, const GValue *value, GParamSpec *param_spec);
static void ag_chart_get_property(GObject *gobject, guint prop_id, GValue *value, GParamSpec *param_spec);
static void ag_chart_finalize(GObject *gobject);

static void
ag_chart_class_init(AgChartClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    g_type_class_add_private(klass, sizeof(AgChartPrivate));

    gobject_class->set_property = ag_chart_set_property;
    gobject_class->get_property = ag_chart_get_property;
    gobject_class->finalize = ag_chart_finalize;

    g_object_class_install_property(gobject_class, PROP_NAME, g_param_spec_string("name", "Chart name", "Name of the person on this chart", NULL, G_PARAM_READWRITE));
    g_object_class_install_property(gobject_class, PROP_COUNTRY, g_param_spec_string("country", "Country name", "Name of the country of birth", NULL, G_PARAM_READWRITE));
    g_object_class_install_property(gobject_class, PROP_CITY, g_param_spec_string("city", "City name", "Name of the city of birth", NULL, G_PARAM_READWRITE));
}

static void
ag_chart_init(AgChart *chart)
{
    chart->priv = GET_PRIVATE(chart);
    chart->priv->name = NULL;
    chart->priv->country = NULL;
    chart->priv->city = NULL;
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
    AgChart *chart;
    GsweCoordinates *coords = g_new0(GsweCoordinates, 1);

    coords->longitude = longitude;
    coords->latitude = latitude;
    coords->altitude = altitude;

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

static GVariant *
get_by_xpath(xmlXPathContextPtr xpath_context, const gchar *uri, const gchar *xpath, XmlConvertType type, GError **err)
{
    xmlXPathObjectPtr xpathObj;
    const gchar *text;
    char *endptr;
    GVariant *ret = NULL;
    gdouble d;
    gint i;

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

    text = (const gchar *)xpathObj->nodesetval->nodeTab[0]->content;

    switch (type) {
        case XML_CONVERT_STRING:
            ret = g_variant_new_string(text);

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
    AgChart *chart = NULL;
    gchar *uri,
          *xml = NULL;
    guint length;
    xmlDocPtr doc;
    xmlXPathContextPtr xpath_context;
    GVariant *chart_name,
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
             *timezone;
    GsweTimestamp *timestamp;

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

    if ((chart_name = get_by_xpath(xpath_context, uri, "/chartinfo/data/name/text()", XML_CONVERT_STRING, err)) == NULL) {
        xmlFreeDoc(doc);
        g_free(xml);
        g_free(uri);

        return NULL;
    }

    if ((country = get_by_xpath(xpath_context, uri, "/chartinfo/data/place/country/text()", XML_CONVERT_STRING, err)) == NULL) {
        g_variant_unref(chart_name);
        xmlFreeDoc(doc);
        g_free(xml);
        g_free(uri);

        return NULL;
    }

    if ((city = get_by_xpath(xpath_context, uri, "/chartinfo/data/place/city/text()", XML_CONVERT_STRING, err)) == NULL) {
        g_variant_unref(chart_name);
        g_variant_unref(country);
        xmlFreeDoc(doc);
        xmlFreeDoc(doc);
        g_free(xml);
        g_free(uri);

        return NULL;
    }

    if ((longitude = get_by_xpath(xpath_context, uri, "/chartinfo/data/place/longitude/text()", XML_CONVERT_DOUBLE, err)) == NULL) {
        g_variant_unref(chart_name);
        g_variant_unref(country);
        g_variant_unref(city);
        xmlFreeDoc(doc);
        g_free(xml);
        g_free(uri);

        return NULL;
    }

    if ((latitude = get_by_xpath(xpath_context, uri, "/chartinfo/data/place/latitude/text()", XML_CONVERT_DOUBLE, err)) == NULL) {
        g_variant_unref(chart_name);
        g_variant_unref(country);
        g_variant_unref(city);
        g_variant_unref(longitude);
        xmlFreeDoc(doc);
        g_free(xml);
        g_free(uri);

        return NULL;
    }

    if ((altitude = get_by_xpath(xpath_context, uri, "/chartinfo/data/place/altitude/text()", XML_CONVERT_DOUBLE, err)) == NULL) {
        g_variant_unref(chart_name);
        g_variant_unref(country);
        g_variant_unref(city);
        g_variant_unref(longitude);
        g_variant_unref(latitude);
        xmlFreeDoc(doc);
        g_free(xml);
        g_free(uri);

        return NULL;
    }

    if ((year = get_by_xpath(xpath_context, uri, "/chartinfo/data/time/year/text()", XML_CONVERT_INT, err)) == NULL) {
        g_variant_unref(chart_name);
        g_variant_unref(country);
        g_variant_unref(city);
        g_variant_unref(longitude);
        g_variant_unref(latitude);
        g_variant_unref(altitude);
        xmlFreeDoc(doc);
        g_free(xml);
        g_free(uri);

        return NULL;
    }

    if ((month = get_by_xpath(xpath_context, uri, "/chartinfo/data/time/month/text()", XML_CONVERT_INT, err)) == NULL) {
        g_variant_unref(chart_name);
        g_variant_unref(country);
        g_variant_unref(city);
        g_variant_unref(longitude);
        g_variant_unref(latitude);
        g_variant_unref(altitude);
        g_variant_unref(year);
        xmlFreeDoc(doc);
        g_free(xml);
        g_free(uri);

        return NULL;
    }

    if ((day = get_by_xpath(xpath_context, uri, "/chartinfo/data/time/day/text()", XML_CONVERT_INT, err)) == NULL) {
        g_variant_unref(chart_name);
        g_variant_unref(country);
        g_variant_unref(city);
        g_variant_unref(longitude);
        g_variant_unref(latitude);
        g_variant_unref(altitude);
        g_variant_unref(year);
        g_variant_unref(month);
        xmlFreeDoc(doc);
        g_free(xml);
        g_free(uri);

        return NULL;
    }

    if ((hour = get_by_xpath(xpath_context, uri, "/chartinfo/data/time/hour/text()", XML_CONVERT_INT, err)) == NULL) {
        g_variant_unref(chart_name);
        g_variant_unref(country);
        g_variant_unref(city);
        g_variant_unref(longitude);
        g_variant_unref(latitude);
        g_variant_unref(altitude);
        g_variant_unref(year);
        g_variant_unref(month);
        g_variant_unref(day);
        xmlFreeDoc(doc);
        g_free(xml);
        g_free(uri);

        return NULL;
    }

    if ((minute = get_by_xpath(xpath_context, uri, "/chartinfo/data/time/minute/text()", XML_CONVERT_INT, err)) == NULL) {
        g_variant_unref(chart_name);
        g_variant_unref(country);
        g_variant_unref(city);
        g_variant_unref(longitude);
        g_variant_unref(latitude);
        g_variant_unref(altitude);
        g_variant_unref(year);
        g_variant_unref(month);
        g_variant_unref(day);
        g_variant_unref(hour);
        xmlFreeDoc(doc);
        g_free(xml);
        g_free(uri);

        return NULL;
    }

    if ((second = get_by_xpath(xpath_context, uri, "/chartinfo/data/time/second/text()", XML_CONVERT_INT, err)) == NULL) {
        g_variant_unref(chart_name);
        g_variant_unref(country);
        g_variant_unref(city);
        g_variant_unref(longitude);
        g_variant_unref(latitude);
        g_variant_unref(altitude);
        g_variant_unref(year);
        g_variant_unref(month);
        g_variant_unref(day);
        g_variant_unref(hour);
        g_variant_unref(minute);
        xmlFreeDoc(doc);
        g_free(xml);
        g_free(uri);

        return NULL;
    }

    if ((timezone = get_by_xpath(xpath_context, uri, "/chartinfo/data/time/timezone/text()", XML_CONVERT_DOUBLE, err)) == NULL) {
        g_variant_unref(chart_name);
        g_variant_unref(country);
        g_variant_unref(city);
        g_variant_unref(longitude);
        g_variant_unref(latitude);
        g_variant_unref(altitude);
        g_variant_unref(year);
        g_variant_unref(month);
        g_variant_unref(day);
        g_variant_unref(hour);
        g_variant_unref(minute);
        g_variant_unref(second);
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

    ag_chart_set_name(chart, g_variant_get_string(chart_name, NULL));
    g_variant_unref(chart_name);

    ag_chart_set_country(chart, g_variant_get_string(country, NULL));
    g_variant_unref(country);

    ag_chart_set_city(chart, g_variant_get_string(city, NULL));
    g_variant_unref(city);

    g_free(xml);
    g_free(uri);
    xmlXPathFreeContext(xpath_context);
    xmlFreeDoc(doc);

    return chart;
}

void
ag_chart_save_to_file(AgChart *chart, GFile *file, GError **err)
{
}

