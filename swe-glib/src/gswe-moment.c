#include <math.h>
#include "gswe-moment.h"

#define GSWE_MOMENT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GSWE_TYPE_MOMENT, GsweMomentPrivate))

struct _GsweMomentPrivate {
    GDateTime *timestamp;
    gdouble julian_timestamp;
    GsweCoordinates coordinates;
};

enum {
    SIGNAL_MOMENT_CHANGED,
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
     * GsweMoment::moment-changed:
     * @moment: the GsweMoment object that received the signal
     *
     * The ::moment-changed signal is emitted each time the time or coordinates are changed
     */
    gswe_moment_signals[SIGNAL_MOMENT_CHANGED] = g_signal_new("moment-changed", G_OBJECT_CLASS_TYPE(gobject_class), G_SIGNAL_RUN_FIRST, G_STRUCT_OFFSET(GsweMomentClass, moment_changed), NULL, NULL, g_cclosure_marshal_generic, G_TYPE_NONE, 0);

    /**
     * GsweMoment:timestamp:
     *
     * The timestamp associated with this moment
     */
    g_object_class_install_property(gobject_class, PROP_TIMESTAMP, g_param_spec_pointer("timestamp", "Timestamp", "Timestamp of this moment", G_PARAM_READWRITE));
}

static void
gswe_moment_emit_moment_changed(GsweMoment *moment)
{
    g_signal_emit(moment, gswe_moment_signals[SIGNAL_MOMENT_CHANGED], 0);
}

void
gswe_moment_init(GsweMoment *self)
{
    self->priv = GSWE_MOMENT_GET_PRIVATE(self);

    //self->priv->an_object = g_object_new(MAMAN_TYPE_BAZ, NULL);
    //self->priv->a_string = g_strdup("Maman");
}

static void
gswe_moment_dispose(GObject *gobject)
{
    //GsweMoment *self = GSWE_MOMENT(gobject);

    //g_clear_object(&self->priv->an_object);

    G_OBJECT_CLASS(gswe_moment_parent_class)->dispose(gobject);
}

static void
gswe_moment_finalize(GObject *gobject)
{
    //GsweMoment *self = GSWE_MOMENT(gobject);

    //g_free(self->priv->a_string);

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
    GsweMomentPrivate *priv = moment->priv;

    /* Emit the moment-changed signal to notify registrants of the change */
    priv->timestamp = timestamp;
    gswe_moment_emit_moment_changed(moment);
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

