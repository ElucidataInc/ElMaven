#include "projectdockwidget.h"

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


    _editor = new QTextEdit(this);
    _editor->setFont(font);
    _editor->setToolTip("Project Description.");
    _editor->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::MinimumExpanding);
    _editor->hide();

    _treeWidget=new QTreeWidget(this);
    _treeWidget->setColumnCount(4);
    _treeWidget->setSortingEnabled(true);
    _treeWidget->setObjectName("Samples");
    _treeWidget->setHeaderHidden(true);
    connect(_treeWidget,SIGNAL(itemSelectionChanged()), SLOT(showInfo()));
    connect( _treeWidget->header(), SIGNAL( sectionClicked(int) ), this,  SLOT( changeSampleOrder() )  );

    QToolBar *toolBar = new QToolBar(this);
    toolBar->setFloatable(false);
    toolBar->setMovable(false);

    QToolButton* loadButton = new QToolButton(toolBar);
    loadButton->setIcon(QIcon(rsrcPath + "/fileopen.png"));
    loadButton->setToolTip("Load Project");
    //Trigger to open() in slot to load samples while uploading .mzroll file --@Giridhari
    connect(loadButton,SIGNAL(clicked()),_mainwindow, SLOT(open()));

    QToolButton* loadMetaDataButton = new QToolButton(toolBar);
    loadMetaDataButton->setIcon(QIcon(rsrcPath + "/setupload.png"));
    loadMetaDataButton->setToolTip("Load Sets");
    connect(loadMetaDataButton,SIGNAL(clicked()),_mainwindow, SLOT(loadMetaInformation()));
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


    toolBar->addWidget(loadButton);
    toolBar->addWidget(loadMetaDataButton);
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
      _mainwindow->alignmentVizWidget->updateGraph();
}

