#include "ag-header-bar.h"
#include "ag-enumtypes.h"

#define DEFAULT_MODE AG_HEADER_BAR_MODE_LIST

typedef struct {
    GtkWidget       *selection_cancel_button;
    GtkWidget       *left_stack;
    GtkWidget       *right_stack;

    AgHeaderBarMode mode;
} AgHeaderBarPrivate;

enum {
    PROP_0,
    PROP_MODE,
    PROP_COUNT
};

enum {
    SIGNAL_MODE_CHANGED,
    SIGNAL_COUNT
};

static void ag_header_bar_dispose(GObject *gobject);
static void ag_header_bar_finalize(GObject *gobject);

G_DEFINE_TYPE_WITH_PRIVATE(AgHeaderBar, ag_header_bar, GTK_TYPE_HEADER_BAR);

#define GET_PRIV(v, o) AgHeaderBarPrivate *v = ag_header_bar_get_instance_private(o);

static GParamSpec *properties[PROP_COUNT];
static guint signals[SIGNAL_COUNT] = { 0 };

static void
ag_header_bar_selection_mode_cb(GtkButton   *button,
                                AgHeaderBar *header_bar)
{
    GET_PRIV(priv, header_bar);

    /* If we are not in list mode, this transition is invalid */
    if (priv->mode != AG_HEADER_BAR_MODE_LIST) {
        g_warning("Invalid header bar mode transition!");

        return;
    }

    g_signal_emit(
            header_bar,
            signals[SIGNAL_MODE_CHANGED], 0,
            AG_HEADER_BAR_MODE_SELECTION
        );
}

static void
ag_header_bar_selection_cancel_cb(GtkButton   *button,
                                  AgHeaderBar *header_bar)
{
    GET_PRIV(priv, header_bar);

    /* If we are not in selection mode, this transition is invalid */
    if (priv->mode != AG_HEADER_BAR_MODE_SELECTION) {
        g_warning("Invalid header bar mode transition!");

        return;
    }

    g_signal_emit(
            header_bar,
            signals[SIGNAL_MODE_CHANGED], 0,
            AG_HEADER_BAR_MODE_LIST
        );
}

static void
ag_header_bar_back_cb(GtkButton   *button,
                      AgHeaderBar *header_bar)
{
    GET_PRIV(priv, header_bar);

    /* If we are not in chart mode, this transition is invalid */
    if (priv->mode != AG_HEADER_BAR_MODE_CHART) {
        g_warning("Invalid header bar mode transition!");

        return;
    }

    g_signal_emit(
            header_bar,
            signals[SIGNAL_MODE_CHANGED], 0,
            AG_HEADER_BAR_MODE_LIST
        );
}

static void
ag_header_bar_set_selection_mode(AgHeaderBar *header_bar, gboolean state)
{
    GtkStyleContext *style;
    GET_PRIV(priv, header_bar);

    if (state == (priv->mode == AG_HEADER_BAR_MODE_SELECTION)) {
         return;
    }

    style = gtk_widget_get_style_context(GTK_WIDGET(header_bar));

    if (state) {
        // Enabling selection mode
        gtk_stack_set_visible_child_name(
                GTK_STACK(priv->right_stack),
                "selection"
            );
        gtk_widget_hide(priv->left_stack);
        gtk_style_context_add_class(style, "selection-mode");
    } else {
        // Disabling selection mode
        gtk_widget_show(priv->left_stack);
        gtk_style_context_remove_class(style, "selection-mode");
    }

    gtk_header_bar_set_show_close_button(
            GTK_HEADER_BAR(header_bar),
            !state
        );
}

