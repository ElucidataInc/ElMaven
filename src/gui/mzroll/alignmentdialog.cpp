#include <QDesktopWidget>

#include "alignmentdialog.h"
#include "common/analytics.h"
#include "background_peaks_update.h"
#include "Compound.h"
#include "globals.h"
#include "ligandwidget.h"
#include "mainwindow.h"
#include "mavenparameters.h"
#include "mzAligner.h"
#include "mzSample.h"
#include "Scan.h"

AlignmentDialog::AlignmentDialog(QWidget *parent) : QDialog(parent) {
        setupUi(this);
        setModal(false);
        setWindowTitle("Alignment");

	workerThread = NULL;
	workerThread = new BackgroundPeakUpdate(this);

        connect(alignAlgo, SIGNAL(currentIndexChanged(int)), this, SLOT(algoChanged()));
	connect(peakDetectionAlgo, SIGNAL(currentIndexChanged(int)), this, SLOT(algoChanged()));
        connect(cancelButton, &QPushButton::clicked, this, &AlignmentDialog::cancel);
        connect(local, SIGNAL(toggled(bool)),this, SLOT(setInitPenalty(bool)));
        connect(restoreDefaultObiWarpParams, SIGNAL(clicked(bool)), this, SLOT(restorDefaultValues(bool)));
        connect(showAdvanceParams, SIGNAL(toggled(bool)), this, SLOT(showAdvanceParameters(bool)));
	connect(this, &AlignmentDialog::changeRefSample, &Aligner::setRefSample);
	connect(samplesBox, &QComboBox::currentTextChanged, this, &AlignmentDialog::refSampleChanged);
}

AlignmentDialog::~AlignmentDialog()
{
	if (workerThread) delete (workerThread);
}

void AlignmentDialog::setWorkerThread(BackgroundPeakUpdate* alignmentWorkerThread)
{
    workerThread = alignmentWorkerThread;
    connect(workerThread,
            &QThread::started,
            this,
            [=] {
                alignAlgo->setEnabled(false);
                alignButton->setEnabled(false);
                UndoAlignment->setEnabled(false);
            });
    connect(workerThread,
            &QThread::finished,
            this,
            [=] {
                alignAlgo->setEnabled(true);
                alignButton->setEnabled(true);
                UndoAlignment->setEnabled(true);
            });
}

void AlignmentDialog::samplesAligned(bool status)
{
	_mw->samplesAlignedFlag = status;
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

void AlignmentDialog::setInitPenalty(bool checked)
{
        initPenalty->setEnabled(checked);
        labelInitPenalty->setEnabled(checked);
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
        initPenalty->setEnabled(false);
        labelInitPenalty->setEnabled(false);
}

void AlignmentDialog::showAdvanceParameters(bool checked)
{
    advancedParamsBox->setVisible(checked);
    if(checked)
        setInitPenalty(local->isChecked());
}

void AlignmentDialog::algoChanged()
{
    bool obiWarp = (alignAlgo->currentIndex() == 0);
    showAdvanceParameters(showAdvanceParams->isChecked());

    auto samples = _mw->getSamples();
    auto mrmData = false;
    for (const auto sample : samples) {
        if(sample->ms1ScanCount() == 0 && sample->ms2ScanCount()) {
            showInfo("No MS1 scans detected for alignment");
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
        stackedWidget->setCurrentIndex(0);
    } else {
        stackedWidget->setCurrentIndex(1);
    }
}

void AlignmentDialog::setDatabase()
{
	selectDatabaseComboBox->disconnect(SIGNAL(currentIndexChanged(QString)));
	selectDatabaseComboBox->clear();
	QSet<QString>set;
	for(int i=0; i< DB.compoundsDB.size(); i++) {
                if (! set.contains( DB.compoundsDB[i]->db().c_str() ) )
                        set.insert( DB.compoundsDB[i]->db().c_str() );
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
