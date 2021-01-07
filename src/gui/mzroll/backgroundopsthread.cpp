#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <QDir>
#include <QCoreApplication>
#include <QProcess>
#include <QJsonObject>

#include "Compound.h"
#include "alignmentdialog.h"
#include "common/analytics.h"
#include "backgroundopsthread.h"
#include "database.h"
#include "groupFiltering.h"
#include "grouprtwidget.h"
#include "mainwindow.h"
#include "masscutofftype.h"
#include "mavenparameters.h"
#include "mzAligner.h"
#include "mzSample.h"
#include "obiwarp.h"
#include "peakdetector.h"
#include "samplertwidget.h"
#include "EIC.h"

BackgroundOpsThread::BackgroundOpsThread(QWidget*)
{
    mainwindow = NULL;
    setTerminationEnabled(true);
    runFunction = "";
    mavenParameters = nullptr;
    peakDetector = nullptr;
    setPeakDetector(new PeakDetector());

    _isotopeFormula = "";
    _isotopeCharge = 0;
    _parentGroup = nullptr;
    _performPolyFitAlignment = false;
}

BackgroundOpsThread::~BackgroundOpsThread()
{
    delete peakDetector;
}

/**
 * BackgroundOpsThread::run This function starts the thread. This function is
 * called by start() internally in QTThread. start() function will be called
 * where the thread starts.
 */
void BackgroundOpsThread::run(void)
{
    // Making sure that instance of the mainwindow is present so that the
    // peakdetection process can be run
    if (mainwindow == NULL) {
        quit();
        return;
    }

    connect(this,
            SIGNAL(alignmentError(QString)),
            mainwindow,
            SLOT(showAlignmentErrorDialog(QString)));
    if (mavenParameters->alignSamplesFlag) {
        connect(this,
                SIGNAL(alignmentComplete(QList<PeakGroup>)),
                mainwindow,
                SLOT(showAlignmentWidget()));
    }

    qRegisterMetaType<QList<PeakGroup>>("QList<PeakGroup>");

    connect(this,
            SIGNAL(alignmentComplete(QList<PeakGroup>)),
            mainwindow,
            SLOT(plotAlignmentVizAllGroupGraph(QList<PeakGroup>)));
    connect(this,
            SIGNAL(alignmentComplete(QList<PeakGroup>)),
            mainwindow->groupRtWidget,
            SLOT(setCurrentGroups(QList<PeakGroup>)));
    connect(this,
            SIGNAL(alignmentComplete(QList<PeakGroup>)),
            mainwindow->sampleRtWidget,
            SLOT(plotGraph()));

    mavenParameters->stop = false;

    if (runFunction == "alignWithObiWarp") {
        alignWithObiWarp();
    } else if (runFunction == "findFeatures") {
        findFeatures();
    } else if (runFunction == "computePeaks") {
        computePeaks();
    } else if (runFunction == "pullIsotopesForFormula") {
        pullIsotopesForFormula(_isotopeFormula,
                               _isotopeCharge);
    } else if (runFunction == "pullIsotopesForGroup") {
        pullIsotopesForGroup(_parentGroup.get());
    } else if (runFunction == "pullIsotopesForBarPlot") {
        pullIsotopesForBarPlot(_parentGroup.get());
    } else {
        qWarning() << QString("Unknown function: \"%1\"")
                          .arg(runFunction.c_str());
    }

    quit();
    return;
}

void BackgroundOpsThread::setPeakDetector(PeakDetector *pd)
{
    if (peakDetector != nullptr)
        delete peakDetector;

    peakDetector = pd;
    peakDetector->boostSignal.connect(boost::bind(&BackgroundOpsThread::qtSlot,
                                                  this,
                                                  _1,
                                                  _2,
                                                  _3));
}

void BackgroundOpsThread::qtSlot(const string& progressText,
                                  unsigned int progress,
                                  int totalSteps)
{
    emit updateProgressBar(QString::fromStdString(progressText),
                           progress,
                           totalSteps);
}

