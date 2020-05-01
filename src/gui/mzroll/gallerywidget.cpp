#include <limits>

#include "Compound.h"
#include "datastructures/mzSlice.h"
#include "EIC.h"
#include "gallerywidget.h"
#include "globals.h"
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
    _nItemsVisible = 1;
    _indexOfVisibleItem = 0;

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

void GalleryWidget::addEicPlots(PeakGroup* group, MavenParameters* mp)
{
    clear();
    if (group == nullptr || !group->hasSlice())
        return;

    const mzSlice& slice = group->getSlice();
    _eics = PeakDetector::pullEICs(&slice, group->samples, mp);
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
                                        1.0);

        Peak* samplePeak = nullptr;
        for (auto& peak : group->peaks) {
            if (peak.getSample() == eic->sample) {
                samplePeak = &peak;
                break;
            }
        }
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
        plot->addDataColor(color);
        _plotItems << plot;
        _peakBounds[eic] = make_pair(peakRtMin, peakRtMax);
        scene()->addItem(plot);
    }

    if (_plotItems.size() > 0) {
        _minRt -= 0.5;
        _maxRt += 0.5;
        _maxIntensity *= 1.1f;
        for (size_t i = 0; i < _plotItems.size(); ++i) {
            auto plot = _plotItems[i];
            auto eic = _eics[i];
            auto& peakBounds = _peakBounds.at(eic);

            // we add data only at this point, once bounds have been determined
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
        replot();
    }
}

void GalleryWidget::replot()
{
    if (recursionCheck == false) {
        recursionCheck = true;
        drawMap();
        recursionCheck = false;
    }
}

void GalleryWidget::drawMap()
{
    if (width() < 50 or height() < 50)
        return;

    int nItems = _plotItems.size();
    if (nItems == 0)
        return;

    _boxW = viewport()->width();
    _boxH = viewport()->height() / _nItemsVisible;

    int sceneW = _boxW;
    int sceneH = nItems * _boxH;
    setSceneRect(0, 0, sceneW, sceneH);

    int col = 0;
    for (int row = 0; row < nItems; ++row) {
        int xpos = col * _boxW;
        int ypos = row * _boxH;

        TinyPlot* item = _plotItems[row];
        item->setWidth(_boxW);
        item->setHeight(_boxH);

        item->setPos(xpos, ypos);
    }

    scene()->update();
    _indexOfVisibleItem = 0;
    _ensureCurrentItemIsVisible();
}

void GalleryWidget::_drawBoundaryMarkers()
{
    EIC* eic = _eics.at(_indexOfVisibleItem);
    TinyPlot* plot = _plotItems[_indexOfVisibleItem];

    auto rtBounds = _peakBounds.at(eic);
    float rtMin = rtBounds.first;
    float rtMax = rtBounds.second;

    float x1 = plot->mapToPlot(rtMin, 0.0f).x();
    float x2 = plot->mapToPlot(rtMax, 0.0f).x();
    float yStart = _indexOfVisibleItem * _boxH;
    float yEnd = yStart + _boxH - _axesOffset;

    if (_leftMarker != nullptr)
        delete _leftMarker;
    _leftMarker = new QGraphicsLineItem(nullptr);
    scene()->addItem(_leftMarker);

    if (_rightMarker != nullptr)
        delete _rightMarker;
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

void GalleryWidget::_ensureCurrentItemIsVisible(bool topToBottom)
{
    qreal y = _indexOfVisibleItem * _boxH;
    qreal yMargin = -1.0;
    if (topToBottom)
        yMargin = (_nItemsVisible - 1) * _boxH;
    ensureVisible(0, y, _boxW, _boxH, 0, yMargin);
    plotIndexChanged(_indexOfVisibleItem);
    _drawBoundaryMarkers();
}

void GalleryWidget::showPlotFor(int index)
{
    int lastIndex = _indexOfVisibleItem;
    _indexOfVisibleItem = index;
    _ensureCurrentItemIsVisible(lastIndex - index < 0);
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
    // correct for current plot - assumes `pos` is defined in terms of viewport
    pos.setY((_indexOfVisibleItem * _boxH) + pos.y());
    auto rect = QRectF(pos.x() - 2.0f, pos.y() - 2.0f, 4.0f, 4.0f);
    if (_leftMarker->boundingRect().intersects(rect))
        return _leftMarker;
    if (_rightMarker->boundingRect().intersects(rect))
        return _rightMarker;
    return nullptr;
}

void GalleryWidget::_refillVisiblePlot(float x1, float x2)
{
    if (_plotItems.empty())
        return;

    auto plot = _plotItems.at(_indexOfVisibleItem);
    auto eic = _eics.at(_indexOfVisibleItem);

    // lambda: given an X-coordinate, converts it to the closest RT in `eic`
    auto toRt = [this, eic](float coordinate) {
        float width = static_cast<float>(_boxW);
        float offset = static_cast<float>(_axesOffset);
        float ratio = (coordinate - offset) / (width - offset);
        float approximatedRt = _minRt + (ratio * (_maxRt - _minRt));

        float closestRealRt = approximatedRt;
        float minDiff = numeric_limits<float>::max();
        for (size_t i = 0; i < eic->size(); ++i) {
            float rt = eic->rt[i];
            float diff = abs(rt - approximatedRt);
            if (diff < minDiff) {
                closestRealRt = rt;
                minDiff = diff;
            }
        }
        return closestRealRt;
    };

    float peakRtMin = toRt(x1);
    float peakRtMax = toRt(x2);
    _peakBounds[eic] = make_pair(peakRtMin, peakRtMax);

    plot->clearData();
    plot->addData(eic, _minRt, _maxRt, true, peakRtMin, peakRtMax);
    plot->setXBounds(_minRt, _maxRt);
    plot->setYBounds(_minIntensity, _maxIntensity);
    scene()->update();
}

void GalleryWidget::wheelEvent(QWheelEvent* event)
{
    if (_plotItems.size() == 0)
        return;

    if (event->modifiers() == Qt::ControlModifier) {
        if (event->delta() > 0) {
            _nItemsVisible = min(4, min(_plotItems.size() - 1,
                                        _nItemsVisible + 1));
        } else {
            _nItemsVisible = max(1, _nItemsVisible - 1);
        }
        int currentIndex = _indexOfVisibleItem;
        replot();
        showPlotFor(currentIndex);
    } else {
        if (event->delta() > 0) {
            _indexOfVisibleItem = max(0, _indexOfVisibleItem - 1);
        } else {
            _indexOfVisibleItem = min(_plotItems.size() - 1,
                                      _indexOfVisibleItem + 1);
        }
        _ensureCurrentItemIsVisible(event->delta() < 0);
    }
}

void GalleryWidget::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
    int currentIndex = _indexOfVisibleItem;
    replot();
    showPlotFor(currentIndex);
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

    auto plot = _plotItems.at(_indexOfVisibleItem);
    auto eic = _eics.at(_indexOfVisibleItem);
    float newRt = 0.0f;

    if (_markerBeingDragged == _leftMarker) {
        float x1 = mousePos.x();
        float x2 = _rightMarker->line().x1();
        if (x1 >= x2)
            return;
        _refillVisiblePlot(x1, x2);
        newRt = _peakBounds.at(eic).first;
    } else if (_markerBeingDragged == _rightMarker) {
        float x1 = _leftMarker->line().x1();
        float x2 = mousePos.x();
        if (x1 >= x2)
            return;
        _refillVisiblePlot(x1, x2);
        newRt = _peakBounds.at(eic).second;
    }

    float x = plot->mapToPlot(newRt, 0.0f).x();
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
