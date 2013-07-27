#include <glib.h>

#include "calculate.h"

const signTypePair_t signType[] = {
    { 0, 0, 0, 0, 0, 0, 0 },
//    Type           Element        Dominating planet       Detriment planet        Fall planet
//                                              Domicile planet         Exalted planet
    { TYPE_CARDINAL, ELEMENT_FIRE,  SE_MARS,    SE_MARS,    SE_VENUS,   SE_SUN,     SE_SATURN  }, // Aries
    { TYPE_FIX,      ELEMENT_EARTH, SE_VENUS,   SE_VENUS,   SE_MARS,    SE_MOON,    0          }, // Taurus
    { TYPE_MUTABLE,  ELEMENT_AIR,   SE_MERCURY, SE_MERCURY, SE_JUPITER, 0,          0          }, // Gemini
    { TYPE_CARDINAL, ELEMENT_WATER, SE_MOON,    SE_MOON,    SE_SATURN,  SE_JUPITER, SE_MARS    }, // Cancer
    { TYPE_FIX,      ELEMENT_FIRE,  SE_SUN,     SE_SUN,     SE_SATURN,  0,          0          }, // Leo
    { TYPE_MUTABLE,  ELEMENT_EARTH, SE_MERCURY, SE_MERCURY, SE_JUPITER, SE_MERCURY, SE_VENUS   }, // Virgo
    { TYPE_CARDINAL, ELEMENT_AIR,   SE_VENUS,   SE_VENUS,   SE_MARS,    SE_SATURN,  SE_SUN     }, // Libra
    { TYPE_FIX,      ELEMENT_WATER, SE_PLUTO,   SE_MARS,    SE_VENUS,   0,          SE_MOON    }, // Scorpio
    { TYPE_MUTABLE,  ELEMENT_FIRE,  SE_JUPITER, SE_JUPITER, SE_MERCURY, 0,          0          }, // Saggitarius
    { TYPE_CARDINAL, ELEMENT_EARTH, SE_SATURN,  SE_SATURN,  SE_MOON,    SE_MARS,    SE_JUPITER }, // Capricorn
    { TYPE_FIX,      ELEMENT_AIR,   SE_URANUS,  SE_SATURN,  SE_SUN,     0,          0          }, // Aquarius
    { TYPE_MUTABLE,  ELEMENT_WATER, SE_NEPTUNE, SE_JUPITER, SE_MERCURY, SE_VENUS,   SE_MERCURY }, // Pisces
};

planetInfo_t *
get_planet_info(int32 planetNo, double date, double cusps[])
{
    int32 iflgret,
          iflag = SEFLG_SPEED | SEFLG_TOPOCTR;
    double x2[6];
    char serr[AS_MAXCH];
    planetInfo_t *ret = g_new0(planetInfo_t, 1);
    int i;

    iflgret = swe_calc(date, planetNo, iflag, x2, serr);

    if (iflgret < 0) {
        printf("error: %s\n", serr);

        return NULL;
    } else if (iflgret != iflag) {
        printf("warning: iflgret != iflag. %s\n", serr);
    }

    ret->house = 0;

    for (i = 1; i < 13; i++) {
        int j = (i < 12) ? i + 1 : 1;

        if (cusps[j] < cusps[i]) {
            if ((x2[0] >= cusps[i]) || (x2[0] < cusps[j])) {
                ret->house = i;

                break;
            }
        } else {
            if ((x2[0] >= cusps[i]) && (x2[0] < cusps[j])) {
                ret->house = i;

                break;
            }
        }
    }

    ret->position = x2[0];
    ret->sign = (int)ceilf(x2[0] / 30.0);
    ret->retrograde = x2[3] < 0;
    ret->type = signType[ret->sign].type;
    ret->element = signType[ret->sign].element;

    return ret;
}

int
set_location_and_time(double lon, double lat, double alt, int year, int month, int day, int hour, int min, double sec, double d_timezone, double *jd)
{
    int utc_year,
        utc_month,
        utc_day,
        utc_hour,
        utc_min;
    double utc_sec,
           retval,
           dret[2];
    char serr[AS_MAXCH];

    swe_set_topo(lon, lat, alt);
    swe_utc_time_zone(year, month, day, hour, min, sec, d_timezone, &utc_year, &utc_month, &utc_day, &utc_hour, &utc_min, &utc_sec);

    if ((retval = swe_utc_to_jd(utc_year, utc_month, utc_day, utc_hour, utc_min, utc_sec, SE_GREG_CAL, dret, serr)) == ERR) {
        printf("error: %s\n", serr);

        return 0;
    }

    *jd = dret[0];

    return 1;
}

long int
get_sign(double pos)
{
    return (int)ceilf(pos / 30.0);
}

moonPhase *
get_moon_phase(gint year, gint month, gint day, gint hour, gint min, gint sec)
{
    GDateTime *baseDate,
              *gds;
    GTimeSpan diff;
    gdouble phasePercent,
            realPercent;
    moonState state;
    moonPhase *ret;

    baseDate = g_date_time_new_utc(2005, 5, 8, 8, 48, 0);
    // TODO: this should use the time zone used at the birth place
    gds = g_date_time_new_local(year, month, day, 0, 0, 0);
    diff = g_date_time_difference(gds, baseDate) / 1000;

    g_date_time_unref(gds);
    g_date_time_unref(baseDate);

    // The current phase of the moon, between 0 and 100 (both 0 and 100 are new moon, 50 is full moon)
    phasePercent = fmod((diff * 100) / (SYNODIC * MSPERDAY), 100);

    if (phasePercent < 0) {
        phasePercent += 100.0;
    }

    if ((phasePercent < 0) || (phasePercent > 100)) {
        fprintf(stderr, "Error during moon phase calculation!\n");

        return NULL;
    }

    // The real percentage is a number around the illumination percentage of the moon
    realPercent = (50.0 - fabs(phasePercent - 50.0)) * 2;

    // Uuuugly!
    if (phasePercent == 0) {
        state = MOON_STATE_NEW;
    } else if (phasePercent < 25) {
        state = MOON_STATE_WAXING_CRESCENT;
    } else if (phasePercent == 25) {
        state = MOON_STATE_WAXING_HALF;
    } else if (phasePercent < 50) {
        state = MOON_STATE_WAXING_GIBBOUS;
    } else if (phasePercent == 50) {
        state = MOON_STATE_FULL;
    } else if (phasePercent < 75) {
        state = MOON_STATE_WANING_GIBBOUS;
    } else if (phasePercent == 75) {
        state = MOON_STATE_WANING_HALF;
    } else if (phasePercent < 100) {
        state = MOON_STATE_WANING_CRESCENT;
    } else {
        state = MOON_STATE_DARK;
    }

    ret = g_new0(moonPhase, 1);
    ret->phase = state;
    ret->visiblePercentage = realPercent;

    return ret;
}

