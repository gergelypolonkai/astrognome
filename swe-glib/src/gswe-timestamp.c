#include <math.h>
#include <glib.h>

#include "../../swe/src/swephexp.h"
#include "swe-glib-private.h"
#include "gswe-timestamp.h"

#define GSWE_TIMESTAMP_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GSWE_TYPE_TIMESTAMP, GsweTimestampPrivate))

typedef enum {
    VALID_GREGORIAN  = 1 << 0,
    VALID_JULIAN_DAY = 1 << 1
} TimestampValidity;

struct _GsweTimestampPrivate {
    gboolean instant_recalc;
    TimestampValidity valid_dates;

    gint gregorian_year;
    gint gregorian_month;
    gint gregorian_day;
    gint gregorian_hour;
    gint gregorian_minute;
    gint gregorian_second;
    gint gregorian_microsecond;
    gdouble gregorian_timezone_offset;

    gdouble julian_day;
};

enum {
    SIGNAL_CHANGED,
    SIGNAL_LAST
};

enum {
    PROP_0,
    PROP_INSTANT_RECALC,
    PROP_GREGORIAN_VALID,
    PROP_GREGORIAN_YEAR,
    PROP_GREGORIAN_MONTH,
    PROP_GREGORIAN_DAY,
    PROP_GREGORIAN_HOUR,
    PROP_GREGORIAN_MINUTE,
    PROP_GREGORIAN_SECOND,
    PROP_GREGORIAN_MICROSECOND,
    PROP_GREGORIAN_TIMEZONE_OFFSET,
    PROP_JULIAN_DAY_VALID
};

static guint gswe_timestamp_signals[SIGNAL_LAST] = { 0 };

static void gswe_timestamp_dispose(GObject *gobject);
static void gswe_timestamp_finalize(GObject *gobject);
static void gswe_timestamp_set_property(GObject *gobject, guint prop_id, const GValue *value, GParamSpec *pspec);
static void gswe_timestamp_get_property(GObject *gobject, guint prop_id, GValue *value, GParamSpec *pspec);
static void gswe_timestamp_calculate_all(GsweTimestamp *timestamp);
static void gswe_timestamp_calculate_gregorian(GsweTimestamp *timestamp);
static void gswe_timestamp_calculate_julian(GsweTimestamp *timestamp);

G_DEFINE_TYPE(GsweTimestamp, gswe_timestamp, G_TYPE_OBJECT);

