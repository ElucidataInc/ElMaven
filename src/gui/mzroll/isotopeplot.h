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

class IsotopeBar : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
#if QT_VERSION >= 0x040600
    Q_INTERFACES( QGraphicsItem )
#endif

	public:
	IsotopeBar(QGraphicsItem *parent, QGraphicsScene *scene):QGraphicsRectItem(parent){
			setFlag(ItemIsSelectable);
			setFlag(ItemIsFocusable);
			setAcceptHoverEvents(true);
	}

        QRectF boundingRect() {
              return QGraphicsRectItem::boundingRect();
        }

	Q_SIGNALS:
		void groupSelected(PeakGroup* g);
		void groupUpdated(PeakGroup*  g);
		void showInfo(QString,int xpos=0, int ypos=0);
		void showMiniEICPlot(PeakGroup*g);

	protected:        
                void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
                    QGraphicsRectItem::paint(painter,option,widget);
                }

	void hoverEnterEvent (QGraphicsSceneHoverEvent*event);
//	void mouseDoubleClickEvent (QGraphicsSceneMouseEvent*event);
//	void mousePressEvent (QGraphicsSceneMouseEvent*event);
	void keyPressEvent(QKeyEvent *e);
};


class IsotopePlot : public QObject, public QGraphicsItem
{
    Q_OBJECT

#if QT_VERSION >= 0x040600
    Q_INTERFACES( QGraphicsItem )
#endif

public:
    IsotopePlot();
    ~IsotopePlot();

    void setPeakGroup(PeakGroup* group);
    void setMainWindow(MainWindow* mw);
    QRectF boundingRect() const;
    void clear();
    void showBars();
    void normalizeIsotopicMatrix(MatrixXf &MM);
    void setBelowAbThresholdMatrixEntries(MatrixXf &MM,MainWindow* _mw);

private Q_SLOTS:
    void showPointToolTip(QMouseEvent *event);
	
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
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

    PeakGroup* _group;
    vector<PeakGroup*> _isotopes;
    MatrixXf MMDuplicate;
};

#endif
