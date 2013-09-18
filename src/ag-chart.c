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

