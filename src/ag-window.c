/* ag-window.c - Main window management for Astrognome
 *
 * Copyright (C) 2014 Polonkai Gergely
 *
 * Astrognome is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 3 of the License,
 * or (at your option) any later version.
 *
 * Astrognome is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; if not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <math.h>
#include <string.h>
#include <glib/gi18n.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <webkit2/webkit2.h>
#include <gtk/gtk.h>

#include <swe-glib.h>

#include "config.h"
#include "ag-app.h"
#include "ag-window.h"
#include "ag-chart.h"
#include "ag-settings.h"
#include "ag-db.h"
#include "ag-display-theme.h"
#include "ag-icon-view.h"

struct _AgWindowPrivate {
    GtkWidget     *header_bar;
    GtkWidget     *menubutton_stack;
    GtkWidget     *new_back_stack;
    GtkWidget     *selection_toolbar;
    GtkWidget     *stack;
    GtkWidget     *name;
    GtkWidget     *country;
    GtkWidget     *city;
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
    GtkWidget     *display_theme;
    GtkWidget     *toolbar_aspect;

    GtkWidget     *tab_list;
    GtkWidget     *tab_chart;
    GtkWidget     *tab_edit;
    GtkWidget     *current_tab;

    GtkWidget     *aspect_table;
    GtkWidget     *chart_web_view;
    GtkWidget     *points_eq;
    GtkAdjustment *year_adjust;

    GtkWidget     *chart_list;
    AgSettings    *settings;
    AgChart       *chart;
    gboolean      aspect_table_populated;
    GtkTextBuffer *note_buffer;
    GtkListStore  *house_system_model;
    AgDbChartSave      *saved_data;
    GtkEntryCompletion *country_comp;
    GtkEntryCompletion *city_comp;
    gchar              *selected_country;
    gchar              *selected_city;
    GList              *style_sheets;
    AgDisplayTheme     *theme;
    GtkListStore       *display_theme_model;
    gulong             chart_changed_handler;
};

struct cc_search {
    const gchar  *target;
    GtkTreeIter  *ret_iter;
    gchar        *ret_code;
};

enum {
    PREVIEW_STATE_STARTED,
    PREVIEW_STATE_LOADING,
    PREVIEW_STATE_COMPLETE,
    PREVIEW_STATE_FINISHED
};

typedef struct {
    guint      load_state;
    guint      load_id;
    AgIconView *icon_view;
    gint       n_items;
    gint       n_loaded;
    GList      *items;
} LoadIdleData;

enum {
    PROP_0,
    PROP_CHART,
    PROP_COUNT
};

G_DEFINE_QUARK(ag_window_error_quark, ag_window_error);

G_DEFINE_TYPE_WITH_PRIVATE(AgWindow, ag_window, GTK_TYPE_APPLICATION_WINDOW);

static GParamSpec *properties[PROP_COUNT];

#define GET_PRIV(o) AgWindowPrivate *priv = ag_window_get_instance_private((o))

static void
ag_window_gear_menu_action(GSimpleAction *action,
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
    GET_PRIV(window);

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

static void
ag_window_set_element_point(AgWindow    *window,
                            GsweElement element,
                            guint       left,
                            guint       top)
{
    guint            points;
    GtkWidget        *label;
    gchar            *points_string;
    GET_PRIV(window);

    points = gswe_moment_get_element_points(
            GSWE_MOMENT(priv->chart),
            element
        );

    if ((label = gtk_grid_get_child_at(
                GTK_GRID(priv->points_eq),
                left, top
            )) == NULL) {
        label = gtk_label_new("");
        gtk_grid_attach(GTK_GRID(priv->points_eq), label, left, top, 1, 1);
        gtk_widget_show(label);
    }

    points_string = g_strdup_printf("%d", points);
    gtk_label_set_text(GTK_LABEL(label), points_string);
    g_free(points_string);
}

static void
ag_window_set_quality_point(AgWindow    *window,
                            GsweQuality quality,
                            guint       left,
                            guint       top)
{
    guint            points;
    GtkWidget        *label;
    gchar            *points_string;
    GET_PRIV(window);

    points = gswe_moment_get_quality_points(
            GSWE_MOMENT(priv->chart),
            quality
        );

    if ((label = gtk_grid_get_child_at(
                GTK_GRID(priv->points_eq),
                left, top
            )) == NULL) {
        label = gtk_label_new("");
        gtk_grid_attach(GTK_GRID(priv->points_eq), label, left, top, 1, 1);
        gtk_widget_show(label);
    }

    points_string = g_strdup_printf("%d", points);
    gtk_label_set_text(GTK_LABEL(label), points_string);
    g_free(points_string);
}

static void
ag_window_redraw_points_table(AgWindow *window)
{
    ag_window_set_element_point(window, GSWE_ELEMENT_FIRE, 4, 1);
    ag_window_set_element_point(window, GSWE_ELEMENT_EARTH, 4, 2);
    ag_window_set_element_point(window, GSWE_ELEMENT_AIR, 4, 3);
    ag_window_set_element_point(window, GSWE_ELEMENT_WATER, 4, 4);

    ag_window_set_quality_point(window, GSWE_QUALITY_CARDINAL, 1, 5);
    ag_window_set_quality_point(window, GSWE_QUALITY_FIX, 2, 5);
    ag_window_set_quality_point(window, GSWE_QUALITY_MUTABLE, 3, 5);
}

/**
 * ag_window_redraw_chart:
 * @window: the #AgWindow to operate on
 *
 * Redraw the chart on the chart view.
 */
