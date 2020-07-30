#include <QDesktopWidget>

#include "alignmentdialog.h"
#include "common/analytics.h"
#include "backgroundopsthread.h"
#include "Compound.h"
#include "globals.h"
#include "ligandwidget.h"
#include "mainwindow.h"
#include "mavenparameters.h"
#include "mzAligner.h"
#include "mzfileio.h"
#include "mzSample.h"
#include "Scan.h"

AlignmentDialog::AlignmentDialog(MainWindow* parent) : QDialog(parent) {
        setupUi(this);
        setModal(false);
        setWindowTitle("Alignment");
    setMainWindow(parent);

	workerThread = NULL;
    workerThread = new BackgroundOpsThread(this);

        connect(alignAlgo, SIGNAL(currentIndexChanged(int)), this, SLOT(algoChanged()));
	connect(peakDetectionAlgo, SIGNAL(currentIndexChanged(int)), this, SLOT(algoChanged()));
        connect(cancelButton, &QPushButton::clicked, this, &AlignmentDialog::cancel);
        connect(local, SIGNAL(toggled(bool)),this, SLOT(setInitPenalty(bool)));
        connect(restoreDefaultObiWarpParams, SIGNAL(clicked(bool)), this, SLOT(restorDefaultValues(bool)));
        connect(showAdvanceParams, SIGNAL(toggled(bool)), this, SLOT(showAdvanceParameters(bool)));
	connect(this, &AlignmentDialog::changeRefSample, &Aligner::setRefSample);
	connect(samplesBox, &QComboBox::currentTextChanged, this, &AlignmentDialog::refSampleChanged);

    inputInitialValuesAlignmentDialog();
    intialSetup();
}

AlignmentDialog::~AlignmentDialog()
{
	if (workerThread) delete (workerThread);
}

void AlignmentDialog::setWorkerThread(BackgroundOpsThread* alignmentWorkerThread)
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
                saveValuesForUi();
                QCoreApplication::processEvents();
                _mw->mavenParameters->allgroups.clear();
            });
}

void AlignmentDialog::updateUiFromValues(map<string, variant> settings)
{
    alignAlgo->setCurrentIndex(BINT(settings.at("alignment_algorithm")));

    samplesBox->setCurrentText(QString::fromStdString(BSTRING(settings.at("obi_warp_reference_sample"))));
    showAdvanceParams->setChecked(BINT(settings.at("obi_warp_show_advance_params")));
    scoreObi->setCurrentText(QString::fromStdString(BSTRING(settings.at("obi_warp_score"))));
    responseObiWarp->setValue(BDOUBLE(settings.at("obi_warp_response")));
    binSizeObiWarp->setValue(BDOUBLE(settings.at("obi_warp_bin_size")));
    gapInit->setValue(BDOUBLE(settings.at("obi_warp_gap_init")));
    gapExtend->setValue(BDOUBLE(settings.at("obi_warp_gap_extend")));
    factorDiag->setValue(BDOUBLE(settings.at("obi_warp_factor_diag")));
    factorGap->setValue(BDOUBLE(settings.at("obi_warp_factor_gap")));
    noStdNormal->setChecked(BINT(settings.at("obi_warp_no_standard_normal")));
    local->setChecked(BINT(settings.at("obi_warp_local")));

    maxIterations->setValue(BINT(settings.at("poly_fit_num_iterations")));
    polynomialDegree->setValue(BINT(settings.at("poly_fit_polynomial_degree")));
    minGoodPeakCount->setValue(BINT(settings.at("alignment_good_peak_count")));
    limitGroupCount->setValue(BINT(settings.at("alignment_limit_group_count")));
    groupingWindow->setValue(BINT(settings.at("alignment_peak_grouping_window")));
    minGroupIntensity->setValue(BDOUBLE(settings.at("alignment_min_peak_intensity")));
    minSN->setValue(BINT(settings.at("alignment_min_signal_noise_ratio")));
    minPeakWidth->setValue(BINT(settings.at("alignment_min_peak_width")));
    peakDetectionAlgo->setCurrentIndex(BINT(settings.at("alignment_peak_detection")));
}

