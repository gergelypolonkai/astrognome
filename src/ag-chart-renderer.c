#include <cairo.h>

#include "ag-chart-renderer.h"

typedef struct {
    gchar    *css_class;
    gboolean checked;
    gboolean toggle_visible;
} AgChartRendererPrivate;

enum {
    PROP_0,
    AG_CHART_RENDERER_PROP_CSS_CLASS,
    AG_CHART_RENDERER_PROP_CHECKED,
    AG_CHART_RENDERER_PROP_TOGGLE_VISIBLE,
};

static void ag_chart_renderer_dispose(GObject *gobject);
static void ag_chart_renderer_finalize(GObject *gobject);

G_DEFINE_TYPE_WITH_PRIVATE(AgChartRenderer, ag_chart_renderer, GTK_TYPE_CELL_RENDERER_PIXBUF);

static void
ag_chart_renderer_render(GtkCellRenderer      *renderer,
                         cairo_t              *cr,
                         GtkWidget            *widget,
                         const GdkRectangle   *background_area,
                         const GdkRectangle   *cell_area,
                         GtkCellRendererState flags)
{
    AgChartRendererPrivate *priv = ag_chart_renderer_get_instance_private(AG_CHART_RENDERER(renderer));
    int             margin;
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    GdkPixbuf       *pixbuf;

    gtk_style_context_save(context);
    gtk_style_context_add_class(context, "ag-chart-renderer");

    if (priv->css_class) {
        gtk_style_context_add_class(context, priv->css_class);
    }

    cairo_save(cr);
    gdk_cairo_rectangle(cr, cell_area);
    cairo_clip(cr);

    cairo_translate(cr, cell_area->x, cell_area->y);

    margin = MAX(AG_CHART_RENDERER_TILE_MARGIN, (int)((cell_area->width - AG_CHART_RENDERER_TILE_SIZE) / 2));

    g_object_get(renderer, "pixbuf", &pixbuf, NULL);

    if (pixbuf != NULL) {
        GdkRectangle area = {margin, margin, AG_CHART_RENDERER_TILE_SIZE, AG_CHART_RENDERER_TILE_SIZE};

        g_debug("Rendering chart with preview image");

        GTK_CELL_RENDERER_CLASS(ag_chart_renderer_parent_class)->render(renderer, cr, widget, &area, &area, flags);
    } else {
        g_debug("Rendering chart without preview image");

        gtk_render_frame(context, cr, margin, margin, AG_CHART_RENDERER_TILE_SIZE, AG_CHART_RENDERER_TILE_SIZE);
        gtk_render_background(context, cr, margin, margin, AG_CHART_RENDERER_TILE_SIZE, AG_CHART_RENDERER_TILE_SIZE);
    }

    gtk_style_context_restore(context);

    if (priv->toggle_visible) {
        gint xpad,
             ypad,
             x_offset,
             check_x,
             check_y;

        gtk_cell_renderer_get_padding(GTK_CELL_RENDERER(renderer), &xpad, &ypad);

        if (gtk_widget_get_direction(widget) == GTK_TEXT_DIR_RTL) {
            x_offset = xpad;
        } else {
            x_offset = cell_area->width - AG_CHART_RENDERER_CHECK_ICON_SIZE - xpad;
        }

        check_x = x_offset;
        check_y = cell_area->height - AG_CHART_RENDERER_CHECK_ICON_SIZE - ypad;

        gtk_style_context_save(context);
        gtk_style_context_add_class(context, GTK_STYLE_CLASS_CHECK);

        if (priv->checked) {
            gtk_style_context_set_state(context, GTK_STATE_FLAG_CHECKED);
        }

        gtk_render_background(context, cr, check_x, check_y, AG_CHART_RENDERER_CHECK_ICON_SIZE, AG_CHART_RENDERER_CHECK_ICON_SIZE);
        gtk_render_frame(context, cr, check_x, check_y, AG_CHART_RENDERER_CHECK_ICON_SIZE, AG_CHART_RENDERER_CHECK_ICON_SIZE);
        gtk_render_check(context, cr, check_x, check_y, AG_CHART_RENDERER_CHECK_ICON_SIZE, AG_CHART_RENDERER_CHECK_ICON_SIZE);

        gtk_style_context_restore(context);
    }

    cairo_restore(cr);
}

void
ag_chart_renderer_set_css_class(AgChartRenderer *chart_renderer, const gchar *css_class)
{
    AgChartRendererPrivate *priv = ag_chart_renderer_get_instance_private(chart_renderer);

    g_free(priv->css_class);
    priv->css_class = g_strdup(css_class);
}

const gchar *
ag_chart_renderer_get_css_class(AgChartRenderer *chart_renderer)
{
    AgChartRendererPrivate *priv = ag_chart_renderer_get_instance_private(chart_renderer);

    return priv->css_class;
}

void
ag_chart_renderer_set_checked(AgChartRenderer *chart_renderer, gboolean checked)
{
    AgChartRendererPrivate *priv = ag_chart_renderer_get_instance_private(chart_renderer);

    priv->checked = checked;
}

gboolean
ag_chart_renderer_get_checked(AgChartRenderer *chart_renderer)
{
    AgChartRendererPrivate *priv = ag_chart_renderer_get_instance_private(chart_renderer);

    return priv->checked;
}

