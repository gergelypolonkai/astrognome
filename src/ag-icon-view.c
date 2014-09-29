#include <gdk-pixbuf/gdk-pixbuf.h>

#include "ag-enumtypes.h"
#include "ag-icon-view.h"
#include "ag-db.h"
#include "ag-chart-renderer.h"
#include "ag-display-theme.h"
#include "ag-chart.h"

typedef struct _AgIconViewPrivate {
    AgIconViewMode  mode;
    AgChartRenderer *thumb_renderer;
    GtkCellRenderer *text_renderer;
    GtkListStore    *model;
} AgIconViewPrivate;

enum {
    PROP_0,
    PROP_MODE,
    PROP_LAST
};

enum {
    AG_ICON_VIEW_COLUMN_SELECTED,
    AG_ICON_VIEW_COLUMN_ITEM,
    AG_ICON_VIEW_COLUMN_PIXBUF,
    AG_ICON_VIEW_COLUMN_COLUMNS
};

G_DEFINE_TYPE_WITH_PRIVATE(AgIconView, ag_icon_view, GTK_TYPE_ICON_VIEW);

static GParamSpec *properties[PROP_LAST];

void
ag_icon_view_set_mode(AgIconView *icon_view, AgIconViewMode mode)
{
    AgIconViewPrivate *priv = ag_icon_view_get_instance_private(icon_view);

    if (priv->mode != mode) {
        priv->mode = mode;

        if (mode != AG_ICON_VIEW_MODE_SELECTION) {
            ag_icon_view_unselect_all(icon_view);
        }

        ag_chart_renderer_set_toggle_visible(
                priv->thumb_renderer,
                (mode == AG_ICON_VIEW_MODE_SELECTION)
            );

        gtk_widget_queue_draw(GTK_WIDGET(icon_view));

        g_object_notify_by_pspec(
                G_OBJECT(icon_view),
                properties[PROP_MODE]
            );
    }
}

AgIconViewMode
ag_icon_view_get_mode(AgIconView *icon_view)
{
    AgIconViewPrivate *priv = ag_icon_view_get_instance_private(icon_view);

    return priv->mode;
}

static void
ag_icon_view_set_property(GObject      *gobject,
                          guint        prop_id,
                          const GValue *value,
                          GParamSpec *param_spec)
{
    switch (prop_id) {
        case PROP_MODE:
            ag_icon_view_set_mode(
                    AG_ICON_VIEW(gobject),
                    g_value_get_enum(value)
                );

            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);

            break;
    }
}

static void
ag_icon_view_get_property(GObject    *gobject,
                          guint      prop_id,
                          GValue     *value,
                          GParamSpec *param_spec)
{
    AgIconViewPrivate *priv = ag_icon_view_get_instance_private(
            AG_ICON_VIEW(gobject)
        );

    switch (prop_id) {
        case PROP_MODE:
            g_value_set_enum(value, priv->mode);

            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);

            break;
    }
}

static void
ag_icon_view_selection_changed(AgIconView *icon_view)
{
    g_signal_emit_by_name(icon_view, "selection-changed");
}

static void
ag_icon_view_item_activated(AgIconView *icon_view, GtkTreePath *path)
{
    g_signal_emit_by_name(icon_view, "item-activated", path);
}

static gboolean
ag_icon_view_button_press_event_cb(GtkWidget      *widget,
                                   GdkEventButton *event)
{
    GtkTreePath *path;
    GtkIconView *gtk_icon_view = GTK_ICON_VIEW(widget);
    AgIconView  *ag_icon_view  = AG_ICON_VIEW(widget);

    path = gtk_icon_view_get_path_at_pos(
            gtk_icon_view,
            ((GdkEventButton *)event)->x,
            ((GdkEventButton *)event)->y
        );

    if (path != NULL) {
        gboolean      selected;
        AgDbChartSave *chart_save;
        GtkListStore  *store = GTK_LIST_STORE(gtk_icon_view_get_model(
                gtk_icon_view)
            );

        if (event->button == GDK_BUTTON_SECONDARY) {
            ag_icon_view_set_mode(ag_icon_view, AG_ICON_VIEW_MODE_SELECTION);
        }

        if (ag_icon_view_get_mode(ag_icon_view) == AG_ICON_VIEW_MODE_SELECTION) {
            GtkTreeIter iter;

            if (gtk_tree_model_get_iter(GTK_TREE_MODEL(store), &iter, path)) {
                gtk_tree_model_get(
                        GTK_TREE_MODEL(store), &iter,
                        AG_ICON_VIEW_COLUMN_SELECTED, &selected,
                        AG_ICON_VIEW_COLUMN_ITEM, &chart_save,
                        -1
                    );

                gtk_list_store_set(
                        store, &iter,
                        AG_ICON_VIEW_COLUMN_SELECTED, !selected,
                        -1
                    );

                ag_icon_view_selection_changed(ag_icon_view);
            }
        } else {
            ag_icon_view_item_activated(ag_icon_view, path);
        }
    }

    return FALSE;
}

