#ifndef GALLERYWIDGET_H
#define GALLERYWIDGET_H

#include "stable.h"
#include "mzSample.h"
#include "mainwindow.h"
#include "tabledockwidget.h"
#include "statistics.h"


class GalleryWidget : public QGraphicsView
{
    Q_OBJECT

		public:
				GalleryWidget(MainWindow* mw);
                ~GalleryWidget();

		public Q_SLOTS: 
            void replot();
			void clear() { scene()->clear(); plotitems.clear(); }
			void addEicPlots(std::vector<PeakGroup*>& groups);
			void addEicPlots(std::vector<Compound*>&compounds);
			void addEicPlots(std::vector<mzSlice*>&slices);
			void addEicPlots(std::vector<mzLink>&links);
            void addIdividualEicPlots(std::vector<EIC*>& eics,PeakGroup* grp);
			void fileGallery(const QString& dir);

			// new features added - kiran
			void print();
            void copyImageToClipboard();
			
		private:
				MainWindow* mainwindow;
				QList<QGraphicsItem*> plotitems;
				int _rowSpacer;
				int _colSpacer;
				int _boxW;
				int _boxH;
				TinyPlot* addEicPlot(std::vector<EIC*>& eics);
				TinyPlot* addEicPlot(mzSlice& slice);


		protected:
				void drawMap();
				void resizeEvent ( QResizeEvent *event );
				void wheelEvent(QWheelEvent *event);
                void mousePressEvent(QMouseEvent *event);
				void keyPressEvent(QKeyEvent *event);
				bool recursionCheck;

				// new features added - kiran
				void contextMenuEvent(QContextMenuEvent * event);

};

#endif

