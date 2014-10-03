#include <math.h>
#include <string.h>
#include <swe-glib.h>

#include "config.h"
#include "ag-chart-edit.h"
#include "astrognome.h"

typedef struct {
    GtkWidget          *name;
    GtkWidget          *country;
    GtkWidget          *city;
    GtkWidget          *north_lat;
    GtkWidget          *south_lat;
    GtkWidget          *latitude;
    GtkWidget          *east_long;
    GtkWidget          *west_long;
    GtkWidget          *longitude;
    GtkWidget          *year;
    GtkWidget          *month;
    GtkWidget          *day;
    GtkWidget          *hour;
    GtkWidget          *minute;
    GtkWidget          *second;
    GtkWidget          *timezone;

    GtkAdjustment      *year_adjust;

    GtkEntryCompletion *country_comp;
    gchar              *selected_country;

    GtkEntryCompletion *city_comp;
    gchar              *selected_city;

    GtkTextBuffer      *note_buffer;
} AgChartEditPrivate;

enum {
    PROP_0,
    PROP_NAME,
    PROP_COUNTRY,
    PROP_CITY,
    PROP_LONGITUDE,
    PROP_LATITUDE,
    PROP_TIMESTAMP,
    PROP_NOTE,
    PROP_COUNT
};

enum {
    SIGNAL_NAME_CHANGED,
    SIGNAL_COUNT
};

struct cc_search {
    const gchar  *target;
    GtkTreeIter  *ret_iter;
    gchar        *ret_code;
};

G_DEFINE_TYPE_WITH_PRIVATE(AgChartEdit, ag_chart_edit, GTK_TYPE_GRID);

static GParamSpec *properties[PROP_COUNT];
static guint      signals[SIGNAL_COUNT];

#define GET_PRIV(o) AgChartEditPrivate *priv = ag_chart_edit_get_instance_private((o))

static gboolean
ag_chart_edit_find_country(GtkTreeModel     *model,
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
 * ag_chart_edit_country_changed_cb:
 * @country: the #GtkSearchEntry for country search
 * @chart_edit: the #AgChartEdit in which the event happens
 *
 * This function is called whenever the text in the country search entry is
 * changed.
 */
static void
ag_chart_edit_country_changed_cb(GtkSearchEntry *country,
                                   AgChartEdit  *chart_edit)
{
    struct cc_search search;
    GET_PRIV(chart_edit);

    search.target   = gtk_entry_get_text(GTK_ENTRY(country));
    search.ret_iter = NULL;

    gtk_tree_model_foreach(
            country_list,
            (GtkTreeModelForeachFunc)ag_chart_edit_find_country,
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

static gint
ag_chart_edit_get_year(AgChartEdit *chart_edit)
{
    GET_PRIV(chart_edit);

    return gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(priv->year));
}

static gint
ag_chart_edit_get_month(AgChartEdit *chart_edit)
{
    GET_PRIV(chart_edit);

    return gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(priv->month));
}

static gint
ag_chart_edit_get_day(AgChartEdit *chart_edit)
{
    GET_PRIV(chart_edit);

    return gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(priv->day));
}

static gint
ag_chart_edit_get_hour(AgChartEdit *chart_edit)
{
    GET_PRIV(chart_edit);

    return gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(priv->hour));
}

static gint
ag_chart_edit_get_minute(AgChartEdit *chart_edit)
{
    GET_PRIV(chart_edit);

    return gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(priv->minute));
}

static gint
ag_chart_edit_get_second(AgChartEdit *chart_edit)
{
    GET_PRIV(chart_edit);

    return gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(priv->second));
}

static gdouble
ag_chart_edit_get_timezone(AgChartEdit *chart_edit)
{
    GET_PRIV(chart_edit);

    return gtk_spin_button_get_value(GTK_SPIN_BUTTON(priv->timezone));
}

void
ag_chart_edit_set_name(AgChartEdit *chart_edit, const gchar *name)
{
    GET_PRIV(chart_edit);

    if (name) {
        gtk_entry_set_text(GTK_ENTRY(priv->name), name);
    } else {
        gtk_entry_set_text(GTK_ENTRY(priv->name), "");
    }
}

const gchar *
ag_chart_edit_get_name(AgChartEdit *chart_edit)
{
    GET_PRIV(chart_edit);

    return gtk_entry_get_text(GTK_ENTRY(priv->name));
}

void
ag_chart_edit_set_country(AgChartEdit *chart_edit, const gchar *country)
{
    GET_PRIV(chart_edit);

    if (country) {
        gtk_entry_set_text(GTK_ENTRY(priv->country), country);
    } else {
        gtk_entry_set_text(GTK_ENTRY(priv->country), "");
    }
}

