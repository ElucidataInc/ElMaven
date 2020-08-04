/***************************************************************************
 *   Copyright (C) 2008 by melamud,,,   *
 *   emelamud@princeton.edu   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifndef LIGANDWIDGET_H
#define LIGANDWIDGET_H

#include "stable.h"

//Added when merged with Maven776
#include <QtNetwork>
#include <QNetworkReply>
#include <QHash>

#include "datastructures/isotope.h"

Q_DECLARE_METATYPE(Isotope)

class QAction;
class QMenu;
class QTextEdit;
class MainWindow;
class Database;
class Compound;
class Adduct;

using namespace std;

class LigandWidget: public QDockWidget {
      Q_OBJECT

public:
    LigandWidget(MainWindow* parent);

    void setDatabaseNames();
    QString getDatabaseName();
    void clear() { treeWidget->clear(); }
    void showNext();
    void showLast();
	Compound* getSelectedCompound();
    void loadCompoundDBMzroll(QString fileName);
    void setHash();

    /**
     * @brief returns QTreeWidgetItem associated with specific compound
     * @param compound pointer to class Compound
     * @return QTreeWidgetItem pointer to class QTreeWidgetItem
     */
    

public Q_SLOTS: 
    void setCompoundFocus(Compound* c);
    void setDatabase(QString dbname, bool insertIsotopesAndAdducts = true);
    void setFilterString(QString s);
    void showMatches(QString needle);

    /**
     * @brief reset the color of rows
     */
    void resetColor();

    void saveCompoundList(QString fileName,QString dbname);
    void updateCurrentItemData();
	void matchFragmentation();

    void showTable(bool insertIsotopesAndAdducts = true);

    void markAsDone(QTreeWidgetItem* item, bool isProxy = false);

    /**
     * @brief Change the color of a compound's entry which is present in the
     * active peak table.
     * @param compound Pointer to a `Compound` object that is present.
     * @param isProxy Whether the compound is only a proxy for its other forms.
     */
    void markAsDone(Compound* compound, bool isProxy = false);

    void markAsDone(Compound* compound, Isotope isotope);
    void markAsDone(Compound* compound, Adduct* adduct);

    /**
     * @brief Refreshes the list of isotope and adduct child items under each
     * compound depending on the current set global state.
     */
    void updateIsotopesAndAdducts();

Q_SIGNALS:
    void urlChanged(QString url);
    void compoundFocused(Compound* c);
    void databaseChanged(QString dbname);
    void mzrollSetDB(QString dbname);

protected:
    void keyPressEvent(QKeyEvent *event);

private Q_SLOTS:
    void showLigand();
    void databaseChanged(int index);

private:

    QTreeWidget *treeWidget;
    QComboBox *databaseSelect;
    QToolButton *libraryButton;
    QLineEdit*  filterEditor;
    QPoint dragStartPosition;
    QHash<Compound *, QTreeWidgetItem *> compoundsHash;

    MainWindow* _mw;
    QString filterString;
    void readCompoundXML(QXmlStreamReader& xml, string dbname);

    QNetworkAccessManager* m_manager;

    int connectionId;
    QXmlStreamReader xml;

};

#endif
