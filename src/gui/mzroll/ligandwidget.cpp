#include "Compound.h"
#include "ligandwidget.h"
#include "alignmentdialog.h"
#include "analytics.h"
#include "globals.h"
#include "mainwindow.h"
#include "masscalcgui.h"
#include "mavenparameters.h"
#include "mzfileio.h"
#include "mzSample.h"
#include "numeric_treewidgetitem.h"
#include "Scan.h"
#include "settingsform.h"
#include "spectramatching.h"

using namespace std;

LigandWidget::LigandWidget(MainWindow* mw) {
  _mw = mw;
 
  setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  setFloating(false);
  setObjectName("Compounds");

  treeWidget=new QTreeWidget(this);
  treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
  treeWidget->setSortingEnabled(false);
  treeWidget->setColumnCount(3);
  treeWidget->setRootIsDecorated(false);
  treeWidget->setUniformRowHeights(true);
  treeWidget->setHeaderHidden(false);
  treeWidget->setObjectName("CompoundTable");
  treeWidget->setDragDropMode(QAbstractItemView::DragOnly);
  treeWidget->setMouseTracking(true);

  connect(treeWidget, SIGNAL(itemSelectionChanged()), SLOT(showLigand()));
  connect(treeWidget,
          SIGNAL(itemClicked(QTreeWidgetItem*, int)),
          SLOT(showLigand()));

  QToolBar *toolBar = new QToolBar(this);
  toolBar->setFloatable(false);
  toolBar->setMovable(false);

  databaseSelect = new QComboBox(toolBar);
  databaseSelect->setObjectName(QString::fromUtf8("databaseSelect"));
  databaseSelect->setDuplicatesEnabled(false);
  databaseSelect->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::MinimumExpanding);


  loadButton = new QToolButton(toolBar);
  loadButton->setIcon(QIcon(rsrcPath + "/fileopen.png"));
  loadButton->setToolTip("Load Custom Compound List");

  connect(loadButton, &QToolButton::clicked, [this]()
  {
    _mw->getAnalytics()->hitEvent("Load Compound DB",
                                  "Custom Compound DB");
  });
  connect(loadButton,SIGNAL(clicked()), mw, SLOT(loadCompoundsFile()));
  connect(this, SIGNAL(compoundFocused(Compound*)), mw, SLOT(setCompoundFocus(Compound*)));
  connect(this, SIGNAL(urlChanged(QString)), mw, SLOT(setUrl(QString)));

  toolBar->addWidget(databaseSelect);
  toolBar->addWidget(loadButton);

  //Feature updated when merging with Maven776- Filter out compounds based on a keyword.
  filterEditor = new QLineEdit(toolBar);
  filterEditor->setPlaceholderText("Compound Name Filter");
  connect(filterEditor, SIGNAL(textEdited(QString)), this, SLOT(showMatches(QString)));

  QWidget *window = new QWidget;
  QVBoxLayout *layout = new QVBoxLayout;
  layout->setSpacing(0);
  layout->addWidget(filterEditor);
  layout->addWidget(treeWidget);
  window->setLayout(layout);

  setWidget(window);
  setTitleBarWidget(toolBar);
  setWindowTitle("Compounds");






  //disconnect(&http, SIGNAL(readyRead(const QHttpResponseHeader &)));
  //connect(&http, SIGNAL(readyRead(const QHttpResponseHeader &)), SLOT(readRemoteData(const QHttpResponseHeader &)));
  
  // Fetches and reads compounds from a remote location when fetch button is clicked - Kiran
  disconnect(_mw->settingsForm->fetchCompounds,SIGNAL(clicked()));
  connect(_mw->settingsForm->fetchCompounds,SIGNAL(clicked()),this,SLOT(fetchRemoteCompounds()));
  connect(this, SIGNAL(mzrollSetDB(QString)), _mw, SLOT(mzrollLoadDB(QString)));

  m_manager = new QNetworkAccessManager(this);
  connect(m_manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(readRemoteData(QNetworkReply*)));

  //get list of methods from central database
  //http://data_server_url?action=fetchcompounds&format=xml
  //fetchRemoteCompounds();

  QDirIterator itr(":/databases/");

  while(itr.hasNext()) {
    auto filename = itr.next().toStdString();
    DB.loadCompoundCSVFile(filename);
    string dbname = mzUtils::cleanFilename(filename);
    _mw->massCalcWidget->database->addItem(QString::fromStdString(dbname));
  }

  QSet<QString>set;
  for(int i=0; i< DB.compoundsDB.size(); i++) {
      if (! set.contains( DB.compoundsDB[i]->db.c_str() ) )
          set.insert( DB.compoundsDB[i]->db.c_str() );
  }

  QIcon icon(rsrcPath + "/dbsearch.png");
  QSetIterator<QString> i(set);
  while (i.hasNext())
      databaseSelect->addItem(icon,i.next());

  connect(this, SIGNAL(databaseChanged(QString)), _mw, SLOT(showSRMList()));
  connect(databaseSelect, SIGNAL(currentIndexChanged(QString)), this, SLOT(setDatabase(QString)));

}
QString LigandWidget::getDatabaseName() {
	return databaseSelect->currentText();
}

