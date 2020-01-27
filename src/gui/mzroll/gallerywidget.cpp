#include "Compound.h"
#include "eiclogic.h"
#include "eicwidget.h"
#include "EIC.h"
#include "gallerywidget.h"
#include "globals.h"
#include "mainwindow.h"
#include "mavenparameters.h"
#include "masscutofftype.h"
#include "mzSample.h"
#include "PeakDetector.h"
#include "Scan.h"
#include "statistics.h"
#include "tabledockwidget.h"
#include "tinyplot.h"

GalleryWidget::GalleryWidget(MainWindow* mw) {
	this->mainwindow = mw;

    setScene(new QGraphicsScene(this));
    scene()->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
	setObjectName("Gallery");
	setAlignment(Qt::AlignLeft | Qt::AlignTop);

    _rowSpacer=1;
    _colSpacer=1;
	_boxW=300;
	_boxH=200;
	recursionCheck=false;
	plotitems.clear();

    connect(mw->getEicWidget(), &EicWidget::eicUpdated, this, &GalleryWidget::replot);
//	fileGallery("/home/melamud/samples/");

	/*
	for(int i=0; i<100; i++ ) {
//		QPen pen(Qt::black); pen.setWidth(2);
//	    QGraphicsRectItem* item = scene()->addRect(QRectF(0,0,_boxW,_boxH),pen,Qt::NoBrush);
//		plotitems << item;
		QVector<float>data(100);
		for(int i=0; i<100; i++) data[i] = pow((float) rand() / INT_MAX*100,10);

		QVector<float>data2(100);
		for(int i=0; i<100; i++) data2[i] = pow((float) rand() / INT_MAX*100,10);

		QVector<float>data3(100);
		for(int i=0; i<100; i++) data3[i] = pow((float) rand() / INT_MAX*100,10);


		TinyPlot* plot = new TinyPlot(0,scene());
		plot->setWidth(_boxW);
		plot->setHeight(_boxH);
		plot->addData(data);
		plot->addData(data2);
		plot->addData(data3);
		plot->setTitle(tr("This a title %1").arg(i));
		plotitems << plot;
	}
	*/
}


GalleryWidget::~GalleryWidget() {
  if (scene()!=NULL) delete(scene());
}

void GalleryWidget::fileGallery(const QString &fromDir) {

		QDir d(fromDir);
		if (!d.exists()) return;
		//qDebug() << "GalleryWidget::fileGallery: dir=" <<  fromDir << " exists=" << d.exists();

		QStringList filters; //filters << "*.mzXML" << "*.mzCSV" << "*.raw" << "*.mzData" << "." << "..";
		QFileInfoList list = d.entryInfoList( filters,
										QDir::Files | QDir::Dirs | QDir::NoSymLinks  | QDir::Readable,
										QDir::Name | QDir::DirsFirst | QDir::IgnoreCase);
		if (list.size() == 0) return;
		int fileCount=0;

		Q_FOREACH ( QFileInfo fi, list ) {
				QString filename = fi.fileName();
				if (filename == "." || filename == "..") continue;

				if (fi.isDir()) {
		//			qDebug() << "GalleryWidget::fileGallery: dir=" << fi.fileName();
					fileGallery(fi.absoluteFilePath()); //recurse
				} else if (fi.isFile()) {
					if ( 	filename.contains("mzXML",Qt::CaseSensitive) ||
							filename.contains("mzData",Qt::CaseSensitive) ||
							filename.contains("mzCSV",Qt::CaseSensitive) ||
							filename.contains("mzML",Qt::CaseSensitive) ||
							filename.contains("raw",Qt::CaseSensitive)
							) {
						fileCount++;
		//				qDebug() << "\tGalleryWidget::fileGallery: file=" << fi.fileName();
					}
				}
		}

}

