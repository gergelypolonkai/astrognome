#include <glib/gi18n.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <errno.h>

#include "ag-app.h"
#include "ag-window.h"
#include "config.h"
#include "astrognome.h"

typedef enum {
    XML_CONVERT_STRING,
    XML_CONVERT_DOUBLE,
    XML_CONVERT_INT
} XmlConvertType;

struct _AgAppPrivate {
};

G_DEFINE_TYPE(AgApp, ag_app, GTK_TYPE_APPLICATION);

GtkWindow *
ag_app_peek_first_window(AgApp *app)
{
    GList *l;

    for (l = gtk_application_get_windows(GTK_APPLICATION(app)); l; l = g_list_next(l)) {
        if (GTK_IS_WINDOW(l->data)) {
            return (GTK_WINDOW(l->data));
        }
    }

    ag_app_new_window(app);

    return ag_app_peek_first_window(app);
}

void
ag_app_new_window(AgApp *app)
{
    g_action_group_activate_action(G_ACTION_GROUP(app), "new-window", NULL);
}

void
ag_app_quit(AgApp *app)
{
    g_action_group_activate_action(G_ACTION_GROUP(app), "quit", NULL);
}

void
ag_app_raise(AgApp *app)
{
    g_action_group_activate_action(G_ACTION_GROUP(app), "raise", NULL);
}

static GtkWidget *
ag_app_create_window(AgApp *app)
{
    GtkWidget *window;

    window = ag_window_new(app);
    gtk_application_add_window(GTK_APPLICATION(app), GTK_WINDOW(window));
    gtk_widget_show_all(window);

    return window;
}

static void
new_window_cb(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    ag_app_create_window(AG_APP(user_data));
}

static void
preferences_cb(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    //ag_preferences_show_dialog();
}

static void
about_cb(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    const gchar *authors[] = {
        "Gergely Polonkai <gergely@polonkai.eu>",
        "Jean-André Santoni <jean.andre.santoni@gmail.com>",
        NULL
    };

    const gchar **documentors = NULL;
    const gchar *translator_credits = _("translator_credits");

    /* i18n: Please don't translate "Astrognome" (it's marked as translatable for transliteration only */
    gtk_show_about_dialog(NULL,
            "name", _("Astrognome"),
            "version", PACKAGE_VERSION,
            "comments", _("Astrologers' software for GNOME"),
            "authors", authors,
            "documentors", documentors,
            "translator_credits", ((strcmp(translator_credits, "translator_credits") != 0) ? translator_credits : NULL),
            "website", PACKAGE_URL,
            "website-label", _("Astrognome Website"),
            "logo-icon-name", PACKAGE_TARNAME,
            NULL);
}

static void
quit_cb(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    GList *l;

    while ((l = gtk_application_get_windows(GTK_APPLICATION(user_data)))) {
        gtk_application_remove_window(GTK_APPLICATION(user_data), GTK_WINDOW(l->data));
    }
}

GVariant *
get_by_xpath(xmlXPathContextPtr ctx, const gchar *xpath, XmlConvertType type)
{
    xmlXPathObjectPtr xpathObj;
    const gchar *text;
    char *endptr;
    GVariant *ret = NULL;
    gdouble d;
    gint i;

    if ((xpathObj = xmlXPathEvalExpression((const xmlChar *)xpath, ctx)) == NULL) {
        // TODO: Warn with a popup or similar way
        g_warning("Could not initialize XPath");

        return NULL;
    }

    if (xpathObj->nodesetval == NULL) {
        // TODO: Warn with a popup or similar way
        g_warning("Required element not found. This is not a valid save file!");
        xmlXPathFreeObject(xpathObj);

        return NULL;
    }

    if (xpathObj->nodesetval->nodeNr > 1) {
        // TODO: Warn with a popup or similar way
        g_warning("Too many elements. This is not a valid save file!");
        xmlXPathFreeObject(xpathObj);

        return NULL;
    }

    text = (const gchar *)xpathObj->nodesetval->nodeTab[0]->content;

    switch (type) {
        case XML_CONVERT_STRING:
            ret = g_variant_new_string(text);

            break;

        case XML_CONVERT_DOUBLE:
            d = g_ascii_strtod(text, &endptr);

            if ((*endptr != 0) || (errno != 0)) {
                ret = NULL;
            } else {
                ret = g_variant_new_double(d);
            }

            break;

        case XML_CONVERT_INT:
            i = strtol(text, &endptr, 10);

            if ((*endptr != 0) || (errno != 0)) {
                ret = NULL;
            } else {
                ret = g_variant_new_int32(i);
            }

            break;

    }

    xmlXPathFreeObject(xpathObj);

    return ret;
}

