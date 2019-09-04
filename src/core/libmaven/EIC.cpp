#include "datastructures/adduct.h"
#include "datastructures/mzSlice.h"
#include "EIC.h"
#include "Peak.h"
#include "PeakGroup.h"
#include "mzPatterns.h"
#include "mzSample.h"
#include "SavGolSmoother.h"
#include "Scan.h"

/**
 * @file EIC.cpp
 * @author Sabu George
 * @author Kiran
 * @author Sahil
 * @version 769
 */
EIC::EIC()
{
    sample = NULL;
    spline = NULL;
    baseline = NULL;
    mzmin = mzmax = rtmin = rtmax = 0;
    maxIntensity = totalIntensity = 0;
    rtAtMaxIntensity = 0.0f;
    mzAtMaxIntensity = 0.0f;
    maxAreaTopIntensity = 0;
    maxAreaIntensity = 0;
    maxAreaTopNotCorrectedIntensity = 0;
    maxAreaNotCorrectedIntensity = 0;
    filterSignalBaselineDiff = 0;
    eic_noNoiseObs = 0;
    smootherType = GAUSSIAN;
    _baselineMode = BaselineMode::Threshold;
    baselineSmoothingWindow = 5;
    //TODO make sure initialization value is same everywhere
    baselineDropTopX = 60;
    _aslsAsymmetry = 80;
    _aslsSmoothness = 2;
    for (unsigned int i = 0; i < 4; i++)
        color[i] = 0;
}

EIC::~EIC()
{
    if (spline != NULL)
        delete[] spline;
    spline = NULL;
    if (baseline != NULL)
        delete[] baseline;
    baseline = NULL;
    peaks.clear();
}

EIC *EIC::eicMerge(const vector<EIC *> &eics)
{
    // Merge to 776
    EIC *meic = new EIC();

    unsigned int maxlen = 0;
    float minRt = DBL_MAX;
    float maxRt = DBL_MIN;
    for (unsigned int i = 0; i < eics.size(); i++)
    {
        if (eics[i]->size() > maxlen)
            maxlen = eics[i]->size();
        if (eics[i]->rtmin < minRt)
            minRt = eics[i]->rtmin;
        if (eics[i]->rtmax > maxRt)
            maxRt = eics[i]->rtmax;
    }

    if (maxlen == 0)
        return meic;

    //create new EIC
    meic->sample = NULL;
    vector<float> intensity(maxlen, 0);
    vector<float> rt(maxlen, 0);
    vector<int> scans(maxlen, 0);
    vector<float> mz(maxlen, 0);
    vector<int> mzcount(maxlen, 0);

    //smoothing   //initalize time array
    for (unsigned int i = 0; i < maxlen; i++)
    {
        rt[i] = minRt + i * ((maxRt - minRt) / maxlen);
        scans[i] = i;
    }

    //combine intensity data from all pulled eics
    for (unsigned int i = 0; i < eics.size(); i++)
    {
        EIC *e = eics[i];
        for (unsigned int j = 0; j < e->size(); j++)
        {
            unsigned int bin = ((e->rt[j] - minRt) / (maxRt - minRt) * maxlen);
            if (bin >= maxlen)
                bin = maxlen - 1;

            if (e->spline and e->spline[j] > 0)
            {
                intensity[bin] += e->spline[j];
            }
            else
            {
                intensity[bin] += e->intensity[j];
            }

            if (e->mz[j] > 0)
            {
                mz[bin] += e->mz[j];
                mzcount[bin]++;
            }
        }
    }

    unsigned int eicCount = eics.size();
    for (unsigned int i = 0; i < maxlen; i++)
    {
        intensity[i] /= eicCount;
        if (intensity[i] > meic->maxIntensity) {
            meic->maxIntensity = intensity[i];
            meic->rtAtMaxIntensity = rt[i];
            meic->mzAtMaxIntensity = mz[i];
        }
        if (mzcount[i])
            mz[i] /= mzcount[i];
        meic->totalIntensity += intensity[i];
    }

    //copy to new EIC
    meic->rtmin = minRt;
    meic->rtmax = maxRt;
    meic->intensity = intensity;
    meic->rt = rt;
    meic->scannum = scans;
    meic->mz = mz;
    meic->sampleName = eics[0]->sampleName;
    meic->sample = eics[0]->sample;
    return meic;
}

bool EIC::_clearBaseline()
{
    if (baseline != nullptr)
    { //delete previous baseline if exists
        delete[] baseline;
        baseline = nullptr;
        eic_noNoiseObs = 0;
    }

    unsigned int n = static_cast<unsigned int>(intensity.size());
    if (!n)
        return false;

    baseline = new float[n];
    std::fill_n(baseline, n, 0.0f);

    return true;
}