void GalleryWidget::addEicPlots(std::vector<Compound*>&compounds) {

	MassCalculator  mcalc;
	MassCutoff *massCutoff=mainwindow->getUserMassCutoff();
	int   ionizationMode=mainwindow->getIonizationMode();

	std::vector<mzSample*>samples = mainwindow->getVisibleSamples();
	if (samples.size() == 0 ) return;

	for (unsigned int i=0; i < compounds.size();  i++ ) {
		Compound* c = compounds[i];
		if ( c == NULL ) continue;

		mzSlice slice;
		slice.compound = c;
		slice.rtmin = 0;
		slice.rtmax = 1e9;
                if (!c->srmId().empty()) slice.srmId=c->srmId();

        if (!c->formula().empty()) {
            int charge = mainwindow->mavenParameters->getCharge(c);
            double mass = mcalc.computeMass(c->formula(), charge);
            double massCutoffW = massCutoff->massCutoffValue(mass);
            slice.mzmin = mass-massCutoffW;
            slice.mzmax = mass+massCutoffW;
		}

    /*	if (c->expectedRt > 0 ) {
			slice.rtmin = c->expectedRt-2.0;
			slice.rtmax = c->expectedRt+2.0;
		}
    */
		TinyPlot* plot = addEicPlot(slice);
                if(plot) plot->setTitle(QString(c->name().c_str()));
		if(plot) plot->setData(0, QVariant::fromValue(c));
	}
	if (plotitems.size() > 0) replot();
}


void GalleryWidget::addEicPlots(std::vector<PeakGroup*>&groups) {
	if (groups.size() == 0) return;

    //for multiple groups. find the widest retention time window
    float rtmin=groups[0]->minRt;
    float rtmax=groups[0]->maxRt;
     for(int i=1; i < groups.size(); i++ ) {
         if(groups[i]->minRt<rtmin) rtmin=groups[i]->minRt;
         if(groups[i]->maxRt>rtmax) rtmax=groups[i]->maxRt;
     }

	//qDebug() << "GalleryWidget::addEicPlots(groups) ";
	for(int i=0; i < groups.size(); i++ ) {
		PeakGroup* group = groups[i];
		if (group == NULL) continue;

		float minmz = group->minMz;
		float maxmz = group->maxMz;
		Compound* c = group->getCompound();

   		mzSlice slice(minmz,maxmz,rtmin,rtmax);
		if(c) slice.compound = c;
                if(c && !c->srmId().empty()) slice.srmId=c->srmId();

		TinyPlot* plot = addEicPlot(slice);
		if ( plot ) {
                        plot->setTitle(QString(group->getName().c_str()));
			plot->setData(0, QVariant::fromValue(slice));

			for(int j=0; j < group->peakCount(); j++ ) {
				plot->addPoint(group->peaks[j].rt, group->peaks[j].peakIntensity);
			}
		}
	}
	replot();
}


void GalleryWidget::addEicPlots(std::vector<mzLink>&links) {

	clear();

	std::vector<mzSample*>samples = mainwindow->getVisibleSamples();
	if (samples.size() == 0 ) return;

	MassCutoff *compoundMassCutoff=mainwindow->getUserMassCutoff();
        mzSlice& current = mainwindow->getEicWidget()->getParameters()->getMzSlice();

	sort(links.begin(), links.end(), mzLink::compCorrelation);

	for (unsigned int i=0; i < links.size();  i++ ) {
		mzLink link = links[i];
   		mzSlice slice;
		float mass = link.mz2;
		slice.mzmin = mass - compoundMassCutoff->massCutoffValue(mass);
		slice.mzmax = mass + compoundMassCutoff->massCutoffValue(mass);
		slice.rtmin = current.rtmin;
		slice.rtmax = current.rtmax;
		TinyPlot* plot = addEicPlot(slice);
		if(plot) plot->setTitle(QString(link.note.c_str()));
		if(plot) plot->setData(0, QVariant::fromValue(slice));

	}
	if (plotitems.size() > 0) replot();
}


