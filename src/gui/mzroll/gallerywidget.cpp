#include <limits>

#include "Compound.h"
#include "datastructures/mzSlice.h"
#include "EIC.h"
#include "gallerywidget.h"
#include "globals.h"
#include "mavenparameters.h"
#include "mzSample.h"
#include "Peak.h"
#include "peakdetector.h"
#include "PeakGroup.h"
#include "Scan.h"
#include "statistics.h"
#include "tinyplot.h"

GalleryWidget::GalleryWidget(QWidget* parent)
{
    setParent(parent);
    setScene(new QGraphicsScene(this));
    scene()->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
    setObjectName("Gallery");
    setAlignment(Qt::AlignLeft | Qt::AlignTop);

    _minRt = 0.0f;
    _maxRt = 0.0f;
    _rtBuffer = 0.5f;

    _yZoomScale = 1.0f;

    _boxW = 300;
    _boxH = 200;
    _axesOffset = 18;
    recursionCheck = false;
    _plotItems.clear();

    _leftEdit = nullptr;
    _leftProxyEdit = nullptr;
    _rightEdit = nullptr;
    _rightProxyEdit = nullptr;

    _rightMarker = nullptr;
    _leftMarker = nullptr;
    _rightMarker = nullptr;
    _markerBeingDragged = nullptr;
    _scaleForHighestPeak = false;

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    horizontalScrollBar()->setEnabled(false);

    setMouseTracking(true);
}

GalleryWidget::~GalleryWidget()
{
    if (scene() != NULL)
        delete (scene());
}

void GalleryWidget::clear()
{
    scene()->clear();
    _plotItems.clear();
    delete_all(_eics);
    _peakBounds.clear();
    _leftEdit = nullptr;
    _rightEdit = nullptr;
    _leftProxyEdit = nullptr;
    _rightProxyEdit = nullptr;
    _leftMarker = nullptr;
    _rightMarker = nullptr;
}

pair<float, float> GalleryWidget::rtBounds()
{
    return make_pair(_minRt, _maxRt);
}

void GalleryWidget::setRtBounds(float minRt, float maxRt)
{
    _minRt = minRt;
    _maxRt = maxRt;
    _fillPlotData();
    replot();
}

void GalleryWidget::setYZoomScale(float yZoomScale)
{
    _yZoomScale = yZoomScale;
    _fillPlotData();
    replot();
}

void GalleryWidget::addEicPlots(PeakGroup* group)
{
    clear();
    if (group == nullptr || !group->hasSlice())
        return;

    // set min/max limits on slice to cover the entire EIC range
    // this helps with baseline calculation as well as RT adjustment
    mzSlice slice = group->getSlice();
    slice.rtmin = numeric_limits<float>::max();
    slice.rtmax = numeric_limits<float>::min();
    for (mzSample* sample : group->samples) {
        slice.rtmin = min(slice.rtmin, sample->minRt);
        slice.rtmax = max(slice.rtmax, sample->maxRt);
    }
    _eics = PeakDetector::pullEICs(&slice,
                                   group->samples,
                                   group->parameters().get(),
                                   false);
    sort(begin(_eics), end(_eics), [this](EIC* first, EIC* second) {
        return first->sample->getSampleOrder()
               < second->sample->getSampleOrder();
    });

    _minRt = numeric_limits<float>::max();
    _maxRt = numeric_limits<float>::min();
    for (EIC* eic : _eics) {
        if (eic == nullptr)
            continue;

        QColor color = QColor::fromRgbF(eic->sample->color[0],
                                        eic->sample->color[1],
                                        eic->sample->color[2],
                                        0.5);

        Peak* samplePeak = group->getPeak(eic->sample);
        float peakRtMin = -1.0f;
        float peakRtMax = -1.0f;
        if (samplePeak != nullptr) {
            peakRtMin = samplePeak->rtmin;
            peakRtMax = samplePeak->rtmax;
            if (peakRtMin < _minRt)
                _minRt = peakRtMin;
            if (peakRtMax > _maxRt)
                _maxRt = peakRtMax;
        }

        // creating empty plots and setting attributes
        TinyPlot* plot = new TinyPlot(nullptr, scene());
        plot->setWidth(_boxW);
        plot->setHeight(_boxH);
        plot->setAxesOffset(_axesOffset);
        plot->setColor(color);
        plot->setNoPeakMessage("NO PEAK");
        plot->setNoPeakSubMessage("(Press \"C\" in this region, "
                                  "to create one)");
        _plotItems << plot;
        _peakBounds[eic] = make_pair(peakRtMin, peakRtMax);
        emit peakRegionSet(eic->sample, peakRtMin, peakRtMax);
    }

    // possible when all peaks are zeroed
    if (_minRt == numeric_limits<float>::max()
        && _maxRt == numeric_limits<float>::min()) {
        _minRt = _maxRt = group->meanRt;
    }
    _minRt -= _rtBuffer;
    _maxRt += _rtBuffer;

    // we add data only at this point, once bounds have been determined
    _fillPlotData();
}

