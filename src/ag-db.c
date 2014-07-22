#include <gio/gio.h>
#include <gobject/gobject.h>
#include <libgda/libgda.h>
#include <sql-parser/gda-sql-parser.h>
#include <gobject/gvaluecollector.h>

#include "config.h"
#include "ag-app.h"
#include "ag-db.h"

#define SCHEMA_VERSION 1

static AgDb *singleton = NULL;

typedef struct _AgDbPrivate {
    gchar         *dsn;
    GdaConnection *conn;
} AgDbPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(AgDb, ag_db, G_TYPE_OBJECT);

static void
ag_db_non_select(AgDb *db, const gchar *sql)
{
    GdaStatement *sth;
    gint         nrows;
    const gchar  *remain;
    GdaSqlParser *parser;
    AgDbPrivate  *priv = ag_db_get_instance_private(db);
    GError       *err = NULL;

    parser = g_object_get_data(G_OBJECT(priv->conn), "parser");
    g_assert(GDA_IS_SQL_PARSER(parser));

    if ((sth = gda_sql_parser_parse_string(
                parser,
                sql,
                &remain,
                &err
            )) == NULL) {
        g_error(
                "SQL error: %s",
                (err && err->message)
                    ? err->message
                    : "no reason"
            );
    }

    if ((nrows = gda_connection_statement_execute_non_select(
                 priv->conn,
                 sth,
                 NULL,
                 NULL,
                 &err
             )) == -1) {
        g_error(
                "SQL error: %s",
                (err && err->message)
                    ? err->message
                    : "no details"
            );
    }

    g_object_unref(sth);
}

static gboolean
ag_db_table_exists(AgDb *db, const gchar *table)
{
    GdaSqlParser *parser;
    GdaStatement *sth;
    GdaDataModel *result;
    GdaSet       *params;
    GdaHolder    *holder;
    const gchar  *remain;
    gboolean     ret;
    GValue       table_value = G_VALUE_INIT;
    AgDbPrivate  *priv       = ag_db_get_instance_private(db);
    GError       *err        = NULL;

    parser = g_object_get_data(G_OBJECT(priv->conn), "parser");

    if ((sth = gda_sql_parser_parse_string(
                parser,
                "SELECT type \n"       \
                "    FROM sqlite_master \n"   \
                "    WHERE type = 'table' \n" \
                "    AND name = ##name::string",
                &remain,
                &err
            )) == NULL) {
        g_error(
                "SQL error: %s",
                (err && err->message)
                    ? err->message
                    : "no reason"
            );
    }

    if (gda_statement_get_parameters(sth, &params, &err) == FALSE) {
        g_error(
                "Params error: %s",
                (err && err->message)
                    ? err->message
                    : "no reason"
            );
    }

    holder = gda_set_get_holder(params, "name");
    g_value_init(&table_value, G_TYPE_STRING);
    g_value_set_string(&table_value, table);
    gda_holder_set_value(holder, &table_value, &err);
    g_value_unset(&table_value);

    result = gda_connection_statement_execute_select(
            priv->conn,
            sth,
            params,
            &err
        );

    if (gda_data_model_get_n_rows(result) > 0) {
        ret = TRUE;
    } else {
        ret = FALSE;
    }

    g_object_unref(result);
    g_object_unref(sth);

    return ret;
}

/**
 * ag_db_select:
 * @db: the database object to work on
 * @err: a #GError or NULL
 * @sql: the query to execute
 * @...: a NULL terminated list of key-value pairs of the query parameters
 *
 * Returns: (transfer full): the #GdaDataModel as the result of the query
 */
