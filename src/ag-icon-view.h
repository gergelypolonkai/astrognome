#ifndef __AG_ICON_VIEW_H__
#define __AG_ICON_VIEW_H__

#include <gtk/gtk.h>

#include "ag-db.h"

G_BEGIN_DECLS

#define AG_TYPE_ICON_VIEW                                               \
   (ag_icon_view_get_type())
#define AG_ICON_VIEW(obj)                                               \
   (G_TYPE_CHECK_INSTANCE_CAST ((obj),                                  \
                                AG_TYPE_ICON_VIEW,                      \
                                AgIconView))
#define AG_ICON_VIEW_CLASS(klass)                                       \
   (G_TYPE_CHECK_CLASS_CAST ((klass),                                   \
                             AG_TYPE_ICON_VIEW,                         \
                             AgIconViewClass))
#define IS_AG_ICON_VIEW(obj)                                            \
   (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                                  \
                                AG_TYPE_ICON_VIEW))
#define IS_AG_ICON_VIEW_CLASS(klass)                                    \
   (G_TYPE_CHECK_CLASS_TYPE ((klass),                                   \
                             AG_TYPE_ICON_VIEW))
#define AG_ICON_VIEW_GET_CLASS(obj)                                     \
   (G_TYPE_INSTANCE_GET_CLASS ((obj),                                   \
                               AG_TYPE_ICON_VIEW,                       \
                               AgIconViewClass))

typedef struct _AgIconView      AgIconView;
typedef struct _AgIconViewClass AgIconViewClass;

struct _AgIconViewClass
{
    GtkIconViewClass parent_class;
};

struct _AgIconView
{
    GtkIconView parent;
};

typedef enum {
    AG_ICON_VIEW_MODE_NORMAL,
    AG_ICON_VIEW_MODE_SELECTION
} AgIconViewMode;

GType ag_icon_view_get_type (void) G_GNUC_CONST;

void ag_icon_view_set_mode(AgIconView *icon_view, AgIconViewMode mode);

AgIconViewMode ag_icon_view_get_mode(AgIconView *icon_view);

void ag_icon_view_add_chart(AgIconView *icon_view, AgDbChartSave *chart_save);

GList *ag_icon_view_get_selected_items(AgIconView *icon_view);

AgDbChartSave *ag_icon_view_get_chart_save_at_path(AgIconView *icon_view,
                                                   const GtkTreePath *path);

void ag_icon_view_select_all(AgIconView *icon_view);

void ag_icon_view_unselect_all(AgIconView *icon_view);

void ag_icon_view_remove_selected(AgIconView *icon_view);

void ag_icon_view_remove_all(AgIconView *icon_view);

G_END_DECLS

#endif /* __AG_ICON_VIEW_H__ */
