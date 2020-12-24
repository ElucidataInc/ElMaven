#include <cmath>
#include "standardincludes.h"
#include "classificationWidget.h"
#include "tabledockwidget.h"
#include "PeakGroup.h"
#include "Compound.h"
#include "plot_axes.h"
#include "mzUtils.h"

ClassificationWidget::ClassificationWidget(TableDockWidget *tabledockWidget)
{
    _inferenceVisual = new QDialog(tabledockWidget);
    _layout = new QVBoxLayout;
    _tableDockWidget = tabledockWidget;
    _scene.setSceneRect(0, 0, 676, 267);
    _sceneView = new QGraphicsView(&_scene);
    _sceneView->setRenderHints(QPainter::Antialiasing);
    _layout->addWidget(_sceneView);
    _inferenceVisual->setLayout(_layout);
    _inferenceVisual->setMinimumSize(QSize(900, 400));
    _inferenceVisual->setMaximumSize(QSize(900, 400));
    _inferenceVisual->setWindowTitle("Classification inference");

    _group = _tableDockWidget->getSelectedGroup().get();
    _sumNegativeWeights = 0;
    _sumPositiveWeights = 0;
    _absoluteTotalWeight = 0;
    _totalWeight = 0;
    
}

void ClassificationWidget::renderGhostGroupMessage()
{
    QFont font = QApplication::font();
    font.setPixelSize(24);
    QGraphicsTextItem* noPeaksMessage =
        _scene.addText("Empty groups are not classified.", font);

    auto messageBounds = noPeaksMessage->boundingRect();
    int messageWidth = messageBounds.width();
    int messageHeight = messageBounds.height();
    int xPos = (_scene.width() / 2) - (messageWidth / 2);
    int yPos = (_scene.height() / 2) - (messageHeight / 2);
    noPeaksMessage->setPos(xPos, yPos);
    noPeaksMessage->setDefaultTextColor(Qt::white);

    QPainterPath path;
    messageBounds.setHeight(messageBounds.height() + 12);
    messageBounds.setWidth(messageBounds.width() + 24);
    path.addRoundedRect(messageBounds, 8, 8);
    QGraphicsPathItem* boundingRect = _scene.addPath(path,
                                                    QPen(Qt::darkGray),
                                                    QBrush(Qt::darkGray));
    boundingRect->setPos(xPos - 12, yPos - 6);
    boundingRect->setZValue(999);
    noPeaksMessage->setZValue(1000);
    _inferenceVisual->exec();
}

void ClassificationWidget::getAttributesTotalWeight()
{
    auto predictionInference = _group->predictionInference();

    for (auto it = predictionInference.begin();
        it != predictionInference.end();
        ++it)
    {
        _totalWeight += it->first;
    }
}

void ClassificationWidget::getAbsoluteTotalWeight() 
{
    auto predictionInference = _group->predictionInference();

    int counter = 0;

    // Only top 5 attributes taken in account.
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
}