void EIC::_computeAsLSBaseline(const float lambda,
                               const float p,
                               const int numIterations)
{
    // Eigen behaves better with double values compared to float
    vector<double> intensity;
    for(unsigned int i = 0; i < this->intensity.size(); ++i)
        intensity.push_back(static_cast<double>(this->intensity[i]));

    auto originalSize = intensity.size();

    // decimate the signal, if it is of very high-resolution
    auto resamplingFactor = mzUtils::approximateResamplingFactor(originalSize);
    intensity = mzUtils::resample(intensity, 1, resamplingFactor);

    using namespace Eigen;
    auto n = static_cast<unsigned int>(intensity.size());

    // Perform difference operation (pseudo-differentiation) on a sparse matrix
    auto diff = [](Eigen::SparseMatrix<double> mat)
    {
        Eigen::SparseMatrix<double> E1 = mat.block(0,
                                                   0,
                                                   mat.rows() - 1,
                                                   mat.cols());
        Eigen::SparseMatrix<double> E2 = mat.block(1,
                                                   0,
                                                   mat.rows() - 1,
                                                   mat.cols());
        return Eigen::SparseMatrix<double>(E2 - E1);
    };

    // create a sparse identity matrix of size `n`
    Eigen::SparseMatrix<double> ident(n, n);
    ident.setIdentity();

    // compute approximate second derivative of the identity matrix
    auto D = diff(diff(ident));

    // create a weights vector of length `n` initially containing only ones
    // for coefficients
    auto w = VectorXd(ArrayXd::Ones(n));

    // instantiate a Cholesky decomposition linear solver for sparse matrices
    SimplicialCholesky<SparseMatrix<double>> solver;

    // instantiate a vector for intensity and one to store baseline
    VectorXd intensityVec = Map<VectorXd>(intensity.data(), n);
    VectorXd baselineVec;

    // helper matrices for `select` operation used when creating binary vectors
    auto ones = VectorXd(MatrixXd::Ones(n, 1));
    auto zeros = VectorXd(MatrixXd::Zero(n, 1));

    // TODO: ideally this should converge to a point where the baseline does
    // not change anymnore, but since we do not have good float comparators
    // yet we can use a decent number of iterations to get as close to the true
    // baseline as possible
    for (int i=0; i < numIterations; ++i) {
        // create a square matrix 'W' with the weights as its diagonal
        auto W = SparseMatrix<double>(w.asDiagonal());

        // compute 'A' and 'b', and then solve for 'x' that satisfies the
        // equation 'A·x = b', where x will be the iteratively estimated
        // baseline
        auto A = W + (lambda * (D.transpose() * D));
        solver.compute(A);
        auto b = VectorXd(w.array() * intensityVec.array());
        baselineVec = solver.solve(b);

        // calculate weights for the next iteration
        auto gtBin = VectorXd(
            ((intensityVec - baselineVec).array() > 0.0).select(ones, zeros)
        );
        auto ltBin = VectorXd(
            ((intensityVec - baselineVec).array() < 0.0).select(ones, zeros)
        );
        w = (p * gtBin) + ((1.0f - p) * ltBin);
    }

    // copy data from an Eigen vector to std::vector
    vector<double> tempVector(baselineVec.data(),
                              baselineVec.data()
                              + baselineVec.rows()
                              * baselineVec.cols());

    // interpolate the signal after possible decimation
    tempVector = mzUtils::resample(tempVector, resamplingFactor, 1);

    // since the interpolated vector may not be of the same size as the original
    // intensity vector, we remove/pad (with zeros) until they are the same size
    while (tempVector.size() < originalSize)
        tempVector.push_back(0.0);
    while (tempVector.size() > originalSize)
        tempVector.pop_back();

    // clip negative values from the vector and switch back to float
    vector<float> clippedFloatBaseline;
    for(unsigned int i = 0; i < tempVector.size(); ++i) {
        auto val = static_cast<float>(tempVector[i]);
        if (val < 0.0f)
            clippedFloatBaseline.push_back(0.0f);
        else
            clippedFloatBaseline.push_back(val);
    }

    // since baseline right now is an array of float, the data from clipped
    // vector is copied to this array
    std::copy(begin(clippedFloatBaseline), end(clippedFloatBaseline), baseline);
}

