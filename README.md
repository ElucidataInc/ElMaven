<h3 align="center">El-MAVEN</h3>
<p align="center">An intuitive, opensource LC-MS data processing engine </p>
<p align="center">from </p>
<p align="center">
  <a href="http://www.elucidata.io"> 
	<img src="http://elucidata.io/static/images/elucidata-logo-with-text.svg">
  </a>

## Travis Build

[![Build Status](https://travis-ci.org/ElucidataInc/ElMaven.svg?branch=develop)](https://travis-ci.org/ElucidataInc/ElMaven)

## Digital Object Identifier (DOI)

[![DOI](https://zenodo.org/badge/70220005.svg)](https://zenodo.org/badge/latestdoi/70220005)

## Table of contents
- [Download](#download)
- [Build](#build)
  - [Windows](#windows)
  - [Ubuntu](#ubuntu)
  - [Mac](#mac)
  - [Switching versions](#switching-versions)
- [El-MAVEN features](#el-maven-features)
- [Bugs and feature requests](#bugs-and-feature-requests)
- [Documentation](#documentation)
- [Contributing](#contributing)
- [Contributors](#contributors)
- [References](#references)
- [Acknowledgment](#acknowledgment)
- [Copyright and license](#copyright-and-license)

## Download

El-MAVEN installers are available for Windows (7, 8, 10) and Mac.
Download [El-MAVEN](https://elucidatainc.github.io/ElMaven/) latest version or daily build for your preferred environment.

## Build

Contributers can build El-MAVEN on Windows, Ubuntu or Mac systems by following these instructions. Users are recommended to download the installers provided on the El-MAVEN website.

### Windows

- Download [MSYS2](http://www.msys2.org/) installer and follow the installation instructions provided on their website.
- Open MSYS2 and give the following commands to set up libraries and tool chains for El-MAVEN. Reopen MSYS2 when required:
- **For 64 bit**:  
  `pacman --force -Sy`  
  `pacman --force -Syu`  
  `pacman --force -Su`  
  `pacman --force -Sy base-devel msys2-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-qt5 mingw64/mingw-w64-x86_64-hdf5 mingw64/mingw-w64-x86_64-netcdf mingw64/mingw-w64-x86_64-boost msys/git`

- **For 32 bit**:  
  `pacman --force -Sy`  
  `pacman --force -Syu`  
  `pacman --force -Su`  
  `pacman --force -Sy base-devel msys2-devel mingw-i686-toolchain mingw-i686-qt5 mingw32/mingw-i686-hdf5 mingw32/mingw-i686-netcdf mingw32/mingw-i686-boost msys/git`

- Open mingw64.exe from the MSYS2 folder and give the following commands:  
  `cd <PathToInstallationFolder> #for example: cd /c/User/Admin/Desktop`  
  `git clone https://github.com/ElucidataInc/ElMaven.git`  
  `./run.sh`  
  `./bin/El_Maven_0.x #for example: ./bin/El_Maven_0.2`

El-MAVEN loads with two windows: one for logging the application status and another El-MAVEN application window for data analysis.

### Ubuntu

- Open the terminal and give the following commands to set up libraries and tool chains for El-MAVEN.
  `sudo apt-get update`  
  `sudo apt-get install g++`  
  `sudo apt-get install qt5-qmake qtbase5-dev qtscript5-dev qtdeclarative5-dev libqt5webkit5-dev libsqlite3-dev libboost-all-dev lcov libnetcdf-dev`  
  `cd <PathToInstallationFolder> #for example: user@pc:~$ cd Desktop/`  
  `git clone https://github.com/ElucidataInc/ElMaven.git`  
  `./run.sh`  
  `./bin/El_Maven_0.x #for example: ./bin/El_Maven_0.2`

El-MAVEN loads with two windows: one for logging the application status and another El-MAVEN application window for data analysis.

### Mac

- Install Xcode from App store

- Download and Install Qt5.6 from http://download.qt.io/official_releases/qt/5.6/5.6.2/qt-opensource-mac-x64-clang-5.6.2.dmg.

This is will give you the Qt5.6.2 dmg file. Using the dmg file install Qt under the directory /Users/Your_User_Name/

**Using the terminal execute the following instructions**

`sudo xcodebuild -license accept`

`xcode-select --install`

`/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"`

`brew install boost`
`brew install llvm@3.7`
`brew install netcdf`

`cd ~`

`touch .profile`

`echo "PATH=/Users/$USER/Qt5.6.2/5.6/clang_64/bin/:$PATH" > .profile`

`source .profile`

`mkdir ~/maven_repo`

`cd ~/maven_repo`

`git clone https://github.com/ElucidataInc/ElMaven.git`

`cd ElMaven`

`source ~/.profile`

`qmake CONFIG+=debug -o Makefile build.pro`

`make -j4`

### Switching versions

Users can switch between versions once they have compiled El-MAVEN successfully on their system. Follow these steps to pull a specific release:

- Choose the version you wish to install from the list of releases. (We recommend the version tagged "Latest release". Pre-releases are not stable and should be avoided)
- Find the version tag (v0.2.x, 0.1.x, etc) on the left of release notes.
- Open your terminal and move to the installation folder
- Give the following commands in the terminal:  
  `cd ElMaven`  
  `./uninstall.sh` (to uninstall your current version)  
  `git checkout develop`  
  `git pull`  
  `git checkout v0.x.y` (Eg. v0.1.5)
- Build the new version using the following command:

For Windows and Ubuntu:
`./run.sh`

For Mac:  
`source ~/.profile`

`qmake CONFIG+=debug -o Makefile build.pro`

`make -j4`

## El-MAVEN features

[Maven](http://genomics-pubs.princeton.edu/mzroll/index.php) and [El-MAVEN](https://elucidatainc.github.io/ElMaven/) share following features:

- Multi-file chromatographic aligner
- Peak-feature detector
- Isotope and adduct calculator
- Formula predictor
- Pathway visualizer
- Isotopic flux animator

El-MAVEN is robust, faster and with more user friendly features compared to Maven.

## Bugs and feature requests

Existing bugs and feature requests can be found on [El-MAVEN github issue page](https://github.com/ElucidataInc/ElMaven/issues). Please search existing bugs and feature requests before you file a bug or request a feature.

## Documentation

El-MAVEN user documentation can be found on the [GitWiki](https://github.com/ElucidataInc/ElMaven/wiki) page.

## Contributing

You are welcome to contribute. Please go through our [contributing guidelines](https://github.com/ElucidataInc/ElMaven/blob/develop/CONTRIBUTING.md) and [code of conduct](https://github.com/ElucidataInc/ElMaven/blob/develop/CODE_OF_CONDUCT.md). These guidelines include directions for coding standards, filing issues and development guidelines.

Pull requests must include relevant unit tests. All the functional features are to be tested before committing the code.

## Contributors

- [Maven team at Princeton University](http://genomics-pubs.princeton.edu/mzroll/index.php)
- [Eugene Melamud](https://www.calicolabs.com/team-member/eugene-melamud/)
- [Victor Chubukov](https://github.com/chubukov)
- [George Sabu](https://github.com/GeorgeSabu)
- [Sahil](https://github.com/sahil21)
- [Raghav Sehgal](https://github.com/Raghavdata)
- [Shubhra Agrawal](https://github.com/shubhra-agrawal)
- [Raghuram Reddy](https://github.com/r-el-maya)
- [Rishabh Gupta](https://github.com/rish9511)
- [Pankaj Kumar](https://github.com/IpankajI)
- [Rahul Kumar](https://github.com/rkdahmiwal)
- [Giridhari](https://github.com/Giridhari013)
- [Kiran Varghese](https://github.com/kiranvarghese2)
- [Naman Gupta](https://github.com/naman)
- [Francis Lee](https://github.com/francisglee)
- [Saiful B. Khan](https://github.com/saifulbkhan)
- [Avijit Zutshi](https://github.com/avijitzutshi)

## References

To understand [Maven](http://genomics-pubs.princeton.edu/mzroll/index.php) and [El-MAVEN](https://elucidatainc.github.io/ElMaven/) workflows and features, please refer to following literature on Maven:

- [Metabolomic Analysis and Visualization Engine for LC-MSData](http://pubs.acs.org/doi/abs/10.1021/ac1021166), Eugene Melamud, Livia Vastag, and Joshua D. Rabinowitz, Analytical Chemistry 2010 82 (23), 9818-9826
- [LC-MS Data Processing with MAVEN: A Metabolomic Analysis and Visualization Engine](http://onlinelibrary.wiley.com/doi/10.1002/0471250953.bi1411s37/abstract), Clasquin, M. F., Melamud, E. and Rabinowitz, J. D. 2012, Current Protocols in Bioinformatics. 37:14.11.1-14.11.23.

## Acknowledgment

El-MAVEN would not have been possible without the unwavering support, constant feedback and financial support from [Agios](http://www.agios.com/). El-MAVEN thanks the metabolomics community for its immense contribution in taking the tool forward and making it a great success.

## Copyright and license

Code and documentation copyright 2017 [Elucidata Inc](http://www.elucidata.io/). Code released under the [GPL v2.0](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html). Documentation is released under [MIT license](https://opensource.org/licenses/MIT).
