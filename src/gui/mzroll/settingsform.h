#ifndef SETTINGS_FORM_H
#define SETTINGS_FORM_H

#include "ui_settingsform.h"

class MainWindow;
class OptionsDialogSettings;

class SettingsForm : public QDialog, public Ui_SettingsForm
{
      Q_OBJECT

      public:
            SettingsForm(QSettings* s, MainWindow *w);
      protected:
            void closeEvent       (QCloseEvent* e);
            void keyPressEvent    (QKeyEvent* e) { QDialog::keyPressEvent(e); getFormValues(); }
            void mouseReleaseEvent(QMouseEvent* e) {QDialog::mouseReleaseEvent(e); getFormValues(); }

      public Q_SLOTS:
            void updateSettingFormGUI();
            void getFormValues();
            void setMavenParameters();
            void recomputeEIC();
            void selectFolder(QString key);
            void selectFile(QString key);
            void  setNumericValue(QString key, double value);
            void  setStringValue(QString key, QString value);
            void updateMultiprocessing();
            void setSettingsIonizationMode(QString);
            void setGroupRankStatus();
            void setInitialGroupRank();
            void triggerSettingsUpdate();
            void setIsotopicPeakFiltering();
            void onReset();
            void setFilterline(string filterlineString);

            /*
             * @brief: Sets polarity based on the ionization type selected.
             */
            void setAppropriatePolarity();

            /*
            * [Enable/disable deltaRtWeight slider and display appropriate group rank formula in UI]
            * @method toggleDeltaRtWeight
            */
            void toggleDeltaRtWeight();

            /*
            * [Enable/disable overlap slider and display appropriate peak grouping formula in UI]
            * @method toggleOverlap
            */
            void toggleOverlap();
            void show();

            /*
            * [Update status of the weight slider on UI]
            * @method setWeightStatus 
            */
            void setWeightStatus();
            void updateSmoothingWindowValue(int value);
            inline void showInstrumentationTab() { tabWidget->setCurrentIndex(0); }
            inline void showFileImportTab() { tabWidget->setCurrentIndex(1); }
            inline void showPeakDetectionTab()   { tabWidget->setCurrentIndex(2); }
            inline void setIsotopeDetectionTab()  { tabWidget->setCurrentIndex(3); }
            inline void selectScriptsFolder() {   selectFolder("scriptsFolder"); }                 
            inline void selectMethodsFolder() {   selectFolder("methodsFolder"); }
            inline void selectPathwaysFolder() {   selectFolder("pathwaysFolder"); }
            inline void selectRProgram() {         selectFile("Rprogram"); }
            inline void selectRawExtractor() {      selectFile("RawExtractProgram"); }
            inline void setQ1Tollrance(double value) { setNumericValue("amuQ1",value); }
            inline void setQ3Tollrance(double value) { setNumericValue("amuQ3",value); }



    Q_SIGNALS:
        void settingsChanged(string key, string value);
        void updateSettings(OptionsDialogSettings* od);
        void settingsUpdated(QString key, QVariant value);
        void resetSettings(QList<QString> keys);


      private:
            QSettings *settings;
            MainWindow *mainwindow;
            OptionsDialogSettings* optionSettings;
            QList<QMetaObject::Connection> _analyticsConnections;

      private Q_SLOTS:
            void _smoothnessChanged(int value);
            void _asymmetryChanged(int value);

            /**
             * @brief Connects elements to be tracked via analytics with
             * relevant hit events.
             */
            void _connectAnalytics();

            /**
             * @brief Disconnects all triggers for elements that were being
             * tracked via analytics.
             */
            void _disconnectAnalytics();

      public:
            bool deltaRtCheckFlag;
};

class OptionsDialogSettings: public QObject
{
    Q_OBJECT
    public:
        OptionsDialogSettings(SettingsForm* dialog);
        QMap<QString, QVariant>& getSettings() { return settings; }

    public Q_SLOTS:
        void updateOptionsDialog(string key, string value);


    private:
        QMap<QString, QVariant> settings;
        SettingsForm* sf;


};

#endif
