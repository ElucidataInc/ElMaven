#include <QPushButton>
#include <QTextEdit>

#include "alignmentvizallgroupswidget.h"
#include "Compound.h"
#include "eicwidget.h"
#include "globals.h"
#include "grouprtwidget.h"
#include "isotopeplot.h"
#include "isotopeswidget.h"
#include "ligandwidget.h"
#include "mainwindow.h"
#include "mavenparameters.h"
#include "mzfileio.h"
#include "mzSample.h"
#include "numeric_treewidgetitem.h"
#include "projectdockwidget.h"
#include "Scan.h"
#include "samplertwidget.h"
#include "spectrawidget.h"
#include "tabledockwidget.h"
#include "treedockwidget.h"

ProjectDockWidget::ProjectDockWidget(QMainWindow *parent):
    QDockWidget("Samples", parent,Qt::Widget)
{

    _mainwindow = (MainWindow*) parent;

    setFloating(false);
    setWindowTitle("Samples");
    setObjectName("Samples");

    QFont font;
    font.setFamily("Helvetica");
    font.setPointSize(10);

    lastUsedSampleColor = QColor(Qt::green);
    setLastOpenedProject("");
    setLastSavedProject("");

    _editor = new QTextEdit(this);
    _editor->setFont(font);
    _editor->setToolTip("Project Description.");
    _editor->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::MinimumExpanding);
    _editor->hide();

    _treeWidget = new ProjectTreeWidget(this);
    _treeWidget->setColumnCount(4);
    _treeWidget->setObjectName("Samples");
    _treeWidget->setHeaderHidden(true);
    _treeWidget->setDragDropMode(QAbstractItemView::InternalMove);
    connect(_treeWidget,SIGNAL(itemSelectionChanged()), SLOT(showInfo()));
    connect( _treeWidget->header(), SIGNAL( sectionClicked(int) ), this,  SLOT( changeSampleOrder() )  );

    QToolBar *toolBar = new QToolBar(this);
    toolBar->setFloatable(false);
    toolBar->setMovable(false);
    toolBar->setIconSize(QSize(24, 24));

    QToolButton* exportMetadataButton = new QToolButton(toolBar);
    // TODO: Replace this icon with something more appropriate
    exportMetadataButton->setIcon(QIcon(rsrcPath + "/exportMetadataTemplate.png"));
    exportMetadataButton->setToolTip("Exports a template file with sample "
                                     "names and which can be edited as a "
                                     "spreadsheet. This will ease the process "
                                     "of metadata file generation.");
    connect(exportMetadataButton, &QToolButton::clicked, [this] {
        auto loadedSamples = _mainwindow->getSamples();
        if (loadedSamples.empty())
            return;
        auto lastSample = loadedSamples.back();
        auto sampleDir = QFileInfo(lastSample->fileName.c_str()).dir();
        QString fileName = QFileDialog::getSaveFileName(
            this,
            "Save sample metadata template as (.CSV)",
            sampleDir.path(),
            "CSV(*.CSV)"
        );

        if (fileName.isEmpty())
            return;

        if (!fileName.endsWith(".csv", Qt::CaseInsensitive))
            fileName = fileName + ".csv";

        prepareSampleCohortFile(fileName);

        QMessageBox::information(this,
                                 "Meta-data file exported",
                                 QString("A file has been exported with your "
                                         "current sample meta-data at the "
                                         "location \"%1\". It can be edited "
                                         "and imported back into "
                                         "El-MAVEN.").arg(fileName));
    });

    QToolButton* loadMetadataButton = new QToolButton(toolBar);
    loadMetadataButton->setIcon(QIcon(rsrcPath + "/setupload.png"));
    loadMetadataButton->setToolTip("Load cohort sets, scaling factor and "
                                   "injection order from sample metadata "
                                   "file.");
    connect(loadMetadataButton,
            &QToolButton::clicked,
            _mainwindow,
            static_cast<void (MainWindow::*)(void)>(&MainWindow::loadMetaInformation));

    connect(_mainwindow,SIGNAL(metaCsvFileLoaded()),SLOT(updateSampleList()));

    QToolButton* colorButton = new QToolButton(toolBar);
    colorButton->setIcon(QIcon(rsrcPath + "/colorfill.png"));
    colorButton->setToolTip("Change Sample Color");
    connect(colorButton,SIGNAL(clicked()), SLOT(changeColors()));

    QToolButton* removeSamples = new QToolButton(toolBar);
    removeSamples->setIcon(QIcon(rsrcPath + "/delete.png"));
    removeSamples->setToolTip("Remove Samples");
    connect(removeSamples,SIGNAL(clicked()), SLOT(unloadSelectedSamples()));

    QToolButton* checkUncheck = new QToolButton(toolBar);
    checkUncheck->setIcon(QIcon(rsrcPath + "/checkuncheck.png"));
    checkUncheck->setToolTip("Show / Hide Selected Samples");
    connect(checkUncheck,SIGNAL(clicked()), SLOT(checkUncheck()));
    QToolButton* blankButton = new QToolButton(toolBar);
    blankButton->setIcon(QIcon(rsrcPath + "/blank sample.png"));
    blankButton->setToolTip("Set As a Blank Sample");
    connect(blankButton,SIGNAL(clicked()), SLOT(SetAsBlankSamples()));

    toolBar->addWidget(exportMetadataButton);
    toolBar->addWidget(loadMetadataButton);
    toolBar->addWidget(colorButton);
    toolBar->addWidget(removeSamples);
    toolBar->addWidget(checkUncheck);
    toolBar->addWidget(blankButton);

    QLineEdit*  filterEditor = new QLineEdit(this);
    filterEditor->setPlaceholderText("Sample name filter"); 
    connect(filterEditor, SIGNAL(textEdited(QString)), this, SLOT(filterTreeItems(QString)));

    QWidget *window = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->addWidget(filterEditor);
    layout->addWidget(_treeWidget);
    window->setLayout(layout);

    setTitleBarWidget(toolBar);
    setWidget(window);

}