const gchar *
ag_chart_edit_get_country(AgChartEdit *chart_edit)
{
    GET_PRIV(chart_edit);

    return gtk_entry_get_text(GTK_ENTRY(priv->country));
}

void
ag_chart_edit_set_city(AgChartEdit *chart_edit, const gchar *city)
{
    GET_PRIV(chart_edit);

    if (city) {
        gtk_entry_set_text(GTK_ENTRY(priv->city), city);
    } else {
        gtk_entry_set_text(GTK_ENTRY(priv->city), "");
    }
}

const gchar *
ag_chart_edit_get_city(AgChartEdit *chart_edit)
{
    GET_PRIV(chart_edit);

    return gtk_entry_get_text(GTK_ENTRY(priv->city));
}

void
ag_chart_edit_set_latitude(AgChartEdit *chart_edit, gdouble latitude)
{
    GET_PRIV(chart_edit);

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(priv->latitude), fabs(latitude));

    if (latitude < 0.0) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->south_lat), TRUE);
    } else {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->north_lat), TRUE);
    }
}

gdouble
ag_chart_edit_get_latitude(AgChartEdit *chart_edit)
{
    gdouble value;
    GET_PRIV(chart_edit);

    value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(priv->latitude));
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->south_lat))) {
        value = 0 - value;
    }

    return value;
}

void
ag_chart_edit_set_longitude(AgChartEdit *chart_edit, gdouble longitude)
{
    GET_PRIV(chart_edit);

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(priv->longitude), fabs(longitude));

    if (longitude < 0.0) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->west_long), TRUE);
    } else {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->east_long), TRUE);
    }
}

gdouble
ag_chart_edit_get_longitude(AgChartEdit *chart_edit)
{
    gdouble value;
    GET_PRIV(chart_edit);

    value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(priv->longitude));
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->west_long))) {
        value = 0 - value;
    }

    return value;
}

void
ag_chart_edit_set_from_timestamp(AgChartEdit *chart_edit,
                                 GsweTimestamp *timestamp)
{
    GET_PRIV(chart_edit);

    gtk_spin_button_set_value(
            GTK_SPIN_BUTTON(priv->year),
            (gdouble)gswe_timestamp_get_gregorian_year(timestamp, NULL)
        );

    gtk_spin_button_set_value(
            GTK_SPIN_BUTTON(priv->month),
            (gdouble)gswe_timestamp_get_gregorian_month(timestamp, NULL)
        );

    gtk_spin_button_set_value(
            GTK_SPIN_BUTTON(priv->day),
            (gdouble)gswe_timestamp_get_gregorian_day(timestamp, NULL)
        );

    gtk_spin_button_set_value(
            GTK_SPIN_BUTTON(priv->hour),
            (gdouble)gswe_timestamp_get_gregorian_hour(timestamp, NULL)
        );

    gtk_spin_button_set_value(
            GTK_SPIN_BUTTON(priv->minute),
            (gdouble)gswe_timestamp_get_gregorian_minute(timestamp, NULL)
        );

    gtk_spin_button_set_value(
            GTK_SPIN_BUTTON(priv->second),
            (gdouble)gswe_timestamp_get_gregorian_second(timestamp, NULL)
        );

    gtk_spin_button_set_value(
            GTK_SPIN_BUTTON(priv->timezone),
            gswe_timestamp_get_gregorian_timezone(timestamp)
        );
}

/**
 * ag_chart_edit_get_to_timestamp:
 * @chart_edit: an #AgChartEdit
 * @timestamp: if not #NULL, this #GsweTimestamp will be set to the
 *             timestamp according to @chart_edit’s data
 *
 * Returns: (transfer full): if @timestamp was NULL, a new #GsweTimestamp
 *          object with the timestamp set on @chart_edit. Otherwise, it
 *          returns @timestamp after modifying it.
 */
