<h3 align="center">Elmaven</h3>
<p align="center">An intuitive, opensource LC-MS data processing engine </p>
<p align="center">from </p>
<p align="center">
  <a href="http://www.elucidata.io"> 
	<img src="http://www.elucidata.io/static/images/logo.png">
  </a>

<p align="center">
  <a href="https://travis-ci.org/ElucidataInc/ElMaven"><img alt="Travis Status" 
  src="https://travis-ci.org/ElucidataInc/ElMaven.svg?branch=develop"></a> </p>

## Table of contents
- [Quick start](#download) 
- [ElMaven features](#elmaven-features)
- [Bugs and feature requests](#bugs-and-feature-requests)
- [Documentation](#documentation)
- [Contributing](#contributing)
- [Contributors](#contributors)
- [References](#references)
- [Acknowledgment](#acknowledgment)
- [Copyright and license](#copyright-and-license)

## Download
Elmaven is supported by both 32 and 64 bit architectures on Windows and Ubuntu.
Download [Elmaven](https://elucidatainc.github.io/ElMaven/) latest version or daily build depending on your OS and architecture

## Installation

### Windows 64 bit Installer
- Double click on the installer and click Next  
![](https://dl.dropboxusercontent.com/s/t38t3jm1spc86hp/1.PNG?dl=0)  
- Choose the path where you want to install El-Maven and then click Next  
![](https://dl.dropboxusercontent.com/s/elanm23f0f4pemc/2.PNG?dl=0)  
- Click Next to start Installing. It'll take less than 30 seconds to install  
![](https://dl.dropboxusercontent.com/s/apkiw057kx52kbc/3.PNG?dl=0)  
- Click finish to end the Installation. A shortcut is made on the Desktop. Now you can start using El-Maven by double-clicking the shortcut.  
![](https://dl.dropboxusercontent.com/s/em2d4dtk59k3f61/5.PNG?dl=0)  

### Windows Daily Build
- Download [MSYS2](http://www.msys2.org/) installer and follow the installation instructions provided on their website.
- Open MSYS2 and give the following commands for setting up libraries and toolchains. Reopen MSYS2 when required:  
-- Update pacman with `pacman --force -Sy`
-- Update core system packages with `pacman --force -Syu`
-- Update the rest with `pacman --force -Su`
-- For 62 bit: Install other supporting packages with `pacman --force -Sy base-devel msys2-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-qt5 mingw64/mingw-w64-x86_64-hdf5 mingw64/mingw-w64-x86_64-netcdf mingw64/mingw-w64-x86_64-boost msys/git`
-- For 32 bit: Install other supporting packages with `pacman --force -Sy base-devel msys2-devel mingw-i686-toolchain mingw-i686-qt5 mingw32/mingw-i686-hdf5 mingw32/mingw-i686-netcdf mingw32/mingw-i686-boost msys/git`  
- Move to the folder where MSYS2 was installed and open mingw64.exe and type the following:  
-- Create a folder on Windows and navigate to it using the cd command `cd <pathtofolder>    #for example: cd /c/User/Admin/Desktop`
-- Clone the repository using `git clone https://github.com/ElucidataInc/ElMaven.git`
-- To build Maven, use the command `./run.sh`  
-- If it compiles successfully, it will create an executable in the bin folder. To launch maven by running the executable, use the command `./bin/El_Maven_0.x`  

- Install Elmaven on [Windows](https://elucidatainc.github.io/ElMaven/main/2016/02/12/windows-installation.html) or on [Ubuntu](https://elucidatainc.github.io/ElMaven/main/2016/02/12/ubuntu-installation.html) by following the installation instructions.
- After installing, click on the Elmaven icon to launch Elmaven.
- Elmaven loads with two windows: one for logging the application status and another Elmaven application window for data analysis.

## Elmaven features
[Maven](http://genomics-pubs.princeton.edu/mzroll/index.php) and [Elmaven](https://elucidatainc.github.io/ElMaven/)  share following features:
* Multi-file chromatographic aligner
* Peak-feature detector
* Isotope and adduct calculator
* Formula predictor
* Pathway visualizer
* Isotopic flux animator

Elmaven is robust, faster and with more user friendly features compared to Maven.

## Bugs and feature requests

Existing bugs and feature requests can be found on [El-maven github issue page](https://github.com/ElucidataInc/ElMaven/issues). Please search existing bugs and feature requests before you file a bug or request a feature.

## Documentation
Please refer to Gitwiki for El maven user documentation.

## Contributing
You are welcome to contribute. Please go through Elmaven’s contributing guidelines and for coding guidelines, please contact Elucidata team. These guidelines include directions for coding standards, opening issues and development guidelines.

Pull requests must include relevant unit tests. All the functional features are to be tested before committing the code.
## Contributors
*	[Maven team at Princeton University](http://genomics-pubs.princeton.edu/mzroll/index.php)
*	[Eugene Melamud](https://www.calicolabs.com/team-member/eugene-melamud/)
*	[Victor Chubukov](https://github.com/chubukov)
*	[George Sabu](https://github.com/GeorgeSabu)
*	[Sahil](https://github.com/sahil21)
*	[Rahul Kumar](https://github.com/rkdahmiwal)
*	[Giridhari](https://github.com/Giridhari013)
*	[Kiran Varghese](https://github.com/kiranvarghese2)
*	[Naman Gupta](https://github.com/naman)


## References
To understand [Maven](http://genomics-pubs.princeton.edu/mzroll/index.php) and [Elmaven](https://elucidatainc.github.io/ElMaven/) workflows and features, please refer to following literature on Maven:

* [Metabolomic Analysis and Visualization Engine for LC-MSData](http://pubs.acs.org/doi/abs/10.1021/ac1021166), Eugene Melamud, Livia Vastag, and Joshua D. Rabinowitz, Analytical Chemistry 2010 82 (23), 9818-9826
* [LC-MS Data Processing with MAVEN: A Metabolomic Analysis and Visualization Engine](http://onlinelibrary.wiley.com/doi/10.1002/0471250953.bi1411s37/abstract), Clasquin, M. F., Melamud, E. and Rabinowitz, J. D. 2012, Current Protocols in Bioinformatics. 37:14.11.1-14.11.23.
## Acknowledgment
Elmaven would not have been possible without the unwavering support, constant feedback and financial support from [Agios](http://www.agios.com/). Elmaven thanks metabolomics community for it’s immense contribution in taking the tool forward and a great sucess.

## Copyright and license
Code and documentation copyright  2017 [Elucidata  Inc](http://www.elucidata.io/). Code released under the [GPL v2.0](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html). Documentation is released under [MIT license](https://opensource.org/licenses/MIT).

