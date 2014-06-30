#Hacking Astrognome

To hack Astrognome, you should get the recent GNOME libraries. Astrognome is
using features of versions up to and including 3.10. Also, you should install
[SWE-Glib](https://github.com/gergelypolonkai/swe-glib/). It worths mentioning
that SWE-GLib is sometimes evolves together with Astrognome: originally the two
were in one project, but after a while I realised that SWE-GLib can be powerful
in itself, so I split the project into two.

As of 30 July, 2014 Astrognome is developed using the [GitHub
flow](https://guides.github.com/introduction/flow/). Basically, the main
requirement is that everything in the master branch is working at any given
time. If you have a new idea, create a separate branch for it, and if you think
it is done, issue a pull request.

If, for any reason, you don’t like GitHub, you can still contribute. After
cloning the repo, you can still create your own feature/bugfix branch, and send
me a git-bundle or a simple patch (in which case you should mention the base
you worked with) via e-mail.

If you don’t know how you can contribute, take a look at the TODO file or the
issue list on GitHub.

## Coding style

Astrognome follows a well-defined coding style. If you contribute, please
follow that by looking at existing sources, or use
[Uncrustify](http://uncrustify.sourceforge.net/) with the config file under
docs/. The only thing it messes up currently is the alignment of object type
\#definitions and multiline variable definition blocks.
