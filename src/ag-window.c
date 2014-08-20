#include <math.h>
#include <string.h>
#include <glib/gi18n.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <webkit2/webkit2.h>
#include <libgd/gd-main-view.h>
#include <libgd/gd-main-view-generic.h>
#include <gtk/gtk.h>

#include <swe-glib.h>

#include "config.h"
#include "ag-app.h"
#include "ag-window.h"
#include "ag-chart.h"
#include "ag-settings.h"
#include "ag-db.h"

struct _AgWindowPrivate {
    GtkWidget     *header_bar;
    GtkWidget     *menubutton_stack;
    GtkWidget     *new_back_stack;
    GtkWidget     *selection_toolbar;
    GtkWidget     *stack;
    GtkWidget     *name;
    GtkWidget     *north_lat;
    GtkWidget     *south_lat;
    GtkWidget     *latitude;
    GtkWidget     *east_long;
    GtkWidget     *west_long;
    GtkWidget     *longitude;
    GtkWidget     *year;
    GtkWidget     *month;
    GtkWidget     *day;
    GtkWidget     *hour;
    GtkWidget     *minute;
    GtkWidget     *second;
    GtkWidget     *timezone;
    GtkWidget     *house_system;

    GtkWidget     *tab_list;
    GtkWidget     *tab_chart;
    GtkWidget     *tab_edit;
    GtkWidget     *current_tab;

    GtkWidget     *aspect_table;
    GtkWidget     *chart_web_view;
    GtkAdjustment *year_adjust;

    AgSettings    *settings;
    AgChart       *chart;
    gboolean      aspect_table_populated;
    GtkTextBuffer *note_buffer;
    GtkListStore  *house_system_model;
    GtkListStore  *db_chart_data;
    AgDbSave      *saved_data;
};

G_DEFINE_QUARK(ag_window_error_quark, ag_window_error);

G_DEFINE_TYPE_WITH_PRIVATE(AgWindow, ag_window, GTK_TYPE_APPLICATION_WINDOW);

static void
ag_window_gear_menu_action(GSimpleAction *action,
                           GVariant      *parameter,
                           gpointer user_data)
{
    GVariant *state;

    state = g_action_get_state(G_ACTION(action));
    g_action_change_state(
            G_ACTION(action),
            g_variant_new_boolean(!g_variant_get_boolean(state))
        );

    g_variant_unref(state);
}

static void
ag_window_view_menu_action(GSimpleAction *action,
                           GVariant      *parameter,
                           gpointer      user_data)
{
    GVariant *state;

    state = g_action_get_state(G_ACTION(action));
    g_action_change_state(
            G_ACTION(action),
            g_variant_new_boolean(!g_variant_get_boolean(state))
        );

    g_variant_unref(state);
}

const gchar *
ag_window_planet_character(GswePlanet planet)
{
    switch (planet) {
        case GSWE_PLANET_ASCENDANT:
            return "AC";

        case GSWE_PLANET_MC:
            return "MC";

        case GSWE_PLANET_VERTEX:
            return "Vx";

        case GSWE_PLANET_SUN:
            return "☉";

        case GSWE_PLANET_MOON:
            return "☽";

        case GSWE_PLANET_MOON_NODE:
            return "☊";

        case GSWE_PLANET_MERCURY:
            return "☿";

        case GSWE_PLANET_VENUS:
            return "♀";

        case GSWE_PLANET_MARS:
            return "♂";

        case GSWE_PLANET_JUPITER:
            return "♃";

        case GSWE_PLANET_SATURN:
            return "♄";

        case GSWE_PLANET_URANUS:
            return "♅";

        case GSWE_PLANET_NEPTUNE:
            return "♆";

        case GSWE_PLANET_PLUTO:
            return "♇";

        case GSWE_PLANET_CERES:
            return "⚳";

        case GSWE_PLANET_PALLAS:
            return "⚴";

        case GSWE_PLANET_JUNO:
            return "⚵";

        case GSWE_PLANET_VESTA:
            return "⚶";

        case GSWE_PLANET_CHIRON:
            return "⚷";

        case GSWE_PLANET_MOON_APOGEE:
            return "⚸";

        default:
            return NULL;
    }
}

GtkWidget *
ag_window_create_planet_widget(GswePlanetInfo *planet_info)
{
    const gchar *planet_char;
    GswePlanet  planet    = gswe_planet_info_get_planet(planet_info);
    GSettings   *settings = ag_settings_peek_main_settings(ag_settings_get());

    if (
            ((planet_char = ag_window_planet_character(planet)) != NULL)
            && (g_settings_get_boolean(settings, "planets-char"))
        )
    {
        return gtk_label_new(planet_char);
    }

    switch (planet) {
        case GSWE_PLANET_SUN:
            return gtk_image_new_from_resource(
                    "/eu/polonkai/gergely"
                    "/Astrognome/default-icons/planet-sun.svg"
                );

        default:
            return gtk_label_new(gswe_planet_info_get_name(planet_info));
    }
}

const gchar *
ag_window_aspect_character(GsweAspect aspect)
{
    switch (aspect) {
        case GSWE_ASPECT_CONJUCTION:
            return "☌";

        case GSWE_ASPECT_OPPOSITION:
            return "☍";

        case GSWE_ASPECT_QUINTILE:
            return "Q";

        case GSWE_ASPECT_BIQUINTILE:
            return "BQ";

        case GSWE_ASPECT_SQUARE:
            return "◽";

        case GSWE_ASPECT_TRINE:
            return "▵";

        case GSWE_ASPECT_SEXTILE:
            return "⚹";

        case GSWE_ASPECT_SEMISEXTILE:
            return "⚺";

        case GSWE_ASPECT_QUINCUNX:
            return "⚻";

        case GSWE_ASPECT_SESQUISQUARE:
            return "⚼";

        default:
            return NULL;
    }
}

GtkWidget *
ag_window_create_aspect_widget(GsweAspectInfo *aspect_info)
{
    const gchar *aspect_char;
    GsweAspect  aspect    = gswe_aspect_info_get_aspect(aspect_info);
    GSettings   *settings = ag_settings_peek_main_settings(ag_settings_get());

    if (
            ((aspect_char = ag_window_aspect_character(aspect)) != NULL)
            && (g_settings_get_boolean(settings, "aspects-char"))
        )
    {
        return gtk_label_new(aspect_char);
    }

    switch (aspect) {
        default:
            return gtk_label_new(gswe_aspect_info_get_name(aspect_info));
    }
}