void ClassificationWidget::showClassification()
{
    setTitle();

    if (_group->isGhost()) {
        renderGhostGroupMessage();
        return;
    }

    showLegend();

    auto predictionInference = _group->predictionInference();

    getAttributesTotalWeight();
    getAbsoluteTotalWeight();
    setBaseValue();

    int counter = 0;
    int startPosition = _scene.width() - 550;
    
    // Extracting values that are needed.
    multimap<float, string> positiveInference;
    multimap<float, string> negativeInference;
    for (auto it = predictionInference.rbegin();
         it != predictionInference.rend() && counter <  5;
         ++it)
    {
        positiveInference.insert({it->first, it->second});
        counter++;
    } 
    counter = 0;
    for (auto it = predictionInference.begin();
         it != predictionInference.end() && counter <  5;
         ++it)
    {
        negativeInference.insert({it->first, it->second});
        counter++;
    } 
    
    //making left side arrows with positives
    counter = 0;
    for (auto it = positiveInference.begin(); it != positiveInference.end(); ++it) {
        if (counter == 0){
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

    setOutputValue(startPosition);

    counter = 0;
    //making right side arrows with negatives
    for (auto it = negativeInference.rbegin();
         it != negativeInference.rend();
         ++it)
    {   
        if(counter == 0){
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

    _inferenceVisual->exec();
}

int ClassificationWidget::makeArrowForPositives(float shapValue, 
                                                string label, 
                                                int counter, 
                                                int startPosition)
{
    // Setting pens and brush
    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::red);
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
    label += " = ";
    label += shapValueString;

    QString peakFeature = QString(label.c_str());
    QString featureText = tr("%1").arg(peakFeature);

    QGraphicsTextItem* title = _scene.addText(featureText, font);
    title->setPos(startPosition - 150, 
                  250 + (4 - counter) * 10);
    
    return (startPosition + x_displacement);
}

int ClassificationWidget::makeArrowForNegatives(float shapValue, 
                                                string label, 
                                                int counter, 
                                                int startPosition)
{
    // Setting pens and brush
    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::blue);
    auto color = pen.color();
    double alpha = 0.6 - shapValue / _sumNegativeWeights;
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
    float normalisedShapValue  = 0.5 + shapValue; // to normalize negative value. as (-0.1 > -0.4).
    float percentage = (abs(normalisedShapValue)/_absoluteTotalWeight) * 100;
    int x_displacement = percentage * 4; // for width of arrow.


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
    label += " = ";
    label += shapValueString;
    QString peakFeature = QString(label.c_str());
    QString featureText = tr("%1").arg(peakFeature);

    QGraphicsTextItem* title = _scene.addText(featureText, font);
    title->setHtml(featureText);

    title->setPos(startPosition + x_displacement/2 + 70 - featureText.size() * 4, 250 + counter * 10);

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

void ClassificationWidget::showLegend()
{
    QFont font = QApplication::font();
    int pxSize = _scene.height() * 0.03;
    if (pxSize < 14)
        pxSize = 14;
    if (pxSize > 20)
        pxSize = 20;
    font.setPixelSize(pxSize);
    
    QString legendString = "Attributes contributing signal";
    QGraphicsTextItem* legend = _scene.addText(legendString, font);
    QGraphicsRectItem* square = new QGraphicsRectItem(_scene.width() - 120,30,10,10);
    square->setBrush(QBrush(Qt::red));
    _scene.addItem(square);
    legend->setPos(_scene.width() - 100, 20);
    legend->update();

    legendString = "Attributes contributing noise";
    legend = _scene.addText(legendString, font);
    square = new QGraphicsRectItem(_scene.width() - 120,50,10,10);
    square->setBrush(QBrush(Qt::blue));
    _scene.addItem(square);
    legend->setPos(_scene.width() - 100, 40);
    legend->update();
}

void ClassificationWidget::setBaseValue()
{
    QString tagString;
    tagString = "Base Value = ";
    QString basevalueString = tr("<b>%1</b>").arg(tagString);

    string baseValue = mzUtils::float2string(_group->baseValue, 3);

    basevalueString += QString::fromStdString(baseValue);

    QFont font = QApplication::font();
    int pxSize = _scene.height() * 0.03;
    if (pxSize < 14)
        pxSize = 14;
    if (pxSize > 20)
        pxSize = 20;
    font.setPixelSize(pxSize);

    QGraphicsTextItem* outputTitle = _scene.addText(basevalueString, font);
    outputTitle->setHtml(basevalueString);
    int titleWidth = outputTitle->boundingRect().width();
    outputTitle->setPos(_scene.width() / 2 - titleWidth / 2, 60);
    outputTitle->update();
}

void ClassificationWidget::setOutputValue(int startPosition)
{
    QString tagString;
    tagString = "Output Value = ";
    QString outputValueString = tr("<b>%1</b>").arg(tagString);

    string outputValue = mzUtils::float2string(_group->outputValue, 3);

    outputValueString += QString::fromStdString(outputValue);

    QFont font = QApplication::font();
    int pxSize = _scene.height() * 0.03;
    if (pxSize < 14)
        pxSize = 14;
    if (pxSize > 20)
        pxSize = 20;
    font.setPixelSize(pxSize);

    QPen Linepen;
    QColor color(Qt::black);
    Linepen.setWidth(1);
    Linepen.setColor(color);
    Linepen.setStyle( Qt::DashLine );
    auto line = _scene.addLine(QLineF(startPosition, 150,
                                      startPosition, 125),
                               Linepen);

    QGraphicsTextItem* outputTitle = _scene.addText(outputValueString, font);
    outputTitle->setHtml(outputValueString);
    int titleWidth = outputTitle->boundingRect().width();
    outputTitle->setPos(startPosition  - 50, 100);
    outputTitle->update();
}
