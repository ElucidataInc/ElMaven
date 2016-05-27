#include "projectdockwidget.h"

ProjectDockWidget::ProjectDockWidget(QMainWindow *parent) :
		QDockWidget("Samples", parent, Qt::Widget) {

	_mainwindow = (MainWindow*) parent;

	setFloating(false);
	setWindowTitle("Samples");
	setObjectName("Samples");

	QFont font;
	font.setFamily("Helvetica");
	font.setPointSize(10);

	// _splitter = new QSplitter(Qt::Vertical,this);

	_editor = new QTextEdit(this);
	_editor->setFont(font);
	_editor->setToolTip("Project Description.");
	_editor->setSizePolicy(QSizePolicy::Preferred,
			QSizePolicy::MinimumExpanding);
	_editor->hide();

	_treeWidget = new QTreeWidget(this);
	_treeWidget->setColumnCount(3);
	_treeWidget->setObjectName("Samples");
	connect(_treeWidget, SIGNAL(itemSelectionChanged()), SLOT(showInfo()));
	_treeWidget->setHeaderHidden(true);
	// _treeWidget->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::MaximumExpanding);

	//_splitter->addWidget(_treeWidget);
	//_splitter->addWidget(_editor);
	//_splitter->setChildrenCollapsible(true);
	//_splitter->setCollapsible(0,false);
	// _splitter->setCollapsible(1,true);
	// QList<int> sizes; sizes << 100 << 0;
	//_splitter->setSizes(sizes);

	setWidget(_treeWidget);
}

QString ProjectDockWidget::getProjectDescription() {
	return _editor->toPlainText();
}

void ProjectDockWidget::setProjectDescription(QString text) {
	return _editor->setPlainText(text);
}

void ProjectDockWidget::changeSampleColor(QTreeWidgetItem* item, int col) {
	if (item == NULL)
		return;
	if (col != 0)
		return;
	QVariant v = item->data(0, Qt::UserRole);
	mzSample* sample = v.value<mzSample*>();
	if (sample == NULL)
		return;

	QColor color = QColor::fromRgbF(sample->color[0], sample->color[1],
			sample->color[2], sample->color[3]);

	color = QColorDialog::getColor(color, this, "Select Sample Color",
			QColorDialog::ShowAlphaChannel);

	if (color.isValid()) {
		sample->color[0] = color.redF();
		sample->color[1] = color.greenF();
		sample->color[2] = color.blueF();
		sample->color[3] = color.alphaF();
		item->setBackgroundColor(0, color);
		_treeWidget->update();
	}
	_mainwindow->getEicWidget()->replot();
}

void ProjectDockWidget::changeSampleSet(QTreeWidgetItem* item, int col) {

	if (item && item->type() == SampleType) {
		QVariant v = item->data(0, Qt::UserRole);
		mzSample* sample = v.value<mzSample*>();

		if (col == 1) {
			QString setName = item->text(1).simplified();
			sample->setSetName(setName.toStdString());
		} else if (col == 0) {
			QString sampleName = item->text(0).simplified();
			if (sampleName.length() > 0) {
				sample->setSampleName(sampleName.toStdString());
			}
		}
		// cerr <<"changeSampleSet: " << sample->sampleName << "  " << sample->getSetName() << endl;
	}
}

void ProjectDockWidget::changeNormalizationConstant(QTreeWidgetItem* item,
		int col) {
	if (item && item->type() == SampleType && col == 2) {
		QVariant v = item->data(0, Qt::UserRole);
		mzSample* sample = v.value<mzSample*>();
		if (sample == NULL)
			return;

		bool ok = false;
		float x = item->text(2).toFloat(&ok);
		if (ok)
			sample->setNormalizationConstant(x);
		cerr << "changeSampleSet: " << sample->sampleName << "  "
				<< sample->getNormalizationConstant() << endl;
	}
}

void ProjectDockWidget::updateSampleList() {

	_mainwindow->setupSampleColors();
	vector<mzSample*> samples = _mainwindow->getSamples();
	std::sort(samples.begin(), samples.end(), mzSample::compSampleOrder);
	if (samples.size() > 0)
		setInfo(samples);

	if (_mainwindow->getEicWidget()) {
		_mainwindow->getEicWidget()->replotForced();
	}

}

