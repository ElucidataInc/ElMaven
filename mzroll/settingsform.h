#ifndef SETTINGS_FORM_H
#define SETTINGS_FORM_H

#include "ui_settingsform.h"
#include "mainwindow.h"


class MainWindow; 
//   Add class while changing in settings form and peaks form --@Giridhari
class PeakDetectionDialog;
class MavenParameters;


class SettingsForm : public QDialog, public Ui_SettingsForm
{
                Q_OBJECT
		public:
				 SettingsForm(QSettings* s, MainWindow *w);

                protected:
                    void closeEvent       (QCloseEvent* e) { getFormValues(); QDialog::closeEvent(e);}
                    void keyPressEvent    (QKeyEvent* e) { QDialog::keyPressEvent(e); getFormValues(); }
                    void mouseReleaseEvent(QMouseEvent* e) {QDialog::mouseReleaseEvent(e); getFormValues(); }

		public Q_SLOTS:
                 void updateSettingFormGUI();
				 void getFormValues();
				 void recomputeEIC();
				 void recomputeIsotopes();
                 void selectFolder(QString key);
                 void selectFile(QString key);
                 void  setNumericValue(QString key, double value);
                 void  setStringValue(QString key, QString value);
                 // Create methods for Initalize value and show dialog box of setting form --@Giridhari
                void initvalue();
                void showsettings();

                 void updateSmoothingWindowValue(double value);
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




		private:
				QSettings *settings;
				MainWindow *mainwindow;
};

#endif
