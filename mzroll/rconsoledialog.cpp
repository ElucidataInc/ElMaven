#include "rconsoledialog.h"

#define MAIN   1      // we are the main program, we need to define this
#define SOCK_ERRORS  // we will use verbose socket errors

#include "Rserve/sisocks.h"
#include "Rserve/Rconnection.h"

RConsoleDialog::RConsoleDialog(QWidget *parent) : QDialog(parent) {
    setupUi(this);
    setModal(false);

    initsocks(); // this is needed for Win32 - it does nothing on unix
    connect(runButton,SIGNAL(clicked()),SLOT(runScript()));

    rc=NULL;
    connectToRserve();
}

int RConsoleDialog::connectToRserve() {
    if (!rc) {
        rc = new Rconnection("127.0.0.1",6311);
        int i=rc->connect();

        if (i) {
            char msg[128];
            sockerrorchecks(msg, 128, -1);
            printf("unable to connect (result=%d, socket:%s).\n", i, msg);
            return i;
        }
    }

    return 0;
}

void RConsoleDialog::runScript() {
    QString script = scriptEdit->document()->toPlainText();
    if(script.isEmpty()) return;

    if(!rc) connectToRserve();

    if (rc) {
        qDebug() << "runScript() " << script;
        int status=0;
        try {
            Rexp* result = rc->eval(script.toStdString().c_str(),&status,0);
            if (status==0) {
                statusLabel->clear();
                cout << result;
            } else {
                statusLabel->setText("Script failed!");
            }
        } catch(...) {

        }
    }
}

void RConsoleDialog::clearScript() {
    statusLabel->clear();
    scriptEdit->clear();
}
