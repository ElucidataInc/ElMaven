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
    _group = _tableDock->getSelectedGroup();
    _sumNegativeWeights = 0;
    _sumPositiveWeights = 0;
}

void ClassificationWidget::showClassification()
{
    _layout->addWidget(_sceneView);
    _inferenceVisual->setLayout(_layout);
    _inferenceVisual->resize(QSize(900, 400));
    _inferenceVisual->setWindowTitle("Classification inference");

    setTitle();

    auto predictionInference = _group->predictionInference();

    int counter = 0;
    int startPosition = _scene.width() - 500;

    //getting total weights.
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

    counter = 0;

    //making left side arrows with negatives
    for (auto it = predictionInference.begin();
         it != predictionInference.end() && counter <  5;
         ++it)
    {
        if (counter == 0){
            minNegative = it->first;
            startPosition = makeArrowForNegatives(it->first, it->second, counter, startPosition);
        }
        else
            startPosition = makeArrowForNegatives(it->first, it->second, counter, startPosition + 4);
        if(counter == 4)
            maxNegative = it->first;
        counter++;
    }

    plotAxes(0, _scene.width() - 500, startPosition, minNegative, maxNegative);
    float startPositive = startPosition;
    counter = 0;
    //making right side arrows with positives
    for (auto it = predictionInference.rbegin();
         it != predictionInference.rend() && counter < 5;
         ++it)
    {
        if(counter == 0){
            maxPositive = it->first;
            startPosition = makeArrowForPositives(it->first, it->second, counter, startPosition);
        }
        else
            startPosition = makeArrowForPositives(it->first, it->second, counter, startPosition + 4);
        if(counter == 4)
            minPositive = it->first;
        counter++;
    }
    plotAxes(1, startPositive, startPosition, minPositive, maxPositive);
    _inferenceVisual->exec();
}

void ClassificationWidget::plotAxes(int type, float startX, float endX, float min, float max)
{
    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::black);

    QFont font = QApplication::font();
    int pxSize = _scene.height() * 0.02;
    if (pxSize < 8)
        pxSize = 8;
    font.setPixelSize(pxSize);

    if(type == 0) {
        auto line = _scene.addLine(startX, 130, endX, 130, pen);

        line  = _scene.addLine(startX, 130 - 5, startX, 130 + 5, pen);
        string val = mzUtils::float2string(min, 2);
        QString value = QString(val.c_str());
        auto text = _scene.addText(value, font);
        text->setPos(startX - 1 , 135);

        line = _scene.addLine(endX - 10 , 130 - 5 , endX - 10, 130 +5, pen);
        val = mzUtils::float2string(max, 2);
        value = QString(val.c_str());
        text = _scene.addText(value, font);
        text->setPos(endX - 25 , 135);

        line = _scene.addLine(startX + (endX - startX)/2, 130 - 5, startX + (endX - startX)/2, 130 + 5, pen);
        val = mzUtils::float2string((max + min) /2, 2);
        value = QString(val.c_str());
        text = _scene.addText(value, font);
        text->setPos(startX + (endX - startX)/2 - 1 , 135);
    } else {

        auto line = _scene.addLine(startX, 130, endX, 130, pen);

        line  = _scene.addLine(startX + 30, 130 - 5, startX+30, 130 + 5, pen);
        string val = mzUtils::float2string(max, 2);
        QString value = QString(val.c_str());
        auto text = _scene.addText(value, font);
        text->setPos(startX +30 , 135);

        line = _scene.addLine(endX , 130 - 5 , endX, 130 +5, pen);
        val = mzUtils::float2string(min, 2);
        value = QString(val.c_str());
        text = _scene.addText(value, font);
        text->setPos(endX - 1 , 135);
    }

}