void ProjectDockWidget::selectSample(QTreeWidgetItem* item, int col) {
	if (item && item->type() == SampleType) {
		QVariant v = item->data(0, Qt::UserRole);
		mzSample* sample = v.value<mzSample*>();
		if (sample && sample->scans.size() > 0) {

			_mainwindow->spectraWidget->setScan(sample, -1);
			_mainwindow->getEicWidget()->replot();

		}
	}
}

void ProjectDockWidget::showInfo() {
	foreach(QTreeWidgetItem* item, _treeWidget->selectedItems() ){
	QString text = item->text(0);
	QVariant v = item->data(0,Qt::UserRole);
	int itemType = item->type();

	if (itemType == SampleType ) {
		showSample(item,0);
	}
}
}

void ProjectDockWidget::changeSampleOrder() {
	int itemCount = _treeWidget->topLevelItemCount();
	int sampleOrder = 0;
	bool changed = false;

	for (int i = 0; i < itemCount; i++) {
		QTreeWidgetItem *item = _treeWidget->topLevelItem(i);
		if (item && item->type() == SampleType) {
			QVariant v = item->data(0, Qt::UserRole);
			mzSample* sample = v.value<mzSample*>();
			if (sample == NULL)
				continue;
			if (sample->getSampleOrder() != sampleOrder)
				changed = true;
			sample->setSampleOrder(sampleOrder);
			sampleOrder++;
			qDebug() << "changeSampleOrder: " << sampleOrder << " "
					<< sample->sampleName.c_str();
		}
	}

	if (changed) {
		_mainwindow->getEicWidget()->replot();
	}
}

void ProjectDockWidget::setInfo(vector<mzSample*>&samples) {

	if (_treeWidget->topLevelItemCount() == 0) {
		_treeWidget->setAcceptDrops(true);
		_treeWidget->setMouseTracking(true);
		connect(_treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
				SLOT(selectSample(QTreeWidgetItem*, int)));
		connect(_treeWidget, SIGNAL(itemPressed(QTreeWidgetItem*, int)),
				SLOT(changeSampleOrder()));
		connect(_treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
				SLOT(changeSampleColor(QTreeWidgetItem*,int)));
		connect(_treeWidget, SIGNAL(itemEntered(QTreeWidgetItem*, int)),
				SLOT(showSampleInfo(QTreeWidgetItem*,int)));
	}

	disconnect(_treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), 0, 0);

	_treeWidget->clear();
	_treeWidget->setDragDropMode(QAbstractItemView::InternalMove);
	QStringList header;
	header << "Sample" << "Set" << "Scalling";
	_treeWidget->setHeaderLabels(header);
	_treeWidget->header()->setStretchLastSection(true);
	_treeWidget->setHeaderHidden(false);
	_treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	_treeWidget->setRootIsDecorated(true);
	_treeWidget->expandToDepth(10);

	for (int i = 0; i < samples.size(); i++) {
		mzSample* sample = samples[i];
		if (!sample)
			continue;

		QTreeWidgetItem *parent = new QTreeWidgetItem(_treeWidget, SampleType);
		QColor color = QColor::fromRgbF(sample->color[0], sample->color[1],
				sample->color[2], sample->color[3]);

		QString ionization_icon = ":/images/unknown_ion.png";
		if (sample->getPolarity() > 0)
			ionization_icon = ":/images/positive_ion.png";
		else if (sample->getPolarity() < 0)
			ionization_icon = ":/images/negative_ion.png";

		parent->setBackgroundColor(0, color);
		parent->setIcon(0, QIcon(ionization_icon));
		parent->setText(0, QString(sample->sampleName.c_str()));
		parent->setData(0, Qt::UserRole, QVariant::fromValue(samples[i]));
		parent->setText(1, QString(sample->getSetName().c_str()));
		parent->setText(2,
				QString::number(sample->getNormalizationConstant(), 'f', 2));
		parent->setFlags(
				Qt::ItemIsEditable | Qt::ItemIsSelectable
						| Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable
						| Qt::ItemIsEnabled);
		sample->isSelected ?
				parent->setCheckState(0, Qt::Checked) :
				parent->setCheckState(0, Qt::Unchecked);
		parent->setExpanded(true);
	}

	_treeWidget->resizeColumnToContents(0);
	connect(_treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
			SLOT(changeSampleSet(QTreeWidgetItem*,int)));
	connect(_treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
			SLOT(changeNormalizationConstant(QTreeWidgetItem*,int)));
	connect(_treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
			SLOT(showSample(QTreeWidgetItem*,int)));

}