QString ProjectDockWidget::getProjectDescription() {
    return _editor->toPlainText();
}


void ProjectDockWidget::setProjectDescription(QString text) {
    return _editor->setPlainText(text);
}

void ProjectDockWidget::changeSampleColor(QTreeWidgetItem* item, int col) {
    if (!item) item = _treeWidget->currentItem();
    if (item == NULL) return;

    if (col != 0) return;
    QVariant v = item->data(0,Qt::UserRole);

    mzSample*  sample =  v.value<mzSample*>();
    if ( sample == NULL) return;

     QColor color = QColor::fromRgbF(
            sample->color[0],
            sample->color[1],
            sample->color[2],
            sample->color[3] );

      lastUsedSampleColor = QColorDialog::getColor(color,this,"Select Sample Color",QColorDialog::ShowAlphaChannel);
      setSampleColor(item,lastUsedSampleColor);

      _treeWidget->update();
      _mainwindow->getEicWidget()->replot();
      _mainwindow->groupRtWidget->updateGraph();
}

void ProjectDockWidget::prepareSampleCohortFile(QString sampleCohortFileName) {
    QFile file(sampleCohortFileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;
	
    vector<mzSample*> loadedSamples = _mainwindow->getSamples();

	QTextStream out(&file);
    out << "Sample" << ","
        << "Cohort" << ","
        << "Scaling" << ","
        << "Injection Order" << "\n";
	for (const auto& sample : loadedSamples) {
        QString injectionOrder = "";
        if (sample->getInjectionOrder() > 0)
            injectionOrder = QString::number(sample->getInjectionOrder());

        out << QString::fromStdString(sample->getSampleName()) << ","
            << QString::fromStdString(sample->getSetName()) << ","
            << QString::number(sample->getNormalizationConstant()) << ","
            << injectionOrder << "\n";
	}

	qDebug() << "sample cohort file prepared";
	file.close();
}

void ProjectDockWidget::changeSampleSet(QTreeWidgetItem* item, int col) {

    if (item && item->type() == SampleType) {
        QVariant v = item->data(0,Qt::UserRole);
        mzSample*  sample =  v.value<mzSample*>();

        if ( col == 1 ) {
            QString setName = item->text(1).simplified();
            sample->setSetName(setName.toStdString());
        } else if ( col == 0) {
            QString sampleName = item->text(0).simplified();
            if (sampleName.length() > 0) {
                sample->setSampleName(sampleName.toStdString());
            }
        }
    }
}

void ProjectDockWidget::changeNormalizationConstant(QTreeWidgetItem* item, int col) {
    if (item && item->type() == SampleType && col == 2 ) {
        QVariant v = item->data(0,Qt::UserRole);
        mzSample*  sample =  v.value<mzSample*>();
        if ( sample == NULL) return;

        bool ok=false;
        float x = item->text(2).toFloat(&ok);
        if (ok) sample->setNormalizationConstant(x);
        cerr <<"changeSampleSet: " << sample->sampleName << "  " << sample->getNormalizationConstant() << endl;
    }
}

void ProjectDockWidget::updateSampleList() {

    vector<mzSample*>samples = _mainwindow->getSamples();
    std::sort(samples.begin(), samples.end(),mzSample::compSampleSort);

    // obtain current maximum sample order
    auto maxOrdSample = max_element(begin(samples),
                                    end(samples),
                                    [] (mzSample* s1, mzSample* s2) {
                                        return s1->getSampleOrder()
                                               < s2->getSampleOrder();
                                    });
    int maxOrder = 0;
    if (maxOrdSample != end(samples))
        maxOrder = (*maxOrdSample)->getSampleOrder();

    // assign sample order to any newly added samples
    for (const auto sample : samples) {
        if (sample->getSampleOrder() == -1)
            sample->setSampleOrder(++maxOrder);
    }

    if ( samples.size() > 0 ) setInfo(samples);

    if ( _mainwindow->getEicWidget() ) {
        _mainwindow->getEicWidget()->replotForced();
    }
    if (_mainwindow->isotopeWidget) {
        _mainwindow->isotopeWidget->updateSampleList();
    }
}

void ProjectDockWidget::selectSample(QTreeWidgetItem* item, int col) {
    if (item && item->type() == SampleType ) {
        _mainwindow->alignmentVizAllGroupsWidget->replotGraph();
        _mainwindow->sampleRtWidget->plotGraph();
        QVariant v = item->data(0,Qt::UserRole);
        mzSample*  sample =  v.value<mzSample*>();
        if (sample && sample->scans.size() > 0 ) {

            _mainwindow->spectraWidget->setScan(sample,-1);
            _mainwindow->getEicWidget()->replot();

        }
    }
}

void ProjectDockWidget::showInfo() {
    QTreeWidgetItem* item = _treeWidget->currentItem();
    if(item != NULL and item->type() == SampleType) showSample(item,0);
}

void ProjectDockWidget::changeSampleOrder() {

     QTreeWidgetItemIterator it(_treeWidget);
     int sampleOrder=0;
     bool changed = false;

     while (*it) {
         if ((*it)->type() == SampleType) {
            QVariant v =(*it)->data(0,Qt::UserRole);
            mzSample*  sample =  v.value<mzSample*>();
            if ( sample != NULL) {
                if ( sample->getSampleOrder() != sampleOrder )  changed=true;
                sample->setSampleOrder(sampleOrder);
                sampleOrder++;
            }
         }
         ++it;
    }

    if (changed) {
        _mainwindow->alignmentVizAllGroupsWidget->replotGraph();
        _mainwindow->sampleRtWidget->plotGraph();
        _mainwindow->groupRtWidget->updateGraph();
        _mainwindow->getEicWidget()->replot();
        _mainwindow->isotopeWidget->updateSampleList();
        _mainwindow->isotopePlot->replot();
        _mainwindow->fragPanel->sortScansBySample();
    }
}

void ProjectDockWidget::filterTreeItems(QString filterString) {
    QRegExp regexp(filterString,Qt::CaseInsensitive,QRegExp::RegExp);

    QTreeWidgetItemIterator it(_treeWidget);
    while (*it) {
        QTreeWidgetItem* item = (*it);
        ++it; //next item
        if (item->type() == SampleType) {
            if (filterString.isEmpty()) {
                item->setHidden(false);
            } else if (item->text(0).contains(regexp) || item->text(1).contains(regexp) || item->text(2).contains(regexp)) {
                item->setHidden(false);
            } else {
                item->setHidden(true);
            }
        }
    }

}

void ProjectDockWidget::changeColors() {

      //get selected items
      QList<QTreeWidgetItem*>selected = _treeWidget->selectedItems();
      if(selected.size() == 0) return;

      //ask for color
      lastUsedSampleColor = QColorDialog::getColor(lastUsedSampleColor,this,"Select Sample Color",QColorDialog::ShowAlphaChannel);

      //change colors of selected items
      Q_FOREACH (QTreeWidgetItem* item, selected) {
          if (item->type() == SampleType) setSampleColor(item,lastUsedSampleColor);
      }

      _treeWidget->update();

     _mainwindow->alignmentVizAllGroupsWidget->replotGraph();
     _mainwindow->sampleRtWidget->plotGraph();

      _mainwindow->getEicWidget()->replot();
}

void ProjectDockWidget::checkUncheck() {

      //get selected items
      QList<QTreeWidgetItem*>selected = _treeWidget->selectedItems();
      if(selected.size() == 0) return;

      qDebug() << "checkUncheck()" << selected.size();
      Q_FOREACH (QTreeWidgetItem* item, selected) {
          if (item->type() == SampleType) {
              QVariant v = item->data(0,Qt::UserRole);
              mzSample*  sample =  v.value<mzSample*>();
              sample->isSelected  ? item->setCheckState(0,Qt::Unchecked) : item->setCheckState(0,Qt::Checked);
          }
      }
     _treeWidget->update();

     _mainwindow->alignmentVizAllGroupsWidget->replotGraph();
     _mainwindow->sampleRtWidget->plotGraph();

      _mainwindow->getEicWidget()->replot();
}

void ProjectDockWidget::unloadSelectedSamples() {
      //get selected items
      QList<QTreeWidgetItem*>selected = _treeWidget->selectedItems();
      if(selected.size() == 0) return;

     //reverse loop as size will decrease on deleting sample
     for (int index = selected.size() - 1; index >= 0; --index)
      {
          QTreeWidgetItem* item = selected[index];
          if (item->type() == SampleType) {
              QVariant v = item->data(0,Qt::UserRole);
              mzSample*  sample =  v.value<mzSample*>();
              int i = _treeWidget->indexOfTopLevelItem(item);
              _treeWidget->takeTopLevelItem(i);
              delete item;
              unloadSample(sample);
              delete(sample);
           }
      }
     _treeWidget->update();
     _mainwindow->getEicWidget()->replotForced();
     _mainwindow->isotopeWidget->updateSampleList();

     // delete any parent folder items that now have zero samples
     for (const auto parentFolder : parentMap.keys()) {
         QTreeWidgetItem* parent = parentMap.value(parentFolder);
         if (parent->childCount() == 0) {
             parentMap.remove(parentFolder);
             delete parent;
         }
     }

     if (_mainwindow->samples.size() < 1) {
		QMessageBox* msgBox = new QMessageBox( this );
		msgBox->setStandardButtons( QMessageBox::Ok );
        QPushButton *connectButton = msgBox->addButton(tr("Restart"), QMessageBox::ActionRole);
		msgBox->setIcon(QMessageBox::Information);
		msgBox->setText(tr("All the samples have been deleted. \nPlease restart El-Maven if you want to process another set of samples for better experience."));
		msgBox->setModal( false );
        msgBox->exec();
        if (msgBox->clickedButton() == connectButton) {
            QSet<QString> fileNames;
            _mainwindow->reBootApp();
        }
        delete msgBox;
     }

    _mainwindow->alignmentVizAllGroupsWidget->replotGraph();
    _mainwindow->sampleRtWidget->plotGraph();

    emit samplesDeleted();
}

void ProjectDockWidget::SetAsBlankSamples() {
      
      int flag =0;
      //get selected items
      QList<QTreeWidgetItem*>selected = _treeWidget->selectedItems();
      if(selected.size() == 0) return;

      Q_FOREACH (QTreeWidgetItem* item, selected) {
          if (item->type() == SampleType) {
              QVariant v = item->data(0,Qt::UserRole);
              mzSample*  sample =  v.value<mzSample*>();
              if ( sample == NULL) return;
              if(!sample->isBlank) markBlank(item);
              else unmarkBlank(item);
           }
      }
     _treeWidget->update();
     _mainwindow->getEicWidget()->replotForced();
}

void ProjectDockWidget::unmarkBlank(QTreeWidgetItem* item) {
    QVariant v = item->data(0,Qt::UserRole);
    mzSample*  sample =  v.value<mzSample*>();
    if ( sample == NULL) return;

    sample->isBlank = false;

    //restore sample color
    QString sampleName = QString::fromStdString(sample->sampleName.c_str());
    setSampleColor(item, storeColor[sampleName]);
    
    QFont font;
    font.setItalic(false); 
    item->setFont(0,font);
}

void ProjectDockWidget::markBlank(QTreeWidgetItem* item) {
    QVariant v = item->data(0,Qt::UserRole);
    mzSample*  sample =  v.value<mzSample*>();
    if ( sample == NULL) return;
    
    sample->isBlank = true; //for manually marking blanks
    
    //Set sample color to black
    QString sampleName = QString::fromStdString(sample->sampleName.c_str());
    storeColor[sampleName] = QColor::fromRgbF(sample->color[0], sample->color[1],sample->color[2], 1.0);
    setSampleColor(item, QColor(Qt::black));
    
    //Sample name in italics
    QFont font;
    font.setItalic(true); 
    item->setFont(0,font);
}

void ProjectDockWidget::setSampleColor(QTreeWidgetItem* item, QColor color) {
    if (item == NULL) return;
    if (!color.isValid()) return;

    QVariant v = item->data(0,Qt::UserRole);
    mzSample*  sample =  v.value<mzSample*>();
    if ( sample == NULL) return;

    setSampleColor(sample,color);

    color.setAlphaF(0.7);
    QPixmap pixmap = QPixmap(20,20); pixmap.fill(color);
    QIcon coloricon = QIcon(pixmap);
    item->setIcon(0,coloricon);
    item->setBackgroundColor(0,color);
    item->setBackgroundColor(1,color);
}

void ProjectDockWidget::setSampleColor(mzSample* sample, QColor color)
{
    if (!color.isValid()) return;
    if ( sample == NULL) return;

    sample->color[0] = color.redF();
    sample->color[1] = color.greenF();
    sample->color[2] = color.blueF();
    sample->color[3] = color.alphaF();
    storeSampleColors[sample] = color;
}

QColor ProjectDockWidget::getSampleColor(mzSample* sample)
{
    if(!sample) return Qt::black;
    return QColor::fromRgbF(sample->color[0],
                            sample->color[1],
                            sample->color[2],
                            sample->color[3]);
}

QIcon ProjectDockWidget::getSampleIcon(mzSample* sample)
{
    QColor color = getSampleColor(sample);
    QPixmap pixmap = QPixmap(20, 20);
    pixmap.fill(color);
    return QIcon(pixmap);
}

void ProjectDockWidget::setInfo(vector<mzSample*>&samples) {

    if ( _treeWidget->topLevelItemCount() == 0 )  {
        _treeWidget->setMouseTracking(true);
        connect(_treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*, int)), SLOT(selectSample(QTreeWidgetItem*, int)));
        connect(_treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), SLOT(changeSampleColor(QTreeWidgetItem*,int)));
        connect(_treeWidget,SIGNAL(itemEntered(QTreeWidgetItem*, int)), SLOT(showSampleInfo(QTreeWidgetItem*,int)));
    }

    disconnect(_treeWidget,SIGNAL(itemChanged(QTreeWidgetItem*, int)),0,0);

    parentMap.clear();
    _treeWidget->clear();

    QStringList header;
    header << "Sample"
           << "Set"
           << "Scaling"
           << "Injection Order"
           << "Sample Number";
    _treeWidget->setHeaderLabels( header );
    _treeWidget->header()->setStretchLastSection(true);
    _treeWidget->setHeaderHidden(false);
    _treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    _treeWidget->setRootIsDecorated(true);
    _treeWidget->expandToDepth(10);

    float N = samples.size();
    sort(samples.begin(), samples.end(), mzSample::compSampleOrder);
    for(int i=0; i < samples.size(); i++ ) {

        mzSample* sample = samples[i];
        if (!sample) continue;

        sample->setSampleOrder(i);

        QTreeWidgetItem* parent = getParentFolder(QString(sample->fileName.c_str()));
        QTreeWidgetItem *item=NULL;

        if (parent) { 
            item = new NumericTreeWidgetItem(parent,SampleType); 
        } else {
            item = new NumericTreeWidgetItem(_treeWidget,SampleType); 
        }

        QColor color;
        if (storeSampleColors.contains(sample)) {
            color = storeSampleColors[sample];
        } else if (sample->color[0] || sample->color[1] || sample->color[2]) {
            color = getSampleColor(sample);
            storeSampleColors[sample] = color;
        } else { 
            float hue = 1 - 0.6 * ((float) (i + 1) / N);
            color = QColor::fromHsvF(hue, 1.0, 1.0, 1.0);
            storeSampleColors[sample] = color;
        }

        sample->color[0] = color.redF();
        sample->color[1] = color.greenF();
        sample->color[2] = color.blueF();
        sample->color[3] = color.alphaF();

        QPixmap pixmap = QPixmap(20, 20);
        pixmap.fill(color);
        QIcon coloricon = QIcon(pixmap);

        item->setBackgroundColor(0, color);
        item->setIcon(0, coloricon);
        item->setText(0, QString(sample->sampleName.c_str()));
        item->setData(0, Qt::UserRole,QVariant::fromValue(samples[i]));
        item->setIcon(1, QIcon(QPixmap(rsrcPath + "/edit.png")));
        item->setText(1, QString(sample->getSetName().c_str()));
        item->setText(2, QString::number(sample->getNormalizationConstant(), 'f', 2));
        if( sample->getInjectionOrder() > 0 )
            item->setText(3, QString::number(sample->getInjectionOrder()));
        else
            item->setText(3, QString("NA"));
        if(sample->sampleNumber != -1)
            item->setText(4, QString::number(sample->sampleNumber));
        else
            item->setText(4, QString("NA"));

        item->setFlags(Qt::ItemIsEditable|Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
        sample->isSelected  ? item->setCheckState(0,Qt::Checked) : item->setCheckState(0,Qt::Unchecked);
        item->setExpanded(true);
        
        //set blank to black italics
		if (sample->isBlank) {
            markBlank(item);
		}
    }

    _treeWidget->resizeColumnToContents(0);
    connect(_treeWidget,SIGNAL(itemChanged(QTreeWidgetItem*, int)), SLOT(changeSampleSet(QTreeWidgetItem*,int)));
    connect(_treeWidget,SIGNAL(itemChanged(QTreeWidgetItem*, int)), SLOT(changeNormalizationConstant(QTreeWidgetItem*,int)));
    connect(_treeWidget,SIGNAL(itemChanged(QTreeWidgetItem*, int)), SLOT(showSample(QTreeWidgetItem*,int)));
    connect(_treeWidget,
            SIGNAL(itemDropped(QTreeWidgetItem*)),
            SLOT(changeSampleOrder()));
    connect(_treeWidget,
            SIGNAL(itemsSorted()),
            SLOT(changeSampleOrder()));
    changeSampleOrder();
}

