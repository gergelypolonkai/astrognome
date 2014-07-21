#ifndef __AG_DB_H__
#define __AG_DB_H__

#include <glib-object.h>

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

typedef struct _AgDbSave {
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
} AgDbSave;

GType ag_db_get_type(void) G_GNUC_CONST;

AgDb *ag_db_get(void);

void ag_db_save_data_free(AgDbSave *save_data);

G_END_DECLS

#endif /* __AG_DB_H__ */
