#include "Compound.h"
#include "database.h"
#include "datastructures/mzSlice.h"
#include "globals.h"
#include "mzSample.h"
#include "Scan.h"
#include "numeric_treewidgetitem.h"
#include "suggest.h"

SuggestPopup::SuggestPopup(QLineEdit *parent): QObject(parent), editor(parent)
{
		popup = new QTreeWidget;
		popup->setWindowFlags(Qt::Popup);
		popup->setFocusPolicy(Qt::NoFocus);
		popup->setFocusProxy(parent);
		popup->setMouseTracking(true);

		popup->setColumnCount(2);
		popup->setUniformRowHeights(true);
		popup->setRootIsDecorated(false);
		popup->setEditTriggers(QTreeWidget::NoEditTriggers);
		popup->setSelectionBehavior(QTreeWidget::SelectRows);
		popup->setFrameStyle(QFrame::Box | QFrame::Plain);
		popup->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		popup->header()->hide();

		popup->installEventFilter(this);


		connect(popup, SIGNAL(itemClicked(QTreeWidgetItem*, int)), SLOT(doneCompletion()));

		timer = new QTimer(this);
		timer->setSingleShot(true);
		timer->setInterval(500);

		connect(editor, SIGNAL(textEdited(QString)), timer, SLOT(start()));
		connect(editor, SIGNAL(textEdited(QString)), this, SLOT(doSearch(QString)));
}

SuggestPopup::~SuggestPopup()
{
		delete popup;
}

void SuggestPopup::addToHistory(QString key, int count) {
//	qDebug() << "SuggestPopup: addToHistory() " << key << " " << count;
	searchHistory[key]=count;
}

void SuggestPopup::addToHistory(QString key) {
	if ( searchHistory.contains(key) ) {
	 	addToHistory(key, searchHistory.value(key)+1);
	} else  {
	 	addToHistory(key, 1);
	}
}

void SuggestPopup::setDatabase(QString db) {
	_currentDatabase =db;
}

void SuggestPopup::addToHistory() {
	 QString key = editor->text();
	 if(! key.isEmpty()) addToHistory(key);
}

QMap<QString,int> SuggestPopup::getHistory() { 
	return searchHistory;
}
bool SuggestPopup::eventFilter(QObject *obj, QEvent *ev)
{
		if (obj != popup)
				return false;

		if (ev->type() == QEvent::MouseButtonPress) {
				popup->hide();
				editor->setFocus();
				return true;
		}

		if (ev->type() == QEvent::KeyPress) {

				bool consumed = false;
				int key = static_cast<QKeyEvent*>(ev)->key();
				switch (key) {
						case Qt::Key_Enter:
						case Qt::Key_Return:
								doneCompletion();
								consumed = true;

						case Qt::Key_Escape:
								editor->setFocus();
								popup->hide();
								consumed = true;

						case Qt::Key_Up:
						case Qt::Key_Down:
						case Qt::Key_Home:
						case Qt::Key_End:
						case Qt::Key_PageUp:
						case Qt::Key_PageDown:
								break;

						default:
								popup->hide();
								editor->setFocus();
								editor->event(ev);
								break;
				}

				return consumed;
		}

		return false;
}

void SuggestPopup::doSearchCompounds(QString needle) { 
    QRegExp regexp(needle,Qt::CaseInsensitive,QRegExp::RegExp);
    if (!needle.isEmpty() && !regexp.isValid()) return;


    string currentDb = _currentDatabase.toStdString();

    for(unsigned int i=0;  i < DB.compoundsDB.size(); i++ ) {
        Compound* c = DB.compoundsDB[i];
        QString name(c->name().c_str() );
        QString formula(c->formula().c_str() );
        QString id(c->id().c_str() );

        if ( name.length()==0) continue;

        //location of match
        int index = regexp.indexIn(name);
        if ( index < 0) index = regexp.indexIn(id);
        if ( index < 0) continue;
        if ( c->db() != currentDb ) continue;

       // qDebug() << "Compound: " << name << " " << currentDb.c_str() ;

        //score
        float c1=0;
        if ( searchHistory.contains(name)) c1 = searchHistory.value(name);
        if ( c->db() == currentDb ) c1 += 10;
        if ( index == 0 ) c1 +=20;
        if ( formula == needle) c1 += 100;

        float c2=0;
        c2 = regexp.matchedLength();

        float c3=0;
        if ( c->expectedRt() > 0 ) c3=1;

        float score=1+c1+c2+c3;



        if ( !scores.contains(name) || scores[name] < score ) {
            scores[name]=score;
            compound_matches[name]=c;
        }
    }
}

void SuggestPopup::doSearchPathways(QString needle) { 
	QRegExp regexp(needle,Qt::CaseInsensitive,QRegExp::RegExp);
	if (!needle.isEmpty() && !regexp.isValid()) return;


	for(unsigned int i=0;  i < DB.pathwayDB.size(); i++ ) {
            Pathway* p = DB.pathwayDB[i];
            QString name(p->name.c_str() );
            QString id(p->id.c_str() );
            if ( name.length()==0) continue;
            if ( !(regexp.indexIn(name) >=0  || regexp.indexIn(id) >= 0) ) continue;
            //score
            float c1=0; float c2=0; float c3=5;
            if ( searchHistory.contains(name)) c1 = searchHistory.value(name);
            c2 = regexp.matchedLength();
            float score=1+c1+c2+c3;

            if ( !scores.contains(name) || scores[name] < score ) {
                scores[name]=score;
                pathway_matches[name]=p;
            }
        }
    }

