#ifndef PLOT_AXES_H
#define PLOT_AXES_H

#include "stable.h"


class Axes : public QGraphicsItem
{
public:
    Axes(QGraphicsItem* parent) : QGraphicsItem(parent) {}
    Axes( int type, float min, float max, int nticks);
    QRectF boundingRect() const;
    void setRange(double a, double b) { _min = a; _max = b; }
    double getMin() {return _min;}
    double getMax() {return _max;}
    void setNumTicks(int x)  { _nticks = x; }
    void setMargin(int m) { _margin = m; }
    void setOffset(int o ) { _offset = o;  }
    void showTickLines(bool f) { _tickLinesFlag = f; }

    static void paintAxes(QPainter* painter,
                          int type,
                          float min,
                          float max,
                          int width,
                          int height,
                          int margin,
                          int offset,
                          int nticks,
                          bool tickLinesFlag);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

private:
   int _type;
   float _min;
   float _max;

   int _nticks;
   int _margin;
   int _offset;
   bool _tickLinesFlag;
};

#endif
