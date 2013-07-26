#include <stdlib.h>
#include <math.h>
#include <cairo.h>
#include <clutter/clutter.h>

#include "../swe/src/swephexp.h"

#define IMAGEDIR "/home/polesz/Projektek/c/gradix/images"
#define EPHEDIR "/home/polesz/Projektek/c/gradix/swe/data"

#define SYNODIC 29.53058867
#define MSPERDAY 86400000

typedef enum {
    SIGN_ARIES = 1,
    SIGN_TAURUS,
    SIGN_GEMINI,
    SIGN_CANCER,
    SIGN_LEO,
    SIGN_VIRGO,
    SIGN_LIBRA,
    SIGN_SCORPIO,
    SIGN_SAGGITARIUS,
    SIGN_CAPRICORN,
    SIGN_AQUARIUS,
    SIGN_PISCES
} zodiacSign;

typedef enum {
    MOON_STATE_NEW,
    MOON_STATE_WAXING_CRESCENT,
    MOON_STATE_WAXING_HALF,
    MOON_STATE_WAXING_GIBBOUS,
    MOON_STATE_FULL,
    MOON_STATE_WANING_GIBBOUS,
    MOON_STATE_WANING_HALF,
    MOON_STATE_WANING_CRESCENT,
    MOON_STATE_DARK
} moonState;

typedef struct {
    moonState phase;
    double visiblePercentage;
} moonPhase;

//RsvgHandle *svgHandle[SE_CHIRON + SIGN_PISCES + 1];

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

double
get_planet_position(int32 planet_no, double date)
{
	int32 iflgret,
	      iflag = SEFLG_SPEED | SEFLG_TOPOCTR;
	double x2[6];
	char serr[AS_MAXCH];  

	iflgret = swe_calc(date, planet_no, iflag, x2, serr);

	if (iflgret < 0) {
		printf("error: %s\n", serr);

		return -1;
	} else if (iflgret != iflag) {
		printf("warning: iflgret != iflag. %s\n", serr);
	}

	return x2[0];
}

int
set_location_and_time(double lon, double lat, double alt, int year, int month, int day, int hour, int min, double sec, double d_timezone, double *jd)
{
	int utc_year,
	    utc_month,
	    utc_day,
	    utc_hour,
	    utc_min;
	double utc_sec,
	       retval,
	       dret[2];
	char serr[AS_MAXCH];  

	swe_set_topo(lon, lat, alt);
	swe_utc_time_zone(year, month, day, hour, min, sec, d_timezone, &utc_year, &utc_month, &utc_day, &utc_hour, &utc_min, &utc_sec);
	if ((retval = swe_utc_to_jd(utc_year, utc_month, utc_day, utc_hour, utc_min, utc_sec, SE_GREG_CAL, dret, serr)) == ERR) {
		printf("error: %s\n", serr);

		return 0;
	}

	*jd = dret[0];

	return 1;
}

int
moonPhase *
get_moon_phase(gint year, gint month, gint day, gint hour, gint min, gint sec)
{
	GDateTime *baseDate,
              *gds;
	GTimeSpan diff;
	gdouble phasePercent,
            realPercent;
    moonState state;
    moonPhase *ret;

	baseDate = g_date_time_new_utc(2005, 5, 8, 8, 48, 0);
	// TODO: this should use the time zone used at the birth place
	gds = g_date_time_new_local(year, month, day, 0, 0, 0);
	diff = g_date_time_difference(gds, baseDate) / 1000;

	g_date_time_unref(gds);
	g_date_time_unref(baseDate);

	// The current phase of the moon, between 0 and 100 (both 0 and 100 are new moon, 50 is full moon)
	phasePercent = fmod((diff * 100) / (SYNODIC * MSPERDAY), 100);

	if (phasePercent < 0) {
		phasePercent += 100.0;
	}

    if ((phasePercent < 0) || (phasePercent > 100)) {
        fprintf(stderr, "Error during moon phase calculation!\n");

        return NULL;
    }

	// The real percentage is a number around the illumination percentage of the moon
	realPercent = (50.0 - fabs(phasePercent - 50.0)) * 2;

    // Uuuugly!
    if (phasePercent == 0) {
        state = MOON_STATE_NEW;
    } else if (phasePercent < 25) {
        state = MOON_STATE_WAXING_CRESCENT;
    } else if (phasePercent == 25) {
        state = MOON_STATE_WAXING_HALF;
    } else if (phasePercent < 50) {
        state = MOON_STATE_WAXING_GIBBOUS;
    } else if (phasePercent == 50) {
        state = MOON_STATE_FULL;
    } else if (phasePercent < 75) {
        state = MOON_STATE_WANING_GIBBOUS;
    } else if (phasePercent == 75) {
        state = MOON_STATE_WANING_HALF;
    } else if (phasePercent < 100) {
        state = MOON_STATE_WANING_CRESCENT;
    } else {
        state = MOON_STATE_DARK;
    }

    ret = g_new0(moonPhase, 1);
    ret->phase = state;
    ret->visiblePercentage = realPercent;

    return ret;
}

