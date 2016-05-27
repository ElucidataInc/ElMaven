#ifndef ENZYMENODE_H
#define ENZYMENODE_H

#include "stable.h"
#include "mzSample.h"
#include "node.h"
#include "graphwidget.h"

class Node;
class Edge;
class GraphWidget;

/**
 * \class EnzymeNode
 *
 * \ingroup mzroll
 *
 * \brief Class for EnzymeNode.
 *
 * This class is used for EnzymeNode.
 *
 * \author Euigen
 * \author(documentation prepared by naman)
 */

class EnzymeNode: public Node {
Q_OBJECT

public:
	EnzymeNode(QGraphicsItem* parent, QGraphicsScene *scene);
	void setReaction(Reaction* r) {
		setDataReference(r);
	}
	Reaction* getReaction() {
		return (Reaction*) getDataReference();
	}

	void setInitConcentration(float x);
	float computeFlux();
	float getMinRate();
	float getFreeConcentration();
	float computeComplexFormation();
	void setFreeConcentration(float x) {
		_freeConcentration = x;
	}
	void setComplexConcentration(float x) {
		_complexConcentration = x;
	}
	void layoutCofactors();
	float summary();
	QPointF activeSitePosition();

	void setFluxes(QList<float> v) {
		_fluxes = v;
	}
	float getFlux(int step) {
		if (step < _fluxes.size())
			return _fluxes[step];
		return 0;
	}

	void setFlux(float value) {
		_flux = value;
	}
	float getFlux() {
		return _flux;
	}

protected:
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
	void mousePressEvent(QGraphicsSceneMouseEvent* event);
	void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
	void contextMenuEvent(QGraphicsSceneContextMenuEvent * event);
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
			QWidget *widget);

signals:
	void enzymeFocused(Reaction*);

private:
	void paintEnzyme(QPainter*);
	void paintCofactors(QPainter*);
	float _freeConcentration;
	float _complexConcentration;

	float _flux;
	QList<float> _fluxes;
};

#endif
