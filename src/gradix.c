#include <glib.h>

#include "calculate.h"

#include "../swe/src/swephexp.h"

#define EPHEDIR "/home/polesz/Projektek/c/gradix/swe/data"

typedef struct {
    int signId;
    signElement_t element;
    signType_t type;
} signData_t;

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

const char *signTypeName[] = {
    NULL,
    "Cardinal",
    "Fix",
    "Mutable"
};

const char *signElementName[] = {
    NULL,
    "Fire",
    "Earth",
    "Air",
    "Water"
};

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

const char *signName[] = {
    NULL,
    "Aries",
    "Taurus",
    "Gemini",
    "Cancer",
    "Leo",
    "Virgo",
    "Libra",
    "Scorpio",
    "Sagittarius",
    "Capricorn",
    "Aquarius",
    "Pisces"
};

typedef struct {
    gchar *name;
    guint size;
    guint orbModifier;
    gboolean harmonic;
    gboolean major;
} aspectData_t;

/*
 * conjuction - 0, 8
 * sextile    - 60, 4
 * square     - 90, 8
 * trine      - 120, 4
 * opposition - 180, 8
 */
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

#define ADD_SIGN(ht, v, s, e, t) (v) = g_new0(signData_t, 1); \
                                 (v)->signId = (s); \
                                 (v)->element = (e); \
                                 (v)->type = (t); \
                                 g_hash_table_replace((ht), GINT_TO_POINTER(s), (v));

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

