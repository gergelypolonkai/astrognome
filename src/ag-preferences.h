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
