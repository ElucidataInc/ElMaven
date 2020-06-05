#include "plot_axes.h"

Axes::Axes( int type, float min, float max, int nticks) {
    this->_type = type;
    this->_min = min;
    this->_max = max;
    this->_nticks = nticks;
    this->_offset=0;
    this->_margin=0;
    this->_tickLinesFlag=false;
}

QRectF Axes::boundingRect() const
{
    int textmargin = 50;
	if (!scene())return QRectF(0,0,0,0);

    if(_type == 0 ) {
    	return(QRectF(0,scene()->height()-textmargin,scene()->width(),scene()->height()-textmargin));
	} else {
    	return(QRectF(0,0,+textmargin,scene()->height()+textmargin));
	}
}

void Axes::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    paintAxes(painter,
              _type,
              _min,
              _max,
              scene()->width(),
              scene()->height(),
              _margin,
              _offset,
              _nticks,
              _tickLinesFlag);
}

void Axes::paintAxes(QPainter* painter,
                     int type,
                     float min,
                     float max,
                     int width,
                     int height,
                     int margin,
                     int offset,
                     int nticks,
                     bool tickLinesFlag)
{
    QPen pen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(pen);

    float fontSize = 8;
    QFont font("Helvetica", fontSize);
    painter->setFont(font);
    QFontMetrics fm(font);

    if (nticks == 0)
        nticks = 2;
    int x0 = margin;
    int x1 = width - margin;

    int y0 = height - margin;
    int y1 = margin;

    int Y0 = height - offset;
    int X0 = margin + offset;

    float range = (max - min);
    float b = range / nticks;
    float ix = (x1 - x0) / nticks;
    float iy = (y1 - y0) / nticks;

    if (b == 0)
        return;

    if (type == 0) {  // X axes
        painter->drawLine(x0, Y0, x1, Y0);
        for (int i = 1; i < nticks; i++)
            painter->drawLine(x0 + ix * i, Y0 - 5, x0 + ix * i, Y0);
        for (int i = 1; i < nticks; ++i) {
            auto value = QString::number(min + b * i, 'f', 2);
            int pixelsWide = fm.width(value);
            painter->drawText(x0 + ix * i - (pixelsWide / 2), Y0 + 10, value);
        }
    } else if (type == 1) {  // Y axes
        painter->drawLine(X0, y0, X0, y1);
        for (int i = 1; i < nticks; i++)
            painter->drawLine(X0 - 5, y0 + iy * i, X0, y0 + iy * i);

        for (int i = 1; i < nticks; ++i) {
            QString value = QString::number(min + b * i, 'g', 2);
            if (max < 10000)
                value = QString::number(min + b * i, 'f', 0);
            int textWidth = fm.width(value);
            int textHeight = fm.height();
            painter->drawText(X0 + 2,
                              y0 + iy * i - (textHeight / 2),
                              textWidth,
                              textHeight,
                              Qt::AlignVCenter,
                              value);
        }

        if (tickLinesFlag) {
            // horizontal tick lines
            QPen pen(Qt::gray, 0.1, Qt::SolidLine);
            painter->setPen(pen);
            for (int i = 1; i < nticks; i++)
                painter->drawLine(X0 - 5, y0 + iy * i, x1, y0 + iy * i);
        }
    }
}
