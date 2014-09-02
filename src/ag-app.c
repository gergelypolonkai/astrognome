#include <glib/gi18n.h>
#include <webkit2/webkit2.h>

#include "ag-app.h"
#include "ag-window.h"
#include "ag-chart.h"
#include "ag-preferences.h"
#include "config.h"
#include "astrognome.h"

typedef struct _AgAppPrivate {
    WebKitUserContentManager *manager;
} AgAppPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(AgApp, ag_app, GTK_TYPE_APPLICATION);

GtkWindow *
ag_app_peek_first_window(AgApp *app)
{
    GList *l;

    for (
                l = gtk_application_get_windows(GTK_APPLICATION(app));
                l;
                l = g_list_next(l)
            ) {
        if (GTK_IS_WINDOW(l->data)) {
            return GTK_WINDOW(l->data);
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
    AgAppPrivate *priv = ag_app_get_instance_private(app);

    window = ag_window_new(app, priv->manager);
    gtk_application_add_window(GTK_APPLICATION(app), GTK_WINDOW(window));
    gtk_widget_show_all(window);

    return window;
}

static void
new_window_cb(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    AgWindow *window = AG_WINDOW(ag_app_create_window(AG_APP(user_data)));

    ag_window_load_chart_list(window);
    ag_window_change_tab(window, "list");
}

static void
preferences_cb(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    AgApp *app = AG_APP(user_data);

    ag_preferences_show_dialog(
            gtk_application_get_active_window(GTK_APPLICATION(app))
        );
}

static void
about_cb(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    const gchar *authors[] = {
        "Gergely Polonkai <gergely@polonkai.eu>",
        "Jean-André Santoni <jean.andre.santoni@gmail.com>",
        NULL
    };
    const gchar *artists[] = {
        "Ákos Szimmer <akos.szimmer@gmail.com>",
        NULL
    };
    const gchar *translator_credits = _("translator_credits");
    AgApp       *app                = AG_APP(user_data);

    /* i18n: Please don't translate "Astrognome" (it's marked as translatable
     * for transliteration only */
    gtk_show_about_dialog(
            gtk_application_get_active_window(GTK_APPLICATION(app)),
            "name", _("Astrognome"),
            "version", PACKAGE_VERSION,
            "comments", _("Astrologers' software for GNOME"),
            "authors", authors,
            "artists", artists,
            "translator_credits", ((strcmp(
                                           translator_credits,
                                           "translator_credits"
                                       ) != 0)
                                   ? translator_credits
                                   : NULL),
            "website", PACKAGE_URL,
            "website-label", _("Astrognome Website"),
            "license-type", GTK_LICENSE_GPL_3_0,
            "logo-icon-name", PACKAGE_TARNAME,
            NULL
        );
}

static void
quit_cb(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    GList *l;

    while ((l = gtk_application_get_windows(GTK_APPLICATION(user_data)))) {
        gtk_application_remove_window(
                GTK_APPLICATION(user_data),
                GTK_WINDOW(l->data)
            );
    }
}

static void
ag_app_import_file(AgApp *app, GFile *file, AgAppImportType type)
{
    GtkWidget *window;
    AgChart   *chart;
    GError    *err = NULL;

    switch (type) {
        case AG_APP_IMPORT_AGC:
            chart = ag_chart_load_from_agc(file, &err);

            break;

        case AG_APP_IMPORT_HOR:
            chart = ag_chart_load_from_placidus_file(file, &err);

            break;

        default:
            g_error("Unknown import type!");

            break;
    }

    if (chart == NULL) {
        ag_app_message_dialog(
                gtk_application_get_active_window(GTK_APPLICATION(app)),
                GTK_MESSAGE_ERROR,
                "Error while loading: %s",
                err->message
            );

        return;
    }

    window = ag_app_create_window(app);
    ag_window_set_chart(AG_WINDOW(window), chart);
    ag_window_update_from_chart(AG_WINDOW(window));
    g_action_group_activate_action(G_ACTION_GROUP(window), "save", NULL);
    ag_window_change_tab(AG_WINDOW(window), "chart");
}

static void
ag_app_import_cb(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    gint            response;
    GtkWidget       *fs;
    GtkFileFilter   *filter;
    GSList          *filenames   = NULL;
    const gchar     *target_type = g_variant_get_string(parameter, NULL);
    AgAppImportType type         = AG_APP_IMPORT_NONE;
    AgApp           *app         = AG_APP(user_data);

    if (strncmp("agc", target_type, 3) == 0) {
        type = AG_APP_IMPORT_AGC;
        filter = filter_chart;
    } else if (strncmp("hor", target_type, 3) == 0) {
        type = AG_APP_IMPORT_HOR;
        filter = filter_hor;
    } else {
        g_error("Unknown import type!");
    }

    fs = gtk_file_chooser_dialog_new(
            _("Select charts"),
            gtk_application_get_active_window(GTK_APPLICATION(app)),
            GTK_FILE_CHOOSER_ACTION_OPEN,
            _("_Cancel"), GTK_RESPONSE_CANCEL,
            _("_Import"), GTK_RESPONSE_ACCEPT,
            NULL
        );
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fs), filter_all);
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fs), filter);
    gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(fs), filter);
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
            char  *data = l->data;

            if (data == NULL) {
                continue;
            }

            file = g_file_new_for_commandline_arg(data);
            ag_app_import_file(AG_APP(user_data), file, type);
        }
    }

    gtk_widget_destroy(fs);
}

