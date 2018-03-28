#include "ligandwidget.h"
#include "numeric_treewidgetitem.h"

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

  connect(treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*, int)), SLOT(showLigand()));
  connect(treeWidget,SIGNAL(itemSelectionChanged()), SLOT(showLigand()));

  QToolBar *toolBar = new QToolBar(this);
  toolBar->setFloatable(false);
  toolBar->setMovable(false);

  databaseSelect = new QComboBox(toolBar);
  databaseSelect->setObjectName(QString::fromUtf8("databaseSelect"));
  databaseSelect->setDuplicatesEnabled(false);
  databaseSelect->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::MinimumExpanding);


//   galleryButton = new QToolButton(toolBar);
//   galleryButton->setIcon(QIcon(rsrcPath + "/gallery.png"));
//   galleryButton->setToolTip(tr("Show Compounds in Gallery Widget"));
//   connect(galleryButton,SIGNAL(clicked()),SLOT(showGallery()));

  loadButton = new QToolButton(toolBar);
  loadButton->setIcon(QIcon(rsrcPath + "/fileopen.png"));
  loadButton->setToolTip("Load Custom Compound List");

  connect(loadButton,SIGNAL(clicked()), mw, SLOT(loadCompoundsFile()));
  connect(this, SIGNAL(compoundFocused(Compound*)), mw, SLOT(setCompoundFocus(Compound*)));
  connect(this, SIGNAL(urlChanged(QString)), mw, SLOT(setUrl(QString)));

  saveButton = new QToolButton(toolBar);
  saveButton->setIcon(QIcon(rsrcPath + "/filesave.png"));
  saveButton->setToolTip("Save Compound List");
  connect(saveButton,SIGNAL(clicked()), SLOT(saveCompoundList()));


  saveButton->setEnabled(false);

  toolBar->addWidget(databaseSelect);
  toolBar->addWidget(loadButton);
  toolBar->addWidget(saveButton);
//   toolBar->addWidget(galleryButton);

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

  connect(this, SIGNAL(databaseChanged(QString)), _mw, SLOT(showSRMList()));

}
QString LigandWidget::getDatabaseName() {
	return databaseSelect->currentText();
}

void LigandWidget::setDatabaseNames() {
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
    precursormz = xml.attributes().value("Precursor Mz").toString().toFloat();
    productmz = xml.attributes().value("Product Mz").toString().toFloat();
    collisionenergy = xml.attributes().value("Collision Energy").toString().toFloat();

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
   if (index != -1 ) databaseSelect->setCurrentIndex(index);

    _mw->getSettings()->setValue("lastCompoundDatabase", getDatabaseName());
    Q_EMIT databaseChanged(getDatabaseName());
    showTable();
}

void LigandWidget::databaseChanged(int index) {
    QString dbname = databaseSelect->currentText();
    setDatabase(dbname);
    setDatabaseAltered(dbname, alteredDatabases[dbname]);
}

void LigandWidget::setDatabaseAltered(QString name, bool altered) {
    alteredDatabases[name]=altered;
    QString dbname = databaseSelect->currentText();

    if (dbname == name && altered == true) {
        saveButton->setEnabled(true);
    }

    if (dbname == name && altered == false) {
        saveButton->setEnabled(false);
    }
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
    QStringList header; header << "name" << "m/z" << "rt" << "category";
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

        if (compound->charge) addItem(parent,"Charge", compound->charge);
        if (compound->formula.length()) addItem(parent,"Formula", compound->formula.c_str());
        if (compound->precursorMz) addItem(parent,"Precursor Mz", compound->precursorMz);
        if (compound->productMz) addItem(parent,"Product Mz", compound->productMz);
        if (compound->collisionEnergy) addItem(parent,"Collision Energy", compound->collisionEnergy);
        if (compound->hasGroup() ) parent->setIcon(0,QIcon(":/images/link.png"));

        if(compound->category.size() > 0) {
            QStringList catList;
            for(unsigned int i=0; i<compound->category.size();i++) {
                catList << compound->category[i].c_str();
            }
            parent->setText(3,catList.join(";"));
        }

        if (compound->fragment_mzs.size()) {
            QStringList mzList;
            for(unsigned int i=0; i<compound->fragment_mzs.size();i++) {
                mzList << QString::number(compound->fragment_mzs[i],'f',2);
            }
            QTreeWidgetItem* child = addItem(parent,"Fragments",compound->fragment_mzs[0]);
            child->setText(1,mzList.join(";"));
        }


        /*for(int i=0; i <compound->category.size(); i++ ) {
                QTreeWidgetItem *item = new QTreeWidgetItem(parent, PathwayType);
                item->setText(0,QString(compound->category[i].c_str()));
                //parent->setData(0,Qt::UserRole,QVariant::fromValue(QString(pathway_id.c_str())));

            }*/

    }
    treeWidget->setSortingEnabled(true);
}

void LigandWidget::saveCompoundList(){
    QSettings *settings = _mw->getSettings();
    string dbname = databaseSelect->currentText().toStdString();
    QString dbfilename = databaseSelect->currentText() + ".tab";
    QString dataDir = settings->value("dataDir").value<QString>();
    QString methodsFolder =     dataDir +  "/"  + settings->value("methodsFolder").value<QString>();

    QString fileName = QFileDialog::getSaveFileName(
                this, "Export Compounds to Filename", methodsFolder, "TAB (*.tab)");

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
        //Addiditional headers added when merging with Maven776 - Kiran
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
            if(compound->db != dbname ) continue;

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
        setDatabaseAltered(databaseSelect->currentText(),false);
    }
}


void LigandWidget::showGallery() {

	vector<Compound*>matches;
    QTreeWidgetItemIterator itr(treeWidget);
    while (*itr) {
        QTreeWidgetItem* item =(*itr);
        QVariant v = item->data(0,Qt::UserRole);
        Compound*  compound =  v.value<Compound*>();
        matches.push_back(compound);
    }

    //qDebug() << "  showGallery()" << matches.size();
	if (matches.size() > 0) {
		_mw->galleryWidget->clear();
		_mw->galleryWidget->addEicPlots(matches);
		_mw->galleryDockWidget->show();
        _mw->galleryDockWidget->raise();

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
	if (!c or c->fragment_mzs.size() == 0) return;

    QStringList searchText;
	int mzCount = c->fragment_mzs.size();
	int intsCount = c->fragment_intensity.size(); 

    int charge = _mw->mavenParameters->getCharge(c); //user specified ionization mode
	float precursorMz = c->precursorMz;
    if (!c->formula.empty()) precursorMz = c->adjustedMass(charge);

    for(int i=0; i < mzCount; i++ ) {
			float mz = c->fragment_mzs[i];
			float ints = 0; 
			if (i < intsCount) ints = c->fragment_intensity[i];

            searchText  << tr("%1\t%2")
                .arg(QString::number(mz,'f', 5))
                .arg(QString::number(ints, 'f', 2));
    }

	qDebug() << "Search: " << searchText << endl;

	_mw->spectraMatchingForm->fragmentsText->setPlainText(searchText.join("\n"));
	_mw->spectraMatchingForm->precursorMz->setText(QString::number(precursorMz,'f',6));

//	_mw->spectraMatchingForm->show();
}