void LigandWidget::setDatabaseNames() {
    // TODO: do not setup signals and slots here. do it only once in the constructor.
    // just add the new db in set and add it in the combo box
	databaseSelect->disconnect(SIGNAL(currentIndexChanged(QString)));
	databaseSelect->clear();
	QSet<QString>set;
	for(int i=0; i< DB.compoundsDB.size(); i++) {
            if (! set.contains( DB.compoundsDB[i]->db.c_str() ) )
                set.insert( DB.compoundsDB[i]->db.c_str() );
	}

        QIcon icon(rsrcPath + "/dbsearch.png");
        QSetIterator<QString> i(set);
        int pos=0;
	while (i.hasNext()) { 
                //databaseSelect->insertItem(pos++, i.next());
                databaseSelect->addItem(icon,i.next());
	}
	connect(databaseSelect, SIGNAL(currentIndexChanged(QString)), this, SLOT(setDatabase(QString)));
	connect(databaseSelect, SIGNAL(currentIndexChanged(QString)), _mw->alignmentDialog, SLOT(setDatabase(QString)));    
}

QTreeWidgetItem* LigandWidget::addItem(QTreeWidgetItem* parentItem, string key , float value) {
	QTreeWidgetItem *item = new QTreeWidgetItem(parentItem);
	item->setText(0, QString(key.c_str()));
    item->setText(1, QString::number(value,'f',3));
	return item;
}

QTreeWidgetItem* LigandWidget::addItem(QTreeWidgetItem* parentItem, string key , string value) {
	QTreeWidgetItem *item = new QTreeWidgetItem(parentItem);
	item->setText(0, QString(key.c_str()));
	item->setText(1, QString(value.c_str()));
	return item;
}

void LigandWidget::loadCompoundDBMzroll(QString fileName) {


    QFile data(fileName);
    if ( !data.open(QFile::ReadOnly) ) {
        QErrorMessage errDialog(this);
        errDialog.showMessage("File open: " + fileName + " failed");
        return;
    }

    QXmlStreamReader xml(&data);

    string dbname = "";
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
            if (xml.name() == "database") {
                dbname = xml.attributes().value("name").toString().toStdString();
            }
            if (xml.name() == "compound") { readCompoundXML(xml, dbname); }
        }
     }
     sort(DB.compoundsDB.begin(),DB.compoundsDB.end(), Compound::compMass);

     Q_EMIT(mzrollSetDB( QString::fromStdString(dbname)));
}
        
void LigandWidget::readCompoundXML(QXmlStreamReader& xml, string dbname) {
    if(dbname == "") return;
    string id, name, formula;
    float rt=0;
    float mz=0;
    float charge=0;
    float collisionenergy=0;
    float precursormz=0;
    float productmz=0;
    vector<string>categorylist;

    id = xml.attributes().value("id").toString().toStdString();
    name = xml.attributes().value("name").toString().toStdString();
    mz = xml.attributes().value("mz").toString().toFloat();
    rt = xml.attributes().value("rt").toString().toFloat();
    charge = xml.attributes().value("Charge").toString().toInt();
    formula = xml.attributes().value("Formula").toString().toStdString();
    precursormz = xml.attributes().value("precursorMz").toString().toFloat();
    productmz = xml.attributes().value("productMz").toString().toFloat();
    collisionenergy = xml.attributes().value("collisionEnergy").toString().toFloat();

    while(xml.readNextStartElement()) {
        if (xml.name() == "categories") {
            Q_FOREACH(const QXmlStreamAttribute &attr, xml.attributes()) {
                if(categorylist.size() == 0) categorylist.push_back(xml.attributes().value(attr.name().toString()).toString().toStdString());
            }
        }
    }

    Compound* compound = new Compound(id,name,formula,charge);
    compound->expectedRt = rt;
    compound->mass = mz;
    compound->db = dbname;
    compound->precursorMz=precursormz;
    compound->productMz=productmz;
    compound->collisionEnergy=collisionenergy;
    for(int i=0; i < categorylist.size(); i++) compound->category.push_back(categorylist[i]);
    DB.addCompound(compound);

}

