#ifndef PEAKEDITOR_H
#define PEAKEDITOR_H

#include <QDialog>

class MainWindow;
class PeakGroup;

namespace Ui {
class PeakEditor;
}

class PeakEditor : public QDialog
{
    Q_OBJECT

public:
    explicit PeakEditor(MainWindow *parent = nullptr);
    ~PeakEditor();

    void setPeakGroup(PeakGroup* group);

private:
    Ui::PeakEditor* ui;
    MainWindow* _mw;

    void _populateSampleList(PeakGroup* group);
};

#endif // PEAKEDITOR_H
