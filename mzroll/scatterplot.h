#ifndef SCATTERPLOT_H
#define SCATTERPLOT_H

#include "plotdock.h"
#include "comparesamplesdialog.h"

class CompareSamplesDialog;
class MainWindow;

class ScatterPlot: public PlotDockWidget  {
    Q_OBJECT

        enum  plotTypeEnum { scatter=0, flower=1 };

		public:
				ScatterPlot(QWidget* w);
                ~ScatterPlot();
				void showSimilar(PeakGroup* g);
				void setTable(TableDockWidget* t);

		public slots:
				void contrastGroups();
				void showSelectedGroups(QPointF a, QPointF b);
                void setPlotTypeScatter() { plotType=scatter;  draw(); }
                void setPlotTypeFlower() { plotType=flower;    draw(); }
                void showSimilarOnClick(bool t) { showSimilarFlag=t; }



		protected:
				void draw();
				void drawScatter(StatisticsVector<float>vecB,StatisticsVector<float>vecY, vector<PeakGroup*>groups);
                void drawFlower(vector<PeakGroup*>groups);
                void setupToolBar();

		private:
				QAction* showSimilarOptions;
				bool showSimilarFlag;
				TableDockWidget* _table;
				CompareSamplesDialog* compareSamplesDialog;

                plotTypeEnum plotType;

};

#endif

