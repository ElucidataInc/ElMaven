#include "ligandwidget.h"
#include "adductwidget.h"
#include "Compound.h"
#include "Scan.h"
#include "alignmentdialog.h"
#include "common/analytics.h"
#include "globals.h"
#include "librarymanager.h"
#include "mainwindow.h"
#include "masscalcgui.h"
#include "mavenparameters.h"
#include "mzSample.h"
#include "mzfileio.h"
#include "numeric_treewidgetitem.h"
#include "settingsform.h"
#include "spectramatching.h"

using namespace std;

LigandWidget::LigandWidget(MainWindow* mw)
{
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
    treeWidget->setObjectName("CompoundTable");
    treeWidget->setDragDropMode(QAbstractItemView::DragOnly);
    treeWidget->setMouseTracking(true);

    connect(treeWidget, SIGNAL(itemSelectionChanged()), SLOT(showLigand()));
    connect(treeWidget,
            SIGNAL(itemClicked(QTreeWidgetItem*, int)),
            SLOT(showLigand()));

    QToolBar* toolBar = new QToolBar(this);
    toolBar->setFloatable(false);
    toolBar->setMovable(false);
    toolBar->setIconSize(QSize(24, 24));
    QString style = "";
    style += "QToolBar { background:    white;               }";
    style += "QToolBar { border:        none;                }";
    style += "QToolBar { border-bottom: 1px solid lightgray; }";
    toolBar->setStyleSheet(style);

    databaseSelect = new QComboBox(toolBar);
    databaseSelect->setObjectName(QString::fromUtf8("databaseSelect"));
    databaseSelect->setDuplicatesEnabled(false);
    databaseSelect->setSizePolicy(QSizePolicy::Expanding,
                                  QSizePolicy::MinimumExpanding);

    connect(this,
            SIGNAL(compoundFocused(Compound*)),
            mw,
            SLOT(setCompoundFocus(Compound*)));
    connect(this, SIGNAL(urlChanged(QString)), mw, SLOT(setUrl(QString)));

    libraryButton = new QToolButton(toolBar);
    libraryButton->setIcon(QIcon(rsrcPath + "/librarymanager.png"));
    libraryButton->setToolTip("Open Library Manager");
    connect(libraryButton,
            &QPushButton::clicked,
            _mw->getLibraryManager(),
            &LibraryManager::exec);

    toolBar->addWidget(databaseSelect);
    toolBar->addWidget(libraryButton);

    filterEditor = new QLineEdit(toolBar);
    filterEditor->setPlaceholderText("Compound Name Filter");
    connect(filterEditor,
            SIGNAL(textEdited(QString)),
            this,
            SLOT(showMatches(QString)));

    QWidget* window = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->addWidget(filterEditor);
    layout->addWidget(treeWidget);
    window->setLayout(layout);

    setWidget(window);
    setTitleBarWidget(toolBar);
    setWindowTitle("Compounds");

    QDirIterator itr(":/databases/");
    while(itr.hasNext()) {
        auto filename = itr.next().toStdString();
        string dbname = mzUtils::cleanFilename(filename);
        _mw->massCalcWidget->database->addItem(QString::fromStdString(dbname));

        QFile file(QString(filename.c_str()));
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) 
            return;
        QTextStream in(&file);
        QString text;    
        text = in.readAll();
        string allContent = text.toStdString();
        string sep = ",";
        DB.loadCompoundCSVFile(allContent, true, dbname, sep);
        file.close();     
    }

    QSet<QString> set;
    auto compoundsDB = DB.compoundsDB();
    for (int i = 0; i < compoundsDB.size(); i++) {
        if (!set.contains(compoundsDB[i]->db().c_str()))
            set.insert(compoundsDB[i]->db().c_str());
    }

    QSetIterator<QString> i(set);
    while (i.hasNext())
        databaseSelect->addItem(i.next());

    connect(databaseSelect,
            SIGNAL(currentIndexChanged(QString)),
            this,
            SLOT(setDatabase(QString)));
    connect(this, SIGNAL(databaseChanged(QString)), _mw, SLOT(showSRMList()));
}