static void
gswe_timestamp_class_init(GsweTimestampClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    GDateTime *local_time = g_date_time_new_now_local();

    g_type_class_add_private(klass, sizeof(GsweTimestampPrivate));

    gobject_class->dispose = gswe_timestamp_dispose;
    gobject_class->finalize = gswe_timestamp_finalize;
    gobject_class->set_property = gswe_timestamp_set_property;
    gobject_class->get_property = gswe_timestamp_get_property;

    /**
     * GsweTimestamp::changed:
     * @timestamp: the GsweTimestamp that receives the signal
     *
     * The ::changed signal is emitted each time the timestamp is changed
     */
    gswe_timestamp_signals[SIGNAL_CHANGED] = g_signal_new("changed", G_OBJECT_CLASS_TYPE(gobject_class), G_SIGNAL_RUN_FIRST, G_STRUCT_OFFSET(GsweTimestampClass, changed), NULL, NULL, g_cclosure_marshal_generic, G_TYPE_NONE, 0);

    /**
     * GsweTimestamp:instant-recalc:
     *
     * If set to TRUE, recalculate timestamp values instantly, when changing a
     * parameter (e.g. recalculate Julian date when changing Gregorian year).
     * Otherwise, the values are recalculated only upon request (e.g. on
     * calling #gswe_timestamp_get_julian_day()).
     */
    g_object_class_install_property(gobject_class, PROP_INSTANT_RECALC, g_param_spec_boolean("instant-recalc", "Instant recalculation", "Instantly recalculate values upon parameter change", FALSE, G_PARAM_READWRITE));

    /**
     * GsweTimestamp:gregorian-valid
     *
     * If TRUE, the Gregorian value stored in the GsweTimestamp object is
     * currently considered as valid, thus, no recalculation is needed.
     * Otherwise, the Gregorian date components will be recalculated upon
     * request.
     */
    g_object_class_install_property(gobject_class, PROP_GREGORIAN_VALID, g_param_spec_boolean("gregorian-valid", "Gregorian date is valid", "TRUE if the Gregorian date components are considered as valid.", TRUE, G_PARAM_READABLE));

    /**
     * GsweTimestamp:gregorian-year:
     *
     * The Gregorian year of the timestamp
     */
    g_object_class_install_property(gobject_class, PROP_GREGORIAN_YEAR, g_param_spec_int("gregorian-year", "Gregorian year", "The year according to the Gregorian calendar", G_MININT, G_MAXINT, g_date_time_get_year(local_time), G_PARAM_READWRITE));

    /**
     * GsweTimestamp:gregorian-month:
     *
     * The Gregorian month of the timestamp
     */
    g_object_class_install_property(gobject_class, PROP_GREGORIAN_MONTH, g_param_spec_int("gregorian-month", "Gregorian month", "The month according to the Gregorian calendar", 1, 12, g_date_time_get_month(local_time), G_PARAM_READWRITE));

    /**
     * GsweTimestamp:gregorian-day:
     *
     * The Gregorian day of the timestamp
     */
    g_object_class_install_property(gobject_class, PROP_GREGORIAN_DAY, g_param_spec_int("gregorian-day", "Gregorian day", "The day according to the Gregorian calendar", 1, 31, g_date_time_get_day_of_month(local_time), G_PARAM_READWRITE));

    /**
     * GsweTimestamp:gregorian-hour:
     *
     * The Gregorian hour of the timestamp
     */
    g_object_class_install_property(gobject_class, PROP_GREGORIAN_HOUR, g_param_spec_int("gregorian-hour", "Gregorian hour", "The hour according to the Gregorian calendar", 0, 23, g_date_time_get_hour(local_time), G_PARAM_READWRITE));

    /**
     * GsweTimestamp:gregorian-minute:
     *
     * The Gregorian minute of the timestamp
     */
    g_object_class_install_property(gobject_class, PROP_GREGORIAN_MINUTE, g_param_spec_int("gregorian-minute", "Gregorian minute", "The minute according to the Gregorian calendar", 0, 59, g_date_time_get_minute(local_time), G_PARAM_READWRITE));

    /**
     * GsweTimestamp:gregorian-second:
     *
     * The Gregorian second of the timestamp
     */
    g_object_class_install_property(gobject_class, PROP_GREGORIAN_SECOND, g_param_spec_int("gregorian-second", "Gregorian second", "The second according to the Gregorian calendar", 0, 61, g_date_time_get_second(local_time), G_PARAM_READWRITE));

    /**
     * GsweTimestamp:gregorian-microsecond:
     *
     * The Gregorian microsecond of the timestamp
     */
    g_object_class_install_property(gobject_class, PROP_GREGORIAN_MICROSECOND, g_param_spec_int("gregorian-microsecond", "Gregorian microsecond", "The microsecond according to the Gregorian calendar", 0, G_MAXINT, g_date_time_get_microsecond(local_time), G_PARAM_READWRITE));

    /**
     * GsweTimestamp:gregorian-timezone-offset:
     *
     * The time zone offset in hours, relative to UTC
     */
    g_object_class_install_property(gobject_class, PROP_GREGORIAN_TIMEZONE_OFFSET, g_param_spec_double("gregorian-timezone-offset", "Gregorian timezone offset", "The offset relative to UTC in the Gregorian calendar", -24.0, 24.0, 0.0, G_PARAM_READWRITE));

    /**
     * GsweTimestamp:julian-day-valid
     *
     * If TRUE, the Julian day value stored in the GsweTimestamp object is
     * currently considered as valid, thus, no recalculation is needed.
     * Otherwise, the Julian day components will be recalculated upon request.
     */
    g_object_class_install_property(gobject_class, PROP_JULIAN_DAY_VALID, g_param_spec_boolean("julian-day-valid", "Julian day is valid", "TRUE if the Julian day components are considered as valid.", TRUE, G_PARAM_READABLE));

    g_date_time_unref(local_time);
}

static void
gswe_timestamp_emit_changed(GsweTimestamp *timestamp)
{
    g_signal_emit(timestamp, gswe_timestamp_signals[SIGNAL_CHANGED], 0);
}

void
gswe_timestamp_init(GsweTimestamp *self)
{
    self->priv = GSWE_TIMESTAMP_GET_PRIVATE(self);
}

static void
gswe_timestamp_dispose(GObject *gobject)
{
    G_OBJECT_CLASS(gswe_timestamp_parent_class)->dispose(gobject);
}

static void
gswe_timestamp_finalize(GObject *gobject)
{
    G_OBJECT_CLASS(gswe_timestamp_parent_class)->finalize(gobject);
}

