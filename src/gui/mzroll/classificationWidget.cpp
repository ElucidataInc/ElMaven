#include <cmath>
#include "standardincludes.h"
#include "classificationWidget.h"
#include "tabledockwidget.h"
#include "PeakGroup.h"
#include "Compound.h"
#include "plot_axes.h"
#include "mzUtils.h"

ClassificationWidget::ClassificationWidget(TableDockWidget *tabledock)
{
    _inferenceVisual = new QDialog(tabledock);
    _layout = new QVBoxLayout;
    _tableDock = tabledock;
    _scene.setSceneRect(0, 0, 676, 267);
    _sceneView = new QGraphicsView(&_scene);
    _sceneView->setRenderHints(QPainter::Antialiasing);
    _layout->addWidget(_sceneView);
    _inferenceVisual->setLayout(_layout);
    _inferenceVisual->resize(QSize(900, 400));
    _inferenceVisual->setWindowTitle("Classification inference");

    _group = _tableDock->getSelectedGroup();
    _sumNegativeWeights = 0;
    _sumPositiveWeights = 0;
    _absoluteTotalWeight = 0;
    _totalWeight = 0;
    
    initialiseLabelName();
}

void ClassificationWidget::showClassification()
{
    setTitle();

    auto predictionInference = _group->predictionInference();

    int counter = 0;
    int startPosition = _scene.width() - 500;

    // Only top 3 attributes taken in account.
    // getting total weights.
    for (auto it = predictionInference.begin();
         it != predictionInference.end() && counter <  5;
         ++it)
    {
        _sumNegativeWeights += it->first;
        counter++;
    }

    counter = 0;
    for (auto it = predictionInference.rbegin();
         it != predictionInference.rend() && counter < 5;
         ++it)
    {
        _sumPositiveWeights += it->first;
        counter++;
    }

    _absoluteTotalWeight = abs(_sumNegativeWeights) + _sumPositiveWeights;
    _totalWeight = _sumNegativeWeights + _sumPositiveWeights;
    setOutputValue();

    counter = 0;
    //making left side arrows with negatives
    for (auto it = predictionInference.begin();
         it != predictionInference.end() && counter <  5;
         ++it)
    {
        if (counter == 0){
            startPosition = makeArrowForNegatives(it->first, 
                                                  it->second, 
                                                  counter, 
                                                  startPosition);
        }
        else
            startPosition = makeArrowForNegatives(it->first, 
                                                  it->second, 
                                                  counter, 
                                                  startPosition + 4);

        counter++;
    }

    counter = 0;
    //making right side arrows with positives
    for (auto it = predictionInference.rbegin();
         it != predictionInference.rend() && counter < 5;
         ++it)
    {   
        if(counter == 0){
            startPosition = makeArrowForPositives(it->first, 
                                                  it->second, 
                                                  counter, 
                                                  startPosition);
        }
        else
            startPosition = makeArrowForPositives(it->first, 
                                                  it->second, 
                                                  counter, 
                                                  startPosition + 4);
        counter++;
     }
    _inferenceVisual->exec();
}

int ClassificationWidget::makeArrowForNegatives(float shapValue, 
                                                string label, 
                                                int counter, 
                                                int startPosition)
{
    // Setting pens and brush
    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::red);
    auto color = pen.color();
    double alpha = 0.8 - (shapValue / _sumNegativeWeights);
    color.setAlphaF(alpha);
    QBrush brush;
    brush.setColor(color);
    brush.setStyle(Qt::SolidPattern);
    pen.setColor(color);

    // Setting font for labelling.
    QFont font = QApplication::font();
    int pxSize = _scene.height() * 0.01;
    if (pxSize < 10)
        pxSize = 10;
    font.setPixelSize(pxSize);
    
    // Calculating percentage for shap value. 
    // and width of the arrow.
    shapValue  = 0.5 + shapValue; // to normalize negative value. as (-0.1 > -0.4).
    float percentage = (abs(shapValue)/_absoluteTotalWeight) * 100;
    int x_displacement = percentage * 4; // for width of arrow.

    if (counter != 4) {
        // for proper arrow shape.
        QPolygonF polygon;
        polygon << QPoint(startPosition, 150);
        polygon << QPoint(startPosition + x_displacement, 150);
        polygon << QPoint(startPosition + x_displacement + 20, 170);
        polygon << QPoint(startPosition + x_displacement, 190);
        polygon << QPoint(startPosition , 190);
        polygon << QPoint(startPosition + 20, 170);
        _scene.addPolygon(polygon, pen, brush);

    } else { 
        // for the 5th arrow, straight line in front.
        x_displacement += 20;
        QPolygonF polygon;
        polygon << QPoint(startPosition, 150);
        polygon << QPoint(startPosition + x_displacement, 150);
        polygon << QPoint(startPosition + x_displacement, 190);
        polygon << QPoint(startPosition , 190);
        polygon << QPoint(startPosition + 20, 170);
        _scene.addPolygon(polygon, pen, brush);
    }

    //Labelings
    QPen Linepen;
    Linepen.setWidth(2);
    Linepen.setColor(color);
    auto line = _scene.addLine(QLineF(startPosition + x_displacement/2 ,190,
                                      startPosition + x_displacement/2 - 150,
                                      220),
                               Linepen);
    line = _scene.addLine(QLineF(startPosition + x_displacement/2 - 150,
                                 220, startPosition + x_displacement/2 - 150,
                                 250 + (4 - counter) * 10),
                          Linepen);


    string shapValueString = mzUtils::float2string(shapValue, 3);
    string changedLabel = _changedLabelName[label];                      
    changedLabel += " = ";
    changedLabel += shapValueString;

    QString peakFeature = QString(changedLabel.c_str());
    QString featureText = tr("%1").arg(peakFeature);

    QGraphicsTextItem* title = _scene.addText(featureText, font);
    title->setPos(startPosition - 150, 
                  250 + (4 - counter) * 10);
    
    return (startPosition + x_displacement);
}

