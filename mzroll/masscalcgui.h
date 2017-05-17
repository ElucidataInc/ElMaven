#ifndef MASSCALCGUI_H
#define MASSCALCGUI_H

#include "stable.h"
#include "mainwindow.h"
#include "ui_masscalcwidget.h"
#include "mzMassCalculator.h"

class QAction;
class QTextEdit;
class MainWindow;
class MassCalculator;
class Database;

extern Database DB; 

class MassCalcWidget: public QDockWidget,  public Ui_MassCalcWidget {
      Q_OBJECT

public:
      MassCalcWidget(MainWindow* mw);
      ~MassCalcWidget();

protected:
        

public Q_SLOTS: 
 	  void setMass(float mz);
	  void setCharge(float charge);
	  void setPPM(float ppm);
      void compute();
	  QSet<Compound*> findMathchingCompounds(float mz, float ppm, float charge);
        QSet<Compound*> findMathchingCompounds(float mz, pair<string,double> pr, float charge);

private Q_SLOTS:
      void showCellInfo(int row, int col, int lrow, int lcol);
	  void getMatches();
      void showTable();
   
private:
      MainWindow* _mw;
      MassCalculator mcalc;
	  std::vector< MassCalculator::Match* > matches;
      std::vector<Compound*> sortedcompounds;

	  double _mz;
	  double _charge;
	  double _ppm;

      void pubChemLink(QString formula);
      void keggLink(QString formula);
      void setupSortedCompoundsDB();
      
};

#endif