static GdaDataModel *
ag_db_select(AgDb *db, GError **err, const gchar *sql, ...)
{
    GdaSqlParser *parser;
    const gchar  *remain;
    GdaSet       *params;
    GdaStatement *sth;
    GdaDataModel *ret;
    gchar        *error = NULL;
    AgDbPrivate  *priv  = ag_db_get_instance_private(db);

    parser = g_object_get_data(G_OBJECT(priv->conn), "parser");

    if ((sth = gda_sql_parser_parse_string(
                parser,
                sql,
                &remain,
                err
            )) == NULL) {
        g_error(
                "SQL error: %s",
                (*err && (*err)->message)
                    ? (*err)->message
                    : "no reason"
            );
    }

    if (!gda_statement_get_parameters(sth, &params, err)) {
        g_error(
                "SQL error: %s",
                (*err && (*err)->message)
                    ? (*err)->message
                    : "no reason"
            );
    }

    if (params) {
        va_list ap;

        va_start(ap, sql);
        while (TRUE) {
            gchar     *key;
            GdaHolder *holder;
            GType     type;
            GValue    value = G_VALUE_INIT;

            if ((key = va_arg(ap, gchar *)) == NULL) {
                break;
            }

            if ((holder = gda_set_get_holder(
                        params,
                        (const gchar *)key
                    )) == NULL) {
                g_error("Error: holder %s is not defined in query.", key);
            }

            type = gda_holder_get_g_type(holder);
            g_value_init(&value, type);
            G_VALUE_COLLECT_INIT(&value, type, ap, 0, &error);

            if (error) {
                g_error("SQL GValue error: %s", error);
            }

            if (!gda_holder_set_value(holder, (const GValue *)&value, err)) {
                g_error(
                        "SQL GdaHolder error: %s",
                        (*err && (*err)->message)
                            ? (*err)->message
                            : "no reason"
                    );
            }
        }
    }

    ret = gda_connection_statement_execute_select(priv->conn, sth, params, err);
    g_object_unref(sth);

    return ret;
}

static void
ag_db_check_version_table(AgDb *db)
{
    if (ag_db_table_exists(db, "version")) {
        GdaDataModel *result;
        gint         ret;

        g_debug(
                "Version table exists. " \
                "Checking if db_version is %d and app_version is %s",
                SCHEMA_VERSION,
                PACKAGE_VERSION
            );
        result = ag_db_select(
                db,
                NULL,
                "SELECT db_version, app_version FROM version",
                NULL
            );

        ret = gda_data_model_get_n_rows(result);

        if (ret < 0) {
            g_error("No number of rows?");
        } else if (ret > 1) {
            g_error("Error in database. Version table has more than one rows!");
        } else if (ret == 0) {
            // TODO: Check schema against current one maybe? If it’s fine, we
            //       may just add a row here.
            g_error("Error in database. Version table has no rows!");
        } else {
            // Version table has one row
            const GValue *value;
            GdaDataModelIter *iter = gda_data_model_create_iter(result);
            gint version;

            gda_data_model_iter_move_next(iter);
            value = gda_data_model_iter_get_value_at(iter, 0);

            if (!G_VALUE_HOLDS_INT(value)) {
                g_error(
                        "Database is invalid. "                     \
                        "version.db_version should be an integer."
                    );
            }

            version = g_value_get_int(value);

            if (version < SCHEMA_VERSION) {
                // TODO
                g_error("Update required!");
            } else if (version > SCHEMA_VERSION) {
                const GValue *app_version_value;
                const gchar *app_version;

                app_version_value = gda_data_model_iter_get_value_at(
                        iter,
                        1
                    );
                app_version = g_value_get_string(
                        app_version_value
                    );

                g_error(
                        "The version of your database is from the future. " \
                        "It seems it was created by Astrognome v%s.",
                        app_version
                    );
            } else {
                g_object_unref(result);
            }
        }
    } else {
        GValue      db_version  = G_VALUE_INIT,
                    app_version = G_VALUE_INIT;
        AgDbPrivate *priv       = ag_db_get_instance_private(db);

        g_value_init(&db_version, G_TYPE_INT);
        g_value_init(&app_version, G_TYPE_STRING);

        g_value_set_int(&db_version, SCHEMA_VERSION);
        g_value_set_static_string(&app_version, PACKAGE_VERSION);

        ag_db_non_select(
                db,
                "CREATE TABLE version ("                \
                    "id INTEGER PRIMARY KEY, "          \
                    "db_version INTEGER UNIQUE NOT NULL, " \
                    "app_version TEXT UNIQUE NOT NULL"  \
                ")"
            );

        gda_connection_insert_row_into_table(
                priv->conn,
                "version",
                NULL,
                "db_version", &db_version,
                "app_version", &app_version,
                NULL
            );
    }
}

static void
ag_db_check_chart_table(AgDb *db)
{
    ag_db_non_select(
            db,
            "CREATE TABLE IF NOT EXISTS chart ("    \
                "id INTEGER PRIMARY KEY, " \
                "name TEXT NOT NULL, " \
                "country_name TEXT, " \
                "city_name TEXT, " \
                "longitude DOUBLE NOT NULL, " \
                "latitude DOUBLE NOT NULL, " \
                "altitude DOUBLE, " \
                "year INTEGER NOT NULL, " \
                "month UNSIGNED INTEGER NOT NULL, " \
                "day UNSIGNED INTEGER NOT NULL, " \
                "hour UNSIGNED INTEGER NOT NULL, " \
                "minute UNSIGNED INTEGER NOT NULL, " \
                "second UNSIGNED INTEGER NOT NULL, " \
                "timezone DOUBLE NOT NULL, " \
                "house_system TEXT NOT NULL, " \
                "note TEXT" \
            ")"
        );
}