int ClassificationWidget::makeArrowForPositives(float shapValue, 
                                                string label, 
                                                int counter, 
                                                int startPosition)
{
    // Setting pens and brush
    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::blue);
    auto color = pen.color();
    double alpha = shapValue / _sumPositiveWeights;
    color.setAlphaF(alpha);
    QBrush brush;
    brush.setColor(color);
    brush.setStyle(Qt::SolidPattern);
    pen.setColor(color);

    // Setting font for labelling.
    QFont font = QApplication::font();
    int pxSize = _scene.height() * 0.01;
    if (pxSize < 10)
        pxSize = 10;
    font.setPixelSize(pxSize);

    // Calculating percentage for shap value. 
    // and width of the arrow.
    float percentage = (shapValue/_absoluteTotalWeight) * 100;
    int x_displacement = percentage * 3; // for width of arrow.

    if (counter == 0) {

        QPolygonF polygon;
        polygon << QPoint(startPosition, 150);
        polygon << QPoint(startPosition + x_displacement, 150);
        polygon << QPoint(startPosition + x_displacement - 20, 170);
        polygon << QPoint(startPosition + x_displacement, 190);
        polygon << QPoint(startPosition , 190);
        _scene.addPolygon(polygon, pen, brush);

    } else {

        QPolygonF polygon;
        polygon << QPoint(startPosition, 150);
        polygon << QPoint(startPosition + x_displacement, 150);
        polygon << QPoint(startPosition + x_displacement - 20, 170);
        polygon << QPoint(startPosition + x_displacement, 190);
        polygon << QPoint(startPosition , 190);
        polygon << QPoint(startPosition - 20, 170);
        _scene.addPolygon(polygon, pen, brush);

    }

    //Labelings
    QPen Linepen;
    Linepen.setWidth(2);
    Linepen.setColor(color);
    auto line = _scene.addLine(QLineF(startPosition + x_displacement/2, 190,
                                      startPosition +x_displacement/2 + 70 ,
                                      220),
                               Linepen);
    line = _scene.addLine(QLineF(startPosition + x_displacement/2 + 70,
                                 220, startPosition + x_displacement/2 + 70 ,
                                 250 + counter * 10),
                          Linepen);


    string shapValueString = mzUtils::float2string(shapValue, 3);
    string changedLabel = _changedLabelName[label];                      
    changedLabel += " = ";
    changedLabel += shapValueString;
    QString peakFeature = QString(changedLabel.c_str());
    QString featureText = tr("%1").arg(peakFeature);

    QGraphicsTextItem* title = _scene.addText(featureText, font);
    title->setHtml(featureText);

    if (featureText.size() < 30)
        title->setPos(startPosition + x_displacement + 40, 250 + counter * 10);
    else if (featureText.size() < 40)
        title->setPos(startPosition + x_displacement, 250 + counter * 10);
    else
        title->setPos(startPosition + x_displacement - changedLabel.size(), 
                      250 + counter * 10);

    return (startPosition + x_displacement); 
}

void ClassificationWidget::setTitle()
{
    QString tagString;
    if (_group != NULL) {
        tagString = QString(_group->getName().c_str());
    } else if (_group->getSlice().compound != NULL) {
        tagString = QString(_group->getSlice().compound->name().c_str());
    } else if (!_group->getSlice().srmId.empty()) {
        tagString = QString(_group->getSlice().srmId.c_str());
    }
    QString titleText = tr("<b>%1</b>").arg(tagString);

    QFont font = QApplication::font();
    int pxSize = _scene.height() * 0.03;
    if (pxSize < 14)
        pxSize = 14;
    if (pxSize > 20)
        pxSize = 20;
    font.setPixelSize(pxSize);

    QGraphicsTextItem* title = _scene.addText(titleText, font);
    title->setHtml(titleText);
    int titleWidth = title->boundingRect().width();
    title->setPos(_scene.width() / 2 - titleWidth / 2, 10);
    title->update();
}

void ClassificationWidget::setOutputValue()
{
    QString tagString;
    tagString = "Output Value = ";
    QString outputString = tr("<b>%1</b>").arg(tagString);
    
    string outputValue = mzUtils::float2string(_totalWeight, 3);

    outputString += QString::fromStdString(outputValue);

    QFont font = QApplication::font();
    int pxSize = _scene.height() * 0.03;
    if (pxSize < 14)
        pxSize = 14;
    if (pxSize > 20)
        pxSize = 20;
    font.setPixelSize(pxSize);

    QGraphicsTextItem* outputTitle = _scene.addText(outputString, font);
    outputTitle->setHtml(outputString);
    int titleWidth = outputTitle->boundingRect().width();
    outputTitle->setPos(_scene.width() / 2 - titleWidth / 2, 100);
    outputTitle->update();
}