void
ag_window_redraw_aspect_table(AgWindow *window)
{
    GList           *planet_list,
                    *planet1,
                    *planet2;
    guint           i,
                    j;
    AgWindowPrivate *priv        = ag_window_get_instance_private(window);

    planet_list = ag_chart_get_planets(priv->chart);

    if (priv->aspect_table_populated == FALSE) {
        GList *planet;
        guint i;

        for (
                    planet = planet_list, i = 0;
                    planet;
                    planet = g_list_next(planet), i++
                ) {
            GtkWidget      *label_hor,
                           *label_ver,
                           *current_widget;
            GswePlanet     planet_id;
            GswePlanetData *planet_data;
            GswePlanetInfo *planet_info;

            planet_id = GPOINTER_TO_INT(planet->data);
            planet_data = gswe_moment_get_planet(
                    GSWE_MOMENT(priv->chart),
                    planet_id,
                    NULL
                );
            planet_info = gswe_planet_data_get_planet_info(planet_data);

            if ((current_widget = gtk_grid_get_child_at(
                        GTK_GRID(priv->aspect_table),
                        i + 1, i
                    )) != NULL) {
                gtk_container_remove(
                        GTK_CONTAINER(priv->aspect_table),
                        current_widget
                    );
            }

            label_hor = ag_window_create_planet_widget(planet_info);
            gtk_grid_attach(
                    GTK_GRID(priv->aspect_table),
                    label_hor,
                    i + 1, i,
                    1, 1
                );

            if (i > 0) {
                if ((current_widget = gtk_grid_get_child_at(
                            GTK_GRID(priv->aspect_table),
                            0, i
                    )) != NULL) {
                    gtk_container_remove(
                            GTK_CONTAINER(priv->aspect_table),
                            current_widget
                        );
                }

                label_ver = ag_window_create_planet_widget(planet_info);
                gtk_grid_attach(
                        GTK_GRID(priv->aspect_table),
                        label_ver,
                        0, i,
                        1, 1
                    );
            }
        }

        priv->aspect_table_populated = TRUE;
    }

    for (
                planet1 = planet_list, i = 0;
                planet1;
                planet1 = g_list_next(planet1), i++
            ) {
        for (
                    planet2 = planet_list, j = 0;
                    planet2;
                    planet2 = g_list_next(planet2), j++
                ) {
            GsweAspectData *aspect;
            GtkWidget      *aspect_widget;
            GError         *err = NULL;

            if (GPOINTER_TO_INT(planet1->data)
                        == GPOINTER_TO_INT(planet2->data)
                    ) {
                break;
            }

            if ((aspect_widget = gtk_grid_get_child_at(
                        GTK_GRID(priv->aspect_table),
                        j + 1, i
                    )) != NULL) {
                gtk_container_remove(
                        GTK_CONTAINER(priv->aspect_table),
                        aspect_widget
                    );
            }

            if ((aspect = gswe_moment_get_aspect_by_planets(
                        GSWE_MOMENT(priv->chart),
                        GPOINTER_TO_INT(planet1->data),
                        GPOINTER_TO_INT(planet2->data),
                        &err
                    )) != NULL) {
                GsweAspectInfo *aspect_info;

                aspect_info   = gswe_aspect_data_get_aspect_info(aspect);

                if (gswe_aspect_data_get_aspect(aspect) != GSWE_ASPECT_NONE) {
                    aspect_widget = ag_window_create_aspect_widget(aspect_info);
                    gtk_grid_attach(
                            GTK_GRID(priv->aspect_table),
                            aspect_widget,
                            j + 1, i,
                            1, 1
                        );
                }
            } else if (err) {
                g_warning("%s\n", err->message);
            } else {
                g_error(
                        "No aspect is returned between two planets. " \
                        "This is a bug in SWE-GLib!"
                    );
            }
        }
    }

    gtk_widget_show_all(priv->aspect_table);
}

void
ag_window_redraw_chart(AgWindow *window)
{
    gsize           length;
    GError          *err         = NULL;
    AgWindowPrivate *priv        = ag_window_get_instance_private(window);
    gchar           *svg_content = ag_chart_create_svg(
            priv->chart,
            &length,
            &err
        );

    if (svg_content == NULL) {
        ag_app_message_dialog(
                GTK_WIDGET(window),
                GTK_MESSAGE_WARNING,
                "Unable to draw chart: %s",
                err->message
            );
    } else {
        GBytes *content;

        content = g_bytes_new_take(svg_content, length);

        webkit_web_view_load_bytes(
                WEBKIT_WEB_VIEW(priv->chart_web_view),
                content, "image/svg+xml",
                "UTF-8", NULL
            );
        g_bytes_unref(content);
    }

    ag_window_redraw_aspect_table(window);
}

static gboolean
ag_window_set_house_system(GtkTreeModel *model,
                            GtkTreePath  *path,
                            GtkTreeIter  *iter,
                            AgWindow     *window)
{
    GsweHouseSystem row_house_system;
    AgWindowPrivate *priv        = ag_window_get_instance_private(window);
    GsweHouseSystem house_system = gswe_moment_get_house_system(
            GSWE_MOMENT(priv->chart)
        );

    gtk_tree_model_get(
            GTK_TREE_MODEL(priv->house_system_model),
            iter,
            0, &row_house_system,
            -1
        );

    if (house_system == row_house_system) {
        gtk_combo_box_set_active_iter(GTK_COMBO_BOX(priv->house_system), iter);

        return TRUE;
    }

    return FALSE;
}

void
ag_window_update_from_chart(AgWindow *window)
{
    AgWindowPrivate *priv        = ag_window_get_instance_private(window);
    GsweTimestamp   *timestamp   = gswe_moment_get_timestamp(
            GSWE_MOMENT(priv->chart)
        );
    GsweCoordinates *coordinates = gswe_moment_get_coordinates(
            GSWE_MOMENT(priv->chart)
        );

    gtk_spin_button_set_value(
            GTK_SPIN_BUTTON(priv->year),
            gswe_timestamp_get_gregorian_year(timestamp, NULL)
        );
    gtk_spin_button_set_value(
            GTK_SPIN_BUTTON(priv->month),
            gswe_timestamp_get_gregorian_month(timestamp, NULL)
        );
    gtk_spin_button_set_value(
            GTK_SPIN_BUTTON(priv->day),
            gswe_timestamp_get_gregorian_day(timestamp, NULL)
        );
    gtk_spin_button_set_value(
            GTK_SPIN_BUTTON(priv->hour),
            gswe_timestamp_get_gregorian_hour(timestamp, NULL)
        );
    gtk_spin_button_set_value(
            GTK_SPIN_BUTTON(priv->minute),
            gswe_timestamp_get_gregorian_minute(timestamp, NULL)
        );
    gtk_spin_button_set_value(
            GTK_SPIN_BUTTON(priv->second),
            gswe_timestamp_get_gregorian_second(timestamp, NULL));
    gtk_spin_button_set_value(
            GTK_SPIN_BUTTON(priv->timezone),
            gswe_timestamp_get_gregorian_timezone(timestamp)
        );
    gtk_spin_button_set_value(
            GTK_SPIN_BUTTON(priv->longitude),
            fabs(coordinates->longitude)
        );

    if (coordinates->longitude < 0.0) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->west_long), TRUE);
    }

    gtk_spin_button_set_value(
            GTK_SPIN_BUTTON(priv->latitude),
            fabs(coordinates->latitude)
        );

    if (coordinates->latitude < 0.0) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->south_lat), TRUE);
    }

    gtk_tree_model_foreach(
            GTK_TREE_MODEL(priv->house_system_model),
            (GtkTreeModelForeachFunc)ag_window_set_house_system,
            window
        );

    gtk_entry_set_text(GTK_ENTRY(priv->name), ag_chart_get_name(priv->chart));

    if (ag_chart_get_note(priv->chart)) {
        // TODO: maybe setting length to -1 here is not that good of an idea…
        gtk_text_buffer_set_text(
                GTK_TEXT_BUFFER(priv->note_buffer),
                ag_chart_get_note(priv->chart),
                -1
            );
    }

    g_free(coordinates);

    ag_window_redraw_chart(window);
}

