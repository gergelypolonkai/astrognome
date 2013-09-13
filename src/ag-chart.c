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
ag_chart_new(void)
{
    return NULL;
}

