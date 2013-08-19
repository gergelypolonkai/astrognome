#include <stdlib.h>
#include <math.h>
#include <cairo.h>
#include <glib.h>
#include <gtk/gtk.h>

#include "calculate.h"

#include "../swe/src/swephexp.h"

#define IMAGEDIR "/home/polesz/Projektek/c/gradix/images"
#define EPHEDIR "/home/polesz/Projektek/c/gradix/swe/data"
#define UI_FILE "/home/polesz/Projektek/c/gradix/src/gradix.ui"

typedef struct {
    int signId;
    signElement_t element;
    signType_t type;
} signData_t;

const char *signTypeName[] = {
    NULL,
    "Cardinal",
    "Fix",
    "Mutable"
};

const char *signElementName[] = {
    NULL,
    "Fire",
    "Earth",
    "Air",
    "Water"
};

const char *moonStateName[] = {
    "New Moon",
    "Waxing Crescent Moon",
    "Waxing Half Moon",
    "Waxing Gibbous Moon",
    "Full Moon",
    "Waning Gibbous Moon",
    "Waning Half Moon",
    "Waning Crescent Moon",
    "Dark Moon"
};

const char *signName[] = {
    NULL,
    "Aries",
    "Taurus",
    "Gemini",
    "Cancer",
    "Leo",
    "Virgo",
    "Libra",
    "Scorpio",
    "Saggitarius",
    "Capricorn",
    "Aquarius",
    "Pisces"
};

#define ADD_SIGN(ht, v, s, e, t) v = g_new0(signData_t, 1); \
                                 (v)->signId = (s); \
                                 (v)->element = (e); \
                                 (v)->type = (t); \
                                 g_hash_table_replace((ht), GINT_TO_POINTER(s), (v));

#define INCREASE_POINTS(dts, dte, dtt, vsd, s, p, val) (vsd) = g_hash_table_lookup((dts), GINT_TO_POINTER(s)); \
                                                       g_assert((vsd) != NULL); \
                                                       \
                                                       if (((p) = g_hash_table_lookup((dte), GINT_TO_POINTER((vsd)->element))) == NULL) { \
                                                           (p) = g_new0(guint, 1); \
                                                       } \
                                                       \
                                                       *(p) += (val); \
                                                       g_hash_table_replace((dte), GINT_TO_POINTER((vsd)->element), (p)); \
                                                       \
                                                       if (((p) = g_hash_table_lookup((dtt), GINT_TO_POINTER((vsd)->type))) == NULL) { \
                                                           (p) = g_new0(guint, 1); \
                                                       } \
                                                       \
                                                       *(p) += (val); \
                                                       g_hash_table_replace((dtt), GINT_TO_POINTER((vsd)->type), (p));

/*
GtkBuilder *builder;

void
action_about_activate_cb(GtkAction *action, gpointer user_data)
{
	GtkDialog *about_dialog;

	about_dialog = GTK_DIALOG(gtk_builder_get_object(builder, "dialog_about"));

	gtk_dialog_run(about_dialog);

	gtk_widget_hide(GTK_WIDGET(about_dialog));
}

void
action_new_activate_cb(GtkAction *action, gpointer user_data)
{
	GtkDialog *new_dialog;

	new_dialog = GTK_DIALOG(gtk_builder_get_object(builder, "dialog_new"));

	g_print("%d\n", gtk_dialog_run(new_dialog));

	gtk_widget_hide(GTK_WIDGET(new_dialog));
}

int
main(int argc, char *argv[])
{
	GtkWidget *mainWindow;
	GError *err = NULL;

	gtk_init(&argc, &argv);

	builder = gtk_builder_new();

	if (gtk_builder_add_from_file(builder, UI_FILE, &err) == 0) {
		g_print("Juj!\n");
	}

	mainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
	gtk_builder_connect_signals (builder, NULL);

	gtk_widget_show(mainWindow);
	gtk_main();

	g_object_unref(G_OBJECT(builder));

	return 0;
}
*/