void EIC::_computeThresholdBaseline(const int smoothingWindow,
                                    const int dropTopX)
{
    auto n = intensity.size();

    //sort intensity vector
    vector<float> tmpv = intensity;
    std::sort(tmpv.begin(), tmpv.end());

    //compute maximum intensity of baseline, any point above this value will
    // be dropped. User specifies quantile of points to keep, for example
    //drop 60% of highest intensities = cut at 40% value;

    float cutvalueF = (100.0 - (float)dropTopX) / 101;
    //cerr << "cutvalue = " << cutvalueF << "\n";
    unsigned int pos = tmpv.size() * cutvalueF;
    //cerr << "pos = " << pos << "\n";
    //cerr << "tmpv size = " << tmpv.size() << "\n";
    float qcut = 0;
    pos < tmpv.size() ? qcut = tmpv[pos] : qcut = tmpv.back();
    //cerr << "qcut = " << qcut << "\n";

    //drop all points above maximum baseline value
    for (int i = 0; i < n; i++)
    {
        if (intensity[i] > qcut)
        {
            baseline[i] = qcut;
        }
        else
        {
            baseline[i] = intensity[i];
        }
    }

    //smooth baseline
    gaussian1d_smoothing(n, smoothingWindow, baseline);

    //count number of observation in EIC above baseline
    for (int i = 0; i < n; i++)
    {
        if (intensity[i] > baseline[i])
            eic_noNoiseObs++;
    }
    //cerr << "eic size = " << n << "\n";
    //cerr << "eic no noise obs = " << eic_noNoiseObs << "\n";
}

void EIC::computeBaseline()
{
    if (!_clearBaseline())
        return;

    switch (_baselineMode) {
    case BaselineMode::Threshold:
        _computeThresholdBaseline(baselineSmoothingWindow, baselineDropTopX);
        break;
    case BaselineMode::AsLSSmoothing:
        _computeAsLSBaseline(pow(10.0f, static_cast<float>(_aslsSmoothness)),
                             static_cast<float>(_aslsAsymmetry) / 100.0f);
        break;
    }
}

void EIC::subtractBaseLine()
{
    // Merged to 776
    if (baseline == NULL)
    {
        cerr << "subtractBaseLine() failed. empty baseline vector\n";
        return;
    }

    eic_noNoiseObs = 0;
    for (unsigned int i = 0; i < intensity.size(); i++)
    {
        intensity[i] -= baseline[i];
        if (intensity[i] < 0)
            intensity[i] = 0;
        if (intensity[i] > 0)
            eic_noNoiseObs++;
    }
}

void EIC::computeSpline(int smoothWindow)
{
    // Merged to 776
    int n = intensity.size();

    if (n == 0)
        return;
    if (this->spline != NULL)
    {
        delete[] spline;
        spline = NULL;
    }

    try
    {
        this->spline = new float[n];
        for (int i = 0; i < n; i++)
            spline[i] = 0;
    }
    catch (...)
    {
        cerr << "Exception caught while allocating memory " << n << "floats " << endl;
    }

    //initalize spline, set to intensity vector
    for (int i = 0; i < n; i++)
        spline[i] = intensity[i];

    if (smoothWindow > n / 3)
        smoothWindow = n / 3; //smoothing window is too large
    if (smoothWindow <= 1)
        return; //nothing to smooth get out

    if (smootherType == SAVGOL)
    { //SAVGOL SMOOTHER
        mzUtils::SavGolSmoother smoother(smoothWindow, smoothWindow, 4);
        vector<float> smoothed = smoother.Smooth(intensity);
        for (int i = 0; i < n; i++)
            spline[i] = smoothed[i];
    }
    else if (smootherType == GAUSSIAN)
    { //GAUSSIAN SMOOTHER
        gaussian1d_smoothing(n, smoothWindow, spline);
    }
    else if (smootherType == AVG)
    {
        float *y = new float[n];
        for (int i = 0; i < n; i++)
            y[i] = intensity[i];
        smoothAverage(y, spline, smoothWindow, n);
        delete[] y;
    }
}

// TODO: This function might not be used
void EIC::clearEICContents()
{
    peaks.clear();
    std::vector<float>().swap(rt);
    std::vector<float>().swap(mz);
    std::vector<float>().swap(intensity);
}

Peak *EIC::addPeak(int peakPos)
{
    peaks.push_back(Peak(this, peakPos));
    return &peaks[peaks.size() - 1];
}

void EIC::getPeakPositions(int smoothWindow)
{
    unsigned int N = intensity.size();
    if (N == 0)
        return;

    computeSpline(smoothWindow);
    if (spline == NULL)
        return;

    findPeaks();

    computeBaseline();
    getPeakStatistics();

    filterPeaks();
}

void EIC::findPeaks()
{
    peaks.clear();

    unsigned int N = intensity.size();

    for (unsigned int i = 1; i < N - 1; i++)
    {
        if (spline[i] > spline[i - 1] && spline[i] > spline[i + 1])
        {
            addPeak(i);
        }
        else if (spline[i] > spline[i - 1] && spline[i] == spline[i + 1])
        {
            float highpoint = spline[i];
            while (i < N - 1)
            {
                i++;
                if (spline[i + 1] == highpoint)
                    continue;
                if (spline[i + 1] > highpoint)
                    break;
                if (spline[i + 1] < highpoint)
                {
                    addPeak(i);
                    break;
                }
            }
        }
    }
}

