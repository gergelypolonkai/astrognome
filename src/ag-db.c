/* ag-db.c - Database manipulation functionality for Astrognome
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
#include <gio/gio.h>
#include <gobject/gobject.h>
#include <libgda/libgda.h>
#include <sql-parser/gda-sql-parser.h>
#include <gobject/gvaluecollector.h>
#include <glib/gi18n.h>

#include "config.h"
#include "ag-app.h"
#include "ag-db.h"

#define SCHEMA_VERSION 1

static AgDb *singleton = NULL;

typedef struct _AgDbPrivate {
    gchar         *dsn;
    GdaConnection *conn;
} AgDbPrivate;

G_DEFINE_QUARK(ag_db_error_quark, ag_db_error);

G_DEFINE_TYPE_WITH_PRIVATE(AgDb, ag_db, G_TYPE_OBJECT);
G_DEFINE_BOXED_TYPE(
        AgDbChartSave,
        ag_db_chart_save,
        (GBoxedCopyFunc)ag_db_chart_save_ref,
        (GBoxedFreeFunc)ag_db_chart_save_unref
    );

enum {
    COLUMN_CHART_ID,
    COLUMN_CHART_NAME,
    COLUMN_CHART_COUNTRY,
    COLUMN_CHART_CITY,
    COLUMN_CHART_LONGITUDE,
    COLUMN_CHART_LATITUDE,
    COLUMN_CHART_ALTITUDE,
    COLUMN_CHART_YEAR,
    COLUMN_CHART_MONTH,
    COLUMN_CHART_DAY,
    COLUMN_CHART_HOUR,
    COLUMN_CHART_MINUTE,
    COLUMN_CHART_SECOND,
    COLUMN_CHART_TIMEZONE,
    COLUMN_CHART_HOUSE_SYSTEM,
    COLUMN_CHART_NOTE,

    /* Leave this as the last element */
    COLUMN_CHART_COUNT
};

typedef struct {
    int id;
    gchar *name;
} TableColumnDef;

static TableColumnDef chart_table_column[] = {
    { COLUMN_CHART_ID,           "id" },
    { COLUMN_CHART_NAME,         "name" },
    { COLUMN_CHART_COUNTRY,      "country_name" },
    { COLUMN_CHART_CITY,         "city_name" },
    { COLUMN_CHART_LONGITUDE,    "longitude" },
    { COLUMN_CHART_LATITUDE,     "latitude" },
    { COLUMN_CHART_ALTITUDE,     "altitude" },
    { COLUMN_CHART_YEAR,         "year" },
    { COLUMN_CHART_MONTH,        "month" },
    { COLUMN_CHART_DAY,          "day" },
    { COLUMN_CHART_HOUR,         "hour" },
    { COLUMN_CHART_MINUTE,       "minute" },
    { COLUMN_CHART_SECOND,       "second" },
    { COLUMN_CHART_TIMEZONE,     "timezone" },
    { COLUMN_CHART_HOUSE_SYSTEM, "house_system" },
    { COLUMN_CHART_NOTE,         "note" },
};

/**
 * ag_db_non_select:
 * @db: the AgDb to operate on
 * @sql: the SQL query to execute
 *
 * Executes a non-SELECT query on @db. No result is returned right now (TODO)
 */
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

/**
 * ag_db_table_exists:
 * @db: the #AgDb object to operate on
 * @table: the nawe of the table to check for
 *
 * Checks if the specified table exists. It is done by querying the
 * sqlite_master system table.
 *
 * Returns: TRUE if the table exists, FALSE otherwise
 */
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
    gchar        *error     = NULL;
    GError       *local_err = NULL;
    AgDbPrivate  *priv      = ag_db_get_instance_private(db);

    parser = g_object_get_data(G_OBJECT(priv->conn), "parser");

    if ((sth = gda_sql_parser_parse_string(
                parser,
                sql,
                &remain,
                &local_err
            )) == NULL) {
        g_error(
                "SQL error: %s",
                (local_err && local_err->message)
                    ? local_err->message
                    : "no reason"
            );
    }

    if (!gda_statement_get_parameters(sth, &params, &local_err)) {
        g_error(
                "SQL error: %s",
                (local_err && local_err->message)
                    ? local_err->message
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

        va_end(ap);
    }

    ret = gda_connection_statement_execute_select(priv->conn, sth, params, err);
    g_object_unref(sth);

    return ret;
}

/**
 * ag_db_check_version_table:
 * @db: the #AgDb object to operate on
 *
 * Checks if the version table exists, and creates it if necessary. It doesn't
 * check if the structure is valid!
 */
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

