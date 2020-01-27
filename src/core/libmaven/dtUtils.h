#ifndef DTUTILS_H
#define DTUTILS_H

#include "masscutofftype.h"
#include "mzSample.h"
#include "mzUtils.h"
#include "mavenparameters.h"
#include "datastructures/mzSlice.h"
#include "databases.h"
#include "classifierNeuralNet.h"
#include "PeakDetector.h"

class SampleLoadingFixture{


    protected:

            vector<mzSample*> samples;

            vector <PeakGroup> allgroups;

            MavenParameters* mavenparameters;

    private:


            Databases database;

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

                samples.push_back(sample1);
                samples.push_back(sample2);
                samples.push_back(sample3);
                samples.push_back(sample4);

            }


            void _loadSamplesAndParameters(vector<mzSample*>& samplesToLoad,
                                         MavenParameters* mavenparameters)
            {

                ClassifierNeuralNet* clsf = new ClassifierNeuralNet();
                string loadmodel = "bin/default.model";
                clsf->loadModel(loadmodel);
                mavenparameters->compoundMassCutoffWindow->setMassCutoffAndType(10, "ppm");
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
            }

            vector<PeakGroup> _getGroupsFromProcessCompounds()
            {
                const char* loadCompoundDB = "bin/methods/KNOWNS.csv";
                database.loadCompoundCSVFile(loadCompoundDB);
                vector<Compound*> compounds =
                    database.getCompoundsSubset("KNOWNS");


                _loadSamplesAndParameters(samples,mavenparameters);

                PeakDetector peakDetector;
                peakDetector.setMavenParameters(mavenparameters);
                vector<mzSlice*> slices =
                    peakDetector.processCompounds(compounds, "compounds");
                peakDetector.processSlices(slices, "compounds");

                return mavenparameters->allgroups;
            }



    public:
            SampleLoadingFixture(){
                mavenparameters= new MavenParameters();

                _makeSampleList();

                allgroups=_getGroupsFromProcessCompounds();

            }

            ~SampleLoadingFixture(){
                delete samples[0];
                delete samples[1];
                delete samples[2];
                delete samples[3];
                delete mavenparameters;
            }

};




#endif // DTUTILS_H