void EIC::findPeakBounds(Peak &peak)
{
    int apex = peak.pos;

    int ii = apex - 1;
    int jj = apex + 1;
    int lb = ii; // left bound 
    int rb = jj; // right bound

    // this will be used to calculate spline area
    int slb = ii; // spline left bound
    int srb = jj; // spline right bound


    unsigned int N = intensity.size();
    if (N == 0)
        return;
    if (!spline)
        return;
    if (!baseline)
        return;

    int directionality = 0;
    float lastValue = spline[apex];
    while (ii > 0 && ii < (int)N)
    { //walk left
        float relSlope = (spline[ii] - lastValue) / lastValue;
        relSlope > 0.01 ? directionality++ : directionality = 0;
        //if (spline[ii]<=spline[lb] ) lb=ii;
        if (intensity[ii] <= intensity[lb])
            lb = ii;
        if (spline[ii] <= spline[slb])
            slb = ii;
        if (spline[ii] == 0)
            break;
        if (spline[ii] <= baseline[ii])
            break;
        if (spline[ii] <= spline[apex] * 0.01)
            break;

        if (directionality >= 2)
            break;
        lastValue = spline[ii];
        ii = ii - 1;
    }

    directionality = 0;
    lastValue = spline[apex];

    while (jj > 0 && jj < (int)N)
    { //walk right
        float relSlope = (spline[jj] - lastValue) / lastValue;
        relSlope > 0.01 ? directionality++ : directionality = 0;
        //if (spline[jj]<=spline[rb] ) rb=jj;
        if (intensity[jj] <= intensity[rb])
            rb = jj;
        if (spline[jj] <= spline[srb])
            srb = jj;
        if (spline[jj] == 0)
            break;
        if (spline[jj] <= baseline[ii])
            break;
        if (spline[jj] <= spline[apex] * 0.01)
            break;

        if (directionality >= 2)
            break;
        lastValue = spline[jj];
        jj = jj + 1;
    }

    //find maximum point in the span from min to max position
    for (int k = lb; k < rb && k < N; k++)
    {
        if (intensity[k] > intensity[peak.pos] && mz[k] > 0)
            peak.pos = k;
    }

    //remove zero intensity points on the left
    for (unsigned int k = lb; k < peak.pos && k < N; k++)
    {
        if (intensity[k] > 0)
            break;
        lb = k;
    }

    //remove zero intensity points on the right
    for (unsigned int k = rb; k > peak.pos && k < N; k--)
    {
        if (intensity[k] > 0)
            break;
        rb = k;
    }

    //for rare cases where peak is a single observation
    if (lb == apex && lb - 1 >= 0)
        lb = apex - 1;
    if (rb == apex && rb + 1 < N)
        rb = apex + 1;

    peak.minpos = lb;
    peak.maxpos = rb;
    peak.splineminpos = slb;
    peak.splinemaxpos = srb;
}