void ProjectDockWidget::showSample(QTreeWidgetItem* item, int col) {
	if (item == NULL)
		return;
	bool checked = (item->checkState(0) != Qt::Unchecked);
	QTreeWidgetItem* parent = item->parent();

	if (item->type() == SampleType) {
		QVariant v = item->data(0, Qt::UserRole);
		mzSample* sample = v.value<mzSample*>();
		if (sample) {
			bool changed = false;
			sample->isSelected != checked ? changed = true : changed = false;
			sample->isSelected = checked;

			if (changed) {
				cerr << "ProjectDockWidget::showSample() changed! " << checked
						<< endl;
				_mainwindow->getEicWidget()->replotForced();
			}
		}
	}
}

void ProjectDockWidget::showSampleInfo(QTreeWidgetItem* item, int col) {

	if (item && item->type() == SampleType) {
		QVariant v = item->data(0, Qt::UserRole);
		mzSample* sample = v.value<mzSample*>();

		QString ionizationMode = "Unknown";
		sample->getPolarity() < 0 ?
				ionizationMode = "Negative" : ionizationMode = "Positive";

		if (sample) {
			this->setToolTip(
					tr(
							"m/z Range: %1-%2<br> rt Range: %3-%4<br> Scan#: %5 <br> MRMs #: %6<br> Ionization: %7<br> Filename: %8").arg(
							sample->minMz).arg(sample->maxMz).arg(sample->minRt).arg(
							sample->maxRt).arg(sample->scanCount()).arg(
							sample->srmScans.size()).arg(ionizationMode).arg(
							sample->fileName.c_str()));
		}
	}

}

void ProjectDockWidget::dropEvent(QDropEvent* e) {
	cerr << "ProjectDockWidget::dropEvent() " << endl;
	QTreeWidgetItem *item = _treeWidget->currentItem();
	if (item && item->type() == SampleType)
		changeSampleOrder();
}

void ProjectDockWidget::saveProject() {

	QSettings* settings = _mainwindow->getSettings();

	QString dir = ".";
	if (settings->contains("lastDir")) {
		QString ldir = settings->value("lastDir").value<QString>();
		QDir test(ldir);
		if (test.exists())
			dir = ldir;
	}

	QString fileName = settings->value("lastOpenedProject").value<QString>();

	if (!fileName.isEmpty()
			&& settings->value("lastSavedProject").value<QString>()
					== fileName) {
		saveProject(fileName);
	} else {
		QString fileName = QFileDialog::getSaveFileName(this,
				"Save Project (.mzroll)", dir, "mzRoll Project(*.mzroll)");

		if (!fileName.endsWith(".mzroll", Qt::CaseInsensitive))
			fileName = fileName + ".mzroll";

		saveProject(fileName);
	}
}

void ProjectDockWidget::loadProject() {

	QSettings* settings = _mainwindow->getSettings();
	QString dir = ".";
	if (settings->contains("lastDir"))
		dir = settings->value("lastDir").value<QString>();

	QString fileName = QFileDialog::getOpenFileName(this,
			"Select Project To Open", dir, "All Files(*.mzroll *.mzRoll)");
	if (fileName.isEmpty())
		return;
	loadProject(fileName);
}