void LigandWidget::setDatabase(QString dbname) {
    int currentIndex = databaseSelect->currentIndex();
    int index = databaseSelect->findText(dbname,Qt::MatchExactly);
    if (index != -1 ) {
        databaseSelect->setCurrentIndex(index);
        _mw->fileLoader->insertSettingForSave("mainWindowSelectedDbName",
                                              variant(dbname.toStdString()));
    }

    _mw->getSettings()->setValue("lastCompoundDatabase", getDatabaseName());
    Q_EMIT databaseChanged(getDatabaseName());
    showTable();
}

void LigandWidget::databaseChanged(int index) {
    QString dbname = databaseSelect->currentText();
    setDatabase(dbname);
}

void LigandWidget::setCompoundFocus(Compound* c) {
	if (c==NULL) return;
	QString dbname(c->db.c_str());
	int index = databaseSelect->findText(dbname,Qt::MatchExactly);
        if (index != -1 ) databaseSelect->setCurrentIndex(index);

	QString filterString(c->name.c_str());
	setWindowTitle("Compounds: " + filterString);
	showTable();
}

void LigandWidget::setFilterString(QString s) {
	if(s != filterString) {
		filterString=s;
        filterEditor->setText(filterString);
        //showMatches(s);
	}
    //if (s.length() >= 4 ) { showMatches(s); }
}

void LigandWidget::showMatches(QString needle) {

    QRegExp regexp(needle,Qt::CaseInsensitive,QRegExp::RegExp);
    if(! regexp.isValid())return;

    QTreeWidgetItemIterator itr(treeWidget);
    while (*itr) {
        QTreeWidgetItem* item =(*itr);
        QVariant v = item->data(0,Qt::UserRole);
        Compound*  compound =  v.value<Compound*>();
        if (compound) {
                item->setHidden(true);
                if (needle.isEmpty()) {
                   item->setHidden(false);
                } else if ( item->text(0).contains(regexp) ){
                   item->setHidden(false);
                } else {
                    QStringList stack;
                    stack << compound->name.c_str()
                          << compound->id.c_str()
                          << compound->formula.c_str();

                    if( compound->category.size()) {
                        for(int i=0; i < compound->category.size(); i++) {
                            stack << compound->category[i].c_str();
                        }
                    }

                    Q_FOREACH( QString x, stack) {
                        if (x.contains(regexp)) {
                             item->setHidden(false); break;
                        }
                    }
                }
        }
        ++itr;
    }
}


void LigandWidget::updateCurrentItemData() {
    QTreeWidgetItem* item = treeWidget->selectedItems().first();
    if (!item) return;
    QVariant v = item->data(0,Qt::UserRole);
    Compound*  c =  v.value<Compound*>();
    if(!c) return;

    QString mass = QString::number(c->mass);
    QString rt = QString::number(c->expectedRt);
    item->setText(1,mass);
    item->setText(2,rt);

    if (c->hasGroup() ) {
        item->setIcon(0,QIcon(":/images/link.png"));
    } else {
        item->setIcon(0,QIcon());
    }
}


