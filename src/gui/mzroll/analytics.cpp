#include "analytics.h"

Analytics::Analytics() {
    trackerId = "UA-118159593-1";    
}

void Analytics::setTracker(const char* screenName) {

    GAnalytics tracker(trackerId);
    tracker.sendScreenView(screenName);

}