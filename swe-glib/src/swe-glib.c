#include <glib.h>
#define GETTEXT_PACKAGE "swe-glib"
#include <glib/gi18n-lib.h>

#include "../../swe/src/swephexp.h"
#include "swe-glib.h"

gboolean gswe_initialized = FALSE;
gchar *gswe_ephe_path = NULL;
GHashTable *gswe_planet_info_table;
GHashTable *gswe_sign_info_table;
GHashTable *gswe_house_system_info_table;

#define ADD_PLANET(ht, v, i, s, r, n, o, h, dom1, dom2, exi1, exi2, exa, fal) (v) = g_new0(GswePlanetInfo, 1); \
                                                                              (v)->planet = (i); \
                                                                              (v)->sweph_id = (s); \
                                                                              (v)->real_body = (r); \
                                                                              (v)->orb = (o); \
                                                                              (v)->name = g_strdup(n); \
                                                                              (v)->points = (h); \
                                                                              (v)->domicile_sign_1 = (dom1); \
                                                                              (v)->domicile_sign_2 = (dom2); \
                                                                              (v)->exile_sign_1 = (exi1); \
                                                                              (v)->exile_sign_2 = (exi2); \
                                                                              (v)->exalted_sign = (exa); \
                                                                              (v)->fall_sign = (fal); \
                                                                              g_hash_table_replace((ht), GINT_TO_POINTER(i), (v));

#define ADD_SIGN(ht, v, s, n, e, q) (v) = g_new0(GsweSignInfo, 1); \
                                    (v)->sign_id = (s); \
                                    (v)->name = g_strdup(n); \
                                    (v)->element = (e); \
                                    (v)->quality = (q); \
                                    g_hash_table_replace((ht), GINT_TO_POINTER(s), (v));

#define ADD_HOUSE_SYSTEM(ht, v, i, s, n) (v) = g_new0(GsweHouseSystemInfo, 1); \
                                         (v)->system = i; \
                                         (v)->sweph_id = s; \
                                         (v)->name = g_strdup(n); \
                                         g_hash_table_replace((ht), GINT_TO_POINTER(i), (v));

void
gswe_free_planet_info(gpointer planet_info)
{
    g_free(((GswePlanetInfo *)planet_info)->name);
    g_free(planet_info);
}

void
gswe_free_sign_info(gpointer sign_info)
{
    g_free(((GsweSignInfo *)sign_info)->name);
    g_free(sign_info);
}