void LigandWidget::showTable() { 
    //	treeWidget->clear();
    treeWidget->clear();
    treeWidget->setColumnCount(4);
    QStringList header;
    header << "name" << "m/z" << "rt" << "category" << "notes";
    treeWidget->setHeaderLabels( header );
    treeWidget->setSortingEnabled(false);

    string dbname = databaseSelect->currentText().toStdString();
    cerr << "ligandwidget::showTable() " << dbname << endl;

    for(unsigned int i=0;  i < DB.compoundsDB.size(); i++ ) {
        Compound* compound = DB.compoundsDB[i];
        if(compound->db != dbname ) continue; //skip compounds from other databases
        NumericTreeWidgetItem *parent  = new NumericTreeWidgetItem(treeWidget,CompoundType);

        QString name(compound->name.c_str() );
       // QString id( compound->id.c_str() );
        parent->setText(0,name.toUpper());  //Feng note: sort names after capitalization

        float mz;
        float precursorMz = compound->precursorMz;
        float productMz = compound->productMz;

        if (compound->formula.length()) {
            int charge = _mw->mavenParameters->getCharge(compound);
            mz = compound->adjustedMass(charge);
        } 
        else {
            mz = compound->mass;
        }

        if (precursorMz > 0 && productMz > 0 && productMz <= precursorMz) {
            parent->setText(1,QString::number(precursorMz,'f',3) + "/" + QString::number(productMz,'f',3));
        } else {
            parent->setText(1,QString::number(mz,'f',6));            
        }


        if(compound->expectedRt > 0) parent->setText(2,QString::number(compound->expectedRt));
        parent->setData(0, Qt::UserRole, QVariant::fromValue(compound));
        parent->setFlags(Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsEnabled);

        if (compound->charge)
            addItem(parent, "Charge", compound->charge);
        if (compound->formula.length())
            addItem(parent, "Formula", compound->formula.c_str());
        if (compound->precursorMz && compound->fragmentMzValues.size() == 0)
            addItem(parent, "Precursor Mz", compound->precursorMz);
        if (compound->productMz)
            addItem(parent, "Product Mz", compound->productMz);
        if (compound->collisionEnergy)
            addItem(parent, "Collision Energy", compound->collisionEnergy);
        if (compound->hasGroup())
            parent->setIcon(0, QIcon(":/images/link.png"));

        if(compound->category.size() > 0) {
            QStringList catList;
            for(unsigned int i=0; i<compound->category.size();i++) {
                catList << compound->category[i].c_str();
            }
            parent->setText(3,catList.join(";"));
        }

        if (!compound->note.empty()) {
            parent->setText(4, QString::fromStdString(compound->note));
        }

        if (compound->fragmentMzValues.size()) {
            QStringList mzList;
            for(unsigned int i=0; i<compound->fragmentMzValues.size();i++) {
                mzList << QString::number(compound->fragmentMzValues[i],'f',2);
            }
            QTreeWidgetItem* child = addItem(parent,"Fragments",compound->fragmentMzValues[0]);
            child->setText(1,mzList.join(";"));
        }


        /*for(int i=0; i <compound->category.size(); i++ ) {
                QTreeWidgetItem *item = new QTreeWidgetItem(parent, PathwayType);
                item->setText(0,QString(compound->category[i].c_str()));
                //parent->setData(0,Qt::UserRole,QVariant::fromValue(QString(pathway_id.c_str())));
            }*/

    }
    setHash();
    treeWidget->setSortingEnabled(true);
}

void LigandWidget::setHash()
{
    CompoundsHash.clear();
    QTreeWidgetItemIterator itr(treeWidget);
    
    while(*itr) {     
        QTreeWidgetItem* item =(*itr);
        QVariant v = item->data(0,Qt::UserRole);
        Compound*  c =  v.value<Compound*>();                                                                                   
        CompoundsHash.insert(c , item);
        ++itr;
    }
}

void LigandWidget::markAsDone(Compound* compound) 
{
    if(compound == nullptr)
        return;
    
    QHash<Compound *, QTreeWidgetItem *>::const_iterator i = CompoundsHash.find(compound);
    
    if (i != CompoundsHash.end() & i.key() == compound) {
        QTreeWidgetItem* item = i.value();  
        if (item != nullptr) {
            for (int col = 0; col < treeWidget->columnCount(); col++) {
                item->setBackground(col,QBrush(QColor(61, 204, 85, 100)));
            }
        }
    }    
}

void LigandWidget::resetColor() 
{
    QTreeWidgetItemIterator itr(treeWidget);
    
    while (*itr) {
        QTreeWidgetItem* item = (*itr);
        if (item) {
            for (int col = 0; col < treeWidget->columnCount(); col++) {
                item->setBackgroundColor(col, QColor(255, 255, 255, 100));
            }
        }
        ++itr;
    }
}

