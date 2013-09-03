#include <glib.h>

#include "../swe-glib/src/swe-glib.h"

#define EPHEDIR "/home/polesz/Projektek/c/astrognome/swe/data"

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

void
print_house_cusp(gpointer data, gpointer user_data)
{
    GsweHouseData *house_data = data;

    g_print("House %2d..: %s (%f)\n", house_data->house, house_data->sign->name, house_data->cusp_position);
}

void
print_aspects(GsweAspectData *aspect_data, gpointer user_data)
{
    GswePlanet planet = GPOINTER_TO_INT(user_data);
    GswePlanetData *other_planet = (aspect_data->planet1->planet_id == planet) ? aspect_data->planet2 : aspect_data->planet1;

    if (aspect_data->aspect != GSWE_ASPECT_NONE) {
        g_print("        %s: %s (±%f%%)\n", other_planet->planet_info->name, aspect_data->aspect_info->name, aspect_data->difference);
    }
}

void
print_mirrorpoints(GsweMirrorData *mirror_data, gpointer user_data)
{
    GswePlanet planet = GPOINTER_TO_INT(user_data);
    GswePlanetData *other_planet = (mirror_data->planet1->planet_id == planet) ? mirror_data->planet2 : mirror_data->planet1;

    if (mirror_data->mirror != GSWE_MIRROR_NONE) {
        g_print("        %s: %s (±%f°)\n", other_planet->planet_info->name, mirror_data->mirror_info->name, mirror_data->difference);
    }
}

