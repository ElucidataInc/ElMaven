#ifndef ISOTOPEPLOT_H
#define ISOTOPEPLOT_H

#include "stable.h"
#include "mzSample.h"
#include "mainwindow.h"
#include "qcustomplot.h"

class MainWindow;
class PeakGroup;
class QGraphicsItem;
class QGraphicsScene;


class IsotopePlot : public QObject, public QGraphicsItem
{
    Q_OBJECT

#if QT_VERSION >= 0x040600
    Q_INTERFACES( QGraphicsItem )
#endif

public:
    IsotopePlot(QCustomPlot* customPlot);
    ~IsotopePlot();

    void setPeakGroup(PeakGroup* group);
    void setMainWindow(MainWindow* mw){
        _mw=mw;
    }
    QRectF boundingRect() const;
    void clear();
    void showBars();
    void normalizeIsotopicMatrix(MatrixXf &MM);
    void setBelowAbThresholdMatrixEntries(MatrixXf &MM,MainWindow* _mw);
    void setIsotopicPlotStyling();

private Q_SLOTS:
    void showPointToolTip(QMouseEvent *event);
	
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){}
    void contextMenuEvent(QContextMenuEvent * event);


private:
    float _width;
    float _height;
    float _barwidth;
    vector<mzSample*> _samples;
    MainWindow* _mw;
    QVector<QString> labels;
    QCPItemText * mpMouseText;
    QVector<QCPBars *> isotopesType;
    QCPTextElement * title;
    QCPAxisRect * bottomAxisRect;
    QCustomPlot *customPlot;

    PeakGroup* _group;
    vector<PeakGroup*> _isotopes;
    MatrixXf MMDuplicate;
};

#endif
