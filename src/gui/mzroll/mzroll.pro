include($$mac_compiler)
include($$top_srcdir/3rdparty/phantomstyle/src/phantom/phantom.pri)

MOC_DIR=$$top_builddir/tmp/mzroll/
OBJECTS_DIR=$$top_builddir/tmp/mzroll/
include($$mzroll_pri)
DESTDIR = $$top_srcdir/bin/
#TEMPLATE = app

QT += concurrent widgets multimedia multimediawidgets network printsupport sql
QT -= qmltest

CONFIG += qt thread warn_off sql svg precompile_header resources_big
win32: CONFIG += windows

#Faster build + C++11 ++ OpenMP

QMAKE_CXXFLAGS +=  -std=c++11
QMAKE_CXXFLAGS += -DOMP_PARALLEL
linux: QMAKE_CXXFLAGS += -Ofast -ffast-math
win32: QMAKE_CXXFLAGS += -Ofast -ffast-math
QMAKE_CXXFLAGS += -fopenmp
!macx: LIBS += -fopenmp

QMAKE_STRIP=echo
PRECOMPILED_HEADER  = stable.h

#add version information during compilation
VERSION=$$system(git describe --tag)
DEFINES += "EL_MAVEN_VERSION=$$VERSION"
DEFINES += "PLATFORM=\"$$QMAKE_HOST.os\""
DEFINES += EMBEDHTTPSERVER

TARGET = "El-MAVEN"

RC_FILE = mzroll.rc
RESOURCES +=  mzroll.qrc
ICON = maven.icns

linux {
    QMAKE_LFLAGS += -L$$top_builddir/libs/
    LIBS += -pthread
}

win32 {
    CONFIG(release, debug|release) {
        INCLUDEPATH  += $$top_srcdir/crashhandler/ \
                        $$top_srcdir/crashhandler/breakpad/src/src
    }
    QMAKE_LFLAGS += -L$$top_builddir/libs/
    LIBS += -pthread
}

mac {
    QMAKE_CXXFLAGS += -fopenmp
    DYLIBPATH = $$system(source ~/.bash_profile ; echo $LDFLAGS)
    isEmpty(DYLIBPATH) {
        warning("LDFLAGS variable is not set. Linking operation might complain about missing OMP library")
        warning("Please follow the README to make sure you have correctly set the LDFLAGS variable")
    }
    QMAKE_LFLAGS += $$DYLIBPATH
    QMAKE_LFLAGS += -L$$top_builddir/libs/
    LIBS += /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
    LIBS += /System/Library/Frameworks/CoreServices.framework/Versions/A/CoreServices
    LIBS += -lobjc -pthread
    LIBS += -lomp

    CONFIG(release, debug|release) {
        LIBS += -lsentry_crashpad
        DEFINES += "SENTRY_DSN_BASE64=$$(SENTRY_DSN_BASE64)"
    }
}

CONFIG(release, debug|release) {
    DEFINES += "UPDATE_REPO_URL_BASE64=$$(UPDATE_REPO_URL_BASE64)"
}

INCLUDEPATH +=  /usr/include/x86_64-linux-gnu/qt5/QtXml/ /usr/include/x86_64-linux-gnu/qt5/QtSql

INCLUDEPATH +=  $$top_srcdir/src/core/libmaven  \
                $$top_srcdir/3rdparty/obiwarp   \
                $$top_srcdir/3rdparty/pugixml/src \
                $$top_srcdir/3rdparty/libneural \
                $$top_srcdir/3rdparty/Eigen/ \
                $$top_srcdir/3rdparty/libpls \
                $$top_srcdir/3rdparty/libcsvparser \
                $$top_srcdir/3rdparty/libcdfread \
                $$top_srcdir/3rdparty/ \
                $$top_srcdir/3rdparty/libdate/ \
                $$top_srcdir/3rdparty/ErrorHandling \
                $$top_srcdir/3rdparty/Logger \
                $$top_srcdir/src/pollyCLI \
                $$top_srcdir/src/projectDB \
                $$top_srcdir/3rdparty/libmgf \
                $$top_srcdir/3rdparty/json \
                $$top_srcdir/src/ 

QMAKE_LFLAGS += -L$$top_builddir/libs/

LIBS +=  -lmaven \
         -lobiwarp \
         -lpugixml \
         -lneural \
         -lcsvparser \
         -lpls \
         -lErrorHandling \
         -lLogger \
         -lcdfread \
         -lnetcdf \
         -lz \
         -lpollyCLI \
         -lprojectDB \
         -lcommon \
         -lmgf 

macx {

  LIBS -= -lnetcdf
  LIBS -= -lcdfread
}

unix {
    QMAKE_LFLAGS += -L/usr/lib/x86_64-linux-gnu/
    LIBS += -lboost_system -lboost_filesystem -lsqlite3
}

win32 {
    LIBS += -lboost_system-mt -lboost_filesystem-mt -lsqlite3
    CONFIG(release, debug|release) {
        LIBS += -lbreakpad
    }
}

INSTALLS += sources target

