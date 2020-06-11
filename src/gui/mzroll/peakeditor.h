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
     * @brief Set the peak-group for whose peaks need to be edited.
     * @details This is the main method that will update the values of almost
     * every visual and data element of peak-editor. Retains state until the
     * next call to this method.
     * @param group Shared pointer to a `PeakGroup` whose attributes will be
     * used to set-up the editor. This same reference will also be updated, when
     * the user choses to apply edits.
     */
    void setPeakGroup(shared_ptr<PeakGroup> group);

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
     * @brief Shared reference to a peak-group object that is currently
     * displayed and being edited.
     */
    shared_ptr<PeakGroup> _group;

    /**
     * @brief A mapping of each sample to the currently set peak boundaries (a
     * pair of left and right boundaries). Updated everytime the user changes
     * it in the gallery widget.
     */
    map<mzSample*, pair<float, float>> _setPeakRegions;

    /**
     * @brief For the current peak-group, set values for baseline settings,
     * based on the settings used when integrating that peak-group.
     */
    void _setBaselineParameters();

    /**
     * @brief For the current peak-group, set the min and max RT values - their
     * allowed ranges as well as their visible values. If the peak-group is part
     * of an isotopologue set, then the min/max RT values of all peak-groups in
     * that set are also considered while setting the default visible range.
     */
    void _setRtRangeAndValues();

    /**
     * @brief Depending on whether the current peak-group was set to have all of
     * its isotopic peaks share the same RT range, the sync checkbox will be
     * checked. The checkbox will be disabled if the peak-group has no isotopic
     * children.
     */
    void _setSyncRtCheckbox();

    /**
     * @brief For the current peak-group, populate the sample list with the
     * samples used for performing peak integration for that group (i.e.,
     * samples unchecked while integration will not appear).
     */
    void _populateSampleList();

    /**
     * @brief Set the RT bounds of the peak, of the given peak-group, for the
     * given sample.
     * @param group Pointer to the peak-group whose peak bounds will be edited.
     * @param peakSample Pointer to an `mzSample` object.
     * @param eics A vector of EICs which used for extracting peak region
     * @param rtMin Minimum retention time (left bound) to set for the peak.
     * @param rtMax Maximum retention time (right bound) to set for the peak.
     */
    void _editPeakRegionForSample(PeakGroup* group,
                                  mzSample* peakSample,
                                  vector<EIC*>& eics,
                                  float rtMin,
                                  float rtMax);

    /**
     * @brief Disables most of the UI and sets the status label to inform user
     * that an edit operation is in progress.
     */
    void _setBusyState();

    /**
     * @brief Enables the whole UI and resets status label to an empty string.
     */
    void _setActiveState();

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
