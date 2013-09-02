#include "swe-glib.h"
#include "gswe-types.h"
#include "gswe-moment.h"
#include "swe-glib-private.h"

#include "../../swe/src/swephexp.h"

#define SYNODIC 29.53058867

#define GSWE_MOMENT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GSWE_TYPE_MOMENT, GsweMomentPrivate))

/**
 * GsweMomentPrivate:
 * @timestamp: a #GsweTimestmp object representing the current local time at
 *             the given position specified by @coordinates. Be warned though,
 *             that the time zone is NOT checked against the coordinates!
 * @coordinates: the coordinates of the observers position
 * @revision: an internal counter which is incremented whenever the timestamp
 *            or the coordinates change. Planetary positions are recalculated
 *            if this number changes
 */
struct _GsweMomentPrivate {
    GsweTimestamp *timestamp;
    GsweCoordinates coordinates;
    GsweHouseSystem house_system;
    guint revision;
    GList *house_list;
    guint house_revision;
    GList *planet_list;
    guint points_revision;
    GHashTable *element_points;
    GHashTable *quality_points;
    guint moon_phase_revision;
    GsweMoonPhaseData moon_phase;
    GList *aspect_list;
    guint aspect_revision;
};

enum {
    SIGNAL_CHANGED,
    SIGNAL_LAST
};

enum {
    PROP_0,
    PROP_TIMESTAMP
};

struct GsweAspectFinder {
    GswePlanet planet1;
    GswePlanet planet2;
};

static guint gswe_moment_signals[SIGNAL_LAST] = {0};

static void gswe_moment_dispose(GObject *gobject);
static void gswe_moment_finalize(GObject *gobject);
static void gswe_moment_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void gswe_moment_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);

G_DEFINE_TYPE(GsweMoment, gswe_moment, G_TYPE_OBJECT);

static void
gswe_moment_class_init(GsweMomentClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    g_type_class_add_private(klass, sizeof(GsweMomentPrivate));

    gobject_class->dispose = gswe_moment_dispose;
    gobject_class->finalize = gswe_moment_finalize;
    gobject_class->set_property = gswe_moment_set_property;
    gobject_class->get_property = gswe_moment_get_property;

    /**
     * GsweMoment::changed:
     * @moment: the GsweMoment object that received the signal
     *
     * The ::changed signal is emitted each time the time or coordinates are changed
     */
    gswe_moment_signals[SIGNAL_CHANGED] = g_signal_new("changed", G_OBJECT_CLASS_TYPE(gobject_class), G_SIGNAL_RUN_FIRST, G_STRUCT_OFFSET(GsweMomentClass, changed), NULL, NULL, g_cclosure_marshal_generic, G_TYPE_NONE, 0);

    /**
     * GsweMoment:timestamp:
     *
     * The timestamp associated with this moment
     */
    g_object_class_install_property(gobject_class, PROP_TIMESTAMP, g_param_spec_object("timestamp", "Timestamp", "Timestamp of this moment", GSWE_TYPE_TIMESTAMP, G_PARAM_READWRITE));
}

static void
gswe_moment_emit_changed(GsweMoment *moment)
{
    g_signal_emit(moment, gswe_moment_signals[SIGNAL_CHANGED], 0);
}

void
gswe_moment_init(GsweMoment *moment)
{
    moment->priv = GSWE_MOMENT_GET_PRIVATE(moment);

    moment->priv->timestamp = NULL;
    moment->priv->house_list = NULL;
    moment->priv->planet_list = NULL;
    moment->priv->aspect_list = NULL;
    moment->priv->element_points = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
    moment->priv->quality_points = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
    moment->priv->house_revision = 0;
    moment->priv->points_revision = 0;
    moment->priv->moon_phase_revision = 0;
    moment->priv->aspect_revision = 0;
    moment->priv->revision = 1;
}

static void
gswe_moment_timestamp_changed(GsweMoment *moment, gpointer data)
{
    moment->priv->revision++;
    gswe_moment_emit_changed(moment);
}

