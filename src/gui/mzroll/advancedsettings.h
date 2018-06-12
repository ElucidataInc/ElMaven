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
    QVariantMap get_ui_elements();
    bool get_upload_compoundDB();
    void initialSetup();

public slots:
    void showCompoundDBUploadFrame();
public Q_SLOTS:
    void populate_comboBox_compound_db();
private slots:
    
private:
    Ui::AdvancedSettings *ui;
};

#endif // ADVANCEDSETTINGS_H
