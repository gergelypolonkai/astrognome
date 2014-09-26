/* ag-display-theme.h - Display theme management for Astrognome
 *
 * Copyright (C) 2014 Polonkai Gergely
 *
 * Astrognome is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 3 of the License,
 * or (at your option) any later version.
 *
 * Astrognome is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; if not, see
 * <http://www.gnu.org/licenses/>.
 */
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