static void
gswe_moment_dispose(GObject *gobject)
{
    GsweMoment *moment = GSWE_MOMENT(gobject);

    g_signal_handlers_disconnect_by_func(moment->priv->timestamp, gswe_moment_timestamp_changed, NULL);

    g_clear_object(&moment->priv->timestamp);

    G_OBJECT_CLASS(gswe_moment_parent_class)->dispose(gobject);
}

static void
gswe_moment_finalize(GObject *gobject)
{
    GsweMoment *moment = GSWE_MOMENT(gobject);

    g_list_free_full(moment->priv->house_list, g_free);
    g_list_free_full(moment->priv->planet_list, g_free);

    G_OBJECT_CLASS(gswe_moment_parent_class)->finalize(gobject);
}

static void
gswe_moment_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
    GsweMoment *moment = GSWE_MOMENT(object);

    switch (prop_id) {
        case PROP_TIMESTAMP:
            gswe_moment_set_timestamp(moment, (g_value_get_object(value)));

            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);

            break;
    }
}

static void
gswe_moment_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    GsweMoment *moment = GSWE_MOMENT(object);
    GsweMomentPrivate *priv = moment->priv;

    switch (prop_id) {
        case PROP_TIMESTAMP:
            g_value_set_object(value, priv->timestamp);

            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);

            break;
    }
}

/**
 * gswe_moment_set_timestamp:
 * @moment: A GsweMoment object
 * @timestamp: A GsweTimestamp object. The moment object holds a reference on
 *             timestamp, which is cleared when a new timestamp is set, or the
 *             moment object is disposed.
 *
 * Sets a new timestamp for this planetary moment. Also emits the ::changed
 * signal to notify owner of this change. This helps redrawing screen data
 * according to the new time value.
 */
void
gswe_moment_set_timestamp(GsweMoment *moment, GsweTimestamp *timestamp)
{
    if (moment->priv->timestamp != NULL) {
        g_signal_handlers_disconnect_by_func(moment->priv->timestamp, gswe_moment_timestamp_changed, NULL);
        g_clear_object(&moment->priv->timestamp);
    }

    moment->priv->timestamp = timestamp;
    g_object_ref(timestamp);
    g_signal_connect(G_OBJECT(timestamp), "changed", G_CALLBACK(gswe_moment_timestamp_changed), NULL);

    /* Emit the changed signal to notify registrants of the change */
    gswe_moment_emit_changed(moment);
}

/**
 * gswe_moment_get_timestamp:
 * @moment: The GsweMoment object of which you requent its timestamp object
 *
 * Get the timestamp object of the given GsweMoment. The moment object holds a
 * reference to this object, so if you need the timestamp object after moment
 * is disposed, call g_object_ref() on it!
 *
 * Returns: a #GsweTimestamp object assigned to the given moment
 */
GsweTimestamp *
gswe_moment_get_timestamp(GsweMoment *moment)
{
    return moment->priv->timestamp;
}

GQuark
gswe_moment_error_quark(void)
{
    return g_quark_from_static_string("swe-glib-gswe-moment-error");
}

GsweMoment *
gswe_moment_new(void)
{
    return (GsweMoment *)g_object_new(GSWE_TYPE_MOMENT, NULL);
}

GsweMoment *
gswe_moment_new_full(GsweTimestamp *timestamp, gdouble longitude, gdouble latitude, gdouble altitude, GsweHouseSystem house_system)
{
    GsweMoment *moment = gswe_moment_new();

    moment->priv->timestamp = timestamp;
    g_object_ref(timestamp);
    g_signal_connect(G_OBJECT(timestamp), "changed", G_CALLBACK(gswe_moment_timestamp_changed), NULL);
    moment->priv->coordinates.longitude = longitude;
    moment->priv->coordinates.latitude = latitude;
    moment->priv->coordinates.altitude = altitude;
    moment->priv->house_system = house_system;

    return moment;
}