static void
ag_header_bar_set_mode_internal(AgHeaderBar     *header_bar,
                                AgHeaderBarMode mode,
                                gboolean        force)
{
    gboolean        invalid = FALSE;
    AgHeaderBarMode old_mode;
    GET_PRIV(priv, header_bar);

    if (!force && (priv->mode == mode)) {
        return;
    }

    old_mode   = priv->mode;

    switch (mode) {
        case AG_HEADER_BAR_MODE_LIST:
            gtk_stack_set_visible_child_name(
                    GTK_STACK(priv->left_stack),
                    "list"
                );
            gtk_stack_set_visible_child_name(
                    GTK_STACK(priv->right_stack),
                    "list"
                );
            ag_header_bar_set_selection_mode(header_bar, FALSE);

            break;

        case AG_HEADER_BAR_MODE_CHART:
            gtk_stack_set_visible_child_name(
                    GTK_STACK(priv->left_stack),
                    "chart"
                );
            gtk_stack_set_visible_child_name(
                    GTK_STACK(priv->right_stack),
                    "chart"
                );
            ag_header_bar_set_selection_mode(header_bar, FALSE);

            break;

        case AG_HEADER_BAR_MODE_SELECTION:
            ag_header_bar_set_selection_mode(header_bar, TRUE);

            break;

        default:
            invalid = TRUE;
            g_warning("Invalid header bar mode!");

            break;
    }

    if (invalid) {
        priv->mode = old_mode;
    } else {
        priv->mode = mode;
        g_object_notify_by_pspec(G_OBJECT(header_bar), properties[PROP_MODE]);
    }
}

void
ag_header_bar_set_mode(AgHeaderBar     *header_bar,
                       AgHeaderBarMode mode)
{
    ag_header_bar_set_mode_internal(header_bar, mode, FALSE);
}

AgHeaderBarMode
ag_header_bar_get_mode(AgHeaderBar *header_bar)
{
    GET_PRIV(priv, header_bar);

    return priv->mode;
}

static void
ag_header_bar_set_property(GObject      *gobject,
                           guint        prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
    switch (prop_id) {
        case PROP_MODE:
            ag_header_bar_set_mode(
                    AG_HEADER_BAR(gobject),
                    g_value_get_enum(value)
                );

            return;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, pspec);

            return;
    }
}

static void
ag_header_bar_get_property(GObject    *gobject,
                           guint      prop_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
    GET_PRIV(priv, AG_HEADER_BAR(gobject));

    switch (prop_id) {
        case PROP_MODE:
            g_value_set_enum(value, priv->mode);

            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, pspec);

            break;
    }
}

static void
ag_header_bar_dispose(GObject *gobject)
{
    G_OBJECT_CLASS(ag_header_bar_parent_class)->dispose(gobject);
}

static void
ag_header_bar_finalize(GObject *gobject)
{
    g_signal_handlers_destroy(gobject);
    G_OBJECT_CLASS(ag_header_bar_parent_class)->finalize(gobject);
}

static void
ag_header_bar_class_init(AgHeaderBarClass *klass)
{
    GObjectClass   *gobject_class = (GObjectClass *)klass;
    GtkWidgetClass *widget_class  = GTK_WIDGET_CLASS(klass);

    gobject_class->dispose      = ag_header_bar_dispose;
    gobject_class->finalize     = ag_header_bar_finalize;
    gobject_class->set_property = ag_header_bar_set_property;
    gobject_class->get_property = ag_header_bar_get_property;

    properties[PROP_MODE] = g_param_spec_enum(
            "mode",
            "Mode",
            "Header bar mode",
            AG_TYPE_HEADER_BAR_MODE,
            AG_HEADER_BAR_MODE_LIST,
            G_PARAM_STATIC_STRINGS
            | G_PARAM_READWRITE
        );
    g_object_class_install_property(
            gobject_class,
            PROP_MODE,
            properties[PROP_MODE]
        );

    signals[SIGNAL_MODE_CHANGED] = g_signal_new(
            "mode-changed",
            AG_TYPE_HEADER_BAR,
            G_SIGNAL_RUN_FIRST,
            0,
            NULL, NULL,
            NULL,
            G_TYPE_NONE,
            1,
            AG_TYPE_HEADER_BAR_MODE
        );

    gtk_widget_class_set_template_from_resource(
            widget_class,
            "/eu/polonkai/gergely/Astrognome/ui/ag-header-bar.ui"
        );

    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgHeaderBar,
            selection_cancel_button
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgHeaderBar,
            left_stack
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgHeaderBar,
            right_stack
        );

    gtk_widget_class_bind_template_callback(
            widget_class,
            ag_header_bar_selection_mode_cb
        );
    gtk_widget_class_bind_template_callback(
            widget_class,
            ag_header_bar_selection_cancel_cb
        );
    gtk_widget_class_bind_template_callback(
            widget_class,
            ag_header_bar_back_cb
        );
}

static void
ag_header_bar_init(AgHeaderBar *header_bar)
{
    gtk_widget_init_template(GTK_WIDGET(header_bar));

    ag_header_bar_set_mode_internal(header_bar, DEFAULT_MODE, TRUE);
}
