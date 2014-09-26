/* ag-settings.c - Settings object for Astrognome
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
#include "ag-settings.h"

#define SETTINGS_SCHEMA_ID_MAIN   "eu.polonkai.gergely.Astrognome"
#define SETTINGS_SCHEMA_ID_WINDOW "eu.polonkai.gergely.Astrognome.state.window"

static AgSettings *singleton = NULL;

typedef struct _AgSettingsPrivate {
    GSettings *settings_main;
    GSettings *settings_window;
} AgSettingsPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(AgSettings, ag_settings, G_TYPE_OBJECT);

static void
ag_settings_init(AgSettings *settings)
{
    AgSettingsPrivate *priv = ag_settings_get_instance_private(settings);

    priv->settings_main   = g_settings_new(SETTINGS_SCHEMA_ID_MAIN);
    priv->settings_window = g_settings_new(SETTINGS_SCHEMA_ID_WINDOW);
}

static void
ag_settings_dispose(GObject *object)
{
    AgSettingsPrivate *priv = ag_settings_get_instance_private(
            AG_SETTINGS(object)
        );

    g_clear_object(&priv->settings_window);
    g_clear_object(&priv->settings_main);

    G_OBJECT_CLASS(ag_settings_parent_class)->dispose(object);
}

static void
ag_settings_finalize(GObject *object)
{
    singleton = NULL;

    G_OBJECT_CLASS(ag_settings_parent_class)->finalize(object);
}

static void
ag_settings_class_init(AgSettingsClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose  = ag_settings_dispose;
    gobject_class->finalize = ag_settings_finalize;
}

AgSettings *
ag_settings_get(void)
{
    if (!singleton) {
        singleton = AG_SETTINGS(g_object_new(AG_TYPE_SETTINGS, NULL));
    } else {
        g_object_ref(singleton);
    }

    g_assert(singleton);

    return singleton;
}

GSettings *
ag_settings_peek_main_settings(AgSettings *settings)
{
    AgSettingsPrivate *priv = ag_settings_get_instance_private(settings);

    return priv->settings_main;
}

GSettings *
ag_settings_peek_window_settings(AgSettings *settings)
{
    AgSettingsPrivate *priv = ag_settings_get_instance_private(settings);

    return priv->settings_window;
}