void ProjectDockWidget::showSample(QTreeWidgetItem* item, int col) {
    if (item == NULL) return;
    bool checked = (item->checkState(0) != Qt::Unchecked );
    QTreeWidgetItem* parent = item->parent();

    if (item->type() == SampleType ) {
        QVariant v = item->data(0,Qt::UserRole);
        mzSample*  sample =  v.value<mzSample*>();
        if (sample)  {
            bool changed=false;
            sample->isSelected != checked ? changed=true : changed=false;
            sample->isSelected=checked;

            if(changed) {
                cerr << "ProjectDockWidget::showSample() changed! " << checked << endl;
                _mainwindow->alignmentVizAllGroupsWidget->replotGraph();
                _mainwindow->sampleRtWidget->plotGraph();
                _mainwindow->groupRtWidget->updateGraph();
                _mainwindow->getEicWidget()->replotForced();
                _mainwindow->isotopeWidget->updateSampleList();
                _mainwindow->isotopePlot->replot();
            }
        }
    }
}

QTreeWidgetItem* ProjectDockWidget::getParentFolder(QString fileName) {
        //get parent name of the directory containg this sample
        QTreeWidgetItem* parent=NULL;
        if (!QFile::exists(fileName)) return NULL;

        QFileInfo fileinfo(fileName);
        QString path = fileinfo.absoluteFilePath();
        QStringList pathlist = path.split("/");
        if (pathlist.size() > 2 ) {
            QString parentFolder = pathlist[ pathlist.size()-2 ];
            if(parentMap.contains(parentFolder)) { 
                parent = parentMap[parentFolder];
            } else {
                parent = new QTreeWidgetItem(_treeWidget);
                parent->setText(0,parentFolder);
                parent->setExpanded(true);
                parentMap[parentFolder]=parent;
            }
        }
        return parent;
}

