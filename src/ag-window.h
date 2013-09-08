#ifndef __AG_WINDOW_H__
#define __AG_WINDOW_H__

#include <gtk/gtk.h>
#include "ag-app.h"

G_BEGIN_DECLS

#define AG_TYPE_WINDOW         (ag_window_get_type())
#define AG_WINDOW(o)           (G_TYPE_CHECK_INSTANCE_CAST((o), AG_TYPE_WINDOW, AgWindow))
#define AG_WINDOW_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), AG_TYPE_WINDOW, AgWindowClass))
#define AG_IS_WINDOW(o)        (G_TYPE_CHECK_INSTANCE_TYPE((o), AG_TYPE_WINDOW))
#define AG_IS_WINDOW_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE((k), AG_TYPE_WINDOW))
#define AG_WINDOW_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS((o), AG_TYPE_WINDOW, AgWindowClass))

typedef struct _AgWindow AgWindow;
typedef struct _AgWindowClass AgWindowClass;
typedef struct _AgWindowPrivate AgWindowPrivate;

struct _AgWindow {
    GtkApplicationWindow parent_instance;
    AgWindowPrivate *priv;
};

struct _AgWindowClass {
    GtkApplicationWindowClass parent_class;
};

GType ag_window_get_type(void) G_GNUC_CONST;
GtkWidget *ag_window_new(AgApp *app);

G_END_DECLS

#endif /* __AG_WINDOW_H__ */
