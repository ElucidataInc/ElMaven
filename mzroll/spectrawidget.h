#ifndef SPECTRAWIDGET_H
#define SPECTRAWIDGET_H

#include "spectralhit.h"
#include "stable.h"
#include "mzSample.h"
#include "mainwindow.h"
#include "../libmaven/eiclogic.h"

class SpectraWidget : public QGraphicsView
{
    Q_OBJECT
public:
    SpectraWidget(MainWindow* mw);
    static vector<mzLink> findLinks(float centerMz, Scan* scan, pair<massAccType,double> pr, int ionizationMode);

        public Q_SLOTS:
                    void setScan(Scan* s);
                    void setScan(Scan* s, float mzmin, float mzmax );
                    void setScan(Peak* peak);
                    void setScan(mzSample* sample, int scanNum);
                    void setMzFocus(float mz);
                    void setMzFocus(Peak* peak);
                    void setScanSet(vector<Scan*> set) { _scanset=set; } //TODO: Sahil, Added while merging spectrawidget
                    void incrementScan(int direction, int msLevel);
                    void gotoScan();
                    void showNextScan();
                    void showLastScan();
                    void showNextFullScan();
                    void showLastFullScan();
                    void replot();
                    void spectraToClipboard();
                    void spectraToClipboardTop();
                    void overlayPeptideFragmentation(QString proteinSeq,float productAmuTolr); //TODO: Sahil, Added while merging point
                    void overlayCompoundFragmentation(Compound* c); //TODO: Sahil, Added while merging spectrawidget
                    void showConsensusSpectra(PeakGroup* group); //TODO: Sahil, Added while merging spectrawidget
                    void overlaySpectralHit(SpectralHit& hit);
                    void drawSpectralHit(SpectralHit& hit); //TODO: Sahil, Added while merging spectrawidget
                    void resetZoom();
                    void zoomIn();
                    void zoomOut();
                    void setProfileMode() { _profileMode=true;  }
                    void setCentroidedMode() { _profileMode=false; }
                    void setCurrentScan(Scan* scan);
                    void constructAverageScan(float rtmin, float rtmax);
                    void findSimilarScans();
                    Scan* getCurrentScan() { return _currentScan; }
                    void copyImageToClipboard(); //TODO: Sahil, Added while merging spectrawidget
                    void assignCharges(); //TODO: Sahil, Added while merging spectrawidget
 
                    void drawScanSet(vector<Scan*>& scanset); //TODO: Sahil, Added while merging spectrawidget
                    void drawScan(Scan* scan, QColor sampleColor, int offsetXpx, int offsetYpx); //TODO: Sahil, Added while merging spectrawidget
                    void drawMzLabels(Scan *scan); //TODO: Sahil, Added while merging spectrawidget
                    void drawAnnotations(); //TODO: Sahil, Added while merging spectrawidget



        private:
                    EICLogic* eicparameters;
                    MainWindow* mainwindow;
                    Scan* _currentScan;
                    Scan* _avgScan;
                    vector<Scan*>_scanset;

                    vector<mzLink> links;
                    bool  _drawXAxis;
                    bool  _drawYAxis;
                    bool  _resetZoomFlag;
                    bool  _profileMode;
                    float _minX;
                    float _maxX;
                    float _minY;
                    float _maxY;
                    float _zoomFactor;
                    QString _titleText;

                    SpectralHit  _spectralHit;

                    QPointF _mouseStartPos;
                    QPointF _mouseEndPos;

                    QPointF _focusCoord;
                    QPointF _nearestCoord;

                    QGraphicsTextItem* _title;
                    QGraphicsTextItem* _note;
                    QGraphicsTextItem* _vnote;
                    QGraphicsLineItem* _arrow;
                    QGraphicsLineItem* _varrow;

                    vector<QGraphicsItem*> _items;  //graphic items on the plot
                    vector<int> chargeStates;
                    vector<int> peakClusters;


                    void initPlot();
                    void addAxes();
                    void findBounds(bool checkX, bool checkY);
                    void drawGraph();

                    float toX(float x)  { return( (x-_minX)/(_maxX-_minX) * scene()->width()); }
                    float toY(float y)  { return( scene()->height()- ((y-_minY)/(_maxY-_minY) * scene()->height())); } //TODO: Sahil, Added while merging spectrawidget
                    float invX(float x) { return(  x/scene()->width())  * (_maxX-_minX) + _minX; }
                    float invY(float y) { return  -1*((y-scene()->height())/scene()->height() * (_maxY-_minY) + _minY); }

                    int findNearestMz(QPointF pos);
                    void drawArrow(float mz1, float int1, float mz2, float ints2);

                    void setDrawXAxis(bool flag) { _drawXAxis = flag; }
                    void setDrawYAxis(bool flag) { _drawYAxis = flag; }
                    void addLabel(QString text, float x, float y);
                    void setTitle(Scan*); //TODO: Sahil, Added while merging spectrawidget
                    void setTitle(QString);
                    void compareScans(Scan*, Scan*);
                    void annotateScan();
                    void clearGraph(); //TODO: Sahil, Added while merging spectrawidget

		protected:
                    void leaveEvent ( QEvent * event );
                    void enterEvent(QEvent * event);
                    void keyPressEvent(QKeyEvent *event);
                    void resizeEvent ( QResizeEvent * event );
                    void mouseReleaseEvent(QMouseEvent * mouseEvent);
                    void mousePressEvent(QMouseEvent * mouseEvent);
                    void mouseMoveEvent(QMouseEvent * mouseEvent);
                    void mouseDoubleClickEvent ( QMouseEvent * event );
                    void wheelEvent(QWheelEvent *event);
                    void timerEvent(QTimerEvent* event);
                    void contextMenuEvent(QContextMenuEvent * event);



                };

#endif