static gint
find_by_planet_id(gconstpointer a, gconstpointer b)
{
    const GswePlanetData *planet_data = a;
    const GswePlanet *planet = b;

    if (planet_data->planet_id == *planet) {
        return 0;
    }

    return 1;
}

static void
gswe_calculate_data_by_position(GsweMoment *moment, GswePlanet planet, gdouble position)
{
    GswePlanetData *planet_data = (GswePlanetData *)(g_list_find_custom(moment->priv->planet_list, &planet, find_by_planet_id)->data);
    GsweZodiac sign;
    GsweSignInfo *sign_info;

    if (planet_data == NULL) {
        return;
    }

    if (planet_data->revision == moment->priv->revision) {
        return;
    }

    sign = (GsweZodiac)ceil(position / 30.0);

    if ((sign_info = g_hash_table_lookup(gswe_sign_info_table, GINT_TO_POINTER(sign))) == NULL) {
        g_error("Calculations brought an unknown sign!");
    }

    planet_data->position = position;
    planet_data->retrograde = FALSE;
    planet_data->house = gswe_moment_get_house(moment, position);
    planet_data->sign = sign_info;
    planet_data->revision = moment->priv->revision;
}

static void
gswe_moment_calculate_house_positions(GsweMoment *moment)
{
    gdouble cusps[13],
            ascmc[10];
    gint i;
    GsweHouseSystemInfo *house_system_data;

    if (moment->priv->house_revision == moment->priv->revision) {
        return;
    }

    if ((house_system_data = g_hash_table_lookup(gswe_house_system_info_table, GINT_TO_POINTER(moment->priv->house_system))) == NULL) {
        g_error("Unknown house system!");
    }

    swe_houses(gswe_timestamp_get_julian_day(moment->priv->timestamp), moment->priv->coordinates.latitude, moment->priv->coordinates.longitude, house_system_data->sweph_id, cusps, ascmc);

    g_list_free_full(moment->priv->house_list, g_free);
    moment->priv->house_list = NULL;

    for (i = 12; i >= 1; i--) {
        GsweHouseData *house_data = g_new0(GsweHouseData, 1);

        house_data->house = i;
        house_data->cusp_position = cusps[i];

        if ((house_data->sign = g_hash_table_lookup(gswe_sign_info_table, GINT_TO_POINTER((gint)ceilf(cusps[i] / 30.0)))) == NULL) {
            g_error("Calculations brought an unknown sign!");
        }

        moment->priv->house_list = g_list_prepend(moment->priv->house_list, house_data);
    }

    moment->priv->house_revision = moment->priv->revision;

    if (gswe_moment_has_planet(moment, GSWE_PLANET_ASCENDENT)) {
        gswe_calculate_data_by_position(moment, GSWE_PLANET_ASCENDENT, ascmc[0]);
    }

    if (gswe_moment_has_planet(moment, GSWE_PLANET_MC)) {
        gswe_calculate_data_by_position(moment, GSWE_PLANET_MC, ascmc[1]);
    }

    if (gswe_moment_has_planet(moment, GSWE_PLANET_VERTEX)) {
        gswe_calculate_data_by_position(moment, GSWE_PLANET_VERTEX, ascmc[3]);
    }
}

GList *
gswe_moment_get_house_cusps(GsweMoment *moment)
{
    if (moment->priv->house_revision != moment->priv->revision) {
        gswe_moment_calculate_house_positions(moment);
    }

    return moment->priv->house_list;
}

gboolean
gswe_moment_has_planet(GsweMoment *moment, GswePlanet planet)
{
    return (g_list_find_custom(moment->priv->planet_list, &planet, find_by_planet_id) != NULL);
}

