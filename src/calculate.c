#include <glib.h>

#include "calculate.h"

/**
 * SECTION:calculate
 * @short_description: functions for astronomical calculations
 * @title: Astronomical calculation functions
 * @section_id:
 * @include: calculate.h
 *
 * These functions are for astronomical calculations. They are deprecated by SWE-GLib on the long run
 */

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
    { TYPE_MUTABLE,  ELEMENT_FIRE,  SE_JUPITER, SE_JUPITER, SE_MERCURY, 0,          0          }, // Sagittarius
    { TYPE_CARDINAL, ELEMENT_EARTH, SE_SATURN,  SE_SATURN,  SE_MOON,    SE_MARS,    SE_JUPITER }, // Capricorn
    { TYPE_FIX,      ELEMENT_AIR,   SE_URANUS,  SE_SATURN,  SE_SUN,     0,          0          }, // Aquarius
    { TYPE_MUTABLE,  ELEMENT_WATER, SE_NEPTUNE, SE_JUPITER, SE_MERCURY, SE_VENUS,   SE_MERCURY }, // Pisces
};

/**
 * get_house:
 * @position: the latitude position of the celestial body on the sky
 * @cusps: an array of gdoubles, which contains the position of the house
 * cusps. MUST contain 12 gdouble values!
 *
 * Calculates in which house the given position is.
 *
 * Returns: the number of the house in which the given position is
 */
guint
get_house(gdouble position, gdouble cusps[])
{
    guint i,
          house = 0;

    for (i = 1; i < 13; i++) {
        int j = (i < 12) ? i + 1 : 1;

        if (cusps[j] < cusps[i]) {
            if ((position >= cusps[i]) || (position < cusps[j])) {
                house = i;

                break;
            }
        } else {
            if ((position >= cusps[i]) && (position < cusps[j])) {
                house = i;

                break;
            }
        }
    }

    return house;
}

planetInfo_t *
get_planet_info(int32 planetNo, double date, double cusps[])
{
    int32 iflgret,
          iflag = SEFLG_SPEED | SEFLG_TOPOCTR;
    double x2[6];
    char serr[AS_MAXCH];
    planetInfo_t *ret = g_new0(planetInfo_t, 1);

    iflgret = swe_calc(date, planetNo, iflag, x2, serr);

    if (iflgret < 0) {
        printf("error: %s\n", serr);

        return NULL;
    } else if (iflgret != iflag) {
        printf("warning: iflgret != iflag. %s\n", serr);
    }

    ret->house = get_house(x2[0], cusps);

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