static void
gswe_timestamp_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
    GsweTimestamp *timestamp = GSWE_TIMESTAMP(object);

    switch (prop_id) {
        case PROP_INSTANT_RECALC:
            gswe_timestamp_calculate_all(timestamp);

            break;

        case PROP_GREGORIAN_YEAR:
            gswe_timestamp_set_gregorian_year(timestamp, g_value_get_int(value));

            break;

        case PROP_GREGORIAN_MONTH:
            gswe_timestamp_set_gregorian_month(timestamp, g_value_get_int(value));

            break;

        case PROP_GREGORIAN_DAY:
            gswe_timestamp_set_gregorian_day(timestamp, g_value_get_int(value));

            break;

        case PROP_GREGORIAN_HOUR:
            gswe_timestamp_set_gregorian_hour(timestamp, g_value_get_int(value));

            break;

        case PROP_GREGORIAN_MINUTE:
            gswe_timestamp_set_gregorian_minute(timestamp, g_value_get_int(value));

            break;

        case PROP_GREGORIAN_SECOND:
            gswe_timestamp_set_gregorian_second(timestamp, g_value_get_int(value));

            break;

        case PROP_GREGORIAN_MICROSECOND:
            gswe_timestamp_set_gregorian_microsecond(timestamp, g_value_get_int(value));

            break;

        case PROP_GREGORIAN_TIMEZONE_OFFSET:
            gswe_timestamp_set_gregorian_timezone(timestamp, g_value_get_double(value));

            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);

            break;
    }
}

static void
gswe_timestamp_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    GsweTimestamp *timestamp = GSWE_TIMESTAMP(object);

    switch (prop_id) {
        case PROP_INSTANT_RECALC:
            g_value_set_boolean(value, timestamp->priv->instant_recalc);

            break;

        case PROP_GREGORIAN_VALID:
            g_value_set_boolean(value, ((timestamp->priv->valid_dates & VALID_GREGORIAN) == VALID_GREGORIAN));

            break;

        case PROP_GREGORIAN_YEAR:
            gswe_timestamp_calculate_gregorian(timestamp);
            g_value_set_int(value, timestamp->priv->gregorian_year);

            break;

        case PROP_GREGORIAN_MONTH:
            gswe_timestamp_calculate_gregorian(timestamp);
            g_value_set_int(value, timestamp->priv->gregorian_month);

            break;

        case PROP_GREGORIAN_DAY:
            gswe_timestamp_calculate_gregorian(timestamp);
            g_value_set_int(value, timestamp->priv->gregorian_day);

            break;

        case PROP_GREGORIAN_HOUR:
            gswe_timestamp_calculate_gregorian(timestamp);
            g_value_set_int(value, timestamp->priv->gregorian_hour);

            break;

        case PROP_GREGORIAN_MINUTE:
            gswe_timestamp_calculate_gregorian(timestamp);
            g_value_set_int(value, timestamp->priv->gregorian_minute);

            break;

        case PROP_GREGORIAN_SECOND:
            gswe_timestamp_calculate_gregorian(timestamp);
            g_value_set_int(value, timestamp->priv->gregorian_second);

            break;

        case PROP_GREGORIAN_MICROSECOND:
            gswe_timestamp_calculate_gregorian(timestamp);
            g_value_set_int(value, timestamp->priv->gregorian_microsecond);

            break;

        case PROP_GREGORIAN_TIMEZONE_OFFSET:
            g_value_set_double(value, timestamp->priv->gregorian_timezone_offset);

            break;

        case PROP_JULIAN_DAY_VALID:
            g_value_set_boolean(value, ((timestamp->priv->valid_dates & VALID_JULIAN_DAY) == VALID_JULIAN_DAY));

            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);

            break;
    }
}

static void
gswe_timestamp_calculate_all(GsweTimestamp *timestamp)
{
    if ((timestamp->priv->valid_dates & VALID_JULIAN_DAY) != VALID_JULIAN_DAY) {
        gswe_timestamp_calculate_julian(timestamp);
    }

    if ((timestamp->priv->valid_dates & VALID_GREGORIAN) != VALID_GREGORIAN) {
        gswe_timestamp_calculate_gregorian(timestamp);
    }
}

static void
gswe_timestamp_calculate_gregorian(GsweTimestamp *timestamp)
{
    if ((timestamp->priv->valid_dates & VALID_GREGORIAN) == VALID_GREGORIAN) {
        return;
    }

    if (timestamp->priv->valid_dates == 0) {
        g_error("This timestamp object holds no valid values. This can't be good.");
    }

    g_warning("This method is not implemented yet.");
}

