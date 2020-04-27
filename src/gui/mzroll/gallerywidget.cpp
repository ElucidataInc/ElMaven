#include "gallerywidget.h"
#include "Compound.h"
#include "EIC.h"
#include "PeakDetector.h"
#include "Scan.h"
#include "eiclogic.h"
#include "eicwidget.h"
#include "globals.h"
#include "mainwindow.h"
#include "masscutofftype.h"
#include "mavenparameters.h"
#include "mzSample.h"
#include "statistics.h"
#include "tabledockwidget.h"
#include "tinyplot.h"

GalleryWidget::GalleryWidget(MainWindow* mw)
{
    this->_mainWindow = mw;

    setScene(new QGraphicsScene(this));
    scene()->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
    setObjectName("Gallery");
    setAlignment(Qt::AlignLeft | Qt::AlignTop);

    _boxW = 300;
    _boxH = 200;
    recursionCheck = false;
    _plotItems.clear();
    _nItemsVisible = 1;
    _indexItemVisible = 0;

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    horizontalScrollBar()->setEnabled(false);
    setStyleSheet("QWidget { border: none; }");

    connect(mw->getEicWidget(),
            &EicWidget::eicUpdated,
            this,
            &GalleryWidget::replot);
}

GalleryWidget::~GalleryWidget()
{
    if (scene() != NULL)
        delete (scene());
}

void GalleryWidget::addEicPlotsWithGroup(vector<EIC*> eics,
                                         PeakGroup* group)
{
    clear();
    sort(begin(eics), end(eics), [this](EIC* first, EIC* second) {
        return first->sample->getSampleOrder()
               < second->sample->getSampleOrder();
    });

    for (EIC* eic : eics) {
        if (eic == nullptr)
            continue;

        Peak* samplePeak = nullptr;
        if (group != nullptr) {
            for (auto& peak : group->peaks) {
                if (peak.getSample() == eic->sample) {
                    samplePeak = &peak;
                    break;
                }
            }
        }

        mzSlice& slice =
            _mainWindow->getEicWidget()->getParameters()->getMzSlice();
        if (group != nullptr)
            slice = group->getSlice();

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
        plot->setWidth(_boxW);
        plot->setHeight(_boxH);
        plot->addData(eic,
                      slice.rtmin,
                      slice.rtmax,
                      group != nullptr,
                      peakRtMin,
                      peakRtMax);
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
}

void GalleryWidget::replot()
{
    if (isVisible()) {
        if (recursionCheck == false) {
            recursionCheck = true;
            drawMap();
            recursionCheck = false;
        }
    }
}

void GalleryWidget::wheelEvent(QWheelEvent* event)
{
    if (_plotItems.size() == 0)
        return;

    if (event->modifiers() == Qt::ControlModifier) {
        if (event->delta() > 0) {
            _nItemsVisible = min(4, min(_plotItems.size(), _nItemsVisible + 1));
        } else {
            _nItemsVisible = max(1, _nItemsVisible - 1);
        }
        replot();
    } else {
        if (event->delta() > 0) {
            _indexItemVisible = max(0, _indexItemVisible - 1);
        } else {
            _indexItemVisible = min(_plotItems.size(), _indexItemVisible + 1);
        }
        _ensureVisible(event->delta() < 0);
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

    for (int i = 0; i < nItems; i++) {
        QGraphicsItem* item = _plotItems[i];
        int row = 0;
        int col = i;
        int xpos = row * _boxW;
        int ypos = col * _boxH;
        static_cast<TinyPlot*>(item)->setWidth(_boxW);
        static_cast<TinyPlot*>(item)->setHeight(_boxH);
        static_cast<TinyPlot*>(item)->setPos(xpos, ypos);
    }

    scene()->update();
    _indexItemVisible = 0;
    _ensureVisible();
}

void GalleryWidget::_ensureVisible(bool topToBottom)
{
    qreal y = _indexItemVisible * _boxH;
    qreal yMargin = -1.0;
    if (topToBottom)
        yMargin = (_nItemsVisible - 1) * _boxH;
    ensureVisible(0, y, _boxW, _boxH, 0, yMargin);
}

void GalleryWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        QGraphicsItem* item = itemAt(event->pos());
        if (item != NULL) {
            QVariant v = item->data(0);
            Compound* compound = v.value<Compound*>();
            if (compound != NULL && _mainWindow != NULL) {
                _mainWindow->setCompoundFocus(compound);
                return;
            }

            mzSlice slice = v.value<mzSlice>();
            if (_mainWindow != NULL) {
                _mainWindow->getEicWidget()->setMzSlice(slice);
                return;
            }
        }
    }
}

void GalleryWidget::resizeEvent(QResizeEvent* event)
{
    replot();
}

void GalleryWidget::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Right:
        break;
    case Qt::Key_Left:
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }
    scene()->update();
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

void GalleryWidget::contextMenuEvent(QContextMenuEvent* event)
{
    event->ignore();
    QMenu menu;

    QAction* copyImage = menu.addAction("Copy Image to Clipboard");
    connect(copyImage, SIGNAL(triggered()), SLOT(copyImageToClipboard()));

    QAction* print = menu.addAction("Print");
    connect(print, SIGNAL(triggered()), SLOT(print()));

    QAction* selectedAction = menu.exec(event->globalPos());
    scene()->update();
}
