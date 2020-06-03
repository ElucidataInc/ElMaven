#include <limits>

#include "Compound.h"
#include "datastructures/mzSlice.h"
#include "EIC.h"
#include "gallerywidget.h"
#include "globals.h"
#include "mavenparameters.h"
#include "mzSample.h"
#include "Peak.h"
#include "PeakDetector.h"
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
    _minIntensity = 0.0f;
    _maxIntensity = 0.0f;

    _boxW = 300;
    _boxH = 200;
    _axesOffset = 18;
    recursionCheck = false;
    _plotItems.clear();

    _leftMarker = nullptr;
    _rightMarker = nullptr;
    _markerBeingDragged = nullptr;

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

pair<float, float> GalleryWidget::intensityBounds()
{
    return make_pair(_minIntensity, _maxIntensity);
}

void GalleryWidget::setIntensityBounds(float minIntensity, float maxIntensity)
{
    _minIntensity = minIntensity;
    _maxIntensity = maxIntensity;
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
                                   group->parameters().get());
    sort(begin(_eics), end(_eics), [this](EIC* first, EIC* second) {
        return first->sample->getSampleOrder()
               < second->sample->getSampleOrder();
    });

    _maxIntensity = numeric_limits<float>::min();
    _minRt = numeric_limits<float>::max();
    _maxRt = numeric_limits<float>::min();
    for (EIC* eic : _eics) {
        if (eic == nullptr)
            continue;

        QColor color = QColor::fromRgbF(eic->sample->color[0],
                                        eic->sample->color[1],
                                        eic->sample->color[2],
                                        0.5);

        Peak* samplePeak = group->getSamplePeak(eic->sample);
        float peakRtMin = -1.0f;
        float peakRtMax = -1.0f;
        if (samplePeak != nullptr) {
            peakRtMin = samplePeak->rtmin;
            peakRtMax = samplePeak->rtmax;
            if (_maxIntensity < samplePeak->peakIntensity)
                _maxIntensity = samplePeak->peakIntensity;
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
    }

    _minRt -= 0.5;
    _maxRt += 0.5;
    _maxIntensity *= 1.1f;

    // we add data only at this point, once bounds have been determined
    _fillPlotData();
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

    bool drawNoPeakMessage = !_visibleItemsHavePeakData();
    for (int index : _indexesOfVisibleItems) {
        TinyPlot* plot = _plotItems.at(index);
        plot->setWidth(_boxW);
        plot->setHeight(_boxH);
        plot->setPos(0, 0);
        plot->setDrawNoPeakMessages(drawNoPeakMessage);

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

void GalleryWidget::_drawBoundaryMarkers()
{
    bool allPeaksEmpty = true;
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

    if (allPeaksEmpty)
        return;

    _leftMarker = new QGraphicsLineItem(nullptr);
    scene()->addItem(_leftMarker);
    _rightMarker = new QGraphicsLineItem(nullptr);
    scene()->addItem(_rightMarker);

    QPen pen(Qt::black, 1, Qt::DashLine, Qt::FlatCap, Qt::RoundJoin);
    _leftMarker->setPen(pen);
    _leftMarker->setZValue(1000);
    _leftMarker->setLine(x1, yStart, x1, yEnd);
    _leftMarker->update();
    _rightMarker->setPen(pen);
    _rightMarker->setZValue(1000);
    _rightMarker->setLine(x2, yStart, x2, yEnd);
    _rightMarker->update();

    scene()->update();
}

void GalleryWidget::showPlotFor(vector<int> indexes)
{
    _indexesOfVisibleItems = indexes;
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

void GalleryWidget::_refillVisiblePlots(float x1, float x2)
{
    if (_plotItems.empty() || _indexesOfVisibleItems.empty())
        return;

    // lambda: given an X-coordinate, converts it to the closest RT in `eic`
    auto toRt = [this](float coordinate, EIC* eic) {
        float width = static_cast<float>(_boxW);
        float offset = static_cast<float>(_axesOffset);
        float ratio = (coordinate - offset) / (width - offset);
        float approximatedRt = _minRt + (ratio * (_maxRt - _minRt));

        float closestRealRt = approximatedRt;
        float minDiff = numeric_limits<float>::max();
        for (size_t i = 0; i < eic->size(); ++i) {
            float rt = eic->rt[i];
            if (rt < _minRt)
                continue;
            if (rt > _maxRt)
                break;
            float diff = abs(rt - approximatedRt);
            if (diff < minDiff) {
                closestRealRt = rt;
                minDiff = diff;
            }
        }
        return closestRealRt;
    };

    for (int index : _indexesOfVisibleItems) {
        auto plot = _plotItems.at(index);
        auto eic = _eics.at(index);

        float peakRtMin = toRt(x1, eic);
        float peakRtMax = toRt(x2, eic);
        _peakBounds[eic] = make_pair(peakRtMin, peakRtMax);
        emit peakRegionChanged(eic->sample, peakRtMin, peakRtMax);

        plot->clearData();
        plot->addData(eic, _minRt, _maxRt, true, peakRtMin, peakRtMax);
        plot->setXBounds(_minRt, _maxRt);
        plot->setYBounds(_minIntensity, _maxIntensity);
    }
    scene()->update();
}

void GalleryWidget::_fillPlotData()
{
    if (_plotItems.empty())
        return;

    for (size_t i = 0; i < _plotItems.size(); ++i) {
        auto plot = _plotItems[i];
        auto eic = _eics[i];
        auto& peakBounds = _peakBounds.at(eic);
        plot->clearData();
        plot->addData(eic,
                      _minRt,
                      _maxRt,
                      true,
                      peakBounds.first,
                      peakBounds.second);

        // make sure all plots are scaled into a single inclusive x-y plane
        plot->setXBounds(_minRt, _maxRt);
        plot->setYBounds(_minIntensity, _maxIntensity);
    }
}

bool GalleryWidget::_visibleItemsHavePeakData()
{
    bool anyHasPeakData = false;
    for (int index : _indexesOfVisibleItems) {
        auto eic = _eics.at(index);
        auto& peakBounds = _peakBounds.at(eic);
        if (peakBounds.first >= 0.0f && peakBounds.second >= 0.0f)
            anyHasPeakData = true;
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
        emit peakRegionChanged(eic->sample, rtMinMean, rtMaxMean);
    }
    _fillPlotData();
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
    event->ignore();
    QMenu menu;

    QAction* copyImage = menu.addAction("Copy Image to Clipboard");
    connect(copyImage, SIGNAL(triggered()), SLOT(copyImageToClipboard()));

    menu.exec(event->globalPos());
    scene()->update();
}

void GalleryWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Up
        || event->key() == Qt::Key_Down
        || event->key() == Qt::Key_Left
        || event->key() == Qt::Key_Right) {
        event->ignore();
    } else if (event->key() == Qt::Key_C) {
        _createNewPeak();
    }
}

void GalleryWidget::mouseMoveEvent(QMouseEvent* event)
{
    auto mousePos = event->pos();

    if (_markerBeingDragged == nullptr) {
        if (_rightMarker != nullptr
            && _markerNear(mousePos) == _rightMarker) {
            setCursor(Qt::SizeHorCursor);
        } else if (_leftMarker != nullptr
                   && _markerNear(mousePos) == _leftMarker) {
            setCursor(Qt::SizeHorCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }
        return;
    }

    float x = 0.0f;
    if (_markerBeingDragged == _leftMarker) {
        float x1 = mousePos.x();
        float x2 = _rightMarker->line().x1();
        if (x1 >= x2)
            return;
        _refillVisiblePlots(x1, x2);

        x = numeric_limits<float>::max();
        for (int index : _indexesOfVisibleItems) {
            auto plot = _plotItems.at(index);
            auto eic = _eics.at(index);
            float rt = _peakBounds.at(eic).first;
            x = min(x, static_cast<float>(plot->mapToPlot(rt, 0.0f).x()));
        }
    } else if (_markerBeingDragged == _rightMarker) {
        float x1 = _leftMarker->line().x1();
        float x2 = mousePos.x();
        if (x1 >= x2)
            return;
        _refillVisiblePlots(x1, x2);

        x = numeric_limits<float>::min();
        for (int index : _indexesOfVisibleItems) {
            auto plot = _plotItems.at(index);
            auto eic = _eics.at(index);
            float rt = _peakBounds.at(eic).second;
            x = max(x, static_cast<float>(plot->mapToPlot(rt, 0.0f).x()));
        }
    }

    auto line = _markerBeingDragged->line();
    _markerBeingDragged->setLine(QLineF(x, line.y1(), x, line.y2()));
    scene()->update();
}

void GalleryWidget::mousePressEvent(QMouseEvent* event)
{
    _markerBeingDragged = _markerNear(event->pos());
}

void GalleryWidget::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    _markerBeingDragged = nullptr;
}