void GalleryWidget::addIdividualEicPlots(std::vector<EIC*>&eics, PeakGroup*group) {

    clear();

    for (unsigned int i=0; i < eics.size();  i++ ) {
        EIC* eic = eics[i];
        if(!eic) continue;
        QColor color = QColor::fromRgbF( eic->sample->color[0], eic->sample->color[1], eic->sample->color[2], 1.0 );

        mzSlice& slice =  mainwindow->getEicWidget()->getParameters()->getMzSlice();

        TinyPlot* plot = new TinyPlot(0,scene());
        plot->setWidth(_boxW);
        plot->setHeight(_boxH);
        plot->addData(eic,slice.rtmin,slice.rtmax);
        plot->addDataColor(color);
        plot->setData(0,QVariant::fromValue(slice));
        plot->setTitle(tr("%1 mz: %1-%2")
                       .arg(eic->sample->sampleName.c_str())
                       .arg(eic->mzmin)
                       .arg(eic->mzmax));
        if(group) {
            for(int j=0; j < group->peakCount(); j++ ) {
                if(group->peaks[j].getSample() ==  eic->getSample() ) {
                    plot->addPoint(group->peaks[j].rt, group->peaks[j].peakIntensity);
                }
            }
        }
        plotitems << plot;
        scene()->addItem(plot);
    }

    if (plotitems.size() > 0) replot();
}

void GalleryWidget::addEicPlots(std::vector<mzSlice*>&slices) {
	clear();
	std::vector<mzSample*>samples = mainwindow->getVisibleSamples();
	if (samples.size() == 0 ) return;

	for (unsigned int i=0; i < slices.size();  i++ ) {
		if (slices[i] == NULL ) continue;
   		mzSlice slice = *slices[i];
		TinyPlot* plot = addEicPlot(slice);
		if(plot) plot->setTitle(tr("%1-%2").arg(slice.mzmin).arg(slice.mzmax));
		if(plot) plot->setData(0, QVariant::fromValue(slice));
	}
	if (plotitems.size() > 0) replot();
}



TinyPlot* GalleryWidget::addEicPlot(mzSlice& slice) {
	std::vector<mzSample*>samples = mainwindow->getVisibleSamples();
	if (samples.size() == 0 ) return NULL;
    QSettings* settings = mainwindow->getSettings();

    vector<EIC*> eics = PeakDetector::pullEICs(&slice,
                                               samples,
                                               mainwindow->mavenParameters);
    TinyPlot* plot = addEicPlot(eics);
    delete_all(eics);
    return plot;
}

TinyPlot* GalleryWidget::addEicPlot(std::vector<EIC*>& eics) {
	if(eics.size() == 0 ) return NULL;

	TinyPlot* plot = new TinyPlot(0,0);
	plot->setWidth(_boxW);
	plot->setHeight(_boxH);

	std::sort(eics.begin(), eics.end(), EIC::compMaxIntensity);
	int insertCount=0;
	for(int i=0; i < eics.size(); i++ ) {
		EIC* eic = eics[i];
		if(!eic) continue;
		if (eic->maxIntensity == 0 ) continue;
        QColor color = QColor::fromRgbF( eic->sample->color[0], eic->sample->color[1], eic->sample->color[2], 1.0 );

		plot->addData(eic);
		plot->addDataColor(color);
		plot->setTitle(tr("mz: %1-%2").arg(eics[i]->mzmin).arg(eics[i]->mzmax));

		insertCount++;
	}
	if (insertCount > 0 ) {
		scene()->addItem(plot);
		plotitems << plot;
		return plot;
	} else {
		delete(plot);
		return NULL;
	}
}



void GalleryWidget::replot() {
	qDebug() << "GalleryWidget::replot() ";

    if(isVisible()){
        if (recursionCheck == false ) {
            recursionCheck=true;
            drawMap();
            recursionCheck=false;
        }
    }
}

void GalleryWidget::wheelEvent(QWheelEvent *event) {
	    qDebug() << "GalleryWidget::wheelEvent() ";
		if ( event->delta() > 0 ) {
            if(_boxH*0.8>50) {
                 _boxH *= 0.8; replot();
            }
		} else {
            if(_boxH*1.2<height()){
                 _boxH *= 1.2; replot();
            }
		}
}