static void
ag_app_open_chart(AgApp *app, GFile *file)
{
    GError *err = NULL;
    gchar *uri,
          *xml;
    guint length;
    xmlDocPtr doc;
    xmlXPathContextPtr xpathCtx;
    GVariant *chart_name,
             *country,
             *city,
             *longitude,
             *latitude,
             *altitude,
             *year,
             *month,
             *day,
             *hour,
             *minute,
             *second;

    uri = g_file_get_uri(file);

    if (!g_file_load_contents(file, NULL, &xml, &length, NULL, &err)) {
        // TODO: Warn with a popup or similar way
        g_warning("Could not open file '%s': %s", uri, err->message);
        g_clear_error(&err);
        g_free(uri);

        return;
    }

    if ((doc = xmlReadMemory(xml, length, "chart.xml", NULL, 0)) == NULL) {
        // TODO: Warn with a popup or similar way
        g_warning("Saved chart is corrupt (or not a saved chart at all)");
        g_free(xml);
        g_free(uri);

        return;
    }

    if ((xpathCtx = xmlXPathNewContext(doc)) == NULL) {
        // TODO: Warn with a popup or similar way
        g_warning("Could not initialize XPath");
        xmlFreeDoc(doc);
        g_free(xml);
        g_free(uri);

        return;
    }

    chart_name = get_by_xpath(xpathCtx, "/chartinfo/data/name/text()", XML_CONVERT_STRING);
    country = get_by_xpath(xpathCtx, "/chartinfo/data/place/country/text()", XML_CONVERT_STRING);
    city = get_by_xpath(xpathCtx, "/chartinfo/data/place/city/text()", XML_CONVERT_STRING);
    longitude = get_by_xpath(xpathCtx, "/chartinfo/data/place/longitude/text()", XML_CONVERT_DOUBLE);
    latitude = get_by_xpath(xpathCtx, "/chartinfo/data/place/latitude/text()", XML_CONVERT_DOUBLE);
    altitude = get_by_xpath(xpathCtx, "/chartinfo/data/place/altitude/text()", XML_CONVERT_DOUBLE);
    year = get_by_xpath(xpathCtx, "/chartinfo/data/time/year/text()", XML_CONVERT_INT);
    month = get_by_xpath(xpathCtx, "/chartinfo/data/time/month/text()", XML_CONVERT_INT);
    day = get_by_xpath(xpathCtx, "/chartinfo/data/time/day/text()", XML_CONVERT_INT);
    hour = get_by_xpath(xpathCtx, "/chartinfo/data/time/hour/text()", XML_CONVERT_INT);
    minute = get_by_xpath(xpathCtx, "/chartinfo/data/time/minute/text()", XML_CONVERT_INT);
    second = get_by_xpath(xpathCtx, "/chartinfo/data/time/second/text()", XML_CONVERT_INT);

    g_variant_unref(chart_name);
    g_variant_unref(country);
    g_variant_unref(city);
    g_variant_unref(longitude);
    g_variant_unref(latitude);
    g_variant_unref(altitude);
    g_variant_unref(year);
    g_variant_unref(month);
    g_variant_unref(day);
    g_variant_unref(hour);
    g_variant_unref(minute);
    g_variant_unref(second);

    g_free(xml);
    g_free(uri);
    xmlXPathFreeContext(xpathCtx);
    xmlFreeDoc(doc);
}

