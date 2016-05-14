#include "ligandwidget.h"
#include "numeric_treewidgetitem.h"

using namespace std;

LigandWidget::LigandWidget(MainWindow* mw) {
  _mw = mw;

  setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  setFloating(false);
  setObjectName("Compounds");

  treeWidget = new QTreeWidget(this);
  treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
  treeWidget->setSortingEnabled(false);
  treeWidget->setColumnCount(3);
  treeWidget->setRootIsDecorated(false);
  treeWidget->setUniformRowHeights(true);
  treeWidget->setHeaderHidden(false);
  treeWidget->setDragDropMode(QAbstractItemView::DragOnly);

  connect(treeWidget, SIGNAL(itemSelectionChanged()), SLOT(showLigand()));

  QToolBar *toolBar = new QToolBar(this);
  toolBar->setFloatable(false);
  toolBar->setMovable(false);

  databaseSelect = new QComboBox(toolBar);
  databaseSelect->setObjectName(QString::fromUtf8("databaseSelect"));
  databaseSelect->setDuplicatesEnabled(false);
  databaseSelect->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);


  galleryButton = new QToolButton(toolBar);
  galleryButton->setIcon(QIcon(rsrcPath + "/gallery.png"));
  galleryButton->setToolTip(tr("Show Compounds in Gallery Widget"));
  connect(galleryButton, SIGNAL(clicked()), SLOT(showGallery()));

  loadButton = new QToolButton(toolBar);
  loadButton->setIcon(QIcon(rsrcPath + "/fileopen.png"));
  loadButton->setToolTip("Load Custom Compound List");

  connect(loadButton, SIGNAL(clicked()), mw, SLOT(loadCompoundsFile()));
  connect(this, SIGNAL(compoundFocused(Compound*)), mw, SLOT(setCompoundFocus(Compound*)));
  connect(this, SIGNAL(urlChanged(QString)), mw, SLOT(setUrl(QString)));

  saveButton = new QToolButton(toolBar);
  saveButton->setIcon(QIcon(rsrcPath + "/filesave.png"));
  saveButton->setToolTip("Save Compound List");
  connect(saveButton, SIGNAL(clicked()), SLOT(saveCompoundList()));


  saveButton->setEnabled(false);

  //toolBar->addWidget(new QLabel("Compounds: "));
  toolBar->addWidget(databaseSelect);
  toolBar->addWidget(loadButton);
  toolBar->addWidget(saveButton);
  toolBar->addWidget(galleryButton);


  setWidget(treeWidget);
  setTitleBarWidget(toolBar);
  setWindowTitle("Compounds");



  connect(&http, SIGNAL(readyRead(const QHttpResponseHeader &)), this,
          SLOT(readRemoteData(const QHttpResponseHeader &)));

  fetchRemoteCompounds();

}
QString LigandWidget::getDatabaseName() {
  return databaseSelect->currentText();
}

void LigandWidget::setDatabaseNames() {
  databaseSelect->disconnect(SIGNAL(currentIndexChanged(QString)));
  databaseSelect->clear();
  QSet<QString>set;
  for (int i = 0; i < DB.compoundsDB.size(); i++) {
    if (! set.contains( DB.compoundsDB[i]->db.c_str() ) )
      set.insert( DB.compoundsDB[i]->db.c_str() );
  }

  QIcon icon(rsrcPath + "/dbsearch.png");
  QSetIterator<QString> i(set);
  int pos = 0;
  while (i.hasNext()) {
    //databaseSelect->insertItem(pos++, i.next());
    databaseSelect->addItem(icon, i.next());
  }
  connect(databaseSelect, SIGNAL(currentIndexChanged(QString)), this, SLOT(setDatabase(QString)));
}

QTreeWidgetItem* LigandWidget::addItem(QTreeWidgetItem* parentItem, string key , float value) {
  QTreeWidgetItem *item = new QTreeWidgetItem(parentItem);
  item->setText(0, QString(key.c_str()));
  item->setText(1, QString::number(value, 'f', 3));
  return item;
}

QTreeWidgetItem* LigandWidget::addItem(QTreeWidgetItem* parentItem, string key , string value) {
  QTreeWidgetItem *item = new QTreeWidgetItem(parentItem);
  item->setText(0, QString(key.c_str()));
  item->setText(1, QString(value.c_str()));
  return item;
}