void
gswe_timestamp_set_instant_recalc(GsweTimestamp *timestamp, gboolean instant_recalc)
{
    timestamp->priv->instant_recalc = instant_recalc;

    if (instant_recalc == TRUE) {
        gswe_timestamp_calculate_all(timestamp);
    }
}

gboolean
gswe_timestamp_get_instant_recalc(GsweTimestamp *timestamp)
{
    return timestamp->priv->instant_recalc;
}

void
gswe_timestamp_set_gregorian_year(GsweTimestamp *timestamp, gint gregorian_year)
{
    timestamp->priv->gregorian_year = gregorian_year;
    timestamp->priv->valid_dates = VALID_GREGORIAN;

    if (timestamp->priv->instant_recalc == TRUE) {
        gswe_timestamp_calculate_all(timestamp);
    }

    gswe_timestamp_emit_changed(timestamp);
}

gint
gswe_timestamp_get_gregorian_year(GsweTimestamp *timestamp)
{
    gswe_timestamp_calculate_gregorian(timestamp);

    return timestamp->priv->gregorian_year;
}

void
gswe_timestamp_set_gregorian_month(GsweTimestamp *timestamp, gint gregorian_month)
{
    timestamp->priv->gregorian_month = gregorian_month;
    timestamp->priv->valid_dates = VALID_GREGORIAN;

    if (timestamp->priv->instant_recalc == TRUE) {
        gswe_timestamp_calculate_all(timestamp);
    }

    gswe_timestamp_emit_changed(timestamp);
}

gint
gswe_timestamp_get_gregorian_month(GsweTimestamp *timestamp)
{
    gswe_timestamp_calculate_gregorian(timestamp);

    return timestamp->priv->gregorian_month;
}

void
gswe_timestamp_set_gregorian_day(GsweTimestamp *timestamp, gint gregorian_day)
{
    timestamp->priv->gregorian_day = gregorian_day;
    timestamp->priv->valid_dates = VALID_GREGORIAN;

    if (timestamp->priv->instant_recalc == TRUE) {
        gswe_timestamp_calculate_all(timestamp);
    }

    gswe_timestamp_emit_changed(timestamp);
}

gint
gswe_timestamp_get_gregorian_day(GsweTimestamp *timestamp)
{
    gswe_timestamp_calculate_gregorian(timestamp);

    return timestamp->priv->gregorian_day;
}

void
gswe_timestamp_set_gregorian_hour(GsweTimestamp *timestamp, gint gregorian_hour)
{
    timestamp->priv->gregorian_hour = gregorian_hour;
    timestamp->priv->valid_dates = VALID_GREGORIAN;

    if (timestamp->priv->instant_recalc == TRUE) {
        gswe_timestamp_calculate_all(timestamp);
    }

    gswe_timestamp_emit_changed(timestamp);
}

gint
gswe_timestamp_get_gregorian_hour(GsweTimestamp *timestamp)
{
    gswe_timestamp_calculate_gregorian(timestamp);

    return timestamp->priv->gregorian_hour;
}

void
gswe_timestamp_set_gregorian_minute(GsweTimestamp *timestamp, gint gregorian_minute)
{
    timestamp->priv->gregorian_minute = gregorian_minute;
    timestamp->priv->valid_dates = VALID_GREGORIAN;

    if (timestamp->priv->instant_recalc == TRUE) {
        gswe_timestamp_calculate_all(timestamp);
    }

    gswe_timestamp_emit_changed(timestamp);
}

gint
gswe_timestamp_get_gregorian_minute(GsweTimestamp *timestamp)
{
    gswe_timestamp_calculate_gregorian(timestamp);

    return timestamp->priv->gregorian_minute;
}

void
gswe_timestamp_set_gregorian_second(GsweTimestamp *timestamp, gint gregorian_second)
{
    timestamp->priv->gregorian_second = gregorian_second;
    timestamp->priv->valid_dates = VALID_GREGORIAN;

    if (timestamp->priv->instant_recalc == TRUE) {
        gswe_timestamp_calculate_all(timestamp);
    }

    gswe_timestamp_emit_changed(timestamp);
}

gint
gswe_timestamp_get_gregorian_second(GsweTimestamp *timestamp)
{
    gswe_timestamp_calculate_gregorian(timestamp);

    return timestamp->priv->gregorian_second;
}

void
gswe_timestamp_set_gregorian_microsecond(GsweTimestamp *timestamp, gint gregorian_microsecond)
{
    timestamp->priv->gregorian_microsecond = gregorian_microsecond;
    timestamp->priv->valid_dates = VALID_GREGORIAN;

    if (timestamp->priv->instant_recalc == TRUE) {
        gswe_timestamp_calculate_all(timestamp);
    }

    gswe_timestamp_emit_changed(timestamp);
}

