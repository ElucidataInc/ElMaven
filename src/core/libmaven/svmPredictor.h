#ifndef SVM_PREDICTOR_H
#define SVM_PREDICTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "groupFeatures.h"
#include "svm.h"
#include <string>

class svmPredictor
{
  public:
    svmPredictor();
    ~svmPredictor();
    void predict(PeakGroup *grp);
    void loadModel(string filename);

  private:
    const string modelFile = "bin/svm.model";
    struct svm_model* model;
    int max_nr_attr = 64;
    struct svm_node *x;
    int num_features;
};

#endif