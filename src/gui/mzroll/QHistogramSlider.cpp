#include "QHistogramSlider.h"
QHistogramSlider::QHistogramSlider(QWidget *parent) : QWidget(parent)
{
    initValue();

}

void QHistogramSlider::initValue()
{
    minVal = 0;
    maxVal = 0;

    minBound = 0;
    maxBound = 0;

    fontDim = 20;
    precision = 0;
    lengthBar = 0;
    colBar = QColor(70,70,70);

	setMinimumWidth(100);
	setMinimumHeight(20);

	setMaximumWidth(5000);
	setMaximumHeight(1000);

	QSizePolicy policy(QSizePolicy::Fixed,QSizePolicy::Fixed); 
	policy.setControlType(QSizePolicy::Slider);
	setSizePolicy(policy);

	setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}



void QHistogramSlider::paintEvent(QPaintEvent *)
{
    paintBorder();
	paintValue(Qt::AlignRight,maxBound);
    paintValue(Qt::AlignLeft,minBound);
	paintLine();
    paintBar(Qt::AlignTop,maxBound);
    paintBar(Qt::AlignBottom, minBound);
}


void QHistogramSlider::paintBorder()
{
	
	float W=width();
	float H=height();

    QPainter painter(this);
    painter.setWindow(0, 0, W, H);
    painter.setRenderHint(QPainter::Antialiasing);
    QRectF border(0,0, W, H);
    painter.drawRoundRect(border, 2);

	float hH=H*.5;
    // value rect
    painter.setBrush(QColor(70, 70, 70));
    QRectF maxvalue(W-(2*hH), H/2-hH, 2*hH, 2*hH);
    painter.drawRoundRect(maxvalue, 0);		

    // value rect
    painter.setBrush(QColor(70, 70, 70));
    QRectF minvalue(0, H/2-hH, 2*hH, 2*hH);
    painter.drawRoundRect(minvalue, 0);
}

void QHistogramSlider::paintBar(Qt::AlignmentFlag flag, float boundVal)
{
	
	if (data.size() == 0) return;
	if (maxVal-minVal == 0) return;

	float W=width();
	float H=height();

    QPainter painter(this);
    painter.setWindow(0, 0, W, H);
    painter.setRenderHint(QPainter::Antialiasing);
    // chech value

	float XMIN=H;
	float XMAX=W-H;
	float L = XMAX-XMIN;
	float frac =  1-(maxVal-boundVal)/(maxVal-minVal);
    float xpos = XMIN+frac*L;

	QStyle* style =  QApplication::style();
    QStyleOption option;
	option.initFrom(this);
	
	painter.setPen(Qt::NoPen);
	if(flag == Qt::AlignTop) {
		painter.setBrush(colBar);
		QRectF bar(xpos,0,3,H);
		painter.drawRoundRect(bar, 3);
		QIcon icon = style->standardIcon(QStyle::SP_ArrowDown,&option,0);
		painter.drawPixmap(xpos-H/8,-H/8,icon.pixmap(H/4,H/4));
	}  else {
		painter.setBrush(colBar);
		QRectF bar(xpos,0,3,H);
		painter.drawRoundRect(bar, 3);
		QIcon icon = style->standardIcon(QStyle::SP_ArrowUp,&option,0);
		painter.drawPixmap(xpos-H/8,H-H/8,icon.pixmap(H/4,H/4));

	} 

    //Q_EMIT valueChanged(BadVal);
}

void QHistogramSlider::paintLine()
{
	if (data.size() == 0) return;
	if (maxVal-minVal == 0) return;

	float W=width();
	float H=height();

    QPainter painter(this);
    painter.setWindow(0, 0, W, H);
    painter.setRenderHint(QPainter::Antialiasing);

	float barH = H;
	float L = W-(2*H); L -= L/data.size();
	float barW = L/(data.size());

    painter.setPen(QPen(Qt::white));
    painter.setBrush(QBrush(Qt::blue));

	if (maxY==minY) { minY=0; }

	Q_FOREACH(QPointF p, data) {
		float pos=  H+(p.x()-minVal)/(maxVal-minVal)*L;
		float ht=   barH-barH*(p.y()-minY)/(maxY-minY);
        painter.drawRect(pos,ht,barW,H);
    }
}