gint
gswe_timestamp_get_gregorian_microsecond(GsweTimestamp *timestamp)
{
    gswe_timestamp_calculate_gregorian(timestamp);

    return timestamp->priv->gregorian_microsecond;
}

void
gswe_timestamp_set_gregorian_timezone(GsweTimestamp *timestamp, gdouble gregorian_timezone_offset)
{
    timestamp->priv->gregorian_timezone_offset = gregorian_timezone_offset;
    timestamp->priv->valid_dates = VALID_GREGORIAN;

    if (timestamp->priv->instant_recalc == TRUE) {
        gswe_timestamp_calculate_all(timestamp);
    }

    gswe_timestamp_emit_changed(timestamp);
}

gdouble
gswe_timestamp_get_gregorian_timezone(GsweTimestamp *timestamp)
{
    gswe_timestamp_calculate_gregorian(timestamp);

    return timestamp->priv->gregorian_timezone_offset;
}

static void
gswe_timestamp_calculate_julian(GsweTimestamp *timestamp)
{
    gint utc_year,
         utc_month,
         utc_day,
         utc_hour,
         utc_minute,
         retval;
    gdouble utc_second,
            dret[2];
    gchar serr[AS_MAXCH];

    if ((timestamp->priv->valid_dates & VALID_JULIAN_DAY) == VALID_JULIAN_DAY) {
        return;
    }

    if (timestamp->priv->valid_dates == 0) {
        g_error("This timestamp object holds no valid values. This can't be good.");
    }

    swe_utc_time_zone(timestamp->priv->gregorian_year, timestamp->priv->gregorian_month, timestamp->priv->gregorian_day, timestamp->priv->gregorian_hour, timestamp->priv->gregorian_minute, timestamp->priv->gregorian_second + timestamp->priv->gregorian_microsecond / 1000.0, timestamp->priv->gregorian_timezone_offset, &utc_year, &utc_month, &utc_day, &utc_hour, &utc_minute, &utc_second);

    if ((retval = swe_utc_to_jd(utc_year, utc_month, utc_day, utc_hour, utc_minute, utc_second, SE_GREG_CAL, dret, serr)) == ERR) {
        g_error("Swiss Ephemeris error: %s", serr);
    } else {
        timestamp->priv->julian_day = dret[0];
        timestamp->priv->valid_dates |= VALID_JULIAN_DAY;
    }
}

void
gswe_timestamp_set_julian_day(GsweTimestamp *timestamp, gdouble julian_day)
{
    timestamp->priv->julian_day = julian_day;
    timestamp->priv->valid_dates = VALID_JULIAN_DAY;

    if (timestamp->priv->instant_recalc == TRUE) {
        gswe_timestamp_calculate_all(timestamp);
    }

    gswe_timestamp_emit_changed(timestamp);
}

gdouble
gswe_timestamp_get_julian_day(GsweTimestamp *timestamp)
{
    gswe_timestamp_calculate_julian(timestamp);

    return timestamp->priv->julian_day;
}

GQuark
gswe_timestamp_error_quark(void)
{
    return g_quark_from_static_string("swe-glib-gswe-timestamp-error");
}

GsweTimestamp *
gswe_timestamp_new(void)
{
    return GSWE_TIMESTAMP(g_object_new(GSWE_TYPE_TIMESTAMP, NULL));
}

GsweTimestamp *
gswe_timestamp_new_from_gregorian_full(gint year, gint month, gint day, gint hour, gint minute, gint second, gint microsecond, gdouble time_zone_offset)
{
    GsweTimestamp *timestamp = GSWE_TIMESTAMP(g_object_new(GSWE_TYPE_TIMESTAMP,
                "gregorian-year",            year,
                "gregorian-month",           month,
                "gregorian-day",             day,
                "gregorian-hour",            hour,
                "gregorian-minute",          minute,
                "gregorian-second",          second,
                "gregorian-microsecond",     microsecond,
                "gregorian-timezone-offset", time_zone_offset,
                NULL));

    timestamp->priv->valid_dates = VALID_GREGORIAN;

    return timestamp;
}

GsweTimestamp *
gswe_timestamp_new_from_julian_day(gdouble julian_day)
{
    GsweTimestamp *timestamp = gswe_timestamp_new();

    gswe_timestamp_set_julian_day(timestamp, julian_day);

    return timestamp;
}

