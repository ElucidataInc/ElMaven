#!/bin/sh
DATE=`date +%Y%m%d`

#clean up
rm -rf Maven.app  Maven.dmg

#get the latest source
cd ..
bzr pull lp:mzroll

#update Makefile
#qmake -spec mac-g++ -r
qmake
#make clean
make -j2 

#copy support files
cd bin
cp *.csv *.db *.model Maven.app/Contents/Resources

mkdir Maven.app/Contents/Resources/methods
mkdir Maven.app/Contents/Resources/pathways
mkdir Maven.app/Contents/Resources/scripts
cp methods/* Maven.app/Contents/Resources/methods
cp pathways/* Maven.app/Contents/Resources/pathways
cp scripts/* Maven.app/Contents/Resources/scripts

#fix Qt dynamic library dependancy
macdeployqt Maven.app

#fix third party dependancies
FRAMEWORKS=Maven.app/Contents/Frameworks
NEWPATH=\@executable_path/../Frameworks

chmod +w $FRAMEWORKS/*.dylib

cp /usr/local/lib/libnetcdf.7.dylib $FRAMEWORKS/
install_name_tool -id $NEWPATH/libnetcdf.7.dylib  $FRAMEWORKS/libnetcdf.7.dylib 
install_name_tool -change /usr/local/lib/libsz.2.0.0.dylib   $NEWPATH/libsz.2.0.0.dylib  $FRAMEWORKS/libnetcdf.7.dylib 
install_name_tool -change /usr/local/lib/libhdf5.7.dylib     $NEWPATH/libhdf5.7.dylib    $FRAMEWORKS/libnetcdf.7.dylib 
install_name_tool -change /usr/local/lib/libhdf5_hl.7.dylib  $NEWPATH/libhdf5_hl.7.dylib $FRAMEWORKS/libnetcdf.7.dylib 

cp /usr/local/lib/libsz.2.0.0.dylib $FRAMEWORKS/
install_name_tool -id $NEWPATH/libsz.2.0.0.dylib  $FRAMEWORKS/libsz.2.0.0.dylib 

cp /usr/local/lib/libhdf5.7.dylib $FRAMEWORKS/
install_name_tool -id $NEWPATH/libhdf5.7.dylib  $FRAMEWORKS/libhdf5.7.dylib 
install_name_tool -change /usr/local/lib/libsz.2.0.0.dylib   $NEWPATH/libsz.2.0.0.dylib  $FRAMEWORKS/libhdf5.7.dylib

cp /usr/local/lib/libhdf5_hl.7.dylib $FRAMEWORKS/
install_name_tool -id $NEWPATH/libhdf5_hl.7.dylib  $FRAMEWORKS/libhdf5_hl.7.dylib 
install_name_tool -change /usr/local/lib/libsz.2.0.0.dylib   $NEWPATH/libsz.2.0.0.dylib  $FRAMEWORKS/libhdf5_hl.7.dylib
install_name_tool -change /usr/local/Cellar/hdf5/1.8.8/lib/libhdf5.7.dylib   $NEWPATH/libhdf5.7.dylib  	 $FRAMEWORKS/libhdf5_hl.7.dylib

#list dependancies
otool -L $FRAMEWORKS/libsz.2.0.0.dylib
otool -L $FRAMEWORKS/libnetcdf.7.dylib
otool -L $FRAMEWORKS/libhdf5_hl.7.dylib
otool -L $FRAMEWORKS/libhdf5.7.dylib


echo "Creating dmg";
rm Maven.dmg
macdeployqt Maven.app -dmg

echo "Copying to Desktop..";
cp Maven.dmg  ~/Desktop/Maven_$DATE.dmg

