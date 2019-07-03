#ifndef SUGGEST_H
#define SUGGEST_H

#include "stable.h"

class QLineEdit;
class QTimer;
class QTreeWidget;
class Compound;
class Pathway;

class SuggestPopup : public QObject
{
     Q_OBJECT

public:
     SuggestPopup(QLineEdit *parent = 0);
     ~SuggestPopup();
     bool eventFilter(QObject *obj, QEvent *ev);

	 void addToHistory(QString,int);
	 QMap<QString,int> getHistory();

public Q_SLOTS:
	 void doSearch(QString needle);
     void doneCompletion();
     void preventSuggest();
	 void addToHistory();
	 void addToHistory(QString);
	 void setDatabase(QString db);

Q_SIGNALS:
	 void compoundSelected(Compound*);
	 void pathwaySelected(Pathway*);

private:
     QLineEdit *editor;
     QTreeWidget *popup;
     QTimer *timer;
	 QString _currentDatabase;
	 QMap<QString,int> searchHistory;


	void doSearchCompounds(QString needle);
	void doSearchPathways(QString needle);
	void doSearchHistory(QString needle);

	QHash<QString,float> scores;
    QHash<QString,Compound*> compound_matches;
    QHash<QString,Pathway*> pathway_matches;




};

#endif
