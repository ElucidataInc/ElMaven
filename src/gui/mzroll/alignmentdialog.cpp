#include "alignmentdialog.h"
#include "mzAligner.h"

AlignmentDialog::AlignmentDialog(QWidget *parent) : QDialog(parent) { 
	setupUi(this); 
	setModal(false);

	workerThread = NULL;
	workerThread = new BackgroundPeakUpdate(this);

	if (peakDetectionAlgo->currentIndex() == 0) {
		selectDatabase->setVisible(true);
	}

	if (alignAlgo->currentIndex() == 0) {
		label_7->setVisible(true);
		label_8->setVisible(true);
	}
	connect(alignAlgo, SIGNAL(currentIndexChanged(int)), this, SLOT(algoChanged()));
	connect(peakDetectionAlgo, SIGNAL(currentIndexChanged(int)), this, SLOT(algoChanged()));
	connect(cancelButton, SIGNAL(clicked(bool)), SLOT(cancel()));
	connect(alignWrtExpectedRt,SIGNAL(clicked(bool)),SLOT(setAlignWrtExpectedRt(bool)));
	connect(local, SIGNAL(clicked(bool)),this, SLOT(setInitPenalty(bool)));
	connect(restoreDefaultObiWarpParams, SIGNAL(clicked(bool)), this, SLOT(restorDefaultValues(bool)));
	connect(showAdvanceParams, SIGNAL(clicked(bool)), this, SLOT(showAdvanceParameters(bool)));
	connect(this, &AlignmentDialog::changeRefSample, &Aligner::setRefSample);
	connect(samplesBox, &QComboBox::currentTextChanged, this, &AlignmentDialog::refSampleChanged);
	QRect rec = QApplication::desktop()->screenGeometry();
	int height = rec.height();
    setFixedHeight(height-height/10);
}

AlignmentDialog::~AlignmentDialog()
{
	if (workerThread) delete (workerThread);
}

void AlignmentDialog::samplesAligned(bool status)
{
	_mw->samplesAlignedFlag = status;
	UndoAlignment->setEnabled(status);
}

void AlignmentDialog::updateRestoreStatus()
{
	showInfo("Restoring to last saved point..");
}

void AlignmentDialog::refSampleChanged()
{
    mzSample* sample = static_cast<mzSample*>(samplesBox->currentData().value<void*>());
    emit changeRefSample(sample);
}

void AlignmentDialog::setAlignWrtExpectedRt(bool checked)
{
	_mw->mavenParameters->alignWrtExpectedRt = checked;
}

void AlignmentDialog::setInitPenalty(bool checked)
{
	initPenalty->setVisible(checked);
	labelInitPenalty->setVisible(checked);
}

void AlignmentDialog::cancel()
{
	showInfo("Canceling..");
    if (workerThread) {
        if (workerThread->isRunning()) {
            workerThread->completeStop();
            return;
        }
    }
    close();
}

void AlignmentDialog::setMainWindow(MainWindow* mw)
{
    _mw = mw;
}

void AlignmentDialog::show()
{
	_mw->getAnalytics()->hitScreenView("AlignmentDialog");
    inputInitialValuesAlignmentDialog();
	intialSetup();
	QDialog::exec();
}

void AlignmentDialog::inputInitialValuesAlignmentDialog()
{
	minGoodPeakCount->setValue(_mw->mavenParameters->minGoodGroupCount);
	groupingWindow->setValue(_mw->mavenParameters->rtStepSize);

	minGroupIntensity->setValue(_mw->mavenParameters->minGroupIntensity);
	minSN->setValue(_mw->mavenParameters->minSignalBaseLineRatio);
	minPeakWidth->setValue(_mw->mavenParameters->minNoNoiseObs);
	minIntensity->setValue(_mw->mavenParameters->minIntensity);
	maxIntensity->setValue(_mw->mavenParameters->maxIntensity);

}

void AlignmentDialog::setProgressBar(QString text,
									 int progress,
									 int totalSteps)
{
    showInfo(text);
    progressBar->setRange(0, totalSteps);
    progressBar->setValue(progress);
}

void AlignmentDialog::showInfo(QString text)
{
        statusText->setText(text);
}

void AlignmentDialog::intialSetup()
{
	UndoAlignment->setEnabled(_mw->samplesAlignedFlag);
	setProgressBar("Status", 0, 1);
	setDatabase();
	setDatabase(_mw->ligandWidget->getDatabaseName());
	algoChanged();
	minIntensity->setValue(_mw->mavenParameters->minIntensity);
	maxIntensity->setValue(_mw->mavenParameters->maxIntensity);

    samplesBox->clear();
    samplesBox->addItem("Select Randomly",
						QVariant(QVariant::fromValue(static_cast<void*>(nullptr))));
    for(auto sample: _mw->samples) {
        if(sample->isSelected)
            samplesBox->addItem(sample->sampleName.c_str(),
								QVariant(QVariant::fromValue(static_cast<void*>(sample))));
    }
}

