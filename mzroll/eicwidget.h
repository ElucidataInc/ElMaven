#ifndef PLOT_WIDGET_H
#define PLOT_WIDGET_H

#include "stable.h"
#include "mainwindow.h"
#include "line.h"
#include "barplot.h"
#include "boxplot.h"
#include "isotopeplot.h"
#include "point.h"
#include "plot_axes.h"

class PeakGroup;
class EIC;
class MainWindow;
class BarPlot;
class BoxPlot;
class IsotopePlot;
class Note;

/**
 * \class EicWidget
 *
 * \ingroup mzroll
 *
 * \brief Class for EicWidget.
 *
 * This class is used for EicWidget.
 *
 * \author Euigen
 * \author(documentation prepared by naman)
 */

class EicWidget: public QGraphicsView {
Q_OBJECT

public:
	EicWidget(QWidget *p);
	~EicWidget();

	vector<EIC*> getEICs() {
		return eics;
	}
	vector<PeakGroup>& getPeakGroups() {
		return peakgroups;
	}
	PeakGroup* getSelectedGroup() {
		return selectedGroup;
	}
	mzSlice& getMzSlice() {
		return _slice;
	}

public slots:
	void setMzSlice(float mz);
	void setPPM(double ppm);
	void resetZoom();
	void zoom(float factor);
	void setMzRtWindow(float mzmin, float mzmax, float rtmin, float rtmax);
	void setMzSlice(const mzSlice& slice);

	void setRtWindow(float rtmin, float rtmax);
	void setSrmId(string srmId);
	void setPeakGroup(PeakGroup* group);
	void setCompound(Compound* c);
	void setSelectedGroup(PeakGroup* group);
	void addEICLines(bool showSpline);
	void addBaseLine();
	void addTicLine();
	void addMergedEIC();
	void setFocusLine(float rt);
	void addFocusLine(PeakGroup*);
	void addBarPlot(PeakGroup*);
	void addBoxPlot(PeakGroup*);
	void addIsotopicPlot(PeakGroup*);
	void addFitLine(PeakGroup*);
	void integrateRegion(float rtmin, float rtmax);
	void recompute();
	void replot(PeakGroup*);
	void replot();
	void replotForced();
	void print(QPaintDevice* printer);
	void showPeakArea(Peak*);
	void addNote();
	void addNote(Peak* p);
	void addNote(float x, float y, QString text);
	void updateNote(Note*);
	void saveRetentionTime();
	void align();

	void selectGroupNearRt(float rt);
	void addPeakGroup(PeakGroup& group);
	void eicToClipboard();

	void showSpline(bool f) {
		_showSpline = f;
	}
	void showPeaks(bool f) {
		_showPeaks = f;
	}
	void showTicLine(bool f) {
		_showTicLine = f;
	}
	void showBaseLine(bool f) {
		_showBaseline = f;
	}
	void showNotes(bool f) {
		_showNotes = f;
	}

	void showIsotopePlot(bool f) {
		_showIsotopePlot = f;
	}
	void showBarPlot(bool f) {
		_showBarPlot = f;
	}
	void showBoxPlot(bool f) {
		_showBoxPlot = f;
	}

	void setStatusText(QString text);
	void autoZoom(bool f) {
		_autoZoom = f;
	}

	void markGroupGood();
	void markGroupBad();
	void copyToClipboard();
	void selectionChangedAction();
	void freezeView(bool freeze);
protected:
	void moved(QMouseEvent *event);
	void selected(const QRect&);
	void wheelEvent(QWheelEvent *event);
	//void scaleView(qreal scaleFactor);
	void mouseReleaseEvent(QMouseEvent * mouseEvent);
	void mousePressEvent(QMouseEvent * mouseEvent);
	void mouseMoveEvent(QMouseEvent * mouseEvent);
	void mouseDoubleClickEvent(QMouseEvent * event);
	void resizeEvent(QResizeEvent *) {
		replot(NULL);
	}
	void contextMenuEvent(QContextMenuEvent * event);
	void keyPressEvent(QKeyEvent *e);
	void timerEvent(QTimerEvent * event);

	void setupColors();
	void setTitle();
	void setScan(Scan*);
	void addAxes();
	void showAllPeaks();
	void addPeakPositions(PeakGroup* group);
	void createNotesTable();

signals:
	void viewSet(float, float, float, float);

private:

	mzSlice _slice;                     // current slice
	float _focusLineRt;                 // 0

	vector<EIC*> eics;               // vectors mass slices one from each sample
	deque<EIC*> tics;                 // vectors total chromatogram intensities;

	float _minX;                        //plot bounds
	float _minY;
	float _maxX;
	float _maxY;
	float _zoomFactor;                  //scaling of zoom in

	QPointF _lastClickPos;
	QPointF _mouseStartPos;
	QPointF _mouseEndPos;

	BarPlot* _barplot;
	BoxPlot* _boxplot;
	IsotopePlot* _isotopeplot;
	Note* _statusText;

	bool _showSpline;
	bool _showBaseline;
	bool _showTicLine;
	bool _showMergedEIC;
	bool _showNotes;
	bool _showPeaks;
	bool _autoZoom;

	bool _showIsotopePlot;
	bool _showBarPlot;
	bool _showBoxPlot;

	bool _frozen;
	int _freezeTime;
	int _timerId;

	vector<PeakGroup> peakgroups;       //peaks grouped across samples
	PeakGroup* selectedGroup;           //currently selected peak group
	PeakGroup _integratedGroup;        //manually integrated peak group

	//gui related
	QWidget *parent;
	QGraphicsLineItem* _focusLine;

	void showPeak(float freq, float amplitude);
	void groupPeaks();
	void computeEICs();
	void cleanup();     //deallocate eics, fragments, peaks, peakgroups
	void clearPlot();   //removes non permenent graphics objects
	void findPlotBounds(); //find _minX, _maxX...etc
	mzSlice visibleSamplesBounds(); //find absoulte max and min for visible samples
	mzSlice visibleEICBounds(); //find absolute min and max for extracted ion chromatograms

	float toX(float x);
	float toY(float y);
	float invX(float x);
	float invY(float y);

	MainWindow* getMainWindow();        //return parent
	void zoomPeakGroup(PeakGroup* group);

	//function to add and remove notes
	void getNotes(float mzmin, float mzmax);

};

#endif