static void
ag_icon_view_class_init(AgIconViewClass *klass)
{
    GObjectClass     *gobject_class   = G_OBJECT_CLASS(klass);
    GtkWidgetClass   *widget_class    = GTK_WIDGET_CLASS(klass);

    gobject_class->set_property = ag_icon_view_set_property;
    gobject_class->get_property = ag_icon_view_get_property;
    widget_class->button_press_event = ag_icon_view_button_press_event_cb;

    properties[PROP_MODE] = g_param_spec_enum(
            "mode",
            "Mode",
            "Mode",
            AG_TYPE_ICON_VIEW_MODE,
            AG_ICON_VIEW_MODE_NORMAL,
            G_PARAM_STATIC_NAME
                | G_PARAM_STATIC_NICK
                | G_PARAM_STATIC_BLURB
                | G_PARAM_READABLE
                | G_PARAM_WRITABLE
        );
    g_object_class_install_property(
            gobject_class,
            PROP_MODE,
            properties[PROP_MODE]
        );
}

static void
ag_icon_view_text_renderer_func(GtkCellLayout   *layout,
                                GtkCellRenderer *renderer,
                                GtkTreeModel    *model,
                                GtkTreeIter     *iter,
                                AgIconView      *icon_view)
{
    AgDbChartSave *chart_save;

    gtk_tree_model_get(model, iter, AG_ICON_VIEW_COLUMN_ITEM, &chart_save, -1);

    if (chart_save) {
        gchar *text;

        text = g_markup_escape_text(chart_save->name, -1);
        g_object_set(renderer, "markup", text, NULL);
        g_free(text);
    }
}

static void
ag_icon_view_init(AgIconView *icon_view)
{
    AgIconViewPrivate *priv = ag_icon_view_get_instance_private(icon_view);
    guint tile_width,
          tile_height;

    priv->model = gtk_list_store_new(
            AG_ICON_VIEW_COLUMN_COLUMNS,
            G_TYPE_BOOLEAN,
            AG_TYPE_DB_CHART_SAVE,
            GDK_TYPE_PIXBUF
        );
    gtk_icon_view_set_model(
            GTK_ICON_VIEW(icon_view),
            GTK_TREE_MODEL(priv->model)
        );

    gtk_icon_view_set_selection_mode(
            GTK_ICON_VIEW(icon_view),
            GTK_SELECTION_NONE
        );
    priv->mode = AG_ICON_VIEW_MODE_NORMAL;

    gtk_icon_view_set_item_padding(GTK_ICON_VIEW(icon_view), 0);
    gtk_icon_view_set_margin(GTK_ICON_VIEW(icon_view), 12);

    tile_width = AG_CHART_RENDERER_TILE_SIZE
        + 2 * AG_CHART_RENDERER_TILE_MARGIN;
    tile_height = AG_CHART_RENDERER_TILE_SIZE
        + AG_CHART_RENDERER_TILE_MARGIN
        + AG_CHART_RENDERER_TILE_MARGIN_BOTTOM;

    priv->thumb_renderer = ag_chart_renderer_new();
    gtk_cell_renderer_set_alignment(
            GTK_CELL_RENDERER(priv->thumb_renderer),
            0.5, 0.5
        );
    gtk_cell_renderer_set_fixed_size(
            GTK_CELL_RENDERER(priv->thumb_renderer),
            tile_width, tile_height
        );
    gtk_cell_layout_pack_start(
            GTK_CELL_LAYOUT(icon_view),
            GTK_CELL_RENDERER(priv->thumb_renderer),
            FALSE
        );
    gtk_cell_layout_add_attribute(
            GTK_CELL_LAYOUT(icon_view),
            GTK_CELL_RENDERER(priv->thumb_renderer),
            "checked", AG_ICON_VIEW_COLUMN_SELECTED
        );
    gtk_cell_layout_add_attribute(
            GTK_CELL_LAYOUT(icon_view),
            GTK_CELL_RENDERER(priv->thumb_renderer),
            "pixbuf", AG_ICON_VIEW_COLUMN_PIXBUF
        );
/*
    gtk_cell_layout_set_cell_data_func(
            GTK_CELL_LAYOUT(icon_view),
            GTK_CELL_RENDERER(priv->thumb_renderer),
            (GtkCellLayoutDataFunc)ag_icon_view_chart_renderer_func,
            icon_view,
            NULL
        );
*/

    priv->text_renderer = gtk_cell_renderer_text_new();
    gtk_cell_renderer_set_alignment(
            GTK_CELL_RENDERER(priv->text_renderer),
            0.5, 0.5
        );
    gtk_cell_layout_pack_start(
            GTK_CELL_LAYOUT(icon_view),
            priv->text_renderer,
            TRUE
        );
    gtk_cell_layout_set_cell_data_func(
            GTK_CELL_LAYOUT(icon_view),
            GTK_CELL_RENDERER(priv->text_renderer),
            (GtkCellLayoutDataFunc)ag_icon_view_text_renderer_func,
            icon_view,
            NULL
        );
}