static void
raise_cb(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    AgApp     *app = AG_APP(user_data);
    GtkWindow *window;

    window = ag_app_peek_first_window(app);
    gtk_window_present(window);
}

static void
show_help(const gchar *topic, GtkWindow *parent)
{
    gchar     *uri;
    GdkScreen *screen;
    GError    *err = NULL;

    if (topic) {
        uri = g_strdup_printf("help:astrognome/%s", topic);
    } else {
        uri = g_strdup("help:astrognome");
    }

    if (parent) {
        screen = gtk_widget_get_screen(GTK_WIDGET(parent));
    } else {
        screen = gdk_screen_get_default();
    }

    if (!gtk_show_uri(screen, uri, gtk_get_current_event_time(), &err)) {
        ag_app_message_dialog(
                GTK_WINDOW(parent),
                GTK_MESSAGE_WARNING,
                "Unable to display help: %s",
                err->message
            );
    }

    g_free(uri);
}

static void
help_cb(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    show_help(NULL, NULL);
}

static GActionEntry app_entries[] = {
    { "new-window",  new_window_cb,    NULL, NULL, NULL },
    { "preferences", preferences_cb,   NULL, NULL, NULL },
    { "about",       about_cb,         NULL, NULL, NULL },
    { "quit",        quit_cb,          NULL, NULL, NULL },
    { "raise",       raise_cb,         NULL, NULL, NULL },
    { "import",      ag_app_import_cb, "s",  NULL, NULL },
    { "help",        help_cb,          NULL, NULL, NULL },
};

static void
setup_actions(AgApp *app)
{
    g_action_map_add_action_entries(
            G_ACTION_MAP(app),
            app_entries,
            G_N_ELEMENTS(app_entries),
            app
        );
}

const gchar *action_accels[] = {
    "win.close",               "<Primary>W",        NULL,
    "win.save",                "<Primary>S",        NULL,
    "win.export",              "<Primary><Shift>E", NULL,
    "win.gear-menu",           "F10",               NULL,
    "app.help",                "F1",                NULL,
    "win.change-tab::chart",   "F5",                NULL,
    "win.change-tab::aspects", "F6",                NULL,
    "win.change-tab::points",  "F7",                NULL,
    "win.change-tab::edit",    "F4",                NULL,
    NULL
};

static void
setup_accelerators(AgApp *app)
{
    const char **it;

    for (it = action_accels; it[0]; it += g_strv_length((gchar **)it) + 1) {
        gtk_application_set_accels_for_action(
                GTK_APPLICATION(app),
                it[0],
                &it[1]
            );
    }
}

