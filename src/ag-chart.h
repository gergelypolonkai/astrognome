/* ag-chart.h - Chart manipulation object for Astrognome
 *
 * Copyright (C) 2014 Polonkai Gergely
 *
 * Astrognome is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 3 of the License,
 * or (at your option) any later version.
 *
 * Astrognome is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; if not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef __AG_CHART_H__
#define __AG_CHART_H__

#include <glib-object.h>
#include <gtk/gtk.h>
#include <swe-glib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "ag-db.h"
#include "ag-display-theme.h"

#define AG_CHART_DEFAULT_RING_SIZE 600
#define AG_CHART_DEFAULT_ICON_SIZE 30

G_BEGIN_DECLS

typedef enum {
    AG_CHART_ERROR_LIBXML,
    AG_CHART_ERROR_CORRUPT_FILE,
    AG_CHART_ERROR_EMPTY_RECORD,
    AG_CHART_ERROR_INVALID_HOUSE_SYSTEM,
    AG_CHART_ERROR_NOT_IMPLEMENTED,
    AG_CHART_ERROR_INVALID_PLAC_FILE,
    AG_CHART_ERROR_UNSUPPORTED_PLAC_FILE,
    AG_CHART_ERROR_RENDERING_ERROR,
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

typedef void (*AgChartSaveImageFunc)(AgChart *,
                                     GFile *,
                                     AgDisplayTheme *,
                                     GError **);

GType ag_chart_get_type(void) G_GNUC_CONST;

AgChart *ag_chart_new_full(GsweTimestamp   *timestamp,
                           gdouble         longitude,
                           gdouble         latitude,
                           gdouble         altitude,
                           GsweHouseSystem house_system);

AgChart *ag_chart_new_preview(GsweTimestamp   *timestamp,
                              gdouble         longitude,
                              gdouble         latitude,
                              gdouble         altitude,
                              GsweHouseSystem house_system);

AgChart *ag_chart_load_from_agc(GFile  *file,
                                GError **err);

AgChart *ag_chart_load_from_placidus_file(GFile  *file,
                                          GError **err);

AgChart *ag_chart_new_from_db_save(AgDbChartSave *save_data,
                                   gboolean      preview,
                                   GError        **err);

void ag_chart_save_to_file(AgChart *chart,
                           GFile   *file,
                           GError  **err);

void ag_chart_export_svg_to_file(AgChart        *chart,
                                 GFile          *file,
                                 AgDisplayTheme *theme,
                                 GError         **err);

void ag_chart_export_jpg_to_file(AgChart        *chart,
                                 GFile          *file,
                                 AgDisplayTheme *theme,
                                 GError         **err);

void ag_chart_set_name(AgChart     *chart,
                       const gchar *name);

const gchar *ag_chart_get_name(AgChart *chart);

void ag_chart_set_country(AgChart     *chart,
                          const gchar *country);

const gchar *ag_chart_get_country(AgChart *chart);

void ag_chart_set_city(AgChart     *chart,
                       const gchar *city);

const gchar *ag_chart_get_city(AgChart *chart);

gchar *ag_chart_create_svg(AgChart        *chart,
                           gsize          *length,
                           gboolean       rendering,
                           AgDisplayTheme *theme,
                           guint          image_size,
                           guint          icon_size,
                           GError         **err);

GList *ag_chart_get_planets(AgChart *chart);

void ag_chart_set_note(AgChart *chart, const gchar *note);

const gchar *ag_chart_get_note(AgChart *chart);

AgDbChartSave *ag_chart_get_db_save(AgChart *chart);

GdkPixbuf *ag_chart_get_pixbuf(AgChart        *chart,
                               guint          image_size,
                               guint          icon_size,
                               AgDisplayTheme *theme,
                               GError         **err);

void ag_chart_set_db_id(AgChart *chart, gint db_id);

gint ag_chart_get_db_id(AgChart *chart);

#define AG_CHART_ERROR (ag_chart_error_quark())
GQuark ag_chart_error_quark(void);

G_END_DECLS

#endif /* __AG_CHART_H__ */