int ClassificationWidget::makeArrowForNegatives(float width, string label, int counter, int startPosition)
{
    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::red);
    auto color = pen.color();
    int x_displacement;
    int upperLimitWidth = 60;
    int lowerLimitWidth = 10;

    double f = 0.8 - (width / _sumNegativeWeights);

    color.setAlphaF(f);
    QBrush brush;
    brush.setColor(color);
    brush.setStyle(Qt::SolidPattern);
    pen.setColor(color);

    if (counter != 4) {

        float widthScaled = 0.7 - abs(width);
        x_displacement = lowerLimitWidth + (upperLimitWidth - lowerLimitWidth) * widthScaled;

        QPolygonF polygon;
        polygon << QPoint(startPosition, 150);
        polygon << QPoint(startPosition + x_displacement, 150);
        polygon << QPoint(startPosition + x_displacement + 20, 170);
        polygon << QPoint(startPosition + x_displacement, 190);
        polygon << QPoint(startPosition , 190);
        polygon << QPoint(startPosition + 20, 170);
        _scene.addPolygon(polygon, pen, brush);

    } else {

        float widthScaled = 0.7 - abs(width);
        x_displacement = lowerLimitWidth + (upperLimitWidth - lowerLimitWidth) * widthScaled;
        x_displacement += 10;     //done because no arrow shape gives less width in visualization
                                  //as compared to other arrows

        QBrush brush;
        brush.setColor(color);
        brush.setStyle(Qt::SolidPattern);

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
                                      startPosition + x_displacement/2 - 70 ,
                                      220),
                               Linepen);
    line = _scene.addLine(QLineF(startPosition + x_displacement/2 - 70,
                                 220, startPosition + x_displacement/2 - 70 ,
                                 250 + (4 - counter) * 10),
                          Linepen);
    label += " = ";
    label += mzUtils::float2string(width, 2);

    QString peakFeature = QString(label.c_str());
    QString featureText = tr("%1").arg(peakFeature);

    QFont font = QApplication::font();
    int pxSize = _scene.height() * 0.02;
    if (pxSize < 8)
        pxSize = 8;
    font.setPixelSize(pxSize);

    QGraphicsTextItem* title = _scene.addText(featureText, font);
    title->setHtml(featureText);
    title->setPos(startPosition - 70, 250 + (4 - counter) * 10);

    return (startPosition + x_displacement);
}

int ClassificationWidget::makeArrowForPositives(float width, string label, int counter, int startPosition)
{
    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::blue);
    auto color = pen.color();
    int upperLimitWidth = 60;
    int lowerLimitWidth = 10;

    double f = abs(width) / _sumPositiveWeights;
    color.setAlphaF(f+0.2);
    pen.setColor(color);
    QBrush brush;
    brush.setColor(color);
    brush.setStyle(Qt::SolidPattern);

    int x_displacement;

    if (counter == 0) {

        x_displacement = lowerLimitWidth + (upperLimitWidth - lowerLimitWidth) * width;

        QPolygonF polygon;
        polygon << QPoint(startPosition, 150);
        polygon << QPoint(startPosition + x_displacement, 150);
        polygon << QPoint(startPosition + x_displacement - 20, 170);
        polygon << QPoint(startPosition + x_displacement, 190);
        polygon << QPoint(startPosition , 190);
        _scene.addPolygon(polygon, pen, brush);

    } else {

        x_displacement = lowerLimitWidth + (upperLimitWidth - lowerLimitWidth) * width;

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
    label += " = ";
    label += mzUtils::float2string(width, 2);
    QString peakFeature = QString(label.c_str());
    QString featureText = tr("%1").arg(peakFeature);

    QFont font = QApplication::font();
    int pxSize = _scene.height() * 0.02;
    if (pxSize < 8)
        pxSize = 8;
    font.setPixelSize(pxSize);

    QGraphicsTextItem* title = _scene.addText(featureText, font);
    title->setHtml(featureText);

    if(peakFeature.size() < 25)
        title->setPos(startPosition +x_displacement - 10, 250 + counter * 10);
    else
        title->setPos(startPosition +x_displacement - 40, 250 + counter * 10);

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
    int titleWith = title->boundingRect().width();
    title->setPos(_scene.width() / 2 - titleWith / 2, 10);
    title->update();
}