GsweTimestamp *
ag_chart_edit_get_to_timestamp(AgChartEdit *chart_edit,
                               GsweTimestamp *timestamp)
{
    GET_PRIV(chart_edit);

    gint  year       = gtk_spin_button_get_value_as_int(
            GTK_SPIN_BUTTON(priv->year)
        );
    guint month      = gtk_spin_button_get_value_as_int(
            GTK_SPIN_BUTTON(priv->month)
        ),
          day        = gtk_spin_button_get_value_as_int(
            GTK_SPIN_BUTTON(priv->day)
        ),
          hour       = gtk_spin_button_get_value_as_int(
            GTK_SPIN_BUTTON(priv->hour)
        ),
          minute     = gtk_spin_button_get_value_as_int(
            GTK_SPIN_BUTTON(priv->minute)
        ),
          second     = gtk_spin_button_get_value_as_int(
            GTK_SPIN_BUTTON(priv->second)
        );
    gdouble timezone = gtk_spin_button_get_value(
            GTK_SPIN_BUTTON(priv->timezone)
        );

     if (timestamp && GSWE_IS_TIMESTAMP(timestamp)) {
        gswe_timestamp_set_gregorian_full(
                timestamp,
                year, month, day,
                hour, minute, second, 0,
                timezone,
                NULL
            );

        return timestamp;
    } else {
        return gswe_timestamp_new_from_gregorian_full(
                year, month, day,
                hour, minute, second, 0,
                timezone
            );
    }
}

static void
ag_chart_edit_dispose(GObject *gobject)
{
    G_OBJECT_CLASS(ag_chart_edit_parent_class)->dispose(gobject);
}

static void
ag_chart_edit_finalize(GObject *gobject)
{
    g_signal_handlers_destroy(gobject);
    G_OBJECT_CLASS(ag_chart_edit_parent_class)->finalize(gobject);
}

static void
ag_chart_edit_set_property(GObject      *gobject,
                           guint        prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
    AgChartEdit *chart_edit = AG_CHART_EDIT(gobject);

    switch (prop_id) {
        case PROP_NAME:
            ag_chart_edit_set_name(chart_edit, g_value_get_string(value));

            break;

        case PROP_COUNTRY:
            ag_chart_edit_set_country(chart_edit, g_value_get_string(value));

            break;

        case PROP_CITY:
            ag_chart_edit_set_city(chart_edit, g_value_get_string(value));

            break;

        case PROP_LONGITUDE:
            ag_chart_edit_set_longitude(chart_edit, g_value_get_double(value));

            break;

        case PROP_LATITUDE:
            ag_chart_edit_set_latitude(chart_edit, g_value_get_double(value));

            break;

        case PROP_TIMESTAMP:
            ag_chart_edit_set_from_timestamp(chart_edit, GSWE_TIMESTAMP(g_value_get_object(value)));

            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, pspec);

            break;
    }
}                           

static void
ag_chart_edit_get_property(GObject    *gobject, 
                           guint      prop_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
    AgChartEdit        *chart_edit = AG_CHART_EDIT(gobject);

    switch (prop_id) {
        case PROP_NAME:
            g_value_set_string(value, ag_chart_edit_get_name(chart_edit));

            break;

        case PROP_COUNTRY:
            g_value_set_string(value, ag_chart_edit_get_country(chart_edit));

            break;

        case PROP_CITY:
            g_value_set_string(value, ag_chart_edit_get_city(chart_edit));

            break;

        case PROP_LONGITUDE:
            g_value_set_double(value, ag_chart_edit_get_longitude(chart_edit));

            break;

        case PROP_LATITUDE:
            g_value_set_double(value, ag_chart_edit_get_latitude(chart_edit));

            break;

        case PROP_TIMESTAMP:
            g_value_set_object(value, ag_chart_edit_get_to_timestamp(chart_edit, NULL));

            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, pspec);

            break;
    }
}