static void
ag_window_chart_changed(AgChart *chart, AgWindow *window)
{
    ag_window_redraw_chart(window);
}

static void
ag_window_recalculate_chart(AgWindow *window, gboolean set_everything)
{
    AgDbSave        *edit_data,
                    *chart_data;
    AgWindowPrivate *priv = ag_window_get_instance_private(window);
    gboolean        south,
                    west;
    GtkTreeIter     house_system_iter;
    GsweHouseSystem house_system;
    GtkTextIter     start_iter,
                    end_iter;
    GsweTimestamp   *timestamp;
    gint            db_id = (priv->saved_data) ? priv->saved_data->db_id : -1;

    south = gtk_toggle_button_get_active(
            GTK_TOGGLE_BUTTON(priv->south_lat)
        );

    west      = gtk_toggle_button_get_active(
            GTK_TOGGLE_BUTTON(priv->west_long)
        );

    edit_data = g_new0(AgDbSave, 1);

    edit_data->db_id = db_id;

    edit_data->name = g_strdup(gtk_entry_get_text(GTK_ENTRY(priv->name)));
    // TODO: This will cause problems with imported charts…
    edit_data->country = NULL;
    edit_data->city = NULL;
    edit_data->longitude = gtk_spin_button_get_value(
            GTK_SPIN_BUTTON(priv->longitude)
        );

    if (west) {
        edit_data->longitude = - edit_data->longitude;
    }

    edit_data->latitude = gtk_spin_button_get_value(
            GTK_SPIN_BUTTON(priv->latitude)
        );

    if (south) {
        edit_data->latitude = - edit_data->latitude;
    }

    // TODO: So as this…
    edit_data->altitude = DEFAULT_ALTITUDE;
    edit_data->year = gtk_spin_button_get_value_as_int(
            GTK_SPIN_BUTTON(priv->year)
        );
    edit_data->month = gtk_spin_button_get_value_as_int(
            GTK_SPIN_BUTTON(priv->month)
        );
    edit_data->day = gtk_spin_button_get_value_as_int(
            GTK_SPIN_BUTTON(priv->day)
        );
    edit_data->hour = gtk_spin_button_get_value_as_int(
            GTK_SPIN_BUTTON(priv->hour)
        );
    edit_data->minute = gtk_spin_button_get_value_as_int(
            GTK_SPIN_BUTTON(priv->minute)
        );
    edit_data->second = gtk_spin_button_get_value_as_int(
            GTK_SPIN_BUTTON(priv->second)
        );
    edit_data->timezone = gtk_spin_button_get_value(
            GTK_SPIN_BUTTON(priv->timezone)
        );

    if (!gtk_combo_box_get_active_iter(
                GTK_COMBO_BOX(priv->house_system),
                &house_system_iter
            )) {
        g_error("House system is not set! This is clearly a bug.");
    }

    gtk_tree_model_get(
            GTK_TREE_MODEL(priv->house_system_model),
            &house_system_iter,
            0, &house_system,
            -1
        );
    edit_data->house_system = g_strdup(
            ag_house_system_id_to_nick(house_system)
        );
    gtk_text_buffer_get_bounds(priv->note_buffer, &start_iter, &end_iter);
    edit_data->note = gtk_text_buffer_get_text(
            priv->note_buffer,
            &start_iter, &end_iter,
            TRUE
        );

    chart_data = (priv->chart)
            ? ag_chart_get_db_save(priv->chart, db_id)
            : NULL
        ;

    if (ag_db_save_identical(edit_data, chart_data, !set_everything)) {
        g_debug("No redrawing needed");

        ag_db_save_data_free(edit_data);
        ag_db_save_data_free(chart_data);

        return;
    }

    ag_db_save_data_free(chart_data);

    g_debug("Recalculating chart data");

    // TODO: Set timezone according to the city selected!
    if (priv->chart == NULL) {
        timestamp = gswe_timestamp_new_from_gregorian_full(
                edit_data->year, edit_data->month, edit_data->day,
                edit_data->hour, edit_data->minute, edit_data->second, 0,
                edit_data->timezone
            );
        priv->chart = ag_chart_new_full(
                timestamp,
                edit_data->longitude, edit_data->latitude, edit_data->altitude,
                house_system
            );
        g_signal_connect(
                priv->chart,
                "changed",
                G_CALLBACK(ag_window_chart_changed),
                window
            );
        ag_window_redraw_chart(window);
    } else {
        gswe_moment_set_house_system(GSWE_MOMENT(priv->chart), house_system);
        timestamp = gswe_moment_get_timestamp(GSWE_MOMENT(priv->chart));
        gswe_timestamp_set_gregorian_full(
                timestamp,
                edit_data->year, edit_data->month, edit_data->day,
                edit_data->hour, edit_data->minute, edit_data->second, 0,
                edit_data->timezone,
                NULL
            );
    }

    if (set_everything) {
        ag_chart_set_name(priv->chart, edit_data->name);
        ag_chart_set_country(priv->chart, edit_data->country);
        ag_chart_set_city(priv->chart, edit_data->city);
        ag_chart_set_note(priv->chart, edit_data->note);
    }

    ag_db_save_data_free(edit_data);
}