#define INCREASE_POINTS(dts, dte, dtt, vsd, s, p, val) (vsd) = g_hash_table_lookup((dts), GINT_TO_POINTER(s)); \
                                                       g_assert((vsd) != NULL); \
                                                       \
                                                       if (((p) = g_hash_table_lookup((dte), GINT_TO_POINTER((vsd)->element))) == NULL) { \
                                                           (p) = g_new0(guint, 1); \
                                                       } \
                                                       \
                                                       *(p) += (val); \
                                                       g_hash_table_replace((dte), GINT_TO_POINTER((vsd)->element), (p)); \
                                                       \
                                                       if (((p) = g_hash_table_lookup((dtt), GINT_TO_POINTER((vsd)->type))) == NULL) { \
                                                           (p) = g_new0(guint, 1); \
                                                       } \
                                                       \
                                                       *(p) += (val); \
                                                       g_hash_table_replace((dtt), GINT_TO_POINTER((vsd)->type), (p));

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

    distance = fabs(outerPlanet->position - innerPlanet->position);
    planetOrb = fmin(outerPlanetData->orb, innerPlanetData->orb);

    if (distance > 180.0) {
        distance = 360.0 - distance;
    }

    for (i = 0; i < sizeof(aspectData) / sizeof(aspectData_t); i++) {
        gdouble diff = fabs(aspectData[i].size - distance);
        gdouble aspectOrb = fmax(1.0, planetOrb - aspectData[i].orbModifier);

        if (diff <= aspectOrb) {
            printf("%f ", diff);
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
free_planet_data(gpointer data)
{
    planetData_t *planetData = data;

    g_free(planetData->name);
    g_free(planetData);
}

int
main(int argc, char *argv[])
{
    int year = 1981,
        month = 3,
        day = 11,
        hour = 23,
        min = 39,
        sec = 34,
        p;
    guint *point;
    double timezone = 1.0,
           lon = 20.766666,
           lat = 48.2,
           alt = 200,
           te,
           cusps[13],
           ascmc[10];
    planetInfo_t *planetInfo;
    moonPhase *phase;
    GHashTable *signDataTable,
               *planetDataTable,
               *elementPointsTable,
               *typePointsTable,
               *planetInfoTable;
    planetData_t *planetData;
    signData_t *signData;
    GList *planetIdList;
    struct aspect_check_data aspectCheckData;

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

    signDataTable = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_free);
    planetDataTable = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, free_planet_data);
    elementPointsTable = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
    typePointsTable = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
    planetInfoTable = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_free);

    // Initialize planet data table

    ADD_PLANET(planetDataTable, planetData, SE_SUN,               "Sun",                 13.0, SIGN_LEO,         SIGN_NONE,     SIGN_AQUARIUS,    SIGN_NONE,   SIGN_ARIES,     SIGN_LIBRA);
    ADD_PLANET(planetDataTable, planetData, SE_MOON,              "Moon",                9.0,  SIGN_CANCER,      SIGN_NONE,     SIGN_CAPRICORN,   SIGN_NONE,   SIGN_TAURUS,    SIGN_SCORPIO);
    ADD_PLANET(planetDataTable, planetData, SE_MERCURY,           "Mercury",             7.0,  SIGN_GEMINI,      SIGN_VIRGO,    SIGN_SAGITTARIUS, SIGN_PISCES, SIGN_VIRGO,     SIGN_PISCES);
    ADD_PLANET(planetDataTable, planetData, SE_VENUS,             "Venus",               7.0,  SIGN_TAURUS,      SIGN_LIBRA,    SIGN_SCORPIO,     SIGN_ARIES,  SIGN_PISCES,    SIGN_VIRGO);
    ADD_PLANET(planetDataTable, planetData, SE_MARS,              "Mars",                7.0,  SIGN_ARIES,       SIGN_SCORPIO,  SIGN_LIBRA,       SIGN_TAURUS, SIGN_CAPRICORN, SIGN_CANCER);
    ADD_PLANET(planetDataTable, planetData, SE_JUPITER,           "Jupiter",             9.0,  SIGN_SAGITTARIUS, SIGN_PISCES,   SIGN_GEMINI,      SIGN_VIRGO,  SIGN_CANCER,    SIGN_CAPRICORN);
    ADD_PLANET(planetDataTable, planetData, SE_SATURN,            "Saturn",              7.0,  SIGN_CAPRICORN,   SIGN_AQUARIUS, SIGN_CANCER,      SIGN_LEO,    SIGN_LIBRA,     SIGN_ARIES);
    ADD_PLANET(planetDataTable, planetData, SE_URANUS,            "Uranus",              5.0,  SIGN_AQUARIUS,    SIGN_NONE,     SIGN_NONE,        SIGN_NONE,   SIGN_NONE,      SIGN_NONE);
    ADD_PLANET(planetDataTable, planetData, SE_NEPTUNE,           "Neptune",             5.0,  SIGN_PISCES,      SIGN_NONE,     SIGN_NONE,        SIGN_NONE,   SIGN_NONE,      SIGN_NONE);
    ADD_PLANET(planetDataTable, planetData, SE_PLUTO,             "Pluto",               3.0,  SIGN_SCORPIO,     SIGN_NONE,     SIGN_NONE,        SIGN_NONE,   SIGN_NONE,      SIGN_NONE);
    ADD_PLANET(planetDataTable, planetData, SE_CHIRON,            "Chiron",              2.0,  SIGN_NONE,        SIGN_NONE,     SIGN_NONE,        SIGN_NONE,   SIGN_NONE,      SIGN_NONE);
    ADD_PLANET(planetDataTable, planetData, SE_MEAN_NODE,         "Ascending Moon Node", 2.0,  SIGN_NONE,        SIGN_NONE,     SIGN_NONE,        SIGN_NONE,   SIGN_NONE,      SIGN_NONE);
    ADD_PLANET(planetDataTable, planetData, SE_MEAN_APOG,         "Dark Moon",           2.0,  SIGN_NONE,        SIGN_NONE,     SIGN_NONE,        SIGN_NONE,   SIGN_NONE,      SIGN_NONE);
    ADD_PLANET(planetDataTable, planetData, SE_CERES,             "Ceres",               2.0,  SIGN_NONE,        SIGN_NONE,     SIGN_NONE,        SIGN_NONE,   SIGN_NONE,      SIGN_NONE);
    ADD_PLANET(planetDataTable, planetData, SE_PALLAS,            "Pallas",              2.0,  SIGN_NONE,        SIGN_NONE,     SIGN_NONE,        SIGN_NONE,   SIGN_NONE,      SIGN_NONE);
    ADD_PLANET(planetDataTable, planetData, SE_JUNO,              "Juno",                2.0,  SIGN_NONE,        SIGN_NONE,     SIGN_NONE,        SIGN_NONE,   SIGN_NONE,      SIGN_NONE);
    ADD_PLANET(planetDataTable, planetData, SE_VESTA,             "Vesta",               2.0,  SIGN_NONE,        SIGN_NONE,     SIGN_NONE,        SIGN_NONE,   SIGN_NONE,      SIGN_NONE);
    ADD_PLANET(planetDataTable, planetData, SE_NPLANETS + SE_ASC, "Ascendent",           9.0,  SIGN_NONE,        SIGN_NONE,     SIGN_NONE,        SIGN_NONE,   SIGN_NONE,      SIGN_NONE);
    ADD_PLANET(planetDataTable, planetData, SE_NPLANETS + SE_MC,  "Midheaven",           5.0,  SIGN_NONE,        SIGN_NONE,     SIGN_NONE,        SIGN_NONE,   SIGN_NONE,      SIGN_NONE);

    // Initialize sign data table

    ADD_SIGN(signDataTable, signData, SIGN_ARIES,       ELEMENT_FIRE,  TYPE_CARDINAL);
    ADD_SIGN(signDataTable, signData, SIGN_TAURUS,      ELEMENT_EARTH, TYPE_FIX);
    ADD_SIGN(signDataTable, signData, SIGN_GEMINI,      ELEMENT_AIR,   TYPE_MUTABLE);
    ADD_SIGN(signDataTable, signData, SIGN_CANCER,      ELEMENT_WATER, TYPE_CARDINAL);
    ADD_SIGN(signDataTable, signData, SIGN_LEO,         ELEMENT_FIRE,  TYPE_FIX);
    ADD_SIGN(signDataTable, signData, SIGN_VIRGO,       ELEMENT_EARTH, TYPE_MUTABLE);
    ADD_SIGN(signDataTable, signData, SIGN_LIBRA,       ELEMENT_AIR,   TYPE_CARDINAL);
    ADD_SIGN(signDataTable, signData, SIGN_SCORPIO,     ELEMENT_WATER, TYPE_FIX);
    ADD_SIGN(signDataTable, signData, SIGN_SAGITTARIUS, ELEMENT_FIRE,  TYPE_MUTABLE);
    ADD_SIGN(signDataTable, signData, SIGN_CAPRICORN,   ELEMENT_EARTH, TYPE_CARDINAL);
    ADD_SIGN(signDataTable, signData, SIGN_AQUARIUS,    ELEMENT_AIR,   TYPE_FIX);
    ADD_SIGN(signDataTable, signData, SIGN_PISCES,      ELEMENT_WATER, TYPE_MUTABLE);

    swe_set_ephe_path(EPHEDIR);

    if (set_location_and_time(lon, lat, alt, year, month, day, hour, min, sec, timezone, &te) == 0) {
        return 1;
    }

    printf("Date: %02d.%02d.%d at %02d:%02d:%02d, at %f, %f\n", year, month, day, hour, min, sec, lon, lat);

    printf("\nHOUSES\n======\n\n");

    swe_houses(te, lat, lon, 'P', cusps, ascmc);

    for (p = 1; p < 13; p++) {
        printf("House %2d..: %s (%f)\n", p, signName[get_sign(cusps[p])], cusps[p]);
    }

    printf("\nPLANETS AND POINTS\n==================\n\n");

    planetInfo = g_new0(planetInfo_t, 1);
    planetInfo->position = ascmc[0];
    planetInfo->sign = get_sign(ascmc[0]);
    planetInfo->house = 1;
    planetInfo->retrograde = FALSE;
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_NPLANETS + SE_ASC), planetInfo);
    printf("Asc.......: %s (%f)\n", signName[planetInfo->sign], planetInfo->position);
    INCREASE_POINTS(signDataTable, elementPointsTable, typePointsTable, signData, planetInfo->sign, point, 2);

    planetInfo = g_new0(planetInfo_t, 1);
    planetInfo->position = ascmc[1];
    planetInfo->sign = get_sign(ascmc[1]);
    planetInfo->house = 10;
    planetInfo->retrograde = FALSE;
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_NPLANETS + SE_MC), planetInfo);
    printf("MC........: %s (%f)\n", signName[planetInfo->sign], planetInfo->position);
    INCREASE_POINTS(signDataTable, elementPointsTable, typePointsTable, signData, planetInfo->sign, point, 1);

    planetInfo = get_planet_info(SE_SUN, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_SUN), planetInfo);
    printf("Sun.......: %s, House: %d (%f%s)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position, (planetInfo->retrograde) ? ", retrograde" : "");
    INCREASE_POINTS(signDataTable, elementPointsTable, typePointsTable, signData, planetInfo->sign, point, 2);

    planetInfo = get_planet_info(SE_MOON, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_MOON), planetInfo);
    phase = get_moon_phase(year, month, day, hour, min, sec);
    printf("Moon......: %s (%.2f%% visibility), %s, House: %d (%f%s)\n", moonStateName[phase->phase], phase->visiblePercentage, signName[planetInfo->sign], planetInfo->house, planetInfo->position, (planetInfo->retrograde) ? ", retrograde" : "");
    INCREASE_POINTS(signDataTable, elementPointsTable, typePointsTable, signData, planetInfo->sign, point, 2);
    g_free(phase);

    planetInfo = get_planet_info(SE_MERCURY, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_MERCURY), planetInfo);
    printf("Mercury...: %s, House: %d (%f%s)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position, (planetInfo->retrograde) ? ", retrograde" : "");
    INCREASE_POINTS(signDataTable, elementPointsTable, typePointsTable, signData, planetInfo->sign, point, 2);

    planetInfo = get_planet_info (SE_VENUS, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_VENUS), planetInfo);
    printf("Venus.....: %s, House: %d (%f%s)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position, (planetInfo->retrograde) ? ", retrograde" : "");
    INCREASE_POINTS(signDataTable, elementPointsTable, typePointsTable, signData, planetInfo->sign, point, 1);

    planetInfo = get_planet_info(SE_MARS, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_MARS), planetInfo);
    printf("Mars......: %s, House: %d (%f%s)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position, (planetInfo->retrograde) ? ", retrograde" : "");
    INCREASE_POINTS(signDataTable, elementPointsTable, typePointsTable, signData, planetInfo->sign, point, 1);

    planetInfo = get_planet_info(SE_JUPITER, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_JUPITER), planetInfo);
    printf("Jupiter...: %s, House: %d (%f%s)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position, (planetInfo->retrograde) ? ", retrograde" : "");
    INCREASE_POINTS(signDataTable, elementPointsTable, typePointsTable, signData, planetInfo->sign, point, 1);

    planetInfo = get_planet_info(SE_SATURN, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_SATURN), planetInfo);
    printf("Saturn....: %s, House: %d (%f%s)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position, (planetInfo->retrograde) ? ", retrograde" : "");
    INCREASE_POINTS(signDataTable, elementPointsTable, typePointsTable, signData, planetInfo->sign, point, 1);

    planetInfo = get_planet_info(SE_URANUS, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_URANUS), planetInfo);
    printf("Uranus....: %s, House: %d (%f%s)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position, (planetInfo->retrograde) ? ", retrograde" : "");
    INCREASE_POINTS(signDataTable, elementPointsTable, typePointsTable, signData, planetInfo->sign, point, 1);

    planetInfo = get_planet_info(SE_NEPTUNE, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_NEPTUNE), planetInfo);
    printf("Neptune...: %s, House: %d (%f%s)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position, (planetInfo->retrograde) ? ", retrograde" : "");
    INCREASE_POINTS(signDataTable, elementPointsTable, typePointsTable, signData, planetInfo->sign, point, 1);

    planetInfo = get_planet_info(SE_PLUTO, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_PLUTO), planetInfo);
    printf("Pluto.....: %s, House: %d (%f%s)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position, (planetInfo->retrograde) ? ", retrograde" : "");
    INCREASE_POINTS(signDataTable, elementPointsTable, typePointsTable, signData, planetInfo->sign, point, 1);

    planetInfo = get_planet_info(SE_CHIRON, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_CHIRON), planetInfo);
    printf("Chiron....: %s, House: %d (%f%s)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position, (planetInfo->retrograde) ? ", retrograde" : "");

    planetInfo = get_planet_info(SE_MEAN_NODE, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_MEAN_NODE), planetInfo);
    printf("North Node: %s, House: %d (%f%s)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position, (planetInfo->retrograde) ? ", retrograde" : "");
    INCREASE_POINTS(signDataTable, elementPointsTable, typePointsTable, signData, planetInfo->sign, point, 1);

    planetInfo = get_planet_info(SE_MEAN_APOG, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_MEAN_APOG), planetInfo);
    printf("Dark Moon.: %s, House: %d (%f%s)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position, (planetInfo->retrograde) ? ", retrograde" : "");

    planetInfo = get_planet_info(SE_CERES, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_CERES), planetInfo);
    printf("Ceres.....: %s, House: %d (%f%s)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position, (planetInfo->retrograde) ? ", retrograde" : "");

    planetInfo = get_planet_info(SE_PALLAS, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_PALLAS), planetInfo);
    printf("Pallas....: %s, House: %d (%f%s)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position, (planetInfo->retrograde) ? ", retrograde" : "");

    planetInfo = get_planet_info(SE_JUNO, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_JUNO), planetInfo);
    printf("Juno......: %s, House: %d (%f%s)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position, (planetInfo->retrograde) ? ", retrograde" : "");

    planetInfo = get_planet_info(SE_VESTA, te, cusps);
    g_hash_table_replace(planetInfoTable, GINT_TO_POINTER(SE_VESTA), planetInfo);
    printf("Vesta.....: %s, House: %d (%f%s)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position, (planetInfo->retrograde) ? ", retrograde" : "");

    printf("\nELEMENTS\n========\n\n");

    point = g_hash_table_lookup(elementPointsTable, GINT_TO_POINTER(ELEMENT_FIRE));
    printf("Fire.: %d\n", (point == NULL) ? 0 : *point);
    point = g_hash_table_lookup(elementPointsTable, GINT_TO_POINTER(ELEMENT_EARTH));
    printf("Earth: %d\n", (point == NULL) ? 0 : *point);
    point = g_hash_table_lookup(elementPointsTable, GINT_TO_POINTER(ELEMENT_AIR));
    printf("Air..: %d\n", (point == NULL) ? 0 : *point);
    point = g_hash_table_lookup(elementPointsTable, GINT_TO_POINTER(ELEMENT_WATER));
    printf("Water: %d\n", (point == NULL) ? 0 : *point);

    printf("\nTYPES\n=====\n\n");

    point = g_hash_table_lookup(typePointsTable, GINT_TO_POINTER(TYPE_CARDINAL));
    printf("Cardinal: %d\n", (point == NULL) ? 0 : *point);
    point = g_hash_table_lookup(typePointsTable, GINT_TO_POINTER(TYPE_FIX));
    printf("Fix.....: %d\n", (point == NULL) ? 0 : *point);
    point = g_hash_table_lookup(typePointsTable, GINT_TO_POINTER(TYPE_MUTABLE));
    printf("Mutable.: %d\n", (point == NULL) ? 0 : *point);

    printf("\nASPECTS\n=======\n\n");

    planetIdList = g_hash_table_get_keys(planetInfoTable);
    aspectCheckData.planetIdList = planetIdList;
    aspectCheckData.currentOuterPlanetId = 0;
    aspectCheckData.planetInfoTable = planetInfoTable;
    aspectCheckData.planetDataTable = planetDataTable;
    g_list_foreach(planetIdList, check_aspects_outer_loop, &aspectCheckData);
    g_list_free(planetIdList);

    g_hash_table_unref(planetInfoTable);
    g_hash_table_unref(typePointsTable);
    g_hash_table_unref(elementPointsTable);
    g_hash_table_unref(planetDataTable);
    g_hash_table_unref(signDataTable);

    return OK;
}

