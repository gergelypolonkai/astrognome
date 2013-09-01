#include <glib.h>
#define GETTEXT_PACKAGE "swe-glib"
#include <glib/gi18n-lib.h>

#include "../../swe/src/swephexp.h"
#include "swe-glib.h"

gboolean gswe_initialized = FALSE;
gchar *gswe_ephe_path = NULL;
GHashTable *gswe_planet_info_table;

#define ADD_PLANET(ht, v, i, s, n, o, dom1, dom2, exi1, exi2, exa, fal) (v) = g_new0(GswePlanetInfo, 1); \
                                                                        (v)->planet = (i); \
                                                                        (v)->sweph_id = (s); \
                                                                        (v)->orb = (o); \
                                                                        (v)->name = g_strdup(n); \
                                                                        (v)->domicile_sign_1 = (dom1); \
                                                                        (v)->domicile_sign_2 = (dom2); \
                                                                        (v)->exile_sign_1 = (exi1); \
                                                                        (v)->exile_sign_2 = (exi2); \
                                                                        (v)->exalted_sign = (exa); \
                                                                        (v)->fall_sign = (fal); \
                                                                        g_hash_table_replace((ht), GINT_TO_POINTER(i), (v));

void
gswe_free_planet_info(gpointer planet_info)
{
    g_free(((GswePlanetInfo *)planet_info)->name);
    g_free(planet_info);
}

/**
 * gswe_init:
 * @sweph_path: the file system path to the Swiss Ephemeris data files
 *
 * Initializes the SWE-GLib library. It must be called before any calculations
 * are made.
 */
