#include <glib.h>

#include "calculate.h"

#include "../swe/src/swephexp.h"
#include "../swe-glib/src/swe-glib.h"

#define EPHEDIR "/home/polesz/Projektek/c/astrognome/swe/data"

typedef struct {
    int planetId;
    gchar *name;
    gdouble orb;
    zodiacSign domicile1,
               domicile2,
               exile1,
               exile2,
               exalted,
               fall;
} planetData_t;

const char *moonStateName[] = {
    "New Moon",
    "Waxing Crescent Moon",
    "Waxing Half Moon",
    "Waxing Gibbous Moon",
    "Full Moon",
    "Waning Gibbous Moon",
    "Waning Half Moon",
    "Waning Crescent Moon",
    "Dark Moon"
};

typedef struct {
    gchar *name;
    guint size;
    guint orbModifier;
    gboolean harmonic;
    gboolean major;
} aspectData_t;

typedef struct {
    gchar *name;
    zodiacSign startSign;
    gboolean middleAxis;
} mirrorpointData_t;

const aspectData_t aspectData[] = {
    // Name            Size Orb  Harmonic Major
    { "Conjuction",    0,   0,   TRUE,    TRUE  },
    { "Opposition",    180, 0,   TRUE,    TRUE  },
    { "Trine",         120, 0,   TRUE,    TRUE  },
    { "Square",        90,  0,   FALSE,   TRUE  },
    { "Sextile",       60,  1,   TRUE,    TRUE  },
    { "Quincunx",      150, 2,   FALSE,   FALSE },
    { "Semi-sextile",  30,  2,   TRUE,    FALSE },
    { "Semi-square",   45,  2,   FALSE,   FALSE },
    { "Sesqui-square", 135, 2,   FALSE,   FALSE },
    { "Quintile",      72,  3,   TRUE,    FALSE },
    { "Bi-quintile",   144, 3,   TRUE,    FALSE }
};

const mirrorpointData_t mirrorpointData[] = {
    { "Aries/Libra",        SIGN_ARIES,  FALSE },
    { "mid Taurus/Scoripo", SIGN_TAURUS, TRUE  },
    { "Cancer/Capricorn",   SIGN_CANCER, FALSE },
    { "mid Leo/Aquarius",   SIGN_LEO,    TRUE  },
};

#define ADD_PLANET(ht, v, i, n, o, dom1, dom2, exi1, exi2, exa, fal) (v) = g_new0(planetData_t, 1); \
                                                                     (v)->planetId = (i); \
                                                                     (v)->name = g_strdup(n); \
                                                                     (v)->orb = (o); \
                                                                     (v)->domicile1 = (dom1); \
                                                                     (v)->domicile2 = (dom2); \
                                                                     (v)->exile1 = (exi1); \
                                                                     (v)->exile2 = (exi2); \
                                                                     (v)->exalted = (exa); \
                                                                     (v)->fall = (fal); \
                                                                     g_hash_table_replace((ht), GINT_TO_POINTER(i), (v));

struct aspect_check_data {
    GList *planetIdList;
    guint currentOuterPlanetId;
    guint currentInnerPlanetId;
    GHashTable *planetInfoTable;
    GHashTable *planetDataTable;
};

void
check_aspects_inner_loop(gpointer data, gpointer user_data)
{
    struct aspect_check_data *checkData = user_data;
    gint outerPlanetId = GPOINTER_TO_INT(g_list_nth_data(checkData->planetIdList, checkData->currentOuterPlanetId));
    gint innerPlanetId = GPOINTER_TO_INT(g_list_nth_data(checkData->planetIdList, checkData->currentInnerPlanetId));
    planetInfo_t *outerPlanet,
                 *innerPlanet;
    planetData_t *outerPlanetData,
                 *innerPlanetData;
    gdouble planetOrb,
            distance,
            difference;
    gint i;
    const aspectData_t *aspect = NULL;

    if (outerPlanetId == innerPlanetId) {
        checkData->currentInnerPlanetId++;

        return;
    }

    outerPlanet = g_hash_table_lookup(checkData->planetInfoTable, GINT_TO_POINTER(outerPlanetId));
    innerPlanet = g_hash_table_lookup(checkData->planetInfoTable, GINT_TO_POINTER(innerPlanetId));

    g_assert(outerPlanet != NULL);
    g_assert(innerPlanet != NULL);

    outerPlanetData = g_hash_table_lookup(checkData->planetDataTable, GINT_TO_POINTER(outerPlanetId));
    innerPlanetData = g_hash_table_lookup(checkData->planetDataTable, GINT_TO_POINTER(innerPlanetId));

    g_assert(outerPlanetData != NULL);
    g_assert(innerPlanetData != NULL);

    planetOrb = fmin(outerPlanetData->orb, innerPlanetData->orb);
    distance = fabs(outerPlanet->position - innerPlanet->position);

    if (distance > 180.0) {
        distance = 360.0 - distance;
    }

    for (i = 0; i < sizeof(aspectData) / sizeof(aspectData_t); i++) {
        gdouble diff = fabs(aspectData[i].size - distance);
        gdouble aspectOrb = fmax(1.0, planetOrb - aspectData[i].orbModifier);

        if (diff <= aspectOrb) {
            aspect = &(aspectData[i]);
            if (aspectData[i].size == 0) {
                difference = (1 - ((360.0 - diff) / 360.0)) * 100.0;
            } else {
                difference = (1 - ((aspectData[i].size - diff) / aspectData[i].size)) * 100.0;
            }

            break;
        }
    }

    if (aspect != NULL) {
        printf("%s vs. %s: %s (±%f%%)\n", outerPlanetData->name, innerPlanetData->name, aspect->name, difference);
    }

    checkData->currentInnerPlanetId++;
}