void GalleryWidget::recomputeBaselinesThresh(int dropTopX, int smoothingWindow)
{
    for (EIC* eic : _eics) {
        eic->setBaselineMode(EIC::BaselineMode::Threshold);
        eic->setBaselineDropTopX(dropTopX);
        eic->setBaselineSmoothingWindow(smoothingWindow);
        eic->computeBaseline();
    }
    _fillPlotData();
    replot();
}

void GalleryWidget::recomputeBaselinesAsLS(int smoothness, int asymmetry)
{
    for (EIC* eic : _eics) {
        eic->setBaselineMode(EIC::BaselineMode::AsLSSmoothing);
        eic->setAsLSSmoothness(smoothness);
        eic->setAsLSAsymmetry(asymmetry);
        eic->computeBaseline();
    }
    _fillPlotData();
    replot();
}

void GalleryWidget::replot()
{
    if (recursionCheck == true)
        return;

    if (width() < 50 || height() < 50)
        return;

    if (_plotItems.empty())
        return;

    recursionCheck = true;

    _boxW = viewport()->width();
    _boxH = viewport()->height();
    setSceneRect(0, 0, _boxW, _boxH);

    // first we remove all existing plots on the scene
    for (TinyPlot* plot : _plotItems) {
        if (plot->scene() == scene())
            scene()->removeItem(plot);
        scene()->update();
    }

    bool noVisiblePeakData = !_visibleItemsHavePeakData();
    for (int index : _indexesOfVisibleItems) {
        TinyPlot* plot = _plotItems.at(index);
        plot->setWidth(_boxW);
        plot->setHeight(_boxH);
        plot->setPos(0, 0);
        plot->setDrawNoPeakMessages(noVisiblePeakData);

        // draw axes for only the last overlayed plot
        if (index == _indexesOfVisibleItems.back()) {
            plot->setDrawAxes(true);
        } else {
            plot->setDrawAxes(false);
        }

        scene()->addItem(plot);
    }

    scene()->update();
    _drawBoundaryMarkers();
    recursionCheck = false;
}

void GalleryWidget::_drawBoundaryEditables(float rt1,
                                           float x1,
                                           float rt2,
                                           float x2,
                                           bool allPeaksEmpty)
{
    if (_leftProxyEdit != nullptr) {
        delete _leftProxyEdit;
        _leftProxyEdit = nullptr;
    }
    scene()->update();
    if (_rightProxyEdit != nullptr) {
        delete _rightProxyEdit;
        _rightProxyEdit = nullptr;
    }
    scene()->update();

    if (allPeaksEmpty)
        return;

    const float height = 24.0f;
    const float width = 48.0f;
    const float xOffset = 2.0f;
    const float yOffset = 12.0f;

    _leftEdit = new QLineEdit();
    _leftEdit->setAlignment(Qt::AlignRight);
    _leftEdit->setText(QString::number(rt1, 'f', 3));
    _leftProxyEdit = scene()->addWidget(_leftEdit);
    _leftProxyEdit->setZValue(1000);
    _leftProxyEdit->setGeometry(QRectF(x1 - width - xOffset,
                                       yOffset,
                                       width,
                                       height));

    _rightEdit = new QLineEdit();
    _rightEdit->setAlignment(Qt::AlignLeft);
    _rightEdit->setText(QString::number(rt2, 'f', 3));
    _rightProxyEdit = scene()->addWidget(_rightEdit);
    _rightProxyEdit->setZValue(1000);
    _rightProxyEdit->setGeometry(QRectF(x2 + xOffset,
                                        yOffset,
                                        width,
                                        height));
}

