#include "mzAligner.h"


vector<double> Aligner::groupMeanRt() {
    //find retention time deviation
    vector<double> groupRt(groups.size());
    for (unsigned int i=0; i < groups.size(); i++ ) groupRt[i]=groups[i]->medianRt();
    return(groupRt);
}