void
check_aspects_outer_loop(gpointer data, gpointer user_data)
{
    struct aspect_check_data *checkData = user_data;

    checkData->currentInnerPlanetId = checkData->currentOuterPlanetId;
    printf("\n");

    g_list_foreach(g_list_nth(checkData->planetIdList, checkData->currentOuterPlanetId), check_aspects_inner_loop, user_data);

    checkData->currentOuterPlanetId++;
}

void
check_mirrorpoints_inner_loop(gpointer data, gpointer user_data)
{
    struct aspect_check_data *checkData = user_data;
    gint outerPlanetId = GPOINTER_TO_INT(g_list_nth_data(checkData->planetIdList, checkData->currentOuterPlanetId));
    gint innerPlanetId = GPOINTER_TO_INT(g_list_nth_data(checkData->planetIdList, checkData->currentInnerPlanetId));
    planetInfo_t *outerPlanet,
                 *innerPlanet;
    planetData_t *outerPlanetData,
                 *innerPlanetData;
    gdouble planetOrb,
            difference;
    gint i;
    const mirrorpointData_t *mirrorpoint = NULL;

    if (outerPlanetId == innerPlanetId) {
        checkData->currentInnerPlanetId++;

        return;
    }

    outerPlanet = g_hash_table_lookup(checkData->planetInfoTable, GINT_TO_POINTER(outerPlanetId));
    innerPlanet = g_hash_table_lookup(checkData->planetInfoTable, GINT_TO_POINTER(innerPlanetId));

    g_assert(outerPlanet != NULL);
    g_assert(innerPlanet != NULL);

    outerPlanetData = g_hash_table_lookup(checkData->planetDataTable, GINT_TO_POINTER(outerPlanetId));
    innerPlanetData = g_hash_table_lookup(checkData->planetDataTable, GINT_TO_POINTER(innerPlanetId));

    g_assert(outerPlanetData != NULL);
    g_assert(innerPlanetData != NULL);

    planetOrb = fmin(outerPlanetData->orb, innerPlanetData->orb);

    for (i = 0; i < sizeof(mirrorpointData) / sizeof(mirrorpointData_t); i++) {
        gdouble mirrorPosition;
        gdouble startPoint = (mirrorpointData[i].startSign - 1) * 30;

        if (mirrorpointData[i].middleAxis == TRUE) {
            startPoint += 15.0;
        }

        mirrorPosition = 2 * startPoint - outerPlanet->position;

        if (mirrorPosition < 0) {
            mirrorPosition += 360.0;
        }

        if ((difference = fabs(innerPlanet->position - mirrorPosition)) <= planetOrb) {
            mirrorpoint = &(mirrorpointData[i]);

            break;
        }
    }

    if (mirrorpoint != NULL) {
        printf("%s vs. %s: %s (±%f)\n", outerPlanetData->name, innerPlanetData->name, mirrorpoint->name, difference);
    }

    checkData->currentInnerPlanetId++;
}

void
check_mirrorpoints_outer_loop(gpointer data, gpointer user_data)
{
    struct aspect_check_data *checkData = user_data;

    checkData->currentInnerPlanetId = checkData->currentOuterPlanetId;
    printf("\n");

    g_list_foreach(g_list_nth(checkData->planetIdList, checkData->currentOuterPlanetId), check_mirrorpoints_inner_loop, user_data);

    checkData->currentOuterPlanetId++;
}

