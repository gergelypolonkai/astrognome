#include "ag-chart.h"

struct _AgChartPrivate {
    gchar *save_buffer;
};

G_DEFINE_TYPE(AgChart, ag_chart, GSWE_TYPE_MOMENT);

#define GET_PRIVATE(instance) (G_TYPE_INSTANCE_GET_PRIVATE((instance), AG_TYPE_CHART, AgChartPrivate))

static void
ag_chart_class_init(AgChartClass *klass)
{
    g_type_class_add_private(klass, sizeof(AgChartPrivate));
}

static void
ag_chart_init(AgChart *chart)
{
    chart->priv = GET_PRIVATE(chart);
}

AgChart *
ag_chart_new_full(GsweTimestamp *timestamp, gdouble longitude, gdouble latitude, gdouble altitude, GsweHouseSystem house_system)
{
    AgChart *ret;
    GsweCoordinates *coords = g_new0(GsweCoordinates, 1);

    coords->longitude = longitude;
    coords->latitude = latitude;
    coords->altitude = altitude;

    ret = AG_CHART(g_object_new(AG_TYPE_CHART,
            "timestamp",    timestamp,
            "coordinates",  coords,
            "house-system", house_system,
            NULL));

    g_free(coords);

    return ret;
}

