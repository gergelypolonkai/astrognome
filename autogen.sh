#!/bin/sh
# Run this to generate all the initial makefiles, etc.

FOUND_GNOME_AUTOGEN_SH=1

which gnome-autogen.sh &> /dev/null || FOUND_GNOME_AUTOGEN_SH=0

if test x"$FOUND_GNOME_AUTOGEN_SH" = x"1"; then
    gnome-autogen.sh
else
    echo "GNOME's gnome-autogen.sh can not be found in your path. If you have it in a"
    echo "non-trivial place, simply call it now, with e.g."
    echo "/non/trivial/dir/gnome-autogen.sh. In RPM based distributions it can be"
    echo "found in the gnome-common package."
fi

