#ifndef __AG_CHART_H__
#define __AG_CHART_H__

#include <glib-object.h>
#include <swe-glib.h>

G_BEGIN_DECLS

#define AG_TYPE_CHART         (ag_chart_get_type())
#define AG_CHART(o)           (G_TYPE_CHECK_INSTANCE_CAST((o), AG_TYPE_CHART, AgChart))
#define AG_CHART_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), AG_TYPE_CHART, AgChartClass))
#define AG_IS_CHART(o)        (G_TYPE_CHECK_INSTANCE_TYPE((o), AG_TYPE_CHART))
#define AG_IS_CHART_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE((k), AG_TYPE_CHART))
#define AG_CHART_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS((o), AG_TYPE_CHART, AgChartClass))

typedef struct _AgChart AgChart;
typedef struct _AgChartClass AgChartClass;
typedef struct _AgChartPrivate AgChartPrivate;

struct _AgChart {
    GsweMoment parent_instance;
    AgChartPrivate *priv;
};

struct _AgChartClass {
    GsweMomentClass parent_class;
};

GType ag_chart_get_type(void) G_GNUC_CONST;
AgChart *ag_chart_new_full(GsweTimestamp *timestamp, gdouble longitude, gdouble latitude, gdouble altitude, GsweHouseSystem house_system);

void ag_chart_load_from_file(const gchar *path, GError **err);
void ag_chart_save_to_file(const gchar *path, GError **err);

G_END_DECLS

#endif /* __AG_CHART_H__ */