void
ag_icon_view_add_chart(AgIconView *icon_view, AgDbChartSave *chart_save)
{
    GtkTreeIter       iter;
    AgChart           *chart;
    AgIconViewPrivate *priv      = ag_icon_view_get_instance_private(icon_view);
    AgDisplayTheme    *theme     = ag_display_theme_get_preview_theme();
    AgDbChartSave     *save_data = chart_save;
    AgDb              *db        = ag_db_get();
    GdkPixbuf         *pixbuf    = NULL;

    g_debug("Adding chart for %s", chart_save->name);

    if (!chart_save->populated) {
        save_data = ag_db_chart_get_data_by_id(
                db,
                chart_save->db_id,
                NULL
        );
    } else {
        save_data = ag_db_chart_save_ref(chart_save);
    }

    g_object_unref(db);

    chart = ag_chart_new_from_db_save(
            save_data,
            TRUE,
            NULL
        );

    pixbuf = ag_chart_get_pixbuf(
            chart,
            AG_CHART_RENDERER_TILE_SIZE,
            AG_CHART_RENDERER_ICON_SIZE,
            theme,
            NULL
        );

    g_object_unref(chart);

    gtk_list_store_append(priv->model, &iter);
    gtk_list_store_set(
            priv->model, &iter,
            AG_ICON_VIEW_COLUMN_SELECTED, FALSE,
            AG_ICON_VIEW_COLUMN_ITEM,     save_data,
            AG_ICON_VIEW_COLUMN_PIXBUF,   pixbuf,
            -1
        );
}

static gboolean
ag_icon_view_check_selected(GtkTreeModel *model,
                            GtkTreePath  *path,
                            GtkTreeIter  *iter,
                            GList        **list)
{
    gboolean selected;

    gtk_tree_model_get(model, iter, AG_ICON_VIEW_COLUMN_SELECTED, &selected, -1);

    if (selected) {
        *list = g_list_prepend(*list, path);
    }

    return FALSE;
}

GList *
ag_icon_view_get_selected_items(AgIconView *icon_view)
{
    AgIconViewPrivate *priv  = ag_icon_view_get_instance_private(icon_view);
    GList             *items = NULL;

    gtk_tree_model_foreach(GTK_TREE_MODEL(priv->model), (GtkTreeModelForeachFunc)ag_icon_view_check_selected, &items);

    return g_list_reverse(items);
}

static gboolean
ag_icon_view_change_item_selection(GtkListStore *model,
                                   GtkTreePath  *path,
                                   GtkTreeIter  *iter,
                                   gboolean     *selected)
{
    gtk_list_store_set(model, iter, AG_ICON_VIEW_COLUMN_SELECTED, *selected, -1);

    return FALSE;
}

void
ag_icon_view_select_all(AgIconView *icon_view)
{
    AgIconViewPrivate *priv    = ag_icon_view_get_instance_private(icon_view);
    gboolean          selected = TRUE;

    gtk_tree_model_foreach(
            GTK_TREE_MODEL(priv->model),
            (GtkTreeModelForeachFunc)ag_icon_view_change_item_selection,
            &selected
        );

    ag_icon_view_selection_changed(icon_view);
}

void
ag_icon_view_unselect_all(AgIconView *icon_view)
{
    AgIconViewPrivate *priv    = ag_icon_view_get_instance_private(icon_view);
    gboolean          selected = FALSE;

    gtk_tree_model_foreach(
            GTK_TREE_MODEL(priv->model),
            (GtkTreeModelForeachFunc)ag_icon_view_change_item_selection,
            &selected
        );

    ag_icon_view_selection_changed(icon_view);
}

AgDbChartSave *
ag_icon_view_get_chart_save_at_path(AgIconView        *icon_view,
                                    const GtkTreePath *path)
{
    AgIconViewPrivate *priv = ag_icon_view_get_instance_private(icon_view);
    GtkTreeIter       iter;
    AgDbChartSave     *save_data;

    if (gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->model), &iter, (GtkTreePath *)path)) {
        gtk_tree_model_get(
                GTK_TREE_MODEL(priv->model), &iter,
                AG_ICON_VIEW_COLUMN_ITEM, &save_data,
                -1
            );
    } else {
        g_warning("Invalid tree path");

        save_data = NULL;
    }

    return save_data;
}

void
ag_icon_view_remove_selected(AgIconView *icon_view)
{
    AgIconViewPrivate *priv = ag_icon_view_get_instance_private(icon_view);
    GList *paths = g_list_reverse(ag_icon_view_get_selected_items(icon_view)),
          *l;

    for (l = paths; l; l = g_list_next(l)) {
        GtkTreeIter iter;
        GtkTreePath *path = l->data;

        if (gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->model), &iter, path)) {
            gtk_list_store_remove(priv->model, &iter);
        }
    }

    ag_icon_view_selection_changed(icon_view);
}

void
ag_icon_view_remove_all(AgIconView *icon_view)
{
    AgIconViewPrivate *priv = ag_icon_view_get_instance_private(icon_view);

    gtk_list_store_clear(priv->model);
}
