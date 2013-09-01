#ifndef __GSWE_MOMENT_H__
#define __GSWE_MOMENT_H__

#include <glib-object.h>

#define GSWE_TYPE_MOMENT            (gswe_moment_get_type())
#define GSWE_MOMENT(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GSWE_TYPE_MOMENT, GsweMoment))
#define GSWE_IS_MOMENT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GSWE_TYPE_MOMENT))
#define GSWE_MOMENT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GSWE_TYPE_MOMENT, GsweMomentClass))
#define GSWE_IS_MOMENT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GSWE_TYPE_MOMENT))
#define GSWE_MOMENT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GSWE_TYPE_MOMENT, GsweMomentClass))

typedef struct _GsweMoment GsweMoment;
typedef struct _GsweMomentClass GsweMomentClass;
typedef struct _GsweMomentPrivate GsweMomentPrivate;

#define GSWE_MOMENT_ERROR gswe_moment_error_quark()
GQuark gswe_moment_error_quark(void);

/*
 * GsweMomentError:
 *
 * Error values for GsweTimestamp initialization
 */
//typedef enum {
//} GsweMomentError;

/**
 * GsweCoordinates:
 * @longitude: longitude part of the coordinates
 * @latitude: latitude part of the coordinates
 *
 * GsweCoordinates specifies an exact point on Earth's surface
 */
typedef struct _GsweCoordinates {
    gdouble longitude;
    gdouble latitude;
} GsweCoordinates;

struct _GsweMoment {
    /* Parent instance structure */
    GObject parent_instance;

    /* Instance members */

    /*< private >*/
    GsweMomentPrivate *priv;
};

struct _GsweMomentClass {
    /* Parent class structure */
    GObjectClass parent_class;

    /* Class members */

    /*< private >*/
    void (*moment_changed)(GsweMoment *moment);
};

/* used by GSWE_TYPE_MOMENT */
GType gswe_moment_get_type(void);

/* Method definitions */
GsweMoment *gswe_moment_new(void);
void gswe_moment_set_timestamp(GsweMoment *moment, GsweTimestamp *timestamp);

#endif /* __GSWE_MOMENT_H__ */

