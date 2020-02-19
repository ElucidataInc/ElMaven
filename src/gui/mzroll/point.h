#ifndef EICPOINT_H
#define EICPOINT_H

#include "stable.h"

class Peak;
class PeakGroup;
class MainWindow;
class Scan;
class SpectraWidget;

class EicPoint : public QObject, public QGraphicsItem {
    Q_OBJECT
    Q_INTERFACES( QGraphicsItem )

public:
    enum POINTSHAPE { CIRCLE, SQUARE, TRIANGLE_UP, TRIANGLE_DOWN };
    EicPoint(QObject *parent = 0);
    EicPoint(float x, float y, Peak* peak, MainWindow* mw);
        ~EicPoint();
    void setColor(QColor &c)  { _color = c; _pen.setColor(c); _brush.setColor(c); }
    void setPen(QPen &p)  { _pen = p;  }
    void setBrush(QBrush &b)  { _brush = b; }
    void setPeakGroup(PeakGroup* g) { _group = g; }
    void setPeak(Peak* p) { _peak=p; }
    void setScan(Scan* x) { _scan=x; }
    Peak* getPeak() { return _peak; }
    PeakGroup* getPeakGroup() { return _group; }
    void setPointShape(POINTSHAPE shape) { pointShape=shape; }
    void forceFillColor(bool flag) { _forceFill = flag; }
    void setSize(float size) { _cSize=size; }
    void removeFromScene();

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
    Scan* _scan;
    Peak* _peak;
    PeakGroup* _group;
    MainWindow* _mw;
    QColor _color;
    QPen _pen;
    QBrush _brush;       
    POINTSHAPE pointShape;
    bool _forceFill;
    float _cSize;

    static void _updateWidgetsForPeakGroup(MainWindow* mw,
                                           PeakGroup* group,
                                           Peak* peak);
    static void _updateWidgetsForScan(MainWindow* mw, Scan* scan);

private Q_SLOTS:
	void bookmark();
	void linkCompound();
	void reorderSamples();
	void setClipboardToGroup();
        void setClipboardToIsotopes();
Q_SIGNALS:
    void peakSelected(Peak*);
    void peakGroupSelected(PeakGroup*);
    void spectaFocused(Peak*);
    void peakGroupFocus(PeakGroup*);
    void ms2MarkerSelected(Scan*);
};

#endif