void
ag_window_redraw_chart(AgWindow *window)
{
    gsize           length;
    GError          *err         = NULL;
    GET_PRIV(window);
    gchar           *svg_content = ag_chart_create_svg(
            priv->chart,
            &length,
            FALSE,
            NULL,
            0, 0,
            &err
        );

    if (svg_content == NULL) {
        ag_app_message_dialog(
                GTK_WINDOW(window),
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
    ag_window_redraw_points_table(window);
}

static gboolean
ag_window_set_model_house_system(GtkTreeModel *model,
                                 GtkTreePath  *path,
                                 GtkTreeIter  *iter,
                                 AgWindow     *window)
{
    GsweHouseSystem row_house_system;
    GET_PRIV(window);
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
    const gchar     *country,
                    *city;
    GET_PRIV(window);
    GsweTimestamp   *timestamp   = gswe_moment_get_timestamp(
            GSWE_MOMENT(priv->chart)
        );
    GsweCoordinates *coordinates = gswe_moment_get_coordinates(
            GSWE_MOMENT(priv->chart)
        );

    if ((country = ag_chart_get_country(priv->chart)) != NULL) {
        gtk_entry_set_text(
                GTK_ENTRY(priv->country),
                ag_chart_get_country(priv->chart)
            );
    } else {
        gtk_entry_set_text(
                GTK_ENTRY(priv->country),
                ""
            );
    }

    if ((city = ag_chart_get_city(priv->chart)) != NULL) {
        gtk_entry_set_text(
                GTK_ENTRY(priv->city),
                ag_chart_get_city(priv->chart)
            );
    } else {
        gtk_entry_set_text(
                GTK_ENTRY(priv->city),
                ""
            );
    }

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
            (GtkTreeModelForeachFunc)ag_window_set_model_house_system,
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

    gtk_header_bar_set_subtitle(
            GTK_HEADER_BAR(priv->header_bar),
            ag_chart_get_name(priv->chart)
        );

    g_free(coordinates);

    ag_window_redraw_chart(window);
}

static void
ag_window_chart_changed(AgChart *chart, AgWindow *window)
{
    g_debug("Chart changed!");
    ag_window_redraw_chart(window);
}

static void
ag_window_recalculate_chart(AgWindow *window, gboolean set_everything)
{
    AgDbChartSave   *edit_data,
                    *chart_data;
    GET_PRIV(window);
    gboolean        south,
                    west;
    GtkTreeIter     house_system_iter;
    GsweHouseSystem house_system;
    GtkTextIter     start_iter,
                    end_iter;
    GsweTimestamp   *timestamp;
    GtkWidget       *current;
    gint            db_id = (priv->saved_data) ? priv->saved_data->db_id : -1;
    AgSettings      *settings;

    south = gtk_toggle_button_get_active(
            GTK_TOGGLE_BUTTON(priv->south_lat)
        );

    west      = gtk_toggle_button_get_active(
            GTK_TOGGLE_BUTTON(priv->west_long)
        );

    // If the current widget is a spin button, force it to update. This is
    // required when the user enters a new value in a spin button, but doesn't
    // leave the spin entry before switching to the chart tab with an accel.
    current = gtk_window_get_focus(GTK_WINDOW(window));

    if (GTK_IS_SPIN_BUTTON(current)) {
        gtk_spin_button_update(GTK_SPIN_BUTTON(current));
    }

    edit_data = ag_db_chart_save_new(TRUE);

    edit_data->db_id = db_id;

    edit_data->name = g_strdup(gtk_entry_get_text(GTK_ENTRY(priv->name)));
    edit_data->country = g_strdup(gtk_entry_get_text(GTK_ENTRY(priv->country)));
    edit_data->city = g_strdup(gtk_entry_get_text(GTK_ENTRY(priv->city)));
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

    // TODO: This will cause problems with imported charts
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

    gtk_text_buffer_get_bounds(priv->note_buffer, &start_iter, &end_iter);
    edit_data->note = gtk_text_buffer_get_text(
            priv->note_buffer,
            &start_iter, &end_iter,
            TRUE
        );

    chart_data = (priv->chart)
            ? ag_chart_get_db_save(priv->chart)
            : NULL
        ;

    if (ag_db_chart_save_identical(edit_data, chart_data, !set_everything)) {
        g_debug("No redrawing needed");

        ag_db_chart_save_unref(edit_data);
        ag_db_chart_save_unref(chart_data);

        return;
    }

    ag_db_chart_save_unref(chart_data);

    g_debug("Recalculating chart data");

    settings = ag_settings_get();
    house_system = ag_settings_get_house_system(settings);
    g_object_unref(settings);

    // TODO: Set timezone according to the city selected!
    if (priv->chart == NULL) {
        AgChart *chart;

        timestamp = gswe_timestamp_new_from_gregorian_full(
                edit_data->year, edit_data->month, edit_data->day,
                edit_data->hour, edit_data->minute, edit_data->second, 0,
                edit_data->timezone
            );
        chart = ag_chart_new_full(
                timestamp,
                edit_data->longitude, edit_data->latitude, edit_data->altitude,
                house_system
            );
        ag_window_set_chart(window, chart);
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

    ag_db_chart_save_unref(edit_data);
}

static void
ag_window_export_image(AgWindow *window, GError **err)
{
    const gchar     *name;
    GtkWidget       *fs;
    gint            response;
    GError          *local_err = NULL;
    GET_PRIV(window);

    ag_window_recalculate_chart(window, TRUE);

    // We should never enter here, but who knows...
    if (priv->chart == NULL) {
        ag_app_message_dialog(
                GTK_WINDOW(window),
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
        ag_app_message_dialog(
                GTK_WINDOW(window),
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

    fs = gtk_file_chooser_dialog_new(_("Export Chart as Image"),
                                     GTK_WINDOW(window),
                                     GTK_FILE_CHOOSER_ACTION_SAVE,
                                     _("_Cancel"), GTK_RESPONSE_CANCEL,
                                     _("_Save"), GTK_RESPONSE_ACCEPT,
                                     NULL);
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fs), filter_svg);
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fs), filter_jpg);
    gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(fs), filter_svg);
    gtk_dialog_set_default_response(GTK_DIALOG(fs), GTK_RESPONSE_ACCEPT);
    gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(fs), FALSE);
    // Due to file name modifying later (depending on the file type selection),
    // we must do this manually
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(fs), FALSE);
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(fs), name);

    while (TRUE) {
        response = gtk_dialog_run(GTK_DIALOG(fs));
        gtk_widget_hide(fs);

        if (response == GTK_RESPONSE_ACCEPT) {
            GFile         *file = gtk_file_chooser_get_file(
                    GTK_FILE_CHOOSER(fs)
                );
            GtkFileFilter *filter = gtk_file_chooser_get_filter(
                    GTK_FILE_CHOOSER(fs)
                );
            gchar *filename = g_file_get_uri(file),
                  *extension,
                  *current_extension;
            AgChartSaveImageFunc save_func = NULL;
            gboolean can_save = FALSE;

            if (filter == filter_svg) {
                extension = ".svg";
                save_func = &ag_chart_export_svg_to_file;
            } else if (filter == filter_jpg) {
                extension = ".jpg";
                save_func = &ag_chart_export_jpg_to_file;
            } else {
                g_warning("Unknown file type");
                gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(fs), filter_svg);
            }

            current_extension = g_utf8_strrchr(filename, -1, '.');

            if (current_extension == NULL) {
                gchar *tmp;

                tmp = filename;
                filename = g_strdup_printf("%s%s", tmp, extension);
                g_free(tmp);
            } else {
                GFileInfo         *fileinfo;
                GFile             *tmp_file;
                gboolean          valid;
                GtkFileFilterInfo filter_info;

                tmp_file = g_file_new_for_uri(filename);
                fileinfo = g_file_query_info(
                        tmp_file,
                        G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME,
                        G_FILE_QUERY_INFO_NONE,
                        NULL,
                        NULL
                    );
                g_object_unref(tmp_file);

                filter_info.contains =
                        GTK_FILE_FILTER_URI
                        | GTK_FILE_FILTER_DISPLAY_NAME;
                filter_info.uri          = filename;
                filter_info.display_name = g_file_info_get_display_name(
                        fileinfo
                    );

                valid = gtk_file_filter_filter(filter, &filter_info);
                g_object_unref(fileinfo);

                if (!valid) {
                    GtkResponseType response;
                    gchar           *message,
                                    *new_filename;

                    new_filename = g_strdup_printf("%s%s", filename, extension);

                    message = g_strdup_printf(
                            "File extension doesn’t match the chosen format. " \
                            "Do you want Astrognome to append the correct " \
                            "extension (the new filename will be %s) or " \
                            "choose a new name?",
                            new_filename
                        );

                    response = ag_app_buttoned_dialog(
                            GTK_WINDOW(window),
                            GTK_MESSAGE_QUESTION,
                            message,
                            "Cancel",           GTK_RESPONSE_CANCEL,
                            "Append extension", GTK_RESPONSE_APPLY,
                            "Choose new file",  GTK_RESPONSE_NO,
                            NULL
                        );

                    if (response == GTK_RESPONSE_APPLY) {
                        g_free(filename);
                        filename = new_filename;
                    } else {
                        g_free(filename);
                        g_clear_object(&file);

                        if (response == GTK_RESPONSE_NO) {
                            continue;
                        }

                        break;
                    }
                }
            }

            g_clear_object(&file);
            file = g_file_new_for_uri(filename);
            g_free(filename);

            // Now check if a file under the modified name exists
            if (g_file_query_exists(file, NULL)) {
                GtkResponseType sub_response;
                gchar           *message;
                GFileInfo       *fileinfo;

                fileinfo = g_file_query_info(
                        file,
                        G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME,
                        G_FILE_QUERY_INFO_NONE,
                        NULL,
                        NULL
                    );
                message = g_strdup_printf(
                        "File %s already exists. Do you want to overwrite it?",
                        g_file_info_get_display_name(fileinfo)
                    );
                g_object_unref(fileinfo);

                sub_response = ag_app_buttoned_dialog(
                        GTK_WINDOW(window), GTK_MESSAGE_QUESTION,
                        message,
                        _("No"),  GTK_RESPONSE_NO,
                        _("Yes"), GTK_RESPONSE_YES,
                        NULL
                    );

                g_free(message);

                can_save = (sub_response == GTK_RESPONSE_YES);
            } else {
                can_save = TRUE;
            }

            if (can_save) {
                g_clear_error(&local_err);
                save_func(priv->chart, file, priv->theme, &local_err);

                if (local_err) {
                    ag_app_message_dialog(
                            GTK_WINDOW(window),
                            GTK_MESSAGE_ERROR,
                            "%s",
                            local_err->message
                        );
                }

                g_clear_object(&file);

                break;
            }

            g_clear_object(&file);
        } else {
            break;
        }
    }

    gtk_widget_destroy(fs);
}