void SuggestPopup::doSearchHistory(QString needle)  {
		QRegExp regexp(needle,Qt::CaseInsensitive,QRegExp::RegExp);
		if (!needle.isEmpty() && !regexp.isValid()) return;


		Q_FOREACH( QString name, searchHistory.keys() ) {
				if ( name.length()==0) continue;
				if ( scores.contains(name) ) continue;
				if ( name.contains(needle) || name.contains(regexp)){
						float c1= searchHistory.value(name);
						float c2= 1.00/name.length();
						float c3=0;
						float score=1+c1+c2+c3;
						scores[name]=score;
				}
		}
}

void SuggestPopup::doSearch(QString needle) 
{

                //minum length of needle in order to start matching
                if (needle.isEmpty() || needle.length() < 3 ) return;
		needle = needle.simplified();
		needle.replace(' ',".*");

		QRegExp regexp(needle,Qt::CaseInsensitive,QRegExp::RegExp);
		if (!needle.isEmpty() && !regexp.isValid()) return;

		QRegExp startsWithNumber("^\\d");
		if (startsWithNumber.indexIn(needle) == 0 ) return;

		popup->setUpdatesEnabled(false);
		popup->clear();
		scores.clear(); 
		compound_matches.clear();
		pathway_matches.clear();

		//doSearchCompounds(needle);
		//doSearchPathways(needle);
		doSearchHistory(needle);
		if ( scores.size() < 10 ) doSearchCompounds(needle);
		if ( scores.size() < 20 ) doSearchPathways(needle);

		 //sort hit list by score
		 QList< QPair<float,QString> > list;
		 Q_FOREACH( QString name, scores.keys() ) { list.append(qMakePair(1/scores[name],name)); }
		 qSort(list);

		//show popup
		int compoundMatches=0;
		int pathwayMatches=0;
                int historyMatches=0;

                QList<QPair<float, QString> >::ConstIterator it = list.constBegin();
                while (it != list.constEnd()) {
                    QString name = (*it).second;
                    float   score = (*it).first;
                    NumericTreeWidgetItem *item=NULL;

                    if( compound_matches.contains(name)) {
                        if (compoundMatches++ < 10) continue;
                        //qDebug() << "Compound: " << name << " score: " << score;
                        item = new NumericTreeWidgetItem(popup,CompoundType);
                        item->setData(0,Qt::UserRole,QVariant::fromValue(compound_matches[name]));
                        item->setIcon(0,QIcon(rsrcPath + "/molecule.png"));
                    } else if (pathway_matches.contains(name) ) {
                        if (pathwayMatches++ < 10) continue;
                        //qDebug() << "Pathway: " << name << " score: " << score;
                        item = new NumericTreeWidgetItem(popup,PathwayType);
                        item->setData(0,Qt::UserRole,QVariant::fromValue(pathway_matches[name]));
                        item->setIcon(0,QIcon(rsrcPath + "/pathway.png"));
                    }  else if (searchHistory.contains(name) ) {
                        if (historyMatches++ < 10 ) continue;
                        //qDebug() << "History: " << name << " score: " << score;
                        item = new NumericTreeWidgetItem(popup,0);
                        item->setIcon(0,QIcon(rsrcPath + "/featuredetect.png"));
                    } else {
                        //qDebug() << "Hmm: " << name << " score: " << score;
                        break;
                    }

                    if (item) {
                        item->setText(0,QString::number(1/score,'f',1));
                        item->setText(1,name);
                        item->setTextColor(0, Qt::gray);
                    }
                    ++it;
                }

		//const QPalette &pal = editor->palette();
		//QColor color = pal.color(QPalette::Disabled, QPalette::WindowText);
		popup->setUpdatesEnabled(true);
		popup->setSortingEnabled(true);
		popup->sortByColumn(0,Qt::DescendingOrder);
		popup->resizeColumnToContents(0);
		popup->resizeColumnToContents(1);
		
		int h = popup->sizeHintForRow(0) * (compoundMatches+pathwayMatches+historyMatches);
		popup->resize(popup->width(), h);
		popup->adjustSize();

		if (popup->topLevelItemCount() > 0 ) {
			popup->show();
			popup->setCurrentItem( popup->itemAt(0,0));
			popup->update();
			popup->setFocus();
		}
		//popup->hideColumn(0);
		popup->move(editor->mapToGlobal(QPoint(0, editor->height())));
}



void SuggestPopup::doneCompletion()
{
		timer->stop();
		popup->hide();
		QTreeWidgetItem *item = popup->currentItem();
		if (!item) return;

		editor->setFocus();
		editor->setText(item->text(1));

		QVariant v = item->data(0,Qt::UserRole);
		if ( item->type() == CompoundType ) {
        	Compound*  com =  v.value<Compound*>();
			if(com) Q_EMIT(compoundSelected(com)); 
		} else if ( item->type() == PathwayType ) {
        	Pathway*  pw =  v.value<Pathway*>();
			if(pw) Q_EMIT(pathwaySelected(pw)); 
		} else {
			QMetaObject::invokeMethod(editor, "returnPressed");
		}
		addToHistory(item->text(1));
}

void SuggestPopup::preventSuggest()
{
		timer->stop();
}