void GalleryWidget::_drawBoundaryMarkers()
{
    bool allPeaksEmpty = true;
    float rt1 = numeric_limits<float>::max();
    float rt2 = numeric_limits<float>::min();
    float x1 = numeric_limits<float>::max();
    float x2 = numeric_limits<float>::min();
    for (int index : _indexesOfVisibleItems) {
        EIC* eic = _eics.at(index);
        TinyPlot* plot = _plotItems[index];

        auto rtBounds = _peakBounds.at(eic);
        float rtMin = rtBounds.first;
        float rtMax = rtBounds.second;
        if (rtMin < 0.0f && rtMax < 0.0f)
            continue;

        allPeaksEmpty = false;
        rt1 = min(rt1, rtMin);
        rt2 = max(rt2, rtMax);
        x1 = min(x1, static_cast<float>(plot->mapToPlot(rtMin, 0.0f).x()));
        x2 = max(x2, static_cast<float>(plot->mapToPlot(rtMax, 0.0f).x()));
    }
    float yStart = 0;
    float yEnd = _boxH - _axesOffset;

    if (_leftMarker != nullptr) {
        delete _leftMarker;
        _leftMarker = nullptr;
    }
    scene()->update();
    if (_rightMarker != nullptr) {
        delete _rightMarker;
        _rightMarker = nullptr;
    }
    scene()->update();

    _drawBoundaryEditables(rt1, x1, rt2, x2, allPeaksEmpty);

    if (allPeaksEmpty)
        return;

    _leftMarker = new QGraphicsLineItem(nullptr);
    scene()->addItem(_leftMarker);
    _rightMarker = new QGraphicsLineItem(nullptr);
    scene()->addItem(_rightMarker);

    QPen pen(Qt::black, 1, Qt::DashLine, Qt::FlatCap, Qt::RoundJoin);
    _leftMarker->setPen(pen);
    _leftMarker->setZValue(990);
    _leftMarker->setLine(x1, yStart, x1, yEnd);
    _leftMarker->update();
    _rightMarker->setPen(pen);
    _rightMarker->setZValue(990);
    _rightMarker->setLine(x2, yStart, x2, yEnd);
    _rightMarker->update();

    scene()->update();
}

void GalleryWidget::showPlotFor(vector<int> indexes)
{
    _indexesOfVisibleItems = indexes;
    _scaleVisibleYAxis();
    replot();
}

void GalleryWidget::setScaleForHighestPeak(bool scale)
{
    _scaleForHighestPeak = scale;
    _scaleVisibleYAxis();
    replot();
}

void GalleryWidget::copyImageToClipboard()
{
    QPixmap image(this->width(), this->height());
    image.fill(Qt::white);
    QPainter painter;
    painter.begin(&image);
    render(&painter);
    painter.end();
    QApplication::clipboard()->setPixmap(image);
}

QGraphicsLineItem* GalleryWidget::_markerNear(QPointF pos)
{
    auto rect = QRectF(pos.x() - 2.0f, pos.y() - 2.0f, 4.0f, 4.0f);
    if (_leftMarker != nullptr
        && _leftMarker->boundingRect().intersects(rect)) {
        return _leftMarker;
    }
    if (_rightMarker != nullptr
        && _rightMarker->boundingRect().intersects(rect)) {
        return _rightMarker;
    }
    return nullptr;
}

float GalleryWidget::_closestRealRt(float approximateRt, EIC* eic)
{
    float closestRealRt = approximateRt;
    float minDiff = numeric_limits<float>::max();
    for (size_t i = 0; i < eic->size(); ++i) {
        float rt = eic->rt[i];
        if (rt < _minRt)
            continue;
        if (rt > _maxRt)
            break;
        float diff = abs(rt - approximateRt);
        if (diff < minDiff) {
            closestRealRt = rt;
            minDiff = diff;
        }
    }
    return closestRealRt;
}

