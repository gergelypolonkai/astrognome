#ifndef __ASTROGNOME_H__
#define __ASTROGNOME_H__

#include <gtk/gtk.h>
#include <swe-glib.h>

typedef struct {
    gboolean version;
    gboolean quit;
    gboolean new_window;
} AstrognomeOptions;

extern GtkFileFilter    *filter_all;
extern GtkFileFilter    *filter_chart;
extern GtkFileFilter    *filter_hor;
extern GtkFileFilter    *filter_svg;
extern GtkTreeModel     *country_list;
extern GtkTreeModel     *city_list;
extern const GswePlanet used_planets[];
extern gsize            used_planets_count;

enum {
    AG_COUNTRY_CODE,
    AG_COUNTRY_NAME,
    AG_COUNTRY_COLCOUNT
};

enum {
    AG_CITY_COUNTRY,
    AG_CITY_NAME,
    AG_CITY_LAT,
    AG_CITY_LONG,
    AG_CITY_ALT,
    AG_CITY_TZO,
    AG_CITY_TZD,
    AG_CITY_COLCOUNT
};

const gchar *ag_house_system_id_to_nick(GsweHouseSystem house_system);
GsweHouseSystem ag_house_system_nick_to_id(const gchar *nick);

const gchar *ag_planet_id_to_nick(GswePlanet planet);
GswePlanet ag_planet_nick_to_id(const gchar *nick);

const gchar *ag_aspect_id_to_nick(GsweAspect aspect);
GsweAspect ag_aspect_nick_to_id(const gchar *nick);

const gchar *ag_antiscion_axis_id_to_nick(GsweAntiscionAxis antiscion_axis);
GsweAntiscionAxis ag_antiscion_axis_nick_to_id(const gchar *nick);

GFile *ag_get_user_data_dir(void);

#ifndef GDOUBLE_FROM_LE
inline static gdouble
GDOUBLE_SWAP_LE_BE(gdouble in)
{
    union {
        guint64 i;
        gdouble d;
    } u;

    u.d = in;
    u.i = GUINT64_SWAP_LE_BE(u.i);

    return u.d;
}

#if (G_BYTE_ORDER == G_LITTLE_ENDIAN)

#define GDOUBLE_TO_LE(val)   ((gdouble)(val))
#define GDOUBLE_TO_BE(val)   (GDOUBLE_SWAP_LE_BE(val))

#elif (G_BYTE_ORDER == G_BIG_ENDIAN)

#define GDOUBLE_TO_LE(val)   (GDOUBLE_SWAP_LE_BE (val))
#define GDOUBLE_TO_BE(val)   ((gdouble) (val))

#else

#error "unknown endian type"

#endif

#define GDOUBLE_FROM_LE(val) (GDOUBLE_TO_LE (val))
#define GDOUBLE_FROM_BE(val) (GDOUBLE_TO_BE (val))

#endif /* !defined GDOUBLE_FROM_LE */

#endif /* __ASTROGNOME_H__ */