/**
 * ag_db_check_chart_table:
 * @db: the #AgDb object to operate on
 *
 * Checks if the chart table exists, and creates it if necessary. It doesn't
 * check if the structure is valid!
 */
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

/**
 * ag_db_verify:
 * @db: the #AgDb object to operate on
 *
 * Checks if the database file is sane.
 *
 * Returns: the status of the database (TODO: make this an enum!)
 */
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
    GFile        *ag_data_dir   = ag_get_user_data_dir();
    AgDbPrivate  *priv          = ag_db_get_instance_private(db);
    gchar        *path          = g_file_get_path(ag_data_dir);
    GError       *err           = NULL;

    gda_init();

    priv->dsn = g_strdup_printf("SQLite://DB_DIR=%s;DB_NAME=charts", path);

    g_free(path);
    g_clear_object(&ag_data_dir);

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

/**
 * ag_db_save_data_free:
 * @save_data: the #AgDbChartSave struct to free
 *
 * Frees @save_data and all its fields
 */
static void
ag_db_chart_save_free(AgDbChartSave *save_data)
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

AgDbChartSave *
ag_db_chart_save_ref(AgDbChartSave *save_data)
{
    if (save_data == NULL) {
        return NULL;
    }

    save_data->refcount++;

    return save_data;
}

void
ag_db_chart_save_unref(AgDbChartSave *save_data)
{
    if (save_data == NULL) {
        return;
    }

    if (--save_data->refcount == 0) {
        ag_db_chart_save_free(save_data);
    }
}

/**
 * ag_db_chart_save:
 * @db: the #AgDb object to operate on
 * @save_data: the data to save.
 * @err: a #GError for storing errors
 *
 * Saves @save_data to the database. If its db_id field is -1, a new record is
 * created. In this case, @save_data is updated and db_id is set to the actual
 * data record ID. Otherwise the row with the given ID will be updated.
 *
 * Returns: TRUE if the save succeeds, FALSE otherwise
 */