static void
open_cb(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    gint response;
    GtkWidget *fs;
    GSList *filenames = NULL;

    fs = gtk_file_chooser_dialog_new(_("Select charts"),
       NULL,
       GTK_FILE_CHOOSER_ACTION_OPEN,
       GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
       GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
       NULL);
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fs), filter_all);
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fs), filter_chart);
    gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(fs), filter_chart);
    gtk_dialog_set_default_response(GTK_DIALOG(fs), GTK_RESPONSE_ACCEPT);
    gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(fs), TRUE);
    gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(fs), FALSE);

    response = gtk_dialog_run(GTK_DIALOG(fs));

    if (response == GTK_RESPONSE_ACCEPT) {
        filenames = gtk_file_chooser_get_uris(GTK_FILE_CHOOSER(fs));
    }

    if (filenames != NULL) {
        GSList *l;

        for (l = filenames; l; l = g_slist_next(l)) {
            GFile *file;
            char *data = l->data;

            if (data == NULL) {
                continue;
            }

            file = g_file_new_for_commandline_arg(data);
            ag_app_open_chart(AG_APP(user_data), file);
        }
    }

    gtk_widget_destroy(fs);
}

static void
raise_cb(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    AgApp *app = AG_APP(user_data);
    GtkWindow *window;

    window = ag_app_peek_first_window(app);
    gtk_window_present(window);
}

static GActionEntry app_entries[] = {
    { "new-window",  new_window_cb,  NULL, NULL, NULL },
    { "preferences", preferences_cb, NULL, NULL, NULL },
    { "about",       about_cb,       NULL, NULL, NULL },
    { "quit",        quit_cb,        NULL, NULL, NULL },
    { "raise",       raise_cb,       NULL, NULL, NULL },
    { "open",        open_cb,        NULL, NULL, NULL },
};

static void
setup_actions(AgApp *app)
{
    g_action_map_add_action_entries(G_ACTION_MAP(app), app_entries, G_N_ELEMENTS(app_entries), app);
}

static void
setup_accelerators(AgApp *app)
{
    gtk_application_add_accelerator(GTK_APPLICATION(app), "<Primary>w", "win.close",     NULL);
    gtk_application_add_accelerator(GTK_APPLICATION(app), "<Primary>s", "win.save",      NULL);
    gtk_application_add_accelerator(GTK_APPLICATION(app), "F10",        "win.gear-menu", NULL);
}

static void
setup_menu(AgApp *app)
{
    GtkBuilder *builder;
    GMenuModel *model;
    GError *err = NULL;

    builder = gtk_builder_new();

    if (!gtk_builder_add_from_resource(builder, "/eu/polonkai/gergely/astrognome/astrognome.ui", &err)) {
        g_error("%s", (err) ? err->message : "unknown error");
    }

    model = G_MENU_MODEL(gtk_builder_get_object(builder, "app-menu"));
    gtk_application_set_app_menu(GTK_APPLICATION(app), model);

    g_object_unref(builder);
}

static void
startup(GApplication *gapp)
{
    AgApp *app = AG_APP(gapp);

    G_APPLICATION_CLASS(ag_app_parent_class)->startup(gapp);

    setup_actions(app);
    setup_menu(app);
    setup_accelerators(app);
}

static void
ag_app_open(GApplication *gapp, GFile **files, gint n_files, const gchar *hint)
{
    gint i;

    for (i = 0; i < n_files; i++) {
        ag_app_open_chart(AG_APP(gapp), files[i]);
    }
}

AgApp *
ag_app_new(void)
{
    AgApp *app;

    /* i18n: Please don't translate "Astrognome" (it's marked as translatable for transliteration only */
    g_set_application_name(_("Astrognome"));

    app = g_object_new(AG_TYPE_APP,
            "application-id",   "eu.polonkai.gergely.Astrognome",
            "flags",            G_APPLICATION_HANDLES_OPEN,
            "register-session", TRUE,
            NULL);

    return app;
}

static void
ag_app_init(AgApp *app)
{
}

static void
ag_app_class_init(AgAppClass *klass)
{
    GApplicationClass *application_class = G_APPLICATION_CLASS(klass);

    application_class->startup = startup;
    application_class->open = ag_app_open;
}

