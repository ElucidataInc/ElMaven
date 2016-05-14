#ifndef EDGE_H
#define EDGE_H

#include "globals.h"
#include "stable.h"

#include "node.h"
//#include "enzyme_node.h"
//#include "metabolite_node.h"


class Node;
class Reaction;
using namespace std;

/**
 * \class Edge
 *
 * \ingroup mzroll
 *
 * \brief Class for Edge.
 *
 * This class is used for Edge.
 *
 * \author Euigen
 * \author(documentation prepared by naman)
 */

class Edge : public QObject, public QGraphicsItem
{
    Q_OBJECT

#if QT_VERSION >= 0x040600
    Q_INTERFACES( QGraphicsItem )
#endif

public:
    Edge();
    ~Edge();

    Node *sourceNode() const;
    void setSourceNode(Node *node);

    Node *destNode() const;
    void setDestNode(Node *node);

    void adjust();

    enum { Type = UserType + 2 };
    int type() const {
        return Type;
    }
    inline void setNote(QString note)  {
        _note = note;
    }
    inline void setData(void* data)  {
        _data = data;
    }
    inline void setReversable(bool t) {
        _reversable = t;
    }
    inline bool isReversable() {
        return _reversable;
    }
    inline void setArrowSize(qreal a)  {
        _arrowSize = a;
    }
    inline void setColor(QColor c)     {
        _color = c;
    }

    inline float setRateForward(float f) 	   {
        _rateF = f;
    }
    inline float setRateReverse(float f) 	   {
        _rateR = f;
    }
    inline  float getRateForward()	 	   	   {
        return _rateF;
    }
    inline  float getRateReverse()	   		   {
        return _rateR;
    }

    inline void setFlux(float f)  {
        _flux = f;
    }
    inline float getFlux()  {
        return _flux;
    }

    inline void setLastFlux(float f)  {
        _lastflux = f;
    }
    inline float getLastFlux()  {
        return _lastflux;
    }

    void setTimerSpeed ( int x ) {
        _timerSpeed = x;
    }
    int  getTimerSpeed() {
        return _timerSpeed;
    }

    void setTimerMaxSteps( int x ) {
        _timerMaxSteps = x;
    }
    int  getTimerMaxSteps() {
        return _timerMaxSteps;
    }

    void  setTimerStep(int x)	{
        _timerStep = x;
    }
    int   getTimerStep()	{
        return _timerStep;
    }

    float computeArrowSize(float flux);

    inline bool isHighlighted() {
        return _hilighted;
    }
    void setHighlighted(bool x) {
        _hilighted = x;
    }

    inline bool showNote() {
        return _showNote;
    }
    void setShowNote(bool x) {
        _showNote = x;
    }



    Reaction* getReaction()			   {
        return (Reaction*) _data;
    }
    QColor		getColor()    		   {
        return _color;
    }
    qreal getArrowSize() {
        return _arrowSize;
    }

    double 	angle();
    double	length();
    void reverseDirection();
    void startAnimation();
    void killAnimation();


protected:
    QRectF boundingRect() const;
    QPainterPath shape () const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void drawArrow(QPainter* painter, int direction );
    void timerEvent(QTimerEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event );
    void hoverEnterEvent (QGraphicsSceneHoverEvent*);
    void hoverLeaveEvent ( QGraphicsSceneHoverEvent*);


private:
    Node *source, *dest;
    QPointF sourcePoint;
    QPointF destPoint;
    QString _note;
    QColor _color;
    qreal _arrowSize;
    bool _reversable;
    void* _data;
    float _rateF;
    float _rateR;

    bool _hilighted;
    bool _showNote;

    float _flux;
    float _lastflux;

    //for animation
    int _timerId;
    int _timerStep;
    int _timerSpeed;
    int _timerMaxSteps;

    QPainterPath _shape;

    //geometry calculations
    double computeAngle(const QPointF&, const QPointF&);

};

#endif
