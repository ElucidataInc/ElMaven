#ifndef ISOTOPEDIALOG_H
#define ISOTOPEDIALOG_H

#include "stable.h"
#include "ui_isotopedialog.h"

class MainWindow;
class IsotopeDialogSettings;

class IsotopeDialog : public QDialog, public Ui_IsotopeDialog
{
    Q_OBJECT

    public:
        explicit IsotopeDialog(MainWindow* parent = 0);
        ~IsotopeDialog();
        bool getReportIsotopes();
    
    Q_SIGNALS:
        void resetSettings(QList<QString> keys);
        void settingsChanged(string key, string value);
        void settingsUpdated(QString key, QVariant value);
        void updateSettings(IsotopeDialogSettings* id);

    public Q_SLOTS:
        void show();
        void triggerSettingsUpdate();
        void onReset();

    private:
        MainWindow* _mw;
        IsotopeDialogSettings* isotopeSettings;
};

class IsotopeDialogSettings: public QObject
{
    Q_OBJECT
    public:
        IsotopeDialogSettings(IsotopeDialog* dialog);
        QMap<QString, QVariant>& getSettings() { return settings; }

    public Q_SLOTS:
        void updateIsotopeDialogSettings(string key, string value);

    private:
        QMap<QString, QVariant> settings;
        IsotopeDialog* id;
};

#endif // ISOTOPEDIALOG_H
