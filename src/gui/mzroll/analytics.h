#ifndef ANALYTICS_H
#define ANALYTICS_H

#include "ganalytics.h"

class Analytics {

    private:
        const char* trackerId;

    public:
        Analytics();
        void setTracker(const char* screenName);

};

#endif //ANALYTICS_H