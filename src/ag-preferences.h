/* ag-preferences.h - Preferences dialog for Astrognome
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
#ifndef __AG_PREFERENCES_H__
#define __AG_PREFERENCES_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define AG_TYPE_PREFERENCES         (ag_preferences_get_type())
#define AG_PREFERENCES(o)           (G_TYPE_CHECK_INSTANCE_CAST((o), \
                                                        AG_TYPE_PREFERENCES, \
                                                        AgPreferences))
#define AG_PREFERENCES_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), \
                                                        AG_TYPE_PREFERENCES, \
                                                        AgPreferencesClass))
#define AG_IS_PREFERENCES(o)        (G_TYPE_CHECK_INSTANCE_TYPE((o), \
                                                        AG_TYPE_PREFERENCES))
#define AG_IS_PREFERENCES_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE((k), \
                                                        AG_TYPE_PREFERENCES))
#define AG_PREFERENCES_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS((o), \
                                                        AG_TYPE_PREFERENCES, \
                                                        AgPreferencesClass))

typedef struct _AgPreferences        AgPreferences;
typedef struct _AgPreferencesClass   AgPreferencesClass;

struct _AgPreferences {
    GtkDialog parent;
};

struct _AgPreferencesClass {
    GtkDialogClass parent_class;
};

GType ag_preferences_get_type(void);

void ag_preferences_show_dialog(GtkWindow *parent);

G_END_DECLS

#endif /* __AG_PREFERENCES_H__ */