static void
ag_window_export_svg(AgWindow *window, GError **err)
{
    gchar           *name;
    gchar           *file_name;
    GtkWidget       *fs;
    gint            response;
    AgWindowPrivate *priv = ag_window_get_instance_private(window);

    ag_window_recalculate_chart(window, TRUE);

    // We should never enter here, but who knows...
    if (priv->chart == NULL) {
        ag_app_message_dialog(
                GTK_WIDGET(window),
                GTK_MESSAGE_ERROR,
                _("Chart cannot be calculated.")
            );
        g_set_error(
                err,
                AG_WINDOW_ERROR, AG_WINDOW_ERROR_EMPTY_CHART,
                "Chart is empty"
            );

        return;
    }

    name = ag_chart_get_name(priv->chart);

    if ((name == NULL) || (*name == 0)) {
        g_free(name);

        ag_app_message_dialog(
                GTK_WIDGET(window),
                GTK_MESSAGE_ERROR,
                _("You must enter a name before saving a chart.")
            );
        g_set_error(
                err,
                AG_WINDOW_ERROR, AG_WINDOW_ERROR_NO_NAME,
                "No name specified"
            );

        return;
    }

    file_name = g_strdup_printf("%s.svg", name);
    g_free(name);

    fs = gtk_file_chooser_dialog_new(_("Export Chart as SVG"),
                                     GTK_WINDOW(window),
                                     GTK_FILE_CHOOSER_ACTION_SAVE,
                                     _("_Cancel"), GTK_RESPONSE_CANCEL,
                                     _("_Save"), GTK_RESPONSE_ACCEPT,
                                     NULL);
    gtk_dialog_set_default_response(GTK_DIALOG(fs), GTK_RESPONSE_ACCEPT);
    gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(fs), FALSE);
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(fs), TRUE);
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(fs), file_name);
    g_free(file_name);

    response = gtk_dialog_run(GTK_DIALOG(fs));
    gtk_widget_hide(fs);

    if (response == GTK_RESPONSE_ACCEPT) {
        GFile *file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(fs));

        ag_chart_export_svg_to_file(priv->chart, file, err);
    }

    gtk_widget_destroy(fs);
}

static void
ag_window_export_svg_action(GSimpleAction *action,
                            GVariant      *parameter,
                            gpointer      user_data)
{
    AgWindow *window = AG_WINDOW(user_data);
    GError *err = NULL;

    ag_window_export_svg(window, &err);

    if (err) {
        ag_app_message_dialog(
                GTK_WIDGET(window),
                GTK_MESSAGE_ERROR,
                "%s",
                err->message
            );
    }
}

static void
ag_window_export(AgWindow *window, GError **err)
{
    gchar           *name;
    gchar           *file_name;
    GtkWidget       *fs;
    gint            response;
    AgWindowPrivate *priv = ag_window_get_instance_private(window);

    ag_window_recalculate_chart(window, FALSE);

    // We should never enter here, but who knows...
    if (priv->chart == NULL) {
        ag_app_message_dialog(
                GTK_WIDGET(window),
                GTK_MESSAGE_ERROR,
                _("Chart cannot be calculated.")
            );
        g_set_error(
                err,
                AG_WINDOW_ERROR, AG_WINDOW_ERROR_EMPTY_CHART,
                "Chart is empty"
            );

        return;
    }

    name = ag_chart_get_name(priv->chart);

    if ((name == NULL) || (*name == 0)) {
        g_free(name);

        ag_app_message_dialog(
                GTK_WIDGET(window),
                GTK_MESSAGE_ERROR,
                _("You must enter a name before saving a chart.")
            );
        g_set_error(
                err,
                AG_WINDOW_ERROR, AG_WINDOW_ERROR_NO_NAME,
                "No name specified"
            );

        return;
    }

    file_name = g_strdup_printf("%s.agc", name);
    g_free(name);

    fs = gtk_file_chooser_dialog_new(_("Export Chart"),
                                     GTK_WINDOW(window),
                                     GTK_FILE_CHOOSER_ACTION_SAVE,
                                     _("_Cancel"), GTK_RESPONSE_CANCEL,
                                     _("_Save"), GTK_RESPONSE_ACCEPT,
                                     NULL);
    gtk_dialog_set_default_response(GTK_DIALOG(fs), GTK_RESPONSE_ACCEPT);
    gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(fs), FALSE);
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(fs), TRUE);
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(fs), file_name);
    g_free(file_name);

    response = gtk_dialog_run(GTK_DIALOG(fs));
    gtk_widget_hide(fs);

    if (response == GTK_RESPONSE_ACCEPT) {
        GFile *file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(fs));

        ag_chart_save_to_file(priv->chart, file, err);
    }

    gtk_widget_destroy(fs);
}

static void
ag_window_export_action(GSimpleAction *action,
                        GVariant      *parameter,
                        gpointer      user_data)
{
    AgWindow *window = AG_WINDOW(user_data);
    GError   *err    = NULL;

    ag_window_recalculate_chart(window, TRUE);
    ag_window_export(window, &err);

    if (err) {
        ag_app_message_dialog(
                GTK_WIDGET(window),
                GTK_MESSAGE_ERROR,
                "%s", err->message
            );
    }
}

gboolean
ag_window_can_close(AgWindow *window, gboolean display_dialog)
{
    AgWindowPrivate *priv      = ag_window_get_instance_private(window);
    gint            db_id      = (priv->saved_data)
            ? priv->saved_data->db_id
            : -1;
    AgDbSave        *save_data = NULL;
    AgDb            *db        = ag_db_get();
    GError          *err       = NULL;
    gboolean        ret        = TRUE;

    if (priv->chart) {
        ag_window_recalculate_chart(window, TRUE);
        save_data = ag_chart_get_db_save(priv->chart, db_id);

        if (
                    !ag_db_save_identical(priv->saved_data, save_data, FALSE)
                    || !(priv->saved_data)
                    || (priv->saved_data->db_id == -1)
                ) {
            g_debug("Save is needed!");

            if (display_dialog) {
                gint response;

                response = ag_app_buttoned_dialog(
                        GTK_WIDGET(window),
                        GTK_MESSAGE_QUESTION,
                        _("Chart is not saved. Do you want to save it?"),
                        _("Save and close"), GTK_RESPONSE_YES,
                        _("Close without saving"), GTK_RESPONSE_NO,
                        _("Return to chart"), GTK_RESPONSE_CANCEL,
                        NULL
                    );

                switch (response) {
                    case GTK_RESPONSE_YES:
                        if (!ag_db_save_chart(db, save_data, &err)) {
                            ag_app_message_dialog(
                                    GTK_WIDGET(window),
                                    GTK_MESSAGE_ERROR,
                                    "Unable to save chart: %s",
                                    err->message
                                );

                            ret = FALSE;
                        } else {
                            ret = TRUE;
                        }

                        break;

                    case GTK_RESPONSE_NO:
                        ret = TRUE;

                        break;

                    default:
                        ret = FALSE;

                        break;
                }
            } else {
                ret = FALSE;
            }
        }
    }

    ag_db_save_data_free(save_data);

    return ret;
}