QString LigandWidget::getDatabaseName()
{
    return databaseSelect->currentText();
}

void LigandWidget::setDatabaseNames()
{
    databaseSelect->disconnect(SIGNAL(currentIndexChanged(QString)));

    databaseSelect->clear();
    QSet<QString> set;
    auto compoundsDB = DB.compoundsDB();
    for (int i = 0; i < compoundsDB.size(); i++) {
        if (!set.contains(compoundsDB[i]->db().c_str()))
            set.insert(compoundsDB[i]->db().c_str());
    }

    QSetIterator<QString> i(set);
    while (i.hasNext())
        databaseSelect->addItem(i.next());

    connect(databaseSelect,
            SIGNAL(currentIndexChanged(QString)),
            this,
            SLOT(setDatabase(QString)));
    connect(databaseSelect,
            SIGNAL(currentIndexChanged(QString)),
            _mw->alignmentDialog,
            SLOT(setDatabase(QString)));
    connect(this, SIGNAL(databaseChanged(QString)), _mw, SLOT(showSRMList()));
}

void LigandWidget::loadCompoundDBMzroll(QString fileName)
{
    QFile data(fileName);
    if (!data.open(QFile::ReadOnly)) {
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
                dbname =
                    xml.attributes().value("name").toString().toStdString();
            }
            if (xml.name() == "compound") {
                readCompoundXML(xml, dbname);
            }
        }
    }

    Q_EMIT(mzrollSetDB( QString::fromStdString(dbname)));
}

void LigandWidget::readCompoundXML(QXmlStreamReader& xml, string dbname)
{
    if (dbname == "")
        return;
    string id, name, formula;
    float rt = 0;
    float mz = 0;
    float charge = 0;
    float collisionenergy = 0;
    float precursormz = 0;
    float productmz = 0;
    vector<string> categorylist;

    id = xml.attributes().value("id").toString().toStdString();
    name = xml.attributes().value("name").toString().toStdString();
    mz = xml.attributes().value("mz").toString().toFloat();
    rt = xml.attributes().value("rt").toString().toFloat();
    charge = xml.attributes().value("Charge").toString().toInt();
    formula = xml.attributes().value("Formula").toString().toStdString();
    precursormz = xml.attributes().value("precursorMz").toString().toFloat();
    productmz = xml.attributes().value("productMz").toString().toFloat();
    collisionenergy =
        xml.attributes().value("collisionEnergy").toString().toFloat();

    while (xml.readNextStartElement()) {
        if (xml.name() == "categories") {
            Q_FOREACH (const QXmlStreamAttribute& attr, xml.attributes()) {
                if (categorylist.size() == 0)
                    categorylist.push_back(xml.attributes()
                                               .value(attr.name().toString())
                                               .toString()
                                               .toStdString());
            }
        }
    }

    Compound* compound = new Compound(id, name, formula, charge);
    compound->setExpectedRt(rt);
    compound->setMz(mz);
    compound->setDb(dbname);
    compound->setPrecursorMz(precursormz);
    compound->setProductMz(productmz);
    compound->setCollisionEnergy(collisionenergy);
    vector<string> category;
    for (int i = 0; i < categorylist.size(); i++)
        category.push_back(categorylist[i]);
    compound->setCategory(category);
    DB.addCompound(compound);
}

