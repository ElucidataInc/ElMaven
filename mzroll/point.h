#ifndef EICPOINT_H
#define EICPOINT_H

#include "stable.h"
#include "globals.h"
#include "mainwindow.h"
#include "note.h"

class Peak;
class PeakGroup;
class MainWindow;
class SpectraWidget;

class EicPoint : public QObject, public QGraphicsItem {
    Q_OBJECT
   
#if QT_VERSION >= 0x040600
    Q_INTERFACES( QGraphicsItem )
#endif


public:
    EicPoint(QObject *parent = 0);
    EicPoint(float x, float y, Peak* peak, MainWindow* mw);
        ~EicPoint();
    void setColor(QColor &c)  { _color = c; }
    void setPen(QPen &p)  { _pen = p;  }
    void setBrush(QBrush &b)  { _brush = b; }
    void setPeakGroup(PeakGroup* g) { _group = g; } 
    Peak* getPeak() { return _peak; }
    PeakGroup* getPeakGroup() { return _group; }

protected:
    QRectF boundingRect() const;
 	void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
	void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
    void mousePressEvent( QGraphicsSceneMouseEvent * event);
	void mouseDoubleClickEvent (QGraphicsSceneMouseEvent* event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	void contextMenuEvent ( QGraphicsSceneMouseEvent * event );
	void keyPressEvent(QKeyEvent *event);
    
private:
    float _x;
    float _y;
    Peak* _peak;
    PeakGroup* _group;
    MainWindow* _mw;
    QColor _color;
    QPen _pen;
    QBrush _brush;       

private slots:
	void bookmark();
	void linkCompound();
	void reorderSamples();
	void setClipboardToGroup();
        void setClipboardToIsotopes();
signals:
    void peakSelected(Peak*);
    void peakGroupSelected(PeakGroup*);
    void spectaFocused(Peak*);
    void peakGroupFocus(PeakGroup*);


};

#endif