main(int argc, char *argv[])
{
#if !CLUTTER_CHECK_VERSION(1, 3, 6)
#error "You need Clutter >= 1.3.6 to compile this software"
#endif

	int year = 1983,
	    month = 3,
	    day = 7,
	    hour = 11,
	    min = 54,
	    sec = 45;
	double timezone = 1.0,
	       lon = 19.081599,
	       lat = 47.462485,
	       alt = 200,
	       te,
	       pos,
	       cusps[13],
	       ascmc[10];
	int p;

	swe_set_ephe_path(EPHEDIR);

	if (set_location_and_time(lon, lat, alt, year, month, day, hour, min, sec, timezone, &te) == 0) {
		return 1;
	}

	printf("date: %02d.%02d.%d at %02d:%02d:%02d, at %f, %f\n", year, month, day, hour, min, sec, lon, lat);

	swe_houses(te, lat, lon, 'P', cusps, ascmc);

	for (p = 1; p < 13; p++) {
		printf("House %2d..: %2.0f (%f)\n", p, ceilf(cusps[p] / 30.0), cusps[p]);
	}

	get_moon_phase(year, month, day, hour, min, sec);
	printf("Asc.......: %.0f\n", ceilf(ascmc[0] / 30.0));
	printf("MC........: %.0f\n", ceilf(ascmc[1] / 30.0));

	pos = get_planet_position(SE_SUN, te);
	printf("Sun.......: %2.0f (%f)\n", ceilf(pos / 30.0), pos);

	pos = get_planet_position(SE_MOON, te);
	printf("Moon......: %2.0f (%f)\n", ceilf(pos / 30.0), pos);

	pos = get_planet_position(SE_MERCURY, te);
	printf("Mercury...: %2.0f (%f)\n", ceilf(pos / 30.0), pos);

	pos = get_planet_position(SE_VENUS, te);
	printf("Venus.....: %2.0f (%f)\n", ceilf(pos / 30.0), pos);

	pos = get_planet_position(SE_MARS, te);
	printf("Mars......: %2.0f (%f)\n", ceilf(pos / 30.0), pos);

	pos = get_planet_position(SE_JUPITER, te);
	printf("Jupiter...: %2.0f (%f)\n", ceilf(pos / 30.0), pos);

	pos = get_planet_position(SE_SATURN, te);
	printf("Saturn....: %2.0f (%f)\n", ceilf(pos / 30.0), pos);

	pos = get_planet_position(SE_URANUS, te);
	printf("Uranus....: %2.0f (%f)\n", ceilf(pos / 30.0), pos);

	pos = get_planet_position(SE_NEPTUNE, te);
	printf("Neptune...: %2.0f (%f)\n", ceilf(pos / 30.0), pos);

	pos = get_planet_position(SE_PLUTO, te);
	printf("Pluto.....: %2.0f (%f)\n", ceilf(pos / 30.0), pos);

	pos = get_planet_position(SE_CHIRON, te);
	printf("Chiron....: %2.0f (%f)\n", ceilf(pos / 30.0), pos);

	pos = get_planet_position(SE_MEAN_NODE, te);
	printf("North Node: %2.0f (%f)\n", ceilf(pos / 30.0), pos);

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
