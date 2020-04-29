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

    _boxW = 300;
    _boxH = 200;
    recursionCheck = false;
    _plotItems.clear();
    _nItemsVisible = 1;
    _indexOfVisibleItem = 0;

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    horizontalScrollBar()->setEnabled(false);
}

GalleryWidget::~GalleryWidget()
{
    if (scene() != NULL)
        delete (scene());
}

void GalleryWidget::addEicPlots(PeakGroup* group, MavenParameters* mp)
{
    clear();
    if (group == nullptr || !group->hasSlice())
        return;

    const mzSlice& slice = group->getSlice();
    vector<EIC*> eics = PeakDetector::pullEICs(&slice, group->samples, mp);
    sort(begin(eics), end(eics), [this](EIC* first, EIC* second) {
        return first->sample->getSampleOrder()
               < second->sample->getSampleOrder();
    });

    for (EIC* eic : eics) {
        if (eic == nullptr)
            continue;

        Peak* samplePeak = nullptr;
        for (auto& peak : group->peaks) {
            if (peak.getSample() == eic->sample) {
                samplePeak = &peak;
                break;
            }
        }

        QColor color = QColor::fromRgbF(eic->sample->color[0],
                                        eic->sample->color[1],
                                        eic->sample->color[2],
                                        1.0);
        float peakRtMin = -1.0f;
        float peakRtMax = -1.0f;
        if (samplePeak != nullptr) {
            peakRtMin = samplePeak->rtmin;
            peakRtMax = samplePeak->rtmax;
        }

        TinyPlot* plot = new TinyPlot(nullptr, scene());
        plot->addData(eic,
                      slice.rtmin,
                      slice.rtmax,
                      true,
                      peakRtMin,
                      peakRtMax);
        plot->setWidth(_boxW);
        plot->setHeight(_boxH);
        plot->addDataColor(color);
        plot->setData(0, QVariant::fromValue(slice));
        plot->setTitle(tr("%1 (m/z: %2 — %3)")
                           .arg(eic->sample->sampleName.c_str())
                           .arg(eic->mzmin)
                           .arg(eic->mzmax));
        _plotItems << plot;
        scene()->addItem(plot);
    }

    if (_plotItems.size() > 0)
        replot();

    delete_all(eics);
}

void GalleryWidget::replot()
{
    if (recursionCheck == false) {
        recursionCheck = true;
        drawMap();
        recursionCheck = false;
    }
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
        replot();
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

        TinyPlot* item = static_cast<TinyPlot*>(_plotItems[row]);
        item->setWidth(_boxW);
        item->setHeight(_boxH);

        item->setPos(xpos, ypos);
    }

    scene()->update();
    _indexOfVisibleItem = 0;
    _ensureCurrentItemIsVisible();
}

void GalleryWidget::_ensureCurrentItemIsVisible(bool topToBottom)
{
    qreal y = _indexOfVisibleItem * _boxH;
    qreal yMargin = -1.0;
    if (topToBottom)
        yMargin = (_nItemsVisible - 1) * _boxH;
    ensureVisible(0, y, _boxW, _boxH, 0, yMargin);
    plotIndexChanged(_indexOfVisibleItem);
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

void GalleryWidget::resizeEvent(QResizeEvent* event)
{
    replot();
}

void GalleryWidget::contextMenuEvent(QContextMenuEvent* event)
{
    event->ignore();
    QMenu menu;

    QAction* copyImage = menu.addAction("Copy Image to Clipboard");
    connect(copyImage, SIGNAL(triggered()), SLOT(copyImageToClipboard()));

    QAction* print = menu.addAction("Print");
    connect(print, SIGNAL(triggered()), SLOT(print()));

    menu.exec(event->globalPos());
    scene()->update();
}
