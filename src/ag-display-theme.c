/* ag-display-theme.c - Display theme management for Astrognome
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
#include <glib/gi18n.h>
#include <swe-glib.h>

#include "astrognome.h"
#include "ag-display-theme.h"

static gchar *planet_rule           = ".planet-%s { visibility: hidden; }\n";
static gchar *aspect_type_rule      = ".aspect-%s { visibility: hidden; }\n";
static gchar *aspect_planet_rule    = ".aspect-p-%s { visibility: hidden; }\n";
static gchar *antiscion_type_rule   = ".antiscion-%s { visibility: hidden; }\n";
static gchar *antiscion_planet_rule = ".antiscion-p-%s { visibility: hidden; }\n";

static AgDisplayTheme **builtin_themes = NULL;
static gchar          *builtin_theme_name[AG_DISPLAY_THEME_COUNT] = {
    NC_("Display theme name", "Everything"),
    NC_("Display theme name", "Classic"),
    NC_("Display theme name", "No comets"),
};

gchar *
ag_display_theme_to_css(AgDisplayTheme *theme)
{
    gint    i;
    GList   *l,
            *all_aspects,
            *all_antiscion_axes;
    gchar   *ret;
    GString *css = NULL;

    // Null themes are possible, deal with it!
    if (theme == NULL) {
        return g_strdup("");
    }

    css = g_string_sized_new(300);

    // Go through the used_planets array. If any of them is not listed
    // by @theme, add a rule to hide it, and the respective aspects
    // and antiscia
    for (i = 0; i < used_planets_count; i++) {
        gboolean in_list;

        in_list = (g_list_find(theme->planets, GINT_TO_POINTER(used_planets[i])) != NULL);

        if (theme->planets_include != in_list) {
            const gchar *planet_name;

            planet_name = ag_planet_id_to_nick(used_planets[i]);

            g_string_append_printf(css, planet_rule, planet_name);
            g_string_append_printf(css, aspect_planet_rule, planet_name);
            g_string_append_printf(css, antiscion_planet_rule, planet_name);
        }
    }

    // Go through gswe_all_aspects(). If any of them is not listed by
    // @theme, add a rule to hide it
    all_aspects = gswe_all_aspects();

    for (l = all_aspects; l; l = g_list_next(l)) {
        const gchar    *aspect_type;
        gboolean       in_list;
        GsweAspect     aspect;
        GsweAspectInfo *aspect_info = l->data;

        aspect      = gswe_aspect_info_get_aspect(aspect_info);
        in_list     = (g_list_find(theme->aspects, GINT_TO_POINTER(aspect)) != NULL);
        aspect_type = ag_aspect_id_to_nick(aspect);

        if (theme->aspects_include != in_list) {
            g_string_append_printf(css, aspect_type_rule, aspect_type);
        }
    }

    g_list_free(all_aspects);

    // Go through gswe_all_antiscion_axes(). If any of them is not
    // listed by @theme, add a rule to hide it
    all_antiscion_axes = gswe_all_antiscion_axes();

    for (l = all_antiscion_axes; l; l = g_list_next(l)) {
        const gchar           *antiscion_axis_type;
        gboolean              in_list;
        GsweAntiscionAxis     antiscion_axis;
        GsweAntiscionAxisInfo *antiscion_axis_info = l->data;

        antiscion_axis      = gswe_antiscion_axis_info_get_axis(antiscion_axis_info);
        in_list             = (g_list_find(theme->antiscia, GINT_TO_POINTER(antiscion_axis)) != NULL);
        antiscion_axis_type = ag_antiscion_axis_id_to_nick(antiscion_axis);

        if (theme->antiscia_include != in_list) {
            g_string_append_printf(css, antiscion_type_rule, antiscion_axis_type);
        }
    }

    g_list_free(all_antiscion_axes);

    ret = g_string_free(css, FALSE);

    g_debug("Generated CSS:\n%s\n", ret);

    return ret;
}

static AgDisplayTheme *
ag_display_theme_get_builtin(gint id)
{
    AgDisplayTheme *theme;

    // Builtin themes all have a negative ID
    g_return_val_if_fail(id < 0, NULL);
    // And must be greater then AG_DISPLAY_THEME_COUNT
    g_return_val_if_fail(id >= -AG_DISPLAY_THEME_COUNT, NULL);

    if (builtin_themes == NULL) {
        builtin_themes = g_new0(AgDisplayTheme *, AG_DISPLAY_THEME_COUNT);
    }

    // If the theme is already created, return it
    if (builtin_themes[-id - 1] != NULL) {
        return builtin_themes[-id - 1];
    }

    builtin_themes[-id - 1] = theme = g_new0(AgDisplayTheme, 1);

    switch (id) {
        case AG_DISPLAY_THEME_ALL:
            theme->id               = id;
            theme->name             = gettext(builtin_theme_name[-id - 1]);
            theme->builtin          = TRUE;
            theme->planets_include  = FALSE;
            theme->planets          = NULL;
            theme->aspects_include  = FALSE;
            theme->aspects          = NULL;
            theme->antiscia_include = FALSE;
            theme->antiscia         = NULL;

            break;

        case AG_DISPLAY_THEME_CLASSIC:
            // TODO: If SWE-GLib would support it, we could programatically get
            //       classic planets and aspects here, thus, if it is messed up,
            //       we need to fix it only once.
            theme->id               = id;
            theme->name             = gettext(builtin_theme_name[-id - 1]);
            theme->builtin          = TRUE;
            theme->planets_include  = TRUE;
            theme->planets          = NULL;
            theme->aspects_include  = TRUE;
            theme->aspects          = NULL;
            theme->antiscia_include = TRUE;
            theme->antiscia         = NULL;

            theme->planets = g_list_prepend(
                    theme->planets,
                    GINT_TO_POINTER(GSWE_PLANET_SUN)
                );
            theme->planets = g_list_prepend(
                    theme->planets,
                    GINT_TO_POINTER(GSWE_PLANET_MOON)
                );
            theme->planets = g_list_prepend(
                    theme->planets,
                    GINT_TO_POINTER(GSWE_PLANET_MERCURY)
                );
            theme->planets = g_list_prepend(
                    theme->planets,
                    GINT_TO_POINTER(GSWE_PLANET_VENUS)
                );
            theme->planets = g_list_prepend(
                    theme->planets,
                    GINT_TO_POINTER(GSWE_PLANET_MARS)
                );
            theme->planets = g_list_prepend(
                    theme->planets,
                    GINT_TO_POINTER(GSWE_PLANET_JUPITER)
                );
            theme->planets = g_list_prepend(
                    theme->planets,
                    GINT_TO_POINTER(GSWE_PLANET_SATURN)
                );
            theme->planets = g_list_prepend(
                    theme->planets,
                    GINT_TO_POINTER(GSWE_PLANET_MOON_NODE)
                );

            theme->aspects = g_list_prepend(
                    theme->aspects,
                    GINT_TO_POINTER(GSWE_ASPECT_CONJUCTION)
                );
            theme->aspects = g_list_prepend(
                    theme->aspects,
                    GINT_TO_POINTER(GSWE_ASPECT_OPPOSITION)
                );
            theme->aspects = g_list_prepend(
                    theme->aspects,
                    GINT_TO_POINTER(GSWE_ASPECT_TRINE)
                );
            theme->aspects = g_list_prepend(
                    theme->aspects,
                    GINT_TO_POINTER(GSWE_ASPECT_SQUARE)
                );
            theme->aspects = g_list_prepend(
                    theme->aspects,
                    GINT_TO_POINTER(GSWE_ASPECT_SEXTILE)
                );

            break;

        case AG_DISPLAY_THEME_NO_COMETS:
            // TODO: If SWE-GLib would support it, we could programatically get
            //       classic planets and aspects here, thus, if it is messed up,
            //       we need to fix it only once.
            theme->id               = id;
            theme->name             = gettext(builtin_theme_name[-id - 1]);
            theme->builtin          = TRUE;
            theme->planets_include  = TRUE;
            theme->planets          = NULL;
            theme->aspects_include  = FALSE;
            theme->aspects          = NULL;
            theme->antiscia_include = FALSE;
            theme->antiscia         = NULL;

            theme->planets = g_list_prepend(
                    theme->planets,
                    GINT_TO_POINTER(GSWE_PLANET_SUN)
                );
            theme->planets = g_list_prepend(
                    theme->planets,
                    GINT_TO_POINTER(GSWE_PLANET_MOON)
                );
            theme->planets = g_list_prepend(
                    theme->planets,
                    GINT_TO_POINTER(GSWE_PLANET_MERCURY)
                );
            theme->planets = g_list_prepend(
                    theme->planets,
                    GINT_TO_POINTER(GSWE_PLANET_VENUS)
                );
            theme->planets = g_list_prepend(
                    theme->planets,
                    GINT_TO_POINTER(GSWE_PLANET_MARS)
                );
            theme->planets = g_list_prepend(
                    theme->planets,
                    GINT_TO_POINTER(GSWE_PLANET_JUPITER)
                );
            theme->planets = g_list_prepend(
                    theme->planets,
                    GINT_TO_POINTER(GSWE_PLANET_SATURN)
                );
            theme->planets = g_list_prepend(
                    theme->planets,
                    GINT_TO_POINTER(GSWE_PLANET_URANUS)
                );
            theme->planets = g_list_prepend(
                    theme->planets,
                    GINT_TO_POINTER(GSWE_PLANET_NEPTUNE)
                );
            theme->planets = g_list_prepend(
                    theme->planets,
                    GINT_TO_POINTER(GSWE_PLANET_PLUTO)
                );
            theme->planets = g_list_prepend(
                    theme->planets,
                    GINT_TO_POINTER(GSWE_PLANET_MOON_NODE)
                );

            break;

        default:
            g_free(theme);
            g_warning("Trying to instantiate unknown builtin theme %d", id);
            return NULL;
    }

    return theme;
}

GList *
ag_display_theme_get_list(void)
{
    int   i;
    GList *ret = NULL;

    for (i = 1; i <= AG_DISPLAY_THEME_COUNT; i++) {
        AgDisplayTheme *theme;

        theme = ag_display_theme_get_builtin(-i);
        ret = g_list_append(ret, theme);
    }

    return ret;
}

AgDisplayTheme *
ag_display_theme_get_by_id(int id)
{
    if (id < 0) {
        return ag_display_theme_get_builtin(id);
    }

    g_warning("Invalid theme ID. Falling back to display everything.");

    return ag_display_theme_get_builtin(AG_DISPLAY_THEME_ALL);
}

void
ag_display_theme_free(AgDisplayTheme *display_theme)
{
    if (!display_theme || display_theme->builtin)
    {
        return;
    }

    g_free(display_theme->name);
    g_list_free(display_theme->planets);
    g_list_free(display_theme->aspects);
    g_list_free(display_theme->antiscia);
    g_free(display_theme);
}