/*
RsvgHandle *svgHandle[SE_CHIRON + SIGN_PISCES + 1];

gboolean
init_graphics(void)
{
    GError *err = NULL;
    char *svgFile[SE_CHIRON + SIGN_PISCES + 1];
    int i;

    memset(&svgFile, 0, sizeof(char *) * SE_CHIRON + SIGN_PISCES + 1);

    svgFile[SE_SUN]       = IMAGEDIR "/planet_sun.svg";
    svgFile[SE_MERCURY]   = IMAGEDIR "/planet_mercury.svg";
    svgFile[SE_VENUS]     = IMAGEDIR "/planet_venus.svg";
    svgFile[SE_MOON]      = IMAGEDIR "/planet_moon.svg";
    svgFile[SE_MARS]      = IMAGEDIR "/planet_mars.svg";
    svgFile[SE_JUPITER]   = IMAGEDIR "/planet_jupiter.svg";
    svgFile[SE_SATURN]    = IMAGEDIR "/planet_saturn.svg";
    svgFile[SE_NEPTUNE]   = IMAGEDIR "/planet_neptune.svg";
    svgFile[SE_URANUS]    = IMAGEDIR "/planet_uranus.svg";
    svgFile[SE_PLUTO]     = IMAGEDIR "/planet_pluto.svg";
    // mean node is used for descending moon node
    svgFile[SE_MEAN_NODE] = IMAGEDIR "/planet_desc_node.svg";
    // true node is used for ascending moon node
    svgFile[SE_TRUE_NODE] = IMAGEDIR "/planet_asc_node.svg";
    svgFile[SE_CHIRON]    = IMAGEDIR "/planet_chiron.svg";

    svgFile[SE_CHIRON + SIGN_ARIES]       = IMAGEDIR "/sign_aries.svg";
    svgFile[SE_CHIRON + SIGN_TAURUS]      = IMAGEDIR "/sign_taurus.svg";
    svgFile[SE_CHIRON + SIGN_GEMINI]      = IMAGEDIR "/sign_gemini.svg";
    svgFile[SE_CHIRON + SIGN_CANCER]      = IMAGEDIR "/sign_cancer.svg";
    svgFile[SE_CHIRON + SIGN_LEO]         = IMAGEDIR "/sign_leo.svg";
    svgFile[SE_CHIRON + SIGN_VIRGO]       = IMAGEDIR "/sign_virgo.svg";
    svgFile[SE_CHIRON + SIGN_LIBRA]       = IMAGEDIR "/sign_libra.svg";
    svgFile[SE_CHIRON + SIGN_SCORPIO]     = IMAGEDIR "/sign_scorpio.svg";
    svgFile[SE_CHIRON + SIGN_SAGGITARIUS] = IMAGEDIR "/sign_saggitarius.svg";
    svgFile[SE_CHIRON + SIGN_CAPRICORN]   = IMAGEDIR "/sign_capricorn.svg";
    svgFile[SE_CHIRON + SIGN_AQUARIUS]    = IMAGEDIR "/sign_aquarius.svg";
    svgFile[SE_CHIRON + SIGN_PISCES]      = IMAGEDIR "/sign_pisces.svg";

    for (i = SE_SUN; i <= SE_CHIRON + SIGN_PISCES; i++) {
        if (svgFile[i] != NULL) {
            g_clear_error(&err);
            //if ((svgHandle[i] = rsvg_handle_new_from_file(svgFile[i], &err)) == NULL) {
            //    printf("Unable to load %s: %s\n", svgFile[i], err->message);
            //}
        }
    }

    return TRUE;
}
*/

