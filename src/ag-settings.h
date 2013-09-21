#ifndef __AG_SETTINGS_H__
#define __AG_SETTINGS_H__

#include <gio/gio.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define AG_TYPE_SETTINGS         (ag_settings_get_type())
#define AG_SETTINGS(o)           (G_TYPE_CHECK_INSTANCE_CAST((o), AG_TYPE_SETTINGS, AgSettings))
#define AG_SETTINGS_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), AG_TYPE_SETTINGS, AgSettingsClass))
#define AG_IS_SETTINGS(o)        (G_TYPE_CHECK_INSTANCE_TYPE((o), AG_TYPE_SETTINGS))
#define AG_IS_SETTINGS_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE((k), AG_TYPE_SETTINGS))
#define AG_SETTINGS_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS((o), AG_TYPE_SETTINGS, AgSettingsClass))

typedef struct _AgSettings AgSettings;
typedef struct _AgSettingsClass AgSettingsClass;
typedef struct _AgSettingsPrivate AgSettingsPrivate;

struct _AgSettings {
    GObject parent_instance;

    /*< private >*/
    AgSettingsPrivate *priv;
};

struct _AgSettingsClass {
    GObjectClass parent_class;
};

GType ag_settings_get_type(void);

AgSettings *ag_settings_get(void);

GSettings *ag_settings_peek_window_settings(AgSettings *settings);
GSettings *ag_settings_peek_chart_settings(AgSettings *settings);

G_END_DECLS

#endif /* __AG_SETTINGS_H__ */

