#include "jsonReports.h"
#include "mainwindow.h"
#include "tabledockwidget.h"
#include "saveJson.h"

void saveJson::run()
{
	Q_EMIT(_pt->updateProgressBar("Writing to json file. Please wait...", 100, 0));
    jsonReports=new JSONReports(_mw->mavenParameters);
    jsonReports->save(_fn,_pt->vallgroups,_mw->getVisibleSamples());
    Q_EMIT(_pt->updateProgressBar("Writing to json complete.", 100, 100));

}