void QHistogramSlider::paintValue(Qt::AlignmentFlag flag, float value)
{
	float W=width();
	float H=height();

    QPainter painter(this);
    painter.setWindow(0, 0, W, H);
    painter.setRenderHint(QPainter::Antialiasing);

	QRectF valueBox;
	if (flag == Qt::AlignLeft) {
		valueBox=QRectF(0, 0, H, H);
	} else {
		valueBox=QRectF(W-H, 0, H, H);
	}

    QString val = QString("%1").arg(value, 0, 'f', precision);
	fontDim = H/2;

    QFont font("Arial", fontDim, QFont::Normal);
	float pSize=fontDim;
	for(int i=0; i < 100; i++ ) {
		QFontMetrics fm(font);
		if (fm.width(val)>H*0.8) { pSize*=0.9;  font.setPointSizeF(pSize); } else { break; }
	}

    painter.setFont(font);
    painter.setPen(QPen(Qt::white));
    painter.drawText(valueBox, Qt::AlignCenter, val, &valueBox);

}


void QHistogramSlider::setMaxBound(double value)
{
    maxBound=value;
	if (value < minBound) maxBound=minBound; 
    if (value > maxVal)  maxBound = maxVal;
    if (value < minVal)  maxBound = minVal;
	Q_EMIT(maxBoundChanged(maxBound));
    update();
}

void QHistogramSlider::setMinBound(double value)
{
    minBound=value;
	if (value > maxBound) minBound=maxBound; 
    if (value > maxVal)  minBound = maxVal;
    if (value < minVal)  minBound = minVal;
	Q_EMIT(minBoundChanged(minBound));
    update();
}

void QHistogramSlider::setMinValue(double min)
{
    minVal = min;
    update();
}

void QHistogramSlider::setMaxValue(double max)
{
    maxVal = max;
    update();
}

void QHistogramSlider::setFontDim(int font)
{
    fontDim = font;
    update();
}

void QHistogramSlider::setPrecision(int numPrec)
{
    precision = numPrec;
    update();
}

void QHistogramSlider::setBarColor(QColor color)
{
    colBar = color;
    update();
}

void QHistogramSlider::mouseMoveEvent ( QMouseEvent * event) {
	float W=width();
	float H=height();
	float xcoord=event->pos().x();
	float ycoord=event->pos().y();

	if ( xcoord > H && xcoord < W-H) {
		float L = W-(2*H); 
		float relPos = (xcoord-H+1)/L;
		//qDebug() << relPos << minVal+relPos*(maxVal-minVal);
		//qDebug() << "y=" << ycoord/H;
		if (ycoord/H > 0.7 ) {
			setMinBound(minVal+relPos*(maxVal-minVal));
		} else if (ycoord/H < 0.3 ) {
			setMaxBound(minVal+relPos*(maxVal-minVal));
		}
	}
}

void QHistogramSlider::mousePressEvent ( QMouseEvent * event ) {
	float W=width();
	float H=height();
	float xcoord=event->pos().x();
}

void QHistogramSlider::mouseReleaseEvent ( QMouseEvent * event) {
//	qDebug() << "mouseReleaseEvent() " <<  event->pos();
}

void QHistogramSlider::recalculatePlotBounds() {

	if (data.size() == 0 ) {
		setMinValue(0); setMaxValue(0); setMinBound(0); setMinBound(0);
	}
	
	minVal= maxVal=minBound=maxBound=data[0].x();
	minY = maxY = data[0].y();

	Q_FOREACH(QPointF p, data ) {
		if (p.x() > maxVal) { maxVal=p.x(); maxBound=p.x(); }
		if (p.x() < minVal) { minVal=p.x(); minBound=p.x(); }
		if (p.y() < minY  ) { minY = p.y(); }
		if (p.y() > maxY  ) { maxY = p.y(); }
	}
}


QSize QHistogramSlider::minimumSizeHint() const
{
	//qDebug() << "minimumSizeHint" << width() << " "  << width()/5;
    return QSize(width(),width()/3 );
}

QSize QHistogramSlider::sizeHint() const
{
    return QSize(100, 30);
}

void QHistogramSlider::resizeEvent(QResizeEvent *event) {
	//qDebug() << "resize" << width() << " "  << height();
	setMinimumHeight( width()/5);
}