void ProjectDockWidget::loadProject(QString fileName) {

	QSettings* settings = _mainwindow->getSettings();

	QFileInfo fileinfo(fileName);
	QString projectPath = fileinfo.path();

	QFile data(fileName);
	if (!data.open(QFile::ReadOnly)) {
		QErrorMessage errDialog(this);
		errDialog.showMessage("File open: " + fileName + " failed");
		return;
	}

	QXmlStreamReader xml(&data);
	mzSample* currentSample = NULL;

	QStringList pathlist;
	pathlist << projectPath << "."
			<< settings->value("lastDir").value<QString>();

	QString projectDescription;
	QStringRef currentXmlElement;

	while (!xml.atEnd()) {
		xml.readNext();
		if (xml.isStartElement()) {
			currentXmlElement = xml.name();

			if (xml.name() == "sample") {
				QString fname = xml.attributes().value("filename").toString();
				QString sname = xml.attributes().value("name").toString();
				QString setname = xml.attributes().value("setName").toString();
				QString sampleOrder =
						xml.attributes().value("sampleOrder").toString();
				QString isSelected =
						xml.attributes().value("isSelected").toString();
				_mainwindow->setStatusText(tr("Loading sample: %1").arg(sname));

				bool checkLoaded = false;
				foreach(mzSample* loadedFile, _mainwindow->getSamples()){
				if (QString(loadedFile->fileName.c_str())== fname) checkLoaded=true;
			}

				if (checkLoaded == true)
					continue;  // skip files that have been loaded already

				QFileInfo sampleFile(fname);
				if (!sampleFile.exists()) {
					foreach(QString path, pathlist){
					fname= path + QDir::separator() + sampleFile.fileName();
					if (sampleFile.exists()) break;
				}
			}

				if (!fname.isEmpty()) {

					mzFileIO* fileLoader = new mzFileIO(this);
					fileLoader->setMainWindow(_mainwindow);
					mzSample* sample = fileLoader->loadSample(fname);
					delete (fileLoader);

					if (sample) {
						_mainwindow->addSample(sample);
						currentSample = sample;
						if (!sname.isEmpty())
							sample->sampleName = sname.toStdString();
						if (!setname.isEmpty())
							sample->setSetName(setname.toStdString());
						if (!sampleOrder.isEmpty())
							sample->setSampleOrder(sampleOrder.toInt());
						if (!isSelected.isEmpty())
							sample->isSelected = isSelected.toInt();
					} else {
						currentSample = NULL;
					}
				}
			}

			if (xml.name() == "color" && currentSample) {
				currentSample->color[0] =
						xml.attributes().value("red").toString().toDouble();
				currentSample->color[1] =
						xml.attributes().value("blue").toString().toDouble();
				currentSample->color[2] =
						xml.attributes().value("green").toString().toDouble();
				currentSample->color[3] =
						xml.attributes().value("alpha").toString().toDouble();
			}

		}
		if (xml.isCharacters() && currentXmlElement == "projectDescription") {
			projectDescription.append(xml.text());
		}
	}
	data.close();

	setProjectDescription(projectDescription);

	// update other widget
	vector<mzSample*> samples = _mainwindow->getSamples();
	int sampleCount = _mainwindow->sampleCount();
	updateSampleList();
	if (_mainwindow->srmDockWidget->isVisible())
		_mainwindow->showSRMList();
	if (_mainwindow->bookmarkedPeaks)
		_mainwindow->bookmarkedPeaks->loadPeakTable(fileName);
	if (_mainwindow->spectraWidget && sampleCount)
		_mainwindow->spectraWidget->setScan(samples[0]->getScan(0));
	settings->setValue("lastOpenedProject", fileName);
}

void ProjectDockWidget::saveProject(QString filename,
		TableDockWidget* peakTable) {

	if (filename.isEmpty())
		return;
	QFile file(filename);
	if (!file.open(QFile::WriteOnly)) {
		QErrorMessage errDialog(this);
		errDialog.showMessage("File open " + filename + " failed");
		return;
	}

	QXmlStreamWriter stream(&file);
	stream.setAutoFormatting(true);
	stream.writeStartElement("project");

	stream.writeStartElement("samples");

	vector<mzSample*> samples = _mainwindow->getSamples();
	for (int i = 0; i < samples.size(); i++) {
		mzSample* sample = samples[i];
		if (sample == NULL)
			continue;

		stream.writeStartElement("sample");
		stream.writeAttribute("name", sample->sampleName.c_str());
		stream.writeAttribute("filename", sample->fileName.c_str());
		stream.writeAttribute("sampleOrder",
				QString::number(sample->getSampleOrder()));
		stream.writeAttribute("setName", sample->getSetName().c_str());
		stream.writeAttribute("isSelected",
				QString::number(sample->isSelected == true ? 1 : 0));

		stream.writeStartElement("color");
		stream.writeAttribute("red", QString::number(sample->color[0], 'f', 2));
		stream.writeAttribute("blue",
				QString::number(sample->color[1], 'f', 2));
		stream.writeAttribute("green",
				QString::number(sample->color[2], 'f', 2));
		stream.writeAttribute("alpha",
				QString::number(sample->color[3], 'f', 2));
		stream.writeEndElement();
		stream.writeEndElement();

	}
	stream.writeEndElement();

	stream.writeStartElement("projectDescription");
	stream.writeCharacters(getProjectDescription());
	stream.writeEndElement();

	if (peakTable) {
		peakTable->writePeakTableXML(stream);
	} else {
		_mainwindow->bookmarkedPeaks->writePeakTableXML(stream);
	}

	stream.writeEndElement();
	QSettings* settings = _mainwindow->getSettings();
	settings->setValue("lastSavedProject", filename);
}
;