void ProjectDockWidget::showSampleInfo(QTreeWidgetItem* item, int col) {

    if (item && item->type() == SampleType ) {
        QVariant v = item->data(0,Qt::UserRole);
        mzSample*  sample =  v.value<mzSample*>();

        QString ionizationMode = "Unknown";
        sample->getPolarity() < 0 ? ionizationMode="Negative" :  ionizationMode="Positive";

        if (sample)  {
            this->setToolTip(tr("m/z Range: %1-%2<br> rt Range: %3-%4<br> Scan#: %5 <br> Ionization: %6<br> Filename: %7").
                   arg(sample->minMz).
                   arg(sample->maxMz).
                   arg(sample->minRt).
                   arg(sample->maxRt).
                   arg(sample->scanCount()).
                   arg(ionizationMode).
                   arg(sample->fileName.c_str()));
        }
    }
}

QTreeWidget* ProjectDockWidget::getTreeWidget(){
    return _treeWidget;
}

QString ProjectDockWidget::getLastOpenedProject()
{
    return _lastOpenedProject;
}

std::chrono::time_point<std::chrono::system_clock>
ProjectDockWidget::getLastOpenedTime()
{
    return _lastLoad;
}

void ProjectDockWidget::setLastOpenedProject(QString filename)
{
    _lastOpenedProject = filename;
    _lastLoad = std::chrono::system_clock::now();
}

