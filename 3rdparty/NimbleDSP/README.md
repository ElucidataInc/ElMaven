NimbleDSP
========

## About
NimbleDSP is a C++ digital signal processing (DSP) library that aims to be fast, comprehensive, and most importantly- really easy to use.

## Features
* Vector classes for real, complex, floating point, and fixed point data types.
* Filter classes
* Matlab-style syntax (e.g. "spectrum_dB = log10(abs(fft(data)));")
* Efficient multi-rate filter functions (decimate, interp, and resample)
* FFT and inverse FFT
* 100% template classes and functions
* Doxygen comments/documentation for all methods and functions.
* Comprehensive unit tests.
* Non-restrictive license (MIT) that allows you to keep your software proprietary if you want.

## Installation
None required!  Well, mostly none.  All of the source code is header files so all you have to do is "include" it in your code and make sure that their directory is in your include path.  The library's header files are all in the "src" directory.

You will also need a header file from KissFFT.  I have forked KissFFT to fix a bug that causes the KissFFT template code to fail to compile with non-gcc compilers.  Download it by executing "git clone https://github.com/JimClay/KissFFT-Fork.git kiss_fft130".  You can put the KissFFT code where you want (though if you wish to build and run the unit tests you should put it at the same level as NimbleDSP- see the Unit Tests section) as long as you put its top-level directory in your include path.

### Unit Tests
That being said, you may want to get the unit tests up and running.  If so, do the following steps:

* Download my fork of Mark Borgerding's excellent KissFFT by executing "git clone https://github.com/JimClay/KissFFT-Fork.git kiss_fft130" at the same level as NimbleDSP (i.e. not inside the NimbleDSP directory).
* Download googletest by executing the following at the same level as NimbleDSP and KissFFT: "svn checkout http://googletest.googlecode.com/svn/trunk/ googletest-read-only".  If you don't have a Subversion client installed and really don't want to install one you can probably download the source as a zip file [here](https://code.google.com/p/googletest/downloads/list), unzip it and rename the directory to "googletest-read-only", but I haven't tried it.
* Linux platform
    * cd to NimbleDSP/build
    * run "cmake .."
    * run "make"
    * If all went well then a NimbleDSPTests executable was created.  Run it to execute the unit tests.
* Mac platform
    * cd to NimbleDSP/build
    * run "cmake .. -G Xcode"
    * Start Xcode and open the NimbleDSP/build/NimbleDSP.xcodeproj file.
    * Build the project.
    * Prepare to run the unit tests inside of Xcode by switching the scheme from "ALL_BUILD" to "NimbleDSPTests->My Mac (64 bit)".
    * Run the program.
* Windows platform
    * Run the Cmake Gui.
    * Set the directory to run out of to the NimbleDSP directory.
    * Set the directory to put the build files in to NimbleDSP/build.
    * Push the "Configure" button.  It will ask you what compiler it should use.  You will have to have a modern C++ compiler installed, like Visual Studio 2013.  I have done this with VC++ 2013 but not with any others.  It will probably work with any compiler that supports C++11, but I don't guarantee it- especially since Microsoft's standards support has often been spotty.
    * Push the "Generate" button.
    * Start Visual Studio (or whatever compiler you configured Cmake for), go to the build directory and open the NimbleDSP.sln file.
    * Try to build the "solution".  It will probably produce some errors that talk about RuntimeLibrary mismatches.
    * In the Solution Explorer window right click on kissfft and click on "Properties" in the menu that appears.
    *  Go to Configuration Properties -> C/C++ -> Code Generation.  In that window there should be a parameter called "Runtime Library" that is set to "Multi-threaded Debug DLL (/MDd)".  Change it to "Multi-threaded Debug (/MTd)".
    *  Follow the same process to change the runtime library for NimbleDSPTests.
    *  Build the solution.
    *  Open up a command line window in the NimbleDSP/build/Debug directory and run NimbleDSPTests.exe.

## Documentation
To create the code documentation do the following:

* Run the Doxygen GUI.
* Tell it to operate in the NimbleDSP directory.
* Open the config file "doxygen.cfg".
* Tell Doxygen to run.  It will put the HTML documentation in the doc directory.

Feel free to modify the config file to alter the HTML documentation to your tastes/needs, output LaTex and PDF, etc.

## License
NimbleDSP is offered under the MIT license.  KissFFT is available under the BSD license.  Both are very similar.  They basically let you do whatever you want short of suing the authors (having said that, I am not a lawyer, so don't take this README as legal advice).  For the fine print read the license files.

## Author
Jim Clay

<a href="https://scan.coverity.com/projects/3375">
  <img alt="Coverity Scan Build Status"
       src="https://scan.coverity.com/projects/3375/badge.svg"/>
</a>