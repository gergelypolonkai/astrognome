/* ag-window.h - Main window management for Astrognome
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
#ifndef __AG_WINDOW_H__
#define __AG_WINDOW_H__

#include <gtk/gtk.h>

#include "ag-app.h"
#include "ag-chart.h"

G_BEGIN_DECLS

typedef enum {
    AG_WINDOW_ERROR_EMPTY_CHART,
    AG_WINDOW_ERROR_NO_NAME,
} AgWindowError;

#define AG_TYPE_WINDOW         (ag_window_get_type())
#define AG_WINDOW(o)           (G_TYPE_CHECK_INSTANCE_CAST((o), \
                                                           AG_TYPE_WINDOW, \
                                                           AgWindow))
#define AG_WINDOW_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), \
                                                        AG_TYPE_WINDOW, \
                                                        AgWindowClass))
#define AG_IS_WINDOW(o)        (G_TYPE_CHECK_INSTANCE_TYPE((o), AG_TYPE_WINDOW))
#define AG_IS_WINDOW_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE((k), AG_TYPE_WINDOW))
#define AG_WINDOW_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS((o), \
                                                          AG_TYPE_WINDOW, \
                                                          AgWindowClass))

typedef struct _AgWindow        AgWindow;
typedef struct _AgWindowClass   AgWindowClass;
typedef struct _AgWindowPrivate AgWindowPrivate;

struct _AgWindow {
    GtkApplicationWindow parent_instance;
};

struct _AgWindowClass {
    GtkApplicationWindowClass parent_class;
};

GType ag_window_get_type(void) G_GNUC_CONST;

GtkWidget *ag_window_new(AgApp *app);

void ag_window_set_chart(AgWindow *window,
                         AgChart  *chart);

AgChart *ag_window_get_chart(AgWindow *window);

void ag_window_update_from_chart(AgWindow *window);

void ag_window_settings_restore(GtkWindow *window,
                                GSettings *settings);

void ag_window_settings_save(GtkWindow *window,
                             GSettings *settings);

void ag_window_change_tab(AgWindow *window, const gchar *tab_name);

gboolean ag_window_reload_chart_list(AgWindow *window);

gboolean ag_window_is_usable(AgWindow *window);

#define AG_WINDOW_ERROR (ag_window_error_quark())

GQuark ag_window_error_quark(void);

G_END_DECLS

#endif /* __AG_WINDOW_H__ */

