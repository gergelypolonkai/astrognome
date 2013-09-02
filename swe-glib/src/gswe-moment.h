#ifndef __GSWE_MOMENT_H__
#define __GSWE_MOMENT_H__

#include <glib-object.h>

#include "gswe-timestamp.h"

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
 * @altitude: altitude relative to sea level
 *
 * GsweCoordinates specifies an exact point on Earth's surface
 */
typedef struct _GsweCoordinates {
    gdouble longitude;
    gdouble latitude;
    gdouble altitude;
} GsweCoordinates;

/**
 * GswePlanetData:
 * @planet_id: A GswePlanet, the identifier of the planet
 * @planet_info: A GswePlanetInfo structure, holding every information about the planet
 * @position: The longitude position of the planet
 * @house: Number of the house in which the planet is in
 * @sign: A GsweSignInfo structure, holding every information about the sign the planet is in
 * @revision: An internal version number of the calculation
 */
typedef struct {
    GswePlanet planet_id;
    GswePlanetInfo *planet_info;
    gdouble position;
    gboolean retrograde;
    gint house;
    GsweSignInfo *sign;
    guint revision;
} GswePlanetData;

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
    void (*changed)(GsweMoment *moment);
};

/* used by GSWE_TYPE_MOMENT */
GType gswe_moment_get_type(void);

/* Method definitions */
GsweMoment *gswe_moment_new(void);
GsweMoment *gswe_moment_new_full(GsweTimestamp *timestamp, gdouble longitude, gdouble latitude, gdouble altitude, GsweHouseSystem house_system);
void gswe_moment_set_timestamp(GsweMoment *moment, GsweTimestamp *timestamp);
GList *gswe_moment_get_house_cusps(GsweMoment *moment);
gint gswe_moment_get_house(GsweMoment *moment, gdouble position);
gboolean gswe_moment_has_planet(GsweMoment *moment, GswePlanet planet);
void gswe_moment_add_planet(GsweMoment *moment, GswePlanet planet);
void gswe_moment_add_all_planets(GsweMoment *moment);
GList *gswe_moment_get_planets(GsweMoment *moment);
GswePlanetData *gswe_moment_get_planet(GsweMoment *moment, GswePlanet planet);

#endif /* __GSWE_MOMENT_H__ */

