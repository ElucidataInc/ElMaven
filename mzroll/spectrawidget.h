#ifndef SPECTRAWIDGET_H
#define SPECTRAWIDGET_H

#include "stable.h"
#include "mzSample.h"
#include "mainwindow.h"

class SpectraWidget: public QGraphicsView {
Q_OBJECT
public:
	SpectraWidget(MainWindow* mw);
	static vector<mzLink> findLinks(float centerMz, Scan* scan, float ppm,
			int ionizationMode);

public slots:
	void setScan(Scan* s);
	void setScan(Scan* s, float mzmin, float mzmax);
	void setScan(Peak* peak);
	void setScan(mzSample* sample, int scanNum);
	void setMzFocus(float mz);
	void setMzFocus(Peak* peak);
	void showScan(int increment);
	void gotoScan();
	void showNextScan();
	void showLastScan();
	void replot();
	void spectraToClipboard();
	void overlaySpectra(QList<double> mzs, QList<double> intensities);
	void resetZoom();
	void zoomIn();
	void zoomOut();

private:
	MainWindow* mainwindow;
	Scan* _currentScan;
	Scan* _lastScan;

	vector<mzLink> links;
	bool _drawXAxis;
	bool _drawYAxis;
	float _minX;
	float _maxX;
	float _minY;
	float _maxY;
	float _zoomFactor;

	QPointF _mouseStartPos;
	QPointF _mouseEndPos;

	QPointF _focusCoord;
	QPointF _nearestCoord;

	QGraphicsTextItem* _title;
	QGraphicsTextItem* _note;
	QGraphicsTextItem* _vnote;
	QGraphicsLineItem* _arrow;
	QGraphicsLineItem* _varrow;

	vector<QGraphicsItem*> _items;  //items on the plot

	void initPlot();
	void addAxes();
	void findBounds(bool checkX, bool checkY);
	void drawGraph();
	void setCurrentScan(Scan* scan) {
		if (_currentScan) {
			_lastScan = _currentScan;
		}
		_currentScan = scan;
	}
	float toX(float x) {
		return ((x - _minX) / (_maxX - _minX) * scene()->width());
	}
	float toY(float y) {
		return (scene()->height()
				- ((y - _minY) / (_maxY - _minY) * scene()->height()));
	}
	;
	float invX(float x) {
		return (x / scene()->width()) * (_maxX - _minX) + _minX;
	}
	float invY(float y) {
		return -1
				* ((y - scene()->height()) / scene()->height() * (_maxY - _minY)
						+ _minY);
	}

	int findNearestMz(QPointF pos);
	void drawArrow(float mz1, float int1, float mz2, float ints2);

	void setDrawXAxis(bool flag) {
		_drawXAxis = flag;
	}
	void setDrawYAxis(bool flag) {
		_drawYAxis = flag;
	}
	void addLabel(QString text, float x, float y);
	void setTitle(QString);
	void compareScans(Scan*, Scan*);
	void annotateScan();

protected:
	void leaveEvent(QEvent * event);
	void enterEvent(QEvent * event);
	void keyPressEvent(QKeyEvent *event);
	void resizeEvent(QResizeEvent * event);
	void mouseReleaseEvent(QMouseEvent * mouseEvent);
	void mousePressEvent(QMouseEvent * mouseEvent);
	void mouseMoveEvent(QMouseEvent * mouseEvent);
	void mouseDoubleClickEvent(QMouseEvent * event);
	void wheelEvent(QWheelEvent *event);
	void timerEvent(QTimerEvent* event);
	void contextMenuEvent(QContextMenuEvent * event);

};

#endif