void LigandWidget::setDatabase(QString dbname) {
  int currentIndex = databaseSelect->currentIndex();
  int index = databaseSelect->findText(dbname, Qt::MatchExactly);
  if (index != -1 ) databaseSelect->setCurrentIndex(index);

  _mw->getSettings()->setValue("lastCompoundDatabase", getDatabaseName());
  emit databaseChanged(getDatabaseName());
  createCategoryItems();
  showTable();
}

void LigandWidget::databaseChanged(int index) {
  QString dbname = databaseSelect->currentText();
  setDatabase(dbname);
  setDatabaseAltered(dbname, alteredDatabases[dbname]);
}

void LigandWidget::setDatabaseAltered(QString name, bool altered) {
  alteredDatabases[name] = altered;
  QString dbname = databaseSelect->currentText();

  if (dbname == name && altered == true) {
    saveButton->setEnabled(true);
  }

  if (dbname == name && altered == false) {
    saveButton->setEnabled(false);
  }
}

void LigandWidget::setCompoundFocus(Compound* c) {
  if (c == NULL) return;
  QString dbname(c->db.c_str());
  int index = databaseSelect->findText(dbname, Qt::MatchExactly);
  if (index != -1 ) databaseSelect->setCurrentIndex(index);

  QString filterString(c->name.c_str());
  setWindowTitle("Compounds: " + filterString);
  showTable();
}


void LigandWidget::setFilterString(QString s) {
  if (s != filterString) {
    filterString = s;
    setWindowTitle("Compounds: " + filterString);
    showTable();
  }
  //if (s.length() >= 4 ) { showMatches(s); }
}

void LigandWidget::showMatches(QString needle) {


}

void LigandWidget::createCategoryItems() {

  string dbname = databaseSelect->currentText().toStdString();

  for (unsigned int i = 0;  i < DB.compoundsDB.size(); i++ ) {
    Compound* compound = DB.compoundsDB[i];
    if (compound->db != dbname) continue;

    for (int i = 0; i < compound->category.size(); i++) {
      QString categoryName(compound->category[i].c_str());
      if (!categoryMap.contains(categoryName.toLower())) {
        QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);
        item->setHidden(true);
        item->setText(0, categoryName);
        categoryMap[categoryName.toLower()] = item;
      }
    }
  }
}


void LigandWidget::showTable() {
  //  treeWidget->clear();
  treeWidget->setColumnCount(3);
  QStringList header; header << "name" << "m/z" << "rt";
  treeWidget->setHeaderLabels( header );
  treeWidget->setSortingEnabled(false);

  string dbname = databaseSelect->currentText().toStdString();
  QRegExp regexp(filterString, Qt::CaseInsensitive, QRegExp::RegExp);
  if (!filterString.isEmpty() && !regexp.isValid())return;
  cerr << "ligandwidget::showTable() " << dbname << endl;

  //hide categories
  foreach(QTreeWidgetItem * item, categoryMap.values()) item->setHidden(true);

  for (unsigned int i = 0;  i < DB.compoundsDB.size(); i++ ) {
    Compound* compound = DB.compoundsDB[i];

    //skip compounds from other databases
    if (compound->db != dbname ) {
      if (items.count(compound) != 0 )  items[compound]->setHidden(true);
      continue;
    }

    QString name(compound->name.c_str() );
    QString formula( compound->formula.c_str() );
    QString id( compound->id.c_str() );

    if ( ! filterString.isEmpty() ) {
      if (! (name.contains(regexp) || id.contains(regexp) || formula.contains(regexp))) {
        if ( items.count(compound) != 0 )  items[compound]->setHidden(true);
        continue;
      }
    }

    //if (compound->hasGroup() ) { PeakGroup* group = compound->getPeakGroup(); }
    QTreeWidgetItem* categoryParent = NULL;
    if (compound->category.size() > 0) {
      QString categoryName = QString(compound->category[0].c_str()).toLower();
      if (categoryMap.contains(categoryName)) {
        categoryParent = categoryMap[categoryName];
        categoryParent->setHidden(false);
        categoryParent->setExpanded(true);
      }
    }

    if ( items.count(compound) == 0 ) {
      NumericTreeWidgetItem *parent = NULL;

      if (categoryParent) {
        parent = new NumericTreeWidgetItem(categoryParent, CompoundType);
      } else {
        parent = new NumericTreeWidgetItem(treeWidget, CompoundType);
      }

      QString mass = QString::number(compound->mass);
      QString rt = QString::number(compound->expectedRt);
      //parent->setText(0,name);
      parent->setText(0, name.toUpper());
      //Feng note: sort names after capitalization
      parent->setText(1, mass);
      parent->setText(2, rt);
      parent->setData(0, Qt::UserRole, QVariant(QString(compound->db.c_str())));
      parent->setData(1, Qt::UserRole, QVariant(QString(compound->id.c_str())));
      parent->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled);


      if (compound->charge) addItem(parent, "Charge", compound->charge);
      if (compound->formula.length()) addItem(parent, "Formula", compound->formula.c_str());
      if (compound->precursorMz) addItem(parent, "Precursor Mz", compound->precursorMz);
      if (compound->productMz) addItem(parent, "Product Mz", compound->productMz);
      if (compound->collisionEnergy) addItem(parent, "Collision Energy", compound->collisionEnergy);


      /*for(int i=0; i <compound->category.size(); i++ ) {
          QTreeWidgetItem *item = new QTreeWidgetItem(parent, PathwayType);
          item->setText(0,QString(compound->category[i].c_str()));
          //parent->setData(0,Qt::UserRole,QVariant::fromValue(QString(pathway_id.c_str())));

      }*/

      items[compound] = parent;   //forwardmap  compound->object
      compoundMap[parent] = compound; //reverse map object->compound

    } else if ( items.count(compound) > 0 ) {
      items[compound]->setHidden(false);
    }
  }
  treeWidget->setSortingEnabled(true);

}