void AlignmentDialog::restorDefaultValues(bool checked)
{
	scoreObi->setCurrentText("cor");
	factorGap->setValue(1);
	factorDiag->setValue(2);
	gapExtend->setValue(3.4);
	gapInit->setValue(0.2);
	binSizeObiWarp->setValue(0.6);
	responseObiWarp->setValue(20);
	noStdNormal->setChecked(false);
	local->setChecked(false);
	initPenalty->setValue(0);
	restoreDefaultObiWarpParams->setChecked(false);
	initPenalty->setVisible(false);
	labelInitPenalty->setVisible(false);
}

void AlignmentDialog::showAdvanceParameters(bool checked)
{
	toggleObiParams(checked);
	
	groupBox->setVisible(0);
	groupBox_2->setVisible(0);
}

void AlignmentDialog::toggleObiParams(bool show)
{
	restoreDefaultObiWarpParams->setVisible(show);
	responseObiWarp->setVisible(show);
	binSizeObiWarp->setVisible(show);
	gapInit->setVisible(show);
	gapExtend->setVisible(show);
	factorDiag->setVisible(show);
	factorGap->setVisible(show);
	initPenalty->setVisible(show);
	noStdNormal->setVisible(show);
	local->setVisible(show);
	scoreObi->setVisible(show);

	labelRestoreDefaultObiWarpParams->setVisible(show);
	labelResponseObiWarp->setVisible(show);
	labelBinSizeObiWarp->setVisible(show);
	labelGapInit->setVisible(show);
	labelGapExtend->setVisible(show);
	labelFactorDiag->setVisible(show);
	labelFactorGap->setVisible(show);
	labelInitPenalty->setVisible(show);
	labelNoStdNormal->setVisible(show);
	labelLocal->setVisible(show);
	labelScoreObi->setVisible(show);

	if(show)
		setInitPenalty(local->isChecked());
}

void AlignmentDialog::algoChanged()
{
    bool obiWarp = (alignAlgo->currentIndex() == 1);
	toggleObiParams(obiWarp);
	showAdvanceParameters(showAdvanceParams->isChecked() && obiWarp);
	showAdvanceParams->setVisible(obiWarp);
    labelShowAdvanceParams->setVisible(obiWarp);
    samplesBox->setVisible(obiWarp);
    refSampleLabel->setVisible(obiWarp);

    auto samples = _mw->getSamples();
    auto mrmData = false;
    for (const auto sample : samples) {
        if(sample->ms1ScanCount() == 0 && sample->ms2ScanCount()) {
            showInfo("No MS1 scans found. Obi-warp works only with MS1 scans.\nWe "
                     "will inform you once the support for MS2 scans has been added.");
            mrmData = true;
            alignButton->setDisabled(true);
            break;
        }
    }
    if (!mrmData || !obiWarp) {
        alignButton->setDisabled(false);
        setProgressBar("Status", 0, 1);
    }

	if (peakDetectionAlgo->currentIndex() == 0) {
		selectDatabase->setVisible(true);
		selectDatabaseComboBox->setVisible(true);
		label_10->setVisible(false);
		label_11->setVisible(false);
		minIntensity->setVisible(false);
		maxIntensity->setVisible(false);

	} else {
		selectDatabase->setVisible(false);
		selectDatabaseComboBox->setVisible(false);
		label_10->setVisible(true);
		label_11->setVisible(true);
		minIntensity->setVisible(true);
		maxIntensity->setVisible(true);
	}

	if (alignAlgo->currentIndex() == 0) {
		label_7->setVisible(true);
		maxIterations->setVisible(true);
		label_8->setVisible(true);
		polynomialDegree->setVisible(true);
	} else {
		label_7->setVisible(false);
		maxIterations->setVisible(false);
		label_8->setVisible(false);
		polynomialDegree->setVisible(false);
	}

	groupBox->setVisible(!obiWarp);
	groupBox_2->setVisible(!obiWarp);
}

void AlignmentDialog::setDatabase()
{
	selectDatabaseComboBox->disconnect(SIGNAL(currentIndexChanged(QString)));
	selectDatabaseComboBox->clear();
	QSet<QString>set;
	for(int i=0; i< DB.compoundsDB.size(); i++) {
		if (! set.contains( DB.compoundsDB[i]->db.c_str() ) )
			set.insert( DB.compoundsDB[i]->db.c_str() );
	}

	QIcon icon(rsrcPath + "/dbsearch.png");
	QSetIterator<QString> i(set);
	int pos=0;
	while (i.hasNext()) { 
		selectDatabaseComboBox->addItem(icon,i.next());
	}
}


void AlignmentDialog::setDatabase(QString db)
{
	selectDatabaseComboBox->setCurrentIndex(selectDatabaseComboBox->findText(db));	
}