int
main(int argc, char *argv[])
{
    int year = 1981,
        month = 3,
        day = 11,
        hour = 23,
        min = 39,
        sec = 45,
        sign,
        p;
    guint *point;
    double timezone = 1.0,
           lon = 19.081599,
           lat = 47.462485,
           alt = 200,
           te,
           cusps[13],
           ascmc[10];
    planetInfo_t *planetInfo;
    moonPhase *phase;
    GHashTable *signDataTable,
               *elementPointsTable,
               *typePointsTable;
    signData_t *signData;

#if 1
    year = 1983;
    month = 3;
    day = 7;
    hour = 11;
    min = 54;
#endif

    signDataTable = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_free);
    elementPointsTable = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
    typePointsTable = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);

    // Initialize sign data table

    ADD_SIGN(signDataTable, signData, SIGN_ARIES,       ELEMENT_FIRE,  TYPE_CARDINAL);
    ADD_SIGN(signDataTable, signData, SIGN_TAURUS,      ELEMENT_EARTH, TYPE_FIX);
    ADD_SIGN(signDataTable, signData, SIGN_GEMINI,      ELEMENT_AIR,   TYPE_MUTABLE);
    ADD_SIGN(signDataTable, signData, SIGN_CANCER,      ELEMENT_WATER, TYPE_CARDINAL);
    ADD_SIGN(signDataTable, signData, SIGN_LEO,         ELEMENT_FIRE,  TYPE_FIX);
    ADD_SIGN(signDataTable, signData, SIGN_VIRGO,       ELEMENT_EARTH, TYPE_MUTABLE);
    ADD_SIGN(signDataTable, signData, SIGN_LIBRA,       ELEMENT_AIR,   TYPE_CARDINAL);
    ADD_SIGN(signDataTable, signData, SIGN_SCORPIO,     ELEMENT_WATER, TYPE_FIX);
    ADD_SIGN(signDataTable, signData, SIGN_SAGGITARIUS, ELEMENT_FIRE,  TYPE_MUTABLE);
    ADD_SIGN(signDataTable, signData, SIGN_CAPRICORN,   ELEMENT_EARTH, TYPE_CARDINAL);
    ADD_SIGN(signDataTable, signData, SIGN_AQUARIUS,    ELEMENT_AIR,   TYPE_FIX);
    ADD_SIGN(signDataTable, signData, SIGN_PISCES,      ELEMENT_WATER, TYPE_MUTABLE);

    swe_set_ephe_path(EPHEDIR);

    if (set_location_and_time(lon, lat, alt, year, month, day, hour, min, sec, timezone, &te) == 0) {
        return 1;
    }

    printf("date: %02d.%02d.%d at %02d:%02d:%02d, at %f, %f\n", year, month, day, hour, min, sec, lon, lat);

    swe_houses(te, lat, lon, 'P', cusps, ascmc);

    for (p = 1; p < 13; p++) {
        printf("House %2d..: %2.0f (%f)\n", p, ceilf(cusps[p] / 30.0), cusps[p]);
    }

    sign = get_sign(ascmc[0]);
    printf("Asc.......: %s\n", signName[sign]);
    INCREASE_POINTS(signDataTable, elementPointsTable, typePointsTable, signData, sign, point, 2);

    sign = get_sign(ascmc[1]);
    printf("MC........: %s\n", signName[sign]);
    INCREASE_POINTS(signDataTable, elementPointsTable, typePointsTable, signData, sign, point, 1);

    planetInfo = get_planet_info(SE_SUN, te, cusps);
    printf("Sun.......: %s, House: %d (%f)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position);
    INCREASE_POINTS(signDataTable, elementPointsTable, typePointsTable, signData, planetInfo->sign, point, 2);
    g_free(planetInfo);

    planetInfo = get_planet_info(SE_MOON, te, cusps);
    phase = get_moon_phase(year, month, day, hour, min, sec);
    printf("Moon......: %s (%.2f%% visibility), %s, House: %d (%f)\n", moonStateName[phase->phase], phase->visiblePercentage, signName[planetInfo->sign], planetInfo->house, planetInfo->position);
    INCREASE_POINTS(signDataTable, elementPointsTable, typePointsTable, signData, planetInfo->sign, point, 2);
    g_free(phase);
    g_free(planetInfo);

    planetInfo = get_planet_info(SE_MERCURY, te, cusps);
    printf("Mercury...: %s, House: %d (%f)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position);
    INCREASE_POINTS(signDataTable, elementPointsTable, typePointsTable, signData, planetInfo->sign, point, 2);
    g_free (planetInfo);

    planetInfo = get_planet_info (SE_VENUS, te, cusps);
    printf("Venus.....: %s, House: %d (%f)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position);
    INCREASE_POINTS(signDataTable, elementPointsTable, typePointsTable, signData, planetInfo->sign, point, 1);
    g_free (planetInfo);

    planetInfo = get_planet_info(SE_MARS, te, cusps);
    printf("Mars......: %s, House: %d (%f)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position);
    INCREASE_POINTS(signDataTable, elementPointsTable, typePointsTable, signData, planetInfo->sign, point, 1);
    g_free (planetInfo);

    planetInfo = get_planet_info(SE_JUPITER, te, cusps);
    printf("Jupiter...: %s, House: %d (%f)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position);
    INCREASE_POINTS(signDataTable, elementPointsTable, typePointsTable, signData, planetInfo->sign, point, 1);
    g_free (planetInfo);

    planetInfo = get_planet_info(SE_SATURN, te, cusps);
    printf("Saturn....: %s, House: %d (%f)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position);
    INCREASE_POINTS(signDataTable, elementPointsTable, typePointsTable, signData, planetInfo->sign, point, 1);
    g_free (planetInfo);

    planetInfo = get_planet_info(SE_URANUS, te, cusps);
    printf("Uranus....: %s, House: %d (%f)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position);
    INCREASE_POINTS(signDataTable, elementPointsTable, typePointsTable, signData, planetInfo->sign, point, 1);
    g_free (planetInfo);

    planetInfo = get_planet_info(SE_NEPTUNE, te, cusps);
    printf("Neptune...: %s, House: %d (%f)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position);
    INCREASE_POINTS(signDataTable, elementPointsTable, typePointsTable, signData, planetInfo->sign, point, 1);
    g_free (planetInfo);

    planetInfo = get_planet_info(SE_PLUTO, te, cusps);
    printf("Pluto.....: %s, House: %d (%f)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position);
    INCREASE_POINTS(signDataTable, elementPointsTable, typePointsTable, signData, planetInfo->sign, point, 1);
    g_free (planetInfo);

    planetInfo = get_planet_info(SE_CHIRON, te, cusps);
    printf("Chiron....: %s, House: %d (%f)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position);
    g_free (planetInfo);

    planetInfo = get_planet_info(SE_MEAN_NODE, te, cusps);
    printf("North Node: %s, House: %d (%f)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position);
    INCREASE_POINTS(signDataTable, elementPointsTable, typePointsTable, signData, planetInfo->sign, point, 1);
    g_free (planetInfo);

    planetInfo = get_planet_info(SE_MEAN_APOG, te, cusps);
    printf("Dark Moon.: %s, House: %d (%f)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position);
    g_free (planetInfo);

    planetInfo = get_planet_info(SE_CERES, te, cusps);
    printf("Ceres.....: %s, House: %d (%f)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position);
    g_free(planetInfo);

    planetInfo = get_planet_info(SE_PALLAS, te, cusps);
    printf("Pallas....: %s, House: %d (%f)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position);
    g_free(planetInfo);

    planetInfo = get_planet_info(SE_JUNO, te, cusps);
    printf("Juno......: %s, House: %d (%f)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position);
    g_free(planetInfo);

    planetInfo = get_planet_info(SE_VESTA, te, cusps);
    printf("Vesta.....: %s, House: %d (%f)\n", signName[planetInfo->sign], planetInfo->house, planetInfo->position);
    g_free(planetInfo);

    point = g_hash_table_lookup(elementPointsTable, GINT_TO_POINTER(ELEMENT_FIRE));
    printf("\nFire.: %d\n", (point == NULL) ? 0 : *point);
    point = g_hash_table_lookup(elementPointsTable, GINT_TO_POINTER(ELEMENT_EARTH));
    printf("Earth: %d\n", (point == NULL) ? 0 : *point);
    point = g_hash_table_lookup(elementPointsTable, GINT_TO_POINTER(ELEMENT_AIR));
    printf("Air..: %d\n", (point == NULL) ? 0 : *point);
    point = g_hash_table_lookup(elementPointsTable, GINT_TO_POINTER(ELEMENT_WATER));
    printf("Water: %d\n", (point == NULL) ? 0 : *point);

    point = g_hash_table_lookup(typePointsTable, GINT_TO_POINTER(TYPE_CARDINAL));
    printf("\nCardinal: %d\n", (point == NULL) ? 0 : *point);
    point = g_hash_table_lookup(typePointsTable, GINT_TO_POINTER(TYPE_FIX));
    printf("Fix.....: %d\n", (point == NULL) ? 0 : *point);
    point = g_hash_table_lookup(typePointsTable, GINT_TO_POINTER(TYPE_MUTABLE));
    printf("Mutable.: %d\n", (point == NULL) ? 0 : *point);

    g_hash_table_unref(typePointsTable);
    g_hash_table_unref(elementPointsTable);
    g_hash_table_unref(signDataTable);

    return OK;
}

/*
static gboolean
draw_clock (ClutterCanvas *canvas, cairo_t *cr, int width, int height)
{
    GDateTime *now;
    float hours, minutes, seconds;
    ClutterColor color;

    int smaller = (width < height) ? width : height;

    // get the current time and compute the angles
    now = g_date_time_new_now_local();
    seconds = g_date_time_get_second(now) * G_PI / 30;
    minutes = g_date_time_get_minute(now) * G_PI / 30;
    hours = g_date_time_get_hour(now) * G_PI / 6;

    cairo_save(cr);

    // clear the contents of the canvas, to avoid painting over the previous frame
    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);

    cairo_restore(cr);

    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

    // scale the modelview to the size of the surface
    cairo_scale(cr, smaller, smaller);

    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_width(cr, 0.01);

    // the black rail that holds the seconds indicator
    clutter_cairo_set_source_color(cr, CLUTTER_COLOR_Black);
    cairo_translate(cr, 0.5, 0.5);
    cairo_arc(cr, 0, 0, 0.4, 0, G_PI * 2);
    cairo_stroke(cr);

    // the seconds indicator
    color = *CLUTTER_COLOR_White;
    color.alpha = 128;
    clutter_cairo_set_source_color(cr, &color);
    cairo_move_to(cr, 0, 0);
    cairo_arc(cr, sinf(seconds) * 0.4, - cosf(seconds) * 0.4, 0.02, 0, G_PI * 2);
    cairo_fill(cr);

    // the minutes hand
    color = *CLUTTER_COLOR_DarkChameleon;
    color.alpha = 196;
    clutter_cairo_set_source_color(cr, &color);
    cairo_move_to(cr, 0, 0);
    cairo_line_to(cr, sinf(minutes) * 0.4, - cosf(minutes) * 0.4);
    cairo_stroke(cr);

    // the hours hand
    cairo_move_to(cr, 0, 0);
    cairo_line_to(cr, sinf(hours) * 0.2, - cosf(hours) * 0.2);
    cairo_stroke(cr);

    g_date_time_unref(now);

    // we're done drawing
    return TRUE;
}

static guint idle_resize_id;

static gboolean
idle_resize(gpointer data)
{
    ClutterActor *actor = data;
    float width, height;

    // match the canvas size to the actor's
    clutter_actor_get_size(actor, &width, &height);
    clutter_canvas_set_size(CLUTTER_CANVAS(clutter_actor_get_content(actor)), ceilf(width), ceilf(height));

    // unset the guard
    idle_resize_id = 0;

    // remove the timeout
    return G_SOURCE_REMOVE;
}

static void
on_actor_resize(ClutterActor *actor, const ClutterActorBox *allocation, ClutterAllocationFlags flags, gpointer user_data)
{
    // throttle multiple actor allocations to one canvas resize; we use a guard variable to avoid queueing multiple resize operations
    if (idle_resize_id == 0) {
        idle_resize_id = clutter_threads_add_timeout(1000, idle_resize, actor);
    }
}

int
gradix_clutter_main(int argc, char *argv[])
{
    ClutterActor *stage,
             *sign_aries,
             *sign_taurus,
             *sign_gemini,
             *sign_cancer,
             *sign_leo,
             *sign_virgo,
             *sign_libra,
             *sign_scorpio,
             *sign_saggitarius,
             *sign_capricorn,
             *sign_aquarius,
             *sign_pisces,
             *actor;
    ClutterContent *canvas;
    GError *err = NULL;

    init_graphics();

    // initialize Clutter
    if (clutter_init(&argc, &argv) != CLUTTER_INIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    // create a resizable stage
    stage = clutter_stage_new();
    clutter_stage_set_title(CLUTTER_STAGE (stage), "GRadix");
    clutter_stage_set_user_resizable(CLUTTER_STAGE (stage), TRUE);
    clutter_actor_set_background_color(stage, CLUTTER_COLOR_LightSkyBlue);
    clutter_actor_set_size(stage, 300, 300);
    clutter_actor_show(stage);

    g_clear_error(&err);
    if ((sign_aries = clutter_texture_new_from_file(IMAGEDIR "/sign_aries.svg", &err)) == NULL) {
        printf("%s\n", err->message);
    } else {
        clutter_actor_add_child(stage, sign_aries);
    }

    g_clear_error(&err);
    if ((sign_taurus = clutter_texture_new_from_file(IMAGEDIR "/sign_taurus.svg", &err)) == NULL) {
        printf("%s\n", err->message);
    } else {
        clutter_actor_add_child(stage, sign_taurus);
    }

    g_clear_error(&err);
    if ((sign_gemini = clutter_texture_new_from_file(IMAGEDIR "/sign_gemini.svg", &err)) == NULL) {
        printf("%s\n", err->message);
    } else {
        clutter_actor_add_child(stage, sign_gemini);
    }

    g_clear_error(&err);
    if ((sign_cancer = clutter_texture_new_from_file(IMAGEDIR "/sign_cancer.svg", &err)) == NULL) {
        printf("%s\n", err->message);
    } else {
        clutter_actor_add_child(stage, sign_cancer);
    }

    g_clear_error(&err);
    if ((sign_leo = clutter_texture_new_from_file(IMAGEDIR "/sign_leo.svg", &err)) == NULL) {
        printf("%s\n", err->message);
    } else {
        clutter_actor_add_child(stage, sign_leo);
    }

    g_clear_error(&err);
    if ((sign_virgo = clutter_texture_new_from_file(IMAGEDIR "/sign_virgo.svg", &err)) == NULL) {
        printf("%s\n", err->message);
    } else {
        clutter_actor_add_child(stage, sign_virgo);
    }

    g_clear_error(&err);
    if ((sign_libra = clutter_texture_new_from_file(IMAGEDIR "/sign_libra.svg", &err)) == NULL) {
        printf("%s\n", err->message);
    } else {
        clutter_actor_add_child(stage, sign_libra);
    }

    g_clear_error(&err);
    if ((sign_scorpio = clutter_texture_new_from_file(IMAGEDIR "/sign_scorpio.svg", &err)) == NULL) {
        printf("%s\n", err->message);
    } else {
        clutter_actor_add_child(stage, sign_scorpio);
    }

    g_clear_error(&err);
    if ((sign_saggitarius = clutter_texture_new_from_file(IMAGEDIR "/sign_saggitarius.svg", &err)) == NULL) {
        printf("%s\n", err->message);
    } else {
        clutter_actor_add_child(stage, sign_saggitarius);
    }

    g_clear_error(&err);
    if ((sign_capricorn = clutter_texture_new_from_file(IMAGEDIR "/sign_capricorn.svg", &err)) == NULL) {
        printf("%s\n", err->message);
    } else {
        clutter_actor_add_child(stage, sign_capricorn);
    }

    g_clear_error(&err);
    if ((sign_aquarius = clutter_texture_new_from_file(IMAGEDIR "/sign_aquarius.svg", &err)) == NULL) {
        printf("%s\n", err->message);
    } else {
        clutter_actor_add_child(stage, sign_aquarius);
    }

    g_clear_error(&err);
    if ((sign_pisces = clutter_texture_new_from_file(IMAGEDIR "/sign_pisces.svg", &err)) == NULL) {
        printf("%s\n", err->message);
    } else {
        clutter_actor_add_child(stage, sign_pisces);
    }

    // our 2D canvas, courtesy of Cairo
    //canvas = clutter_canvas_new();
    //clutter_canvas_set_size(CLUTTER_CANVAS (canvas), 300, 300);

    //actor = clutter_actor_new();
    //clutter_actor_set_content(actor, canvas);
    //clutter_actor_set_content_scaling_filters(actor, CLUTTER_SCALING_FILTER_TRILINEAR, CLUTTER_SCALING_FILTER_LINEAR);
    //clutter_actor_add_child(stage, actor);

    // the actor now owns the canvas
    //g_object_unref(canvas);

    // bind the size of the actor to that of the stage
    //clutter_actor_add_constraint(actor, clutter_bind_constraint_new(stage, CLUTTER_BIND_SIZE, 0));

    // resize the canvas whenever the actor changes size
    //g_signal_connect(actor, "allocation-changed", G_CALLBACK(on_actor_resize), NULL);

    // quit on destroy
    g_signal_connect(stage, "destroy", G_CALLBACK(clutter_main_quit), NULL);

    // connect our drawing code
    //g_signal_connect(canvas, "draw", G_CALLBACK(draw_clock), NULL);

    // invalidate the canvas, so that we can draw before the main loop starts
    //clutter_content_invalidate(canvas);

    // set up a timer that invalidates the canvas every second
    //clutter_threads_add_timeout(1000, invalidate_clock, canvas);

    clutter_main();

    return EXIT_SUCCESS;
}
*/
