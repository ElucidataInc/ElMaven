#include "plot_axes.h"

Axes::Axes( int type, float min, float max, int nticks) {
    this->type = type;
    this->min = min;
    this->max = max;
    this->nticks = nticks;
	this->offset=0;
	this->margin=0;
	this->tickLinesFlag=false;
}

QRectF Axes::boundingRect() const
{   

    int textmargin=50;
	if (!scene())return QRectF(0,0,0,0);

	if(type == 0 ) {
    	return(QRectF(0,scene()->height()-textmargin,scene()->width(),scene()->height()-textmargin));
	} else {
    	return(QRectF(0,0,+textmargin,scene()->height()+textmargin));
	}
}

void Axes::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{ 
	QPen pen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	painter->setPen(pen);

    float fontsize = 8;
	QFont font("Helvetica",8);
	painter->setFont(font);

    if (nticks == 0 ) nticks = 2;
    int x0 = margin;
    int x1 = scene()->width()-margin;

    int y0 = scene()->height()-margin;
    int y1 = margin;

	int Y0=scene()->height()-offset;
	int X0=margin+offset;

    float range = (max-min);
    float expbase = pow(10,floor( log(range/nticks)/log(10) ) );
    float b;
    for(int i=0; i<10; i+=1) { b=i*expbase; if (b>range/nticks) break; }

    float ticks = range/b;
    float ix = (x1-x0)/ticks;
    float iy = (y1-y0)/ticks;

    if ( b == 0 ) return;

    if ( type == 0) { 	//X axes
        painter->drawLine(x0,Y0,x1,Y0);
        for (int i=0; i <= ticks; i++ ) painter->drawLine(x0+ix*i,Y0-5,x0+ix*i,Y0+5);
        for (int i=0; i <= ticks; i++ ) painter->drawText(x0+ix*i,Y0+10,QString::number(min+b*i,'f',2));
    } else if ( type == 1 ) { //Y axes
        painter->drawLine(X0,y0,X0,y1);
		for (int i=0; i <= ticks; i++ ) {
			painter->drawLine(X0-5,y0+iy*i,X0+5,y0+iy*i);
		}

        for (int i=0; i <= ticks; i++ ) { 
				QString value = QString::number(min+b*i,'g',2);
				if ( max < 10000 ) { value = QString::number(min+b*i,'f',1); }
				painter->drawText(X0+2,y0+iy*i,value);
		}

		if(tickLinesFlag) {
			//horizontal tick lines
			QPen pen(Qt::gray, 0.1,Qt::DotLine);
			painter->setPen(pen);
			for (int i=0; i <= ticks; i++ ) painter->drawLine(X0-5,y0+iy*i,x1,y0+iy*i);
		}
    }
	//painter->drawText(posX-10,posY-3,QString( eics[i]->sampleName.c_str() ));
}

