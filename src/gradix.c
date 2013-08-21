#include <glib.h>

#include "calculate.h"

#include "../swe/src/swephexp.h"

#define EPHEDIR "/home/polesz/Projektek/c/gradix/swe/data"

typedef struct {
    int signId;
    signElement_t element;
    signType_t type;
} signData_t;

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
    "Saggitarius",
    "Capricorn",
    "Aquarius",
    "Pisces"
};

typedef struct {
	gchar *name;
	guint size;
	gdouble orbis;
	gboolean harmonic;
	gboolean strong;
} aspectData_t;

const aspectData_t aspectData[] = {
	{ "Trigon", 60, 6.0, TRUE, TRUE },
	{ NULL, 0, 0.0, FALSE, FALSE }
};

#define ADD_SIGN(ht, v, s, e, t) (v) = g_new0(signData_t, 1); \
                                 (v)->signId = (s); \
                                 (v)->element = (e); \
                                 (v)->type = (t); \
                                 g_hash_table_replace((ht), GINT_TO_POINTER(s), (v));

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

int
main(int argc, char *argv[])
{
    int year = 1981,
        month = 3,
        day = 11,
        hour = 23,
        min = 39,
        sec = 45,
        p;
    guint *point;
    double timezone = 1.0,
           lon = 19.081599,
           lat = 47.462485,
           alt = 200,
           te,
           cusps[13],
           ascmc[10];
    planetInfo_t *planetInfo;
    moonPhase *phase;
    GHashTable *signDataTable,
               *elementPointsTable,
               *typePointsTable,
               *planetInfoTable;
    signData_t *signData;
    GList *planetIdList;

#if 1
    year = 1983;
    month = 3;
    day = 7;
    hour = 11;
    min = 54;
#endif

    signDataTable = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_free);
    elementPointsTable = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
    typePointsTable = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
    planetInfoTable = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_free);

    // Initialize sign data table

    ADD_SIGN(signDataTable, signData, SIGN_ARIES,       ELEMENT_FIRE,  TYPE_CARDINAL);
    ADD_SIGN(signDataTable, signData, SIGN_TAURUS,      ELEMENT_EARTH, TYPE_FIX);
    ADD_SIGN(signDataTable, signData, SIGN_GEMINI,      ELEMENT_AIR,   TYPE_MUTABLE);
    ADD_SIGN(signDataTable, signData, SIGN_CANCER,      ELEMENT_WATER, TYPE_CARDINAL);
    ADD_SIGN(signDataTable, signData, SIGN_LEO,         ELEMENT_FIRE,  TYPE_FIX);
    ADD_SIGN(signDataTable, signData, SIGN_VIRGO,       ELEMENT_EARTH, TYPE_MUTABLE);
    ADD_SIGN(signDataTable, signData, SIGN_LIBRA,       ELEMENT_AIR,   TYPE_CARDINAL);
    ADD_SIGN(signDataTable, signData, SIGN_SCORPIO,     ELEMENT_WATER, TYPE_FIX);
    ADD_SIGN(signDataTable, signData, SIGN_SAGGITARIUS, ELEMENT_FIRE,  TYPE_MUTABLE);
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
    g_list_free(planetIdList);

    g_hash_table_unref(planetInfoTable);
    g_hash_table_unref(typePointsTable);
    g_hash_table_unref(elementPointsTable);
    g_hash_table_unref(signDataTable);

    return OK;
}