QString ProjectDockWidget::getLastSavedProject()
{
    return _lastSavedProject;
}

std::chrono::time_point<std::chrono::system_clock>
ProjectDockWidget::getLastSavedTime()
{
    return _lastSave;
}

void ProjectDockWidget::setLastSavedProject(QString filename)
{
    _lastSavedProject = filename;
    _lastSave = std::chrono::system_clock::now();
}

void ProjectDockWidget::saveProjectAsSQLite()
{
    QSettings* settings = _mainwindow->getSettings();

    QString dir = ".";
    if (settings->contains("lastDir")) {
        QString ldir = settings->value("lastDir").value<QString>();
        QDir test(ldir);
        if (test.exists())
            dir = ldir;
    }
    QString fileName = QFileDialog::getSaveFileName(
        _mainwindow,
        "Save project as (.emDB)",
        dir,
        "emDB Project(*.emDB)"
    );

    if (fileName.isEmpty())
        return;

    if (!fileName.endsWith(".emDB", Qt::CaseInsensitive))
        fileName = fileName + ".emDB";

    _mainwindow->threadSave(fileName);
}

void ProjectDockWidget::saveSQLiteProject(QString filename)
{
    auto success = _mainwindow->fileLoader->writeSQLiteProject(filename);
    if (success && !_mainwindow->timestampFileExists)
        setLastSavedProject(filename);
}

