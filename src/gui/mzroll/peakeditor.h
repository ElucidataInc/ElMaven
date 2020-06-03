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
    /**
     * @brief Constructor that takes a parent window widget and a shared peak
     * classifier.
     * @param parent A `MainWindow` reference to which this editor belongs.
     * @param clsf The classifier that will be used to assign peak qualities
     * after they are edited.
     */
    explicit PeakEditor(MainWindow *parent, ClassifierNeuralNet* clsf);

    ~PeakEditor();

    /**
     * @brief Set the `PeakFiltering` object that should be used to filter for
     * candidate peaks.
     * @details This method is needed mostly for setting up to filter parent vs.
     * non-parent isotopes. By default non-parent isotope filters are applied.
     * @param filter A `PeakFiltering` object.
     */
    void setPeakFilter(PeakFiltering filter) { _peakFilter = filter; }

    /**
     * @brief Set the peak-group for whose peaks need to be edited.
     * @details This is the main method that will update the values of almost
     * every visual and data element of peak-editor. Retains state until the
     * next call to this method.
     * @param group Pointer to a `PeakGroup` object whose attributes will be
     * used to set-up the editor. This same reference will also be updated, when
     * the user choses to apply edits.
     */
    void setPeakGroup(PeakGroup* group);

private:
    Ui::PeakEditor* ui;

    /**
     * @brief Feference to the parent main window object.
     */
    MainWindow* _mw;

    /**
     * @brief Reference to a `GalleryWidget` owned by this editor.
     */
    GalleryWidget* _gallery;

    /**
     * @brief Reference to a shared classifier object.
     */
    ClassifierNeuralNet* _clsf;

    /**
     * @brief A filter that will be used to judge chromatographic area as
     * being peak-worthy or not.
     */
    PeakFiltering _peakFilter;

    /**
     * @brief Reference to a peak-group object that is currently displayed and
     * being edited.
     */
    PeakGroup* _group;

    /**
     * @brief A mapping of each sample to the currently set peak boundaries (a
     * pair of left and right boundaries). Updated everytime the user changes
     * it in the gallery widget.
     */
    map<mzSample*, pair<float, float>> _editedPeakRegions;

    /**
     * @brief For the current peak-group, set the min and max RT values - their
     * allowed ranges as well as their visible values. If the peak-group is part
     * of an isotopologue set, then the min/max RT values of all peak-groups in
     * that set are also considered while setting the default visible range.
     */
    void _setRtRangeAndValues();

    /**
     * @brief For the current peak-group, populate the sample list with the
     * samples used for performing peak integration for that group (i.e.,
     * samples unchecked while integration will not appear).
     */
    void _populateSampleList();

    /**
     * @brief Set the RT bounds of the peak (in the current peak-group) for the
     * given sample.
     * @param peakSample Pointer to an `mzSample` object.
     * @param rtMin Minimum retention time (left bound) to set for the peak.
     * @param rtMax Maximum retention time (right bound) to set for the peak.
     */
    void _editPeakRegionForSample(mzSample* peakSample,
                                  float rtMin,
                                  float rtMax);

private slots:
    /**
     * @brief Updates the gallery widget based on the current set of selected
     * items.
     */
    void _selectionChanged();

    /**
     * @brief Apply any edits to peak boundaries made and hide the peak-editor.
     */
    void _applyEdits();
};

#endif // PEAKEDITOR_H