void EIC::getPeakDetails(Peak &peak)
{
    unsigned int N = intensity.size();
    if (N == 0)
        return;
    if (baseline == NULL)
        return;
    if (peak.pos >= N)
        return;

    //intensity and mz at the apex of the peaks
    peak.peakIntensity = intensity[peak.pos];
    peak.noNoiseObs = 0;
    peak.peakAreaCorrected = 0;
    peak.peakArea = 0;
    peak.peakSplineArea = 0;
    float baselineArea = 0;

    if (sample != NULL && sample->isBlank)
    {
        peak.fromBlankSample = true;
    }

    StatisticsVector<float> allmzs;
    string bitstring;
    if (peak.maxpos >= N)
        peak.maxpos = N - 1;
    if (peak.minpos >= N)
        peak.minpos = peak.pos; //unsigned number weirdness.

    for (unsigned int i = peak.splineminpos; i <= peak.splinemaxpos; i++) {
        if (peak.splineminpos == 0 && peak.splinemaxpos == 0) break;
        peak.peakSplineArea += spline[i];
    }

    float lastValue = intensity[peak.minpos];
    for (unsigned int j = peak.minpos; j <= peak.maxpos; j++)
    {
        peak.peakArea += intensity[j];
        baselineArea += baseline[j];
        if (intensity[j] > baseline[j])
            peak.noNoiseObs++;

        if (peak.peakIntensity < intensity[j])
        {
            peak.peakIntensity = intensity[j];
            peak.pos = j;
        }

        if (mz.size() > 0 && mz[j] > 0)
            allmzs.push_back(mz[j]);

        if (intensity[j] <= baseline[j])
        {
            bitstring += "0";
        }
        else if (intensity[j] > lastValue)
        {
            bitstring += "+";
        }
        else if (intensity[j] < lastValue)
        {
            bitstring += "-";
        }
        else if (intensity[j] == lastValue)
        {
            if (bitstring.length() > 1)
                bitstring += bitstring[bitstring.length() - 1];
            else
                bitstring += "0";
        }

        lastValue = intensity[j];
    }

    getPeakWidth(peak);

    if (rt.size() > 0 && rt.size() == N)
    {
        peak.rt = rt[peak.pos];
        peak.rtmin = rt[peak.minpos];
        peak.rtmax = rt[peak.maxpos];
    }

    if (scannum.size() && scannum.size() == N)
    {
        peak.scan = scannum[peak.pos];       //scan number at the apex of the peak
        peak.minscan = scannum[peak.minpos]; //scan number at left most bound
        peak.maxscan = scannum[peak.maxpos]; //scan number at the right most bound
    }

    int n = 1;
    peak.peakAreaTop = intensity[peak.pos];
    peak.peakAreaTopCorrected = intensity[peak.pos] - baseline[peak.pos];
    if (peak.pos - 1 < N)
    {
        peak.peakAreaTop += intensity[peak.pos - 1];
        peak.peakAreaTopCorrected += intensity[peak.pos - 1] - baseline[peak.pos - 1];
        n++;
    }
    if (peak.pos + 1 < N)
    {
        peak.peakAreaTop += intensity[peak.pos + 1];
        peak.peakAreaTopCorrected += intensity[peak.pos + 1] - baseline[peak.pos + 1];
        n++;
    }
    peak.peakAreaTop /= n;
    peak.peakAreaTopCorrected /= n;
    if (peak.peakAreaTopCorrected < 0) peak.peakAreaTopCorrected = 0;

    peak.peakMz = mz[peak.pos];

    float maxBaseLine = MAX(MAX(baseline[peak.pos], 10), MAX(intensity[peak.minpos], intensity[peak.maxpos]));
    peak.peakBaseLineLevel = baseline[peak.pos];
    peak.noNoiseFraction = (float)peak.noNoiseObs / (this->eic_noNoiseObs + 1);
    peak.peakAreaCorrected = peak.peakArea - baselineArea;
    if (peak.peakAreaCorrected < 0) peak.peakAreaCorrected = 0; 
    peak.peakAreaFractional = peak.peakAreaCorrected / (totalIntensity + 1);
    peak.signalBaselineRatio = peak.peakIntensity / maxBaseLine;
    peak.signalBaselineDifference = peak.peakIntensity - maxBaseLine;

    if (allmzs.size() > 0)
    {
        peak.medianMz = allmzs.median();
        peak.baseMz = allmzs.mean();
        peak.mzmin = allmzs.minimum();
        peak.mzmax = allmzs.maximum();
    }

    if (peak.medianMz == 0)
    {
        peak.medianMz = peak.peakMz;
    }

    mzPattern p(bitstring);
    if (peak.width >= 5)
        peak.symmetry = p.longestSymmetry('+', '-');
    checkGaussianFit(peak);
}

void EIC::getPeakWidth(Peak &peak)
{

    int width = 1;
    int left = 0;
    int right = 0;
    size_t N = intensity.size();

    for (unsigned int i = peak.pos - 1; i > peak.minpos && i < N; i--)
    {
        if (intensity[i] < baseline[i] ||
            mzUtils::almostEqual(baseline[i], intensity[i], 0.0000005f))
            break;
        else
            left++;
    }
    for (unsigned int j = peak.pos + 1; j < peak.maxpos && j < N; j++)
    {
        if (intensity[j] > baseline[j])
            right++;
        else
            break;
    }
    peak.width = width + left + right;
}

void EIC::filterPeaks()
{

    unsigned int i = 0;
    while (i < peaks.size())
    {
        if (filterSignalBaselineDiff > peaks[i].signalBaselineDifference)
        {
            peaks.erase(peaks.begin() + i);
        }
        else
        {
            ++i;
        }
    }
}

vector<mzPoint> EIC::getIntensityVector(Peak &peak)
{
    vector<mzPoint> y;

    if (intensity.size() > 0)
    {
        unsigned int maxi = peak.maxpos;
        unsigned int mini = peak.minpos;
        if (maxi >= intensity.size())
            maxi = intensity.size() - 1;

        for (unsigned int i = mini; i <= maxi; i++)
        {
            //TODO all intensity points are being pushed
            if (baseline and intensity[i] > baseline[i])
            {
                y.push_back(mzPoint(rt[i], intensity[i], mz[i]));
            }
            else
            {
                y.push_back(mzPoint(rt[i], intensity[i], mz[i]));
            }
        }
    }
    return y;
}