void ProjectDockWidget::prepareSampleCohortFile(QString sampleCohortFileName) {
    QFile file(sampleCohortFileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;
	
    vector<mzSample*> loadedSamples = _mainwindow->getSamples();

	QTextStream out(&file);
	out << "Sample" << ",Cohort" << "\n";
	for (const auto& sample : loadedSamples) {
		out << QString::fromStdString(sample->getSampleName()) << "," << QString::fromStdString(sample->getSetName()) << "\n";
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
        _mainwindow->alignmentVizWidget->updateGraph();
        _mainwindow->getEicWidget()->replot();
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
     if (_mainwindow->samples.size() < 1) {
		QMessageBox* msgBox = new QMessageBox( this );
		msgBox->setAttribute( Qt::WA_DeleteOnClose );
		msgBox->setStandardButtons( QMessageBox::Ok );
        QPushButton *connectButton = msgBox->addButton(tr("Restart"), QMessageBox::ActionRole);
		msgBox->setIcon(QMessageBox::Information);
		msgBox->setText(tr("All the samples have been deleted. \nPlease restart El-Maven if you want to process another set of samples for better experience."));
		msgBox->setModal( false );
		msgBox->open();
        msgBox->exec();
        if (msgBox->clickedButton() == connectButton) {
            QSet<QString> fileNames;
            _mainwindow->SaveMzrollListvar.clear();
            _mainwindow->reBootApp();
        }
     }

    _mainwindow->alignmentVizAllGroupsWidget->replotGraph();
    _mainwindow->sampleRtWidget->plotGraph();
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

void ProjectDockWidget::setSampleColor(mzSample* sample, QColor color) {
    if (!color.isValid()) return;
    if ( sample == NULL) return;

    sample->color[0] = color.redF();
    sample->color[1] = color.greenF();
    sample->color[2] = color.blueF();
    sample->color[3] = color.alphaF();
}


void ProjectDockWidget::setInfo(vector<mzSample*>&samples) {

    if ( _treeWidget->topLevelItemCount() == 0 )  {
        _treeWidget->setAcceptDrops(true);
        _treeWidget->setMouseTracking(true);
        connect(_treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*, int)), SLOT(selectSample(QTreeWidgetItem*, int)));
        connect(_treeWidget,SIGNAL(itemPressed(QTreeWidgetItem*, int)), SLOT(changeSampleOrder()));
        connect(_treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), SLOT(changeSampleColor(QTreeWidgetItem*,int)));
        connect(_treeWidget,SIGNAL(itemEntered(QTreeWidgetItem*, int)), SLOT(showSampleInfo(QTreeWidgetItem*,int)));
    }

    disconnect(_treeWidget,SIGNAL(itemChanged(QTreeWidgetItem*, int)),0,0);

    parentMap.clear();
    _treeWidget->clear();

    _treeWidget->setDragDropMode(QAbstractItemView::InternalMove);
    QStringList header;
    header << "Sample" << "Set" << "Scaling" << "Injection Order";
    _treeWidget->setHeaderLabels( header );
    _treeWidget->header()->setStretchLastSection(true);
    _treeWidget->setHeaderHidden(false);
    _treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    _treeWidget->setRootIsDecorated(true);
    _treeWidget->expandToDepth(10);

    float N = samples.size();
    sort(samples.begin(), samples.end(), mzSample::compSampleOrder);
    reverse(samples.begin(),samples.end());
    for(int i=0; i < samples.size(); i++ ) {

        mzSample* sample = samples[i];
        if (!sample) continue;

        sample->setSampleOrder(i);

		float hue = 1 - 0.6 * ((float) (i + 1) / N);
		QColor c = QColor::fromHsvF(hue, 1.0, 1.0, 1.0);

        storeSampleColors[sample] = c;

		sample->color[0] = c.redF();
		sample->color[1] = c.greenF();
		sample->color[2] = c.blueF();
		sample->color[3] = c.alphaF();

        
        QTreeWidgetItem* parent = getParentFolder(QString(sample->fileName.c_str()));
        QTreeWidgetItem *item=NULL;

        if (parent) { 
            item = new NumericTreeWidgetItem(parent,SampleType); 
        } else {
            item = new NumericTreeWidgetItem(_treeWidget,SampleType); 
        }

        QColor color = QColor::fromRgbF( sample->color[0], sample->color[1], sample->color[2], sample->color[3] );

        QPixmap pixmap = QPixmap(20,20); pixmap.fill(color); QIcon coloricon = QIcon(pixmap);

        item->setBackgroundColor(0,color);
        item->setIcon(0,coloricon);
        item->setText(0,QString(sample->sampleName.c_str()));
        item->setData(0,Qt::UserRole,QVariant::fromValue(samples[i]));
        item->setIcon(1,QIcon(QPixmap(rsrcPath + "/edit.png")));
        item->setText(1,QString(sample->getSetName().c_str()));
        item->setText(2,QString::number(sample->getNormalizationConstant(),'f',2));
        if( sample->getInjectionOrder() > 0 )
            item->setText(3,QString::number(sample->getInjectionOrder()));
        else
            item->setText(3,QString("NA"));

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
                _mainwindow->getEicWidget()->replotForced();
                _mainwindow->isotopeWidget->updateSampleList();
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

void ProjectDockWidget::dropEvent(QDropEvent* e) {
    cerr << "ProjectDockWidget::dropEvent() " << endl;
    QTreeWidgetItem *item = _treeWidget->currentItem();
    if (item && item->type() == SampleType ) changeSampleOrder();
}

void ProjectDockWidget::saveProject() {
    
    QSettings* settings = _mainwindow->getSettings();

    QString dir = ".";
    if ( settings->contains("lastDir") ) {
        QString ldir = settings->value("lastDir").value<QString>();
        QDir test(ldir);
        if (test.exists()) dir = ldir;
    }
    QString fileName = QFileDialog::getSaveFileName( this,
            "Save Project As(.mzroll)", dir, "mzRoll Project(*.mzroll)");

    if(!fileName.endsWith(".mzroll",Qt::CaseInsensitive)) fileName = fileName + ".mzroll";

	QList<QPointer<TableDockWidget> > peaksTableList =
		_mainwindow->getPeakTableList();
	peaksTableList.append(0);

	TableDockWidget* peaksTable;

	int j = 1;
	_mainwindow->SaveMzrollListvar.clear();
	Q_FOREACH(peaksTable, peaksTableList) {
        _mainwindow->savePeaksTable(peaksTable, fileName, QString::number(j));
		j++;
	}
}

QTreeWidget* ProjectDockWidget::getTreeWidget(){
    return _treeWidget;
}

void ProjectDockWidget::loadProject() {

    QSettings* settings = _mainwindow->getSettings();
    QString dir = ".";
    if ( settings->contains("lastDir") ) dir = settings->value("lastDir").value<QString>();

    QString fileName = QFileDialog::getOpenFileName( this, "Select Project To Open", dir, "All Files(*.mzroll *.mzRoll)");
    if (fileName.isEmpty()) return;
    loadProject(fileName);
}

void ProjectDockWidget::loadProject(QString fileName) {
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
                        if (id > 0) sample->id = id;
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
				currentSample->saveOriginalRetentionTimes();
				currentSample->applyPolynomialTransform();
			}
        }
        if (xml.isCharacters() && currentXmlElement == "projectDescription") {
            projectDescription.append( xml.text() );
        }
    sendBoostSignal(progressText.toStdString(), samplecount, i);
    }
    data.close();

    lastOpennedProject = fileName;
}