static void
ag_window_save_action(GSimpleAction *action,
                      GVariant      *parameter,
                      gpointer      user_data)
{
    AgWindow        *window = AG_WINDOW(user_data);
    AgWindowPrivate *priv   = ag_window_get_instance_private(window);
    AgDb            *db     = ag_db_get();
    GError          *err    = NULL;
    gint            old_id;
    AgDbSave        *save_data;

    ag_window_recalculate_chart(window, TRUE);

    if (!ag_window_can_close(window, FALSE)) {
        old_id    = (priv->saved_data) ? priv->saved_data->db_id : -1;
        save_data = ag_chart_get_db_save(priv->chart, old_id);

        if (!ag_db_save_chart(db, save_data, &err)) {
            ag_app_message_dialog(
                    GTK_WIDGET(window),
                    GTK_MESSAGE_ERROR,
                    _("Unable to save: %s"),
                    err->message
                );
        }

        ag_db_save_data_free(priv->saved_data);
        priv->saved_data = save_data;
    }
}

static void
ag_window_close_action(GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer      user_data)
{
    AgWindow *window = AG_WINDOW(user_data);

    if (ag_window_can_close(window, TRUE)) {
        gtk_widget_destroy(GTK_WIDGET(window));
    }
}

static gboolean
ag_window_delete_event_callback(AgWindow *window,
                                GdkEvent *event,
                                gpointer user_data)
{
    return (!ag_window_can_close(window, TRUE));
}

static void
ag_window_tab_changed_cb(GtkStack *stack, GParamSpec *pspec, AgWindow *window)
{
    GtkWidget       *active_tab;
    const gchar     *active_tab_name = gtk_stack_get_visible_child_name(stack);
    AgWindowPrivate *priv            = ag_window_get_instance_private(window);

    g_debug("Active tab changed: %s", active_tab_name);

    if (active_tab_name == NULL) {
        return;
    }

    active_tab = gtk_stack_get_visible_child(stack);

    if (strcmp("chart", active_tab_name) == 0) {
        gtk_widget_set_size_request(active_tab, 600, 600);
    }

    if (strcmp("list", active_tab_name) == 0) {
        gtk_stack_set_visible_child_name(
                GTK_STACK(priv->menubutton_stack),
                "list"
            );
        gtk_stack_set_visible_child_name(
                GTK_STACK(priv->new_back_stack),
                "new"
            );
    } else {
        gtk_stack_set_visible_child_name(
                GTK_STACK(priv->menubutton_stack),
                "chart"
            );
        gtk_stack_set_visible_child_name(
                GTK_STACK(priv->new_back_stack),
                "back"
            );

        // Note that priv->current_tab is actually the previously selected tab,
        // not the real active one!
        if (priv->current_tab == priv->tab_edit) {
            ag_window_recalculate_chart(window, FALSE);
        }
    }

    priv->current_tab = active_tab;
}

static void
ag_window_change_tab_action(GSimpleAction *action,
                            GVariant      *parameter,
                            gpointer      user_data)
{
    AgWindow        *window     = AG_WINDOW(user_data);
    const gchar     *target_tab = g_variant_get_string(parameter, NULL);
    AgWindowPrivate *priv       = ag_window_get_instance_private(window);

    gtk_stack_set_visible_child_name(GTK_STACK(priv->stack), target_tab);
    g_action_change_state(G_ACTION(action), parameter);
}

static gboolean
ag_window_set_default_house_system(GtkTreeModel *model,
                            GtkTreePath  *path,
                            GtkTreeIter  *iter,
                            AgWindow     *window)
{
    GsweHouseSystem row_house_system;
    AgWindowPrivate *priv          = ag_window_get_instance_private(window);
    AgSettings      *settings      = ag_settings_get();
    GSettings       *main_settings = ag_settings_peek_main_settings(settings);
    GsweHouseSystem house_system   = g_settings_get_enum(
            main_settings,
            "default-house-system"
        );

    g_object_unref(settings);

    gtk_tree_model_get(
            GTK_TREE_MODEL(priv->house_system_model),
            iter,
            0, &row_house_system,
            -1
        );

    if (house_system == row_house_system) {
        gtk_combo_box_set_active_iter(GTK_COMBO_BOX(priv->house_system), iter);

        return TRUE;
    }

    return FALSE;
}

static void
ag_window_new_chart_action(GSimpleAction *action,
                           GVariant      *parameter,
                           gpointer      user_data)
{
    AgWindow        *window = AG_WINDOW(user_data);
    AgWindowPrivate *priv   = ag_window_get_instance_private(window);

    /* Empty edit tab values */
    gtk_entry_set_text(GTK_ENTRY(priv->name), "");
    //gtk_entry_set_text(GTK_ENTRY(priv->country), "");
    //gtk_entry_set_text(GTK_ENTRY(priv->city), "");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(priv->year), (gdouble)1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(priv->month), (gdouble)1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(priv->day), (gdouble)1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(priv->hour), (gdouble)1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(priv->minute), (gdouble)1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(priv->second), (gdouble)1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(priv->timezone), 0.0);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->north_lat), TRUE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->east_long), TRUE);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(priv->longitude), 0.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(priv->latitude), 0.0);
    gtk_tree_model_foreach(
            GTK_TREE_MODEL(priv->house_system_model),
            (GtkTreeModelForeachFunc)ag_window_set_default_house_system,
            window
        );
    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(priv->note_buffer), "", 0);

    if (priv->chart) {
        ag_app_message_dialog(
                GTK_WIDGET(window),
                GTK_MESSAGE_ERROR,
                "This window already has a chart. " \
                "This should not happen, " \
                "please consider issuing a bug report!"
            );

        gtk_stack_set_visible_child_name(GTK_STACK(priv->stack), "chart");

        return;
    }

    gtk_stack_set_visible_child_name(GTK_STACK(priv->stack), "edit");
}

static void
ag_window_back_action(GSimpleAction *action,
                      GVariant      *parameter,
                      gpointer      user_data)
{
    AgWindow        *window = AG_WINDOW(user_data);
    AgWindowPrivate *priv   = ag_window_get_instance_private(window);

    g_debug("Back button pressed");

    if (ag_window_can_close(window, TRUE)) {
        g_clear_object(&(priv->chart));
        ag_db_save_data_free(priv->saved_data);
        priv->saved_data = NULL;

        ag_window_load_chart_list(window);
        gtk_stack_set_visible_child_name(GTK_STACK(priv->stack), "list");
        gtk_header_bar_set_subtitle(GTK_HEADER_BAR(priv->header_bar), NULL);
    }
}

static void
ag_window_refresh_action(GSimpleAction *action,
                         GVariant      *parameter,
                         gpointer      user_data)
{
    ag_window_load_chart_list(AG_WINDOW(user_data));
}