void EIC::checkGaussianFit(Peak &peak)
{
    peak.gaussFitSigma = 0;
    peak.gaussFitR2 = 0.03;
    int left = peak.pos - peak.minpos;
    int right = peak.maxpos - peak.pos;
    if (left <= 0 || right <= 0)
        return;
    int moves = min(left, right);
    if (moves < 3)
        return;

    //copy intensities into separate vector
    //dim
    vector<float> pints(moves * 2 + 1);

    int j = peak.pos + moves;
    if (j >= intensity.size())
        j = intensity.size() - 1;
    if (j < 1)
        j = 1;
    int i = peak.pos - moves;
    if (i < 1)
        i = 1;

    int k = 0;
    for (; i <= j; i++)
    {
        pints[k] = intensity[i];
        k++;
    }
    mzUtils::gaussFit(pints, &(peak.gaussFitSigma), &(peak.gaussFitR2));
}

void EIC::getPeakStatistics()
{
    for (unsigned int i = 0; i < peaks.size(); i++)
    {
        findPeakBounds(peaks[i]);
        getPeakDetails(peaks[i]);

        if (peaks[i].peakAreaTopCorrected > maxAreaTopIntensity)
            maxAreaTopIntensity = peaks[i].peakAreaTopCorrected;

        if (peaks[i].peakAreaTop > maxAreaTopNotCorrectedIntensity)
            maxAreaTopNotCorrectedIntensity = peaks[i].peakAreaTop;

        if (peaks[i].peakAreaCorrected > maxAreaIntensity)
            maxAreaIntensity = peaks[i].peakAreaCorrected;

        if (peaks[i].peakArea > maxAreaNotCorrectedIntensity)
            maxAreaNotCorrectedIntensity = peaks[i].peakArea;
    }

    //assign peak ranks based on total area of the peak
    sort(peaks.begin(), peaks.end(), Peak::compArea);
    for (unsigned int i = 0; i < peaks.size(); i++)
        peaks[i].peakRank = i;
}

void EIC::deletePeak(unsigned int i)
{
    if (i < peaks.size())
    {
        peaks.erase(peaks.begin() + i);
    }
}

void EIC::summary()
{
    cerr << "EIC: mz=" << mzmin << "-" << mzmax << " rt=" << rtmin << "-" << rtmax << endl;
    cerr << "   : maxIntensity=" << maxIntensity << endl;
    cerr << "   : peaks=" << peaks.size() << endl;
}

void EIC::removeLowRankGroups(vector<PeakGroup> &groups, unsigned int rankLimit)
{
    // Merged to 776
    if (groups.size() < rankLimit)
        return;
    std::sort(groups.begin(), groups.end(), PeakGroup::compIntensity);
    for (unsigned int i = 0; i < groups.size(); i++)
    {
        if (i > rankLimit)
        {
            groups.erase(groups.begin() + i);
            i--;
        }
    }
}

