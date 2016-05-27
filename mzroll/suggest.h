#ifndef SUGGEST_H
#define SUGGEST_H

#include <qhash.h>
#include <qmap.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qstring.h>

#include "../libmaven/database.h"
#include "../libmaven/mzSample.h"

class QEvent;

class QLineEdit;
class QTimer;
class QTreeWidget;
extern Database DB;

class SuggestPopup: public QObject {
Q_OBJECT

public:
	SuggestPopup(QLineEdit *parent = 0);
	~SuggestPopup();
	bool eventFilter(QObject *obj, QEvent *ev);

	void addToHistory(QString, int);
	QMap<QString, int> getHistory();

public slots:
	void doSearch(QString needle);
	void doneCompletion();
	void preventSuggest();
	void addToHistory();
	void addToHistory(QString);
	void setDatabase(QString db);

signals:
	void compoundSelected(Compound*);
	void pathwaySelected(Pathway*);

private:
	QLineEdit *editor;
	QTreeWidget *popup;
	QTimer *timer;
	QString _currentDatabase;
	QMap<QString, int> searchHistory;

	void doSearchCompounds(QString needle);
	void doSearchPathways(QString needle);
	void doSearchHistory(QString needle);

	QHash<QString, float> scores;
	QHash<QString, Compound*> compound_matches;
	QHash<QString, Pathway*> pathway_matches;

};

#endif