void
ag_chart_renderer_set_toggle_visible(AgChartRenderer *chart_renderer, gboolean toggle_visible)
{
    AgChartRendererPrivate *priv = ag_chart_renderer_get_instance_private(chart_renderer);

    priv->toggle_visible = toggle_visible;
}

gboolean
ag_chart_renderer_get_toggle_visible(AgChartRenderer *chart_renderer)
{
    AgChartRendererPrivate *priv = ag_chart_renderer_get_instance_private(chart_renderer);

    return priv->toggle_visible;
}

static void
ag_chart_renderer_get_property(GObject    *gobject,
                               guint      prop_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
    AgChartRenderer *chart_renderer = AG_CHART_RENDERER(gobject);

    switch (prop_id) {
        case AG_CHART_RENDERER_PROP_CSS_CLASS:
            g_value_set_string(value, ag_chart_renderer_get_css_class(chart_renderer));

            break;

        case AG_CHART_RENDERER_PROP_CHECKED:
            g_value_set_boolean(value, ag_chart_renderer_get_checked(chart_renderer));

            break;

        case AG_CHART_RENDERER_PROP_TOGGLE_VISIBLE:
            g_value_set_boolean(value, ag_chart_renderer_get_toggle_visible(chart_renderer));

            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, pspec);

            break;
    }
}

static void
ag_chart_renderer_set_property(GObject      *gobject,
                               guint        prop_id,
                               const GValue *value,
                               GParamSpec   *pspec)
{
    AgChartRenderer *chart_renderer = AG_CHART_RENDERER(gobject);

    switch (prop_id) {
        case AG_CHART_RENDERER_PROP_CSS_CLASS:
            ag_chart_renderer_set_css_class(chart_renderer, g_value_get_string(value));

            break;

        case AG_CHART_RENDERER_PROP_CHECKED:
            ag_chart_renderer_set_checked(chart_renderer, g_value_get_boolean(value));

            break;

        case AG_CHART_RENDERER_PROP_TOGGLE_VISIBLE:
            ag_chart_renderer_set_toggle_visible(chart_renderer, g_value_get_boolean(value));

            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, pspec);

            break;
    }
}

static void
ag_chart_renderer_class_init(AgChartRendererClass *klass)
{
    GObjectClass         *gobject_class       = (GObjectClass *)klass;
    GtkCellRendererClass *cell_renderer_class = (GtkCellRendererClass *)klass;

    gobject_class->dispose      = ag_chart_renderer_dispose;
    gobject_class->finalize     = ag_chart_renderer_finalize;
    gobject_class->set_property = ag_chart_renderer_set_property;
    gobject_class->get_property = ag_chart_renderer_get_property;
    cell_renderer_class->render = ag_chart_renderer_render;

    g_object_class_install_property(
            G_OBJECT_CLASS(klass),
            AG_CHART_RENDERER_PROP_CSS_CLASS,
            g_param_spec_string(
                    "css-class",
                    "css-class",
                    "CSS Class",
                    NULL,
                    G_PARAM_STATIC_NAME
                    | G_PARAM_STATIC_NICK
                    | G_PARAM_STATIC_BLURB
                    | G_PARAM_READABLE
                    | G_PARAM_WRITABLE
                )
        );

    g_object_class_install_property(
            G_OBJECT_CLASS(klass),
            AG_CHART_RENDERER_PROP_CHECKED,
            g_param_spec_boolean(
                    "checked",
                    "checked",
                    "Checked",
                    FALSE,
                    G_PARAM_STATIC_NAME
                    | G_PARAM_STATIC_NICK
                    | G_PARAM_STATIC_BLURB
                    | G_PARAM_READABLE
                    | G_PARAM_WRITABLE
                )
        );

    g_object_class_install_property(
            G_OBJECT_CLASS(klass),
            AG_CHART_RENDERER_PROP_TOGGLE_VISIBLE,
            g_param_spec_boolean(
                    "toggle-visible",
                    "toggle-visible",
                    "Toggle visible",
                    FALSE,
                    G_PARAM_STATIC_NAME
                    | G_PARAM_STATIC_NICK
                    | G_PARAM_STATIC_BLURB
                    | G_PARAM_READABLE
                    | G_PARAM_WRITABLE
                )
        );
}

static void
ag_chart_renderer_init(AgChartRenderer *chart_renderer)
{
    AgChartRendererPrivate *priv = ag_chart_renderer_get_instance_private(chart_renderer);

    priv->checked = FALSE;
    priv->toggle_visible = FALSE;
}

static void
ag_chart_renderer_dispose(GObject *gobject)
{
    G_OBJECT_CLASS(ag_chart_renderer_parent_class)->dispose(gobject);
}

static void
ag_chart_renderer_finalize (GObject *gobject)
{
    g_signal_handlers_destroy(gobject);
    G_OBJECT_CLASS(ag_chart_renderer_parent_class)->finalize(gobject);
}

AgChartRenderer *
ag_chart_renderer_new(void)
{
    AgChartRenderer *chart_renderer = NULL;

    chart_renderer = g_object_new(AG_TYPE_CHART_RENDERER, NULL);

    return chart_renderer;
}
