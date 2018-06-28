#include "mzAligner.h"
#include "mzMassSlicer.h"
#include "mzSample.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <QJsonArray>
#include <QJsonValue>


vector<double> Aligner::groupMeanRt() {
    //find retention time deviation
    vector<double> groupRt(groups.size());
    for (unsigned int i=0; i < groups.size(); i++ ) groupRt[i]=groups[i]->medianRt();
    return(groupRt);
}