static void
setup_menu(AgApp *app)
{
    GtkBuilder *builder;
    GMenuModel *model;
    GError     *err = NULL;

    builder = gtk_builder_new();

    if (!gtk_builder_add_from_resource(
                builder,
                "/eu/polonkai/gergely/Astrognome/ui/astrognome.ui",
                &err
            )) {
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
ag_app_import(GApplication *gapp,
              GFile **files,
              gint n_files,
              const gchar *hint)
{
    gint i;

    for (i = 0; i < n_files; i++) {
        ag_app_import_file(AG_APP(gapp), files[i], AG_APP_IMPORT_AGC);
    }
}

void
ag_app_run_action(AgApp                   *app,
                  gboolean                is_remote,
                  const AstrognomeOptions *options)
{
    if (options && options->new_window) {
        if (is_remote) {
            ag_app_new_window(app);
        }
    } else if (options && options->quit) {
        ag_app_quit(app);
    } else if (is_remote) { // Keep this option the last one!
        ag_app_raise(app);
    }
}

static void
application_activate_cb(AgApp *app, gpointer user_data)
{
    ag_app_new_window(app);
    ag_app_run_action(app, FALSE, NULL);
}

AgApp *
ag_app_new(void)
{
    AgApp *app;

    /* i18n: Please don't translate "Astrognome" (it's marked as translatable
     * for transliteration only */
    g_set_application_name(_("Astrognome"));

    app = g_object_new(AG_TYPE_APP,
                       "application-id",   "eu.polonkai.gergely.Astrognome",
                       "flags",            G_APPLICATION_HANDLES_OPEN,
                       "register-session", TRUE,
                       NULL);
    g_signal_connect(
            app,
            "activate",
            G_CALLBACK(application_activate_cb),
            NULL
        );

    return app;
}

static void
ag_app_init(AgApp *app)
{
    AgAppPrivate         *priv;
    GBytes               *css_data;
    const gchar          *css_source;
    gsize                css_length;
    WebKitUserStyleSheet *stylesheet;

    priv = ag_app_get_instance_private(app);
    priv->manager = webkit_user_content_manager_new();

    css_data = g_resources_lookup_data(
            "/eu/polonkai/gergely/Astrognome/ui/chart-default.css",
            G_RESOURCE_LOOKUP_FLAGS_NONE,
            NULL
        );

    if ((css_source = g_bytes_get_data(css_data, &css_length)) != NULL) {
        stylesheet = webkit_user_style_sheet_new(
                css_source,
                WEBKIT_USER_CONTENT_INJECT_TOP_FRAME,
                WEBKIT_USER_STYLE_LEVEL_USER,
                NULL, NULL
            );
        webkit_user_content_manager_add_style_sheet(priv->manager, stylesheet);
        webkit_user_style_sheet_unref(stylesheet);
    }

    g_bytes_unref(css_data);
}

static void
ag_app_class_init(AgAppClass *klass)
{
    GApplicationClass *application_class = G_APPLICATION_CLASS(klass);

    application_class->startup = startup;
    application_class->open    = ag_app_import;
}

GtkResponseType
ag_app_buttoned_dialog(GtkWindow      *window,
                       GtkMessageType message_type,
                       const gchar    *message,
                       const gchar    *first_button_text,
                       ...)
{
    va_list     ap;
    const gchar *button_text;
    gint        response_id;
    GtkWidget   *dialog;
    GtkWindow   *parent = NULL;

    g_return_val_if_fail(GTK_IS_WINDOW(window), GTK_RESPONSE_NONE);

    dialog = gtk_message_dialog_new(
            parent,
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            message_type,
            GTK_BUTTONS_NONE,
            "%s",
            message
        );

    if (first_button_text) {
        button_text = first_button_text;

        va_start(ap, first_button_text);
        response_id = va_arg(ap, gint);
        gtk_dialog_add_button(GTK_DIALOG(dialog), button_text, response_id);

        while ((button_text = va_arg(ap, gchar *)) != NULL) {
            response_id = va_arg(ap, gint);
            gtk_dialog_add_button(GTK_DIALOG(dialog), button_text, response_id);
        }

        va_end(ap);
    }

    response_id = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    return response_id;
}

void
ag_app_message_dialog(GtkWindow      *window,
                      GtkMessageType message_type,
                      gchar          *fmt, ...)
{
    gchar     *msg;
    va_list   args;

    va_start(args, fmt);
    msg = g_strdup_vprintf(fmt, args);
    va_end(args);

    ag_app_buttoned_dialog(
            window,
            message_type,
            msg,
            _("Close"), GTK_RESPONSE_CLOSE,
            NULL
        );

    g_free(msg);
}
