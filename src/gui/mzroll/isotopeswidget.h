#ifndef ISOTOPEWIDGETGUI_H
#define ISOTOPEWIDGETGUI_H

#include "stable.h"
#include "mzSample.h"
#include "mainwindow.h"
#include "background_peaks_update.h"
#include "ui_isotopeswidget.h"
#include "numeric_treewidgetitem.h"
#include "isotopelogic.h"

class QAction;
class QMenu;
class QTextEdit;
class MainWindow;

using namespace std;

class IsotopeWidget: public QDockWidget, public Ui_isotopesWidget {
Q_OBJECT

public:
	IsotopeWidget(MainWindow*);
	~IsotopeWidget();
	BackgroundPeakUpdate* workerThread;

 	BackgroundPeakUpdate* workerThreadBarplot;


public Q_SLOTS:
	void setCharge(double charge);
	void setFormula(QString f);
	void userChangedFormula(QString f);
	void computeIsotopes(string f);
	/**
	 * @brief sets necessary variables when a group is selected
	 * @details sets peakgroup, sample (1st sample if none are selected),
	 * scan, compound and formula for calculation of isotopes
	**/
	void setPeakGroupAndMore(PeakGroup* grp, bool bookmarkflg = false);
	/**
	 * @brief sets compound, window title and formula
	**/
	void setCompound(Compound* compound);
	void setIonizationMode(int mode);
	void setClipboard();
	void setClipboard(PeakGroup* group);
	void setClipboard(QList<PeakGroup*>& groups);
	void pullIsotopes(PeakGroup* group);
	void pullIsotopesForBarplot(PeakGroup* group);
	void updateIsotopicBarplot(PeakGroup* grp);
	void updateIsotopicBarplot();
	/**
	 * @brief set peak and group if a specific peak is selected
	**/
	void peakSelected(Peak* peak, PeakGroup* group);
	/**
	 * @brief Add sample names to sample drop-down list
	 * @details List is updated whenever a sample is loaded/deleted/checked/unchecked
	**/
	void updateSampleList();

private Q_SLOTS:
	void showInfo();
	void showTable();
	QString groupTextEport(PeakGroup* group);
	/**
	 * @brief Set private member sample to the selected sample in drop-down list
	**/
	void updateSelectedSample(int index);

private:
	  IsotopeLogic* isotopeParameters;
	  IsotopeLogic* isotopeParametersBarPlot;
	  IsotopeDetection* isotopeDetector;
      MainWindow* _mw;
	  bool bookmarkflag;
	  mzSample* _selectedSample;

	  void clearWidget();
	  void populateByParentGroup(vector<Isotope> masslist, double parentMass);
      QString groupIsotopeMatrixExport(PeakGroup* group, bool includeSampleHeader); //TODO: Changed the structure of the function while merging isotopewidget

};

#endif
