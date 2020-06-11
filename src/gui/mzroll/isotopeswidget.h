#ifndef ISOTOPEWIDGETGUI_H
#define ISOTOPEWIDGETGUI_H

#include "stable.h"
#include "ui_isotopeswidget.h"

class QAction;
class QMenu;
class QTextEdit;
class BackgroundPeakUpdate;
class MainWindow;
class IsotopeDetection;
class IsotopeLogic;
class Isotope;
class Compound;
class mzSample;
class Peak;
class PeakGroup;

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
    void setPeakGroupAndMore(shared_ptr<PeakGroup> grp,
                             bool bookmarkflg = false);
	/**
	 * @brief sets compound, window title and formula
	**/
	void setCompound(Compound* compound);
	void setIonizationMode(int mode);
	void setClipboard();
	void setClipboard(PeakGroup* group);
    void setClipboard(QList<shared_ptr<PeakGroup> > &groups);
	void pullIsotopes(PeakGroup* group);
	void pullIsotopesForBarplot(PeakGroup* group);
    void updateIsotopicBarplot(shared_ptr<PeakGroup> grp);
	void updateIsotopicBarplot();
	/**
	 * @brief set peak and group if a specific peak is selected
	**/
    void peakSelected(Peak* peak, shared_ptr<PeakGroup> group);

	/**
	 * @brief Add sample names to sample drop-down list
	 * @details List is updated whenever a sample is loaded/deleted/checked/unchecked
	**/
	void updateSampleList();

    /**
     * @brief This slot is meant to be called when the isotopes list needs to be
     * updated based on changes to some external state. It will simply
     * repopulate the list for an already set peak.
     */
    void refreshForCurrentPeak();

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
	  void reset();
	  void populateByParentGroup(vector<Isotope> masslist, double parentMass);
      QString groupIsotopeMatrixExport(PeakGroup* group, bool includeSampleHeader); //TODO: Changed the structure of the function while merging isotopewidget

};

#endif
