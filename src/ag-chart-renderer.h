#ifndef __AG_CHART_RENDERER_H__
#define __AG_CHART_RENDERER_H__

#include <gtk/gtk.h>


G_BEGIN_DECLS

#define AG_TYPE_CHART_RENDERER                                          \
   (ag_chart_renderer_get_type())
#define AG_CHART_RENDERER(obj)                                          \
   (G_TYPE_CHECK_INSTANCE_CAST ((obj),                                  \
                                AG_TYPE_CHART_RENDERER,                 \
                                AgChartRenderer))
#define AG_CHART_RENDERER_CLASS(klass)                                  \
   (G_TYPE_CHECK_CLASS_CAST ((klass),                                   \
                             AG_TYPE_CHART_RENDERER,                    \
                             AgChartRendererClass))
#define IS_AG_CHART_RENDERER(obj)                                       \
   (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                                  \
                                AG_TYPE_CHART_RENDERER))
#define IS_AG_CHART_RENDERER_CLASS(klass)                               \
   (G_TYPE_CHECK_CLASS_TYPE ((klass),                                   \
                             AG_TYPE_CHART_RENDERER))
#define AG_CHART_RENDERER_GET_CLASS(obj)                                \
   (G_TYPE_INSTANCE_GET_CLASS ((obj),                                   \
                               AG_TYPE_CHART_RENDERER,                  \
                               AgChartRendererClass))

typedef struct _AgChartRenderer      AgChartRenderer;
typedef struct _AgChartRendererClass AgChartRendererClass;

struct _AgChartRendererClass
{
    GtkCellRendererPixbufClass parent_class;
};

struct _AgChartRenderer
{
    GtkCellRendererPixbuf parent;
};

#define AG_CHART_RENDERER_TILE_SIZE 256
#define AG_CHART_RENDERER_CHECK_ICON_SIZE 40
#define AG_CHART_RENDERER_TILE_MARGIN AG_CHART_RENDERER_CHECK_ICON_SIZE / 4
#define AG_CHART_RENDERER_TILE_MARGIN_BOTTOM AG_CHART_RENDERER_CHECK_ICON_SIZE / 8

GType ag_chart_renderer_get_type (void) G_GNUC_CONST;

AgChartRenderer *ag_chart_renderer_new(void);

void ag_chart_renderer_set_toggle_visible(AgChartRenderer *chart_renderer, gboolean toggle_visible);

gboolean ag_chart_renderer_get_toggle_visible(AgChartRenderer *chart_renderer);

G_END_DECLS

#endif /* __AG_CHART_RENDERER_H__ */
