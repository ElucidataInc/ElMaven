!/bin/bash

wget http://repo.msys2.org/mingw/x86_64/mingw-w64-x86_64-gcc-7.3.0-1-any.pkg.tar.xz \
     http://repo.msys2.org/mingw/x86_64/mingw-w64-x86_64-gcc-ada-7.3.0-1-any.pkg.tar.xz \
     http://repo.msys2.org/mingw/x86_64/mingw-w64-x86_64-gcc-fortran-7.3.0-1-any.pkg.tar.xz \
     http://repo.msys2.org/mingw/x86_64/mingw-w64-x86_64-gcc-libgfortran-7.3.0-1-any.pkg.tar.xz \
     http://repo.msys2.org/mingw/x86_64/mingw-w64-x86_64-gcc-libs-7.3.0-1-any.pkg.tar.xz \
     http://repo.msys2.org/mingw/x86_64/mingw-w64-x86_64-gcc-objc-7.3.0-1-any.pkg.tar.xz


pacman --noconfirm -U mingw-w64-x86_64-gcc-7.3.0-1-any.pkg.tar.xz \
	  mingw-w64-x86_64-gcc-ada-7.3.0-1-any.pkg.tar.xz \
	  mingw-w64-x86_64-gcc-fortran-7.3.0-1-any.pkg.tar.xz \
	  mingw-w64-x86_64-gcc-libgfortran-7.3.0-1-any.pkg.tar.xz \
	  mingw-w64-x86_64-gcc-libs-7.3.0-1-any.pkg.tar.xz \
	  mingw-w64-x86_64-gcc-objc-7.3.0-1-any.pkg.tar.xz