void LigandWidget::saveCompoundList(QString fileName,QString dbname){

   if (fileName.isEmpty()) return;

   QString SEP="\t";
   if (fileName.endsWith(".csv",Qt::CaseInsensitive)) {
       SEP=",";
   } else if (!fileName.endsWith(".tab",Qt::CaseInsensitive)) {
       fileName = fileName + tr(".tab");
       SEP="\t";
   }

    QFile data(fileName);
    if (data.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&data);

        //header
        out << "polarity" << SEP;
        out << "compound" << SEP;
        out << "mz" << SEP;
        out << "charge" << SEP;
        out << "precursorMz" << SEP;
        out << "collisionEnergy" << SEP;
        out << "productMz" << SEP;
        out << "expectedRt" << SEP;
        out << "id" << SEP;
        out << "formula" << SEP;
        out << "srmId" << SEP;
        out << "category" << endl;

        for(unsigned int i=0;  i < DB.compoundsDB.size(); i++ ) {
            Compound* compound = DB.compoundsDB[i];
            if(compound->db != dbname.toStdString() ) continue;

            QString charpolarity;
            if (compound->charge > 0) charpolarity = "+";
            if (compound->charge < 0) charpolarity = "-";

            QStringList category;

            for(int i=0; i < compound->category.size(); i++) {
                category << QString(compound->category[i].c_str());
            }

            out << charpolarity << SEP;
            out << QString(compound->name.c_str()) << SEP;
            out << compound->mass << SEP;
            out << compound->charge << SEP;
            out << compound->precursorMz  << SEP;
            out << compound->collisionEnergy << SEP;
            out << compound->productMz    << SEP;
            out << compound->expectedRt   << SEP;
            out << compound->id.c_str() <<  SEP;
            out << compound->formula.c_str() << SEP;
            out << compound->srmId.c_str() << SEP;
            out << category.join(";") << SEP;
            out << "\n";
        }
    }
}

void LigandWidget::showNext() {
	QTreeWidgetItem * item 	=treeWidget->currentItem();
	if (item && treeWidget->itemBelow(item) ) {
			treeWidget->setCurrentItem(treeWidget->itemBelow(item));
	}
}

void LigandWidget::showLast() {
	QTreeWidgetItem * item 	=treeWidget->currentItem();
	if (item && treeWidget->itemAbove(item) ) {
			treeWidget->setCurrentItem(treeWidget->itemAbove(item));
	}
}

void LigandWidget::showLigand() {
	if (!_mw) return;

    qDebug() << "LigandWidget::showLigand()";
    Q_FOREACH(QTreeWidgetItem* item, treeWidget->selectedItems() ) {
            QVariant v = item->data(0,Qt::UserRole);
            Compound*  c =  v.value<Compound*>();
            if (c)  _mw->setCompoundFocus(c);
			if (c)   matchFragmentation();

    }
}
void LigandWidget::fetchRemoteCompounds()
{
    qDebug() << "fetchRemoteCompounds()";
    xml.clear();
    QSettings *settings = _mw->getSettings();

    if ( settings->contains("data_server_url")) {
        QUrl url(settings->value("data_server_url").toString());
        QUrlQuery query;
        query.addQueryItem("action", "fetchcompounds");
        query.addQueryItem("format", "xml");
        url.setQuery(query);
         QNetworkRequest request;
        request.setUrl(url);

         QNetworkReply *reply = m_manager->get(request);
         qDebug() << url.toEncoded();
    }
}

void LigandWidget::readRemoteData(QNetworkReply* reply)
  {
      //qDebug() << "readRemoteData() << " << resp.statusCode();
  
    //  if (resp.statusCode() == 302 || resp.statusCode() == 200 ) { //redirect
    //      xml.addData(http.readAll());
    //      parseXMLRemoteCompounds();
    //      setDatabaseNames();
     if (reply) { //redirect
         xml.addData(reply->readAll());
      } else {
        //  http.abort();
         reply->abort();
      }
 
     parseXMLRemoteCompounds();
     setDatabaseNames();
  }

