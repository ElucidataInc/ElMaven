#ifndef BOXPLOT_H
#define BOXPLOT_H

#include "stable.h"
#include "mzSample.h"
#include "mainwindow.h"


class MainWindow;
class PeakGroup;
class QGraphicsItem;
class QGraphicsScene;

class BoxPlot : public QObject, public QGraphicsItem
{
    Q_OBJECT
   
#if QT_VERSION >= 0x040600
    Q_INTERFACES( QGraphicsItem )
#endif

	static PeakGroup::QType qtype; 

public:
	BoxPlot(QGraphicsItem *parent, QGraphicsScene *scene);
	~BoxPlot();
	void setPeakGroup(PeakGroup *group);
    QRectF boundingRect() const;
	void setMainWindow(MainWindow* mw) { _mw = mw; }
	void switchQValue();
	void clear();
	
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void mousePressEvent (QGraphicsSceneMouseEvent*) { switchQValue(); update(); }
	
private:
	QVector<QString> _labels;
	QVector<QColor>  _colors; 
	QVector<float>   _yvalues;
	int _width;
	int _height;
	float _barwidth;

    MainWindow* _mw;
};

#endif
