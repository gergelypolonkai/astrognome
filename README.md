#Astrognome

## Features

The features below are already implemented.

* Calculate planetary positions for any date, time and place on Earth, their aspects and mirrorpoints
* Create natal charts for any date, time and place on earth, with exact planetary locations and their aspects

## Upcoming features

The features below will be implemented in the first release of Astrognome.

* Optionally include dwarf planets and comets: Chiron, Pallas, Ceres, Juno and Vesta
* Optionally include Vertex
* Time stepping
* Application settings
  * Default display properties (can be overridden per window)
    * Traditional view (personal planets and Ptolemaic aspects only)
    * Display major (Ptolemaic) aspects
    * Display minor aspects
    * Display antiscia/contra-antiscia
    * Display personal planets
    * Display outer planets
    * Display dwarf planets and asteroids
    * Display fixed stars
    * Display hidden ascendent
    * Display Vertex/anti-Vertex
  * Symbol of Uranus and Pluto to use in charts

## Later features

The features below will become part of Astrognome in later versions.

* Custom location database
* Synastries (Composite charts)
* Transits
* Progression

## Credits

Astrognome was originally created by Jean-André Santoni, and was hosted on [Google Code](https://code.google.com/p/astrognome/). The original project is now rewrote from scratch, using the [SWE-GLib](http://github.com/gergelypolonkai/swe-glib) library (which is actually a wrapper around [Swiss Ephemeris library](http://www.astro.com/swisseph/)). Several ideas come from the original project, and from others like [Placidus](http://placidus.hu/) and [Aquarius2Go](https://play.google.com/store/apps/details?id=net.wilfinger.aquarius2go).

## Hacking

The project is currently hosted on [GitHub](https://github.com/gergelypolonkai/astrognome). Just fork the repo, make your changes and issue a pull request. Don't like GitHub? That's fine with me; in this case, clone the repo, and send your changes, and send me your modifications at gergely@polonkai.eu as a git-bundle or a patch (in this latter case, you should also mention which commit is your base).

The entire project is written in C, utilizing GTK+ (3.8 currently, but the final version may come only with 3.10) and SWE-GLib (which is used for the calculations part).

## !!!WARNING!!!
-------------

The code is a bit messy yet, and displays calculated data only in a textual form. Graphics will be added later.
