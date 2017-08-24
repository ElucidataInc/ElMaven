#include "saveJson.h"
void saveJson::run()
{
    jsonReports=new JSONReports(_mw->mavenParameters);
    jsonReports->saveMzEICJson(_fn,_pt->vallgroups,_mw->getVisibleSamples());
}