static void
ag_window_selection_mode_action(GSimpleAction *action,
                                GVariant      *parameter,
                                gpointer      user_data)
{
    GVariant        *state;
    gboolean        new_state;
    GtkStyleContext *style;
    AgWindow        *window = AG_WINDOW(user_data);
    AgWindowPrivate *priv = ag_window_get_instance_private(window);

    state = g_action_get_state(G_ACTION(action));
    new_state = !g_variant_get_boolean(state);
    g_action_change_state(G_ACTION(action), g_variant_new_boolean(new_state));
    g_variant_unref(state);

    style = gtk_widget_get_style_context(priv->header_bar);

    if (new_state) {
        gtk_header_bar_set_show_close_button(
                GTK_HEADER_BAR(priv->header_bar),
                FALSE
            );
        gtk_style_context_add_class(style, "selection-mode");
        gd_main_view_set_selection_mode(GD_MAIN_VIEW(priv->tab_list), TRUE);
        gtk_widget_hide(priv->new_back_stack);
        gtk_stack_set_visible_child_name(
                GTK_STACK(priv->menubutton_stack),
                "selection"
            );
    } else {
        gtk_header_bar_set_show_close_button(
                GTK_HEADER_BAR(priv->header_bar),
                TRUE
            );
        gtk_style_context_remove_class(style, "selection-mode");
        gd_main_view_set_selection_mode(GD_MAIN_VIEW(priv->tab_list), FALSE);
        gtk_widget_show_all(priv->new_back_stack);
        gtk_stack_set_visible_child_name(
                GTK_STACK(priv->menubutton_stack),
                "list"
            );
    }
}

static void
ag_window_delete_action(GSimpleAction *action,
                        GVariant      *parameter,
                        gpointer      user_data)
{
    GList           *selection,
                    *item;
    GtkTreeModel    *model;
    AgWindow        *window = AG_WINDOW(user_data);
    AgWindowPrivate *priv   = ag_window_get_instance_private(window);
    AgDb            *db     = ag_db_get();

    selection = gd_main_view_get_selection(GD_MAIN_VIEW(priv->tab_list));
    model     = gd_main_view_get_model(GD_MAIN_VIEW(priv->tab_list));

    for (item = selection; item; item = g_list_next(item)) {
        GtkTreePath *path = item->data;
        GtkTreeIter iter;
        gchar       *id_str;
        gint        id;
        GError      *err = NULL;

        gtk_tree_model_get_iter(model, &iter, path);
        gtk_tree_model_get(
                model, &iter,
                GD_MAIN_COLUMN_ID, &id_str,
                -1
            );
        id = atoi(id_str);
        g_free(id_str);

        if (!ag_db_delete_chart(db, id, &err)) {
            ag_app_message_dialog(
                    GTK_WIDGET(window),
                    GTK_MESSAGE_ERROR,
                    "Unable to delete chart: %s",
                    (err && err->message)
                        ? err->message
                        : "No reason"
                );
        }
    }

    g_action_group_activate_action(G_ACTION_GROUP(window), "selection", NULL);
    g_action_group_activate_action(G_ACTION_GROUP(window), "refresh", NULL);
}

static void
ag_window_js_callback(GObject *object, GAsyncResult *res, gpointer user_data)
{
    WebKitJavascriptResult *js_result;
    GError                 *err = NULL;

    if ((js_result = webkit_web_view_run_javascript_finish(
                WEBKIT_WEB_VIEW(object),
                res,
                &err
            )) != NULL) {
        webkit_javascript_result_unref(js_result);
    }
}

static void
ag_window_connection_action(GSimpleAction *action,
                            GVariant      *parameter,
                            gpointer      user_data)
{
    GVariant        *current_state;
    const gchar     *state;
    gchar           *js_code = NULL;
    AgWindowPrivate *priv    = ag_window_get_instance_private(
            AG_WINDOW(user_data)
        );
    static gchar *js         = "aspects = document.getElementById('aspects');\n"   \
                               "antiscia = document.getElementById('antiscia');\n" \
                               "aspects.setAttribute('visibility', '%s');\n"       \
                               "antiscia.setAttribute('visibility', '%s');\n";

    current_state = g_action_get_state(G_ACTION(action));

    if (g_variant_equal(current_state, parameter)) {
        return;
    }

    g_action_change_state(G_ACTION(action), parameter);

    state = g_variant_get_string(parameter, NULL);

    if (strcmp("aspects", state) == 0) {
        g_debug("Switching to aspects");
        js_code = g_strdup_printf(js, "visible", "hidden");
    } else if (strcmp("antiscia", state) == 0) {
        g_debug("Switching to antiscia");
        js_code = g_strdup_printf(js, "hidden", "visible");
    } else {
        g_warning("Connection type '%s' is invalid", state);
    }

    if (js_code) {
        webkit_web_view_run_javascript(
                WEBKIT_WEB_VIEW(priv->chart_web_view),
                js_code,
                NULL,
                ag_window_js_callback,
                NULL
            );
        g_free(js_code);
    }
}

static GActionEntry win_entries[] = {
    { "close",      ag_window_close_action,          NULL, NULL,        NULL },
    { "save",       ag_window_save_action,           NULL, NULL,        NULL },
    { "export",     ag_window_export_action,         NULL, NULL,        NULL },
    { "export-svg", ag_window_export_svg_action,     NULL, NULL,        NULL },
    { "view-menu",  ag_window_view_menu_action,      NULL, "false",     NULL },
    { "gear-menu",  ag_window_gear_menu_action,      NULL, "false",     NULL },
    { "change-tab", ag_window_change_tab_action,     "s",  "'edit'",    NULL },
    { "new-chart",  ag_window_new_chart_action,      NULL, NULL,        NULL },
    { "back",       ag_window_back_action,           NULL, NULL,        NULL },
    { "refresh",    ag_window_refresh_action,        NULL, NULL,        NULL },
    { "selection",  ag_window_selection_mode_action, NULL, "false",     NULL },
    { "delete",     ag_window_delete_action,         NULL, NULL,        NULL },
    { "connection", ag_window_connection_action,     "s",  "'aspects'", NULL },
};

static void
ag_window_display_changed(GSettings *settings, gchar *key, AgWindow *window)
{
    AgWindowPrivate *priv = ag_window_get_instance_private(window);

    /* The planet symbols are redrawn only if aspect_table_populated is
     * set to FALSE */
    if (g_str_equal("planets-char", key)) {
        priv->aspect_table_populated = FALSE;
    }

    ag_window_redraw_aspect_table(window);
}

static void
ag_window_add_house_system(GsweHouseSystemInfo *house_system_info,
                           AgWindowPrivate *priv)
{
    GtkTreeIter iter;

    gtk_list_store_append(priv->house_system_model, &iter);
    gtk_list_store_set(
            priv->house_system_model, &iter,
            0, gswe_house_system_info_get_house_system(house_system_info),
            1, gswe_house_system_info_get_name(house_system_info),
            -1
        );
}

