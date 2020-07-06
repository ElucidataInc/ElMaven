<h1 align="center">El-MAVEN</h1>
<p align="center">An intuitive, open-source LC-MS data processing tool</p>
<p align="center">from</p>
<p align="center">
  <a href="http://www.elucidata.io">
	<img src="https://elucidata.io/wp-content/uploads/2020/01/elucidata-logo.svg">
  </a>
</p>

---

[![Build Status](https://travis-ci.org/ElucidataInc/ElMaven.svg?branch=develop)](https://travis-ci.org/ElucidataInc/ElMaven)
[![Build status](https://ci.appveyor.com/api/projects/status/hj92puovbucth7o1/branch/develop?svg=true)](https://ci.appveyor.com/project/ElucidataInc/elmaven/branch/develop)
[![codecov](https://codecov.io/gh/ElucidataInc/ElMaven/branch/develop/graph/badge.svg)](https://codecov.io/gh/ElucidataInc/ElMaven)
[![DOI](https://zenodo.org/badge/70220005.svg)](https://zenodo.org/badge/latestdoi/70220005)

---

## Index
- [Features](#features)
- [Download](#download)
- [Build from source](#build-from-source)
- [Bugs and feature requests](#bugs-and-feature-requests)
- [Documentation](#documentation)
- [Contributing](#contributing)
- [Contributors](#contributors)
- [References](#references)
- [How to cite](#how-to-cite)
- [Acknowledgment](#acknowledgment)
- [Copyright and license](#copyright-and-license)


## Features

[Maven](http://genomics-pubs.princeton.edu/mzroll/index.php) and [El-MAVEN](https://elucidatainc.github.io/ElMaven/) share following features:
* Multi-file chromatographic aligner
* Peak-feature detector
* Isotope calculator

El-MAVEN is robust, faster and with more user friendly features compared to Maven, and includes the following additional capabilities:
* Adduct calculator
* Fragmentation spectra matching
* Peak editor


## Download
El-MAVEN installers are available for Windows (7 and above) and Mac OS (10.12 and above). Users can download the latest release for these platforms from the [official website](https://resources.elucidata.io/elmaven/).


## Build from source
Contributers and developers can build El-MAVEN on Windows, Ubuntu or Mac OS by following the following instructions.
- Setting up a development environment
  - [Windows](#windows)
  - [Ubuntu](#ubuntu)
  - [Mac OS](#mac-os)
- [Getting the source code](#source-code)
- [Build](#build)


### Setting up a development environment (64 bit platforms only)
Each of the three operating systems need to be set-up differently to be able to compile El-MAVEN from its source. Please follow instructions according to your platform.

#### Windows
We use MSYS2 (and mingw64) envoronment for developing El-MAVEN on Windows.
- Download [MSYS2](http://www.msys2.org/) installer and follow the installation instructions provided on their website.
- Download OpenSSL package using https://indy.fulgan.com/SSL/openssl-1.0.2r-x64_86-win64.zip
- Extract the contents of OpenSSL package in `/c/msys64/mingw64/bin`

Note: To verify whether the above installation steps completed successfully make sure you have "libeay32.dll" and "ssleay32.dll" in path `C:\msys64\mingw64\bin\`. You might see a shortcut for the MSYS2 shell on your Desktop or Start menu, unless you chose to not add them during installation.

After completing the above steps, the following commands needs to be executed from the MSYS2 shell. This updates and installs required dependencies to compile El-MAVEN:
```
pacman --force -Sy
pacman --force -Syu
pacman --force -Su
pacman --force -Sy base-devel msys2-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-qt5 mingw64/mingw-w64-x86_64-hdf5 mingw64/mingw-w64-x86_64-netcdf mingw64/mingw-w64-x86_64-boost msys/git mingw-w64-x86_64-curl
```

It can be helpful if the `$PATH` variable is changed permanently to point to our newly installed packages. To do this, you will have to make changes in your `.bashrc` file for MSYS2, which can be done manually or via shell's command line.

**Manually editing `.bashrc`:**
  - Open .bashrc(located in `C:\msys64\home\<USERNAME>\`) in any text editor
  - Add `export PATH=/c/msys64/mingw64/bin/:$PATH` at the end of the file
  - Save and exit the editor
  - From msys2 shell: `source ~/.bashrc`

**Editing `.bashrc` via the MSYS2 command line:**
```
echo export PATH=/c/msys64/usr/bin/:/c/msys64/mingw64/bin/:$PATH > ~/.bashrc
source ~/.bashrc
```

---

#### Ubuntu
The following commands can be executed to update packages, repositories and package lists as well as install dependencies required for all versions of Ubuntu:
```
sudo apt-get update
sudo apt-get install g++
sudo apt-get install libsqlite3-dev libboost-all-dev lcov libnetcdf-dev
```

Please note that, El-MAVEN has not been built on Ubuntu 14.xx or below for some time now and we do intend to support these older platforms. You may or may not be able to compile it successfull if you are running one of these older Ubuntu systems.

**Important:** Execute the command below only if Ubuntu <=16.04 (Required for adding a repository providing Qt >=5.7):
```
sudo add-apt-repository 'deb http://archive.neon.kde.org/user/lts/ xenial main binary-amd64'
```

We can now finally install Qt framework, along with related libraries and plugins:
```
sudo apt-get install qt5-qmake qtbase5-dev qtscript5-dev qtdeclarative5-dev libqt5multimedia5
sudo apt-get install libqt5multimedia5-plugins qtmultimedia5-dev libqt5webkit5-dev
```

---

#### Mac OS
Development on Mac OS requires the installation of Xcode (and the host of tools that it brings). Please install Xcode from the "App Store" before proceeding.

Once Xcode has been installed, use "Terminal" to perform a necessary action and install `xcode-select`:
```
sudo xcodebuild -license accept
xcode-select --install
```

Next, install [Homebrew](https://brew.sh/) and using brew, install dependencies of El-MAVEN.
```
usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
brew install boost
brew install qt
brew install llvm@6
brew install netcdf
```

Find out the path where `LLVM` and `Qt` have been installed by brew. Typically this is usually `/usr/local/opt/llvm@6` and `/usr/local/opt/qt`, respectively.

Configure the `$PATH` variable to allow your build system to find these installed packages and binaries in all future sessions (replace `LLVM_DIR` and `QT_DIR` with the appropriate paths found in the last step):
```
cd ~
touch .bash_profile
echo "export PATH=/QT_DIR/bin:/LLVM_DIR/bin/:$PATH" >> .bash_profile
echo "export LDFLAGS="-L/QT_DIR/lib -L/LLVM_DIR/lib -Wl,-rpath,/LLVM_DIR/lib" >> .bash_profile
echo "export CPPFLAGS+="-I/QT_DIR/include -I/LLVM_DIR/include  -I/LLVM_DIR/c++/v1/" >> .bash_profile
source .bash_profile
```

Additionally, for MacOS, if building in release mode, sentry-native (Crashpad backend) should be compiled and available through the build environment. In a separate location, download the latest release, unzip and compile:
```
curl -O -J -L https://github.com/getsentry/sentry-native/releases/download/0.1.2/sentry-native-0.1.2.zip
unzip sentry-native-0.1.2.zip &> /dev/null
cd sentry-native/gen_macosx
make config=release sentry_crashpad
echo "export SENTRY_MACOSX_BIN='$PWD/bin/Release'" >> ~/.bash_profile
cd -
```

Once this has been done the path to sentry-native binaries and libraries should be exported in the user's enviroment:
```
echo "export PATH='$SENTRY_MACOSX_BIN:$PATH'" >> ~/.bash_profile
echo "export LDFLAGS='$LDFLAGS -L/$SENTRY_MACOSX_BIN'" >> ~/.bash_profile
source ~/.bash_profile
```

While the above set-up will allow compilation and linking in release mode without any errors, the crash reporting itself will not be functional until a unique Sentry DSN value is exported and available as a base64-encoded string _before_ platform config (qmake) step.
```
export SENTRY_DSN=<YOUR_SECRECT_SENTRY_DSN>
export SENTRY_DSN_BASE64=`echo $SENTRY_DSN | base64`
```


### <a name=source-code></a> Getting the source code
Switch to a directory to where you want the code and then clone this repo:
```
cd <path/to/work/>
git clone https://github.com/ElucidataInc/ElMaven.git
```


### Build
Before building, make sure the environment has ben setup correctly and make sure that the correct versions of libraries have been installed by issuing the following commands:
- Qt version should be >= 5.7: `qmake -v`
- Boost should be >= 1.58
  - On Ubuntu : `apt-cache policy libboost-all-dev`
  - On Mac OS: `brew info boost`
  - On Windows: `pacman -Qi mingw64/mingw-w64-x86_64-boost`
  
---

For building the application and peakdetector CLI in release mode (without any tests) execute the following commands:
```
cd ElMaven
qmake CONFIG+=release NOTESTS=yes build.pro
make -j4
```

If, following earlier steps, El-MAVEN was linked to sentry-native on MacOS, then
two additional commands need to be run for crash reporter to work correctly
(this assumes that the `$SENTRY_MACOSX_BIN` env var is defined already):
```
install_name_tool -change @rpath/libsentry_crashpad.dylib $SENTRY_MACOSX_BIN/libsentry_crashpad.dylib bin/El-MAVEN.app/Contents/MacOS/El-MAVEN
cp $SENTRY_MACOSX_BIN/crashpad_handler bin/El-MAVEN.app/Contents/MacOS/
```
---

For compiling El-MAVEN in debug mode (along with tests), execute these commands instead:
```
cd ElMaven
qmake CONFIG+=debug build.pro
make -j4
```

To run the GUI application run one of the following, according to your OS:
- Windows: `bin/El-MAVEN.exe`
- Linux: `bin/El-MAVEN`
- Mac OS: `bin/El-MAVEN.app/Contents/MacOS/El-MAVEN`

To run the peakdetector CLI application run (example shows `-h` for help on available arguments):
- Windows: `bin/peakdetector.exe -h`
- Linux: `bin/peakdetector -h`
- Mac OS: `bin/peakdetector.app/Contents/MacOS/peakdetector -h`

To help with this process, we provide the build script named `./run.sh`, available in in root source directory. It will ask whether you want to build in release or debug mode when executed. If you want to switch from/to release or debug mode, a clean build must be done. Use the `uninstall.sh` script to clean any built objects of artifacts.

To run tests, please execute the `run_tests.sh` script - which assumes that the test executables (`bin/MavenTests` and `bin/test-libmaven`) were compiled and built beforehand.


## Bugs and feature requests
Existing bugs and feature requests can be found on [El-MAVEN github issue page](https://github.com/ElucidataInc/ElMaven/issues). Please make sure that your bug/feature does not already exist in the issues list before you file new bugs or request a feature.


## Documentation
El-MAVEN user documentation can be found on the project's [GitHub wiki](https://github.com/ElucidataInc/ElMaven/wiki) page.


## Contributing
All contributions are welcome. Please go through our [contributing guidelines](https://github.com/ElucidataInc/ElMaven/blob/develop/CONTRIBUTING.md) and [code of conduct](https://github.com/ElucidataInc/ElMaven/blob/develop/CODE_OF_CONDUCT.md). These guidelines include directions for coding standards, filing issues and development guidelines.

All the functional features should be tested before committing the code and creating pull requests. When contributing C++ code, please make sure that it follows the [style guide](https://github.com/ElucidataInc/ElMaven/blob/develop/STYLE_GUIDE.md) for this project.

Any and all contributions/corrections to the documentation itself will also be very helpful (to the project and the community at large).


## Contributors
*	[Maven team at Princeton University](http://genomics-pubs.princeton.edu/mzroll/index.php)
*	[Eugene Melamud](https://www.calicolabs.com/team-member/eugene-melamud/)
*	[Victor Chubukov](https://github.com/chubukov)
*	[George Sabu](https://github.com/GeorgeSabu)
*	[Sahil](https://github.com/sahil21)
*	[Raghav Sehgal](https://github.com/Raghavdata)
*	[Shubhra Agrawal](https://github.com/shubhra-agrawal)
*	[Rishabh Gupta](https://github.com/rish9511)
*	[Saiful B. Khan](https://github.com/saifulbkhan)
*	[Raghuram Reddy](https://github.com/r-maya)
*	[Pankaj Kumar](https://github.com/IpankajI)
*	[… and more](https://github.com/ElucidataInc/ElMaven/graphs/contributors)


## References
To understand [El-MAVEN](https://elucidatainc.github.io/ElMaven/)'s workflows and features, please refer to following literature on El-MAVEN and MAVEN:
* [El-MAVEN: A Fast, Robust, and User-Friendly Mass Spectrometry Data Processing Engine for Metabolomics](https://doi.org/10.1007/978-1-4939-9236-2_19), Shubhra Agrawal, Sahil Kumar, Raghav Sehgal, Sabu George, Rishabh Gupta, Surbhi Poddar, Abhishek Jha and Swetabh Pathak (2019), D'Alessandro A. (eds) High-Throughput Metabolomics. Methods in Molecular Biology, vol 1978.
* [LC‐MS Data Processing with MAVEN: A Metabolomic Analysis and Visualization Engine](https://doi.org/10.1002/0471250953.bi1411s37), Clasquin, M. F., Melamud, E. and Rabinowitz, J. D. 2012, Current Protocols in Bioinformatics. 37:14.11.1-14.11.23.
* [Metabolomic Analysis and Visualization Engine for LC-MS Data](http://pubs.acs.org/doi/abs/10.1021/ac1021166), Eugene Melamud, Livia Vastag, and Joshua D. Rabinowitz, Analytical Chemistry 2010 82 (23), 9818-9826.

## How to Cite
Cite the protocol as:
> Agrawal S. et al. (2019) El-MAVEN: A Fast, Robust, and User-Friendly Mass Spectrometry Data Processing Engine for Metabolomics. In: D'Alessandro A. (eds) High-Throughput Metabolomics. Methods in Molecular Biology, vol 1978. Humana, New York, NY

When citing analyses performed using the program, cite the [DOI](#digital-object-identifier-doi) for the relevant version of El-MAVEN used for the analyses. To ensure that a reader is able to reproduce your analysis, please mention the key algorithms and parameters used to obtain the final results in your research.


## Acknowledgment
El-MAVEN would not have been possible without the unwavering support, constant feedback and financial support from [Agios](http://www.agios.com/). All contributors and supporters are also thankful to the metabolomics community for its immense contribution in taking the tool forward and making it a great success.


## Copyright and license
Code and documentation copyright  2017 [Elucidata Inc](http://www.elucidata.io/). Code released under the [GPL v2.0](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html). Documentation is released under [MIT license](https://opensource.org/licenses/MIT).
