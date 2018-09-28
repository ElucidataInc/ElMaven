#ifndef ADVANCEDSETTINGS_H
#define ADVANCEDSETTINGS_H

#include "ui_advancedsettings.h"
#include <QWidget>
#include "database.h"
extern Database DB;

namespace Ui {
class AdvancedSettings;
}

class AdvancedSettings : public QDialog
{
    Q_OBJECT

public:
    AdvancedSettings();
    ~AdvancedSettings();
    void cancel();
    QVariantMap getUIElements();
    bool getUploadCompoundDB();
    bool getUploadPeakTable();
    void initialSetup();

public slots:
    void showCompoundDBUploadFrame();
    void showpeakTableFrame();
public Q_SLOTS:
    void populateCompoundDb();
private slots:
    
private:
    Ui::AdvancedSettings *ui;
};

#endif // ADVANCEDSETTINGS_H