float GalleryWidget::_convertXCoordinateToRt(float x, EIC* eic)
{
    // given an X-coordinate, converts it to the closest RT in `eic`
    float width = static_cast<float>(_boxW);
    float offset = static_cast<float>(_axesOffset);
    float ratio = (x - offset) / (width - offset);
    float approximatedRt = _minRt + (ratio * (_maxRt - _minRt));
    return _closestRealRt(approximatedRt, eic);
}

void GalleryWidget::_refillVisiblePlots(float x1, float x2)
{
    if (_plotItems.empty() || _indexesOfVisibleItems.empty())
        return;

    for (int index : _indexesOfVisibleItems) {
        auto plot = _plotItems.at(index);
        auto eic = _eics.at(index);

        float peakRtMin = _convertXCoordinateToRt(x1, eic);
        float peakRtMax = _convertXCoordinateToRt(x2, eic);
        _peakBounds[eic] = make_pair(peakRtMin, peakRtMax);
        emit peakRegionSet(eic->sample, peakRtMin, peakRtMax);

        plot->clearData();
        plot->addData(eic, _minRt, _maxRt, true, peakRtMin, peakRtMax);
        plot->setXBounds(_minRt, _maxRt);
    }

    bool noVisiblePeakData = !_visibleItemsHavePeakData();
    for (int index : _indexesOfVisibleItems) {
        auto plot = _plotItems.at(index);
        plot->setDrawNoPeakMessages(noVisiblePeakData);
    }

    _scaleVisibleYAxis();
    scene()->update();
}

void GalleryWidget::_scaleVisibleYAxis()
{
    float minIntensity = numeric_limits<float>::max();
    float maxIntensity = numeric_limits<float>::min();
    if (_scaleForHighestPeak) {
        for (size_t i = 0; i < _plotItems.size(); ++i) {
            auto plot = _plotItems[i];
            plot->computeYBoundsFromData();
            auto plotIntensities = plot->yBounds();
            minIntensity = min(minIntensity, plotIntensities.first);
            maxIntensity = max(maxIntensity, plotIntensities.second);
        }
    } else {
        for (int index : _indexesOfVisibleItems) {
            auto plot = _plotItems.at(index);
            plot->computeYBoundsFromData();
            auto plotIntensities = plot->yBounds();
            minIntensity = min(minIntensity, plotIntensities.first);
            maxIntensity = max(maxIntensity, plotIntensities.second);
        }
    }

    maxIntensity = _yZoomScale * maxIntensity;

    for (int index : _indexesOfVisibleItems) {
        auto plot = _plotItems.at(index);
        plot->setYBounds(minIntensity, maxIntensity);
    }
}

void GalleryWidget::_fillPlotData()
{
    if (_plotItems.empty())
        return;

    for (size_t i = 0; i < _plotItems.size(); ++i) {
        auto plot = _plotItems[i];
        auto eic = _eics[i];
        auto& peakBounds = _peakBounds.at(eic);
        float peakRtMin = _closestRealRt(peakBounds.first, eic);
        float peakRtMax = _closestRealRt(peakBounds.second, eic);

        plot->clearData();
        plot->addData(eic, _minRt, _maxRt, true, peakRtMin, peakRtMax);

        // make sure all plots are scaled into a single inclusive x-range
        plot->setXBounds(_minRt, _maxRt);
    }
    _scaleVisibleYAxis();
}

bool GalleryWidget::_visibleItemsHavePeakData()
{
    bool anyHasPeakData = false;
    for (int index : _indexesOfVisibleItems) {
        auto eic = _eics.at(index);
        auto& peakBounds = _peakBounds.at(eic);
        if (peakBounds.first != peakBounds.second
            && peakBounds.first >= 0.0f
            && peakBounds.second >= 0.0f) {
            anyHasPeakData = true;
        }
    }
    return anyHasPeakData;
}

