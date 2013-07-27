#include "../swe/src/swephexp.h"

#define SYNODIC 29.53058867
#define MSPERDAY 86400000

typedef enum {
    SIGN_ARIES = 1,
    SIGN_TAURUS,
    SIGN_GEMINI,
    SIGN_CANCER,
    SIGN_LEO,
    SIGN_VIRGO,
    SIGN_LIBRA,
    SIGN_SCORPIO,
    SIGN_SAGGITARIUS,
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

typedef enum {
    MOON_STATE_NEW,
    MOON_STATE_WAXING_CRESCENT,
    MOON_STATE_WAXING_HALF,
    MOON_STATE_WAXING_GIBBOUS,
    MOON_STATE_FULL,
    MOON_STATE_WANING_GIBBOUS,
    MOON_STATE_WANING_HALF,
    MOON_STATE_WANING_CRESCENT,
    MOON_STATE_DARK
} moonState;

typedef struct {
    moonState phase;
    double visiblePercentage;
} moonPhase;

typedef struct {
    double position;
    zodiacSign sign;
    int house;
    short int retrograde;
    signType_t type;
    signElement_t element;
} planetInfo_t;

planetInfo_t *get_planet_info(int32 planetNo, double date, double cusps[]);
int set_location_and_time(double lon, double lat, double alt, int year, int month, int day, int hour, int min, double sec, double d_timezone, double *jd);
long int get_sign(double pos);
moonPhase *get_moon_phase(gint year, gint month, gint day, gint hour, gint min, gint sec);