void ProjectDockWidget::saveSQLiteProject()
{
    if (!_mainwindow->getLatestUserProject().isEmpty()) {
        _mainwindow->threadSave(_mainwindow->getLatestUserProject());
    } else {
        saveProjectAsSQLite();
    }
}

void ProjectDockWidget::savePeakGroupInSQLite(PeakGroup* group,
                                              QString filename)
{
    if (group == nullptr)
        return;

    if (!_mainwindow->fileLoader->sqliteProjectIsOpen()
            && !filename.isEmpty()) {
        saveSQLiteProject(filename);
    } else {
        auto tableName = QString::fromStdString(group->tableName());
        _mainwindow->fileLoader->updateGroup(group, tableName);
    }
}

void ProjectDockWidget::saveAndCloseCurrentSQLiteProject()
{
    // return if there are no samples, since its unlikely that the user has done
    // any sort of work yet
    if (_mainwindow->getSamples().size() == 0) {
        clearSession();
        return;
    }

    auto userSessionWarning = "Do you want to save your current session before "
                              "opening the project?";
    if (!_mainwindow->getLatestUserProject().isEmpty())
        userSessionWarning = "Do you want to save and close the current "
                             "project?";
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Opening Project",
        userSessionWarning,
        QMessageBox::No | QMessageBox::Yes,
        QMessageBox::Yes
    );
    if (reply == QMessageBox::Yes) {
        saveSQLiteProject();
    } else {
        _mainwindow->resetAutosave();
    }

    // if an existing project is being saved, stall before clearing the session
    while(_mainwindow->autosave->isRunning());

    _mainwindow->fileLoader->closeSQLiteProject();
    setLastSavedProject("");
    setLastOpenedProject("");

    // clear session regardless of whether the project was saved
    clearSession();
}

