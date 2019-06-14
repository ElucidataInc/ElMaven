#ifndef METABLITENODE_H
#define METABLITENODE_H

#include "node.h"
#include "stable.h"

class BarPlot;
class Compound;
class Molecule2D;

class MetaboliteNode: public Node
{
    Q_OBJECT


public:
    MetaboliteNode(QGraphicsItem* parent, QGraphicsScene *scene);

public Q_SLOTS:
    void setCompound(Compound* c) { setDataReference(c); }
    Compound* getCompound() { return (Compound*) getDataReference(); }

    bool isLinked()			  { return _isLinked; }
    void setLinked(bool v)	  { _isLinked = v; }
    float getTextWidth();
    int getSelectedAtom() { return _selectedAtom; }
    void setSelectedAtom(int atomNumber) { _selectedAtom=atomNumber; }
    void showCoordinates(bool flag) { _showCoordinates=flag; }
    void setMolecularCoordinates(Molecule2D* mol) { coordinates=mol; }
    bool showBarPlot(bool flag) { _showBarPlot=flag; }
    void addBarPlot();

Q_SIGNALS:
    void compoundFocused(Compound*);
    void expandOnCompound(Compound*);
    void compoundHover(Compound*);
    void atomSelected(Compound*, int);

protected:

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent( QGraphicsSceneMouseEvent * event );
    void hoverEnterEvent(QGraphicsSceneHoverEvent*);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent*);

    void dropEvent ( QGraphicsSceneDragDropEvent * event );
    void dragEnterEvent ( QGraphicsSceneDragDropEvent * event );
    void dragLeaveEvent ( QGraphicsSceneDragDropEvent * event );

    void contextMenuEvent(QGraphicsSceneContextMenuEvent * event );
    void keyPressEvent(QKeyEvent *e);


private Q_SLOTS:
    void editGroup();
    void expandOnCompound();

private:
    void paintCofactor(QPainter* painter);
    void paintMetabolite(QPainter*);
    void paintCarbonBalls(QPainter*);

    bool _isLinked;
    bool _showBarPlot;
    bool _showCoordinates;
    float _defaultAtomSize;
    Molecule2D*  coordinates;
    BarPlot*     _barplot;
    QVector<QPointF>atomMap;
    int _selectedAtom;

};

#endif