void GalleryWidget::drawMap()
{
    // gallery widget is too small
    if (width() < 50 or height() < 50) {
        return;
    }

    int nItems = plotitems.size();
    if (nItems == 0)
        return;

    _boxW = width() - 30;
    if (_boxH < 50)
        _boxH = 50;
    if (_boxH > height()) {
        _boxH = height();
    }

    //_rowSpacer = _boxH/5;
    // if  (_rowSpacer < 1) _rowSpacer=1;
    // if  (_colSpacer < 1) _colSpacer=1;

    // int nItemPerRow=width()/(_boxW+_colSpacer);
    // if  (nItemPerRow < 1 ) nItemPerRow=1;
    int nItemPerRow = 1;
    int nRows = ceil(nItems / nItemPerRow);
    if (nRows < 1)
        nRows = 1;

    int sceneW = width() - 30;
    int sceneH = nRows * (_boxH + _rowSpacer) + _boxH + 100;
    setSceneRect(0, 0, sceneW, sceneH);

    /*
        cerr << "GalleryWidget::drawMap() " << nItemPerRow << " " <<
       _boxW+_rowSpacer << endl; cerr << "GalleryWidget::drawMap() width()=" <<
       sceneW << endl; cerr << "GalleryWidget::drawMap() nItems=" << nItems <<
       endl; cerr << "GalleryWidget::drawMap() nItemPerRow=" <<  nItemPerRow <<
       endl; cerr << "GalleryWidget::drawMap() nRows=" <<  nRows << endl;
*/
    for (int i = 0; i < nItems; i++) {
        QGraphicsItem* item = plotitems[i];
        int row = i % nItemPerRow;
        int col = i / nItemPerRow;
        int xpos = row * (_colSpacer + _boxW);
        int ypos = col * (_rowSpacer + _boxH);
        ((TinyPlot*)item)->setWidth(_boxW);
        ((TinyPlot*)item)->setHeight(_boxH);
        ((TinyPlot*)item)->setPos(xpos, ypos);
        //((QGraphicsRectItem*)item)->setRect(xpos,ypos,_boxW,_boxH);
    }

    scene()->update();
    fitInView(0, 0, sceneW, _boxH + _rowSpacer + 50, Qt::KeepAspectRatio);
}

void GalleryWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QGraphicsItem* item = itemAt(event->pos());
        if ( item != NULL )  {
			/*
			QVariant v = item->data(0);
   			PeakGroup*  group =  v.value<PeakGroup*>();
            if (group != NULL && mainwindow != NULL) {
                mainwindow->setPeakGroup(group);
            }
			*/
			QVariant v = item->data(0);
   			Compound*  compound =  v.value<Compound*>();
            if (compound != NULL && mainwindow != NULL) {
                mainwindow->setCompoundFocus(compound);
				return;
            }

			mzSlice slice =  v.value<mzSlice>();
            if (mainwindow != NULL) {
                mainwindow->getEicWidget()->setMzSlice(slice);
				return;
            }

        }
	}
}

void GalleryWidget::resizeEvent ( QResizeEvent * event ) {

    replot();
}

void GalleryWidget::keyPressEvent(QKeyEvent *event)
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

// new features added - Kiran
void GalleryWidget::print() {
    QPrinter printer;
    QPrintDialog dialog(&printer);

    if ( dialog.exec() ) {
        printer.setOrientation(QPrinter::Landscape);;
        QPainter painter;
        if (! painter.begin(&printer)) { // failed to open file
            qWarning("failed to open file, is it writable?");
            return;
        }
        render(&painter);
        painter.end();
    }
}

// new features added - Kiran
void GalleryWidget::copyImageToClipboard() {
    QPixmap image(this->width(),this->height());
    image.fill(Qt::white);
    QPainter painter;
    painter.begin(&image);
    render(&painter);
    painter.end();
    QApplication::clipboard()->setPixmap(image);
}

// new features added - Kiran
void GalleryWidget::contextMenuEvent(QContextMenuEvent * event) {
 //qDebug <<"EicWidget::contextMenuEvent(QContextMenuEvent * event) ";

    event->ignore();
    QMenu menu;

    QAction* copyImage = menu.addAction("Copy Image to Clipboard");
    connect(copyImage, SIGNAL(triggered()), SLOT(copyImageToClipboard()));

    QAction* print = menu.addAction("Print");
    connect(print, SIGNAL(triggered()), SLOT(print()));

    QAction *selectedAction = menu.exec(event->globalPos());
    scene()->update();
}
