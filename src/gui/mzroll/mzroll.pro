MOC_DIR=$$top_builddir/tmp/mzroll/
OBJECTS_DIR=$$top_builddir/tmp/mzroll/
include($$mzroll_pri)
DESTDIR = $$top_srcdir/bin/
#TEMPLATE = app

CONFIG += qt thread warn_off sql svg console precompile_header

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

TARGET = El_Maven_$$VERSION
macx:TARGET=El_Maven_$$VERSION

RC_FILE = mzroll.rc
RESOURCES +=  mzroll.qrc
ICON = maven.icns


QT += sql network xml printsupport

linux {
    INCLUDEPATH  += $$top_srcdir/3rdparty/google-breakpad/src/
    QMAKE_LFLAGS += -L$$top_builddir/libs/
    LIBS += -lgoogle-breakpad -pthread
}

win32 {
    INCLUDEPATH  += $$top_srcdir/3rdparty/google-breakpad/src/
    QMAKE_LFLAGS += -L$$top_builddir/libs/
    LIBS += -lgoogle-breakpad -pthread
}

mac {
    QMAKE_CXXFLAGS += -fopenmp
    INCLUDEPATH  += $$top_srcdir/3rdparty/google-breakpad/src/ /usr/local/Cellar/llvm/6.0.1/lib/clang/6.0.1/include/
    QMAKE_LFLAGS += -L$$top_builddir/libs/ -L/usr/local/Cellar/llvm/6.0.1/lib/
    LIBS += /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
    LIBS += /System/Library/Frameworks/CoreServices.framework/Versions/A/CoreServices
    LIBS += -lgoogle-breakpad -lobjc -pthread
    LIBS += -lomp
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
                $$top_srcdir/3rdparty/libpillow \
                $$top_srcdir/3rdparty/libdate/ \
                $$top_srcdir/3rdparty/ErrorHandling \
                $$top_srcdir/3rdparty/Logger \
                $$top_srcdir/src/pollyCLI

QMAKE_LFLAGS += -L$$top_builddir/libs/

# we need this flag  since on windows libpillow is built as a dynamic lib
win32 {

    QMAKE_LFLAGS += -L$$top_srcdir/bin/
}


LIBS +=  -lmaven -lobiwarp -lpugixml -lneural -lcsvparser -lpls -lErrorHandling -lLogger -lcdfread -lnetcdf -lz -lpollyCLI               #64bit

macx {

  LIBS -= -lnetcdf
  LIBS -= -lcdfread
}

message($$LDFLAGS)

INSTALLS += sources target

FORMS = forms/settingsform.ui  \
        forms/masscalcwidget.ui \
#	forms/ligandwidget.ui \
        forms/adductwidget.ui \
        forms/isotopeswidget.ui \
        forms/peakdetectiondialog.ui \
        forms/pollyelmaveninterface.ui \
        forms/comparesamplesdialog.ui \
        forms/trainingdialog.ui \
        forms/alignmentdialog.ui \
        forms/animationcontrol.ui  \
        forms/noteswidget.ui   \
        forms/rconsolewidget.ui \
        forms/clusterdialog.ui \
        forms/loginform.ui \
        forms/aboutpolly.ui \
        forms/spectramatching.ui\
        forms/peptidefragmentation.ui \
    forms/awsbucketcredentialsdialog.ui \
    forms/samplertwidget.ui \
    forms/isotopeplotdockwidget.ui \
    forms/gettingstarted.ui \
    forms/pollywaitdialog.ui \
    forms/peaktabledeletiondialog.ui

HEADERS +=  stable.h \
            globals.h \
            mainwindow.h \
            tinyplot.h \
            node.h \
                    enzyme_node.h \
                    plotdock.h \
                    metabolite_node.h \
                    pathwaywidget.h \
                    graphwidget.h \
                    settingsform.h \
                    database.h \
                    background_peaks_update.h \
                    isotopeplot.h\
                    barplot.h \
                    boxplot.h \
                    edge.h \
                    line.h \
                    point.h \
                    history.h \
                    plot_axes.h \
                    spectrawidget.h\
                    masscalcgui.h \
                    adductwidget.h \
                    isotopeswidget.h \
                    ligandwidget.h \
                    eicwidget.h \
                    peakdetectiondialog.h \
                    pollyelmaveninterface.h \
                    comparesamplesdialog.h \
                    traindialog.h \
                    tabledockwidget.h  \
                    treedockwidget.h  \
                    heatmap.h  \
                    treemap.h  \
                    note.h  \
                    suggest.h \
                    animationcontrol.h \
                    alignmentdialog.h \
                    scatterplot.h \
                    noteswidget.h \
                    gallerywidget.h \
                    QHistogramSlider.h \
                    qdownloader.h \
                    rconsolewidget.h \
                    highlighter.h \
                    widgets/qprog.h \
                    projectdockwidget.h \
                    spectramatching.h \
                    mzfileio.h \
                    logwidget.h \
                    spectralhit.h \
                    grouprtwidget.h \
                    alignmentvizallgroupswidget.h \
                   spectralhitstable.h\
                    clusterdialog.h \
                    loginform.h \
                    aboutpolly.h \
                    peptidefragmentation.h \
                    qcustomplot.h \
                    saveJson.h \
                    messageBoxResize.h \
                    awsbucketcredentialsdialog.h \
                    samplertwidget.h \
                    controller.h \
                    numeric_treewidgetitem.h \
                    analytics.h \
                    ElmavCrashHandler.h \
                    isotopeplotdockwidget.h \
                    gettingstarted.h \
                    pollywaitdialog.h \
                    peaktabledeletiondialog.h \
                    notificator.h \
                    notificator_p.h



SOURCES += mainwindow.cpp  \
database.cpp \
 plotdock.cpp \
 treedockwidget.cpp \
 node.cpp \
 enzyme_node.cpp \
 metabolite_node.cpp \
 tinyplot.cpp \
 graphwidget.cpp \
 pathwaywidget.cpp \
 settingsform.cpp \
 background_peaks_update.cpp \
 isotopeplot.cpp \
 barplot.cpp \
 boxplot.cpp \
 edge.cpp \
 point.cpp \
 history.cpp \
 spectrawidget.cpp \
 masscalcgui.cpp \
 adductwidget.cpp \
 isotopeswidget.cpp \
 ligandwidget.cpp \
 main.cpp \
 eicwidget.cpp \
 plot_axes.cpp \
 tabledockwidget.cpp \
 peakdetectiondialog.cpp \
 pollyelmaveninterface.cpp \
 comparesamplesdialog.cpp \
 traindialog.cpp \
 line.cpp  \
 heatmap.cpp \
 treemap.cpp \
 note.cpp  \
 suggest.cpp \
 animationcontrol.cpp\
 alignmentdialog.cpp\
 scatterplot.cpp \
 noteswidget.cpp \
 gallerywidget.cpp \
 QHistogramSlider.cpp \
 qdownloader.cpp \
 highlighter.cpp \
 rconsolewidget.cpp \
 clusterdialog.cpp \
 loginform.cpp \
 aboutpolly.cpp \
 widgets/qprog.cpp \
 projectdockwidget.cpp \
 spectramatching.cpp \
 mzfileio.cpp \
 spectralhitstable.cpp \
 grouprtwidget.cpp \
 alignmentvizallgroupswidget.cpp \
 logwidget.cpp \
 peptidefragmentation.cpp \
 qcustomplot.cpp \
 saveJson.cpp \
    awsbucketcredentialsdialog.cpp \
    samplertwidget.cpp \
    controller.cpp \
    numeric_treewidgetitem.cpp \
    analytics.cpp \
    ElmavCrashHandler.cpp \
    isotopeplotdockwidget.cpp \
    gettingstarted.cpp \
    pollywaitdialog.cpp \
    peaktabledeletiondialog.cpp \
    notificator.cpp


contains (DEFINES,EMBEDHTTPSERVER) {
    SOURCES += remotespectrahandler.cpp
    HEADERS += remotespectrahandler.h
    INCLUDEPATH += ../libpillow
    LIBS += -lpillowcore
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
