#ifndef __AG_APP_H__
#define __AG_APP_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define AG_TYPE_APP         (ag_app_get_type())
#define AG_APP(o)           (G_TYPE_CHECK_INSTANCE_CAST((o), AG_TYPE_APP, AgApp))
#define AG_APP_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), AG_TYPE_APP, AgAppClass))
#define AG_IS_APP(o)        (G_TYPE_CHECK_INSTANCE_TYPE((o), AG_TYPE_APP))
#define AG_IS_APP_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE((k), AG_TYPE_APP))
#define AG_APP_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS((o), AG_TYPE_APP, AgAppClass))

typedef struct _AgApp        AgApp;
typedef struct _AgAppClass   AgAppClass;
typedef struct _AgAppPrivate AgAppPrivate;

struct _AgApp {
    GtkApplication parent_instance;
    AgAppPrivate *priv;
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

G_END_DECLS

#endif /* __AG_APP_H__ */

