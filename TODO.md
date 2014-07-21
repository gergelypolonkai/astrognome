#TODO list for Astrognome

The code is currently tested only on Fedora. If you can install the required
libraries on your system, please test it and provide feedback (even positive)!

We are looking for people to package Astrognome for other systems, like
Debian/Ubuntu, Arch or Gentoo.

## Priority features to implement

The features below will be implemented in the first release of Astrognome.

* Implement storage database. Database should contain a small preview image of
  the chart. The selection screen should show a small box with the name and the
  preview image. Upon mouseover it should blur/darken the image, and write some
  chart data over it, like birth date and place
* Placidus .HOR import
* Chart cleanup and manipulation
* Generate, load and save synastries
* Calculate transits and progressions
* Stepping through time without modifying the chart data
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
* Regiomontanus import

## Later features

The features below will become part of Astrognome in later versions.

* Custom location database
* Synastries (Composite charts)
* Transits
* Progression
* Chart export as different types of images
* Chart printing

