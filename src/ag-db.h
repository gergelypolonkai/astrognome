/* ag-db.h - Database manipulation functionality for Astrognome
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
#ifndef __AG_DB_H__
#define __AG_DB_H__

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define AG_TYPE_DB         (ag_db_get_type())
#define AG_DB(o)           (G_TYPE_CHECK_INSTANCE_CAST((o), \
                                                       AG_TYPE_DB, \
                                                       AgDb))
#define AG_DB_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), \
                                                    AG_TYPE_DB, \
                                                    AgDbClass))
#define AG_IS_DB(o)        (G_TYPE_CHECK_INSTANCE_TYPE((o), AG_TYPE_DB))
#define AG_IS_DB_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE((k), AG_TYPE_DB))
#define AG_DB_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS((o), \
                                                      AG_TYPE_DB, \
                                                      AgDbClass))

typedef struct _AgDb      AgDb;
typedef struct _AgDbClass AgDbClass;

struct _AgDb {
    GObject parent_instance;
};

struct _AgDbClass {
    GObjectClass parent_class;
};

typedef struct _AgDbChartSave {
    gint db_id;
    gchar *name;
    gchar *country;
    gchar *city;
    gdouble longitude;
    gdouble latitude;
    gdouble altitude;
    gint year;
    guint month;
    guint day;
    guint hour;
    guint minute;
    guint second;
    gdouble timezone;
    gchar *house_system;
    gchar *note;
    gint refcount;
} AgDbChartSave;

GType ag_db_chart_save_get_type(void);
#define AG_TYPE_DB_CHART_SAVE (ag_db_chart_save_get_type())

typedef enum {
    AG_DB_ERROR_NO_CHART,
    AG_DB_ERROR_DATABASE_ERROR,
} AgDbError;

GType ag_db_get_type(void) G_GNUC_CONST;

AgDb *ag_db_get(void);

gboolean ag_db_chart_save(AgDb           *db,
                          AgDbChartSave  *save_data,
                          GError         **err);

AgDbChartSave *ag_db_chart_save_new(void);

AgDbChartSave *ag_db_chart_save_ref(AgDbChartSave *save_data);

void ag_db_chart_save_unref(AgDbChartSave *save_data);

GList *ag_db_chart_get_list(AgDb *db, GError **err);

AgDbChartSave *ag_db_chart_get_data_by_id(AgDb *db, guint row_id, GError **err);

gboolean ag_db_chart_delete(AgDb *db, gint row_id, GError **err);

gboolean ag_db_chart_save_identical(const AgDbChartSave *a,
                                    const AgDbChartSave *b,
                                    gboolean            chart_only);

#define AG_DB_ERROR (ag_db_error_quark())
GQuark ag_db_error_quark(void);

G_END_DECLS

#endif /* __AG_DB_H__ */
