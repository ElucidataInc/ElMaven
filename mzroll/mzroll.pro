include(../mzroll.pri)
#TEMPLATE = app
CONFIG += qt thread warn_off sql svg console precompile_header
#CONFIG += debug qt thread sql svg console precompile_header
QMAKE_STRIP=echo
PRECOMPILED_HEADER  = stable.h

TARGET = maven
RC_FILE = mzroll.rc
RESOURCES +=  mzroll.qrc
ICON = images/icon.icns

DESTDIR = ../bin

QT += sql network xml svg
DEFINES += QT_CORE_LIB QT_DLL QT_NETWORK_LIB QT_SQL_LIB QT_NO_DEBUG QT_THREAD_LIB
INCLUDEPATH +=  /usr/include/qt4/QtXml/ /usr/include/qt/QtSql 
INCLUDEPATH += ../libmaven ../mzorbi ../pugixml/src ../libneural ../Rserve/
LIBS += -L.  -lmzorbi -lpugixml -lneural -lcdfread -lnetcdf
INSTALLS += sources target

#MOC_DIR 	= ../tmp/
#UI_DIR   =  ../tmp/
#OBJECTS_DIR = ../tmp/

FORMS = forms/settingsform.ui  \
		forms/masscalcwidget.ui \
#		forms/ligandwidget.ui \
		forms/adductwidget.ui \
		forms/isotopeswidget.ui \
		forms/peakdetectiondialog.ui \
		forms/comparesamplesdialog.ui \
	    forms/trainingdialog.ui \
	    forms/alignmentdialog.ui \
	    forms/animationcontrol.ui  \
		forms/noteswidget.ui   \
		forms/rconsoledialog.ui \
        forms/spectramatching.ui 

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
 classifier.h \
 classifierNaiveBayes.h \
 classifierNeuralNet.h \
 csvreports.h \
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
# rconsoledialog.h \
#Rserve/Rconnection.h \
#Rserve/Rsrv.h \
#Rserve/sisocks.h
 widgets/qprog.h \ 
    projectdockwidget.h \
    spectramatching.h \
    mzfileio.h


SOURCES += mainwindow.cpp  \
classifier.cpp \
classifierNaiveBayes.cpp \
classifierNeuralNet.cpp \
database.cpp \
csvreports.cpp \
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
# rconsoledialog.cpp \
#Rserve/Rconnection.cc \
 widgets/qprog.cpp \ 
    projectdockwidget.cpp \
    spectramatching.cpp \
    mzfileio.cpp


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

win32:DEFINES += WIN32 
win32:DEFINES += Win32 
macx:DEFINES += unix