void
gswe_init(gchar *sweph_path)
{
    GswePlanetInfo *planet_info;

    bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");

    gswe_planet_info_table = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, gswe_free_planet_info);

    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_SUN,         SE_SUN,       _("Sun"),                 13.0, GSWE_SIGN_LEO,         GSWE_SIGN_NONE,     GSWE_SIGN_AQUARIUS,    GSWE_SIGN_NONE,   GSWE_SIGN_ARIES,     GSWE_SIGN_LIBRA);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_MOON,        SE_MOON,      _("Moon"),                9.0,  GSWE_SIGN_CANCER,      GSWE_SIGN_NONE,     GSWE_SIGN_CAPRICORN,   GSWE_SIGN_NONE,   GSWE_SIGN_TAURUS,    GSWE_SIGN_SCORPIO);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_MERCURY,     SE_MERCURY,   _("Mercury"),             7.0,  GSWE_SIGN_GEMINI,      GSWE_SIGN_VIRGO,    GSWE_SIGN_SAGITTARIUS, GSWE_SIGN_PISCES, GSWE_SIGN_VIRGO,     GSWE_SIGN_PISCES);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_VENUS,       SE_VENUS,     _("Venus"),               7.0,  GSWE_SIGN_TAURUS,      GSWE_SIGN_LIBRA,    GSWE_SIGN_SCORPIO,     GSWE_SIGN_ARIES,  GSWE_SIGN_PISCES,    GSWE_SIGN_VIRGO);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_MARS,        SE_MARS,      _("Mars"),                7.0,  GSWE_SIGN_ARIES,       GSWE_SIGN_SCORPIO,  GSWE_SIGN_LIBRA,       GSWE_SIGN_TAURUS, GSWE_SIGN_CAPRICORN, GSWE_SIGN_CANCER);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_JUPITER,     SE_JUPITER,   _("Jupiter"),             9.0,  GSWE_SIGN_SAGITTARIUS, GSWE_SIGN_PISCES,   GSWE_SIGN_GEMINI,      GSWE_SIGN_VIRGO,  GSWE_SIGN_CANCER,    GSWE_SIGN_CAPRICORN);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_SATURN,      SE_SATURN,    _("Saturn"),              7.0,  GSWE_SIGN_CAPRICORN,   GSWE_SIGN_AQUARIUS, GSWE_SIGN_CANCER,      GSWE_SIGN_LEO,    GSWE_SIGN_LIBRA,     GSWE_SIGN_ARIES);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_URANUS,      SE_URANUS,    _("Uranus"),              5.0,  GSWE_SIGN_AQUARIUS,    GSWE_SIGN_NONE,     GSWE_SIGN_NONE,        GSWE_SIGN_NONE,   GSWE_SIGN_NONE,      GSWE_SIGN_NONE);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_NEPTUNE,     SE_NEPTUNE,   _("Neptune"),             5.0,  GSWE_SIGN_PISCES,      GSWE_SIGN_NONE,     GSWE_SIGN_NONE,        GSWE_SIGN_NONE,   GSWE_SIGN_NONE,      GSWE_SIGN_NONE);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_PLUTO,       SE_PLUTO,     _("Pluto"),               3.0,  GSWE_SIGN_SCORPIO,     GSWE_SIGN_NONE,     GSWE_SIGN_NONE,        GSWE_SIGN_NONE,   GSWE_SIGN_NONE,      GSWE_SIGN_NONE);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_CHIRON,      SE_CHIRON,    _("Chiron"),              2.0,  GSWE_SIGN_NONE,        GSWE_SIGN_NONE,     GSWE_SIGN_NONE,        GSWE_SIGN_NONE,   GSWE_SIGN_NONE,      GSWE_SIGN_NONE);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_CERES,       SE_CERES,     _("Ceres"),               2.0,  GSWE_SIGN_NONE,        GSWE_SIGN_NONE,     GSWE_SIGN_NONE,        GSWE_SIGN_NONE,   GSWE_SIGN_NONE,      GSWE_SIGN_NONE);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_PALLAS,      SE_PALLAS,    _("Pallas"),              2.0,  GSWE_SIGN_NONE,        GSWE_SIGN_NONE,     GSWE_SIGN_NONE,        GSWE_SIGN_NONE,   GSWE_SIGN_NONE,      GSWE_SIGN_NONE);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_JUNO,        SE_JUNO,      _("Juno"),                2.0,  GSWE_SIGN_NONE,        GSWE_SIGN_NONE,     GSWE_SIGN_NONE,        GSWE_SIGN_NONE,   GSWE_SIGN_NONE,      GSWE_SIGN_NONE);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_VESTA,       SE_VESTA,     _("Vesta"),               2.0,  GSWE_SIGN_NONE,        GSWE_SIGN_NONE,     GSWE_SIGN_NONE,        GSWE_SIGN_NONE,   GSWE_SIGN_NONE,      GSWE_SIGN_NONE);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_MOON_NODE,   SE_MEAN_NODE, _("Ascending Moon Node"), 2.0,  GSWE_SIGN_NONE,        GSWE_SIGN_NONE,     GSWE_SIGN_NONE,        GSWE_SIGN_NONE,   GSWE_SIGN_NONE,      GSWE_SIGN_NONE);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_MOON_APOGEE, SE_MEAN_APOG, _("Dark Moon"),           2.0,  GSWE_SIGN_NONE,        GSWE_SIGN_NONE,     GSWE_SIGN_NONE,        GSWE_SIGN_NONE,   GSWE_SIGN_NONE,      GSWE_SIGN_NONE);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_ASCENDENT,   -1,           _("Ascendent"),           9.0,  GSWE_SIGN_NONE,        GSWE_SIGN_NONE,     GSWE_SIGN_NONE,        GSWE_SIGN_NONE,   GSWE_SIGN_NONE,      GSWE_SIGN_NONE);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_MC,          -1,           _("Midheaven"),           5.0,  GSWE_SIGN_NONE,        GSWE_SIGN_NONE,     GSWE_SIGN_NONE,        GSWE_SIGN_NONE,   GSWE_SIGN_NONE,      GSWE_SIGN_NONE);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_VERTEX,      -1,           _("Vertex"),              2.0,  GSWE_SIGN_NONE,        GSWE_SIGN_NONE,     GSWE_SIGN_NONE,        GSWE_SIGN_NONE,   GSWE_SIGN_NONE,      GSWE_SIGN_NONE);

    gswe_ephe_path = g_strdup(sweph_path);
    swe_set_ephe_path(sweph_path);
    gswe_initialized = TRUE;
}