void ProjectDockWidget::saveProject(QString filename, TableDockWidget* peakTable) {


    if (filename.isEmpty() ) return;
    QFile file(filename);
    if (!file.open(QFile::WriteOnly) ) {
        QErrorMessage errDialog(this);
        errDialog.showMessage("File open " + filename + " failed");
        return;
    }

    QXmlStreamWriter stream(&file);
    stream.setAutoFormatting(true);
    stream.writeStartElement("project");
    stream.writeAttribute("mzrollVersion", "1.0");
    stream.writeStartElement("samples");

    vector<mzSample*> samples = _mainwindow->getSamples();
    for(int i=0; i < samples.size(); i++ ) {
        mzSample* sample = samples[i];
        if (sample == NULL ) continue;

        stream.writeStartElement("sample");
        stream.writeAttribute("name",  sample->sampleName.c_str() );
        stream.writeAttribute("id", QString::number(sample->id));
        stream.writeAttribute("filename",  sample->fileName.c_str() );
        stream.writeAttribute("sampleOrder", QString::number(sample->getSampleOrder()));
        stream.writeAttribute("setName", sample->getSetName().c_str());
        stream.writeAttribute("isSelected", QString::number(sample->isSelected==true ? 1 : 0));

        stream.writeStartElement("color");
        stream.writeAttribute("red", QString::number(sample->color[0],'f',2));
        stream.writeAttribute("blue", QString::number(sample->color[1],'f',2));
        stream.writeAttribute("green", QString::number(sample->color[2],'f',2));
        stream.writeAttribute("alpha", QString::number(sample->color[3],'f',2));
        stream.writeEndElement();
        stream.writeEndElement();

    }
    stream.writeEndElement();
    
    stream.writeStartElement("projectDescription");
    stream.writeCharacters(getProjectDescription());
    stream.writeEndElement();

    if( peakTable ){
         peakTable->writePeakTableXML(stream);
    } else {
        _mainwindow->bookmarkedPeaks->writePeakTableXML(stream);
    }

    stream.writeStartElement("database");
    
    string dbname = _mainwindow->ligandWidget->getDatabaseName().toStdString();
    stream.writeAttribute("name",  dbname.c_str());
    for(unsigned int i=0;  i < DB.compoundsDB.size(); i++ ) {
        Compound* compound = DB.compoundsDB[i];
        if(compound->db != dbname ) continue; //skip compounds from other databases

        stream.writeStartElement("compound");
        stream.writeAttribute("id",  compound->id.c_str());
        stream.writeAttribute("name",  compound->name.c_str());
        stream.writeAttribute("mz", QString::number(compound->mass, 'f', 6));
        if(compound->expectedRt > 0) stream.writeAttribute("rt", QString::number(compound->expectedRt, 'f', 6));

        if (compound->charge) stream.writeAttribute("Charge",  QString::number(compound->charge));
        if (compound->formula.length()) stream.writeAttribute("Formula", compound->formula.c_str());
        if (compound->precursorMz) stream.writeAttribute("precursorMz", QString::number(compound->precursorMz, 'f', 6));
        if (compound->productMz) stream.writeAttribute("productMz", QString::number(compound->productMz, 'f', 6));
        if (compound->collisionEnergy) stream.writeAttribute("collisionEnergy", QString::number(compound->collisionEnergy, 'f' ,6));

        if(compound->category.size() > 0) {
            stream.writeStartElement("categories");
            for(unsigned int i=0; i<compound->category.size();i++) {
                stream.writeAttribute("category" + QString::number(i), compound->category[i].c_str());
            }
            stream.writeEndElement();
        }

        stream.writeEndElement();
    }
    stream.writeEndElement();  
    
    stream.writeEndElement();
    QSettings* settings = _mainwindow->getSettings();
    settings->setValue("lastSavedProject", filename);
    lastSavedProject=filename;
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
