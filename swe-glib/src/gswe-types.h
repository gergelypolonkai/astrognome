#ifndef __SWE_GLIB_GSWE_PLANETS_H__
#define __SWE_GLIB_GSWE_PLANETS_H__

typedef enum {
    GSWE_PLANET_NONE,
    GSWE_PLANET_SUN,
    GSWE_PLANET_MOON,
    GSWE_PLANET_MERCURY,
    GSWE_PLANET_VENUS,
    GSWE_PLANET_EARTH,
    GSWE_PLANET_MARS,
    GSWE_PLANET_JUPITER,
    GSWE_PLANET_SATURN,
    GSWE_PLANET_URANUS,
    GSWE_PLANET_NEPTUNE,
    GSWE_PLANET_PLUTO,
    GSWE_PLANET_CHIRON,
    GSWE_PLANET_CERES,
    GSWE_PLANET_PALLAS,
    GSWE_PLANET_JUNO,
    GSWE_PLANET_VESTA,
    GSWE_PLANET_MOON_NODE,
    GSWE_PLANET_MOON_APOGEE,
    GSWE_PLANET_ASCENDENT,
    GSWE_PLANET_MC,
    GSWE_PLANET_VERTEX
} GswePlanet;

typedef enum {
    GSWE_SIGN_NONE,
    GSWE_SIGN_ARIES,
    GSWE_SIGN_TAURUS,
    GSWE_SIGN_GEMINI,
    GSWE_SIGN_CANCER,
    GSWE_SIGN_LEO,
    GSWE_SIGN_VIRGO,
    GSWE_SIGN_LIBRA,
    GSWE_SIGN_SCORPIO,
    GSWE_SIGN_SAGITTARIUS,
    GSWE_SIGN_CAPRICORN,
    GSWE_SIGN_AQUARIUS,
    GSWE_SIGN_PISCES
} GsweZodiac;

typedef enum {
    GSWE_ELEMENT_NONE,
    GSWE_ELEMENT_FIRE,
    GSWE_ELEMENT_EARTH,
    GSWE_ELEMENT_AIR,
    GSWE_ELEMENT_WATER
} GsweElement;

typedef enum {
    GSWE_QUALITY_NONE,
    GSWE_QUALITY_CARDINAL,
    GSWE_QUALITY_FIX,
    GSWE_QUALITY_MUTABLE
} GsweQuality;

/**
 * GswePlanetInfo:
 * @planet:          the planet ID
 * @orb:             the planet's “personal” orb
 * @name:            the planet's name
 * @domicile_sign_1: the first sign in which the planet is domicile
 * @domicile_sign_2: the second sign in which the planet is domicile
 * @exile_sign_1:    the first sign in which the planet is in exile
 * @exile_sign_2:    the second sign in which the planet is in exile
 * @exalted_sign:    the sign in which the planet is exalted
 * @fall_sign:       the sign in which the planet is in fall
 */
typedef struct {
    GswePlanet planet;
    gdouble orb;
    gchar *name;
    GsweZodiac domicile_sign_1;
    GsweZodiac domicile_sign_2;
    GsweZodiac exile_sign_1;
    GsweZodiac exile_sign_2;
    GsweZodiac exalted_sign;
    GsweZodiac fall_sign;
} GswePlanetInfo;

#endif /* __SWE_GLIB_GSWE_PLANETS_H__ */