void
gswe_moment_add_planet(GsweMoment *moment, GswePlanet planet)
{
    GswePlanetData *planet_data = g_new0(GswePlanetData, 1);
    GswePlanetInfo *planet_info;

    if (gswe_moment_has_planet(moment, planet)) {
        return;
    }

    if ((planet_info = g_hash_table_lookup(gswe_planet_info_table, GINT_TO_POINTER(planet))) == NULL) {
        g_warning("Unknown planet ID: %d", planet);

        return;
    }

    planet_data->planet_id = planet;
    planet_data->planet_info = planet_info;
    planet_data->position = 0.0;
    planet_data->house = 1;
    planet_data->sign = NULL;
    planet_data->revision = 0;

    moment->priv->planet_list = g_list_append(moment->priv->planet_list, planet_data);
}

static void
planet_add(gpointer key, gpointer value, gpointer user_data)
{
    GswePlanet planet = (GswePlanet)GPOINTER_TO_INT(key);
    GsweMoment *moment = GSWE_MOMENT(user_data);

    gswe_moment_add_planet(moment, planet);
}

void
gswe_moment_add_all_planets(GsweMoment *moment)
{
    g_hash_table_foreach(gswe_planet_info_table, planet_add, moment);
}

static void
gswe_moment_calculate_planet(GsweMoment *moment, GswePlanet planet)
{
    GswePlanetData *planet_data = (GswePlanetData *)(g_list_find_custom(moment->priv->planet_list, &planet, find_by_planet_id)->data);
    gchar serr[AS_MAXCH];
    gint ret;
    gdouble x2[6];

    if (planet_data == NULL) {
        return;
    }

    if (planet_data->revision == moment->priv->revision) {
        return;
    }

    if (planet_data->planet_info->real_body == FALSE) {
        g_warning("The position data of planet %d can not be calculated by this function", planet);

        return;
    }

    swe_set_topo(moment->priv->coordinates.longitude, moment->priv->coordinates.latitude, moment->priv->coordinates.altitude);
    if ((ret = swe_calc(gswe_timestamp_get_julian_day(moment->priv->timestamp), planet_data->planet_info->sweph_id, SEFLG_SPEED | SEFLG_TOPOCTR, x2, serr)) < 0) {
        g_warning("Swiss Ephemeris error: %s", serr);

        return;
    } else if (ret != (SEFLG_SPEED | SEFLG_TOPOCTR)) {
        g_warning("Swiss Ephemeris error: %s", serr);
    }

    gswe_calculate_data_by_position(moment, planet, x2[0]);

    planet_data->retrograde = (x2[3] < 0);
}

void
calculate_planet(gpointer data, gpointer user_data)
{
    GswePlanetData *planet_data = data;
    GsweMoment *moment = user_data;

    gswe_moment_calculate_planet(moment, planet_data->planet_id);
}

void
gswe_moment_calculate_all_planets(GsweMoment *moment)
{
    g_list_foreach(moment->priv->planet_list, calculate_planet, moment);
}

GList *
gswe_moment_get_planets(GsweMoment *moment)
{
    return moment->priv->planet_list;
}

gint
gswe_moment_get_house(GsweMoment *moment, gdouble position)
{
    gint i;
    gswe_moment_calculate_house_positions(moment);

    for (i = 1; i <= 12; i++) {
        gint j = (i < 12) ? i + 1 : 1;
        gdouble cusp_i = *(gdouble *)g_list_nth_data(moment->priv->house_list, i - 1),
                cusp_j = *(gdouble *)g_list_nth_data(moment->priv->house_list, j - 1);

        if (cusp_j < cusp_i) {
            if ((position >= cusp_i) || (position < cusp_j)) {
                return i;
            }
        } else {
            if ((position >= cusp_i) && (position < cusp_j)) {
                return i;
            }
        }
    }

    return 0;
}

GswePlanetData *
gswe_moment_get_planet(GsweMoment *moment, GswePlanet planet)
{
    GswePlanetData *planet_data = (GswePlanetData *)(g_list_find_custom(moment->priv->planet_list, &planet, find_by_planet_id)->data);

    if (planet_data == NULL) {
        return NULL;
    }

    gswe_moment_calculate_planet(moment, planet);

    return planet_data;
}

