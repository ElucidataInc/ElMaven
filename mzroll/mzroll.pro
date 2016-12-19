include(../mzroll.pri)
#TEMPLATE = app

CONFIG += qt thread warn_off sql svg console precompile_header

#Faster build + C++11 ++ OpenMP

QMAKE_CXXFLAGS += -Ofast -ffast-math -march=native -std=c++11
QMAKE_CXXFLAGS += -DOMP_PARALLEL
QMAKE_CXXFLAGS += -fopenmp
LIBS += -fopenmp


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
ICON = images/icon.icns

DESTDIR = ../bin

QT += sql network xml svg printsupport
DEFINES += QT_CORE_LIB QT_DLL QT_NETWORK_LIB QT_SQL_LIB QT_THREAD_LIB

INCLUDEPATH +=  /usr/include/qt4/QtXml/ /usr/include/qt4/QtSql
INCLUDEPATH += ../libmaven ../maven ../pugixml/src ../libneural ../zlib/ ../Eigen/ ../libpls ../libcsvparser 

LIBS += -L.  -lmaven -lpugixml -lneural -lcsvparser -lpls                   #64bit

message($$LIBS)

INSTALLS += sources target

FORMS = forms/settingsform.ui  \
        forms/masscalcwidget.ui \
#	forms/ligandwidget.ui \
        forms/adductwidget.ui \
        forms/isotopeswidget.ui \
        forms/peakdetectiondialog.ui \
        forms/comparesamplesdialog.ui \
        forms/trainingdialog.ui \
        forms/alignmentdialog.ui \
        forms/animationcontrol.ui  \
        forms/noteswidget.ui   \
        forms/rconsolewidget.ui \
        forms/clusterdialog.ui \
        forms/spectramatching.ui\
        forms/peptidefragmentation.ui 

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
                   spectralhitstable.h\
                    clusterdialog.h \
                    peptidefragmentation.h \
                    qcustomplot.h
                     


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
 widgets/qprog.cpp \
 projectdockwidget.cpp \
 spectramatching.cpp \
 mzfileio.cpp \
 spectralhitstable.cpp \
 logwidget.cpp \ 
 peptidefragmentation.cpp \
 qcustomplot.cpp

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
