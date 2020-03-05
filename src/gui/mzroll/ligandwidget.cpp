#include "adductwidget.h"
#include "Compound.h"
#include "ligandwidget.h"
#include "alignmentdialog.h"
#include "common/analytics.h"
#include "globals.h"
#include "librarymanager.h"
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

LigandWidget::LigandWidget(MainWindow* mw)
{
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

  adductWidget = new AdductWidget(_mw);
  
  QToolBar *toolBar = new QToolBar(this);
  toolBar->setFloatable(false);
  toolBar->setMovable(false);
  toolBar->setIconSize(QSize(24, 24));

  databaseSelect = new QComboBox(toolBar);
  databaseSelect->setObjectName(QString::fromUtf8("databaseSelect"));
  databaseSelect->setDuplicatesEnabled(false);
  databaseSelect->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::MinimumExpanding);

  connect(this, SIGNAL(compoundFocused(Compound*)), mw, SLOT(setCompoundFocus(Compound*)));
  connect(this, SIGNAL(urlChanged(QString)), mw, SLOT(setUrl(QString)));

  libraryButton = new QToolButton(toolBar);
  libraryButton->setIcon(QIcon(rsrcPath + "/librarymanager.png"));
  libraryButton->setToolTip("Open Library Manager");
  connect(libraryButton,
          &QPushButton::clicked,
          _mw->getLibraryManager(),
          &LibraryManager::exec);

  btnAdducts = new QToolButton(toolBar);
  btnAdducts->setIcon(QIcon(rsrcPath + "/adducts.png"));
  btnAdducts->setToolTip("Open Adducts widget");
  connect(btnAdducts, &QToolButton::clicked, adductWidget, &AdductWidget::show);

  toolBar->addWidget(databaseSelect);
  toolBar->addWidget(libraryButton);
  toolBar->addWidget(btnAdducts);

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

  // Fetches and reads compounds from a remote location when fetch button is clicked - Kiran
  disconnect(_mw->settingsForm->fetchCompounds,SIGNAL(clicked()));
  connect(_mw->settingsForm->fetchCompounds,SIGNAL(clicked()),this,SLOT(fetchRemoteCompounds()));
  connect(this, SIGNAL(mzrollSetDB(QString)), _mw, SLOT(mzrollLoadDB(QString)));

  m_manager = new QNetworkAccessManager(this);
  connect(m_manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(readRemoteData(QNetworkReply*)));

  QDirIterator itr(":/databases/");

  while(itr.hasNext()) {
    auto filename = itr.next().toStdString();
    DB.loadCompoundCSVFile(filename);
    string dbname = mzUtils::cleanFilename(filename);
    _mw->massCalcWidget->database->addItem(QString::fromStdString(dbname));
  }

  QSet<QString>set;
  for(int i=0; i< DB.compoundsDB.size(); i++) {
      if (! set.contains( DB.compoundsDB[i]->db().c_str() ) )
          set.insert( DB.compoundsDB[i]->db().c_str() );
  }

  QSetIterator<QString> i(set);
  while (i.hasNext())
      databaseSelect->addItem(i.next());

  QDirIterator adductItr(":/databases/Adducts/");

    while (adductItr.hasNext()) {
        auto filename = adductItr.next().toStdString();
        DB.loadAdducts(filename);
    }

    adductWidget->loadAdducts();

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
            if (! set.contains( DB.compoundsDB[i]->db().c_str() ) )
                set.insert( DB.compoundsDB[i]->db().c_str() );
	}

    QSetIterator<QString> i(set);
	while (i.hasNext()) { 
                //databaseSelect->insertItem(pos++, i.next());
                databaseSelect->addItem(i.next());
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
    compound->setExpectedRt(rt);
    compound->setMz(mz);
    compound->setDb (dbname);
    compound->setPrecursorMz(precursormz);
    compound->setProductMz(productmz);
    compound->setCollisionEnergy(collisionenergy);
    vector<string> category;
    for(int i=0; i < categorylist.size(); i++) category.push_back(categorylist[i]);
    compound->setCategory(category);
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

    auto dbName = getDatabaseName();
    auto dbPath = _mw->getLibraryManager()->filePathForDatabase(dbName);
    _mw->setLastLoadedDatabase(dbPath);
    _mw->getSettings()->setValue("lastCompoundDatabase", dbName);
    Q_EMIT databaseChanged(dbName);
    showTable();
}