void
free_planet_data(gpointer data)
{
    planetData_t *planetData = data;

    g_free(planetData->name);
    g_free(planetData);
}

void
print_house_cusp(gpointer data, gpointer user_data)
{
    GsweHouseData *house_data = data;

    printf("House %2d..: %s (%f)\n", house_data->house, house_data->sign->name, house_data->cusp_position);
}

int
main(int argc, char *argv[])
{
    int year = 1981,
        month = 3,
        day = 11,
        hour = 23,
        min = 39,
        sec = 34;
    double timezone = 1.0,
           lon = 20.766666,
           lat = 48.2,
           alt = 200,
           te,
           cusps[13],
           ascmc[10];
    planetInfo_t *planetInfo;
    GsweMoonPhaseData *moon_phase;
    GHashTable *planetDataTable,
               *planetInfoTable;
    planetData_t *planetData;
    GList *planetIdList;
    struct aspect_check_data aspectCheckData;
    GsweTimestamp *timestamp;
    GsweMoment *moment;
    GswePlanetData *planet_data;

#if 1
    year = 1983;
    month = 3;
    day = 7;
    hour = 11;
    min = 54;
    sec = 47;
    lon = 19.081599;
    lat = 47.462485;
#endif

    planetDataTable = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, free_planet_data);
    planetInfoTable = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_free);

    // Initialize planet data table

    ADD_PLANET(planetDataTable, planetData, SE_SUN,                  "Sun",                 13.0, SIGN_LEO,         SIGN_NONE,     SIGN_AQUARIUS,    SIGN_NONE,   SIGN_ARIES,     SIGN_LIBRA);
    ADD_PLANET(planetDataTable, planetData, SE_MOON,                 "Moon",                9.0,  SIGN_CANCER,      SIGN_NONE,     SIGN_CAPRICORN,   SIGN_NONE,   SIGN_TAURUS,    SIGN_SCORPIO);
    ADD_PLANET(planetDataTable, planetData, SE_MERCURY,              "Mercury",             7.0,  SIGN_GEMINI,      SIGN_VIRGO,    SIGN_SAGITTARIUS, SIGN_PISCES, SIGN_VIRGO,     SIGN_PISCES);
    ADD_PLANET(planetDataTable, planetData, SE_VENUS,                "Venus",               7.0,  SIGN_TAURUS,      SIGN_LIBRA,    SIGN_SCORPIO,     SIGN_ARIES,  SIGN_PISCES,    SIGN_VIRGO);
    ADD_PLANET(planetDataTable, planetData, SE_MARS,                 "Mars",                7.0,  SIGN_ARIES,       SIGN_SCORPIO,  SIGN_LIBRA,       SIGN_TAURUS, SIGN_CAPRICORN, SIGN_CANCER);
    ADD_PLANET(planetDataTable, planetData, SE_JUPITER,              "Jupiter",             9.0,  SIGN_SAGITTARIUS, SIGN_PISCES,   SIGN_GEMINI,      SIGN_VIRGO,  SIGN_CANCER,    SIGN_CAPRICORN);
    ADD_PLANET(planetDataTable, planetData, SE_SATURN,               "Saturn",              7.0,  SIGN_CAPRICORN,   SIGN_AQUARIUS, SIGN_CANCER,      SIGN_LEO,    SIGN_LIBRA,     SIGN_ARIES);
    ADD_PLANET(planetDataTable, planetData, SE_URANUS,               "Uranus",              5.0,  SIGN_AQUARIUS,    SIGN_NONE,     SIGN_NONE,        SIGN_NONE,   SIGN_NONE,      SIGN_NONE);
    ADD_PLANET(planetDataTable, planetData, SE_NEPTUNE,              "Neptune",             5.0,  SIGN_PISCES,      SIGN_NONE,     SIGN_NONE,        SIGN_NONE,   SIGN_NONE,      SIGN_NONE);
    ADD_PLANET(planetDataTable, planetData, SE_PLUTO,                "Pluto",               3.0,  SIGN_SCORPIO,     SIGN_NONE,     SIGN_NONE,        SIGN_NONE,   SIGN_NONE,      SIGN_NONE);
    ADD_PLANET(planetDataTable, planetData, SE_CHIRON,               "Chiron",              2.0,  SIGN_NONE,        SIGN_NONE,     SIGN_NONE,        SIGN_NONE,   SIGN_NONE,      SIGN_NONE);
    ADD_PLANET(planetDataTable, planetData, SE_MEAN_NODE,            "Ascending Moon Node", 2.0,  SIGN_NONE,        SIGN_NONE,     SIGN_NONE,        SIGN_NONE,   SIGN_NONE,      SIGN_NONE);
    ADD_PLANET(planetDataTable, planetData, SE_MEAN_APOG,            "Dark Moon",           2.0,  SIGN_NONE,        SIGN_NONE,     SIGN_NONE,        SIGN_NONE,   SIGN_NONE,      SIGN_NONE);
    ADD_PLANET(planetDataTable, planetData, SE_CERES,                "Ceres",               2.0,  SIGN_NONE,        SIGN_NONE,     SIGN_NONE,        SIGN_NONE,   SIGN_NONE,      SIGN_NONE);
    ADD_PLANET(planetDataTable, planetData, SE_PALLAS,               "Pallas",              2.0,  SIGN_NONE,        SIGN_NONE,     SIGN_NONE,        SIGN_NONE,   SIGN_NONE,      SIGN_NONE);
    ADD_PLANET(planetDataTable, planetData, SE_JUNO,                 "Juno",                2.0,  SIGN_NONE,        SIGN_NONE,     SIGN_NONE,        SIGN_NONE,   SIGN_NONE,      SIGN_NONE);
    ADD_PLANET(planetDataTable, planetData, SE_VESTA,                "Vesta",               2.0,  SIGN_NONE,        SIGN_NONE,     SIGN_NONE,        SIGN_NONE,   SIGN_NONE,      SIGN_NONE);
    ADD_PLANET(planetDataTable, planetData, SE_NPLANETS + SE_ASC,    "Ascendent",           9.0,  SIGN_NONE,        SIGN_NONE,     SIGN_NONE,        SIGN_NONE,   SIGN_NONE,      SIGN_NONE);
    ADD_PLANET(planetDataTable, planetData, SE_NPLANETS + SE_MC,     "Midheaven",           5.0,  SIGN_NONE,        SIGN_NONE,     SIGN_NONE,        SIGN_NONE,   SIGN_NONE,      SIGN_NONE);
    ADD_PLANET(planetDataTable, planetData, SE_NPLANETS + SE_VERTEX, "Vertex",              2.0,  SIGN_NONE,        SIGN_NONE,     SIGN_NONE,        SIGN_NONE,   SIGN_NONE,      SIGN_NONE);

    gswe_init(EPHEDIR);

    timestamp = gswe_timestamp_new_from_gregorian_full(year, month, day, hour, min, sec, 0, 1.0);
    moment = gswe_moment_new_full(timestamp, lon, lat, alt, GSWE_HOUSE_SYSTEM_PLACIDUS);
    gswe_moment_add_all_planets(moment);

    if (set_location_and_time(lon, lat, alt, year, month, day, hour, min, sec, timezone, &te) == 0) {
        return 1;
    }

    printf("Date: %02d.%02d.%d at %02d:%02d:%02d, at %f, %f\n", year, month, day, hour, min, sec, lon, lat);

    printf("\nHOUSES\n======\n\n");

    swe_houses(te, lat, lon, 'P', cusps, ascmc);

    g_list_foreach(gswe_moment_get_house_cusps(moment), print_house_cusp, NULL);

    printf("\nPLANETS AND POINTS\n==================\n\n");

    // Ascendent
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_ASCENDENT);
    printf("%s: %s (%f)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->position);

    planetInfo = g_new0(planetInfo_t, 1);
    planetInfo->position = planet_data->position;
    planetInfo->sign = planet_data->sign->sign_id;
    planetInfo->house = 1;
    planetInfo->retrograde = FALSE;
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_NPLANETS + SE_ASC), planetInfo);

    // Midheaven
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_MC);
    printf("%s: %s (%f)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->position);

    planetInfo = g_new0(planetInfo_t, 1);
    planetInfo->position = planet_data->position;
    planetInfo->sign = planet_data->sign->sign_id;
    planetInfo->house = 10;
    planetInfo->retrograde = FALSE;
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_NPLANETS + SE_MC), planetInfo);

    // Vertex
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_VERTEX);
    printf("%s: %s (%f)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->position);

    planetInfo = g_new0(planetInfo_t, 1);
    planetInfo->position = planet_data->position;
    planetInfo->sign = planet_data->sign->sign_id;
    planetInfo->house = planet_data->house;
    planetInfo->retrograde = FALSE;
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_NPLANETS + SE_VERTEX), planetInfo);

    // Sun
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_SUN);
    printf("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    planetInfo = get_planet_info(SE_SUN, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_SUN), planetInfo);

    // Moon
    moon_phase = gswe_moment_get_moon_phase(moment);
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_MOON);
    printf("%s: %s (%.2f%% visibility), %s, House: %d (%f%s)\n", planet_data->planet_info->name, moonStateName[moon_phase->phase], moon_phase->illumination, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    planetInfo = get_planet_info(SE_MOON, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_MOON), planetInfo);

    // Mercury
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_MERCURY);
    printf("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    planetInfo = get_planet_info(SE_MERCURY, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_MERCURY), planetInfo);

    // Venus
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_VENUS);
    printf("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    planetInfo = get_planet_info (SE_VENUS, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_VENUS), planetInfo);

    // Mars
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_MARS);
    printf("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    planetInfo = get_planet_info(SE_MARS, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_MARS), planetInfo);

    // Jupiter
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_JUPITER);
    printf("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    planetInfo = get_planet_info(SE_JUPITER, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_JUPITER), planetInfo);

    // Saturn
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_SATURN);
    printf("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    planetInfo = get_planet_info(SE_SATURN, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_SATURN), planetInfo);

    // Uranus
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_URANUS);
    printf("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    planetInfo = get_planet_info(SE_URANUS, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_URANUS), planetInfo);

    // Neptune
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_NEPTUNE);
    printf("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    planetInfo = get_planet_info(SE_NEPTUNE, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_NEPTUNE), planetInfo);

    // Pluto
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_PLUTO);
    printf("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    planetInfo = get_planet_info(SE_PLUTO, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_PLUTO), planetInfo);

    // Mean node
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_MOON_NODE);
    printf("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    planetInfo = get_planet_info(SE_MEAN_NODE, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_MEAN_NODE), planetInfo);

    // Mean apogee
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_MOON_APOGEE);
    printf("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    planetInfo = get_planet_info(SE_MEAN_APOG, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_MEAN_APOG), planetInfo);

    // Chiron
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_CHIRON);
    printf("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    planetInfo = get_planet_info(SE_CHIRON, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_CHIRON), planetInfo);

    // Ceres
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_CERES);
    printf("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    planetInfo = get_planet_info(SE_CERES, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_CERES), planetInfo);

    // Pallas
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_PALLAS);
    printf("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    planetInfo = get_planet_info(SE_PALLAS, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_PALLAS), planetInfo);

    // Juno
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_JUNO);
    printf("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    planetInfo = get_planet_info(SE_JUNO, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_JUNO), planetInfo);

    // Vesta
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_VESTA);
    printf("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    planetInfo = get_planet_info(SE_VESTA, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_VESTA), planetInfo);

    printf("\nELEMENTS\n========\n\n");

    printf("Fire: %d\n", gswe_moment_get_element_points(moment, GSWE_ELEMENT_FIRE));
    printf("Earth: %d\n", gswe_moment_get_element_points(moment, GSWE_ELEMENT_EARTH));
    printf("Air: %d\n", gswe_moment_get_element_points(moment, GSWE_ELEMENT_AIR));
    printf("Water: %d\n", gswe_moment_get_element_points(moment, GSWE_ELEMENT_WATER));

    printf("\nQUALITIES\n=========\n\n");

    printf("Cardinal: %d\n", gswe_moment_get_quality_points(moment, GSWE_QUALITY_CARDINAL));
    printf("Fix: %d\n", gswe_moment_get_quality_points(moment, GSWE_QUALITY_FIX));
    printf("Mutable: %d\n", gswe_moment_get_quality_points(moment, GSWE_QUALITY_MUTABLE));

    printf("\nASPECTS\n=======\n\n");

    planetIdList = g_hash_table_get_keys(planetInfoTable);
    aspectCheckData.planetIdList = planetIdList;
    aspectCheckData.currentOuterPlanetId = 0;
    aspectCheckData.planetInfoTable = planetInfoTable;
    aspectCheckData.planetDataTable = planetDataTable;
    g_list_foreach(planetIdList, check_aspects_outer_loop, &aspectCheckData);

    printf("\nANTISCIA\n========\n\n");

    aspectCheckData.planetIdList = planetIdList;
    aspectCheckData.currentOuterPlanetId = 0;
    aspectCheckData.planetInfoTable = planetInfoTable;
    aspectCheckData.planetDataTable = planetDataTable;
    g_list_foreach(planetIdList, check_mirrorpoints_outer_loop, &aspectCheckData);
    g_list_free(planetIdList);

    g_hash_table_unref(planetInfoTable);
    g_hash_table_unref(planetDataTable);

    return OK;
}

