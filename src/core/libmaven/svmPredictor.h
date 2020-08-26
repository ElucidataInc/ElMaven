#ifndef SVM_PREDICTOR_H
#define SVM_PREDICTOR_H

#include <string>

class PeakGroup;

class svmPredictor
{
  public:
    svmPredictor();
    ~svmPredictor();
    void predict(PeakGroup *grp);
    void loadModel(string filename);

  private:
    const string modelFile = "bin/weights/svm.model";
    struct svm_model* model;
    int max_nr_attr = 64;
    struct svm_node *x;
    int num_features;
};

#endif