static gint
ag_db_verify(AgDb *db)
{
    ag_db_check_version_table(db);
    ag_db_check_chart_table(db);

    return 0;
}

static void
ag_db_init(AgDb *db)
{
    GdaSqlParser *parser;
    GFile        *user_data_dir = g_file_new_for_path(g_get_user_data_dir()),
                 *ag_data_dir   = g_file_get_child(user_data_dir, "astrognome");
    AgDbPrivate  *priv          = ag_db_get_instance_private(db);
    gchar        *path          = g_file_get_path(ag_data_dir);
    GError       *err           = NULL;

    gda_init();

    if (!g_file_query_exists(ag_data_dir, NULL)) {
        gchar *path = g_file_get_path(ag_data_dir);

        if (g_mkdir_with_parents(path, 0700) != 0) {
            g_error(
                    "Data directory %s does not exist and can not be created.",
                    path
                );
        }
    }

    priv->dsn = g_strdup_printf("SQLite://DB_DIR=%s;DB_NAME=charts", path);

    g_free(path);
    g_object_unref(user_data_dir);
    g_object_unref(ag_data_dir);

    priv->conn = gda_connection_open_from_string(
            NULL,
            priv->dsn,
            NULL,
            GDA_CONNECTION_OPTIONS_NONE,
            &err
        );

    if (priv->conn == NULL) {
        g_error(
                "Unable to initialize database: %s",
                (err && err->message)
                    ? err->message
                    : "no reason"
            );
    }

    if ((parser = gda_connection_create_parser(priv->conn)) == NULL) {
        parser = gda_sql_parser_new();
    }

    g_object_set_data_full(
            G_OBJECT(priv->conn),
            "parser",
            parser,
            g_object_unref
        );

    ag_db_verify(db);
}

static void
ag_db_dispose(GObject *gobject)
{
    AgDbPrivate *priv = ag_db_get_instance_private(AG_DB(gobject));

    g_object_unref(priv->conn);
    g_free(priv->dsn);
    G_OBJECT_CLASS(ag_db_parent_class)->dispose(gobject);
}

static void
ag_db_finalize(GObject *gobject)
{
    singleton = NULL;

    G_OBJECT_CLASS(ag_db_parent_class)->finalize(gobject);
}

static void
ag_db_class_init(AgDbClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose  = ag_db_dispose;
    gobject_class->finalize = ag_db_finalize;
}

AgDb *
ag_db_get(void)
{
    if (!singleton) {
        singleton = AG_DB(g_object_new(AG_TYPE_DB, NULL));
    } else {
        g_object_ref(singleton);
    }

    g_assert(singleton);

    return singleton;
}

void
ag_db_save_data_free(AgDbSave *save_data)
{
    if (!save_data) {
        return;
    }

    if (save_data->name) {
        g_free(save_data->name);
    }

    if (save_data->country) {
        g_free(save_data->country);
    }

    if (save_data->city) {
        g_free(save_data->city);
    }

    if (save_data->house_system) {
        g_free(save_data->house_system);
    }

    if (save_data->note) {
        g_free(save_data->note);
    }

    g_free(save_data);
}

