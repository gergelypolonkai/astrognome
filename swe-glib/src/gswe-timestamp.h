#ifndef __SWE_GLIB_GSWE_TIMESTAMP_H__
#define __SWE_GLIB_GSWE_TIMESTAMP_H__

#include <glib-object.h>

#define GSWE_TYPE_TIMESTAMP             (gswe_timestamp_get_type())
#define GSWE_TIMESTAMP(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), GSWE_TYPE_TIMESTAMP, GsweTimestamp))
#define GSWE_IS_TIMESTAMP(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), GSWE_TYPE_TIMESTAMP))
#define GSWE_TIMESTAMP_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), GSWE_TYPE_TIMESTAMP, GsweTimestampClass))
#define GSWE_IS_TIMESTAMP_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), GSWE_TYPE_TIMESTAMP))
#define GSWE_TIMESTAMP_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), GSWE_TYPE_TIMESTAMP, GsweTimestampClass))

typedef struct _GsweTimestamp GsweTimestamp;
typedef struct _GsweTimestampClass GsweTimestampClass;
typedef struct _GsweTimestampPrivate GsweTimestampPrivate;

#define GSWE_TIMESTAMP_ERROR (gswe_timestamp_error_quark())
GQuark gswe_timestamp_error_quark(void);

typedef enum {
    GSWE_TIMESTAMP_ERROR_INVALID_DATE,
    GSWE_TIMESTAMP_ERROR_INVALID_TIME
} GsweTimestampError;

struct _GsweTimestamp {
    /* Parent instance structure */
    GObject parent_instance;

    /* Instance members */

    /*< private >*/
    GsweTimestampPrivate *priv;
};

struct _GsweTimestampClass {
    /* Parent class */
    GObjectClass parent_class;

    /* Class members */
    void (*changed)(GsweTimestamp *self);
};

GType gswe_timestamp_get_type(void);

/* Method definitions */
GsweTimestamp *gswe_timestamp_new(void);
GsweTimestamp *gswe_timestamp_new_from_julian_day(gdouble julian_day);
GsweTimestamp * gswe_timestamp_new_from_gregorian_full(gint year, gint month, gint day, gint hour, gint minute, gint second, gint microsecond, gdouble time_zone_offset);
void gswe_timestamp_set_instant_recalc(GsweTimestamp *timestamp, gboolean instant_recalc);
gboolean gswe_timestamp_get_instant_recalc(GsweTimestamp *timestamp);
void gswe_timestamp_set_gregorian_year(GsweTimestamp *timestamp, gint gregorian_year);
gint gswe_timestamp_get_gregorian_year(GsweTimestamp *timestamp);
void gswe_timestamp_set_gregorian_month(GsweTimestamp *timestamp, gint gregorian_month);
gint gswe_timestamp_get_gregorian_month(GsweTimestamp *timestamp);
void gswe_timestamp_set_gregorian_day(GsweTimestamp *timestamp, gint gregorian_day);
gint gswe_timestamp_get_gregorian_day(GsweTimestamp *timestamp);
void gswe_timestamp_set_gregorian_hour(GsweTimestamp *timestamp, gint gregorian_hour);
gint gswe_timestamp_get_gregorian_hour(GsweTimestamp *timestamp);
void gswe_timestamp_set_gregorian_minute(GsweTimestamp *timestamp, gint gregorian_minute);
gint gswe_timestamp_get_gregorian_minute(GsweTimestamp *timestamp);
void gswe_timestamp_set_gregorian_second(GsweTimestamp *timestamp, gint gregorian_second);
gint gswe_timestamp_get_gregorian_second(GsweTimestamp *timestamp);
void gswe_timestamp_set_gregorian_microsecond(GsweTimestamp *timestamp, gint gregorian_microsecond);
gint gswe_timestamp_get_gregorian_microsecond(GsweTimestamp *timestamp);
void gswe_timestamp_set_gregorian_timezone(GsweTimestamp *timestamp, gdouble gregorian_timezone_offset);
gdouble gswe_timestamp_get_gregorian_timezone(GsweTimestamp *timestamp);
void gswe_timestamp_set_julian_day(GsweTimestamp *timestamp, gdouble julian_day);
gdouble gswe_timestamp_get_julian_day(GsweTimestamp *timestamp);

#endif /* __SWE_GLIB_GSWE_TIMESTAMP_H__ */

