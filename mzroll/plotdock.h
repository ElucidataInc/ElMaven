#ifndef  PLOT_DOCK_WIDGET
#define  PLOT_DOCK_WIDGET

#include "stable.h"
#include "globals.h"
#include "mzSample.h"
#include "note.h"

class TableDockWidget;

class PlotScene : public QGraphicsScene
{
    Q_OBJECT

	public:
	PlotScene(QObject * parent = 0);
	~PlotScene();

	void setPlotRect(QRectF r) { plotRect = r; }
    QRectF getPlotRect() { return plotRect; }
	QPointF plotToMap(float x,float y);
	QPointF mapToPlot(float sceneX, float sceneY);

    QPointF getXDim() { return xDim; }
    QPointF getYDim() { return yDim; }
    QPointF getZoomXDim() { return zoomXDim; }
    QPointF getZoomYDim() { return zoomYDim; }

    void setXDim(float min, float max) { xDim = QPointF(min,max);  }
    void setYDim(float min, float max) { yDim = QPointF(min,max);  }
    void setZoomXDim(float min, float max) { zoomXDim = QPointF(min,max);  }
    void setZoomYDim(float min, float max) { zoomYDim = QPointF(min,max);  }

    void resetZoom() { zoomXDim = xDim; zoomYDim = yDim; }
    void setLogTransformed( bool x, bool y) { logX=x; logY=y;}

    bool logTrasformedX() { return logX; }
    bool logTrasformedY() { return logY; }

	void showVLine( bool flag) { if(vline) { addItem(vline); vline->setVisible(flag);  }}
	void showHLine( bool flag) { if(hline) { addItem(hline); hline->setVisible(flag);  }}
	void showXLabel( QString text ); 
	void showYLabel( QString text );
	// New Funtion - Kiran
    void setLogBase( double x ) { logBase=x; }


	void clear();
	void clearData() { data.clear(); }

	//graphics items
	//QGraphicsEllipseItem* addPoint(float x, float y, float radius, QColor color);

	protected:
	void mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent );
	void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );
	void mouseMoveEvent  ( QGraphicsSceneMouseEvent * mouseEvent );

    Q_SIGNALS:
	void zoomArea(QPointF from, QPointF to );
	void selectArea(QPointF from, QPointF to );

	private:
	QPointF down;
	QPointF up;
    QRectF plotRect;

    bool logX;
    bool logY;
	// New Variable for New Funtion - Kiran
    double logBase;

    QPointF xDim;
    QPointF yDim;

    QPointF zoomXDim;
    QPointF zoomYDim;

	QGraphicsRectItem *selectionRect;
	QGraphicsLineItem* vline;
	QGraphicsLineItem* hline;
	QGraphicsTextItem*  ylabel;
	QGraphicsTextItem*  xlabel;
	Note* xValueLabel;
	Note* yValueLabel;

	bool _mousePressed;
	bool _mouseReleased;

	QVector< QVector<QPointF> > data;

};

class PlotAxes : public QGraphicsItem
{
public:
	PlotAxes(int type, int nticks, PlotScene *scene):QGraphicsItem(0,scene){ 
        this->type = type; this->nticks=nticks; offset=0; tickLinesFlag=false; logTrasformed=false;
    };

    QRectF boundingRect() const;
	void setNumTicks(int x)  { nticks = x; }
	void setOffset(int o ) { offset=o;  }
	void showTicLines(bool f) { tickLinesFlag=f; }
    void setLogTransformed(bool x) { logTrasformed=x; }
	
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    
private:
   int type;
   int nticks;
   int offset;
   bool tickLinesFlag;
   bool logTrasformed;
};


class PlotDockWidget: public QDockWidget {
Q_OBJECT

public:
			PlotDockWidget(QWidget * parent, Qt::WindowFlags flags);
		~PlotDockWidget();

public Q_SLOTS: 
						void replot();
		void selectionChanged();
		void exportPDF();
						void resetZoom();
						void zoomArea(QPointF from, QPointF to);

Q_SIGNALS:
		void groupSelected(PeakGroup*);
		void peakSelected(Peak*);
		void scanSelected(Scan*);

protected:
		virtual void draw();
						virtual void draw(QPointF a, QPointF b);
						void contextMenuEvent(QContextMenuEvent * event);

		void drawAxes();
		void resizeEvent ( QResizeEvent *event );
						// void mousePressEvent(QMouseEvent *event);
		void keyPressEvent(QKeyEvent *event);

		inline PlotScene* scene() { return myScene; }
		inline QGraphicsView* view()   { return myView; }
		void   addToolBar();

						QStack<QRectF> zoomHistory;
		QWidget* mainWidget;
		QGraphicsView *myView;
		PlotScene *myScene;
		QToolBar*	toolBar;
		QMenu*	    contextMenu;

};



#endif