void LigandWidget::databaseChanged(int index) {
    QString dbname = databaseSelect->currentText();
    setDatabase(dbname);
}

void LigandWidget::setCompoundFocus(Compound* c) {
	if (c==NULL) return;
        QString dbname(c->db().c_str());
	int index = databaseSelect->findText(dbname,Qt::MatchExactly);
        if (index != -1 ) databaseSelect->setCurrentIndex(index);

        QString filterString(c->name().c_str());
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
                    stack << compound->name().c_str()
                          << compound->id().c_str()
                          << compound->formula().c_str();

                    if( compound->category().size()) {
                        auto category = compound->category();
                        for(int i=0; i < category.size(); i++) {
                            stack << category[i].c_str();
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

    QString mass = QString::number(c->mz());
    QString rt = QString::number(c->expectedRt());
    item->setText(1,mass);
    item->setText(2,rt);
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
        if(compound->db() != dbname ) continue; //skip compounds from other databases
        NumericTreeWidgetItem *parent  = new NumericTreeWidgetItem(treeWidget,CompoundType);

        QString name(compound->name().c_str() );
       // QString id( compound->id.c_str() );
        parent->setText(0,name.toUpper());  //Feng note: sort names after capitalization

        float mz;
        float precursorMz = compound->precursorMz();
        float productMz = compound->productMz();

        if (compound->formula().length() || compound->neutralMass() != 0.0f) {
            int charge = _mw->mavenParameters->getCharge(compound);
            mz = compound->adjustedMass(charge);
        } 
        else {
            mz = compound->mz();
        }

        if (precursorMz > 0 && productMz > 0 && productMz <= precursorMz) {
            parent->setText(1,QString::number(precursorMz,'f',3) + "/" + QString::number(productMz,'f',3));
        } else {
            parent->setText(1,QString::number(mz,'f',6));            
        }


        if(compound->expectedRt() > 0) parent->setText(2,QString::number(compound->expectedRt()));
        parent->setData(0, Qt::UserRole, QVariant::fromValue(compound));
        parent->setFlags(Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsEnabled);

        if (compound->charge())
            addItem(parent, "Charge", compound->charge());
        if (compound->formula().length())
            addItem(parent, "Formula", compound->formula().c_str());
        if (compound->precursorMz() && compound->fragmentMzValues().size() == 0)
            addItem(parent, "Precursor Mz", compound->precursorMz());
        if (compound->productMz())
            addItem(parent, "Product Mz", compound->productMz());
        if (compound->collisionEnergy())

            addItem(parent, "Collision Energy", compound->collisionEnergy());

        if(compound->category().size() > 0) {
            QStringList catList;
            auto category = compound->category();
            for(unsigned int i=0; i < category.size();i++) {
                catList << category[i].c_str();
            }
            parent->setText(3,catList.join(";"));
        }

        if (!compound->note().empty()) {
            parent->setText(4, QString::fromStdString(compound->note()));
        }

        if (!compound->fragmentMzValues().empty()) {
            for (unsigned int i=0; i < compound->fragmentMzValues().size(); ++i) {
                addItem(parent,
                        "Fragment",
                        to_string(compound->fragmentMzValues()[i]));
            }
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
            if(compound->db() != dbname.toStdString() ) continue;

            QString charpolarity;
            if (compound->charge() > 0) charpolarity = "+";
            if (compound->charge() < 0) charpolarity = "-";

            QStringList category;
            auto categoryVect = compound->category();
            for(int i=0; i < categoryVect.size(); i++) {
                category << QString(categoryVect[i].c_str());
            }

            out << charpolarity << SEP;
            out << QString(compound->name().c_str()) << SEP;
            out << compound->mz() << SEP;
            out << compound->charge() << SEP;
            out << compound->precursorMz()  << SEP;
            out << compound->collisionEnergy() << SEP;
            out << compound->productMz()    << SEP;
            out << compound->expectedRt()   << SEP;
            out << compound->id().c_str() <<  SEP;
            out << compound->formula().c_str() << SEP;
            out << compound->srmId().c_str() << SEP;
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
        Compound* compound =  v.value<Compound*>();
        if (compound) {
            _mw->setCompoundFocus(compound);
            matchFragmentation();
        } else if (item->parent() != nullptr && item->text(0) == "Fragment") {
            QVariant v = item->parent()->data(0, Qt::UserRole);
            Compound* precursorCompound =  v.value<Compound*>();
            if (precursorCompound == nullptr)
                continue;

            _mw->setCompoundFocus(precursorCompound, item->text(1).toFloat());
        }
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

                    if (!remoteCompound->formula().empty()) {
                        remoteCompound->setMz(remoteCompound->adjustedMass(0));
                    }

                    if (remoteCompound->name() == "Unknown") {
                        remoteCompound->setName(remoteCompound->id()) ;
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
                remoteCompound->setId(xmltext.toStdString());
                qDebug() << "ID: " << xmltext;
            }

            else if (currentTag == "metabolite_name")
                remoteCompound->setName(xmltext.toStdString());

            else if (currentTag == "formula")
                remoteCompound->setFormula(xmltext.toStdString());

            else if (currentTag == "kegg_id")
                remoteCompound->setKegg_id (xmltext.toStdString());

            else if (currentTag == "pubmed_id")
                remoteCompound->setPubchem_id (xmltext.toStdString());

            else if (currentTag == "hmdb_id")
                remoteCompound->setHmdb_id (xmltext.toStdString());

            else if (currentTag == "precursormz") {
                remoteCompound->setPrecursorMz(xmltext.toDouble());
                remoteCompound->setMz(remoteCompound->precursorMz());
            }
            else if (currentTag == "productmz")
                remoteCompound->setProductMz(xmltext.toDouble());

            else if (currentTag == "ce")
                remoteCompound->setCollisionEnergy(xmltext.toDouble());

            else if (currentTag == "retentiontime")
                remoteCompound->setExpectedRt( xmltext.toDouble());

            else if (currentTag == "method_id") {
                remoteCompound->setDb (remoteDBPrefix + xmltext.toStdString());
                remoteCompound->setMethod_id (xmltext.toStdString());

            } else if (currentTag == "transition_id")
                remoteCompound->setTransition_id (xmltext.toInt());
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
    auto selectedItems = treeWidget->selectedItems();
    if (selectedItems.isEmpty())
        return nullptr;

    QTreeWidgetItem* item = selectedItems.first();
    if (!item) return NULL;

    QVariant v = item->data(0,Qt::UserRole);
    Compound* c =  v.value<Compound*>();
	if (!c) return NULL;

	return c;
}


void LigandWidget::matchFragmentation() {
    // New feature added - Merged with Maven776 - Kiran
	Compound* c = getSelectedCompound();
        if (!c or c->fragmentMzValues().size() == 0) return;

    QStringList searchText;
    int mzCount = c->fragmentMzValues().size();
    int intsCount = c->fragmentIntensities().size();

    int charge = _mw->mavenParameters->getCharge(c); //user specified ionization mode
    float precursorMz = c->precursorMz();
    if (!c->formula().empty()) precursorMz = c->adjustedMass(charge);
    auto mzValues = c->fragmentMzValues();
    auto intensities = c->fragmentIntensities();
    for(int i=0; i < mzCount; i++ ) {
                        float mz = mzValues[i];
			float ints = 0;
                        if (i < intsCount) ints = intensities[i];

            searchText  << tr("%1\t%2")
                .arg(QString::number(mz,'f', 5))
                .arg(QString::number(ints, 'f', 2));
    }

	qDebug() << "Search: " << searchText << endl;

	_mw->spectraMatchingForm->fragmentsText->setPlainText(searchText.join("\n"));
	_mw->spectraMatchingForm->precursorMz->setText(QString::number(precursorMz,'f',6));

//	_mw->spectraMatchingForm->show();
}