//TODO: Lots of parameters. Refactor this code - Sahil
vector<PeakGroup> EIC::groupPeaks(vector<EIC *> &eics,
                                  mzSlice* slice,
                                  int smoothingWindow,
                                  float maxRtDiff,
                                  double minQuality,
                                  double distXWeight,
                                  double distYWeight,
                                  double overlapWeight,
                                  bool useOverlap,
                                  double minSignalBaselineDifference,
                                  float productPpmTolerance,
                                  string scoringAlgo)
{
    vector<mzSample*> samples;
    for(int i=0;i<eics.size();++i){
            samples.push_back(eics[i]->sample); //collect all mzSample into vector samples 
    }
    //list filled and return by this function
    vector<PeakGroup> pgroups;

    //case there is only a single EIC, there is nothing to group
    // if (eics.size() == 1 && eics[0] != NULL)
    // {
    //     EIC *m = eics[0];
    //     for (unsigned int i = 0; i < m->peaks.size(); i++)
    //     {
    //         PeakGroup grp;
    //         grp.minQuality = minQuality;
    //         grp.groupId = i;
    //         grp.addPeak(m->peaks[i]);
    //         grp.groupStatistics();
    //         grp.setSelectedSamples(samples);
    //         pgroups.push_back(grp);
    //     }
    //     return pgroups;
    // }

    //create EIC compose from all sample eics
    EIC *m = EIC::eicMerge(eics);
    if (!m)
        return pgroups;

    //find peaks in merged eic
    m->setFilterSignalBaselineDiff(minSignalBaselineDifference);
    m->getPeakPositions(smoothingWindow);
    sort(m->peaks.begin(), m->peaks.end(), Peak::compRt);

    for (unsigned int i = 0; i < m->peaks.size(); i++)
    {
        PeakGroup grp;
        grp.groupId = i;
        if (slice) {
            grp.setSlice(*slice);
            grp.setAdduct(slice->adduct);
        }
        grp.setSelectedSamples(samples);
        pgroups.push_back(grp);
    }

    //cerr << "EIC::groupPeaks() peakgroups=" << pgroups.size() << endl;

    for (unsigned int i = 0; i < eics.size(); i++)
    { //for every sample
        for (unsigned int j = 0; j < eics[i]->peaks.size(); j++)
        { //for every peak in the sample
            Peak &b = eics[i]->peaks[j];
            b.groupNum = -1;
            b.groupOverlap = FLT_MIN;

            vector<Peak>::iterator itr = lower_bound(m->peaks.begin(), m->peaks.end(), b, Peak::compRtMin);
            int lb = (itr - (m->peaks.begin())) - 1;
            if (lb < 0)
                lb = 0;
            //cerr << "\tb=" << b.rtmin << "<=>" << b.rtmax << " lb=" << lb << endl;

            //Find best matching group
            for (unsigned int k = 0; k < m->peaks.size(); k++)
            {
                Peak &a = m->peaks[k];

                float score;

                float overlap = checkOverlap(a.rtmin, a.rtmax, b.rtmin, b.rtmax); //check for overlap
                float distx = abs(b.rt - a.rt);
                float disty = abs(b.peakIntensity - a.peakIntensity);

                if (useOverlap)
                {

                    if (overlap == 0 and a.rtmax < b.rtmin)
                        continue;
                    if (overlap == 0 and a.rtmin > b.rtmax)
                        break;

                    if (distx > maxRtDiff && overlap < 0.2)
                        continue;

                    score = 1.0 / (distXWeight * distx + 0.01) / (distYWeight * disty + 0.01) * (overlapWeight * overlap);
                }
                else
                {

                    if (distx > maxRtDiff)
                        continue;

                    score = 1.0 / (distXWeight * distx + 0.01) / (distYWeight * disty + 0.01);
                }

                if (score > b.groupOverlap)
                {
                    b.groupNum = k;
                    b.groupOverlap = score;
                }
            }

            /*
               cerr << b->peakMz <<  " " << b->rtmin << " " << b->rtmax << "->"  << b->groupNum <<
               " " << b->groupOverlap << endl;
               */

            if (b.groupNum != -1)
            {
                PeakGroup &bestPeakGroup = pgroups[b.groupNum];
                bestPeakGroup.addPeak(b);
            }
            else
            {
                PeakGroup grp;
                pgroups.push_back(grp);
                grp.groupId = pgroups.size() + 1;
                grp.setSlice(*slice);
                grp.addPeak(b);
                b.groupOverlap = 0;
            }
        }
    }

    //clean up peakgroup such that there is only one peak for each sample
    //does the same funtion of vector::erase(), but much faster
    pgroups.erase(std::remove_if(pgroups.begin(), pgroups.end(), [](const PeakGroup &grp) { return grp.peaks.size() <= 0; }), pgroups.end());

    for (unsigned int i = 0; i < pgroups.size(); i++)
    {
        PeakGroup &grp = pgroups[i];
        grp.minQuality = minQuality;
        grp.reduce();
        //grp.fillInPeaks(eics);
        //Feng note: fillInPeaks is unecessary
        grp.groupStatistics();
        grp.computeAvgBlankArea(eics);
        if (grp.getFragmentationEvents().size()) {
            grp.computeFragPattern(productPpmTolerance);
            grp.matchFragmentation(productPpmTolerance, scoringAlgo);
        }
    }

    //now merge overlapping groups
    //EIC::mergeOverlapingGroups(pgroups);
    //cerr << "Found " << pgroups.size() << "groups" << endl;

    if (m)
        delete (m);
    return (pgroups);
}

void EIC::interpolate()
{
    // Merged to 776
    unsigned int lastNonZero = 0;
    for (unsigned int posi = 0; posi < intensity.size(); posi++)
    {

        if (intensity[posi] != 0)
        {
            lastNonZero = posi; //if this position has nonzero intensity, mark it as lastNonZero position
        }

        if (intensity[posi] == 0 and lastNonZero > 0)
        { //interplate
            unsigned int nextNonZero = 0;
            for (unsigned int j = posi; j < intensity.size(); j++)
            {
                if (intensity[j] != 0)
                {
                    nextNonZero = j;
                }
            }
            if (nextNonZero == 0)
                continue;

            //start at first empty position and until next non empty
            for (unsigned int j = posi; j < nextNonZero; j++)
            {
                float fracDist = (j - lastNonZero) / (float)(nextNonZero - lastNonZero);
                float newIntensity = intensity[lastNonZero] + fracDist * intensity[nextNonZero];
                intensity[j] = newIntensity;
                lastNonZero = j;
                posi++;
            }
        }
    }
}

