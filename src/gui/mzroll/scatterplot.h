#ifndef SCATTERPLOT_H
#define SCATTERPLOT_H

#include "plotdock.h"
#include "comparesamplesdialog.h"

class CompareSamplesDialog;
class MainWindow;

class ScatterPlot: public PlotDockWidget  {
    Q_OBJECT
        // updated enum values - Kiran
        enum  plotTypeEnum { SCATTERPLOT=0, FLOWRPLOT=1, PLSPLOT=2 };

		public:
				ScatterPlot(QWidget* w);
                ~ScatterPlot();
				void showSimilar(PeakGroup* g);
				void setTable(TableDockWidget* t);
                void setPeakTable(QWidget* w);
                vector<PeakGroup*> presentGroups;
                QToolButton *btnPeakTable;

		public Q_SLOTS:
				void contrastGroups();
                void deleteGroup();
				void showSelectedGroups(QPointF a, QPointF b);
                //New funtions defined - Kiran                
		void showSelectedGroupGallery(QPointF from, QPointF to);
                void setPlotTypeScatter() { plotType=SCATTERPLOT;  draw(); }
                void setPlotTypeFlower() { plotType=FLOWRPLOT;    draw(); }
                void setPlotTypePLS() { plotType=PLSPLOT;    draw(); }
                void showSimilarOnClick(bool t) { showSimilarFlag=t; }
                void showPeakTable();




		protected:
				void draw();
				void drawScatter(StatisticsVector<float>vecB,StatisticsVector<float>vecY, vector<PeakGroup*>groups);
                void drawFlower(vector<PeakGroup*>groups);
                //New funtions defined - Kiran   
                void drawPLS(vector<PeakGroup*>groups);
                void setupToolBar();
                //New funtions defined - Kiran   
                void keyPressEvent( QKeyEvent *e );
                QSet<PeakGroup*> getGroupsInRect(QPointF from, QPointF to);

		private:
				QAction* showSimilarOptions;
				bool showSimilarFlag;
				TableDockWidget* _table;
                TableDockWidget* _peakTable;
				CompareSamplesDialog* compareSamplesDialog;

                plotTypeEnum plotType;

};

#endif

