#ifndef __AG_CHART_H__
#define __AG_CHART_H__

#include <glib-object.h>
#include <gtk/gtk.h>
#include <swe-glib.h>

#include "ag-db.h"

G_BEGIN_DECLS

typedef enum {
    AG_CHART_ERROR_LIBXML,
    AG_CHART_ERROR_CORRUPT_FILE,
    AG_CHART_ERROR_EMPTY_RECORD,
    AG_CHART_ERROR_INVALID_HOUSE_SYSTEM,
} AgChartError;

#define AG_TYPE_CHART         (ag_chart_get_type())
#define AG_CHART(o)           (G_TYPE_CHECK_INSTANCE_CAST((o), \
                                                          AG_TYPE_CHART, \
                                                          AgChart))
#define AG_CHART_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), \
                                                       AG_TYPE_CHART, \
                                                       AgChartClass))
#define AG_IS_CHART(o)        (G_TYPE_CHECK_INSTANCE_TYPE((o), AG_TYPE_CHART))
#define AG_IS_CHART_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE((k), AG_TYPE_CHART))
#define AG_CHART_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS((o), \
                                                         AG_TYPE_CHART, \
                                                         AgChartClass))

typedef struct _AgChart        AgChart;
typedef struct _AgChartClass   AgChartClass;

struct _AgChart {
    GsweMoment     parent_instance;
};

struct _AgChartClass {
    GsweMomentClass parent_class;
};

GType ag_chart_get_type(void) G_GNUC_CONST;

AgChart *ag_chart_new_full(GsweTimestamp   *timestamp,
                           gdouble         longitude,
                           gdouble         latitude,
                           gdouble         altitude,
                           GsweHouseSystem house_system);

AgChart *ag_chart_load_from_file(GFile  *file,
                                 GError **err);

AgChart *ag_chart_new_from_db_save(AgDbSave *save_data, GError **err);

void ag_chart_save_to_file(AgChart *chart,
                           GFile   *file,
                           GError  **err);

void ag_chart_export_svg_to_file(AgChart *chart,
                                 GFile   *file,
                                 GError  **err);

void ag_chart_set_name(AgChart     *chart,
                       const gchar *name);

gchar *ag_chart_get_name(AgChart *chart);

void ag_chart_set_country(AgChart     *chart,
                          const gchar *country);

gchar *ag_chart_get_country(AgChart *chart);

void ag_chart_set_city(AgChart     *chart,
                       const gchar *city);

gchar *ag_chart_get_city(AgChart *chart);

gchar *ag_chart_create_svg(AgChart *chart,
                           gsize   *length,
                           GError  **err);

GList *ag_chart_get_planets(AgChart *chart);

void ag_chart_set_note(AgChart *chart, const gchar *note);

const gchar *ag_chart_get_note(AgChart *chart);

gboolean ag_chart_save_to_db(AgChart *chart,
                             AgDbSave **old_save,
                             GtkWidget *window);

#define AG_CHART_ERROR (ag_chart_error_quark())
GQuark ag_chart_error_quark(void);

G_END_DECLS

#endif /* __AG_CHART_H__ */