void LigandWidget::saveCompoundList() {

  string dbname = databaseSelect->currentText().toStdString();
  QString dbfilename = databaseSelect->currentText() + ".csv";

  QString fileName = QFileDialog::getSaveFileName(
                       this, "Export Compounds to Filename", dbfilename, "CSV (*.csv)");

  if (fileName.isEmpty()) return;
  if (!fileName.endsWith(".csv", Qt::CaseInsensitive)) fileName = fileName + tr(".csv");

  QFile data(fileName);
  if (data.open(QFile::WriteOnly | QFile::Truncate)) {
    QTextStream out(&data);

    out << "polarity,compound,precursorMz,collisionEnergy,productMz,expectedRt,id,formula,srmId,category\n";

    for (unsigned int i = 0;  i < DB.compoundsDB.size(); i++ ) {
      Compound* compound = DB.compoundsDB[i];
      if (compound->db != dbname ) continue;

      QString charpolarity;
      if (compound->charge > 0) charpolarity = "+";
      if (compound->charge < 0) charpolarity = "-";

      QStringList category;

      for (int i = 0; i < compound->category.size(); i++) {
        category << QString(compound->category[i].c_str());
      }

      out << charpolarity << ",";
      out << QString(compound->name.c_str()) << ",";
      out << compound->precursorMz  << ",";
      out << compound->collisionEnergy << ",";
      out << compound->productMz    << ",";
      out << compound->expectedRt   << ",";
      out << compound->id.c_str() << ",";
      out << compound->formula.c_str() << ",";
      out << compound->srmId.c_str() << ",";
      out << category.join(";") << ",";
      out << "\n";
      // out << QString(compound->category)
    }
    setDatabaseAltered(databaseSelect->currentText(), false);
  }
}


void LigandWidget::showGallery() {
  vector<Compound*>matches;

  foreach (Compound* c, items.keys()) {
    if (items[c]->isHidden() == false ) matches.push_back(c);
  }

  //qDebug() << "  showGallery()" << matches.size();
  if (matches.size() > 0) {
    _mw->galleryWidget->clear();
    _mw->galleryWidget->addEicPlots(matches);
    _mw->galleryDockWidget->show();
  }
}

void LigandWidget::showNext() {
  QTreeWidgetItem * item  = treeWidget->currentItem();
  if (item && treeWidget->itemBelow(item) ) {
    treeWidget->setCurrentItem(treeWidget->itemBelow(item));
  }
}

void LigandWidget::showLast() {
  QTreeWidgetItem * item  = treeWidget->currentItem();
  if (item && treeWidget->itemAbove(item) ) {
    treeWidget->setCurrentItem(treeWidget->itemAbove(item));
  }
}

