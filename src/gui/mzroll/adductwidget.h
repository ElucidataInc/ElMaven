#ifndef ADDUCTWIDGET_H
#define ADDUCTWIDGET_H

#include "ui_adductwidget.h"
#include "datastructures/adduct.h"
#include "stable.h"

class Adduct;
class Database;
class MainWindow;
class AdductsDialogSettings;

Q_DECLARE_METATYPE(Adduct*)

class AdductWidget : public QDialog, public Ui_AdductWidget {
    Q_OBJECT

    public:
        explicit AdductWidget(MainWindow* parent = nullptr);
        void loadAdducts();
        vector<Adduct*> getSelectedAdducts();
        Adduct* defaultAdduct();

    public slots:
        void selectAdductsForCurrentPolarity();
        void triggerSettingsUpdate();
        void onReset();
        void show();

    signals:
        void resetSettings(QList<QString> keys);
        void settingsChanged(string key, string value);
        void settingsUpdated(QString key, QVariant value);
        void updateSettings(AdductsDialogSettings* id);

    protected:
        void showEvent(QShowEvent* event);
        void hideEvent(QHideEvent* event);

    private:
        MainWindow*_mw;
        AdductsDialogSettings* _adductSettings;

    private slots:
        void _updateWindowState();
};

class AdductsDialogSettings: public QObject
{
    Q_OBJECT
    public:
        AdductsDialogSettings(AdductWidget* dialog);
        QMap<QString, QVariant>& getSettings() { return settings; }

    public slots:
        void updateAdductsDialogSettings(string key, string value);

    private:
        QMap<QString, QVariant> settings;
        AdductWidget* id;
};

#endif // ADDUCTWIDGET_H
