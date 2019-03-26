#ifndef PLOT_WIDGET_H
#define PLOT_WIDGET_H

#include <qnamespace.h>

#include "stable.h"

class EIC;
class EICLogic;
class Note;
class BoxPlot;
class BarPlot;
class PeakGroup;
class MainWindow;
class MassCutoff;
class mzSlice;
class Compound;
class Peak;
class Scan;
class mzSlice;

class EicWidget: public QGraphicsView {
Q_OBJECT

public:
	EicWidget(QWidget *p);
	~EicWidget();

	EICLogic* getParameters() {
		return eicParameters;
	}
	QString eicToTextBuffer(); //TODO: Sahil Added while merging eicwidget
	void addPeakPositions();
	void setBarplotPosition(PeakGroup* group);

public Q_SLOTS:
	void setMzSlice(float mz1, float mz2 = 0.0);
	void setMassCutoff(MassCutoff *massCutoff);
	void resetZoom();
	void zoom(float factor);
	void setMzSlice(const mzSlice& slice, bool replotEic=true);

	void setRtWindow(float rtmin, float rtmax);
	void setSrmId(string srmId);
	void setPeakGroup(PeakGroup* group);

	/**
	 * @brief Updates EIC widget for the selected compound.
	 * @details Sets appropriate slice in the EIC widget and focusLine for
	 * expected retention time, if the `replot` flag is true.
	 * @param c Selected compound object
	 * @param replot A boolean flag to indicate whether EIC should be
	 * replotted to show the slice for given compound.
	 **/
	void setCompound(Compound* c, bool replotEic=true);

	void setSelectedGroup(PeakGroup* group);
	PeakGroup* getSelectedGroup();
	void addEICLines(bool showSpline, bool showEIC);
    void addCubicSpline(); //TODO: Sahil Added while merging eicWidget
	void addBaseLine();
    void addBaseLine(EIC*);
	void addTicLine();
	void addMergedEIC();
	void setFocusLine(float rt);
	void drawSelectionLine(float rtmin, float rtmax);
	void addFocusLine(PeakGroup*);
	void addBarPlot(PeakGroup*);
	void addBoxPlot(PeakGroup*);
	void addFitLine(PeakGroup*);
    void addMS2Events(float mzmin, float mzmax);
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
	void setGalleryToEics();

	void selectGroupNearRt(float rt);
	void eicToClipboard();

	void showEIC(bool f) {
		_showEIC = f;
	}
	void showSpline(bool f) {
		_showSpline = f;
	}
	void showCubicSpline(bool f) {
		_showCubicSpline = f;
	}
	void showPeaks(bool f) {
		_showPeaks = f;
	}
	void showTicLine(bool f) {
		_showTicLine = f;
	}
    void showBicLine(bool f) { //TODO: Sahil Added while mergin eicWidget
		_showBicLine=f;
	}
	void showBaseLine(bool f) {
		_showBaseline = f;
	}
	void showNotes(bool f) {
		_showNotes = f;
	}
    void showMergedEIC(bool f) { 
		_showMergedEIC=f;
	}
    void showEICLines(bool f) { 
		_showEICLines=f;
	}
    void automaticPeakGrouping(bool f) { 
		_groupPeaks=f;
	}
    void showMS2Events(bool f) {
		_showMS2Events = f;
	}

	void startAreaIntegration() {
		toggleAreaIntegration(true);
	}
	void startSpectralAveraging() {
		toggleSpectraAveraging(true);
	}
	void toggleAreaIntegration(bool f) {
		_areaIntegration = f;
		f ? setCursor(Qt::SizeHorCursor) : setCursor(Qt::ArrowCursor);
	}
	void toggleSpectraAveraging(bool f) {
		_spectraAveraging = f;
		f ? setCursor(Qt::SizeHorCursor) : setCursor(Qt::ArrowCursor);
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
    void unSetPeakTableGroup(PeakGroup*);
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

Q_SIGNALS:
	void viewSet(float, float, float, float);
    void scanChanged(Scan*); //TODO: Sahil Added while mergin eicWidget
	void peakMarkedEicWidget();
    void eicUpdated();

private:
	EICLogic* eicParameters;
	float _focusLineRt;					// 0

	float _minX;						//plot bounds
	float _minY;
	float _maxX;
	float _maxY;
	float _zoomFactor;					//scaling of zoom in

	float ymin;
	float ymax;
	bool zoomFlag;
	QPointF _lastClickPos;
	QPointF _mouseStartPos;
	QPointF _mouseEndPos;

	BarPlot* _barplot;
	BoxPlot* _boxplot;
	Note* _statusText;

	bool _showEIC;
	bool _showSpline;
	bool _showCubicSpline;
	bool _showBaseline;
	bool _showTicLine;
    bool _showBicLine; //TODO: Sahil Added while mergin eicWidget
	bool _showMergedEIC;
	bool _showNotes;
	bool _showPeaks;
    bool _showEICLines; //TODO: Sahil Added while mergin eicWidget
	bool _autoZoom;
    bool _groupPeaks; //TODO: Sahil Added while mergin eicWidget
    bool _showMS2Events;

	bool _areaIntegration;
	bool _spectraAveraging;

	bool _showBarPlot;
	bool _showBoxPlot;

	bool _frozen;
	int _freezeTime;
	int _timerId;

	//gui related
	QWidget *parent;
	QGraphicsLineItem* _focusLine;
	QGraphicsLineItem* _selectionLine;

	void showPeak(float freq, float amplitude);
	void groupPeaks();
	void computeEICs();
	void cleanup();		//deallocate eics, fragments, peaks, peakgroups
	void clearPlot();	//removes non permenent graphics objects
	void findPlotBounds(); //find _minX, _maxX...etc
	mzSlice visibleSamplesBounds();

	float toX(float x);
	float toY(float y);
	float invX(float x);
	float invY(float y);

	MainWindow* getMainWindow();		//return parent
	void zoomPeakGroup(PeakGroup* group);

	//function to add and remove notes
	void getNotes(float mzmin, float mzmax);

};

#endif
