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
    /**
     * details- This class is used to display isotopic bar plot for a group.
     *  To use this class create one instance of this class.
     * => customPlot is field that will be used to draw all isotopic bars
     * => widht and height are window width and height
     * => stackingZValue is stacking parameters of different graphr, refer Qt-doc
     * => abundanceThresold is abundanc thresold for isotopes to display
     * 
     * After creating this object, set this object to scene that is holding this isotope-plot,(scene()->addItem(This Obejct))
     * Now specify a group by setPeakGroup method to be show
     * now call show() method on this object that is inherited from QGraphicsItem
     */
    IsotopePlot(QWidget* parent, float width, float height, float stackingZValue, 
                vector<mzSample*> samples, float abundanceThresold, PeakGroup::QType qtype);
    ~IsotopePlot();

    void setPeakGroup(PeakGroup* group);
    QRectF boundingRect() const;
    void clear();
    void showBars();
    void normalizeIsotopicMatrix(MatrixXf &MM);
    MatrixXf getIsotopicMatrix(PeakGroup* group);
    void setBelowAbThresholdMatrixEntries(MatrixXf &MM);
    void setIsotopicPlotStyling();

private Q_SLOTS:
    void showPointToolTip(QMouseEvent *event);
	
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){}


private:
    float _width;
    float _height;
    float _barwidth;
    float _abundanceThresold;
    vector<mzSample*> _samples;
    QVector<QString> labels;
    QCPItemText * mpMouseText;
    QVector<QCPBars *> isotopesType;
    QCPTextElement * title;
    QCPAxisRect * bottomAxisRect;
    QCustomPlot *customPlot;

    PeakGroup::QType _qtype;
    PeakGroup* _group;
    vector<PeakGroup*> _isotopes;
    MatrixXf MMDuplicate;
};

#endif
