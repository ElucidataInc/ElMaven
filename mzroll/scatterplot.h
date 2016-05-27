#ifndef SCATTERPLOT_H
#define SCATTERPLOT_H

#include "plotdock.h"
#include "comparesamplesdialog.h"

class CompareSamplesDialog;
class MainWindow;

class ScatterPlot: public PlotDockWidget {
public:
	ScatterPlot(QWidget* w);
	~ScatterPlot();
	void showSimilar(PeakGroup* g);
	void setTable(TableDockWidget* t);

public slots:
	void contrastGroups();
	void showSelectedGroups(QPointF a, QPointF b);

protected:
	void draw();
	void drawScatter(StatisticsVector<float> vecB, StatisticsVector<float> vecY,
			vector<PeakGroup*> groups);

private:
	QAction* showSimilarOptions;
	bool showSimilarFlag;
	TableDockWidget* _table;
	CompareSamplesDialog* compareSamplesDialog;

};

#endif

