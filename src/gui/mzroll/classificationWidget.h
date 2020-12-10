#ifndef CLASSIFICATIONWIDGET_H
#define CLASSIFICATIONWIDGET_H

#include "stable.h"

class TableDockWidget;
class PeakGroup;
class Compound;

class ClassificationWidget : public QGraphicsView
{
    Q_OBJECT
    public:
        ClassificationWidget(TableDockWidget * tabledock);

    public slots:
        /**
         * @brief Manages all the plotting functions.
         */ 
        void showClassification();

    private:
        QDialog* _inferenceVisual;
        QVBoxLayout* _layout;
        QGraphicsScene _scene;
        QGraphicsView* _sceneView;
        TableDockWidget* _tableDockWidget;
        PeakGroup* _group;
        // Maintains the sum for negative shap values.
        float _sumNegativeWeights;
        // Maintails the sum for positive shap values.
        float _sumPositiveWeights;
        // Sum of abs(negative weights) and positive weights.
        float _absoluteTotalWeight;
        // Sum of negative weights and positive weights. 
        float _totalWeight;


        /** 
         * @brief Sets title (name of group) to the scene.   
         */ 
        void setTitle();

        /** 
         * @brief Sets Legend to the scene.   
         */ 
        void showLegend();

        /**
         * @brief Makes arrows for negative shap values. 
         * @param shapValue ShapValue to plot.
         * @param label Feature for the shapValue.
         * @param counter Number of arrow plotted.
         * @param Start position for the arrow.  
         */ 
        int makeArrowForNegatives(float shapValue, 
                                  string label, 
                                  int counter, 
                                  int startPosition);

        /**
         * @brief Makes arrows for positive shap values. 
         * @param shapValue ShapValue to plot.
         * @param label Feature for the shapValue.
         * @param Number of arrow plotted.
         * @param Start position for the arrow.  
         */ 
        int makeArrowForPositives(float shapValue, 
                                  string label, 
                                  int counter, 
                                  int startPosition);


        /**
         * @brief Sets output value for a peakgroup to the
         * scene.
         */ 
        void setOutputValue();

        /**
         * @brief Render message that ghost peaks are not 
         * classified.
         */ 
        void renderGhostGroupMessage();

        /**
         * @brief Calculates the total weight of the attributes. It
         * is required for setting output value for the shap figure. 
         * 
         * This function will be removed once kailash provides the new output
         * file format.
         */ 
        void getAttributesTotalWeight();

        void getAbsoluteTotalWeight();
};

#endif // CLASSIFICATIONWIDGET_H
