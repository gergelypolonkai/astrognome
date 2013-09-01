#include <math.h>
#include "gswe-moment.h"

#define GSWE_MOMENT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GSWE_TYPE_MOMENT, GsweMomentPrivate))

struct _GsweMomentPrivate {
    GsweTimestamp *timestamp;
    GsweCoordinates coordinates;
};

enum {
    SIGNAL_CHANGED,
    SIGNAL_LAST
};

enum {
    PROP_0,
    PROP_TIMESTAMP
};

static guint gswe_moment_signals[SIGNAL_LAST] = {0};

static void gswe_moment_dispose(GObject *gobject);
static void gswe_moment_finalize(GObject *gobject);
static void gswe_moment_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void gswe_moment_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);

G_DEFINE_TYPE(GsweMoment, gswe_moment, G_TYPE_OBJECT);

static void
gswe_moment_class_init(GsweMomentClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    g_type_class_add_private(klass, sizeof(GsweMomentPrivate));

    gobject_class->dispose = gswe_moment_dispose;
    gobject_class->finalize = gswe_moment_finalize;
    gobject_class->set_property = gswe_moment_set_property;
    gobject_class->get_property = gswe_moment_get_property;

    /**
     * GsweMoment::changed:
     * @moment: the GsweMoment object that received the signal
     *
     * The ::changed signal is emitted each time the time or coordinates are changed
     */
    gswe_moment_signals[SIGNAL_CHANGED] = g_signal_new("changed", G_OBJECT_CLASS_TYPE(gobject_class), G_SIGNAL_RUN_FIRST, G_STRUCT_OFFSET(GsweMomentClass, changed), NULL, NULL, g_cclosure_marshal_generic, G_TYPE_NONE, 0);

    /**
     * GsweMoment:timestamp:
     *
     * The timestamp associated with this moment
     */
    g_object_class_install_property(gobject_class, PROP_TIMESTAMP, g_param_spec_object("timestamp", "Timestamp", "Timestamp of this moment", GSWE_TYPE_TIMESTAMP, G_PARAM_READWRITE));
}

static void
gswe_moment_emit_changed(GsweMoment *moment)
{
    g_signal_emit(moment, gswe_moment_signals[SIGNAL_CHANGED], 0);
}

void
gswe_moment_init(GsweMoment *moment)
{
    moment->priv = GSWE_MOMENT_GET_PRIVATE(moment);

    moment->priv->timestamp = NULL;

    //moment->priv->a_string = g_strdup("Maman");
}

static void
gswe_moment_timestamp_changed(GsweMoment *moment, gpointer data)
{
}

static void
gswe_moment_dispose(GObject *gobject)
{
    GsweMoment *moment = GSWE_MOMENT(gobject);

    g_signal_handlers_disconnect_by_func(moment->priv->timestamp, gswe_moment_timestamp_changed, NULL);

    g_clear_object(&moment->priv->timestamp);

    G_OBJECT_CLASS(gswe_moment_parent_class)->dispose(gobject);
}

static void
gswe_moment_finalize(GObject *gobject)
{
    //GsweMoment *moment = GSWE_MOMENT(gobject);

    //g_free(moment->priv->a_string);

    G_OBJECT_CLASS(gswe_moment_parent_class)->finalize(gobject);
}

static void
gswe_moment_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
    GsweMoment *moment = GSWE_MOMENT(object);

    switch (prop_id) {
        case PROP_TIMESTAMP:
            gswe_moment_set_timestamp(moment, (g_value_get_object(value)));

            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);

            break;
    }
}

static void
gswe_moment_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    GsweMoment *moment = GSWE_MOMENT(object);
    GsweMomentPrivate *priv = moment->priv;

    switch (prop_id) {
        case PROP_TIMESTAMP:
            g_value_set_object(value, priv->timestamp);

            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);

            break;
    }
}

void
gswe_moment_set_timestamp(GsweMoment *moment, GsweTimestamp *timestamp)
{
    if (moment->priv->timestamp != NULL) {
        g_signal_handlers_disconnect_by_func(moment->priv->timestamp, gswe_moment_timestamp_changed, NULL);
        g_clear_object(&moment->priv->timestamp);
    }

    moment->priv->timestamp = timestamp;
    g_object_ref(timestamp);
    g_signal_connect(G_OBJECT(timestamp), "changed", G_CALLBACK(gswe_moment_timestamp_changed), NULL);

    /* Emit the changed signal to notify registrants of the change */
    gswe_moment_emit_changed(moment);
}

/**
 * gswe_moment_get_timestamp:
 * @moment: The GsweMoment object of which you requent its timestamp object
 *
 * Returns: a #GsweTimestamp object assigned to the given moment
 */
GsweTimestamp *
gswe_moment_get_timestamp(GsweMoment *moment)
{
    return moment->priv->timestamp;
}

GQuark
gswe_moment_error_quark(void)
{
    return g_quark_from_static_string("swe-glib-gswe-moment-error");
}

GsweMoment *
gswe_moment_new(void)
{
    return (GsweMoment *)g_object_new(GSWE_TYPE_MOMENT, NULL);
}

