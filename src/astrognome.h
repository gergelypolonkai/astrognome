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

#endif /* __ASTROGNOME_H__ */

