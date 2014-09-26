/* ag-settings.h - Settings object for Astrognome
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
#ifndef __AG_SETTINGS_H__
#define __AG_SETTINGS_H__

#include <gio/gio.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define AG_TYPE_SETTINGS         (ag_settings_get_type())
#define AG_SETTINGS(o)           (G_TYPE_CHECK_INSTANCE_CAST((o), \
                                                             AG_TYPE_SETTINGS, \
                                                             AgSettings))
#define AG_SETTINGS_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), \
                                                          AG_TYPE_SETTINGS, \
                                                          AgSettingsClass))
#define AG_IS_SETTINGS(o)        (G_TYPE_CHECK_INSTANCE_TYPE((o), \
                                                             AG_TYPE_SETTINGS))
#define AG_IS_SETTINGS_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE((k), \
                                                          AG_TYPE_SETTINGS))
#define AG_SETTINGS_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS((o), \
                                                            AG_TYPE_SETTINGS, \
                                                            AgSettingsClass))

typedef struct _AgSettings        AgSettings;
typedef struct _AgSettingsClass   AgSettingsClass;

struct _AgSettings {
    GObject parent_instance;
};

struct _AgSettingsClass {
    GObjectClass parent_class;
};

GType ag_settings_get_type(void);

AgSettings *ag_settings_get(void);

GSettings *ag_settings_peek_main_settings(AgSettings *settings);
GSettings *ag_settings_peek_window_settings(AgSettings *settings);

G_END_DECLS

#endif /* __AG_SETTINGS_H__ */