void BackgroundOpsThread::qtSignalSlot(const string& progressText,
                                        unsigned int completed_slices,
                                        int total_slices)
{
    emit updateProgressBar(QString::fromStdString(progressText),
                           completed_slices,
                           total_slices);
}

void BackgroundOpsThread::align()
{
    // these if-else statements will take care of all corner cases of
    // "undo alignment"
    if (mavenParameters->alignSamplesFlag && mavenParameters->alignButton > 0) {
        ;
    } else if (mavenParameters->alignSamplesFlag
               && mavenParameters->alignButton == 0) {
        mavenParameters->alignButton++;
        mavenParameters->undoAlignmentGroups = mavenParameters->allgroups;
    } else if (mavenParameters->alignSamplesFlag
               && mavenParameters->alignButton == -1) {
        ;
    } else {
        mavenParameters->alignButton = -1;
        mavenParameters->undoAlignmentGroups = mavenParameters->allgroups;
    }

    if (mavenParameters->alignSamplesFlag && !mavenParameters->stop) {
        emit updateProgressBar("Aligning samples…", 0, 0);
        vector<PeakGroup*> groups(mavenParameters->allgroups.size());
        for (int i = 0; i < mavenParameters->allgroups.size(); i++)
            groups[i] = &mavenParameters->allgroups[i];
        Aligner aligner;
        int alignAlgo = mainwindow->alignmentDialog->alignAlgo->currentIndex();

        if (alignAlgo == 1) {
            mainwindow->getAnalytics()->hitEvent("Alignment", "PolyFit");
            aligner.setMaxIterations(
                mainwindow->alignmentDialog->maxIterations->value());
            aligner.setPolymialDegree(
                mainwindow->alignmentDialog->polynomialDegree->value());
            aligner.doAlignment(groups);
            mainwindow->sampleRtWidget->setDegreeMap(aligner.sampleDegree);
            mainwindow->sampleRtWidget->setCoefficientMap(
                aligner.sampleCoefficient);
        }

        mainwindow->deltaRt = aligner.getDeltaRt();
        mavenParameters->alignSamplesFlag = false;
    }
    QList<PeakGroup> listGroups;
    for (unsigned int i = 0; i < mavenParameters->allgroups.size(); i++) {
        listGroups.append(mavenParameters->allgroups.at(i));
    }

    emit alignmentComplete(listGroups);
    emit samplesAligned(true);
}

void BackgroundOpsThread::alignWithObiWarp()
{
    ObiParams* obiParams = new ObiParams(
        mainwindow->alignmentDialog->scoreObi->currentText().toStdString(),
        false,
        mainwindow->alignmentDialog->factorDiag->value(),
        mainwindow->alignmentDialog->factorGap->value(),
        mainwindow->alignmentDialog->gapInit->value(),
        mainwindow->alignmentDialog->gapExtend->value(),
        0.0f,
        mainwindow->alignmentDialog->responseObiWarp->value(),
        false,
        mainwindow->alignmentDialog->binSizeObiWarp->value());

    emit updateProgressBar("Aligning samples…", 0, 100);

    Aligner aligner;
    aligner.setAlignmentProgress.connect(
        boost::bind(&BackgroundOpsThread::qtSlot, this, _1, _2, _3));

    auto stopped = aligner.alignWithObiWarp(mavenParameters->samples,
                                            obiParams,
                                            mavenParameters);
    delete obiParams;

    if (stopped) {
        emit restoreAlignment();

        // restore previous RTs
        for (auto sample : mavenParameters->samples)
            sample->restorePreviousRetentionTimes();

        // stopped without user intervention
        if (!mavenParameters->stop) {
            emit alignmentError(
                QString("There was an error during alignment. "
                        "Please try again."));
        }

        mavenParameters->stop = false;
        return;
    }

    mainwindow->sampleRtWidget->plotGraph();
    emit samplesAligned(true);
}

void BackgroundOpsThread::emitGroups()
{
    for (PeakGroup& group : mavenParameters->allgroups) {
        if (mavenParameters->keepFoundGroups) {
            emit newPeakGroup(&group);
            QCoreApplication::processEvents();
        }
    }
}

