/* ag-app.h - Application definition for Astrognome
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
#ifndef __AG_APP_H__
#define __AG_APP_H__

#include <gtk/gtk.h>

#include "astrognome.h"

G_BEGIN_DECLS

typedef enum {
    AG_APP_IMPORT_NONE,
    AG_APP_IMPORT_AGC,
    AG_APP_IMPORT_HOR,
} AgAppImportType;

#define AG_TYPE_APP         (ag_app_get_type())
#define AG_APP(o)           (G_TYPE_CHECK_INSTANCE_CAST((o), \
                                                        AG_TYPE_APP, \
                                                        AgApp))
#define AG_APP_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), \
                                                     AG_TYPE_APP, \
                                                     AgAppClass))
#define AG_IS_APP(o)        (G_TYPE_CHECK_INSTANCE_TYPE((o), AG_TYPE_APP))
#define AG_IS_APP_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE((k), AG_TYPE_APP))
#define AG_APP_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS((o), \
                                                       AG_TYPE_APP, \
                                                       AgAppClass))

typedef struct _AgApp        AgApp;
typedef struct _AgAppClass   AgAppClass;

struct _AgApp {
    GtkApplication parent_instance;
};

struct _AgAppClass {
    GtkApplicationClass parent_class;
};

GType ag_app_get_type(void) G_GNUC_CONST;

AgApp *ag_app_new(void);

GtkWindow *ag_app_peek_first_window(AgApp *self);

void ag_app_new_window(AgApp *self);

void ag_app_quit(AgApp *self);

void ag_app_raise(AgApp *self);

void ag_app_run_action(AgApp                   *app,
                       gboolean                is_remote,
                       const AstrognomeOptions *options);

gint ag_app_buttoned_dialog(GtkWindow      *window,
                            GtkMessageType message_type,
                            const gchar    *message,
                            const gchar    *first_button_text, ...);

void ag_app_message_dialog(GtkWindow      *window,
                           GtkMessageType message_type,
                           gchar          *fmt, ...);

G_END_DECLS

#endif /* __AG_APP_H__ */
