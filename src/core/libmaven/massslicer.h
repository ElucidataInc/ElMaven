#ifndef MASSSLICES_H
#define MASSSLICES_H

#include "standardincludes.h"

class MassCutoff;
class MavenParameters;
class mzSample;
class mzSlice;

using namespace std;

/**
 * @brief The MassSlicer class provides the functions to create slices (regions
 * of interest) for the various different detection methods.
 */
class MassSlicer {

    public:
        MassSlicer();
        ~MassSlicer();

        void sendSignal(const string& progressText,
                        unsigned int completed_samples,
                        int total_samples);

        vector<mzSlice*> slices;

        /**
         * @brief This function is responsible for creating slices that can be
         * used to perform feature detection.
         * @details At first, every observation in the mz-rt space gets its own
         * slice. These are then reduced (see _reduceSlices), merged (see
         * _mergeSlices and _compareSlices) and adjusted (see _adjustSlices) to
         * finally obtain regions over which peak detection can be performed.
         * @param massCutoff The user defined mass tolerance for m/z domain.
         * @param rtStep Minimum time range for a slice over RT domain.
         */
        void findFeatureSlices(MassCutoff *massCutoff, int rtStep);

        void setSamples(vector<mzSample*> samples)  { this->_samples = samples; }
        void setMaxIntensity( float v) {  _maxIntensity=v; }
        void setMinIntensity( float v) {  _minIntensity=v; }
        void setMinRt       ( float v) {  _minRt = v; }
        void setMaxRt	    ( float v) {  _maxRt = v; }
        void setMaxMz       ( float v) {  _maxMz = v; }
        void setMinMz	    ( float v) {  _minMz = v; }
        void setMinCharge   ( float v) {  _minCharge = v; }
        void setMaxCharge   ( float v) {  _maxCharge = v; }
        void setMavenParameters(MavenParameters* mp) { _mavenParameters = mp;}
        void stopSlicing();

    private:
        unsigned int _maxSlices;
        float _minRt;
        float _maxRt;
        float _minMz;
        float _maxMz;
        float _maxIntensity;
        float _minIntensity;
        int _minCharge;
        int _maxCharge;
        MassCutoff *_massCutoff;
        vector<mzSample*> _samples;
        MavenParameters* _mavenParameters;

        /**
         * @brief Merge neighbouring slices that are related to each other,
         * i.e., the highest intensities of these slices fall in a small window.
         * @details This method uses `_compareSlices` function to decide whether
         * two slices should be merged. Iteration happens for each slice in the
         * `slices` vector and for each sample. For each slice, the neighbouring
         * slices (positive and negative look-ahead) are checked until the
         * second value returned from a comparison call is found to be `false`,
         * signalling that further neighbours are not qualified for merging, by
         * definition.
         * @param massCutoff A `MassCutoff` object that decides the maximum
         * width of a slice in the m/z domain. Any merged slice that expands to
         * a size more than what this cutoff dictates, will be resized around
         * its mean m/z value.
         * @param rtTolerance A time value, that will be used to judge the
         * "closeness" of two points in two different slices.
         * @param updateMessage A string message that will be emitted along with
         * progress updates on the completion of the merge operation.
         */
        void _mergeSlices(const MassCutoff* massCutoff,
                          const float rtTolerance);

        /**
         * @brief A function that takes in a vector of `mzSample` objects, and
         * two pointers to the mzSlices that need to be compared.
         * @param samples A vector of `mzSample` objects, each of which will be
         * used to obtain the EIC for slices, while deciding their mergeability.
         * @param The first slice for comparison.
         * @param The second slice for comparison.
         * @param massCutoff A `MassCutoff` object that can be used to compare
         * and quantify the distance between two m/z values. This value will,
         * therefore, be used to tell whether two slices or their highest
         * intensities are too far in the m/z domain.
         * @param rtTolerance The tolerance for retention time axis. If the
         * highest intensity of two slices differ by more than this value on
         * rt axis, then they will not be merged.
         * @return The function returns a pair of boolean values, the first of
         * which can be used to determine whether the slices should be merged or
         * not. The second boolean can be used to decide whether iteration needs
         * to proceed in the current direction (proceed if `true`, stop if
         * `false`).
         */
        pair<bool, bool> _compareSlices(vector<mzSample*>& samples,
                                        mzSlice* slice,
                                        mzSlice* comparisonSlice,
                                        const MassCutoff *massCutoff,
                                        const float rtTolerance);

        /**
         * @brief This method will reduce the internal slice vector by merging
         * and resizing them if they share a signifant region of interest.
         */
        void _reduceSlices();

        /**
         * @brief Adjust all slices in m/z domain such that they are centered
         * around its current highest intensity.
         */
        void _adjustSlices();
};
#endif