void BackgroundOpsThread::computePeaks()
{
    vector<Compound*> set = mavenParameters->compounds;
    if (set.size() == 0)
        return;

    emit updateProgressBar("Processing Compounds", 0, 0);

    bool hadPullIsotopes = peakDetector->mavenParameters()->pullIsotopesFlag;
    bool hadSearchAdducts = peakDetector->mavenParameters()->searchAdducts;
    if (_performPolyFitAlignment) {
        peakDetector->mavenParameters()->pullIsotopesFlag = false;
        peakDetector->mavenParameters()->searchAdducts = false;
    }
    peakDetector->processCompounds(set);
    if (_performPolyFitAlignment) {
        align();
        peakDetector->mavenParameters()->pullIsotopesFlag = hadPullIsotopes;
        peakDetector->mavenParameters()->searchAdducts = hadSearchAdducts;
    }
    emitGroups();

    emit updateProgressBar("Status", 0, 100);
}

void BackgroundOpsThread::findFeatures()
{
    emit updateProgressBar("Computing Mass Slices", 0, 0);
    mavenParameters->sig.connect(
        boost::bind(&BackgroundOpsThread::qtSignalSlot, this, _1, _2, _3));

    bool hadPullIsotopes = peakDetector->mavenParameters()->pullIsotopesFlag;
    bool hadSearchAdducts = peakDetector->mavenParameters()->searchAdducts;
    if (_performPolyFitAlignment) {
        peakDetector->mavenParameters()->pullIsotopesFlag = false;
        peakDetector->mavenParameters()->searchAdducts = false;
    }
    peakDetector->processFeatures(mavenParameters->compounds);
    if (_performPolyFitAlignment) {
        align();
        peakDetector->mavenParameters()->pullIsotopesFlag = hadPullIsotopes;
        peakDetector->mavenParameters()->searchAdducts = hadSearchAdducts;
    }
    emitGroups();

    emit updateProgressBar("Status", 0, 100);
}

void BackgroundOpsThread::completeStop()
{
    peakDetector->resetProgressBar();
    mavenParameters->stop = true;
}

void BackgroundOpsThread::setRunFunction(QString functionName)
{
    runFunction = functionName.toStdString();
}

void BackgroundOpsThread::pullIsotopesForFormula(string formula, int charge)
{
    if (!mavenParameters->pullIsotopesFlag || formula.empty())
        return;

    Compound tempCompound("tmp_id", "tmp_name", formula, charge);
    peakDetector->processCompounds({&tempCompound}, false);
}

void BackgroundOpsThread::pullIsotopesForGroup(PeakGroup* parentGroup)
{
    peakDetector->detectIsotopesForParent(*parentGroup);
}

void BackgroundOpsThread::pullIsotopesForBarPlot(PeakGroup* parentGroup)
{
    peakDetector->detectIsotopesForParent(*parentGroup, true);
}

void BackgroundOpsThread::updateGroups(QList<shared_ptr<PeakGroup>>& groups,
                                       vector<mzSample*> samples)
{
    auto updateGroup = [samples](PeakGroup* group) {
        MavenParameters* mp = group->parameters().get();
        auto slice = group->getSlice();
        slice.rtmin = samples[0]->minRt;
        slice.rtmax = samples[0]->maxRt;

        auto eics  = PeakDetector::pullEICs(&slice, samples, mp);
        for(auto eic : eics) {
            for(Peak& peak :  group->peaks) {
                if (eic->getSample() == peak.getSample())
                    eic->getPeakDetails(peak);

                if (mp->clsf->hasModel())
                    mp->clsf->scorePeak(peak);
            }
        }
        group->updateQuality();
        group->groupStatistics();
        delete_all(eics);
    };

    for(auto group : groups) {
        updateGroup(group.get());
        for (auto& child : group->childIsotopes())
            updateGroup(child.get());
        for (auto& child : group->childAdducts())
            updateGroup(child.get());
    }
}
