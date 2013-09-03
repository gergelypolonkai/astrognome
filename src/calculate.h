#include "../swe/src/swephexp.h"

typedef enum {
    SIGN_NONE,
    SIGN_ARIES,
    SIGN_TAURUS,
    SIGN_GEMINI,
    SIGN_CANCER,
    SIGN_LEO,
    SIGN_VIRGO,
    SIGN_LIBRA,
    SIGN_SCORPIO,
    SIGN_SAGITTARIUS,
    SIGN_CAPRICORN,
    SIGN_AQUARIUS,
    SIGN_PISCES
} zodiacSign;

typedef enum {
    TYPE_CARDINAL = 1,
    TYPE_FIX,
    TYPE_MUTABLE
} signType_t;

typedef enum {
    ELEMENT_FIRE = 1,
    ELEMENT_EARTH,
    ELEMENT_AIR,
    ELEMENT_WATER
} signElement_t;

typedef struct {
    signType_t type;
    signElement_t element;
    int rulingPlanet;
    int domicilePlanet;
    int detrimentPlanet;
    int exaltedPlanet;
    int fallingPlanet;
} signTypePair_t;

typedef struct {
    double position;
    zodiacSign sign;
    int house;
    short int retrograde;
    signType_t type;
    signElement_t element;
} planetInfo_t;

guint get_house(gdouble position, gdouble cusps[]);
planetInfo_t *get_planet_info(int32 planetNo, double date, double cusps[]);
int set_location_and_time(double lon, double lat, double alt, int year, int month, int day, int hour, int min, double sec, double d_timezone, double *jd);