QList<Compound*> LigandWidget::parseXMLRemoteCompounds()
{
    //Merged with Maven776 - Kiran
    qDebug() << "LigandWidget::parseXMLRemoteCompounds()";
    Compound *remoteCompound=NULL;
    QString currentTag;
    QList<Compound*>remoteCompounds;
    int itemCount=0;

    //remote databases will have the following prefix appended
    //to the name of the database to indicate that they were fetched.
    std::string remoteDBPrefix="REMOTE:";

    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
            currentTag = xml.name().toString().toLower();
            if (currentTag == "item"){
                itemCount++;
                //qDebug() << "new item:" << itemCount;
                remoteCompound = new Compound("Unknown","Unknown","",0);
            }
        } else if (xml.isEndElement()) {
            currentTag = xml.name().toString().toLower();
            if (currentTag == "item") {
                if (remoteCompound !=NULL) {

                    if (!remoteCompound->formula.empty()) {
                        remoteCompound->mass=remoteCompound->adjustedMass(0);
                    }

					if (remoteCompound->name == "Unknown") {
                        remoteCompound->name = remoteCompound->id;
					}

                    DB.addCompound(remoteCompound);
                    remoteCompounds << remoteCompound;
                    //qDebug() << "new remote compound..: " << remoteCompound->id.c_str();
                    remoteCompound = NULL;

                }
         }
        }  else {
            QString xmltext =xml.text().toString().simplified();
            //qDebug() << xmltext;
            if (xmltext.length() == 0 ) continue;

            if (remoteCompound == NULL ){
                qDebug() << "Parse Error: " << currentTag << " XML: " << xmltext;
                continue;
            } else if (currentTag == "metabolite_id") {
                remoteCompound->id = xmltext.toStdString();
                qDebug() << "ID: " << xmltext;
            }

            else if (currentTag == "metabolite_name")
                remoteCompound->name = xmltext.toStdString();

            else if (currentTag == "formula")
                remoteCompound->formula = xmltext.toStdString();

            else if (currentTag == "kegg_id")
                remoteCompound->kegg_id = xmltext.toStdString();

            else if (currentTag == "pubmed_id")
                remoteCompound->pubchem_id = xmltext.toStdString();

            else if (currentTag == "hmdb_id")
                remoteCompound->hmdb_id = xmltext.toStdString();

            else if (currentTag == "precursormz") {
                remoteCompound->precursorMz = xmltext.toDouble();
                remoteCompound->mass = remoteCompound->precursorMz;
            }
            else if (currentTag == "productmz")
                remoteCompound->productMz = xmltext.toDouble();

            else if (currentTag == "ce")
                remoteCompound->collisionEnergy = xmltext.toDouble();

            else if (currentTag == "retentiontime")
                remoteCompound->expectedRt = xmltext.toDouble();

            else if (currentTag == "method_id") {
                remoteCompound->db = remoteDBPrefix + xmltext.toStdString();
                remoteCompound->method_id = xmltext.toStdString();

            } else if (currentTag == "transition_id")
                remoteCompound->transition_id = xmltext.toInt();
        }
    }

    if (xml.error()) {
        if ( xml.error() != QXmlStreamReader::PrematureEndOfDocumentError) {
            qWarning() << "XML ERROR: BAD END TO DOCUMENT" << xml.lineNumber() << ": " << xml.errorString();
            // http.abort();
        } else {
            qWarning() << "XML ERROR:" << xml.lineNumber() << ": " << xml.errorString();
        }
    }

    qDebug() << "Done! remoteCompoundFetch() Found=" << remoteCompounds.size();

    return remoteCompounds;
}

Compound* LigandWidget::getSelectedCompound() { 
    //Merged with Maven776
	//get current compound
    QTreeWidgetItem* item = treeWidget->selectedItems().first();
    if (!item) return NULL;

    QVariant v = item->data(0,Qt::UserRole);
    Compound* c =  v.value<Compound*>();
	if (!c) return NULL;

	return c;
}


void LigandWidget::matchFragmentation() {
    // New feature added - Merged with Maven776 - Kiran
	Compound* c = getSelectedCompound();
        if (!c or c->fragmentMzValues.size() == 0) return;

    QStringList searchText;
        int mzCount = c->fragmentMzValues.size();
        int intsCount = c->fragmentIntensities.size();

    int charge = _mw->mavenParameters->getCharge(c); //user specified ionization mode
	float precursorMz = c->precursorMz;
    if (!c->formula.empty()) precursorMz = c->adjustedMass(charge);

    for(int i=0; i < mzCount; i++ ) {
                        float mz = c->fragmentMzValues[i];
			float ints = 0;
                        if (i < intsCount) ints = c->fragmentIntensities[i];

            searchText  << tr("%1\t%2")
                .arg(QString::number(mz,'f', 5))
                .arg(QString::number(ints, 'f', 2));
    }

	qDebug() << "Search: " << searchText << endl;

	_mw->spectraMatchingForm->fragmentsText->setPlainText(searchText.join("\n"));
	_mw->spectraMatchingForm->precursorMz->setText(QString::number(precursorMz,'f',6));

//	_mw->spectraMatchingForm->show();
}