void EIC::getRTMinMaxPerScan()
{
    if (this->rt.size() > 0)
    {
        this->rtmin = this->rt[0];
        this->rtmax = this->rt[this->size() - 1];
    }
}

/**
 * This is the functon which gets the EIC of the given scan for the
 * given mzmin and mzmax. This function will go through the each scan
 * and find the the max intensity and mz corresponding to that max intensity.
 * Total intensity is calculated by adding the maxintensity from each scan.
 * @param[in] scan This is the 
 */
bool EIC::makeEICSlice(mzSample *sample, float mzmin, float mzmax, float rtmin, float rtmax, int mslevel, int eicType, string filterline)
{
    float eicMz = 0, eicIntensity = 0;
    int lb, scanNum;
    vector<float>::iterator mzItr;
    deque<Scan *>::iterator scanItr;
    deque<Scan *> scans;

    scans = sample->scans;
    //binary search rt domain iterator
    Scan tmpScan(sample, 0, 1, rtmin - 0.1, 0, -1);
    scanItr = lower_bound(scans.begin(), scans.end(), &tmpScan, Scan::compRt);
    if (scanItr >= scans.end())
    {
        return false;
    }

    int estimatedScans = scans.size();

    //TODO: why is 10 added?
    if (sample->maxRt - sample->minRt > 0 && (rtmax - rtmin) / (sample->maxRt - sample->minRt) <= 1)
    {

        estimatedScans = float(rtmax - rtmin) / (sample->maxRt - sample->minRt) * scans.size() + 10;
    }

    this->scannum.reserve(estimatedScans);
    this->rt.reserve(estimatedScans);
    this->intensity.reserve(estimatedScans);
    this->mz.reserve(estimatedScans);

    scanNum = scanItr - scans.begin() - 1;

    for (; scanItr != scans.end(); scanItr++)
    {
        Scan *scan = *(scanItr);
        scanNum++;

        if (!(scan->filterLine == filterline || filterline == ""))
            continue;
        if (scan->mslevel != mslevel)
            continue;
        if (scan->rt < rtmin)
            continue;
        if (scan->rt > rtmax)
            break;

        eicMz = 0;
        eicIntensity = 0;

        //binary search
        mzItr = lower_bound(scan->mz.begin(), scan->mz.end(), mzmin);
        lb = mzItr - scan->mz.begin();

        switch ((EIC::EicType)eicType)
        {

        //takes the maximum intensity for given m/z range in a scan
            case EIC::MAX:
        {
            for (unsigned int scanIdx = lb; scanIdx < scan->nobs(); scanIdx++)
            {
                if (scan->mz[scanIdx] < mzmin)
                    continue;
                if (scan->mz[scanIdx] > mzmax)
                    break;

                if (scan->intensity[scanIdx] > eicIntensity)
                {
                    eicIntensity = scan->intensity[scanIdx];
                    eicMz = scan->mz[scanIdx];
                }
            }
            break;
        }

        //takes the sum of all intensities for given m/z range in a scan
        //associated m/z is the weighted average(with intensities as weights)
        case EIC::SUM:
        {
            float n = 0;
            for (unsigned int scanIdx = lb; scanIdx < scan->nobs(); scanIdx++)
            {
                if (scan->mz[scanIdx] < mzmin)
                    continue;
                if (scan->mz[scanIdx] > mzmax)
                    break;

                eicIntensity += scan->intensity[scanIdx];
                eicMz += scan->mz[scanIdx] * scan->intensity[scanIdx];
                n += scan->intensity[scanIdx];
            }
            eicMz /= n;
            break;
        }

        default:
        {
            for (unsigned int scanIdx = lb; scanIdx < scan->nobs(); scanIdx++)
            {
                if (scan->mz[scanIdx] < mzmin)
                    continue;
                if (scan->mz[scanIdx] > mzmax)
                    break;

                if (scan->intensity[scanIdx] > eicIntensity)
                {
                    eicIntensity = scan->intensity[scanIdx];
                    eicMz = scan->mz[scanIdx];
                }
            }
            break;
        }
        }

        this->scannum.push_back(scanNum);
        this->rt.push_back(scan->rt);
        this->intensity.push_back(eicIntensity);
        this->mz.push_back(eicMz);
        this->totalIntensity += eicIntensity;
        if (eicIntensity > this->maxIntensity) {
            this->maxIntensity = eicIntensity;
            this->rtAtMaxIntensity = scan->rt;
            this->mzAtMaxIntensity = eicMz;
        }
    }

    return true;
}

void EIC::normalizeIntensityPerScan(float scale)
{
    if (scale != 1.0)
    {
        for (unsigned int j = 0; j < this->size(); j++)
        {
            this->intensity[j] *= scale;
        }
    }
}
