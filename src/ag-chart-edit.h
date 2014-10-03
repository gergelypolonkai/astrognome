#ifndef __AG_CHART_EDIT_H__
#define __AG_CHART_EDIT_H__

#include <gtk/gtk.h>

#include "ag-db.h"

G_BEGIN_DECLS

#define AG_TYPE_CHART_EDIT                                              \
   (ag_chart_edit_get_type())
#define AG_CHART_EDIT(obj)                                              \
   (G_TYPE_CHECK_INSTANCE_CAST ((obj),                                  \
                                AG_TYPE_CHART_EDIT,                     \
                                AgChartEdit))
#define AG_CHART_EDIT_CLASS(klass)                                      \
   (G_TYPE_CHECK_CLASS_CAST ((klass),                                   \
                             AG_TYPE_CHART_EDIT,                        \
                             AgChartEditClass))
#define IS_AG_CHART_EDIT(obj)                                           \
   (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                                  \
                                AG_TYPE_CHART_EDIT))
#define IS_AG_CHART_EDIT_CLASS(klass)                                   \
   (G_TYPE_CHECK_CLASS_TYPE ((klass),                                   \
                             AG_TYPE_CHART_EDIT))
#define AG_CHART_EDIT_GET_CLASS(obj)                                    \
   (G_TYPE_INSTANCE_GET_CLASS ((obj),                                   \
                               AG_TYPE_CHART_EDIT,                      \
                               AgChartEditClass))

typedef struct _AgChartEdit      AgChartEdit;
typedef struct _AgChartEditClass AgChartEditClass;

struct _AgChartEditClass
{
    GtkGridClass parent_class;
};

struct _AgChartEdit
{
    GtkGrid parent;
};

GType ag_chart_edit_get_type (void) G_GNUC_CONST;

void ag_chart_edit_set_name(AgChartEdit *chart_edit, const gchar *name);

const gchar *ag_chart_edit_get_name(AgChartEdit *chart_edit);

void ag_chart_edit_set_country(AgChartEdit *chart_edit, const gchar *country);

const gchar *ag_chart_edit_get_country(AgChartEdit *chart_edit);

void ag_chart_edit_set_city(AgChartEdit *chart_edit, const gchar *city);

const gchar *ag_chart_edit_get_city(AgChartEdit *chart_edit);

void ag_chart_edit_set_latitude(AgChartEdit *chart_edit, gdouble latitude);

gdouble ag_chart_edit_get_latitude(AgChartEdit *chart_edit);

void ag_chart_edit_set_longitude(AgChartEdit *chart_edit, gdouble longitude);

gdouble ag_chart_edit_get_longitude(AgChartEdit *chart_edit);

void ag_chart_edit_set_from_timestamp(AgChartEdit   *chart_edit,
                                      GsweTimestamp *timestamp);

GsweTimestamp *ag_chart_edit_get_to_timestamp(AgChartEdit   *chart_edit,
                                              GsweTimestamp *timestamp);

void ag_chart_edit_set_note(AgChartEdit *chart_edit, const gchar *note);

gchar *ag_chart_edit_get_note(AgChartEdit *chart_edit);

void ag_chart_edit_update(AgChartEdit *chart_edit);

AgDbChartSave *ag_chart_edit_get_chart_save(AgChartEdit *chart_edit);

void ag_chart_edit_clear(AgChartEdit *chart_edit);

G_END_DECLS

#endif /* __AG_CHART_EDIT_H__ */