void LigandWidget::setDatabase(QString dbname)
{
    int index = databaseSelect->findText(dbname, Qt::MatchExactly);
    if (index != -1) {
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

void LigandWidget::databaseChanged(int index)
{
    QString dbname = databaseSelect->currentText();
    setDatabase(dbname);
}

void LigandWidget::setCompoundFocus(Compound* c)
{
    if (c == NULL)
        return;
    QString dbname(c->db().c_str());
    int index = databaseSelect->findText(dbname, Qt::MatchExactly);
    if (index != -1)
        databaseSelect->setCurrentIndex(index);

    QString filterString(c->name().c_str());
    setWindowTitle("Compounds: " + filterString);
    showTable();
}

void LigandWidget::setFilterString(QString s)
{
    if (s != filterString) {
        filterString = s;
        filterEditor->setText(filterString);
    }
}

void LigandWidget::showMatches(QString needle)
{
    QRegExp regexp(needle, Qt::CaseInsensitive, QRegExp::RegExp);
    if (!regexp.isValid())
        return;

    QTreeWidgetItemIterator itr(treeWidget);
    while (*itr) {
        QTreeWidgetItem* item = (*itr);
        QVariant v = item->data(0, Qt::UserRole);
        Compound* compound = v.value<Compound*>();
        if (compound) {
            item->setHidden(true);
            if (needle.isEmpty()) {
                item->setHidden(false);
            } else if (item->text(0).contains(regexp)) {
                item->setHidden(false);
            } else {
                QStringList stack;
                stack << compound->name().c_str() << compound->id().c_str()
                      << compound->formula().c_str();

                if (compound->category().size()) {
                    auto category = compound->category();
                    for (int i = 0; i < category.size(); i++) {
                        stack << category[i].c_str();
                    }
                }

                Q_FOREACH (QString x, stack) {
                    if (x.contains(regexp)) {
                        item->setHidden(false);
                        break;
                    }
                }
            }
        }
        ++itr;
    }
}

void LigandWidget::updateCurrentItemData()
{
    QTreeWidgetItem* item = treeWidget->selectedItems().first();
    if (!item)
        return;
    QVariant v = item->data(0, Qt::UserRole);
    Compound* c = v.value<Compound*>();
    if (!c)
        return;

    QString mass = QString::number(c->mz());
    QString rt = QString::number(c->expectedRt());
    item->setText(1, mass);
    item->setText(2, rt);
}

void LigandWidget::showTable()
{
    treeWidget->clear();
    QStringList header;
    header << "Name"
           << "Formula"
           << "m/z"
           << "RT"
           << "# fragments"
           << "Category"
           << "Notes";
    treeWidget->setHeaderLabels(header);
    treeWidget->setSortingEnabled(false);

    int numCompoundsWithFormula = 0;
    int numCompoundsWithRt = 0;
    int numCompoundsWithFragments = 0;
    int numCompoundsWithCategory = 0;
    int numCompoundsWithNotes = 0;
    string dbname = databaseSelect->currentText().toStdString();
    auto compoundsDB = DB.compoundsDB();
    for (unsigned int i = 0; i < compoundsDB.size(); i++) {
        Compound* compound = compoundsDB[i];
        if (compound->db() != dbname)
            continue;  // skip compounds from other databases

        NumericTreeWidgetItem* item = new NumericTreeWidgetItem(treeWidget,
                                                                CompoundType);
        item->setText(0, QString::fromStdString(compound->name()));
        item->setData(0, Qt::UserRole, QVariant::fromValue(compound));
        item->setFlags(Qt::ItemIsSelectable
                         | Qt::ItemIsDragEnabled
                         | Qt::ItemIsEnabled);

        if (!compound->formula().empty()) {
            item->setText(1, QString::fromStdString(compound->formula()));
            ++numCompoundsWithFormula;
        }

        float mz;
        float precursorMz = compound->precursorMz();
        float productMz = compound->productMz();
        if (!compound->formula().empty() || compound->neutralMass() > 0.0f) {
            int charge = _mw->mavenParameters->getCharge(compound);
            mz = compound->adjustedMass(charge);
        } else {
            mz = compound->mz();
        }
        if (precursorMz > 0 && productMz > 0 && productMz <= precursorMz) {
            QString transitionString = QString("%1 / %2 (CE: %3)").arg(
                QString::number(precursorMz, 'f', 3),
                QString::number(productMz, 'f', 3),
                QString::number(compound->collisionEnergy(), 'f', 2));
            item->setText(2, transitionString);
        } else {
            item->setText(2, QString::number(mz, 'f', 6));
        }

        if (compound->expectedRt() > 0) {
            item->setText(3, QString::number(compound->expectedRt(), 'f', 2));
            ++numCompoundsWithRt;
        }

        if (!compound->fragmentMzValues().empty()) {
            item->setText(4,
                          QString::number(compound->fragmentMzValues().size()));
            ++numCompoundsWithFragments;
        }

        if (!compound->category().empty()) {
            QStringList catList;
            auto category = compound->category();
            for (unsigned int i = 0; i < category.size(); i++) {
                catList << category[i].c_str();
            }
            item->setText(5, catList.join(", "));
            ++numCompoundsWithCategory;
        }

        if (!compound->note().empty()) {
            item->setText(6, QString::fromStdString(compound->note()));
            ++numCompoundsWithNotes;
        }
    }

    treeWidget->setColumnWidth(0, 250);
    treeWidget->resizeColumnToContents(2);
    if (numCompoundsWithFormula == 0) {
        treeWidget->hideColumn(1);
    } else {
        treeWidget->showColumn(1);
        treeWidget->resizeColumnToContents(1);
    }
    if (numCompoundsWithRt == 0) {
        treeWidget->hideColumn(3);
    } else {
        treeWidget->showColumn(3);
        treeWidget->resizeColumnToContents(3);
    }
    if (numCompoundsWithFragments == 0) {
        treeWidget->hideColumn(4);
    } else {
        treeWidget->showColumn(4);
        treeWidget->resizeColumnToContents(4);
    }
    if (numCompoundsWithCategory == 0) {
        treeWidget->hideColumn(5);
    } else {
        treeWidget->showColumn(5);
    }
    if (numCompoundsWithNotes == 0) {
        treeWidget->hideColumn(6);
    } else {
        treeWidget->showColumn(6);
    }

    setHash();
    treeWidget->sortByColumn(0, Qt::AscendingOrder);
    treeWidget->setSortingEnabled(true);
}

void LigandWidget::setHash()
{
    CompoundsHash.clear();
    QTreeWidgetItemIterator itr(treeWidget);

    while (*itr) {
        QTreeWidgetItem* item = (*itr);
        if (item->parent() != nullptr)
            continue;

        QVariant v = item->data(0, Qt::UserRole);
        Compound* c = v.value<Compound*>();
        CompoundsHash.insert(c, item);
        ++itr;
    }
}

void LigandWidget::markAsDone(Compound* compound, bool isProxy)
{
    if (compound == nullptr)
        return;

    auto color = QColor(101, 243, 124, 100);  // green
    if (isProxy)
        color = QColor(253, 204, 101, 100);   // yellow
    auto i = CompoundsHash.find(compound);
    if (i != CompoundsHash.end() & i.key() == compound) {
        QTreeWidgetItem* item = i.value();
        if (item != nullptr) {
            for (int col = 0; col < treeWidget->columnCount(); col++)
                item->setBackground(col, color);
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

void LigandWidget::saveCompoundList(QString fileName, QString dbname)
{
    if (fileName.isEmpty())
        return;

    QString SEP = "\t";
    if (fileName.endsWith(".csv", Qt::CaseInsensitive)) {
        SEP = ",";
    } else if (!fileName.endsWith(".tab", Qt::CaseInsensitive)) {
        fileName = fileName + tr(".tab");
        SEP = "\t";
    }

    QFile data(fileName);
    if (data.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&data);

        // header
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
        
        auto compoundsDB = DB.compoundsDB();
        for (unsigned int i = 0; i < compoundsDB.size(); i++) {
            Compound* compound = compoundsDB[i];
            if (compound->db() != dbname.toStdString())
                continue;

            QString charpolarity;
            if (compound->charge() > 0)
                charpolarity = "+";
            if (compound->charge() < 0)
                charpolarity = "-";

            QStringList category;
            auto categoryVect = compound->category();
            for (int i = 0; i < categoryVect.size(); i++) {
                category << QString(categoryVect[i].c_str());
            }

            out << charpolarity << SEP;
            out << QString(compound->name().c_str()) << SEP;
            out << compound->mz() << SEP;
            out << compound->charge() << SEP;
            out << compound->precursorMz() << SEP;
            out << compound->collisionEnergy() << SEP;
            out << compound->productMz() << SEP;
            out << compound->expectedRt() << SEP;
            out << compound->id().c_str() << SEP;
            out << compound->formula().c_str() << SEP;
            out << compound->srmId().c_str() << SEP;
            out << category.join(";") << SEP;
            out << "\n";
        }
    }
}

void LigandWidget::showNext()
{
    QTreeWidgetItem* item = treeWidget->currentItem();
    if (item && treeWidget->itemBelow(item)) {
        treeWidget->setCurrentItem(treeWidget->itemBelow(item));
    }
}

void LigandWidget::showLast()
{
    QTreeWidgetItem* item = treeWidget->currentItem();
    if (item && treeWidget->itemAbove(item)) {
        treeWidget->setCurrentItem(treeWidget->itemAbove(item));
    }
}

void LigandWidget::showLigand()
{
    if (!_mw)
        return;

    Q_FOREACH (QTreeWidgetItem* item, treeWidget->selectedItems()) {
        QVariant v = item->data(0, Qt::UserRole);
        Compound* compound = v.value<Compound*>();
        if (compound != nullptr) {
            Isotope isotope;
            Adduct* adduct = nullptr;
            if (_mw->mavenParameters->searchAdducts)
                adduct = _mw->adductWidget->defaultAdduct();
            if (_mw->mavenParameters->pullIsotopesFlag
                && !compound->formula().empty()) {
                adduct = _mw->adductWidget->defaultAdduct();
                bool findC13 = _mw->mavenParameters->C13Labeled_BPE;
                bool findN15 = _mw->mavenParameters->N15Labeled_BPE;
                bool findS34 = _mw->mavenParameters->S34Labeled_BPE;
                bool findD2 = _mw->mavenParameters->D2Labeled_BPE;
                int charge = _mw->mavenParameters->getCharge(compound);
                auto isotopes =
                    MassCalculator::computeIsotopes(compound->formula(),
                                                    charge,
                                                    findC13,
                                                    findN15,
                                                    findS34,
                                                    findD2,
                                                    adduct);
                auto c12IsotopePos = find_if(begin(isotopes),
                                             end(isotopes),
                                             [] (Isotope iso) {
                                                 return (iso.name
                                                         == C12_PARENT_LABEL);
                                             });
                if (c12IsotopePos != end(isotopes))
                    isotope = *c12IsotopePos;
            }
            _mw->setCompoundFocus(compound, isotope, adduct);
            matchFragmentation();
        }
    }
}

Compound* LigandWidget::getSelectedCompound()
{
    auto selectedItems = treeWidget->selectedItems();
    if (selectedItems.isEmpty())
        return nullptr;

    QTreeWidgetItem* item = selectedItems.first();
    if (!item)
        return NULL;

    QVariant v = item->data(0, Qt::UserRole);
    Compound* c = v.value<Compound*>();
    if (!c)
        return NULL;

    return c;
}

void LigandWidget::matchFragmentation()
{
    Compound* c = getSelectedCompound();
    if (!c or c->fragmentMzValues().size() == 0)
        return;

    QStringList searchText;
    int mzCount = c->fragmentMzValues().size();
    int intsCount = c->fragmentIntensities().size();

    // user specified ionization mode
    int charge = _mw->mavenParameters->getCharge(c);
    float precursorMz = c->precursorMz();
    if (!c->formula().empty())
        precursorMz = c->adjustedMass(charge);
    auto mzValues = c->fragmentMzValues();
    auto intensities = c->fragmentIntensities();
    for (int i = 0; i < mzCount; i++) {
        float mz = mzValues[i];
        float ints = 0;
        if (i < intsCount)
            ints = intensities[i];

        searchText << tr("%1\t%2")
                          .arg(QString::number(mz, 'f', 5))
                          .arg(QString::number(ints, 'f', 2));
    }

    _mw->spectraMatchingForm->fragmentsText->setPlainText(
        searchText.join("\n"));
    _mw->spectraMatchingForm->precursorMz->setText(
        QString::number(precursorMz, 'f', 6));
}