void GalleryWidget::_createNewPeak()
{
    if (_visibleItemsHavePeakData())
        return;

    int presentPeaks = 0;
    float rtMinMean = 0.0f;
    float rtMaxMean = 0.0f;
    for (auto& elem : _peakBounds) {
        auto& peakBounds = elem.second;
        if (peakBounds.first < 0.0f && peakBounds.second < 0.0f)
            continue;

        rtMinMean += peakBounds.first;
        rtMaxMean += peakBounds.second;
        ++presentPeaks;
    }
    if (rtMinMean == 0.0f && rtMaxMean == 0.0f) {
        float rtCenter = _minRt + ((_maxRt - _minRt) / 2.0f);
        float twoSeconds = 2.0f / 60.0f;
        rtMinMean = max(rtCenter - twoSeconds, _minRt + twoSeconds);
        rtMaxMean = min(rtCenter + twoSeconds, _maxRt - twoSeconds);
    } else {
        rtMinMean /= presentPeaks;
        rtMaxMean /= presentPeaks;
    }

    for (int index : _indexesOfVisibleItems) {
        EIC* eic = _eics.at(index);
        _peakBounds[eic] = make_pair(rtMinMean, rtMaxMean);
        emit peakRegionSet(eic->sample, rtMinMean, rtMaxMean);
    }
    _fillPlotData();
    replot();
}

void GalleryWidget::_deleteCurrentPeak()
{
    if (!_visibleItemsHavePeakData())
        return;

    for (int index : _indexesOfVisibleItems) {
        EIC* eic = _eics.at(index);
        _peakBounds[eic] = make_pair(-1.0f, -1.0f);
        emit peakRegionSet(eic->sample, -1.0f, -1.0f);
    }
    _fillPlotData();
    replot();
}

void GalleryWidget::_setRtRegionForVisiblePeaks(float minRt, float maxRt)
{
    if (_plotItems.empty() || _indexesOfVisibleItems.empty())
        return;

    if (_minRt >= minRt - 0.5f)
        _minRt = max(0.0f, minRt - 0.5f);
    if (_maxRt <= maxRt + 0.5f)
        _maxRt = maxRt + 0.5f;

    for (int index : _indexesOfVisibleItems) {
        auto eic = _eics.at(index);
        float peakRtMin = _closestRealRt(minRt, eic);
        float peakRtMax = _closestRealRt(maxRt, eic);
        _peakBounds[eic] = make_pair(peakRtMin, peakRtMax);
        emit peakRegionSet(eic->sample, peakRtMin, peakRtMax);
    }

    _fillPlotData();
    _scaleVisibleYAxis();
    replot();
}

void GalleryWidget::wheelEvent(QWheelEvent* event)
{
    event->ignore();
}

void GalleryWidget::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
    replot();
}

void GalleryWidget::contextMenuEvent(QContextMenuEvent* event)
{
    if ((_leftProxyEdit != nullptr && _leftEdit->hasFocus())
        || (_rightProxyEdit != nullptr && _rightEdit->hasFocus())) {
        QGraphicsView::contextMenuEvent(event);
        return;
    }

    event->ignore();
    QMenu menu;

    QAction* copyImage = menu.addAction("Copy Image to Clipboard");
    connect(copyImage, SIGNAL(triggered()), SLOT(copyImageToClipboard()));

    QAction* createPeak = menu.addAction("Create new peak");
    connect(createPeak,
            &QAction::triggered,
            this,
            &GalleryWidget::_createNewPeak);
    if (_visibleItemsHavePeakData())
        createPeak->setEnabled(false);

    QAction* deletePeak = menu.addAction("Delete current peak");
    connect(deletePeak,
            &QAction::triggered,
            this,
            &GalleryWidget::_deleteCurrentPeak);
    if (!_visibleItemsHavePeakData())
        deletePeak->setEnabled(false);

    menu.exec(event->globalPos());
    scene()->update();
}

void GalleryWidget::keyPressEvent(QKeyEvent *event)
{
    if ((_leftProxyEdit != nullptr && _leftEdit->hasFocus())
        || (_rightProxyEdit != nullptr && _rightEdit->hasFocus())) {
        QGraphicsView::keyPressEvent(event);
        if (event->key() == Qt::Key_Return) {
            bool minOk;
            float minRt = _leftEdit->text().toFloat(&minOk);
            bool maxOk;
            float maxRt = _rightEdit->text().toFloat(&maxOk);
            if (minOk && maxOk) {
                if (minRt > maxRt)
                    minRt = maxRt;
                if (maxRt < minRt)
                    maxRt = minRt;
                _setRtRegionForVisiblePeaks(minRt, maxRt);
            }
        }
        return;
    }

    if (event->key() == Qt::Key_Up
        || event->key() == Qt::Key_Down
        || event->key() == Qt::Key_Left
        || event->key() == Qt::Key_Right) {
        event->ignore();
    } else if (event->key() == Qt::Key_C) {
        event->accept();
        _createNewPeak();
    } else if (event->key() == Qt::Key_Delete) {
        event->accept();
        _deleteCurrentPeak();
    }
}