gboolean
ag_db_chart_save(AgDb *db, AgDbChartSave *save_data,  GError **err)
{
    GError      *local_err   = NULL;
    gboolean    save_success = TRUE;
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

    if (save_data == NULL) {
        g_error("Trying to save a NULL chart!");
    }

    if (!save_data->populated) {
        g_error("Only populated chart data can be saved!");
    }

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

    /* It is possible to get 0 here, which is as non-existant as -1 */
    if (save_data->db_id < 0) {
        if (!gda_connection_insert_row_into_table(
                    priv->conn,
                    "chart",
                    &local_err,
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

            g_set_error(
                    err,
                    AG_DB_ERROR,
                    AG_DB_ERROR_DATABASE_ERROR,
                    "%s",
                    (local_err && local_err->message)
                        ? local_err->message
                        : _("Reason unknown")
                );

            save_success = FALSE;
        } else {
            // Get inserted row's id
            GdaDataModel *result = ag_db_select(
                    db,
                    &local_err,
                    "SELECT last_insert_rowid()"
                );

            if (result == NULL) {
                if (err) {
                    *err = g_error_copy(local_err);
                }

                // TODO: a more reasonable return value should be given here
                save_success = FALSE;
            } else {
                const GValue *value = gda_data_model_get_value_at(
                        result,
                        0, 0,
                        NULL
                    );

                save_data->db_id = g_value_get_int(value);
            }
        }
    } else {
        g_value_init(&db_id, G_TYPE_INT);
        g_value_set_int(&db_id, save_data->db_id);

        if (!gda_connection_update_row_in_table(
                    priv->conn,
                    "chart",
                    "id",
                    &db_id,
                    &local_err,
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

            g_set_error(
                    err,
                    AG_DB_ERROR,
                    AG_DB_ERROR_DATABASE_ERROR,
                    "%s",
                    (local_err && local_err->message)
                        ? local_err->message
                        : _("Reason unknown")
                );

            save_success = FALSE;
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

    return save_success;
}

AgDbChartSave *
ag_db_chart_save_new(gboolean populated)
{
    AgDbChartSave *save_data;

    save_data = g_new0(AgDbChartSave, 1);
    save_data->refcount = 1;
    save_data->populated = populated;

    return save_data;
}

/**
 * ag_db_chart_get_list:
 * @db: the #AgDb object to operate on
 * @err: a #GError
 *
 * Creates a list of all charts in the database, ordered by name. As the return
 * value may be NULL even if there are no charts or if there was an error, you
 * may want to check @err if the return value is NULL.
 *
 * Please be aware that the #AgDbChartSave objects of the returned value are not
 * fully realised chart records. To get one, you need to call
 * ag_db_chart_get_data_by_id()
 *
 * Returns: (element-type AgDbChartSave) (transfer full): the list of all
 *          charts, or NULL if there are none, or if there is an error.
 */
GList *
ag_db_chart_get_list(AgDb *db, GError **err)
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
        const GValue  *value;
        AgDbChartSave *save_data = ag_db_chart_save_new(FALSE);

        value = gda_data_model_iter_get_value_at(iter, 0);
        save_data->db_id = g_value_get_int(value);

        value = gda_data_model_iter_get_value_at(iter, 1);
        save_data->name = g_strdup(g_value_get_string(value));

        ret = g_list_prepend(ret, save_data);
    }

    return g_list_reverse(ret);
}

/**
 * ag_db_chart_get_data_by_id:
 * @db: the #AgDb object to operate on
 * @row_id: the ID field of the requested chart
 * @err: a #GError
 *
 * Fetches the specified row from the chart table.
 *
 * Returns: (transfer full): A fully filled #AgDbChartSave record of the chart
 */
AgDbChartSave *
ag_db_chart_get_data_by_id(AgDb *db, guint row_id, GError **err)
{
    AgDbChartSave     *save_data;
    const GValue      *value;
    gchar             *query,
                      *columns;
    guint             i;
    GdaDataModel      *result;
    GError            *local_err = NULL;

    columns = NULL;

    for (i = 1; i < COLUMN_CHART_COUNT; i++) {
        gchar *tmp;

        if (i == 1) {
            columns = g_strjoin(
                    ", ",
                    chart_table_column[0].name,
                    chart_table_column[1].name,
                    NULL
                );
        } else {
            tmp = g_strjoin(", ", columns, chart_table_column[i].name, NULL);
            g_free(columns);
            columns = tmp;
        }
    }

    query = g_strdup_printf(
            "SELECT %s FROM chart WHERE id = ##id::gint",
            columns
        );
    g_free(columns);

    result = ag_db_select(db, &local_err, query, "id", row_id, NULL);
    g_free(query);

    if (local_err && (local_err->message)) {
        return NULL;
    }

    if (gda_data_model_get_n_rows(result) < 1) {
        g_set_error(
                err,
                AG_DB_ERROR, AG_DB_ERROR_NO_CHART,
                "Chart does not exist"
            );

        return NULL;
    }

    save_data = ag_db_chart_save_new(TRUE);

    /* id */
    value = gda_data_model_get_value_at(
            result,
            COLUMN_CHART_ID,
            0,
            NULL
        );
    save_data->db_id = g_value_get_int(value);

    /* name */
    value = gda_data_model_get_value_at(
            result,
            COLUMN_CHART_NAME,
            0,
            NULL
        );
    save_data->name = g_strdup(g_value_get_string(value));

    /* country */
    value = gda_data_model_get_value_at(result, COLUMN_CHART_COUNTRY, 0, NULL);

    if (GDA_VALUE_HOLDS_NULL(value)) {
        save_data->country = NULL;
    } else {
        save_data->country = g_strdup(g_value_get_string(value));
    }

    value = gda_data_model_get_value_at(result, COLUMN_CHART_CITY, 0, NULL);

    if (GDA_VALUE_HOLDS_NULL(value)) {
        save_data->city = NULL;
    } else {
        save_data->city = g_strdup(g_value_get_string(value));
    }

    value = gda_data_model_get_value_at(
            result,
            COLUMN_CHART_LONGITUDE,
            0,
            NULL
        );
    save_data->longitude = g_value_get_double(value);

    value = gda_data_model_get_value_at(
            result,
            COLUMN_CHART_LATITUDE,
            0,
            NULL
        );
    save_data->latitude = g_value_get_double(value);

    value = gda_data_model_get_value_at(result, COLUMN_CHART_ALTITUDE, 0, NULL);

    if (GDA_VALUE_HOLDS_NULL(value)) {
        save_data->altitude = DEFAULT_ALTITUDE;
    } else {
        save_data->altitude = g_value_get_double(value);
    }

    value = gda_data_model_get_value_at(
            result,
            COLUMN_CHART_YEAR,
            0,
            NULL
        );
    save_data->year = g_value_get_int(value);

    value = gda_data_model_get_value_at(
            result,
            COLUMN_CHART_MONTH,
            0,
            NULL
        );
    save_data->month = g_value_get_uint(value);

    value = gda_data_model_get_value_at(result, COLUMN_CHART_DAY, 0, NULL);
    save_data->day = g_value_get_uint(value);

    value = gda_data_model_get_value_at(result, COLUMN_CHART_HOUR, 0, NULL);
    save_data->hour = g_value_get_uint(value);

    value = gda_data_model_get_value_at(
            result,
            COLUMN_CHART_MINUTE,
            0,
            NULL
        );
    save_data->minute = g_value_get_uint(value);

    value = gda_data_model_get_value_at(
            result,
            COLUMN_CHART_SECOND,
            0,
            NULL
        );
    save_data->second = g_value_get_uint(value);

    value = gda_data_model_get_value_at(
            result,
            COLUMN_CHART_TIMEZONE,
            0,
            NULL
        );
    save_data->timezone = g_value_get_double(value);

    value = gda_data_model_get_value_at(
            result,
            COLUMN_CHART_HOUSE_SYSTEM,
            0,
            NULL
        );
    save_data->house_system = g_strdup(g_value_get_string(value));

    value = gda_data_model_get_value_at(result, COLUMN_CHART_NOTE, 0, NULL);

    if (GDA_VALUE_HOLDS_NULL(value)) {
        save_data->note = NULL;
    } else {
        save_data->note = g_strdup(g_value_get_string(value));
    }

    g_object_unref(result);

    return save_data;
}

/**
 * string_collate:
 * @str1: the first string
 * @str2: the second string
 *
 * A wrapper function around g_utf8_collate() that can handle NULL values. NULL
 * and empty strings ("") are considered equal.
 *
 * Returns: -1 if str1 is ordered before str2, 1 if str2 comes first, or 0 if
 * they are identical
 */
static gint
string_collate(const gchar *str1, const gchar *str2)
{
    if (
                ((str1 == NULL) && str2 && (*str2 == '\0'))
                || (str1 && (*str1 == '\0') && (str2 == NULL))
            ) {
        return 0;
    }

    if (((str1 == NULL) || (str2 == NULL)) && (str1 != str2)) {
        return (str1 == NULL) ? -1 : 1;
    }

    if (str1 == str2) {
        return 0;
    }

    return g_utf8_collate(str1, str2);
}

/**
 * ag_db_chart_save_identical:
 * @a: the first #AgDbChartSave structure
 * @b: the second #AgDbChartSave structure
 *
 * Compares two #AgDbChartSave structures and their contents.
 *
 * Returns: TRUE if the two structs hold equal values (strings are also compared
 *          with string_collate()), FALSE otherwise
 */
gboolean
ag_db_chart_save_identical(const AgDbChartSave *a,
                           const AgDbChartSave *b,
                           gboolean            chart_only)
{
    if (a == b) {
        g_debug("identical: Equal");

        return TRUE;
    }

    if ((a == NULL) || (b == NULL)) {
        g_debug("identical: One is NULL");

        return FALSE;
    }

    if (!chart_only && string_collate(a->name, b->name) != 0) {
        g_debug("identical: Names differ");

        return FALSE;
    }

    if (!chart_only && string_collate(a->country, b->country) != 0) {
        g_debug("identical: Countries differ");

        return FALSE;
    }

    if (!chart_only && string_collate(a->city, b->city) != 0) {
        g_debug("identical: Cities differ");

        return FALSE;
    }

    if (a->longitude != b->longitude) {
        g_debug("identical: Longitudes differ");

        return FALSE;
    }

    if (a->latitude != b->latitude) {
        g_debug("identical: Latitudes differ");

        return FALSE;
    }

    if (a->altitude != b->altitude) {
        g_debug("identical: Altitudes differ");

        return FALSE;
    }

    if (a->year != b->year) {
        g_debug("identical: Years differ");

        return FALSE;
    }

    if (a->month != b->month) {
        g_debug("identical: Months differ");

        return FALSE;
    }

    if (a->day != b->day) {
        g_debug("identical: Days differ");

        return FALSE;
    }

    if (a->hour != b->hour) {
        g_debug("identical: Hours differ");

        return FALSE;
    }

    if (a->minute != b->minute) {
        g_debug("identical: Minutes differ");

        return FALSE;
    }

    if (a->second != b->second) {
        g_debug("identical: Seconds differ");

        return FALSE;
    }

    if (a->timezone != b->timezone) {
        g_debug("identical: Timezones differ");

        return FALSE;
    }

    if (string_collate(a->house_system, b->house_system) != 0) {
        g_debug("identical: House systems differ");

        return FALSE;
    }

    if (!chart_only && string_collate(a->note, b->note) != 0) {
        g_debug("identical: Notes differ");

        return FALSE;
    }

    return TRUE;
}

gboolean
ag_db_chart_delete(AgDb *db, gint row_id, GError **err)
{
    AgDbPrivate *priv = ag_db_get_instance_private(db);
    GValue      id    = G_VALUE_INIT;

    g_value_init(&id, G_TYPE_INT);
    g_value_set_int(&id, row_id);

    return gda_connection_delete_row_from_table(
            priv->conn, "chart",
            "id", &id,
            err
        );
}