static void
add_points(GswePlanetData *planet_data, GsweMoment *moment)
{
    guint point;

    gswe_moment_calculate_planet(moment, planet_data->planet_id);

    point = GPOINTER_TO_INT(g_hash_table_lookup(moment->priv->element_points, GINT_TO_POINTER(planet_data->sign->element))) + planet_data->planet_info->points;
    g_hash_table_replace(moment->priv->element_points, GINT_TO_POINTER(planet_data->sign->element), GINT_TO_POINTER(point));

    point = GPOINTER_TO_INT(g_hash_table_lookup(moment->priv->quality_points, GINT_TO_POINTER(planet_data->sign->quality)));

    point += planet_data->planet_info->points;
    g_hash_table_replace(moment->priv->quality_points, GINT_TO_POINTER(planet_data->sign->quality), GINT_TO_POINTER(point));
}

static void
gswe_moment_calculate_points(GsweMoment *moment)
{
    if (moment->priv->points_revision == moment->priv->revision) {
        return;
    }

    g_hash_table_remove_all(moment->priv->element_points);
    g_hash_table_remove_all(moment->priv->quality_points);

    g_list_foreach(moment->priv->planet_list, (GFunc)add_points, moment);

    moment->priv->points_revision = moment->priv->revision;
}

guint
gswe_moment_get_element_points(GsweMoment *moment, GsweElement element)
{
    guint point;

    gswe_moment_calculate_points(moment);

    point = GPOINTER_TO_INT(g_hash_table_lookup(moment->priv->element_points, GINT_TO_POINTER(element)));

    return point;
}

guint
gswe_moment_get_quality_points(GsweMoment *moment, GsweQuality quality)
{
    guint point;

    gswe_moment_calculate_points(moment);

    point = GPOINTER_TO_INT(g_hash_table_lookup(moment->priv->quality_points, GINT_TO_POINTER(quality)));

    return point;
}

GsweMoonPhaseData *
gswe_moment_get_moon_phase(GsweMoment *moment)
{
    gdouble difference,
            phase_percent;

    if (moment->priv->moon_phase_revision == moment->priv->revision) {
        return &(moment->priv->moon_phase);
    }

    difference = (gswe_timestamp_get_julian_day(moment->priv->timestamp) - gswe_timestamp_get_julian_day(gswe_full_moon_base_date));
    phase_percent = fmod((difference * 100) / SYNODIC, 100);

    if (phase_percent < 0) {
        phase_percent += 100.0;
    }

    if ((phase_percent < 0) || (phase_percent > 100)) {
        g_error("Error during Moon phase calculation!");
    }

    moment->priv->moon_phase.illumination = (50.0 - fabs(phase_percent - 50.0)) * 2;

    if (phase_percent == 0) {
        moment->priv->moon_phase.phase = GSWE_MOON_PHASE_NEW;
    } else if (phase_percent < 25) {
        moment->priv->moon_phase.phase = GSWE_MOON_PHASE_WAXING_CRESCENT;
    } else if (phase_percent == 25) {
        moment->priv->moon_phase.phase = GSWE_MOON_PHASE_WAXING_HALF;
    } else if (phase_percent < 50) {
        moment->priv->moon_phase.phase = GSWE_MOON_PHASE_WAXING_GIBBOUS;
    } else if (phase_percent == 50) {
        moment->priv->moon_phase.phase = GSWE_MOON_PHASE_FULL;
    } else if (phase_percent < 75) {
        moment->priv->moon_phase.phase = GSWE_MOON_PHASE_WANING_GIBBOUS;
    } else if (phase_percent == 75) {
        moment->priv->moon_phase.phase = GSWE_MOON_PHASE_WANING_HALF;
    } else if (phase_percent < 100) {
        moment->priv->moon_phase.phase = GSWE_MOON_PHASE_WANING_CRESCENT;
    } else {
        moment->priv->moon_phase.phase = GSWE_MOON_PHASE_DARK;
    }

    moment->priv->moon_phase_revision = moment->priv->revision;

    return &(moment->priv->moon_phase);
}

