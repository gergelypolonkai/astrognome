#include <glib/gi18n.h>
#include <swe-glib.h>

#include "astrognome.h"
#include "ag-display-theme.h"

static gchar *planet_all = ".planet {\n" \
                           "    visibility: visible;\n" \
                           "}\n";

static gchar *planet_none = ".planet {\n" \
                            "    visibility: hidden;\n" \
                            " }\n";

static gchar *planet_tmpl = ".planet-%s {\n" \
                            "    visibility: %s !important;\n" \
                            "}\n";

static gchar *aspect_all = ".aspect {\n" \
                           "    visibility: visible;\n" \
                           "}\n";

static gchar *aspect_none = ".aspect {\n" \
                            "    visibility: hidden;\n" \
                            " }\n";

static gchar *aspect_tmpl = ".aspect-%s {\n" \
                            "    visibility: %s !important;\n" \
                            "}\n";

static gchar *antiscion_all = ".antiscion {\n" \
                              "    visibility: visible;\n" \
                              "}\n";

static gchar *antiscion_none = ".antiscion {\n" \
                               "    visibility: hidden;\n" \
                               "}\n";

static gchar *antiscion_tmpl = ".antiscion-%s {\n" \
                            "    visibility: %s !important;\n" \
                            "}\n";

static AgDisplayTheme **builtin_themes = NULL;
static gchar          *builtin_theme_name[AG_DISPLAY_THEME_COUNT] = {
    NC_("Display theme name", "Everything"),
    NC_("Display theme name", "Classic"),
    NC_("Display theme name", "No comets"),
};

gchar *
ag_display_theme_to_css(AgDisplayTheme *theme)
{
    GList   *i;
    gchar   *ret;
    GString *css = NULL;

    // Null themes are possible, deal with it!
    if (theme == NULL) {
        return g_strdup("");
    }

    if (theme->planets_include) {
        css = g_string_new(planet_none);
    } else {
        css = g_string_new(planet_all);
    }

    for (i = theme->planets; i; i = g_list_next(i)) {
        const gchar *planet_name;

        planet_name = ag_planet_id_to_nick(GPOINTER_TO_INT(i->data));

        g_string_append_printf(
                css,
                planet_tmpl,
                planet_name,
                (theme->planets_include) ? "visible" : "hidden"
            );
    }

    if (theme->aspects_include) {
        g_string_append(css, aspect_none);
    } else {
        g_string_append(css, aspect_all);
    }

    for (i = theme->aspects; i; i = g_list_next(i)) {
        const gchar *aspect_name;

        aspect_name = ag_aspect_id_to_nick(GPOINTER_TO_INT(i->data));

        g_string_append_printf(
                css,
                aspect_tmpl,
                aspect_name,
                (theme->aspects_include) ? "visible" : "hidden"
            );
    }

    if (theme->antiscia_include) {
        g_string_append(css, antiscion_none);
    } else {
        g_string_append(css, antiscion_all);
    }

    for (i = theme->antiscia; i; i = g_list_next(i)) {
        const gchar *antiscion_axis_name;

        antiscion_axis_name = ag_antiscion_axis_id_to_nick(
                GPOINTER_TO_INT(i->data)
            );

        g_string_append_printf(
                css,
                antiscion_tmpl,
                antiscion_axis_name,
                (theme->antiscia_include) ? "visible" : "hidden"
            );
    }

    ret = g_string_free(css, FALSE);

    g_debug("%s", ret);

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