void GalleryWidget::mouseMoveEvent(QMouseEvent* event)
{
    auto mousePos = event->pos();

    if (_markerBeingDragged == nullptr) {
        QGraphicsView::mouseMoveEvent(event);
        if (_rightMarker != nullptr
            && _markerNear(mousePos) == _rightMarker) {
            viewport()->setCursor(Qt::SizeHorCursor);
        } else if (_leftMarker != nullptr
                   && _markerNear(mousePos) == _leftMarker) {
            viewport()->setCursor(Qt::SizeHorCursor);
        } else {
            viewport()->setCursor(Qt::ArrowCursor);
        }
        return;
    }

    float editableRt = 0.0f;
    QLineEdit* editable = nullptr;
    QGraphicsProxyWidget* editableWidget = nullptr;

    float x = 0.0f;
    if (_markerBeingDragged == _leftMarker) {
        editable = _leftEdit;
        editableWidget = _leftProxyEdit;

        float x1 = mousePos.x();
        float x2 = _rightMarker->line().x1();
        if (x1 >= x2)
            return;
        _refillVisiblePlots(x1, x2);

        x = numeric_limits<float>::max();
        editableRt = numeric_limits<float>::max();
        for (int index : _indexesOfVisibleItems) {
            auto plot = _plotItems.at(index);
            auto eic = _eics.at(index);
            float rt = _peakBounds.at(eic).first;
            x = min(x, static_cast<float>(plot->mapToPlot(rt, 0.0f).x()));
            editableRt = min(editableRt, rt);
        }
    } else if (_markerBeingDragged == _rightMarker) {
        editable = _rightEdit;
        editableWidget = _rightProxyEdit;

        float x1 = _leftMarker->line().x1();
        float x2 = mousePos.x();
        if (x1 >= x2)
            return;
        _refillVisiblePlots(x1, x2);

        x = numeric_limits<float>::min();
        editableRt = numeric_limits<float>::min();
        for (int index : _indexesOfVisibleItems) {
            auto plot = _plotItems.at(index);
            auto eic = _eics.at(index);
            float rt = _peakBounds.at(eic).second;
            x = max(x, static_cast<float>(plot->mapToPlot(rt, 0.0f).x()));
            editableRt = max(editableRt, rt);
        }
    }

    auto line = _markerBeingDragged->line();
    _markerBeingDragged->setLine(QLineF(x, line.y1(), x, line.y2()));

    // move the editable entries for RT for the corresponding marker
    if (editable != nullptr && editableWidget != nullptr) {
        editable->setText(QString::number(editableRt, 'f', 3));
        auto rect = editableWidget->geometry();
        auto rectX = rect.x();
        auto rectY = rect.y();
        auto rectWidth = rect.width();
        auto rectHeight = rect.height();

        auto xOffset = 2.0f;
        if (editableWidget == _leftProxyEdit) {
            rectX = x - rectWidth - xOffset;
            if (rectX < _axesOffset)
                rectX = _axesOffset + 1;
        } else if (editableWidget == _rightProxyEdit) {
            rectX = x + xOffset;
            if (rectX + rectWidth > _boxW)
                rectX = _boxW - rectWidth - 1;
        }
        editableWidget->setGeometry(QRectF(rectX,
                                           rectY,
                                           rectWidth,
                                           rectHeight));
    }

    scene()->update();
}

void GalleryWidget::mousePressEvent(QMouseEvent* event)
{
    _markerBeingDragged = _markerNear(event->pos());
    if (_markerBeingDragged == nullptr)
        QGraphicsView::mousePressEvent(event);
}

void GalleryWidget::mouseReleaseEvent(QMouseEvent* event)
{
    _markerBeingDragged = nullptr;
    QGraphicsView::mouseReleaseEvent(event);
}