static void
ag_window_list_item_activated_cb(GdMainView        *view,
                                 const gchar       *id,
                                 const GtkTreePath *path,
                                 AgWindow          *window)
{
    guint           row_id = atoi(id);
    AgWindowPrivate *priv  = ag_window_get_instance_private(window);
    AgDb            *db    = ag_db_get();
    GError          *err   = NULL;

    if (priv->saved_data != NULL) {
        ag_app_message_dialog(
                GTK_WIDGET(window),
                GTK_MESSAGE_ERROR,
                "Window chart is not saved. " \
                "This is a bug, it should not happen here. " \
                "Please consider opening a bug report!"
            );

        ag_window_change_tab(window, "chart");

        return;
    }

    g_debug("Loading chart with ID %d", row_id);

    if ((priv->saved_data = ag_db_get_chart_data_by_id(
                 db,
                 row_id,
                 &err)) == NULL) {
        ag_app_message_dialog(
                GTK_WIDGET(window),
                GTK_MESSAGE_ERROR,
                "Could not open chart."
            );

        return;
    }

    if (priv->chart) {
        g_clear_object(&(priv->chart));
    }

    if ((priv->chart = ag_chart_new_from_db_save(
                 priv->saved_data,
                 &err
            )) == NULL) {
        ag_app_message_dialog(
                GTK_WIDGET(window),
                GTK_MESSAGE_ERROR,
                "Error: %s",
                err->message
            );
        ag_db_save_data_free(priv->saved_data);
        priv->saved_data = NULL;

        return;
    }

    ag_window_update_from_chart(window);

    ag_window_change_tab(window, "chart");
}

static void
ag_window_list_selection_changed_cb(GdMainView *view, AgWindow *window)
{
    GList           *selection;
    guint           count;
    AgWindowPrivate *priv = ag_window_get_instance_private(window);

    selection = gd_main_view_get_selection(view);

    if ((count = g_list_length(selection)) > 0) {
        gtk_revealer_set_reveal_child(
                GTK_REVEALER(priv->selection_toolbar),
                TRUE
            );
    } else {
        gtk_revealer_set_reveal_child(
                GTK_REVEALER(priv->selection_toolbar),
                FALSE
            );
    }

    // Here it is possible to set button sensitivity later
}

static void
ag_window_init(AgWindow *window)
{
    GtkAccelGroup   *accel_group;
    GSettings       *main_settings;
    GList           *house_system_list;
    GtkCellRenderer *house_system_renderer;
    AgWindowPrivate *priv = ag_window_get_instance_private(window);

    gtk_widget_init_template(GTK_WIDGET(window));

    priv->settings = ag_settings_get();
    main_settings  = ag_settings_peek_main_settings(priv->settings);

    g_signal_connect(
            G_OBJECT(main_settings),
            "changed::planets-char",
            G_CALLBACK(ag_window_display_changed),
            window
        );
    g_signal_connect(
            G_OBJECT(main_settings),
            "changed::aspects-char",
            G_CALLBACK(ag_window_display_changed),
            window
        );

    house_system_list = gswe_all_house_systems();
    g_list_foreach(house_system_list, (GFunc)ag_window_add_house_system, priv);
    g_list_free(house_system_list);
    gtk_tree_model_foreach(
            GTK_TREE_MODEL(priv->house_system_model),
            (GtkTreeModelForeachFunc)ag_window_set_default_house_system,
            window
        );

    house_system_renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(
            GTK_CELL_LAYOUT(priv->house_system),
            house_system_renderer,
            TRUE
        );
    gtk_cell_layout_set_attributes(
            GTK_CELL_LAYOUT(priv->house_system),
            house_system_renderer,
            "text", 1,
            NULL
        );

    priv->tab_list = GTK_WIDGET(gd_main_view_new(GD_MAIN_VIEW_ICON));
    gtk_stack_add_titled(
            GTK_STACK(priv->stack),
            priv->tab_list,
            "list",
            "Chart list"
        );

    gd_main_view_set_selection_mode(GD_MAIN_VIEW(priv->tab_list), FALSE);
    gd_main_view_set_model(
            GD_MAIN_VIEW(priv->tab_list),
            GTK_TREE_MODEL(priv->db_chart_data)
        );
    g_signal_connect(
            priv->tab_list,
            "item-activated",
            G_CALLBACK(ag_window_list_item_activated_cb),
            window
        );
    g_signal_connect(
            priv->tab_list,
            "view-selection-changed",
            G_CALLBACK(ag_window_list_selection_changed_cb),
            window
        );

    gtk_stack_set_visible_child_name(GTK_STACK(priv->stack), "list");
    priv->current_tab = priv->tab_list;

    g_object_set(
            priv->year_adjust,
            "lower", (gdouble)G_MININT,
            "upper", (gdouble)G_MAXINT,
            NULL
        );

    priv->chart    = NULL;

    g_action_map_add_action_entries(
            G_ACTION_MAP(window),
            win_entries,
            G_N_ELEMENTS(win_entries),
            window
        );

    accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
}

static void
ag_window_dispose(GObject *gobject)
{
    AgWindowPrivate *priv = ag_window_get_instance_private(AG_WINDOW(gobject));

    g_clear_object(&priv->settings);

    G_OBJECT_CLASS(ag_window_parent_class)->dispose(gobject);
}

static void
ag_window_name_changed_cb(GtkEntry *name_entry, AgWindow *window)
{
    const gchar     *name;
    AgWindowPrivate *priv = ag_window_get_instance_private(window);

    name = gtk_entry_get_text(name_entry);

    gtk_header_bar_set_subtitle(GTK_HEADER_BAR(priv->header_bar), name);
}

static void
ag_window_class_init(AgWindowClass *klass)
{
    GObjectClass   *gobject_class = G_OBJECT_CLASS(klass);
    GtkWidgetClass *widget_class  = GTK_WIDGET_CLASS(klass);

    gobject_class->dispose = ag_window_dispose;

    gtk_widget_class_set_template_from_resource(
            widget_class,
            "/eu/polonkai/gergely/Astrognome/ui/ag-window.ui"
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            header_bar
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            new_back_stack
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            menubutton_stack
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            db_chart_data
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            tab_edit
        );
    gtk_widget_class_bind_template_child_private(widget_class, AgWindow, name);
    gtk_widget_class_bind_template_child_private(widget_class, AgWindow, year);
    gtk_widget_class_bind_template_child_private(widget_class, AgWindow, month);
    gtk_widget_class_bind_template_child_private(widget_class, AgWindow, day);
    gtk_widget_class_bind_template_child_private(widget_class, AgWindow, hour);
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            minute
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            second
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            timezone
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            north_lat
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            south_lat
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            east_long
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            west_long
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            latitude
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            longitude
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            house_system_model
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            house_system
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            tab_chart
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            aspect_table
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            year_adjust
        );
    gtk_widget_class_bind_template_child_private(widget_class, AgWindow, stack);
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            note_buffer
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            selection_toolbar
        );

    gtk_widget_class_bind_template_callback(
            widget_class,
            ag_window_delete_event_callback
       );
    gtk_widget_class_bind_template_callback(
            widget_class,
            ag_window_tab_changed_cb
        );
    gtk_widget_class_bind_template_callback(
            widget_class,
            ag_window_name_changed_cb
        );
}

