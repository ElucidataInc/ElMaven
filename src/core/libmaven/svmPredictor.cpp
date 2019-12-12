//@Pawan: Classes and methods to compute probability for peak groups

#include "PeakGroup.h"
#include "svmPredictor.h"
#include "groupFeatures.h"
#include "svm.h"

svmPredictor::svmPredictor()
{
    num_features = 8;
}

svmPredictor::~svmPredictor()
{
    svm_free_and_destroy_model(&model);
    free(x);
}

void svmPredictor::loadModel(string filename)
{
    model = svm_load_model(filename.c_str());
    if (!model)
    {
        cerr << "Can't load " << filename << endl;
        return;
    }
}

void svmPredictor::predict(PeakGroup *grp)
{
    vector<float> features = getFeatures(grp);

    max_nr_attr = 9;
    if (model)
    {
        int svm_type = svm_get_svm_type(model);
        int nr_class = svm_get_nr_class(model);
        x = (struct svm_node *)malloc(max_nr_attr * sizeof(struct svm_node));

        for (size_t i=0; i < num_features; i++)
        {

            x[i].index = i+1;
            x[i].value = double(features[i]);
        }
        x[num_features].index = -1;
        //Only predicted labels

        int *labels=(int *) malloc(nr_class*sizeof(int));

		svm_get_labels(model,labels);
        if (int(svm_predict(model,x)) == 1) {
            grp->predictedLabel = PeakGroup::ClassifiedLabel::Signal;
        } else if (int(svm_predict(model,x)) == -1) {
            grp->predictedLabel = PeakGroup::ClassifiedLabel::Noise;
        } else if (int(svm_predict(model,x)) == 0) {
            grp->predictedLabel = PeakGroup::ClassifiedLabel::None;
        }

        //cerr << endl << grp->predictedLabel << endl;
    }
}