void LigandWidget::showLigand() {
  if (!_mw) return;

  qDebug() << "LigandWidget::showLigand()";
  foreach (QTreeWidgetItem* item, treeWidget->selectedItems() ) {
    if (compoundMap.contains(item)) {
      Compound* c = compoundMap[item];
      if (c)  _mw->setCompoundFocus(c);

      break;
    }
  }
  //} else {
  //if ( itemType == PathwayType ) {
  //  _mw->pathwayDockWidget->setVisible(true);
  //  _mw->getPathwayWidget()->setPathway(text);
  //  //qDebug() << "SETPATHWAY=" << text;
  //}
  //}


}

void LigandWidget::fetchRemoteCompounds()
{
  qDebug() << "fetchRemoteCompounds()";
  xml.clear();
  QSettings *settings = _mw->getSettings();

  if ( settings->contains("pathway_server_url")) {
    QUrl url(settings->value("pathway_server_url").toString());
    url.addQueryItem("action", "fetchcompounds");
    url.addQueryItem("format", "xml");
    http.setHost(url.host());
    connectionId = http.get(url.toEncoded());
    // qDebug() << " ConnectionId=" << connectionId;
  }
}

void LigandWidget::readRemoteData(const QHttpResponseHeader &resp)
{
  //qDebug() << "readRemoteData() << " << resp.statusCode();

  if (resp.statusCode() == 302 || resp.statusCode() == 200 ) { //redirect
    xml.addData(http.readAll());
    parseXMLRemoteCompounds();
    setDatabaseNames();
  } else {
    http.abort();
  }
}

QList<Compound*> LigandWidget::parseXMLRemoteCompounds()
{
  //qDebug() << "LigandWidget::parseXMLRemoteCompounds()";
  Compound *remoteCompound = NULL;
  QString currentTag;
  QList<Compound*>remoteCompounds;


  while (!xml.atEnd()) {
    xml.readNext();
    if (xml.isStartElement()) {
      if (xml.name() == "item") {
        remoteCompound = new Compound("Unknown", "Unknown", "", 0);
      }
      currentTag = xml.name().toString().toLower();
    } else if (xml.isEndElement()) {
      if (xml.name() == "item") {
        if (remoteCompound != NULL) {
          if (!remoteCompound->formula.empty())
            remoteCompound->mass = remoteCompound->ajustedMass(0);

          if (!remoteCompound->id.empty()) {
            DB.addCompound(remoteCompound);
            qDebug() << "new remote compound..: " << remoteCompound->id.c_str();
          }
        }
      }

    }  else if (xml.isCharacters() && !xml.isWhitespace()) {

      if (remoteCompound == NULL ) {
        qDebug() << "Parse Error: " << currentTag; continue;
        //return remoteCompounds;
      } else if (currentTag == "metabolite_id")
        remoteCompound->id = xml.text().toString().toStdString();

      else if (currentTag == "metabolite_name")
        remoteCompound->name = xml.text().toString().toStdString();

      else if (currentTag == "formula")
        remoteCompound->formula = xml.text().toString().toStdString();

      else if (currentTag == "kegg_id")
        remoteCompound->kegg_id = xml.text().toString().toStdString();

      else if (currentTag == "pubmed_id")
        remoteCompound->pubchem_id = xml.text().toString().toStdString();

      else if (currentTag == "hmdb_id")
        remoteCompound->hmdb_id = xml.text().toString().toStdString();

      else if (currentTag == "precursormz") {
        remoteCompound->precursorMz = xml.text().toString().toDouble();
        remoteCompound->mass = remoteCompound->precursorMz;
      }
      else if (currentTag == "productmz")
        remoteCompound->productMz = xml.text().toString().toDouble();

      else if (currentTag == "ce")
        remoteCompound->collisionEnergy = xml.text().toString().toDouble();

      else if (currentTag == "retentiontime")
        remoteCompound->expectedRt = xml.text().toString().toDouble();

      else if (currentTag == "method_id") {
        remoteCompound->db = xml.text().toString().toStdString();
        remoteCompound->method_id = xml.text().toString().toStdString();

      } else if (currentTag == "transition_id")
        remoteCompound->transition_id = xml.text().toString().toInt();

    }
  }

  if (xml.error() && xml.error() != QXmlStreamReader::PrematureEndOfDocumentError) {
    qWarning() << "XML ERROR:" << xml.lineNumber() << ": " << xml.errorString();
    http.abort();
  }


  return remoteCompounds;
}

