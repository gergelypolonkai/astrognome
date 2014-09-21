#ifndef __AG_DISPLAY_THEME_H__
#define __AG_DISPLAY_THEME_H__

#include <glib.h>

typedef struct _AgDisplayTheme {
    gint     id;
    gchar    *name;
    gboolean builtin;
    gboolean planets_include;
    GList    *planets;
    gboolean aspects_include;
    GList    *aspects;
    gboolean antiscia_include;
    GList    *antiscia;
} AgDisplayTheme;

enum {
    AG_DISPLAY_THEME_ALL       = -1,
    AG_DISPLAY_THEME_CLASSIC   = -2,
    AG_DISPLAY_THEME_NO_COMETS = -3,
    AG_DISPLAY_THEME_PREV,
    AG_DISPLAY_THEME_COUNT     = - AG_DISPLAY_THEME_PREV + 1
};

gchar *ag_display_theme_to_css(AgDisplayTheme *display_theme);

AgDisplayTheme *ag_display_theme_get_by_id(int id);

GList *ag_display_theme_get_list(void);

void ag_display_theme_free(AgDisplayTheme *display_theme);

#endif /* __AG_DISPLAY_THEME_H__ */
