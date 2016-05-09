May 1994
Analytical Data Interchange for Mass Spectrometry
ANDI/MS

This directory contains the source code for the ANDI/MS
Application Programming Interface tool kit.  An example
Makefile (for a Digital Equipment Corportaion Ultrix
platform - a UNIX variant) is also provided.

You will almost certainly have to edit the (or create a new)
Makefile if you are not using a Unix platform.  The goal
is to compile each of the .c files in this directory, and
make an object code library from them, which can be linked
into programs.  Microsoft Windows programmers might wish
to compile into a dynamic link library (DLL) instead of
a static library.

Aside from system include files, these source code files
depend on include files from the netCDF "include" subdirectory.
You may have to add an explicit include directive to your
compiler options or project manager which points to the
netCDF include subdirectory.

Programs linked with this library also require that the
netCDF library (and possibly the optional XDR library
included with the netCDF distribution, if your platform
does not provide one as standard) be available.  You must
therefore build those parts of the netCDF distribution before
and applications can be linked.