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

#include "globals.h"
#include "stable.h"
#include "mainwindow.h"
#include "numeric_treewidgetitem.h"

//Added when merged with Maven776
#include <QtNetwork>
#include <QNetworkReply>

class QAction;
class QMenu;
class QTextEdit;
class MainWindow;
class Database;


using namespace std;

extern Database DB; 

class LigandWidget: public QDockWidget {
      Q_OBJECT

public:
    LigandWidget(MainWindow* parent);

    void setDatabaseNames();
    QString getDatabaseName();
    void clear() { treeWidget->clear(); }
    void showNext();
    void showLast();
    void setDatabaseAltered(QString dbame,bool altered);
	Compound* getSelectedCompound();
    void loadCompoundDBMzroll(QString fileName);

    /**
     * @brief returns QTreeWidgetItem associated with specific compound
     * @param compound pointer to class Compound
     * @return QTreeWidgetItem pointer to class QTreeWidgetItem
     */
    QTreeWidgetItem* getItem(Compound* compound);

public Q_SLOTS: 
    void setCompoundFocus(Compound* c);
    void setDatabase(QString dbname);
    void setFilterString(QString s);
    void showMatches(QString needle);
    void showGallery();

    /**
     * @brief reset the color of rows
     */
    void resetColor();

    void saveCompoundList();
    void saveCompoundListToPolly(QString fileName,QString dbname);
    void updateTable() { showTable(); }
    void updateCurrentItemData();
	void matchFragmentation();

    /**
     * @brief change the color of the compound which is present is peaks table
     * @param compound pointer to class Compound
     */
    void markAsDone(Compound* compound);


Q_SIGNALS:
    void urlChanged(QString url);
    void compoundFocused(Compound* c);
    void databaseChanged(QString dbname);
    void mzrollSetDB(QString dbname);

private Q_SLOTS:
    void showLigand();
    void showTable();
    void databaseChanged(int index);
    void readRemoteData(QNetworkReply* reply);
    void fetchRemoteCompounds();
    QList<Compound*> parseXMLRemoteCompounds();

private:

    QTreeWidget *treeWidget;
    QComboBox *databaseSelect;
    QToolButton *galleryButton;
    QToolButton *saveButton;
    QToolButton *loadButton;
    QLineEdit*  filterEditor;
    QPoint dragStartPosition;

    QHash<QString,bool>alteredDatabases;

    MainWindow* _mw;
    QString filterString;
    QTreeWidgetItem* addItem(QTreeWidgetItem* parentItem, string key , float value);
    QTreeWidgetItem* addItem(QTreeWidgetItem* parentItem, string key , string value);
    void readCompoundXML(QXmlStreamReader& xml, string dbname);

    QNetworkAccessManager* m_manager;

    int connectionId;
    QXmlStreamReader xml;

};

#endif