void
gswe_free_house_system_info(gpointer house_system_info)
{
    g_free(((GsweHouseSystemInfo *)house_system_info)->name);
    g_free(house_system_info);
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
    GsweSignInfo *sign_info;
    GsweHouseSystemInfo *house_system_info;

    bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");

    gswe_planet_info_table = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, gswe_free_planet_info);

    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_SUN,         SE_SUN,       TRUE,  _("Sun"),                 13.0, 2, GSWE_SIGN_LEO,         GSWE_SIGN_NONE,     GSWE_SIGN_AQUARIUS,    GSWE_SIGN_NONE,   GSWE_SIGN_ARIES,     GSWE_SIGN_LIBRA);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_MOON,        SE_MOON,      TRUE,  _("Moon"),                9.0,  2, GSWE_SIGN_CANCER,      GSWE_SIGN_NONE,     GSWE_SIGN_CAPRICORN,   GSWE_SIGN_NONE,   GSWE_SIGN_TAURUS,    GSWE_SIGN_SCORPIO);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_MERCURY,     SE_MERCURY,   TRUE,  _("Mercury"),             7.0,  2, GSWE_SIGN_GEMINI,      GSWE_SIGN_VIRGO,    GSWE_SIGN_SAGITTARIUS, GSWE_SIGN_PISCES, GSWE_SIGN_VIRGO,     GSWE_SIGN_PISCES);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_VENUS,       SE_VENUS,     TRUE,  _("Venus"),               7.0,  1, GSWE_SIGN_TAURUS,      GSWE_SIGN_LIBRA,    GSWE_SIGN_SCORPIO,     GSWE_SIGN_ARIES,  GSWE_SIGN_PISCES,    GSWE_SIGN_VIRGO);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_MARS,        SE_MARS,      TRUE,  _("Mars"),                7.0,  1, GSWE_SIGN_ARIES,       GSWE_SIGN_SCORPIO,  GSWE_SIGN_LIBRA,       GSWE_SIGN_TAURUS, GSWE_SIGN_CAPRICORN, GSWE_SIGN_CANCER);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_JUPITER,     SE_JUPITER,   TRUE,  _("Jupiter"),             9.0,  1, GSWE_SIGN_SAGITTARIUS, GSWE_SIGN_PISCES,   GSWE_SIGN_GEMINI,      GSWE_SIGN_VIRGO,  GSWE_SIGN_CANCER,    GSWE_SIGN_CAPRICORN);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_SATURN,      SE_SATURN,    TRUE,  _("Saturn"),              7.0,  1, GSWE_SIGN_CAPRICORN,   GSWE_SIGN_AQUARIUS, GSWE_SIGN_CANCER,      GSWE_SIGN_LEO,    GSWE_SIGN_LIBRA,     GSWE_SIGN_ARIES);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_URANUS,      SE_URANUS,    TRUE,  _("Uranus"),              5.0,  1, GSWE_SIGN_AQUARIUS,    GSWE_SIGN_NONE,     GSWE_SIGN_NONE,        GSWE_SIGN_NONE,   GSWE_SIGN_NONE,      GSWE_SIGN_NONE);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_NEPTUNE,     SE_NEPTUNE,   TRUE,  _("Neptune"),             5.0,  1, GSWE_SIGN_PISCES,      GSWE_SIGN_NONE,     GSWE_SIGN_NONE,        GSWE_SIGN_NONE,   GSWE_SIGN_NONE,      GSWE_SIGN_NONE);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_PLUTO,       SE_PLUTO,     TRUE,  _("Pluto"),               3.0,  1, GSWE_SIGN_SCORPIO,     GSWE_SIGN_NONE,     GSWE_SIGN_NONE,        GSWE_SIGN_NONE,   GSWE_SIGN_NONE,      GSWE_SIGN_NONE);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_CHIRON,      SE_CHIRON,    TRUE,  _("Chiron"),              2.0,  0, GSWE_SIGN_NONE,        GSWE_SIGN_NONE,     GSWE_SIGN_NONE,        GSWE_SIGN_NONE,   GSWE_SIGN_NONE,      GSWE_SIGN_NONE);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_CERES,       SE_CERES,     TRUE,  _("Ceres"),               2.0,  0, GSWE_SIGN_NONE,        GSWE_SIGN_NONE,     GSWE_SIGN_NONE,        GSWE_SIGN_NONE,   GSWE_SIGN_NONE,      GSWE_SIGN_NONE);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_PALLAS,      SE_PALLAS,    TRUE,  _("Pallas"),              2.0,  0, GSWE_SIGN_NONE,        GSWE_SIGN_NONE,     GSWE_SIGN_NONE,        GSWE_SIGN_NONE,   GSWE_SIGN_NONE,      GSWE_SIGN_NONE);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_JUNO,        SE_JUNO,      TRUE,  _("Juno"),                2.0,  0, GSWE_SIGN_NONE,        GSWE_SIGN_NONE,     GSWE_SIGN_NONE,        GSWE_SIGN_NONE,   GSWE_SIGN_NONE,      GSWE_SIGN_NONE);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_VESTA,       SE_VESTA,     TRUE,  _("Vesta"),               2.0,  0, GSWE_SIGN_NONE,        GSWE_SIGN_NONE,     GSWE_SIGN_NONE,        GSWE_SIGN_NONE,   GSWE_SIGN_NONE,      GSWE_SIGN_NONE);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_MOON_NODE,   SE_MEAN_NODE, TRUE,  _("Ascending Moon Node"), 2.0,  1, GSWE_SIGN_NONE,        GSWE_SIGN_NONE,     GSWE_SIGN_NONE,        GSWE_SIGN_NONE,   GSWE_SIGN_NONE,      GSWE_SIGN_NONE);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_MOON_APOGEE, SE_MEAN_APOG, TRUE,  _("Dark Moon"),           2.0,  0, GSWE_SIGN_NONE,        GSWE_SIGN_NONE,     GSWE_SIGN_NONE,        GSWE_SIGN_NONE,   GSWE_SIGN_NONE,      GSWE_SIGN_NONE);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_ASCENDENT,   -1,           FALSE, _("Ascendent"),           9.0,  2, GSWE_SIGN_NONE,        GSWE_SIGN_NONE,     GSWE_SIGN_NONE,        GSWE_SIGN_NONE,   GSWE_SIGN_NONE,      GSWE_SIGN_NONE);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_MC,          -1,           FALSE, _("Midheaven"),           5.0,  1, GSWE_SIGN_NONE,        GSWE_SIGN_NONE,     GSWE_SIGN_NONE,        GSWE_SIGN_NONE,   GSWE_SIGN_NONE,      GSWE_SIGN_NONE);
    ADD_PLANET(gswe_planet_info_table, planet_info, GSWE_PLANET_VERTEX,      -1,           FALSE, _("Vertex"),              2.0,  0, GSWE_SIGN_NONE,        GSWE_SIGN_NONE,     GSWE_SIGN_NONE,        GSWE_SIGN_NONE,   GSWE_SIGN_NONE,      GSWE_SIGN_NONE);

    gswe_sign_info_table = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, gswe_free_sign_info);

    ADD_SIGN(gswe_sign_info_table, sign_info, GSWE_SIGN_ARIES,       _("Aries"),       GSWE_ELEMENT_FIRE, GSWE_QUALITY_CARDINAL);
    ADD_SIGN(gswe_sign_info_table, sign_info, GSWE_SIGN_TAURUS,      _("Taurus"),      GSWE_ELEMENT_FIRE, GSWE_QUALITY_CARDINAL);
    ADD_SIGN(gswe_sign_info_table, sign_info, GSWE_SIGN_GEMINI,      _("Gemini"),      GSWE_ELEMENT_FIRE, GSWE_QUALITY_CARDINAL);
    ADD_SIGN(gswe_sign_info_table, sign_info, GSWE_SIGN_CANCER,      _("Cancer"),      GSWE_ELEMENT_FIRE, GSWE_QUALITY_CARDINAL);
    ADD_SIGN(gswe_sign_info_table, sign_info, GSWE_SIGN_LEO,         _("Leo"),         GSWE_ELEMENT_FIRE, GSWE_QUALITY_CARDINAL);
    ADD_SIGN(gswe_sign_info_table, sign_info, GSWE_SIGN_VIRGO,       _("Virgo"),       GSWE_ELEMENT_FIRE, GSWE_QUALITY_CARDINAL);
    ADD_SIGN(gswe_sign_info_table, sign_info, GSWE_SIGN_LIBRA,       _("Libra"),       GSWE_ELEMENT_FIRE, GSWE_QUALITY_CARDINAL);
    ADD_SIGN(gswe_sign_info_table, sign_info, GSWE_SIGN_SCORPIO,     _("Scorpio"),     GSWE_ELEMENT_FIRE, GSWE_QUALITY_CARDINAL);
    ADD_SIGN(gswe_sign_info_table, sign_info, GSWE_SIGN_SAGITTARIUS, _("Sagittarius"), GSWE_ELEMENT_FIRE, GSWE_QUALITY_CARDINAL);
    ADD_SIGN(gswe_sign_info_table, sign_info, GSWE_SIGN_CAPRICORN,   _("Capricorn"),   GSWE_ELEMENT_FIRE, GSWE_QUALITY_CARDINAL);
    ADD_SIGN(gswe_sign_info_table, sign_info, GSWE_SIGN_AQUARIUS,    _("Aquarius"),    GSWE_ELEMENT_FIRE, GSWE_QUALITY_CARDINAL);
    ADD_SIGN(gswe_sign_info_table, sign_info, GSWE_SIGN_PISCES,      _("Pisces"),      GSWE_ELEMENT_FIRE, GSWE_QUALITY_CARDINAL);

    gswe_house_system_info_table = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, gswe_free_house_system_info);

    ADD_HOUSE_SYSTEM(gswe_house_system_info_table, house_system_info, GSWE_HOUSE_SYSTEM_NONE,     0,   _("None"));
    ADD_HOUSE_SYSTEM(gswe_house_system_info_table, house_system_info, GSWE_HOUSE_SYSTEM_PLACIDUS, 'P', _("Placidus"));
    ADD_HOUSE_SYSTEM(gswe_house_system_info_table, house_system_info, GSWE_HOUSE_SYSTEM_KOCH,     'K', _("Koch"));
    ADD_HOUSE_SYSTEM(gswe_house_system_info_table, house_system_info, GSWE_HOUSE_SISTEM_EQUAL,    'E', _("Equal"));

    gswe_ephe_path = g_strdup(sweph_path);
    swe_set_ephe_path(sweph_path);
    gswe_initialized = TRUE;
}

