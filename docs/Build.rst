Build
=====

|

Contributors can build El-MAVEN on Windows, Ubuntu or Mac systems by following these instructions. 
Users are recommended to download the installers provided on the El-MAVEN website.

Windows
-------

* Download `MSYS2 <http://www.msys2.org/>`_ installer and follow the installation instructions provided on their website.

* Open MSYS2 and give the following commands to set up libraries and tool chains for El-MAVEN. Reopen MSYS2 when required:

   * For 64 bit

   ::

   $ pacman --force -Sy
   $ pacman --force -Syu
   $ pacman --force -Su
   $ pacman --force -Sy base-devel msys2-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-qt5 mingw64/mingw-w64-x86_64-hdf5 mingw64/mingw-w64-x86_64-netcdf mingw64/mingw-w64-x86_64-boost msys/git

   * For 32 bit

   ::

   $ pacman --force -Sy
   $ pacman --force -Syu
   $ pacman --force -Su
   $ pacman --force -Sy base-devel msys2-devel mingw-i686-toolchain mingw-i686-qt5 mingw32/mingw-i686-hdf5 mingw32/mingw-i686-netcdf mingw32/mingw-i686-boost msys/git

* Open mingw64.exe from the MSYS2 folder and give the following commands:

   ::

   $ cd <PathToInstallationFolder> #for example: cd /c/User/Admin/Desktop
   $ git clone https://github.com/ElucidataInc/ElMaven.git
   $ ./run.sh
   $ ./bin/El_Maven_0.x #for example: ./bin/El_Maven_0.2

El-MAVEN loads with two windows: one for logging the application status and another El-MAVEN 
application window for data analysis.

Ubuntu
------

* Open the terminal and give the following commands to set up libraries and tool chains for El-MAVEN 

   ::

   $ sudo apt-get update``
   $ sudo apt-get install g++
   $ sudo apt-get install qt5-qmake qtbase5-dev qtscript5-dev qtdeclarative5-dev libqt5webkit5-dev libsqlite3-dev libboost-all-dev lcov libnetcdf-dev
   $ cd <PathToInstallationFolder> #for example: user@pc:~$ cd Desktop/
   $ git clone https://github.com/ElucidataInc/ElMaven.git
   $ ./run.sh
   $ ./bin/El_Maven_0.x #for example: ./bin/El_Maven_0.2

El-MAVEN loads with two windows: one for logging the application status and another El-MAVEN application window for data analysis.

MacOS
-----

* Install Xcode from App store

* Download and Install `Qt5.6 <http://download.qt.io/official_releases/qt/5.6/5.6.2/qt-opensource-mac-x64-clang-5.6.2.dmg>`_

This will give you the Qt5.6.2 dmg file. Using the dmg file install Qt under the directory /Users/Your_User_Name/

* Using the terminal execute the following commands:

   ::

   $ sudo xcodebuild -license accept
   $ xcode-select --install
   $ /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
   $ brew install boost, brew install llvm@3.7, brew install netcdf
   $ cd ~
   $ touch .profile
   $ echo "PATH=/Users/$USER/Qt5.6.2/5.6/clang_64/bin/:$PATH" > .profile
   $ source .profile
   $ mkdir ~/maven_repo
   $ cd ~/maven_repo
   $ git clone https://github.com/ElucidataInc/ElMaven.git
   $ cd ElMaven
   $ source ~/.profile
   $ qmake CONFIG+=debug -o Makefile build.pro
   $ make -j4

Switching Versions
------------------

Users can switch between versions once they have compiled El-MAVEN successfully on their system. 
Follow these steps to pull a specific release:

* Choose the version you wish to install from the list of releases. (We recommend the version tagged "Latest release". Pre-releases are not stable and should be avoided)

* Find the version tag (v0.2.x, 0.1.x, etc) on the left of release notes.

* Open your terminal and move to the installation folder

* Give the following commands in the terminal:

   ::

   $ cd ElMaven
   $ ./uninstall.sh
   $ git checkout develop
   $ git pull
   $ git checkout v0.x.y (Example: v0.4.1)

   * Build the new version using the following commands:

      * For Windows and Ubuntu

      ::

         $ ./run.sh

      * For MacOS

      ::

         $ source ~/.profile
         $ qmake CONFIG+=debug -o Makefile build.pro
         $ make -j4
