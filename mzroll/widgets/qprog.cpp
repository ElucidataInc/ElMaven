/***************************************************************************
 *   Copyright (C) 2008 - Giuseppe Cigala                                  *
 *   g_cigala@virgilio.it                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "qprog.h"

QProg::QProg(QGraphicsItem *parent, QGraphicsScene *scene) : QGraphicsItem(parent,scene)
{
    initValue();

}

void QProg::initValue()
{
    minVal = 0;
    maxVal = 100;
    actVal = 0;
    fontDim = 20;
    precision = 0;
    lengthBar = 0;
    colBar = Qt::gray;
}



void QProg::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget *) 
{
    paintBorder(painter);
    paintBar(painter);
    paintLine(painter);
    paintValue(painter);
}


void QProg::paintBorder(QPainter* painter)
{

    QLinearGradient linGrad(5, 40, 15, 40);
    linGrad.setColorAt(0, Qt::white);
    linGrad.setColorAt(1, Qt::gray);
    linGrad.setSpread(QGradient::PadSpread);
    painter->setBrush(linGrad);
    QRectF border(5, 5, 460, 70);
    painter->drawRoundRect(border, 3);

    // value rect
    painter->setBrush(QColor(70, 70, 70));
    QRectF value(385, 10, 75, 60);
    painter->drawRoundRect(value, 15);

}

void QProg::paintBar(QPainter* painter)
{
    painter->setBrush(QColor(70, 70, 70));
    //background color
    QRectF back(20, 10, 360, 60);
    painter->drawRoundRect(back, 3);
    // chech value
    if (actVal > maxVal)
        return;
    if (actVal < minVal)
        return;
    // waiting state if min = max
    if(minVal == maxVal)
    {
    painter->setBrush(colBar);
    QRectF bar(40, 10, 40, 60);
    QRectF bar1(130, 10, 40, 60);
    QRectF bar2(220, 10, 40, 60);
    QRectF bar3(310, 10, 40, 60);
    painter->drawRoundRect(bar, 3);
    painter->drawRoundRect(bar1, 3);
    painter->drawRoundRect(bar2, 3);
    painter->drawRoundRect(bar3, 3);
    return;
	}	
   	// check positive or negative scale
    if (maxVal >= 0 && minVal >= 0 || maxVal >= 0 && minVal <= 0)
        lengthBar = 360-360 * (maxVal-actVal)/(maxVal-minVal);
    if (maxVal <= 0 && minVal <= 0)
        lengthBar = 360 * (minVal-actVal)/(minVal-maxVal);
    // length and color bar
    painter->setBrush(colBar);
    QRectF bar(20, 10, lengthBar, 60);
    painter->drawRoundRect(bar, 3);

}

void QProg::paintLine(QPainter* painter)
{
    painter->setPen(QPen(Qt::gray, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    for (int i = 0; i <= 40; i++)
    {
        painter->drawLine(20+(360/40*i), 10, 20+(360/40*i), 70);
    }
}

void QProg::paintValue(QPainter* painter)
{
    QRectF value(385, 10, 75, 60);
    QFont font("Arial", fontDim, QFont::Normal);
    painter->setFont(font);
    painter->setPen(QPen(Qt::white));
    double x = 100;
    double y = 360;
    QString val = QString("%1").arg(lengthBar*(x/y), 0, 'f', precision);
    val.append("%");
    painter->drawText(value, Qt::AlignCenter, val);

}


void QProg::setValue(double value)
{
    if (value > maxVal)
    {
    	actVal = maxVal;
    	update();
    	return;    	
   	}
    if (value < minVal)
    {
    	actVal = minVal;
    	update();
    	return;    	
   	}
    actVal = value;
    update();
}

void QProg::setMinValue(double min)
{
    if (min > maxVal)
    {
    	minVal = maxVal;
    	maxVal = min;
    	actVal = minVal;
    	update();
    	return;
   	}
    minVal = min;
    update();
}

void QProg::setMaxValue(double max)
{
    if (max < minVal)
    {
    	maxVal = minVal;
    	minVal = max;
    	actVal = minVal;
    	update();
    	return;
   	}
    maxVal = max;
    update();
}

void QProg::setFontDim(int font)
{
    fontDim = font;
    update();
}

void QProg::setPrecision(int numPrec)
{
    precision = numPrec;
    update();
}

void QProg::setBarColor(QColor color)
{
    colBar = color;
    update();
}


QRectF QProg::boundingRect() const
{
    return QRectF(0,0,470,80);
}



