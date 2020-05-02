#ifndef PEAKEDITOR_H
#define PEAKEDITOR_H

#include <QDialog>

#include "peakFiltering.h"

class ClassifierNeuralNet;
class GalleryWidget;
class MainWindow;
class mzSample;
class PeakGroup;
class PeakFiltering;

namespace Ui {
class PeakEditor;
}

class PeakEditor : public QDialog
{
    Q_OBJECT

public:
    explicit PeakEditor(MainWindow *parent, ClassifierNeuralNet* clsf);
    ~PeakEditor();

    void setPeakFilter(PeakFiltering filter) { _peakFilter = filter; }
    void setPeakGroup(PeakGroup* group);

private:
    Ui::PeakEditor* ui;
    MainWindow* _mw;
    GalleryWidget* _gallery;
    ClassifierNeuralNet* _clsf;
    PeakFiltering _peakFilter;
    PeakGroup* _group;
    map<mzSample*, pair<float, float>> _editedPeakRegions;

    void _populateSampleList(PeakGroup* group);
    void _editPeakRegionForSample(mzSample* peakSample,
                                  float rtMin,
                                  float rtMax);

private slots:
    void _selectionChanged(QTreeWidgetItem* current,
                           QTreeWidgetItem* previous);
    void _applyEdits();
};

#endif // PEAKEDITOR_H