FORMS = forms/settingsform.ui  \
        forms/isotopeswidget.ui \
        forms/correlationtable.ui \
        forms/librarymanager.ui \
        forms/masscalcwidget.ui \
        forms/peakdetectiondialog.ui \
        forms/pollyelmaveninterface.ui \
        forms/comparesamplesdialog.ui \
        forms/alignmentdialog.ui \
        forms/clusterdialog.ui \
        forms/loginform.ui \
        forms/aboutpolly.ui \
        forms/samplertwidget.ui \
        forms/isotopeplotdockwidget.ui \
        forms/pollywaitdialog.ui \
        forms/peaktabledeletiondialog.ui \
        forms/isotopedialog.ui \
        forms/infodialog.ui \
        forms/adductwidget.ui \
        forms/updatedialog.ui \
        forms/peakeditor.ui \
        forms/groupsettingslog.ui   \
        forms/relabelgroupsdialog.ui

HEADERS += stable.h \
           backgroundopsthread.h \
           globals.h \
           infodialog.h \
           isotopeswidget.h \
           librarymanager.h \
           mainwindow.h \
           tinyplot.h \
           plotdock.h \
           settingsform.h \
           isotopeplot.h\
           barplot.h \
           boxplot.h \
           line.h \
           point.h \
           history.h \
           plot_axes.h \
           spectrawidget.h\
           masscalcgui.h \
           ligandwidget.h \
           eicwidget.h \
           peakdetectiondialog.h \
           pollyelmaveninterface.h \
           comparesamplesdialog.h \
           tabledockwidget.h  \
           treedockwidget.h  \
           suggest.h \
           alignmentdialog.h \
           scatterplot.h \
           gallerywidget.h \
           projectdockwidget.h \
           mzfileio.h \
           spectralhit.h \
           grouprtwidget.h \
           alignmentvizallgroupswidget.h \
           clusterdialog.h \
           loginform.h \
           aboutpolly.h \
           qcustomplot.h \
           saveJson.h \
           messageBoxResize.h \
           samplertwidget.h \
           controller.h \
           numeric_treewidgetitem.h \
           isotopeplotdockwidget.h \
           pollywaitdialog.h \
           peaktabledeletiondialog.h \
           notificator.h \
           notificator_p.h \
#           $$top_srcdir/crasehandler/elmavexceptionhandler.h \
           isotopedialog.h \
           adductwidget.h \
           autoupdater.h \
           updatedialog.h \
           peakeditor.h \
           groupsettingslog.h \
           projectsaveworker.h \
           multiselectcombobox.h \
           correlationtable.h   \
           classificationWidget.h   \
           QHistogramSlider.h       \
           superSlider.h            \
           relabelGroupsDialog.h 

SOURCES += mainwindow.cpp  \
           backgroundopsthread.cpp \
           infodialog.cpp \
           isotopeswidget.cpp \
           librarymanager.cpp \
           plotdock.cpp \
           spectralhit.cpp \
           treedockwidget.cpp \
           tinyplot.cpp \
           settingsform.cpp \
           isotopeplot.cpp \
           barplot.cpp \
           boxplot.cpp \
           point.cpp \
           history.cpp \
           spectrawidget.cpp \
           masscalcgui.cpp \
           ligandwidget.cpp \
           main.cpp \
           eicwidget.cpp \
           plot_axes.cpp \
           tabledockwidget.cpp \
           peakdetectiondialog.cpp \
           pollyelmaveninterface.cpp \
           comparesamplesdialog.cpp \
           line.cpp  \
           suggest.cpp \
           alignmentdialog.cpp\
           scatterplot.cpp \
           gallerywidget.cpp \
           clusterdialog.cpp \
           loginform.cpp \
           aboutpolly.cpp \
           projectdockwidget.cpp \
           mzfileio.cpp \
           grouprtwidget.cpp \
           alignmentvizallgroupswidget.cpp \
           qcustomplot.cpp \
           saveJson.cpp \
           samplertwidget.cpp \
           controller.cpp \
           numeric_treewidgetitem.cpp \
           isotopeplotdockwidget.cpp \
           pollywaitdialog.cpp \
           peaktabledeletiondialog.cpp \
           notificator.cpp \
           isotopedialog.cpp \
           adductwidget.cpp \
           autoupdater.cpp \
           updatedialog.cpp \
           peakeditor.cpp \
           groupsettingslog.cpp \
           projectsaveworker.cpp \
           multiselectcombobox.cpp \
           correlationtable.cpp     \
           classificationWidget.cpp \
           QHistogramSlider.cpp     \
           superSlider.cpp          \
           relabelGroupsDialog.cpp

linux {
    SOURCES -= autoupdater.cpp
    HEADERS -= autoupdater.h
}

sources.files =  $$HEADERS \
  $$RESOURCES \
  $$SOURCES \
  *.doc \
  *.html \
  *.pro \
  images
 sources.path =  ./src
 target.path =  ../bin

build_all {
  !build_pass {
   CONFIG -=    build_all
  }
}