static gboolean
ag_chart_edit_find_city(GtkTreeModel     *model,
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
ag_chart_edit_city_changed_cb(GtkSearchEntry *city, AgChartEdit *chart_edit)
{
    struct cc_search search;
    GET_PRIV(chart_edit);

    search.target   = gtk_entry_get_text(GTK_ENTRY(city));
    search.ret_iter = NULL;

    gtk_tree_model_foreach(
            city_list,
            (GtkTreeModelForeachFunc)ag_chart_edit_find_city,
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
ag_chart_edit_name_changed_cb(GtkEntry *name_entry, AgChartEdit *chart_edit)
{
    g_signal_emit(chart_edit, signals[SIGNAL_NAME_CHANGED], 0);
}

static void
ag_chart_edit_class_init (AgChartEditClass *klass)
{
    GObjectClass   *gobject_class = (GObjectClass *)klass;
    GtkWidgetClass *widget_class  = GTK_WIDGET_CLASS(klass);

    gobject_class->dispose = ag_chart_edit_dispose;
    gobject_class->finalize = ag_chart_edit_finalize;
    gobject_class->set_property = ag_chart_edit_set_property;
    gobject_class->get_property = ag_chart_edit_get_property;

    signals[SIGNAL_NAME_CHANGED] = g_signal_new(
            "name-changed",
            G_TYPE_FROM_CLASS(klass),
            G_SIGNAL_RUN_FIRST,
            0, NULL, NULL,
            g_cclosure_marshal_generic, G_TYPE_NONE, 0
        );

    properties[PROP_NAME] = g_param_spec_string(
            "name",
            "Name",
            "The person's name",
            NULL,
            G_PARAM_STATIC_NICK
            | G_PARAM_STATIC_NAME
            | G_PARAM_STATIC_BLURB
            | G_PARAM_READABLE
            | G_PARAM_WRITABLE
        );
    g_object_class_install_property(
            gobject_class,
            PROP_NAME,
            properties[PROP_NAME]
        );

    properties[PROP_COUNTRY] = g_param_spec_string(
            "country",
            "Country",
            "Country of birth",
            NULL,
            G_PARAM_STATIC_NICK
            | G_PARAM_STATIC_NAME
            | G_PARAM_STATIC_BLURB
            | G_PARAM_READABLE
            | G_PARAM_WRITABLE
        );
    g_object_class_install_property(
            gobject_class,
            PROP_COUNTRY,
            properties[PROP_COUNTRY]
        );

    properties[PROP_CITY] = g_param_spec_string(
            "city",
            "City",
            "City of birth",
            NULL,
            G_PARAM_STATIC_NICK
            | G_PARAM_STATIC_NAME
            | G_PARAM_STATIC_BLURB
            | G_PARAM_READABLE
            | G_PARAM_WRITABLE
        );
    g_object_class_install_property(
            gobject_class,
            PROP_CITY,
            properties[PROP_CITY]
        );

    properties[PROP_LONGITUDE] = g_param_spec_double(
            "longitude",
            "Longitude",
            "Longitude of birth",
            -180.0, 180.0,
            0.0,
            G_PARAM_STATIC_NICK
            | G_PARAM_STATIC_NAME
            | G_PARAM_STATIC_BLURB
            | G_PARAM_READABLE
            | G_PARAM_WRITABLE
        );
    g_object_class_install_property(
            gobject_class,
            PROP_LONGITUDE,
            properties[PROP_LONGITUDE]
        );

    properties[PROP_LATITUDE] = g_param_spec_double(
            "latitude",
            "Latitude",
            "Latitude of birth",
            -90.0, 90.0,
            0.0,
            G_PARAM_STATIC_NICK
            | G_PARAM_STATIC_NAME
            | G_PARAM_STATIC_BLURB
            | G_PARAM_READABLE
            | G_PARAM_WRITABLE
        );
    g_object_class_install_property(
            gobject_class,
            PROP_LATITUDE,
            properties[PROP_LATITUDE]
        );

    properties[PROP_TIMESTAMP] = g_param_spec_object(
            "timestamp",
            "Timestamp",
            "Timestamp of birth",
            GSWE_TYPE_TIMESTAMP, 
            G_PARAM_STATIC_NICK
            | G_PARAM_STATIC_NAME
            | G_PARAM_STATIC_BLURB
            | G_PARAM_READABLE
            | G_PARAM_WRITABLE
        );
    g_object_class_install_property(
            gobject_class,
            PROP_TIMESTAMP,
            properties[PROP_TIMESTAMP]
        );

    properties[PROP_NOTE] = g_param_spec_string(
            "note",
            "Note",
            "Chart notes",
            NULL,
            G_PARAM_STATIC_NICK
            | G_PARAM_STATIC_NAME
            | G_PARAM_STATIC_BLURB
            | G_PARAM_READABLE
            | G_PARAM_WRITABLE
        );
    g_object_class_install_property(
            gobject_class,
            PROP_NOTE,
            properties[PROP_NOTE]
        );

    gtk_widget_class_set_template_from_resource(
            widget_class,
            "/eu/polonkai/gergely/Astrognome/ui/ag-chart-edit.ui"
        );

    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgChartEdit,
            name
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgChartEdit,
            country
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgChartEdit,
            country_comp
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgChartEdit,
            city
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgChartEdit,
            city_comp
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgChartEdit,
            year
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgChartEdit,
            month
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgChartEdit,
            day
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgChartEdit,
            hour
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgChartEdit,
            minute
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgChartEdit,
            second
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgChartEdit,
            timezone
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgChartEdit,
            north_lat
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgChartEdit,
            south_lat
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgChartEdit,
            east_long
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgChartEdit,
            west_long
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgChartEdit,
            latitude
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgChartEdit,
            longitude
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgChartEdit,
            year_adjust
        );
    gtk_widget_class_bind_template_child_private(
            widget_class,
            AgChartEdit,
            note_buffer
        );

    gtk_widget_class_bind_template_callback(
            widget_class,
            ag_chart_edit_country_changed_cb
        );
    gtk_widget_class_bind_template_callback(
            widget_class,
            ag_chart_edit_city_changed_cb
        );
    gtk_widget_class_bind_template_callback(
            widget_class,
            ag_chart_edit_name_changed_cb
        );
}

static gboolean
ag_chart_edit_city_matches(GtkEntryCompletion *city_comp,
                           const gchar        *key,
                           GtkTreeIter        *iter,
                           AgChartEdit        *chart_edit)
{
    GET_PRIV(chart_edit);
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

static void
ag_chart_edit_init (AgChartEdit *chart_edit)
{
    GET_PRIV(chart_edit);

    gtk_widget_init_template(GTK_WIDGET(chart_edit));

    g_object_set(
            priv->year_adjust,
            "lower", (gdouble)G_MININT,
            "upper", (gdouble)G_MAXINT,
            NULL
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
            (GtkEntryCompletionMatchFunc)ag_chart_edit_city_matches,
            chart_edit,
            NULL
        );

}

void
ag_chart_edit_set_note(AgChartEdit *chart_edit, const gchar *note)
{
    GET_PRIV(chart_edit);

    if (note) {
        // TODO: maybe setting length to -1 here is not that good of an idea…
        gtk_text_buffer_set_text(priv->note_buffer, note, -1);
    } else {
        gtk_text_buffer_set_text(priv->note_buffer, "", 0);
    }
}

gchar *
ag_chart_edit_get_note(AgChartEdit *chart_edit)
{
    GET_PRIV(chart_edit);
    GtkTextIter start_iter,
                end_iter;

    gtk_text_buffer_get_bounds(priv->note_buffer, &start_iter, &end_iter);

    return gtk_text_buffer_get_text(priv->note_buffer, &start_iter, &end_iter, TRUE);
}

/**
 * ag_chart_edit_update:
 * @chart_edit: an #AgChartEdit
 *
 * If the current widget is a spin button, force it to update. This is
 * required when the user enters a new value in a spin button, but
 * doesn't leave the spin entry before switching tabs with an accel.
 */
void
ag_chart_edit_update(AgChartEdit *chart_edit)
{
    GtkWidget *current;
    GtkWindow *window = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(chart_edit)));

    current = gtk_window_get_focus(window);

    if (GTK_IS_SPIN_BUTTON(current)) {
        gtk_spin_button_update(GTK_SPIN_BUTTON(current));
    }
}

AgDbChartSave *
ag_chart_edit_get_chart_save(AgChartEdit *chart_edit)
{
    AgDbChartSave *edit_data;

    edit_data = ag_db_chart_save_new(TRUE);

    edit_data->name      = g_strdup(ag_chart_edit_get_name(chart_edit));
    edit_data->country   = g_strdup(ag_chart_edit_get_country(chart_edit));
    edit_data->city      = g_strdup(ag_chart_edit_get_city(chart_edit));
    edit_data->longitude = ag_chart_edit_get_longitude(chart_edit);
    edit_data->latitude  = ag_chart_edit_get_latitude(chart_edit);
    // TODO: This will cause problems with imported charts
    edit_data->altitude  = DEFAULT_ALTITUDE;
    edit_data->year      = ag_chart_edit_get_year(chart_edit);
    edit_data->month     = ag_chart_edit_get_month(chart_edit);
    edit_data->day       = ag_chart_edit_get_day(chart_edit);
    edit_data->hour      = ag_chart_edit_get_hour(chart_edit);
    edit_data->minute    = ag_chart_edit_get_minute(chart_edit);
    edit_data->second    = ag_chart_edit_get_second(chart_edit);
    edit_data->timezone  = ag_chart_edit_get_timezone(chart_edit);
    edit_data->note      = ag_chart_edit_get_note(chart_edit);

    return edit_data;
}

void
ag_chart_edit_clear(AgChartEdit *chart_edit)
{
    GET_PRIV(chart_edit);

    /* Empty edit tab values */
    gtk_entry_set_text(GTK_ENTRY(priv->name), "");
    gtk_entry_set_text(GTK_ENTRY(priv->country), "");
    gtk_entry_set_text(GTK_ENTRY(priv->city), "");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(priv->year), (gdouble)1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(priv->month), (gdouble)1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(priv->day), (gdouble)1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(priv->hour), (gdouble)0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(priv->minute), (gdouble)0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(priv->second), (gdouble)0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(priv->timezone), 0.0);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->north_lat), TRUE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->east_long), TRUE);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(priv->longitude), 0.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(priv->latitude), 0.0);
    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(priv->note_buffer), "", 0);
}
