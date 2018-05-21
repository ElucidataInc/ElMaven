#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <QString>

class Analytics {

    private:
        const char* trackerId;

    public:
        Analytics();
        void post();

};

#endif //ANALYTICS_H