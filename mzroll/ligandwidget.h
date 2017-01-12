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
    //Added when merged with Maven776 - Kiran
	Compound* getSelectedCompound();
    void loadCompoundDBMzroll(QString fileName);

public Q_SLOTS: 
    void setCompoundFocus(Compound* c);
    void setDatabase(QString dbname);
    void setFilterString(QString s);
    //Moved when merged with Maven776- Kiran
    void showMatches(QString needle);
    void showGallery();
    void saveCompoundList();
    void updateTable() { showTable(); }
    void updateCurrentItemData();
    //Added when merged with Maven776- Kiran
	void matchFragmentation();


Q_SIGNALS:
    void urlChanged(QString url);
    void compoundFocused(Compound* c);
    void databaseChanged(QString dbname);

private Q_SLOTS:
    void showLigand();
    void showTable();
    void databaseChanged(int index);
    //Added when merged with Maven776- Kiran
    void readRemoteData(QNetworkReply* reply);
    void fetchRemoteCompounds();
    QList<Compound*> parseXMLRemoteCompounds();

private:

    QTreeWidget *treeWidget;
    QComboBox *databaseSelect;
    QToolButton *galleryButton;
    QToolButton *saveButton;
    QToolButton *loadButton;
    //Added when merged with Maven776- Kiran
    QLineEdit*  filterEditor;
    QPoint dragStartPosition;

    QHash<QString,bool>alteredDatabases;

    MainWindow* _mw;
    QString filterString;
    QTreeWidgetItem* addItem(QTreeWidgetItem* parentItem, string key , float value);
    QTreeWidgetItem* addItem(QTreeWidgetItem* parentItem, string key , string value);
    void readCompoundXML(QXmlStreamReader& xml);

    //fetching of compounds from remote database
    //Added when merged with Maven776- Kiran
    QNetworkAccessManager* m_manager;

    int connectionId;
    QHttp http;
    QXmlStreamReader xml;

};

#endif