gboolean
ag_db_save_chart(AgDb *db, AgDbSave *save_data, GtkWidget *window, GError **err)
{
    GValue      db_id        = G_VALUE_INIT,
                name         = G_VALUE_INIT,
                country      = G_VALUE_INIT,
                city         = G_VALUE_INIT,
                longitude    = G_VALUE_INIT,
                latitude     = G_VALUE_INIT,
                altitude     = G_VALUE_INIT,
                year         = G_VALUE_INIT,
                month        = G_VALUE_INIT,
                day          = G_VALUE_INIT,
                hour         = G_VALUE_INIT,
                minute       = G_VALUE_INIT,
                second       = G_VALUE_INIT,
                timezone     = G_VALUE_INIT,
                house_system = G_VALUE_INIT,
                note         = G_VALUE_INIT;
    AgDbPrivate *priv = ag_db_get_instance_private(db);

    g_value_init(&name, G_TYPE_STRING);
    g_value_set_string(&name, save_data->name);

    g_value_init(&country, G_TYPE_STRING);
    g_value_set_string(&country, save_data->country);

    g_value_init(&city, G_TYPE_STRING);
    g_value_set_string(&city, save_data->city);

    g_value_init(&longitude, G_TYPE_DOUBLE);
    g_value_set_double(&longitude, save_data->longitude);

    g_value_init(&latitude, G_TYPE_DOUBLE);
    g_value_set_double(&latitude, save_data->latitude);

    g_value_init(&altitude, G_TYPE_DOUBLE);
    g_value_set_double(&altitude, save_data->altitude);

    g_value_init(&year, G_TYPE_INT);
    g_value_set_int(&year, save_data->year);

    g_value_init(&month, G_TYPE_UINT);
    g_value_set_uint(&month, save_data->month);

    g_value_init(&day, G_TYPE_UINT);
    g_value_set_uint(&day, save_data->day);

    g_value_init(&hour, G_TYPE_UINT);
    g_value_set_uint(&hour, save_data->hour);

    g_value_init(&minute, G_TYPE_UINT);
    g_value_set_uint(&minute, save_data->minute);

    g_value_init(&second, G_TYPE_UINT);
    g_value_set_uint(&second, save_data->second);

    g_value_init(&timezone, G_TYPE_DOUBLE);
    g_value_set_double(&timezone, save_data->timezone);

    g_value_init(&house_system, G_TYPE_STRING);
    g_value_set_string(&house_system, save_data->house_system);

    g_value_init(&note, G_TYPE_STRING);
    g_value_set_string(&note, save_data->note);

    if (save_data->db_id == -1) {
        if (!gda_connection_insert_row_into_table(
                    priv->conn,
                    "chart",
                    err,
                    "name",         &name,
                    "country_name", &country,
                    "city_name",    &city,
                    "longitude",    &longitude,
                    "latitude",     &latitude,
                    "altitude",     &altitude,
                    "year",         &year,
                    "month",        &month,
                    "day",          &day,
                    "hour",         &hour,
                    "minute",       &minute,
                    "second",       &second,
                    "timezone",     &timezone,
                    "house_system", &house_system,
                    "note",         &note,
                    NULL
                )) {

            ag_app_message_dialog(
                    window,
                    GTK_MESSAGE_ERROR,
                    "Unable to save: %s",
                    (*err && (*err)->message)
                        ? (*err)->message
                        : "no reason"
                );
        }
    } else {
        g_value_init(&db_id, G_TYPE_INT);
        g_value_set_int(&db_id, save_data->db_id);

        if (!gda_connection_update_row_in_table(
                    priv->conn,
                    "chart",
                    "id",
                    &db_id,
                    err,
                    "name",         &name,
                    "country_name", &country,
                    "city_name",    &city,
                    "longitude",    &longitude,
                    "latitude",     &latitude,
                    "altitude",     &altitude,
                    "year",         &year,
                    "month",        &month,
                    "day",          &day,
                    "hour",         &hour,
                    "minute",       &minute,
                    "second",       &second,
                    "timezone",     &timezone,
                    "house_system", &house_system,
                    "note",         &note,
                    NULL
                )) {

            ag_app_message_dialog(
                    window,
                    GTK_MESSAGE_ERROR,
                    "Unable to save: %s",
                    (*err && (*err)->message)
                        ? (*err)->message
                        : "no reason"
                );
        }

        g_value_unset(&db_id);
    }

    g_value_unset(&note);
    g_value_unset(&house_system);
    g_value_unset(&timezone);
    g_value_unset(&second);
    g_value_unset(&minute);
    g_value_unset(&hour);
    g_value_unset(&day);
    g_value_unset(&month);
    g_value_unset(&year);
    g_value_unset(&altitude);
    g_value_unset(&latitude);
    g_value_unset(&longitude);
    g_value_unset(&city);
    g_value_unset(&country);
    g_value_unset(&name);

    return FALSE;
}

GList *
ag_db_get_chart_list(AgDb *db, GError **err)
{
    GdaDataModelIter *iter;
    GList            *ret    = NULL;
    GdaDataModel     *result = ag_db_select(
            db,
            err,
            "SELECT id, name FROM chart ORDER BY name",
            NULL
        );

    if (result == NULL) {
        return NULL;
    }

    iter = gda_data_model_create_iter(result);

    while (gda_data_model_iter_move_next(iter)) {
        const GValue *value;
        AgDbSave     *save_data = g_new0(AgDbSave, 1);

        value = gda_data_model_iter_get_value_at(iter, 0);
        save_data->db_id = g_value_get_int(value);

        value = gda_data_model_iter_get_value_at(iter, 1);
        save_data->name = g_strdup(g_value_get_string(value));

        ret = g_list_prepend(ret, save_data);
    }

    return g_list_reverse(ret);
}
