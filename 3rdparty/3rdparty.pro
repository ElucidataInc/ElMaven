TEMPLATE = subdirs
CONFIG += ordered qt thread

SUBDIRS += obiwarp libneural libcdfread  libcsvparser pugixml/src  libpillow libpls Logger ErrorHandling google-breakpad qt-google-analytics

macx {

  SUBDIRS -= libcdfread google-breakpad

}
