#ifndef __ASTROGNOME_H__
#define __ASTROGNOME_H__

#include <swe-glib.h>

typedef struct {
    gboolean version;
    gboolean quit;
    gboolean new_window;
} AstrognomeOptions;

extern GtkFileFilter *filter_all;
extern GtkFileFilter *filter_chart;

const gchar *ag_house_system_id_to_nick(GsweHouseSystem house_system);
GsweHouseSystem ag_house_system_nick_to_id(const gchar *nick);

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