void AlignmentDialog::saveValuesForUi()
{
    _mw->fileLoader->insertSettingForSave("alignment_algorithm",
                                          variant(alignAlgo->currentIndex()));

    _mw->fileLoader->insertSettingForSave("obi_warp_reference_sample",
                                          variant(samplesBox->currentText().toStdString()));
    _mw->fileLoader->insertSettingForSave("obi_warp_show_advance_params",
                                          variant(static_cast<int>(showAdvanceParams->isChecked())));
    _mw->fileLoader->insertSettingForSave("obi_warp_score",
                                          variant(scoreObi->currentText().toStdString()));
    _mw->fileLoader->insertSettingForSave("obi_warp_response",
                                          variant(responseObiWarp->value()));
    _mw->fileLoader->insertSettingForSave("obi_warp_bin_size",
                                          variant(binSizeObiWarp->value()));
    _mw->fileLoader->insertSettingForSave("obi_warp_gap_init",
                                          variant(gapInit->value()));
    _mw->fileLoader->insertSettingForSave("obi_warp_gap_extend",
                                          variant(gapExtend->value()));
    _mw->fileLoader->insertSettingForSave("obi_warp_factor_diag",
                                          variant(factorDiag->value()));
    _mw->fileLoader->insertSettingForSave("obi_warp_factor_gap",
                                          variant(factorGap->value()));
    _mw->fileLoader->insertSettingForSave("obi_warp_no_standard_normal",
                                          variant(static_cast<int>(noStdNormal->isChecked())));
    _mw->fileLoader->insertSettingForSave("obi_warp_local",
                                          variant(static_cast<int>(local->isChecked())));

    _mw->fileLoader->insertSettingForSave("poly_fit_num_iterations",
                                          variant(maxIterations->value()));
    _mw->fileLoader->insertSettingForSave("poly_fit_polynomial_degree",
                                          variant(polynomialDegree->value()));
    _mw->fileLoader->insertSettingForSave("alignment_good_peak_count",
                                          variant(minGoodPeakCount->value()));
    _mw->fileLoader->insertSettingForSave("alignment_limit_group_count",
                                          variant(limitGroupCount->value()));
    _mw->fileLoader->insertSettingForSave("alignment_peak_grouping_window",
                                          variant(groupingWindow->value()));
    _mw->fileLoader->insertSettingForSave("alignment_min_peak_intensity",
                                          variant(minGroupIntensity->value()));
    _mw->fileLoader->insertSettingForSave("alignment_min_signal_noise_ratio",
                                          variant(minSN->value()));
    _mw->fileLoader->insertSettingForSave("alignment_min_peak_width",
                                          variant(minPeakWidth->value()));
    _mw->fileLoader->insertSettingForSave("alignment_peak_detection",
                                          variant(peakDetectionAlgo->currentIndex()));
}

void AlignmentDialog::samplesAligned(bool status)
{
	_mw->samplesAlignedFlag = status;
}

void AlignmentDialog::updateRestoreStatus()
{
    showInfo("Restoring to last saved point…");
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
    showInfo("Canceling…");
    if (workerThread && workerThread->isRunning()) {
        workerThread->completeStop();
        return;
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

    auto lastItem = samplesBox->currentText();
    samplesBox->clear();
    samplesBox->addItem("Select Randomly",
                        QVariant(QVariant::fromValue(static_cast<void*>(nullptr))));
    auto selectedSamples = _mw->getVisibleSamples();
    sort(selectedSamples.begin(), selectedSamples.end(), mzSample::compSampleSort);
    for(auto sample: selectedSamples)
    {
        samplesBox->addItem(sample->sampleName.c_str(),
                            QVariant(QVariant::fromValue(static_cast<void*>(sample))));
    }
    if (samplesBox->findText(lastItem) != -1)
        samplesBox->setCurrentText(lastItem);

    saveValuesForUi();
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
    saveValuesForUi();
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
    saveValuesForUi();
}

void AlignmentDialog::setDatabase()
{
	selectDatabaseComboBox->disconnect(SIGNAL(currentIndexChanged(QString)));
	selectDatabaseComboBox->clear();
	QSet<QString>set;
    auto compoundsDB = DB.compoundsDB();
	for(int i=0; i< compoundsDB.size(); i++) {
                if (! set.contains( compoundsDB[i]->db().c_str() ) )
                        set.insert( compoundsDB[i]->db().c_str() );
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