static gint
find_aspect_by_both_planets(GsweAspectData *aspect, struct GsweAspectFinder *aspect_finder)
{
    if (((aspect->planet1->planet_id == aspect_finder->planet1) && (aspect->planet2->planet_id == aspect_finder->planet2)) || ((aspect->planet1->planet_id == aspect_finder->planet2) && (aspect->planet2->planet_id == aspect_finder->planet1))) {
        return 0;
    }

    return 1;
}

static gboolean
find_aspect(gpointer aspect_p, GsweAspectInfo *aspect_info, GsweAspectData *aspect_data)
{
    GsweAspect aspect = GPOINTER_TO_INT(aspect_p);
    gdouble distance,
            diff,
            planet_orb,
            aspect_orb;

    distance = fabs(aspect_data->planet1->position - aspect_data->planet2->position);

    if (distance > 180.0) {
        distance = 360.0 - distance;
    }

    diff = fabs(aspect_info->size - distance);
    planet_orb = fmin(aspect_data->planet1->planet_info->orb, aspect_data->planet2->planet_info->orb);
    aspect_orb = fmax(1.0, planet_orb - aspect_info->orb_modifier);

    if (diff < aspect_orb) {
        aspect_data->aspect = aspect;
        aspect_data->aspect_info = aspect_info;

        if (aspect_info->size == 0) {
            aspect_data->difference = (1 - ((360.0 - diff) / 360.0)) * 100.0;
        } else {
            aspect_data->difference = (1 - ((aspect_info->size - diff) / aspect_info->size)) * 100.0;
        }

        return TRUE;
    }

    return FALSE;
}

static void
gswe_moment_calculate_aspects(GsweMoment *moment)
{
    GList *oplanet,
          *iplanet;

    if (moment->priv->aspect_revision == moment->priv->revision) {
        return;
    }

    gswe_moment_calculate_all_planets(moment);
    g_list_free_full(moment->priv->aspect_list, g_free);

    for (oplanet = moment->priv->planet_list; oplanet; oplanet = oplanet->next) {
        for (iplanet = moment->priv->planet_list; iplanet; iplanet = iplanet->next) {
            GswePlanetData *outer_planet = oplanet->data,
                           *inner_planet = iplanet->data;
            struct GsweAspectFinder aspect_finder;
            GsweAspectData *aspect_data;

            if (outer_planet->planet_id == inner_planet->planet_id) {
                continue;
            }

            aspect_finder.planet1 = outer_planet->planet_id;
            aspect_finder.planet2 = inner_planet->planet_id;

            if (g_list_find_custom(moment->priv->aspect_list, &aspect_finder, (GCompareFunc)find_aspect_by_both_planets) != NULL) {
                continue;
            }

            aspect_data = g_new0(GsweAspectData, 1);
            aspect_data->planet1 = outer_planet;
            aspect_data->planet2 = inner_planet;
            aspect_data->aspect = GSWE_ASPECT_NONE;

            (void)g_hash_table_find(gswe_aspect_info_table, (GHRFunc)find_aspect, aspect_data);

            if (aspect_data->aspect == GSWE_ASPECT_NONE) {
                aspect_data->aspect_info = g_hash_table_lookup(gswe_aspect_info_table, GINT_TO_POINTER(GSWE_ASPECT_NONE));
            }

            moment->priv->aspect_list = g_list_prepend(moment->priv->aspect_list, aspect_data);
        }
    }

    moment->priv->aspect_revision = moment->priv->revision;
}

GList *
gswe_moment_get_aspects(GsweMoment *moment)
{
    gswe_moment_calculate_aspects(moment);

    return moment->priv->aspect_list;
}

GList *
gswe_moment_get_planet_aspects(GsweMoment *moment, GswePlanet planet)
{
    return NULL;
}