void ProjectDockWidget::clearSession()
{
    for (auto sample : _mainwindow->getSamples())
        unloadSample(sample);
    _mainwindow->removeAllPeakTables();
    _mainwindow->bookmarkedPeaks->deleteAll();
    TableDockWidget::clearTitleRegistry();
}

void ProjectDockWidget::loadMzRollProject(QString fileName) {
    int samplecount = 0;

    QFile data(fileName);
    if ( !data.open(QFile::ReadOnly) ) {
        QErrorMessage errDialog(this);
        errDialog.showMessage("File open: " + fileName + " failed");
        return;
    }

    QXmlStreamReader xml(&data);
    mzSample* currentSample=NULL;

    QString projectDescription;
    QStringRef currentXmlElement;

    int i=0;
    while(!xml.atEnd()){
        if (xml.isStartElement()) {
         if (xml.name() == "sample") {
        i++;}
        }
        xml.readNext();
    }
    data.seek(0);

    xml.setDevice(xml.device());
    QString progressText;
    while (!xml.atEnd()) {


        xml.readNext();
        if (xml.isStartElement()) {
            currentXmlElement = xml.name();


            if (xml.name() == "sample") {
                unsigned int id = 0;
                QString sname   = xml.attributes().value("name").toString();
                id = xml.attributes().value("id").toInt();
                QString fname   = xml.attributes().value("filename").toString();
                QString setname   = xml.attributes().value("setName").toString();
                QString sampleOrder   = xml.attributes().value("sampleOrder").toString();
                QString isSelected   = xml.attributes().value("isSelected").toString();

                bool checkLoaded=false;
                Q_FOREACH(mzSample* loadedFile, _mainwindow->getSamples()) {
                    if (QString(loadedFile->fileName.c_str())== fname) checkLoaded=true;
                }

                if(checkLoaded == true) continue;  // skip files that have been loaded already
                
                samplecount++;
                qDebug() << "Checking:" << fname;
                QFileInfo sampleFile(fname);
                if (!sampleFile.exists()) {
                    Q_FOREACH(QString path, _mainwindow->pathlist) {
                        fname= path + QDir::separator() + sampleFile.fileName();
                        qDebug() << "Checking if exists:" << fname;
                        sampleFile.setFile(fname);
                        if (sampleFile.exists())  break;
                    }
                }

                QDir d = QFileInfo(fname).absoluteDir();
                QString dStr = d.absolutePath();
                progressText = "Importing files from " + dStr;

                if ( !fname.isEmpty() ) {

                    mzSample* sample = _mainwindow->fileLoader->loadSample(fname);
                    if (sample) {
                        _mainwindow->addSample(sample);
                        currentSample=sample;
                        if (!sname.isEmpty() ) sample->sampleName = sname.toStdString();
                        if (id > 0) sample->setSampleId(id);
                        if (!setname.isEmpty() ) sample->setSetName(setname.toStdString());
                        if (!sampleOrder.isEmpty()) sample->setSampleOrder(sampleOrder.toInt());
                        if (!isSelected.isEmpty()) sample->isSelected = isSelected.toInt();
                    } else {
                        currentSample=NULL;
                    }
                }
            }

			//change sample color
            if (xml.name() == "color" && currentSample) {
                currentSample->color[0]   = xml.attributes().value("red").toString().toDouble();
                currentSample->color[1]   = xml.attributes().value("blue").toString().toDouble();
                currentSample->color[2]   = xml.attributes().value("green").toString().toDouble();
                currentSample->color[3]  = xml.attributes().value("alpha").toString().toDouble();
            }

			//polynomialAlignmentTransformation vector
            if (xml.name() == "polynomialAlignmentTransformation" && currentSample) {
				vector<double>transform;
				Q_FOREACH(QXmlStreamAttribute coef, xml.attributes() ) {
					double coefValue =coef.value().toString().toDouble();
					transform.push_back(coefValue);
				}
				qDebug() << "polynomialAlignmentTransformation: "; printF(transform);
				currentSample->polynomialAlignmentTransformation = transform;
				currentSample->saveCurrentRetentionTimes();
				currentSample->applyPolynomialTransform();
			}
        }
        if (xml.isCharacters() && currentXmlElement == "projectDescription") {
            projectDescription.append( xml.text() );
        }
    sendBoostSignal(progressText.toStdString(), samplecount, i);
    }
    data.close();

    setLastOpenedProject(fileName);
}

