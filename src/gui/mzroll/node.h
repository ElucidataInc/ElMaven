#ifndef NODE_H
#define NODE_H

#include "stable.h"

#define POW2(x) ((x)*(x))

class TinyPlot;
class Edge;
class QGraphicsSceneMouseEvent;
class GraphWidget;

class Node : public QObject, public QGraphicsItem
{
    Q_OBJECT
#if QT_VERSION >= 0x040600
    Q_INTERFACES( QGraphicsItem )
#endif

public:
            Node(QGraphicsItem* parent, QGraphicsScene *scene);
    ~Node();
    enum { Type = UserType + 10 };
    int type() const { return Type; };

    enum   MolClass { Unassigned=0, Enzyme=1, Metabolite=2, Cofactor=3};
    inline MolClass molClass()		{ return _class; }
    void   setMolClass(MolClass v)		  { _class = v; }


    void addEdge(Edge *edge);
    void removeEdge(Edge* edge);
    inline QList<Edge *> edges() { return edgeList; }
    QList<Edge *> edgesIn();
    QList<Edge *> edgesOut();
    QList<Edge *> findConnectedEdges(Node* other);


    float calculateMetaboliteConcentrations();

    inline bool isHighlighted() { return _hilighted; }
    void setHighlighted(bool x) { _hilighted = x; }

    void setFontSize(float v)		{ _fontSize = v; }
    float getFontSize();
    void drawLabel();
    void showLabel(bool f) { _showLabelFlag=f; }
	bool labelIsVisible() { return _showLabelFlag; }

    double computeNodeSize(float concentration);

    void setInitConcentration(float v) { _initConcentration = v; }
    inline float getInitConcentration() { return _initConcentration; }

    inline float getConcentration() { return _concentration; }
    void setConcentration(float v) { _lastConcentration=_concentration; _concentration = v; }

    inline float getLabeledConcentration() { return _labeledConcentration; }
    void setLabeledConcentration(float v) { _labeledConcentration = v; }

    inline const QString getNote()		{ return _note; }
    void setNote(QString v)			  { _note = v; }

    inline const QString getId()		{ return _id; }
    void setId(QString v)			  { _id = v; }

    inline void* getDataReference()			{ return _data; }
    void setDataReference(void* v) 			  { _data = v; }
    //void setData( int key, const QVariant & value){ QGraphicsItem:setData(key,value); }

    inline bool  isEnzyme()  	  { return _class == Node::Enzyme; }
    inline bool  isMetabolite()   { return _class == Node::Metabolite; }
    inline bool  isCofactor()   { return _class == Node::Cofactor; }
    inline void  setCofactor(bool v) { v ? _class=Node::Cofactor : _class=Node::Metabolite; }

    void setFixedPosition(bool flag)  { _fixedPosition=flag; }
    inline bool  isFixedPosition()	{ return _fixedPosition; }

    inline void  setScalingFactor(float f) 	 { _scalingFactor = f; }
    inline float getScalingFactor()		 { return _scalingFactor; }

    float setNodeSize(float x) { return _nodeSize; }
    float getNodeSize() { return _nodeSize; }
    QPointF centerPoint()     {return pos(); }

    void setConcentrations( QVector<float>& v ) { concentrations = v; }
    QVector<float> getConcentrations() { return concentrations; }
    int getConcentration(int step )  { if(step < concentrations.size() ) return concentrations[step]; return -1; }

    void setLabeledConcentrations( QVector<float>& v ) { concentrationsLabeled = v; }
    QVector<float> getLabaledConcentrations() { return concentrationsLabeled; }
    int  getLabeledConcentration(int step ) { if(step < concentrationsLabeled.size() ) return concentrationsLabeled[step]; return -1; }

    int getMaxConcentrationSteps() { return concentrations.size(); }

    void setDepth(int d) { _depth=d; }
    int  getDepth() { return _depth; }

    void setBrush(QBrush x) { _brush=x; }

    QRectF boundingRect() const { return _shape.boundingRect(); }
    QPainterPath shape() const  { return _shape; }

    void setGraphWidget(GraphWidget *g);
    GraphWidget* getGraphWidget();

    QRect getTextRect(const QString text, float fontsize);
    float getTextWidth();
    static bool compDepth(Node* n1, Node*n2) { return n1->getDepth() < n2->getDepth(); }

    bool setNewPos(float x, float y);

public Q_SLOTS:
    bool unlinkGroup();

Q_SIGNALS:
    void nodeMoved(Node*);
    void nodePressed(Node*);
    void nodeDoubleClicked(Node*);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void wheelEvent ( QGraphicsSceneWheelEvent * event );
    void paintLabel(QPainter *painter);

    QVector<float>  concentrations;
    QVector<float>  concentrationsLabeled;

    GraphWidget* _graph;
    QPainterPath _shape; //shape of the node

private:
    void paint(QPainter*);

private:
    QList<Edge *> edgeList;
    QString _note;
    QString _id;
    MolClass _class;
    void*  _data;

    QGraphicsTextItem* _label;
    QGraphicsRectItem* _labelBox;
    bool _showLabelFlag;

    QPointF newPos;
    QBrush  _brush;

    bool _hilighted;
    float _fontSize;
    float _scalingFactor;

    float _initConcentration;
    float _concentration;
    float _labeledConcentration;

    float _lastConcentration;
    float _lastLabeledFraction;

    bool _fixedPosition;
    bool _isCofactor;

    int _depth;

    //node size
    float _nodeSize;


};

#endif
