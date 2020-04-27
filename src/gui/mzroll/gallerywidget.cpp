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

void GalleryWidget::addEicPlots(const mzSlice& slice)
{
    clear();
    vector<mzSample*> samples = _mainWindow->getVisibleSamples();
    if (samples.size() == 0)
        return;

    TinyPlot* plot = _addEicPlot(slice);
    if (plot)
        plot->setTitle(tr("m/z: %1 — %2").arg(slice.mzmin).arg(slice.mzmax));
    if (plot)
        plot->setData(0, QVariant::fromValue(slice));

    if (_plotItems.size() > 0)
        replot();
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

        QColor color = QColor::fromRgbF(eic->sample->color[0],
                                        eic->sample->color[1],
                                        eic->sample->color[2],
                                        1.0);

        mzSlice& slice =
            _mainWindow->getEicWidget()->getParameters()->getMzSlice();

        TinyPlot* plot = new TinyPlot(nullptr, scene());
        plot->setWidth(_boxW);
        plot->setHeight(_boxH);
        plot->addData(eic, slice.rtmin, slice.rtmax);
        plot->addDataColor(color);
        plot->setData(0, QVariant::fromValue(slice));
        plot->setTitle(tr("%1 (m/z: %2 — %3)")
                           .arg(eic->sample->sampleName.c_str())
                           .arg(eic->mzmin)
                           .arg(eic->mzmax));
        if (group) {
            for (int j = 0; j < group->peakCount(); j++) {
                if (group->peaks[j].getSample() == eic->getSample()) {
                    plot->addPoint(group->peaks[j].rt,
                                   group->peaks[j].peakIntensity);
                }
            }
        }
        _plotItems << plot;
        scene()->addItem(plot);
    }

    if (_plotItems.size() > 0)
        replot();
}

TinyPlot* GalleryWidget::_addEicPlot(const mzSlice& slice)
{
    vector<mzSample*> samples = _mainWindow->getVisibleSamples();
    if (samples.size() == 0)
        return NULL;

    vector<EIC*> eics = PeakDetector::pullEICs(&slice,
                                               samples,
                                               _mainWindow->mavenParameters);
    TinyPlot* plot = _addEicPlot(eics);
    delete_all(eics);
    return plot;
}

TinyPlot* GalleryWidget::_addEicPlot(vector<EIC*>& eics)
{
    if (eics.size() == 0)
        return NULL;

    TinyPlot* plot = new TinyPlot(nullptr, nullptr);
    plot->setWidth(_boxW);
    plot->setHeight(_boxH);

    sort(eics.begin(), eics.end(), EIC::compMaxIntensity);
    int insertCount = 0;
    for (EIC* eic : eics) {
        if (!eic)
            continue;
        if (eic->maxIntensity == 0)
            continue;
        QColor color = QColor::fromRgbF(eic->sample->color[0],
                                        eic->sample->color[1],
                                        eic->sample->color[2],
                                        1.0);

        plot->addData(eic);
        plot->addDataColor(color);
        plot->setTitle(tr("m/z: %1 — %2").arg(eic->mzmin).arg(eic->mzmax));

        insertCount++;
    }
    if (insertCount > 0) {
        scene()->addItem(plot);
        _plotItems << plot;
        return plot;
    } else {
        delete (plot);
        return NULL;
    }
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
    if (event->modifiers() != Qt::ControlModifier)
        return QGraphicsView::wheelEvent(event);

    if (event->delta() > 0) {
        if (_boxH * 0.8 > 50) {
            _boxH *= 0.8;
        } else {
            _boxH = 50;
        }
    } else {
        if (_boxH * 1.2 < height()) {
            _boxH *= 1.2;
        } else {
            _boxH = height();
        }
    }
    replot();
}

void GalleryWidget::drawMap()
{
    if (width() < 50 or height() < 50)
        return;

    int nItems = _plotItems.size();
    if (nItems == 0)
        return;

    _boxW = width();
    if (_boxH < 50)
        _boxH = 50;
    if (_boxH > height())
        _boxH = height();

    int sceneW = width();
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
    fitInView(0, 0, sceneW, _boxH, Qt::KeepAspectRatio);
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