int
main(int argc, char *argv[])
{
    int year = 1981,
        month = 3,
        day = 11,
        hour = 23,
        min = 39,
        sec = 34;
    double lon = 20.766666,
           lat = 48.2,
           alt = 200;
    GsweMoonPhaseData *moon_phase;
    GsweTimestamp *timestamp;
    GsweMoment *moment;
    GswePlanetData *planet_data;
    GList *aspects,
          *mirrorpoints;

#if 1
    year = 1983;
    month = 3;
    day = 7;
    hour = 11;
    min = 54;
    sec = 47;
    lon = 19.081599;
    lat = 47.462485;
#endif

    gswe_init(EPHEDIR);

    timestamp = gswe_timestamp_new_from_gregorian_full(year, month, day, hour, min, sec, 0, 1.0);
    moment = gswe_moment_new_full(timestamp, lon, lat, alt, GSWE_HOUSE_SYSTEM_PLACIDUS);
    gswe_moment_add_all_planets(moment);

    g_print("Date: %02d.%02d.%d at %02d:%02d:%02d, at %f, %f\n", year, month, day, hour, min, sec, lon, lat);

    g_print("\nHOUSES\n======\n\n");

    g_list_foreach(gswe_moment_get_house_cusps(moment), print_house_cusp, NULL);

    g_print("\nPLANETS AND POINTS\n==================\n\n");

    // Ascendent
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_ASCENDENT);
    g_print("%s: %s (%f)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->position);

    aspects = gswe_moment_get_planet_aspects(moment, GSWE_PLANET_ASCENDENT);
    g_print("    Aspects:\n");
    g_list_foreach(aspects, (GFunc)print_aspects, GINT_TO_POINTER(GSWE_PLANET_ASCENDENT));
    g_list_free(aspects);

    mirrorpoints = gswe_moment_get_all_planet_mirrorpoints(moment, GSWE_PLANET_ASCENDENT);
    g_print("    Antiscia:\n");
    g_list_foreach(mirrorpoints, (GFunc)print_mirrorpoints, GINT_TO_POINTER(GSWE_PLANET_ASCENDENT));
    g_list_free(mirrorpoints);

    // Midheaven
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_MC);
    g_print("%s: %s (%f)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->position);

    aspects = gswe_moment_get_planet_aspects(moment, GSWE_PLANET_MC);
    g_print("    Aspects:\n");
    g_list_foreach(aspects, (GFunc)print_aspects, GINT_TO_POINTER(GSWE_PLANET_MC));
    g_list_free(aspects);

    mirrorpoints = gswe_moment_get_all_planet_mirrorpoints(moment, GSWE_PLANET_MC);
    g_print("    Antiscia:\n");
    g_list_foreach(mirrorpoints, (GFunc)print_mirrorpoints, GINT_TO_POINTER(GSWE_PLANET_MC));
    g_list_free(mirrorpoints);

    // Vertex
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_VERTEX);
    g_print("%s: %s (%f)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->position);

    aspects = gswe_moment_get_planet_aspects(moment, GSWE_PLANET_VERTEX);
    g_print("    Aspects:\n");
    g_list_foreach(aspects, (GFunc)print_aspects, GINT_TO_POINTER(GSWE_PLANET_VERTEX));
    g_list_free(aspects);

    mirrorpoints = gswe_moment_get_all_planet_mirrorpoints(moment, GSWE_PLANET_VERTEX);
    g_print("    Antiscia:\n");
    g_list_foreach(mirrorpoints, (GFunc)print_mirrorpoints, GINT_TO_POINTER(GSWE_PLANET_VERTEX));
    g_list_free(mirrorpoints);

    // Sun
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_SUN);
    g_print("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    aspects = gswe_moment_get_planet_aspects(moment, GSWE_PLANET_SUN);
    g_print("    Aspects:\n");
    g_list_foreach(aspects, (GFunc)print_aspects, GINT_TO_POINTER(GSWE_PLANET_SUN));
    g_list_free(aspects);

    mirrorpoints = gswe_moment_get_all_planet_mirrorpoints(moment, GSWE_PLANET_SUN);
    g_print("    Antiscia:\n");
    g_list_foreach(mirrorpoints, (GFunc)print_mirrorpoints, GINT_TO_POINTER(GSWE_PLANET_SUN));
    g_list_free(mirrorpoints);

    // Moon
    moon_phase = gswe_moment_get_moon_phase(moment);
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_MOON);
    g_print("%s: %s (%.2f%% visibility), %s, House: %d (%f%s)\n", planet_data->planet_info->name, moonStateName[moon_phase->phase], moon_phase->illumination, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    aspects = gswe_moment_get_planet_aspects(moment, GSWE_PLANET_MOON);
    g_print("    Aspects:\n");
    g_list_foreach(aspects, (GFunc)print_aspects, GINT_TO_POINTER(GSWE_PLANET_MOON));
    g_list_free(aspects);

    mirrorpoints = gswe_moment_get_all_planet_mirrorpoints(moment, GSWE_PLANET_MOON);
    g_print("    Antiscia:\n");
    g_list_foreach(mirrorpoints, (GFunc)print_mirrorpoints, GINT_TO_POINTER(GSWE_PLANET_MOON));
    g_list_free(mirrorpoints);

    // Mercury
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_MERCURY);
    g_print("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    aspects = gswe_moment_get_planet_aspects(moment, GSWE_PLANET_MERCURY);
    g_print("    Aspects:\n");
    g_list_foreach(aspects, (GFunc)print_aspects, GINT_TO_POINTER(GSWE_PLANET_MERCURY));
    g_list_free(aspects);

    mirrorpoints = gswe_moment_get_all_planet_mirrorpoints(moment, GSWE_PLANET_MERCURY);
    g_print("    Antiscia:\n");
    g_list_foreach(mirrorpoints, (GFunc)print_mirrorpoints, GINT_TO_POINTER(GSWE_PLANET_MERCURY));
    g_list_free(mirrorpoints);

    // Venus
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_VENUS);
    g_print("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    aspects = gswe_moment_get_planet_aspects(moment, GSWE_PLANET_VENUS);
    g_print("    Aspects:\n");
    g_list_foreach(aspects, (GFunc)print_aspects, GINT_TO_POINTER(GSWE_PLANET_VENUS));
    g_list_free(aspects);

    mirrorpoints = gswe_moment_get_all_planet_mirrorpoints(moment, GSWE_PLANET_VENUS);
    g_print("    Antiscia:\n");
    g_list_foreach(mirrorpoints, (GFunc)print_mirrorpoints, GINT_TO_POINTER(GSWE_PLANET_VENUS));
    g_list_free(mirrorpoints);

    // Mars
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_MARS);
    g_print("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    aspects = gswe_moment_get_planet_aspects(moment, GSWE_PLANET_MARS);
    g_print("    Aspects:\n");
    g_list_foreach(aspects, (GFunc)print_aspects, GINT_TO_POINTER(GSWE_PLANET_MARS));
    g_list_free(aspects);

    mirrorpoints = gswe_moment_get_all_planet_mirrorpoints(moment, GSWE_PLANET_MARS);
    g_print("    Antiscia:\n");
    g_list_foreach(mirrorpoints, (GFunc)print_mirrorpoints, GINT_TO_POINTER(GSWE_PLANET_MARS));
    g_list_free(mirrorpoints);

    // Jupiter
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_JUPITER);
    g_print("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    aspects = gswe_moment_get_planet_aspects(moment, GSWE_PLANET_JUPITER);
    g_print("    Aspects:\n");
    g_list_foreach(aspects, (GFunc)print_aspects, GINT_TO_POINTER(GSWE_PLANET_JUPITER));
    g_list_free(aspects);

    mirrorpoints = gswe_moment_get_all_planet_mirrorpoints(moment, GSWE_PLANET_JUPITER);
    g_print("    Antiscia:\n");
    g_list_foreach(mirrorpoints, (GFunc)print_mirrorpoints, GINT_TO_POINTER(GSWE_PLANET_JUPITER));
    g_list_free(mirrorpoints);

    // Saturn
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_SATURN);
    g_print("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    aspects = gswe_moment_get_planet_aspects(moment, GSWE_PLANET_SATURN);
    g_print("    Aspects:\n");
    g_list_foreach(aspects, (GFunc)print_aspects, GINT_TO_POINTER(GSWE_PLANET_SATURN));
    g_list_free(aspects);

    mirrorpoints = gswe_moment_get_all_planet_mirrorpoints(moment, GSWE_PLANET_SATURN);
    g_print("    Antiscia:\n");
    g_list_foreach(mirrorpoints, (GFunc)print_mirrorpoints, GINT_TO_POINTER(GSWE_PLANET_SATURN));
    g_list_free(mirrorpoints);

    // Uranus
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_URANUS);
    g_print("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    aspects = gswe_moment_get_planet_aspects(moment, GSWE_PLANET_URANUS);
    g_print("    Aspects:\n");
    g_list_foreach(aspects, (GFunc)print_aspects, GINT_TO_POINTER(GSWE_PLANET_URANUS));
    g_list_free(aspects);

    mirrorpoints = gswe_moment_get_all_planet_mirrorpoints(moment, GSWE_PLANET_URANUS);
    g_print("    Antiscia:\n");
    g_list_foreach(mirrorpoints, (GFunc)print_mirrorpoints, GINT_TO_POINTER(GSWE_PLANET_URANUS));
    g_list_free(mirrorpoints);

    // Neptune
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_NEPTUNE);
    g_print("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    aspects = gswe_moment_get_planet_aspects(moment, GSWE_PLANET_NEPTUNE);
    g_print("    Aspects:\n");
    g_list_foreach(aspects, (GFunc)print_aspects, GINT_TO_POINTER(GSWE_PLANET_NEPTUNE));
    g_list_free(aspects);

    mirrorpoints = gswe_moment_get_all_planet_mirrorpoints(moment, GSWE_PLANET_NEPTUNE);
    g_print("    Antiscia:\n");
    g_list_foreach(mirrorpoints, (GFunc)print_mirrorpoints, GINT_TO_POINTER(GSWE_PLANET_NEPTUNE));
    g_list_free(mirrorpoints);

    // Pluto
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_PLUTO);
    g_print("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    aspects = gswe_moment_get_planet_aspects(moment, GSWE_PLANET_PLUTO);
    g_print("    Aspects:\n");
    g_list_foreach(aspects, (GFunc)print_aspects, GINT_TO_POINTER(GSWE_PLANET_PLUTO));
    g_list_free(aspects);

    mirrorpoints = gswe_moment_get_all_planet_mirrorpoints(moment, GSWE_PLANET_PLUTO);
    g_print("    Antiscia:\n");
    g_list_foreach(mirrorpoints, (GFunc)print_mirrorpoints, GINT_TO_POINTER(GSWE_PLANET_PLUTO));
    g_list_free(mirrorpoints);

    // Mean node
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_MOON_NODE);
    g_print("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    aspects = gswe_moment_get_planet_aspects(moment, GSWE_PLANET_MOON_NODE);
    g_print("    Aspects:\n");
    g_list_foreach(aspects, (GFunc)print_aspects, GINT_TO_POINTER(GSWE_PLANET_MOON_NODE));
    g_list_free(aspects);

    mirrorpoints = gswe_moment_get_all_planet_mirrorpoints(moment, GSWE_PLANET_MOON_NODE);
    g_print("    Antiscia:\n");
    g_list_foreach(mirrorpoints, (GFunc)print_mirrorpoints, GINT_TO_POINTER(GSWE_PLANET_MOON_NODE));
    g_list_free(mirrorpoints);

    // Mean apogee
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_MOON_APOGEE);
    g_print("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    aspects = gswe_moment_get_planet_aspects(moment, GSWE_PLANET_MOON_APOGEE);
    g_print("    Aspects:\n");
    g_list_foreach(aspects, (GFunc)print_aspects, GINT_TO_POINTER(GSWE_PLANET_MOON_APOGEE));
    g_list_free(aspects);

    mirrorpoints = gswe_moment_get_all_planet_mirrorpoints(moment, GSWE_PLANET_MOON_APOGEE);
    g_print("    Antiscia:\n");
    g_list_foreach(mirrorpoints, (GFunc)print_mirrorpoints, GINT_TO_POINTER(GSWE_PLANET_MOON_APOGEE));
    g_list_free(mirrorpoints);

    // Chiron
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_CHIRON);
    g_print("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    aspects = gswe_moment_get_planet_aspects(moment, GSWE_PLANET_CHIRON);
    g_print("    Aspects:\n");
    g_list_foreach(aspects, (GFunc)print_aspects, GINT_TO_POINTER(GSWE_PLANET_CHIRON));
    g_list_free(aspects);

    mirrorpoints = gswe_moment_get_all_planet_mirrorpoints(moment, GSWE_PLANET_CHIRON);
    g_print("    Antiscia:\n");
    g_list_foreach(mirrorpoints, (GFunc)print_mirrorpoints, GINT_TO_POINTER(GSWE_PLANET_CHIRON));
    g_list_free(mirrorpoints);

    // Ceres
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_CERES);
    g_print("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    aspects = gswe_moment_get_planet_aspects(moment, GSWE_PLANET_CERES);
    g_print("    Aspects:\n");
    g_list_foreach(aspects, (GFunc)print_aspects, GINT_TO_POINTER(GSWE_PLANET_CERES));
    g_list_free(aspects);

    mirrorpoints = gswe_moment_get_all_planet_mirrorpoints(moment, GSWE_PLANET_CERES);
    g_print("    Antiscia:\n");
    g_list_foreach(mirrorpoints, (GFunc)print_mirrorpoints, GINT_TO_POINTER(GSWE_PLANET_CERES));
    g_list_free(mirrorpoints);

    // Pallas
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_PALLAS);
    g_print("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    aspects = gswe_moment_get_planet_aspects(moment, GSWE_PLANET_PALLAS);
    g_print("    Aspects:\n");
    g_list_foreach(aspects, (GFunc)print_aspects, GINT_TO_POINTER(GSWE_PLANET_PALLAS));
    g_list_free(aspects);

    mirrorpoints = gswe_moment_get_all_planet_mirrorpoints(moment, GSWE_PLANET_PALLAS);
    g_print("    Antiscia:\n");
    g_list_foreach(mirrorpoints, (GFunc)print_mirrorpoints, GINT_TO_POINTER(GSWE_PLANET_PALLAS));
    g_list_free(mirrorpoints);

    // Juno
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_JUNO);
    g_print("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    aspects = gswe_moment_get_planet_aspects(moment, GSWE_PLANET_JUNO);
    g_print("    Aspects:\n");
    g_list_foreach(aspects, (GFunc)print_aspects, GINT_TO_POINTER(GSWE_PLANET_JUNO));
    g_list_free(aspects);

    mirrorpoints = gswe_moment_get_all_planet_mirrorpoints(moment, GSWE_PLANET_JUNO);
    g_print("    Antiscia:\n");
    g_list_foreach(mirrorpoints, (GFunc)print_mirrorpoints, GINT_TO_POINTER(GSWE_PLANET_JUNO));
    g_list_free(mirrorpoints);

    // Vesta
    planet_data = gswe_moment_get_planet(moment, GSWE_PLANET_VESTA);
    g_print("%s: %s, House: %d (%f%s)\n", planet_data->planet_info->name, planet_data->sign->name, planet_data->house, planet_data->position, (planet_data->retrograde) ? ", retrograde" : "");

    aspects = gswe_moment_get_planet_aspects(moment, GSWE_PLANET_VESTA);
    g_print("    Aspects:\n");
    g_list_foreach(aspects, (GFunc)print_aspects, GINT_TO_POINTER(GSWE_PLANET_VESTA));
    g_list_free(aspects);

    mirrorpoints = gswe_moment_get_all_planet_mirrorpoints(moment, GSWE_PLANET_VESTA);
    g_print("    Antiscia:\n");
    g_list_foreach(mirrorpoints, (GFunc)print_mirrorpoints, GINT_TO_POINTER(GSWE_PLANET_VESTA));
    g_list_free(mirrorpoints);

    g_print("\nELEMENTS\n========\n\n");

    g_print("Fire: %d\n", gswe_moment_get_element_points(moment, GSWE_ELEMENT_FIRE));
    g_print("Earth: %d\n", gswe_moment_get_element_points(moment, GSWE_ELEMENT_EARTH));
    g_print("Air: %d\n", gswe_moment_get_element_points(moment, GSWE_ELEMENT_AIR));
    g_print("Water: %d\n", gswe_moment_get_element_points(moment, GSWE_ELEMENT_WATER));

    g_print("\nQUALITIES\n=========\n\n");

    g_print("Cardinal: %d\n", gswe_moment_get_quality_points(moment, GSWE_QUALITY_CARDINAL));
    g_print("Fix: %d\n", gswe_moment_get_quality_points(moment, GSWE_QUALITY_FIX));
    g_print("Mutable: %d\n", gswe_moment_get_quality_points(moment, GSWE_QUALITY_MUTABLE));

    return 0;
}

