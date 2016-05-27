#ifndef SETTINGS_FORM_H
#define SETTINGS_FORM_H

#include "ui_settingsform.h"
#include "mainwindow.h"

class MainWindow;

class SettingsForm: public QDialog, public Ui_SettingsForm {
Q_OBJECT
public:
	SettingsForm(QSettings* s, MainWindow *w);
protected:
	void closeEvent(QCloseEvent* e) {
		getFormValues();
		QDialog::closeEvent(e);
	}
	void keyPressEvent(QKeyEvent* e) {
		QDialog::keyPressEvent(e);
		getFormValues();
	}
	void mouseReleaseEvent(QMouseEvent* e) {
		QDialog::mouseReleaseEvent(e);
		getFormValues();
	}

public slots:
	void setFormValues();
	void getFormValues();
	void recomputeEIC();
	void recomputeIsotopes();

	void updateSmoothingWindowValue(double value);
	inline void showInstrumentationTab() {
		tabWidget->setCurrentIndex(0);
	}
	inline void showFileImportTab() {
		tabWidget->setCurrentIndex(1);
	}
	inline void showPeakDetectionTab() {
		tabWidget->setCurrentIndex(2);
	}
	inline void setIsotopeDetectionTab() {
		tabWidget->setCurrentIndex(3);
	}

private:
	QSettings *settings;
	MainWindow *mainwindow;
};

#endif