static void
ag_window_export_image_action(GSimpleAction *action,
                              GVariant      *parameter,
                              gpointer      user_data)
{
    AgWindow *window = AG_WINDOW(user_data);
    GError   *err    = NULL;

    ag_window_export_image(window, &err);

    if (err) {
        ag_app_message_dialog(
                GTK_WINDOW(window),
                GTK_MESSAGE_ERROR,
                "%s",
                err->message
            );
    }
}

static void
ag_window_export_agc(AgWindow *window, GError **err)
{
    const gchar     *name;
    gchar           *file_name;
    GtkWidget       *fs;
    gint            response;
    GET_PRIV(window);

    ag_window_recalculate_chart(window, FALSE);

    // We should never enter here, but who knows...
    if (priv->chart == NULL) {
        ag_app_message_dialog(
                GTK_WINDOW(window),
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
        ag_app_message_dialog(
                GTK_WINDOW(window),
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
ag_window_export_agc_action(GSimpleAction *action,
                            GVariant      *parameter,
                            gpointer      user_data)
{
    AgWindow *window = AG_WINDOW(user_data);
    GError   *err    = NULL;

    ag_window_recalculate_chart(window, TRUE);
    ag_window_export_agc(window, &err);

    if (err) {
        ag_app_message_dialog(
                GTK_WINDOW(window),
                GTK_MESSAGE_ERROR,
                "%s", err->message
            );
    }
}

gboolean
ag_window_can_close(AgWindow *window, gboolean display_dialog)
{
    GET_PRIV(window);
    AgDbChartSave   *save_data = NULL;
    AgDb            *db        = ag_db_get();
    GError          *err       = NULL;
    gboolean        ret        = TRUE;

    if (priv->chart) {
        ag_window_recalculate_chart(window, TRUE);
        save_data = ag_chart_get_db_save(priv->chart);

        if (
                    !ag_db_chart_save_identical(priv->saved_data, save_data, FALSE)
                    || !(priv->saved_data)
                    || (priv->saved_data->db_id == -1)
                ) {
            g_debug("Save is needed!");

            if (display_dialog) {
                gint response;

                response = ag_app_buttoned_dialog(
                        GTK_WINDOW(window),
                        GTK_MESSAGE_QUESTION,
                        _("Chart is not saved. Do you want to save it?"),
                        _("Save and close"), GTK_RESPONSE_YES,
                        _("Close without saving"), GTK_RESPONSE_NO,
                        _("Return to chart"), GTK_RESPONSE_CANCEL,
                        NULL
                    );

                switch (response) {
                    case GTK_RESPONSE_YES:
                        if (!ag_db_chart_save(db, save_data, &err)) {
                            ag_app_message_dialog(
                                    GTK_WINDOW(window),
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

    ag_db_chart_save_unref(save_data);

    return ret;
}

static void
ag_window_save_action(GSimpleAction *action,
                      GVariant      *parameter,
                      gpointer      user_data)
{
    AgWindow        *window = AG_WINDOW(user_data);
    GET_PRIV(window);
    AgDb            *db     = ag_db_get();
    GError          *err    = NULL;
    AgDbChartSave   *save_data;

    ag_window_recalculate_chart(window, TRUE);

    if (!ag_window_can_close(window, FALSE)) {
        save_data = ag_chart_get_db_save(priv->chart);

        if (!ag_db_chart_save(db, save_data, &err)) {
            ag_app_message_dialog(
                    GTK_WINDOW(window),
                    GTK_MESSAGE_ERROR,
                    _("Unable to save: %s"),
                    err->message
                );
        }

        ag_db_chart_save_unref(priv->saved_data);
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
ag_window_clear_style_sheets(AgWindow *window)
{
    WebKitUserContentManager *manager;
    GET_PRIV(window);

    g_debug("Clearing style sheets");

    manager = webkit_web_view_get_user_content_manager(
            WEBKIT_WEB_VIEW(priv->chart_web_view)
        );

    webkit_user_content_manager_remove_all_style_sheets(manager);
    g_list_free_full(
            priv->style_sheets,
            (GDestroyNotify)webkit_user_style_sheet_unref
        );
    priv->style_sheets = NULL;
}

static void
ag_window_add_style_sheet(AgWindow *window, const gchar *path)
{
    gchar           *css_source;
    gboolean        source_free = FALSE;
    GET_PRIV(window);

    if (strncmp("gres://", path, 7) == 0) {
        gchar  *res_path = g_strdup_printf(
                "/eu/polonkai/gergely/Astrognome/%s",
                path + 7
            );
        GBytes *css_data = g_resources_lookup_data(
                res_path,
                G_RESOURCE_LOOKUP_FLAGS_NONE,
                NULL
            );

        css_source  = g_strdup(g_bytes_get_data(css_data, NULL));
        source_free = TRUE;
        g_bytes_unref(css_data);
    } else if (strncmp("raw:", path, 4) == 0) {
        css_source = (gchar *)path + 4;
    } else {
        GFile  *css_file = g_file_new_for_uri(path);
        GError *err = NULL;

        g_file_load_contents(
                css_file,
                NULL,
                &css_source, NULL,
                NULL,
                &err
            );
        source_free = TRUE;
        g_object_unref(css_file);
    }

    if (css_source) {
        WebKitUserStyleSheet *style_sheet = webkit_user_style_sheet_new(
                css_source,
                WEBKIT_USER_CONTENT_INJECT_TOP_FRAME,
                WEBKIT_USER_STYLE_LEVEL_USER,
                NULL, NULL
            );

        priv->style_sheets = g_list_append(priv->style_sheets, style_sheet);

        if (source_free) {
            g_free(css_source);
        }
    }
}

static void
ag_window_update_style_sheets(AgWindow *window)
{
    GList                    *item;
    WebKitUserContentManager *manager;
    GET_PRIV(window);

    g_debug("Updating style sheets");

    manager = webkit_web_view_get_user_content_manager(
            WEBKIT_WEB_VIEW(priv->chart_web_view)
        );

    webkit_user_content_manager_remove_all_style_sheets(manager);

    for (item = priv->style_sheets; item; item = g_list_next(item)) {
        WebKitUserStyleSheet *style_sheet = item->data;

        webkit_user_content_manager_add_style_sheet(manager, style_sheet);
    }
}

static void
ag_window_set_theme(AgWindow *window, AgDisplayTheme *theme)
{
    gchar           *css,
                    *css_final;
    GET_PRIV(window);

    g_debug("Setting theme to %s", (theme) ? theme->name : "no theme");
    ag_window_clear_style_sheets(window);

    priv->theme = theme;

    // Add the default style sheet
    ag_window_add_style_sheet(
            window,
            "gres://ui/chart-default.css"
        );

    if (theme) {
        css = ag_display_theme_to_css(theme);
        css_final = g_strdup_printf("raw:%s", css);
        g_free(css);

        ag_window_add_style_sheet(window, css_final);

        g_free(css_final);
    }

    ag_window_update_style_sheets(window);
}

static void
ag_window_tab_changed_cb(GtkStack *stack, GParamSpec *pspec, AgWindow *window)
{
    GtkWidget       *active_tab;
    const gchar     *active_tab_name = gtk_stack_get_visible_child_name(stack);
    GET_PRIV(window);

    g_debug("Active tab changed: %s", active_tab_name);

    if (active_tab_name == NULL) {
        return;
    }

    active_tab = gtk_stack_get_visible_child(stack);

    if (strcmp("chart", active_tab_name) == 0) {
        gtk_widget_set_size_request(active_tab, 600, 600);
        if (priv->theme == NULL) {
            AgSettings           *settings;
            GSettings            *main_settings;
            gint                 default_theme;

            settings      = ag_settings_get();
            main_settings = ag_settings_peek_main_settings(settings);
            default_theme = g_settings_get_int(
                    main_settings,
                    "default-display-theme"
                );
            g_object_unref(settings);

            priv->theme = ag_display_theme_get_by_id(default_theme);

            ag_window_set_theme(window, priv->theme);
        }
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
            ag_window_redraw_chart(window);
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
    GET_PRIV(window);

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
    GET_PRIV(window);
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

static gboolean
ag_window_set_default_display_theme(GtkTreeModel *model,
                                    GtkTreePath  *path,
                                    GtkTreeIter  *iter,
                                    AgWindow     *window)
{
    gint            row_display_theme;
    GET_PRIV(window);
    AgSettings      *settings      = ag_settings_get();
    GSettings       *main_settings = ag_settings_peek_main_settings(settings);
    gint            default_theme  = g_settings_get_int(
            main_settings,
            "default-display-theme"
        );

    g_clear_object(&settings);
    gtk_tree_model_get(
            model, iter,
            0, &row_display_theme,
            -1
        );

    if (default_theme == row_display_theme) {
        gtk_combo_box_set_active_iter(GTK_COMBO_BOX(priv->display_theme), iter);

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
    GET_PRIV(window);

    /* Empty edit tab values */
    gtk_entry_set_text(GTK_ENTRY(priv->name), "");
    gtk_entry_set_text(GTK_ENTRY(priv->country), "");
    gtk_entry_set_text(GTK_ENTRY(priv->city), "");
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
                GTK_WINDOW(window),
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
    GET_PRIV(window);

    g_debug("Back button pressed");

    if (ag_window_can_close(window, TRUE)) {
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(priv->toolbar_aspect),
                TRUE
            );
        g_clear_object(&(priv->chart));
        ag_db_chart_save_unref(priv->saved_data);
        priv->saved_data = NULL;

        ag_window_reload_chart_list(window);
        gtk_stack_set_visible_child_name(GTK_STACK(priv->stack), "list");
        gtk_header_bar_set_subtitle(GTK_HEADER_BAR(priv->header_bar), NULL);
    }
}

static void
ag_window_refresh_action(GSimpleAction *action,
                         GVariant      *parameter,
                         gpointer      user_data)
{
    ag_window_reload_chart_list(AG_WINDOW(user_data));
}

static void
ag_window_set_selection_mode(AgWindow *window, gboolean state)
{
    GtkStyleContext *style;
    GET_PRIV(window);

    style = gtk_widget_get_style_context(priv->header_bar);

    if (state) {
        gtk_header_bar_set_show_close_button(
                GTK_HEADER_BAR(priv->header_bar),
                FALSE
            );
        gtk_style_context_add_class(style, "selection-mode");
        ag_icon_view_set_mode(
                AG_ICON_VIEW(priv->chart_list),
                AG_ICON_VIEW_MODE_SELECTION
            );
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
        ag_icon_view_set_mode(
                AG_ICON_VIEW(priv->chart_list),
                AG_ICON_VIEW_MODE_NORMAL
            );
        gtk_widget_show_all(priv->new_back_stack);
        gtk_stack_set_visible_child_name(
                GTK_STACK(priv->menubutton_stack),
                "list"
            );
    }
}

static void
ag_window_icon_view_mode_cb(AgIconView *icon_view,
                            GParamSpec *pspec,
                            AgWindow   *window)
{
    AgIconViewMode mode       = ag_icon_view_get_mode(icon_view);
    GVariant       *state_var = g_variant_new_boolean(
            (mode == AG_ICON_VIEW_MODE_SELECTION)
        );

    g_action_group_activate_action(
            G_ACTION_GROUP(window),
            "selection",
            state_var
        );
}

static void
ag_window_selection_mode_action(GSimpleAction *action,
                                GVariant      *parameter,
                                gpointer      user_data)
{
    GVariant        *state;
    gboolean        new_state;
    AgWindow        *window = AG_WINDOW(user_data);

    state = g_action_get_state(G_ACTION(action));
    new_state = !g_variant_get_boolean(state);
    g_action_change_state(G_ACTION(action), g_variant_new_boolean(new_state));
    g_variant_unref(state);

    g_debug("Set selection mode: %d", new_state);

    ag_window_set_selection_mode(window, new_state);
}

static void
ag_window_delete_action(GSimpleAction *action,
                        GVariant      *parameter,
                        gpointer      user_data)
{
    GList           *selection,
                    *item;
    AgWindow        *window = AG_WINDOW(user_data);
    GET_PRIV(window);
    AgDb            *db     = ag_db_get();

    selection = ag_icon_view_get_selected_items(AG_ICON_VIEW(priv->chart_list));

    for (item = selection; item; item = g_list_next(item)) {
        GtkTreePath   *path = item->data;
        GError        *err = NULL;
        AgDbChartSave *save_data;

        save_data = ag_icon_view_get_chart_save_at_path(AG_ICON_VIEW(priv->chart_list), path);

        if (!ag_db_chart_delete(db, save_data->db_id, &err)) {
            ag_app_message_dialog(
                    GTK_WINDOW(window),
                    GTK_MESSAGE_ERROR,
                    "Unable to delete chart: %s",
                    (err && err->message)
                        ? err->message
                        : "No reason"
                );
        }
    }

    ag_icon_view_remove_selected(AG_ICON_VIEW(priv->chart_list));

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
    GET_PRIV(AG_WINDOW(user_data));
    static gchar *js         = "aspects = document.getElementById('aspects');\n"   \
                               "antiscia = document.getElementById('antiscia');\n" \
                               "aspects.setAttribute('display', '%s');\n"       \
                               "antiscia.setAttribute('display', '%s');\n";

    current_state = g_action_get_state(G_ACTION(action));

    if (g_variant_equal(current_state, parameter)) {
        return;
    }

    g_action_change_state(G_ACTION(action), parameter);

    state = g_variant_get_string(parameter, NULL);

    if (strcmp("aspects", state) == 0) {
        g_debug("Switching to aspects");
        js_code = g_strdup_printf(js, "block", "none");
    } else if (strcmp("antiscia", state) == 0) {
        g_debug("Switching to antiscia");
        js_code = g_strdup_printf(js, "none", "block");
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
    { "close",        ag_window_close_action,          NULL, NULL,        NULL },
    { "save",         ag_window_save_action,           NULL, NULL,        NULL },
    { "export-agc",   ag_window_export_agc_action,     NULL, NULL,        NULL },
    { "export-image", ag_window_export_image_action,   NULL, NULL,        NULL },
    { "view-menu",    ag_window_view_menu_action,      NULL, "false",     NULL },
    { "gear-menu",    ag_window_gear_menu_action,      NULL, "false",     NULL },
    { "change-tab",   ag_window_change_tab_action,     "s",  "'edit'",    NULL },
    { "new-chart",    ag_window_new_chart_action,      NULL, NULL,        NULL },
    { "back",         ag_window_back_action,           NULL, NULL,        NULL },
    { "refresh",      ag_window_refresh_action,        NULL, NULL,        NULL },
    { "selection",    ag_window_selection_mode_action, "b",  "false",     NULL },
    { "delete",       ag_window_delete_action,         NULL, NULL,        NULL },
    { "connection",   ag_window_connection_action,     "s",  "'aspects'", NULL },
};

static void
ag_window_display_changed(GSettings *settings, gchar *key, AgWindow *window)
{
    GET_PRIV(window);

    /* The planet symbols are redrawn only if aspect_table_populated is
     * set to FALSE */
    if (g_str_equal("planets-char", key)) {
        priv->aspect_table_populated = FALSE;
    }

    ag_window_redraw_aspect_table(window);
    ag_window_redraw_points_table(window);
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
ag_window_add_display_theme(AgDisplayTheme *display_theme,
                            AgWindowPrivate *priv)
{
    GtkTreeIter iter;

    gtk_list_store_append(priv->display_theme_model, &iter);
    gtk_list_store_set(
            priv->display_theme_model, &iter,
            0, display_theme->id,
            1, display_theme->name,
            -1
        );
}

static void
ag_window_list_item_activated_cb(AgIconView        *icon_view,
                                 const GtkTreePath *path,
                                 AgWindow          *window)
{
    AgChart         *chart;
    GET_PRIV(window);
    AgDb            *db    = ag_db_get();
    GError          *err   = NULL;
    AgDbChartSave   *save_data;

    if (priv->saved_data != NULL) {
        ag_app_message_dialog(
                GTK_WINDOW(window),
                GTK_MESSAGE_ERROR,
                "Window chart is not saved. " \
                "This is a bug, it should not happen here. " \
                "Please consider opening a bug report!"
            );

        ag_window_change_tab(window, "chart");

        return;
    }

    save_data = ag_icon_view_get_chart_save_at_path(icon_view, path);

    if (save_data == NULL) {
        return;
    }

    g_debug("Loading chart with ID %d", save_data->db_id);

    if ((priv->saved_data = ag_db_chart_get_data_by_id(
                 db,
                 save_data->db_id,
                 &err)) == NULL) {
        ag_app_message_dialog(
                GTK_WINDOW(window),
                GTK_MESSAGE_ERROR,
                "Could not open chart."
            );

        return;
    }

    if (priv->chart) {
        g_clear_object(&(priv->chart));
    }

    if ((chart = ag_chart_new_from_db_save(
                 priv->saved_data,
                 FALSE,
                 &err
            )) == NULL) {
        ag_app_message_dialog(
                GTK_WINDOW(window),
                GTK_MESSAGE_ERROR,
                "Error: %s",
                err->message
            );
        ag_db_chart_save_unref(priv->saved_data);
        priv->saved_data = NULL;

        return;
    }

    ag_window_set_chart(window, chart);

    ag_window_update_from_chart(window);

    ag_window_change_tab(window, "chart");
}

static void
ag_window_list_selection_changed_cb(AgIconView *view, AgWindow *window)
{
    GList           *selection;
    guint           count;
    GET_PRIV(window);

    selection = ag_icon_view_get_selected_items(view);

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

static gboolean
ag_window_city_matches(GtkEntryCompletion *city_comp,
                       const gchar        *key,
                       GtkTreeIter        *iter,
                       AgWindow           *window)
{
    GET_PRIV(window);
    gchar           *ccode,
                    *name,
                    *normalized_name,
                    *case_normalized_name;
    gboolean        ret = FALSE;

    gtk_tree_model_get(
            gtk_entry_completion_get_model(city_comp), iter,
            AG_CITY_NAME,    &name,
            AG_CITY_COUNTRY, &ccode,
            -1
        );

    if (
                (priv->selected_country == NULL)
                || (strcmp(priv->selected_country, ccode) == 0)
            ) {
        normalized_name = g_utf8_normalize(name, -1, G_NORMALIZE_ALL);

        if (normalized_name) {
            case_normalized_name = g_utf8_casefold(normalized_name, -1);
            if (strncmp(key, case_normalized_name, strlen(key)) == 0) {
                ret = TRUE;
            }

            g_free(case_normalized_name);
            g_free(normalized_name);
        }
    }

    g_free(name);
    g_free(ccode);

    return ret;
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

static void
ag_window_init(AgWindow *window)
{
    GtkAccelGroup            *accel_group;
    GSettings                *main_settings;
    GList                    *house_system_list,
                             *display_theme_list;
    GtkCellRenderer          *house_system_renderer,
                             *display_theme_renderer;
    WebKitUserContentManager *manager = webkit_user_content_manager_new();
    GET_PRIV(window);

    gtk_widget_init_template(GTK_WIDGET(window));

    priv->chart_web_view = webkit_web_view_new_with_user_content_manager(
            manager
        );
    gtk_box_pack_end(
            GTK_BOX(priv->tab_chart),
            priv->chart_web_view,
            TRUE, TRUE, 0
        );

    g_signal_connect(
            priv->chart_web_view,
            "context-menu",
            G_CALLBACK(ag_window_chart_context_cb),
            NULL
        );

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

    gtk_entry_completion_set_model(priv->country_comp, country_list);
    gtk_entry_completion_set_text_column(priv->country_comp, AG_COUNTRY_NAME);
    gtk_entry_set_completion(GTK_ENTRY(priv->country), priv->country_comp);

    gtk_entry_completion_set_model(priv->city_comp, city_list);
    gtk_entry_completion_set_text_column(priv->city_comp, AG_CITY_NAME);
    gtk_entry_completion_set_minimum_key_length(priv->city_comp, 3);
    gtk_entry_set_completion(GTK_ENTRY(priv->city), priv->city_comp);
    gtk_entry_completion_set_match_func(
            priv->city_comp,
            (GtkEntryCompletionMatchFunc)ag_window_city_matches,
            window,
            NULL
        );

    // Fill the house system model and set the combo box on the Edit tab to the
    // default one
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

    display_theme_list = ag_display_theme_get_list();
    g_list_foreach(
            display_theme_list,
            (GFunc)ag_window_add_display_theme,
            priv
        );
    g_list_free_full(display_theme_list, (GDestroyNotify)ag_display_theme_free);
    gtk_tree_model_foreach(
            GTK_TREE_MODEL(priv->display_theme_model),
            (GtkTreeModelForeachFunc)ag_window_set_default_display_theme,
            window
        );

    display_theme_renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(
            GTK_CELL_LAYOUT(priv->display_theme),
            display_theme_renderer,
            TRUE
        );
    gtk_cell_layout_set_attributes(
            GTK_CELL_LAYOUT(priv->display_theme),
            display_theme_renderer,
            "text", 1,
            NULL
        );

    gtk_stack_set_visible_child_name(GTK_STACK(priv->stack), "list");
    priv->current_tab = priv->tab_list;

    g_object_set(
            priv->year_adjust,
            "lower", (gdouble)G_MININT,
            "upper", (gdouble)G_MAXINT,
            NULL
        );

    ag_window_set_chart(window, NULL);

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
    GET_PRIV(AG_WINDOW(gobject));

    g_clear_object(&priv->settings);

    G_OBJECT_CLASS(ag_window_parent_class)->dispose(gobject);
}

static void
ag_window_name_changed_cb(GtkEntry *name_entry, AgWindow *window)
{
    const gchar     *name;
    GET_PRIV(window);

    name = gtk_entry_get_text(name_entry);

    gtk_header_bar_set_subtitle(GTK_HEADER_BAR(priv->header_bar), name);
}

static gboolean
ag_window_find_country(GtkTreeModel     *model,
                       GtkTreePath      *path,
                       GtkTreeIter      *iter,
                       struct cc_search *search)
{
    gchar    *name,
             *ccode;
    gboolean found = FALSE;

    gtk_tree_model_get(
            model, iter,
            AG_COUNTRY_NAME, &name,
            AG_COUNTRY_CODE, &ccode,
            -1
        );

    if (g_utf8_collate(search->target, name) == 0) {
        found = TRUE;
        search->ret_iter = gtk_tree_iter_copy(iter);
        search->ret_code = ccode;
    } else {
        g_free(ccode);
    }

    return found;
}

/**
 * ag_window_country_changed_callback:
 * @country: the #GtkSearchEntry for country search
 * @window: the window in which the event happens
 *
 * This function is called whenever the text in the country search entry is
 * changed.
 */
static void
ag_window_country_changed_callback(GtkSearchEntry *country, AgWindow *window)
{
    struct cc_search search;
    GET_PRIV(window);

    search.target   = gtk_entry_get_text(GTK_ENTRY(country));
    search.ret_iter = NULL;

    gtk_tree_model_foreach(
            country_list,
            (GtkTreeModelForeachFunc)ag_window_find_country,
            &search
        );

    g_free(priv->selected_country);

    if (search.ret_iter != NULL) {
        g_debug("Country (entry-changed): %s", search.ret_code);
        gtk_tree_iter_free(search.ret_iter);
        priv->selected_country = search.ret_code;
    } else {
        priv->selected_country = NULL;
    }
}

static gboolean
ag_window_find_city(GtkTreeModel     *model,
                    GtkTreePath      *path,
                    GtkTreeIter      *iter,
                    struct cc_search *search)
{
    gchar    *name,
             *ccode;
    gboolean found = FALSE;

    gtk_tree_model_get(
            model, iter,
            AG_CITY_NAME, &name,
            AG_CITY_COUNTRY, &ccode,
            -1
        );

    if (g_utf8_collate(search->target, name) == 0) {
        found = TRUE;
        search->ret_iter = gtk_tree_iter_copy(iter);
        search->ret_code = ccode;
    } else {
        g_free(ccode);
    }

    return found;
}

static void
ag_window_city_changed_callback(GtkSearchEntry *city, AgWindow *window)
{
    struct cc_search search;
    GET_PRIV(window);

    search.target   = gtk_entry_get_text(GTK_ENTRY(city));
    search.ret_iter = NULL;

    gtk_tree_model_foreach(
            city_list,
            (GtkTreeModelForeachFunc)ag_window_find_city,
            &search
        );

    g_free(priv->selected_city);

    if (search.ret_iter != NULL) {
        gdouble longitude,
                latitude,
                altitude;
        gchar   *name,
                *ccode;

        gtk_tree_model_get(
                city_list, search.ret_iter,
                AG_CITY_COUNTRY, &ccode,
                AG_CITY_NAME,    &name,
                AG_CITY_LAT,     &latitude,
                AG_CITY_LONG,    &longitude,
                AG_CITY_ALT,     &altitude,
                -1
            );

        if (
                    (priv->selected_country != NULL)
                    && (strcmp(priv->selected_country, ccode) != 0)
                ) {
            return;
        }

        if (latitude < 0.0) {
            gtk_toggle_button_set_active(
                    GTK_TOGGLE_BUTTON(priv->south_lat),
                    TRUE
                );
            gtk_spin_button_set_value(
                    GTK_SPIN_BUTTON(priv->latitude),
                    -latitude
                );
        } else {
            gtk_toggle_button_set_active(
                    GTK_TOGGLE_BUTTON(priv->north_lat),
                    TRUE
                );
            gtk_spin_button_set_value(
                    GTK_SPIN_BUTTON(priv->latitude),
                    latitude
                );
        }

        if (longitude < 0.0) {
            gtk_toggle_button_set_active(
                    GTK_TOGGLE_BUTTON(priv->west_long),
                    TRUE
                );
            gtk_spin_button_set_value(
                    GTK_SPIN_BUTTON(priv->longitude),
                    -longitude
                );
        } else {
            gtk_toggle_button_set_active(
                    GTK_TOGGLE_BUTTON(priv->east_long),
                    TRUE
                );
            gtk_spin_button_set_value(
                    GTK_SPIN_BUTTON(priv->longitude),
                    longitude
                );
        }

        // TODO: implement setting altitude maybe? Is that really necessary?

        g_debug("City (entry-changed): %s (%s); %.6f, %.6f, %.6f", name, search.ret_code, longitude, latitude, altitude);
        g_free(name);
        gtk_tree_iter_free(search.ret_iter);
        priv->selected_city = search.ret_code;
    } else {
        priv->selected_city = NULL;
    }
}

static void
ag_window_house_system_changed_cb(GtkComboBox *combo_box,
                                  AgWindow    *window)
{
    GtkTreeIter     iter;
    GsweHouseSystem house_system;
    GET_PRIV(window);

    gtk_combo_box_get_active_iter(combo_box, &iter);
    gtk_tree_model_get(
            GTK_TREE_MODEL(priv->house_system_model), &iter,
            0, &house_system,
            -1
        );

    if (priv->chart) {
        gswe_moment_set_house_system(GSWE_MOMENT(priv->chart), house_system);
    }

    g_debug("House system changed: %d", house_system);
}

static void
ag_window_display_theme_changed_cb(GtkComboBox *combo_box,
                                   AgWindow    *window)
{
    GtkTreeIter     iter;
    gint            theme_id;
    AgDisplayTheme  *theme;
    GET_PRIV(window);

    gtk_combo_box_get_active_iter(combo_box, &iter);
    gtk_tree_model_get(
            GTK_TREE_MODEL(priv->display_theme_model), &iter,
            0, &theme_id,
            -1
        );

    theme = ag_display_theme_get_by_id(theme_id);
    ag_window_set_theme(window, theme);
}

static void
ag_window_destroy(GtkWidget *widget)
{
    GET_PRIV(AG_WINDOW(widget));

    // Destroy the signal handlers on priv->stack, as “tab” switching
    // can cause trouble during destroy. However, this function might
    // get called multiple times for the same object, in which case
    // priv->stack is NULL.
    if (priv->stack) {
        g_signal_handlers_destroy(priv->stack);
    }

    GTK_WIDGET_CLASS(ag_window_parent_class)->destroy(widget);
}

static void
ag_window_selection_mode_cancel_cb(GtkButton *button, AgWindow *window)
{
    ag_window_set_selection_mode(window, FALSE);
}

static void
ag_window_set_property(GObject      *gobject,
                       guint        prop_id,
                       const GValue *value,
                       GParamSpec   *pspec)
{
    AgWindow *window = AG_WINDOW(gobject);

    switch (prop_id) {
        case PROP_CHART:
            ag_window_set_chart(window, (g_value_get_object(value)));

            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, pspec);

            break;
    }
}

static void
ag_window_get_property(GObject    *gobject,
                       guint      prop_id,
                       GValue     *value,
                       GParamSpec *pspec)
{
    GET_PRIV(AG_WINDOW(gobject));

    switch (prop_id) {
        case PROP_CHART:
            g_value_set_object(value, priv->chart);

            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, pspec);

            break;
    }
}

static void
ag_window_class_init(AgWindowClass *klass)
{
    GObjectClass   *gobject_class = G_OBJECT_CLASS(klass);
    GtkWidgetClass *widget_class  = GTK_WIDGET_CLASS(klass);

    widget_class->destroy = ag_window_destroy;
    gobject_class->dispose = ag_window_dispose;
    gobject_class->set_property = ag_window_set_property;
    gobject_class->get_property = ag_window_get_property;

    properties[PROP_CHART] = g_param_spec_object(
            "chart",
            "Chart",
            "The AgChart associated with this window",
            AG_TYPE_CHART,
            G_PARAM_STATIC_NICK
            | G_PARAM_STATIC_NAME
            | G_PARAM_STATIC_BLURB
            | G_PARAM_READABLE
            | G_PARAM_WRITABLE
        );
    g_object_class_install_property(
            gobject_class,
            PROP_CHART,
            properties[PROP_CHART]
        );

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
            tab_edit
        );
    gtk_widget_class_bind_template_child_private(widget_class, AgWindow, name);
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            country
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            country_comp
        );
    gtk_widget_class_bind_template_child_private(widget_class, AgWindow, city);
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            city_comp
        );
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
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            points_eq
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            display_theme
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            display_theme_model
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            toolbar_aspect
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            tab_list
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgWindow,
            chart_list
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
    gtk_widget_class_bind_template_callback(
            widget_class,
            ag_window_country_changed_callback
        );
    gtk_widget_class_bind_template_callback(
            widget_class,
            ag_window_city_changed_callback
        );
    gtk_widget_class_bind_template_callback(
            widget_class,
            ag_window_display_theme_changed_cb
        );
    gtk_widget_class_bind_template_callback(
            widget_class,
            ag_window_list_item_activated_cb
        );
    gtk_widget_class_bind_template_callback(
            widget_class,
            ag_window_list_selection_changed_cb
        );
    gtk_widget_class_bind_template_callback(
            widget_class,
            ag_window_icon_view_mode_cb
        );
    gtk_widget_class_bind_template_callback(
            widget_class,
            ag_window_selection_mode_cancel_cb
        );
    gtk_widget_class_bind_template_callback(
            widget_class,
            ag_window_house_system_changed_cb
        );
}

static gboolean
ag_window_configure_event_cb(GtkWidget         *widget,
                             GdkEventConfigure *event,
                             gpointer          user_data)
{
    AgWindow        *window = AG_WINDOW(widget);
    GET_PRIV(window);

    ag_window_settings_save(
            GTK_WINDOW(window),
            ag_settings_peek_window_settings(priv->settings)
        );

    return FALSE;
}

GtkWidget *
ag_window_new(AgApp *app)
{
    AgWindow *window  = g_object_new(AG_TYPE_WINDOW, NULL);
    GET_PRIV(window);

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
    GET_PRIV(window);

    if (priv->chart != NULL) {
        g_signal_handler_disconnect(
                priv->chart,
                priv->chart_changed_handler
            );
        g_clear_object(&(priv->chart));
    }

    ag_db_chart_save_unref(priv->saved_data);

    priv->chart = chart;

    if (chart) {
        priv->chart_changed_handler = g_signal_connect(
                priv->chart,
                "changed",
                G_CALLBACK(ag_window_chart_changed),
                window
            );
        g_object_ref(chart);
        priv->saved_data = ag_chart_get_db_save(chart);
    } else {
        priv->saved_data = NULL;
    }

    g_object_notify_by_pspec(G_OBJECT(window), properties[PROP_CHART]);
}

AgChart *
ag_window_get_chart(AgWindow *window)
{
    GET_PRIV(window);

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
    GET_PRIV(window);

    gtk_stack_set_visible_child_name(GTK_STACK(priv->stack), tab_name);
    g_action_change_state(
            g_action_map_lookup_action(G_ACTION_MAP(window), "change-tab"),
            g_variant_new_string(tab_name)
        );
}

static gboolean
ag_window_add_chart(LoadIdleData *idle_data)
{
    AgDbChartSave *save_data;

    g_assert(
            (idle_data->load_state == PREVIEW_STATE_STARTED)
            || (idle_data->load_state == PREVIEW_STATE_LOADING)
        );

    if (!idle_data->items) {
        idle_data->load_state = PREVIEW_STATE_COMPLETE;

        return FALSE;
    }

    if (!idle_data->n_items) {
        idle_data->n_items = g_list_length(idle_data->items);
        idle_data->n_loaded = 0;
        idle_data->load_state = PREVIEW_STATE_LOADING;
    }

    save_data = g_list_nth_data(idle_data->items, idle_data->n_loaded);

    g_assert(save_data);

    ag_icon_view_add_chart(idle_data->icon_view, save_data);

    idle_data->n_loaded++;

    // TODO: maybe a progress bar should update somewhere during loading?

    if (idle_data->n_loaded == idle_data->n_items) {
        idle_data->load_state = PREVIEW_STATE_COMPLETE;
        idle_data->n_loaded = 0;
        idle_data->n_items = 0;
        g_list_free(idle_data->items);
        idle_data->items = NULL;

        return FALSE;
    } else {
        return TRUE;
    }
}

static void
ag_window_cleanup_load_items(LoadIdleData *idle_data)
{
    g_assert(idle_data->load_state == PREVIEW_STATE_COMPLETE);

    g_free(idle_data);
}

gboolean
ag_window_reload_chart_list(AgWindow *window)
{
    LoadIdleData    *idle_data;
    AgDb            *db         = ag_db_get();
    GError          *err        = NULL;
    GList           *chart_list = ag_db_chart_get_list(db, &err);
    GET_PRIV(window);

    ag_icon_view_remove_all(AG_ICON_VIEW(priv->chart_list));

    /* Lazy loading of charts with previews. Idea is from
     * http://blogs.gnome.org/ebassi/documentation/lazy-loading/ */

    idle_data = g_new(LoadIdleData, 1);
    idle_data->items = chart_list;
    idle_data->n_items = 0;
    idle_data->n_loaded = 0;
    idle_data->icon_view = AG_ICON_VIEW(priv->chart_list);
    idle_data->load_state = PREVIEW_STATE_STARTED;

    idle_data->load_id = g_idle_add_full(
            G_PRIORITY_DEFAULT_IDLE,
            (GSourceFunc)ag_window_add_chart,
            idle_data,
            (GDestroyNotify)ag_window_cleanup_load_items
        );

    return TRUE;
}

/**
 * ag_window_is_usable:
 * @window: an #AgWindow to test
 *
 * Checks if the given window is usable for new charts. Usability is
 * currently means that it has no charts open.
 *
 * Returns: TRUE if @window is usable, FALSE otherwise
 */
gboolean
ag_window_is_usable(AgWindow *window)
{
    GET_PRIV(window);

    return (priv->current_tab == priv->tab_list);
}
