#ifndef PATHWAYWIDGET_H
#define PATHWAYWIDGET_H

#include "stable.h"
#include "graphwidget.h"

class AnimationControl;
class Compound;
class MetaboliteNode;
class Node;
class EnzymeNode;
class MainWindow;
class BackgroundPeakUpdate;
class BarPlot;
class TinyPlot;
class Reaction;
class mzSample;

struct Rpair {
    QString reaction_id;
    QString rpair_id;
    QString species1;
    QString species2;
};

class PathwayWidget : public GraphWidget
{
    Q_OBJECT

public:
    PathwayWidget(MainWindow* parent);
    ~PathwayWidget();
    void itemMoved();

    int  getTimerSpeed() { return _timerSpeed; }
    int  getTimerMaxSteps() { return _timerMaxSteps; }
    double  getTimerStep()	{ return _timerStep; }

public:
    MainWindow* getMainWindow() { return mw; }

public Q_SLOTS:
    void setCompound(Compound* c);			//clear graph, and expond this compound
    void setCompoundFocus(Compound* c);		//keep graph, and  show information about compound
    void setCompoundSelected(Compound* c);	//keep graph add nodes from from this compound
    void expandOnCompound(Compound* c);

    void startAtomTransformationAnimation(Compound* c, int atomNumber );
    void showAtomTrasformations();
    void clearSelectedAtoms();

    bool setPathway(QString pathway_id);
    void clear();
    void hideEmpty();
    void checkCompoundExistance();
    void updateCompoundConcentrations();

    void updateConcentrations();
    void recalculateConcentrations();
    void setAnimationControl(AnimationControl* a) { animationControl=a; }

    void setTimerSpeed ( int x ) { _timerSpeed = x; }
    void setTimerMaxSteps( int x ) { _timerMaxSteps = x; }
    void setTimerStep( float x)	{   _timerStep=x; }

    void setEncodeVideo(bool f) { _encodeVideo=f; }

    bool saveLayout();
    void loadModelFile();
    void saveModelFile();
    void exportPDF();
    void loadModelFile(QString filename);
    void saveModelFile(QString filename);
    int  loadSpreadsheet(QString filename);

    void showAtomCoordinates(bool flag) { _showAtomCoordinatesFlag=flag; }
    void showCofactors(bool flag);
    void showEnzymes(bool flag);
    void calculateIsotopes(bool flag);
    void showSample(int);
    void showTinyPlot(Node*);
    void normalizeNodeSizes(int x);
    void changeLayoutAlgorithm(int x);

    void setTitle(QString s) { GraphWidget::setTitle(s); }
    void setTitle(Reaction*);

    void setupAnimationMatrix();
    void setupPairWiseComparisonMatrix();
    void showAnimationStep(float fraction);
    void startSimulation();
    void resetSimulation();
    void stopSimulation();

    void loadBackgroundImage();

Q_SIGNALS:
    void compoundFocused(Compound*);
    void compoundSelected(Compound*);
    void compoundHover(Compound*);
    void reactionFocused(Reaction*);


protected: 
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);

    void dropEvent (QDropEvent * event );
    void keyPressEvent(QKeyEvent *event);
    void timerEvent(QTimerEvent *event);
    void wheelEvent(QWheelEvent *event);
    bool loadLayout(QString pathway_id);

    void layoutCofactors();
    void contextMenuEvent(QContextMenuEvent * event);

    EnzymeNode* addEnzyme(QString id, Reaction* r);
    MetaboliteNode* addMetabolite(QString id, Compound* c);
    EnzymeNode*  addReaction(Reaction* r);
    void getReactionPairs();

    void setReactions(vector<string>& reactionIds);
    void addReactions(vector<string>& reactionsIds);

    void showConnectedNodes(Node* n, int depth);
    bool cofactorCheck(const QString id);
    void cleanTempVideoDir();
    void encodeVideo();


    AnimationControl* 	  animationControl;
    BackgroundPeakUpdate* workerThread;
    TinyPlot*		  	  tinyPlot;


private:

    MainWindow* mw;			
    vector<mzSample*>samples;		//list of samples

    //atom transformation animation
    QMap<Compound*,int> animationQ;
    vector<string> getCompoundReactions(Compound* c, int depth=0);
    QVector<QPoint> getEqualentAtoms(QString rpairId);
    void showAtomTrasformations(Compound* c, int atomNumber);
    vector<Rpair*>rpairs;
    bool _showAtomCoordinatesFlag;

    QSet<QString> cofactors;		//names of compouns that are cofactors
    QList<Node*> vnodes; 			//visible nodes

    Compound* _focusedCompound;		//currently focused compound
    QString _pathwayId;				//currently dispalayed pathways

    bool _showCofactorsFlag;
    bool _showEnzymesFlag;
    bool _encodeVideo;
    bool _forceUpdate;

    MatrixXf ConcMatrix;		//concentrations of metabolties
    MatrixXf LabelingMatrix;	//fraction labaled

    QString _backgroundImageFile;

    //animation control
    int _timerId;
    int _timerMaxSteps;
    int _timerSpeed;
    double _timerStep;


};


#endif