void ProjectDockWidget::keyPressEvent(QKeyEvent *e ) {
    //cerr << "TableDockWidget::keyPressEvent()" << e->key() << endl;

    QTreeWidgetItem *item = _treeWidget->currentItem();
    if (e->key() == Qt::Key_Delete ) {
        unloadSelectedSamples();
    }

    QDockWidget::keyPressEvent(e);
}

void ProjectDockWidget::unloadSample(mzSample* sample) {
    if ( sample == NULL) return;

    //mark sample as unselected
    sample->isSelected=false;
    delete_all(sample->scans);

    QList< QPointer<TableDockWidget> > peaksTableList = _mainwindow->getPeakTableList();
    peaksTableList.prepend(_mainwindow->getBookmarkedPeaks());
    TableDockWidget* peaksTable;
    Q_FOREACH(peaksTable, peaksTableList) {
        PeakGroup* grp;
        Q_FOREACH(grp, peaksTable->getGroups()) {
            vector<Peak>& peaks = grp->getPeaks();
            for(unsigned int j=0; j< peaks.size(); j++) {
                Peak p = peaks.at(j);
                if (p.getSample()->sampleName == sample->sampleName) {
                    peaks.erase(peaks.begin()+j);
                }
            }
        }
    }

    //remove sample from sample list
    for(unsigned int i=0; i<_mainwindow->samples.size(); i++) {
        if (_mainwindow->samples[i] == sample) {
            _mainwindow->samples.erase( _mainwindow->samples.begin()+i);
            break;
        }
    }

    //remove sample from maven parameters sample list
    for(unsigned int i=0; i<_mainwindow->mavenParameters->samples.size(); i++) {
        if (_mainwindow->mavenParameters->samples[i] == sample) {
            _mainwindow->mavenParameters->samples.erase( _mainwindow->mavenParameters->samples.begin()+i);
            break;
        }
    }
}

ProjectTreeWidget::ProjectTreeWidget(QWidget* parent) : QTreeWidget(parent)
{
    _draggedItem = nullptr;

    auto currentHeader = header();
    auto newHeader = new ProjectHeaderView(currentHeader->orientation(),
                                           currentHeader->parentWidget());
    setHeader(newHeader);
    connect(newHeader,
            &ProjectHeaderView::sortRequested,
            this,
            [=](int column, Qt::SortOrder sortOrder) {
                sortByColumn(column, sortOrder);
                emit itemsSorted();
            });
}

void ProjectTreeWidget::dragEnterEvent(QDragEnterEvent* event)
{
    _draggedItem = currentItem();
    QTreeWidget::dragEnterEvent(event);
}

void ProjectTreeWidget::dropEvent(QDropEvent* event)
{
    QModelIndex droppedIndex = indexAt(event->pos());
    if (!droppedIndex.isValid())
        return;

    if (_draggedItem) {
        QTreeWidgetItem* itemParent = _draggedItem->parent();
        if (itemParent) {
            // we are not allowing samples to be dropped in a different sample
            // set (at least not yet)
            if (itemFromIndex(droppedIndex.parent()) != itemParent)
                return;
        }
    }
    QTreeWidget::dropEvent(event);
    QTreeWidgetItem* item = currentItem();
    emit itemDropped(item);
    header()->setSortIndicatorShown(false);
}

ProjectHeaderView::ProjectHeaderView(Qt::Orientation orientation,
                                     QWidget* parent)
    : QHeaderView(orientation, parent)
{
    setSectionsMovable(true);
    setSectionsClickable(true);
    setSectionResizeMode(QHeaderView::ResizeToContents);
    setDefaultAlignment(Qt::AlignLeft);
}

void ProjectHeaderView::mouseReleaseEvent(QMouseEvent* event)
{
    auto column = logicalIndexAt(event->pos());
    if (column > -1) {
        if (isSortIndicatorShown()
            && sortIndicatorSection() == column
            && sortIndicatorOrder() == Qt::AscendingOrder) {
            setSortIndicator(column, Qt::DescendingOrder);
            emit sortRequested(column, Qt::DescendingOrder);
        } else {
            setSortIndicatorShown(true);
            setSortIndicator(column, Qt::AscendingOrder);
            emit sortRequested(column, Qt::AscendingOrder);
        }
    }
    QHeaderView::mouseReleaseEvent(event);
}
