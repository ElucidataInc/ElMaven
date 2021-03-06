#ifndef DTUTILS_H
#define DTUTILS_H

#include "masscutofftype.h"
#include "mzSample.h"
#include "mzUtils.h"
#include "mavenparameters.h"
#include "datastructures/mzSlice.h"
#include "database.h"
#include "classifierNeuralNet.h"
#include "peakdetector.h"

class SampleLoadingFixture
{
    private:
    vector<mzSample*> _samples;
    vector<PeakGroup> _allgroups;
    MavenParameters* _mavenparameters;
    std::list<PeakGroup> _isotopeGroups;
    Database database;

    void _makeSampleList()
    {
        auto sample1 = new mzSample();
        auto sample2 = new mzSample();
        auto sample3 = new mzSample();
        auto sample4 = new mzSample();
        sample1->loadSample("bin/methods/091215_120i.mzXML");
        sample2->loadSample("bin/methods/091215_120M.mzXML");
        sample3->loadSample("bin/methods/091215_240i.mzXML");
        sample4->loadSample("bin/methods/091215_240M.mzXML");

        _samples.push_back(sample1);
        _samples.push_back(sample2);
        _samples.push_back(sample3);
        _samples.push_back(sample4);
    }

    void _loadSamplesAndParameters(vector<mzSample*>& samplesToLoad,
                                   MavenParameters* mavenparameters)
    {
        ClassifierNeuralNet* clsf = new ClassifierNeuralNet();
        string loadmodel = "bin/default.model";
        clsf->loadModel(loadmodel);
        mavenparameters->compoundMassCutoffWindow->setMassCutoffAndType(10,
                                                                        "ppm");
        mavenparameters->clsf = clsf;
        mavenparameters->ionizationMode = -1;
        mavenparameters->matchRtFlag = true;
        mavenparameters->compoundRTWindow = 1;
        mavenparameters->samples = samplesToLoad;
        mavenparameters->eic_smoothingWindow = 10;
        mavenparameters->eic_smoothingAlgorithm = 1;
        mavenparameters->amuQ1 = 0.25;
        mavenparameters->amuQ3 = 0.30;
        mavenparameters->baseline_smoothingWindow = 5;
        mavenparameters->baseline_dropTopX = 80;
        mavenparameters->pullIsotopesFlag = false;
        mavenparameters->searchAdducts = false;
    }

    vector<PeakGroup> _getTargetedGroupsFromProcessCompounds()
    {
        const char* loadCompoundDB = "bin/methods/KNOWNS.csv";
        database.loadCompoundCSVFile(loadCompoundDB);
        vector<Compound*> compounds = database.getCompoundsSubset("KNOWNS");

        _loadSamplesAndParameters(_samples, _mavenparameters);
        PeakDetector peakDetector;
        peakDetector.setMavenParameters(_mavenparameters);
        peakDetector.processCompounds(compounds);
        return _mavenparameters->allgroups;
    }

    vector<PeakGroup> _getUntargetedGroups()
    {
        _loadSamplesAndParameters(_samples, _mavenparameters);
        PeakDetector peakDetector;
        peakDetector.setMavenParameters(_mavenparameters);
        peakDetector.processFeatures();
        return _mavenparameters->allgroups;
    }

    public:
    SampleLoadingFixture()
    {
        _mavenparameters = new MavenParameters();
        _makeSampleList();
    }

    ~SampleLoadingFixture()
    {
        delete _samples[0];
        delete _samples[1];
        delete _samples[2];
        delete _samples[3];
        delete _mavenparameters;
    }

    void targetedGroup()
    {
        _allgroups = _getTargetedGroupsFromProcessCompounds();
        for (size_t i = 0; i < _allgroups.size(); i++)
            _isotopeGroups.push_back(_allgroups[i]);
    }

    void untargetedGroup()
    {
        _allgroups = _getUntargetedGroups();
        for (size_t i = 0; i < _allgroups.size(); i++)
            _isotopeGroups.push_back(_allgroups[i]);
    }

    /**
     * @brief samples Returns vector of samples loaded.
     * @return
     */
    vector<mzSample*> samples()
    {
        return _samples;
    }

    /**
     * @brief allgroups Returns all peakgroups of the mavenparameters.
     * @return
     */
    vector<PeakGroup> allgroups()
    {
        return _allgroups;
    }

    /**
     * @brief mavenparameters   Returns MavenParameters
     * @return
     */
    MavenParameters* mavenparameters()
    {
        return _mavenparameters;
    }

    /**
     * @brief mavenparameters   Returns isotopesGroups
     * @return
     */
    std::list<PeakGroup> isotopeGroup()
    {
        return _isotopeGroups;
    }
};
#endif // DTUTILS_H