gboolean
ag_window_chart_context_cb(WebKitWebView       *web_view,
                           GtkWidget           *default_menu,
                           WebKitHitTestResult *hit_test_result,
                           gboolean            triggered_with_keyboard,
                           gpointer user_data)
{
    return TRUE;
}

static gboolean
ag_window_configure_event_cb(GtkWidget         *widget,
                             GdkEventConfigure *event,
                             gpointer          user_data)
{
    AgWindow        *window = AG_WINDOW(widget);
    AgWindowPrivate *priv   = ag_window_get_instance_private(window);

    ag_window_settings_save(
            GTK_WINDOW(window),
            ag_settings_peek_window_settings(priv->settings)
        );

    return FALSE;
}

GtkWidget *
ag_window_new(AgApp *app, WebKitUserContentManager *manager)
{
    AgWindow        *window = g_object_new(AG_TYPE_WINDOW, NULL);
    AgWindowPrivate *priv   = ag_window_get_instance_private(window);

    priv->chart_web_view = webkit_web_view_new_with_user_content_manager(
            manager
        );
    gtk_box_pack_end(
            GTK_BOX(priv->tab_chart),
            priv->chart_web_view,
            TRUE, TRUE, 0
        );

    // TODO: translate this error message!
    webkit_web_view_load_html(
            WEBKIT_WEB_VIEW(priv->chart_web_view),
            "<html>" \
                "<head>" \
                    "<title>No Chart</title>" \
                "</head>" \
                "<body>" \
                    "<h1>No Chart</h1>" \
                    "<p>No chart is loaded. Create one on the " \
                    "edit view, or open one from the application menu!</p>" \
                "</body>" \
            "</html>",
            NULL);

    gtk_window_set_application(GTK_WINDOW(window), GTK_APPLICATION(app));

    gtk_window_set_icon_name(GTK_WINDOW(window), "astrognome");
    g_signal_connect(
             window,
             "configure-event",
             G_CALLBACK(ag_window_configure_event_cb),
             NULL
         );

    ag_window_settings_restore(
            GTK_WINDOW(window),
            ag_settings_peek_window_settings(priv->settings)
        );

    return GTK_WIDGET(window);
}

void
ag_window_set_chart(AgWindow *window, AgChart *chart)
{
    AgWindowPrivate *priv = ag_window_get_instance_private(window);

    if (priv->chart != NULL) {
        g_signal_handlers_disconnect_by_func(
                priv->chart,
                ag_window_chart_changed,
                window
            );
        g_clear_object(&(priv->chart));
    }

    ag_db_save_data_free(priv->saved_data);

    priv->chart = chart;
    g_signal_connect(
            priv->chart,
            "changed",
            G_CALLBACK(ag_window_chart_changed),
            window
        );
    g_object_ref(chart);
    priv->saved_data = ag_chart_get_db_save(chart, -1);
}

AgChart *
ag_window_get_chart(AgWindow *window)
{
    AgWindowPrivate *priv = ag_window_get_instance_private(window);

    return priv->chart;
}

void
ag_window_settings_restore(GtkWindow *window, GSettings *settings)
{
    gint      width,
              height;
    gboolean  maximized;
    GdkScreen *screen;

    width     = g_settings_get_int(settings, "width");
    height    = g_settings_get_int(settings, "height");
    maximized = g_settings_get_boolean(settings, "maximized");

    if ((width > 1) && (height > 1)) {
        gint max_width,
             max_height;

        screen     = gtk_widget_get_screen(GTK_WIDGET(window));
        max_width  = gdk_screen_get_width(screen);
        max_height = gdk_screen_get_height(screen);

        width  = CLAMP(width, 0, max_width);
        height = CLAMP(height, 0, max_height);

        gtk_window_set_default_size(window, width, height);
    }

    if (maximized) {
        gtk_window_maximize(window);
    }
}

void
ag_window_settings_save(GtkWindow *window, GSettings *settings)
{
    GdkWindowState state;
    gint           width,
                   height;
    gboolean       maximized;

    state     = gdk_window_get_state(gtk_widget_get_window(GTK_WIDGET(window)));
    maximized = (
                (state & GDK_WINDOW_STATE_MAXIMIZED)
                == GDK_WINDOW_STATE_MAXIMIZED
            );

    g_settings_set_boolean(settings, "maximized", maximized);

    gtk_window_get_size(window, &width, &height);
    g_settings_set_int(settings, "width", width);
    g_settings_set_int(settings, "height", height);
}

void
ag_window_change_tab(AgWindow *window, const gchar *tab_name)
{
    AgWindowPrivate *priv = ag_window_get_instance_private(window);

    gtk_stack_set_visible_child_name(GTK_STACK(priv->stack), tab_name);
    g_action_change_state(
            g_action_map_lookup_action(G_ACTION_MAP(window), "change-tab"),
            g_variant_new_string(tab_name)
        );
}

static void
ag_window_add_chart_to_list(AgDbSave *save_data, AgWindow *window)
{
    GtkTreeIter     iter;
    AgWindowPrivate *priv = ag_window_get_instance_private(window);
    gchar           *id   = g_strdup_printf("%d", save_data->db_id);

    gtk_list_store_append(priv->db_chart_data, &iter);
    gtk_list_store_set(
            priv->db_chart_data, &iter,
            0, id,              /* ID             */
            1, NULL,            /* URI            */
            2, save_data->name, /* Primary text   */
            3, NULL,            /* Secondary text */
            4, NULL,            /* Icon           */
            5, 0,               /* mtime          */
            6, FALSE,           /* Selected       */
            7, 0,               /* Pulse          */
            -1
        );
    g_free(id);
}

static void
ag_window_clear_chart_list(AgWindow *window)
{
    AgWindowPrivate *priv = ag_window_get_instance_private(window);

    gtk_list_store_clear(priv->db_chart_data);
}

gboolean
ag_window_load_chart_list(AgWindow *window)
{
    AgDb   *db         = ag_db_get();
    GError *err        = NULL;
    GList  *chart_list = ag_db_get_chart_list(db, &err);

    ag_window_clear_chart_list(window);
    /* With only a few charts, this should be fine. Maybe implementing lazy
     * loading would be a better idea. See:
     * http://blogs.gnome.org/ebassi/documentation/lazy-loading/
     */
    g_list_foreach(chart_list, (GFunc)ag_window_add_chart_to_list, window);

    return TRUE;
}
