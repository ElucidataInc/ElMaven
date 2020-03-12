#include "deconvolution.h"
#include "EIC.h"
#include "Eigen"
#include "mzUtils.h"
#include "Peak.h"

using namespace Eigen;

/**
 * @brief Local helper method that ensures that model peak regions are of at
 * least average width.
 * @details Consequently, some regions might end up sharing some left or right
 * boundaries (or maybe even overlap), with other regions. This makes them a
 * candidate for deconvolution.
 */
vector<pair<size_t, size_t>>
_refineModelRegions(EIC *eic,
                    vector<pair<size_t, size_t>> regions,
                    int averagePeakWidth)
{
    vector<pair<size_t, size_t>> refinedRegions;
    for (auto& region : regions) {
        auto signal = eic->intensitySegment(region.first, region.second, true);
        size_t peakTop = distance(begin(signal),
                                  max_element(begin(signal), end(signal)));
        if (peakTop < 0 || peakTop >= signal.size())
            continue;

        size_t peakLeft = 0;
        for (size_t i = peakTop; i > 0; --i) {
            if (peakTop - i < (averagePeakWidth / 2))
                continue;
            if (signal[i - 1] >= signal[i]) {
                peakLeft = i;
                break;
            }
        }
        size_t peakRight = signal.size() - 1;
        for (size_t i = peakTop; i < signal.size() - 1; ++i) {
            if (i - peakTop < (averagePeakWidth / 2))
                continue;
            if (signal[i] <= signal[i + 1]) {
                peakRight = i;
                break;
            }
        }

        // translate peak boundaries to positions on parent EIC
        peakLeft += region.first;
        peakRight += region.first;

        // if this segment already exists (probably the last one), we ignore it
        auto finalSegment = make_pair(peakLeft, peakRight);
        if (!refinedRegions.empty() && finalSegment == refinedRegions.back())
            continue;

        signal = eic->intensitySegment(peakLeft, peakRight, true);
        if (signal.empty())
            continue;

        // peak regions must not be too noisy
        auto idealSlope = mzUtils::idealSlopeValue(signal);
        // TODO: increase threshold when, derivative method is replaced.
        if (isnan(idealSlope) || idealSlope < 0.90)
            continue;

        // the final segment should now be as wide as the average peak
        refinedRegions.push_back(finalSegment);
    }
    return refinedRegions;
}

vector<pair<size_t, size_t>>
Deconvolution::modelPeakRegions(EIC* eic,
                                int smoothingWindow,
                                float sigma,
                                int averagePeakWidth)
{
    if (eic->size() == 0)
        return {};

    // "currently this value should be enough for LC", why?
    float halfPoint = 10.0f;
    size_t filterSize = 2 * static_cast<size_t>(halfPoint) + 1;
    vector<float> filter;
    for (int i = 0; i < filterSize; i++) {
        float coeff = (1 - powf((-halfPoint + i) / sigma, 2.0f))
                       * exp(-0.5 * powf((-halfPoint + i) / sigma, 2.0f));
        filter.push_back(coeff);
    }

    // this will destructively modify the peaks within the EIC,
    // but then we intend to find better peaks, so it should be fine
    eic->getPeakPositions(smoothingWindow);
    if (eic->peaks.empty())
        return {};

    vector<pair<size_t, size_t>> peakRegions;
    sort(begin(eic->peaks), end(eic->peaks), Peak::compRt);
    for (const auto& peak : eic->peaks) {
        if (peak.width < 3)
            continue;

        peakRegions.push_back(make_pair(peak.minpos, peak.maxpos));
    }

    vector<pair<size_t, size_t>> modelRegions;
    for (size_t i = 0; i < peakRegions.size(); i++) {
        float sum = 0.0f;
        for (int j = -1 * halfPoint; j <= halfPoint; j++) {
            if (i + j < 0 || i + j > peakRegions.size() - 1) {
                continue;
            } else {
                auto signal = eic->intensitySegment(peakRegions.at(i + j).first,
                                                    peakRegions.at(i + j).second,
                                                    true);
                auto sharpness = mzUtils::sharpnessValue(signal);
                sum += sharpness * filter[static_cast<size_t>(j + halfPoint)];
            }
        }

        // these zero sum regions do not contain any "model" peaks
        if (sum == 0.0f)
            continue;

        modelRegions.push_back(peakRegions.at(i));
    }

    return _refineModelRegions(eic, modelRegions, averagePeakWidth);
}

Deconvolution::Pattern _getPattern(bool hasOneLeft,
                                   bool hasTwoLeft,
                                   bool hasOneRight,
                                   bool hasTwoRight)
{
    if (!hasOneLeft && !hasTwoLeft && !hasOneRight && !hasTwoRight)
        return Deconvolution::Pattern::C;
    if (hasOneLeft && !hasTwoLeft && !hasOneRight && !hasTwoRight)
        return Deconvolution::Pattern::BC;
    if (!hasOneLeft && !hasTwoLeft && hasOneRight && !hasTwoRight)
        return Deconvolution::Pattern::CD;
    if (hasOneLeft && !hasTwoLeft && hasOneRight && !hasTwoRight)
        return Deconvolution::Pattern::BCD;
    if (hasOneLeft && hasTwoLeft && !hasOneRight && !hasTwoRight)
        return Deconvolution::Pattern::ABC;
    if (!hasOneLeft && !hasTwoLeft && hasOneRight && hasTwoRight)
        return Deconvolution::Pattern::CDE;
    if (hasOneLeft && hasTwoLeft && hasOneRight && !hasTwoRight)
        return Deconvolution::Pattern::ABCD;
    if (hasOneLeft && !hasTwoLeft && hasOneRight && hasTwoRight)
        return Deconvolution::Pattern::BCDE;
    if (hasOneLeft && hasTwoLeft && hasOneRight && hasTwoRight)
        return Deconvolution::Pattern::ABCDE;
    return Deconvolution::Pattern::C;
}

Deconvolution::SignalVector
Deconvolution::convolutedSignals(const pair<size_t, size_t> &roi,
                                 const vector<pair<size_t, size_t>>& regions,
                                 const EIC* eic)
{
    SignalVector convoluted;
    bool hasOneLeft = false;
    bool hasOneRight = false;
    bool hasTwoLeft = false;
    bool hasTwoRight = false;

    auto foundAt = find(begin(regions), end(regions), roi);
    if (foundAt == end(regions))
        return convoluted;

    int positionOfRoi = foundAt - begin(regions);

    // the next few checks, all assume that segments were sent in increasing
    // order of retention time, while checking whether the ROI has peaks to the
    // left or right and, if so, whether their boundaries overlap with ROI
    if (positionOfRoi > 1
        && regions.at(positionOfRoi - 2).second >= roi.first) {
        hasTwoLeft = true;
    }
    if (positionOfRoi > 0
        && regions.at(positionOfRoi - 1).second >= roi.first) {
        hasOneLeft = true;
    }
    if (positionOfRoi < static_cast<int>(regions.size()) - 2
        && regions.at(positionOfRoi + 2).first <= roi.second) {
        hasTwoRight = true;
    }
    if (positionOfRoi < static_cast<int>(regions.size()) - 1
        && regions.at(positionOfRoi + 1).first <= roi.second) {
        hasOneRight = true;
    }

    float leftBound = roi.first;
    float rightBound = roi.second;
    if (hasTwoLeft) {
        leftBound = min(roi.first,
                        min(regions.at(positionOfRoi - 2).first,
                            regions.at(positionOfRoi - 1).first));
    } else if (hasOneLeft) {
        leftBound = min(roi.first,
                        regions.at(positionOfRoi - 1).first);
    }
    if (hasTwoRight) {
        rightBound = max(roi.second,
                         max(regions.at(positionOfRoi + 2).second,
                             regions.at(positionOfRoi + 1).second));
    } else if (hasOneRight) {
        rightBound = max(roi.second,
                         regions.at(positionOfRoi + 1).second);
    }

    // fill each signal based on its region's boundaries, padding zeros where
    // the index is outside the bounds such that all five are of the same length
    for (size_t i = leftBound; i <= rightBound; ++i) {
        double corrected = fmaxf(eic->intensity[i] - eic->baseline[i], 0.0);

        if (roi.first > i || roi.second < i) {
            convoluted.target.push_back(0.0);
        } else {
            convoluted.target.push_back(corrected);
        }

        if (hasTwoLeft) {
            if (regions.at(positionOfRoi - 2).first > i
                || regions.at(positionOfRoi - 2).second < i) {
                convoluted.twoLeft.push_back(0.0);
            } else {
                convoluted.twoLeft.push_back(corrected);
            }
        }

        if (hasOneLeft || hasTwoLeft) {
            if (regions.at(positionOfRoi - 1).first > i
                || regions.at(positionOfRoi - 1).second > i) {
                convoluted.oneLeft.push_back(0.0);
            } else {
                convoluted.oneLeft.push_back(corrected);
            }
        }

        if (hasTwoRight) {
            if (regions.at(positionOfRoi + 2).first > i
                || regions.at(positionOfRoi + 2).second < i) {
                convoluted.twoRight.push_back(0.0);
            } else {
                convoluted.twoRight.push_back(corrected);
            }
        }

        if (hasOneRight || hasTwoRight) {
            if (regions.at(positionOfRoi + 1).first > i
                || regions.at(positionOfRoi + 1).second > i) {
                convoluted.oneRight.push_back(0.0);
            } else {
                convoluted.oneRight.push_back(corrected);
            }
        }
    }
    convoluted.leftBound = leftBound;
    convoluted.rightBound = rightBound;
    convoluted.pattern = _getPattern(hasOneLeft,
                                     hasTwoLeft,
                                     hasOneRight,
                                     hasTwoRight);
    return convoluted;
}

vector<float> _deconvolutePatternDouble(Deconvolution::SignalVector convoluted,
                                        vector<double> trueChromatogram)
{
    VectorXd aVector;
    if (convoluted.pattern == Deconvolution::Pattern::BC) {
        aVector = Map<VectorXd>(convoluted.oneLeft.data(),
                                convoluted.oneLeft.size());
    } else if (convoluted.pattern == Deconvolution::Pattern::CD) {
        aVector = Map<VectorXd>(convoluted.oneRight.data(),
                                convoluted.oneRight.size());
    }
    VectorXd targetVector = Map<VectorXd>(convoluted.target.data(),
                                          convoluted.target.size());
    VectorXd trueVector = Map<VectorXd>(trueChromatogram.data(),
                                        trueChromatogram.size());

    double t_t = targetVector.squaredNorm();
    double a_a = aVector.squaredNorm();

    double t_a = targetVector.dot(aVector);

    typedef Matrix<double, 2, 2> Matrix2x2;
    Matrix2x2 matrix;
    matrix << t_t,  t_a,
              t_a,  a_a;
    Eigen::FullPivLU<Matrix2x2> lu(matrix);
    if (!lu.isInvertible())
        return vector<float>(begin(convoluted.target), end(convoluted.target));

    double z_t = targetVector.dot(trueVector);
    double z_a = aVector.dot(trueVector);

    VectorXd z(2);
    z << z_t,
         z_a;
    double coefficient = lu.solve(z)(0);
    if (coefficient <= 0.0)
        return vector<float>(begin(convoluted.target), end(convoluted.target));

    VectorXd deconvolutedTarget = coefficient * targetVector;
    return vector<float>(deconvolutedTarget.data(),
                         deconvolutedTarget.data()
                         + deconvolutedTarget.rows()
                         * deconvolutedTarget.cols());
}

vector<float> _deconvolutePatternTriple(Deconvolution::SignalVector convoluted,
                                        vector<double> trueChromatogram)
{
    VectorXd aVector;
    VectorXd bVector;
    if (convoluted.pattern == Deconvolution::Pattern::ABC) {
        aVector = Map<VectorXd>(convoluted.twoLeft.data(),
                                convoluted.twoLeft.size());
        bVector = Map<VectorXd>(convoluted.oneLeft.data(),
                                convoluted.oneLeft.size());
    } else if (convoluted.pattern == Deconvolution::Pattern::BCD) {
        aVector = Map<VectorXd>(convoluted.oneLeft.data(),
                                convoluted.oneLeft.size());
        bVector = Map<VectorXd>(convoluted.oneRight.data(),
                                convoluted.oneRight.size());
    } else if (convoluted.pattern == Deconvolution::Pattern::CDE) {
        aVector = Map<VectorXd>(convoluted.oneRight.data(),
                                convoluted.oneRight.size());
        bVector = Map<VectorXd>(convoluted.twoRight.data(),
                                convoluted.twoRight.size());
    }
    VectorXd targetVector = Map<VectorXd>(convoluted.target.data(),
                                          convoluted.target.size());
    VectorXd trueVector = Map<VectorXd>(trueChromatogram.data(),
                                        trueChromatogram.size());

    double t_t = targetVector.squaredNorm();
    double a_a = aVector.squaredNorm();
    double b_b = bVector.squaredNorm();

    double t_a = targetVector.dot(aVector);
    double t_b = targetVector.dot(bVector);

    double a_b = aVector.dot(bVector);

    typedef Matrix<double, 3, 3> Matrix3x3;
    Matrix3x3 matrix;
    matrix << t_t,  t_a,  t_b,
              t_a,  a_a,  a_b,
              t_b,  a_b,  b_b;
    Eigen::FullPivLU<Matrix3x3> lu(matrix);
    if (!lu.isInvertible()) {
        if (convoluted.pattern == Deconvolution::Pattern::ABC) {
            convoluted.pattern = Deconvolution::Pattern::BC;
            return _deconvolutePatternDouble(convoluted, trueChromatogram);
        } else {
            convoluted.pattern = Deconvolution::Pattern::CD;
            return _deconvolutePatternDouble(convoluted, trueChromatogram);
        }
    }

    double z_t = targetVector.dot(trueVector);
    double z_a = aVector.dot(trueVector);
    double z_b = bVector.dot(trueVector);

    VectorXd z(3);
    z << z_t,
         z_a,
         z_b;
    double coefficient = lu.solve(z)(0);
    if (coefficient <= 0.0)
        return vector<float>(begin(convoluted.target), end(convoluted.target));

    VectorXd deconvolutedTarget = coefficient * targetVector;
    return vector<float>(deconvolutedTarget.data(),
                         deconvolutedTarget.data()
                         + deconvolutedTarget.rows()
                         * deconvolutedTarget.cols());
}

vector<float> _deconvolutePatternQuadruple(Deconvolution::SignalVector convoluted,
                                           vector<double> trueChromatogram)
{
    VectorXd aVector;
    VectorXd bVector;
    VectorXd cVector;
    if (convoluted.pattern == Deconvolution::Pattern::ABCD) {
        aVector = Map<VectorXd>(convoluted.twoLeft.data(),
                                convoluted.twoLeft.size());
        bVector = Map<VectorXd>(convoluted.oneLeft.data(),
                                convoluted.oneLeft.size());
        cVector = Map<VectorXd>(convoluted.oneRight.data(),
                                convoluted.oneRight.size());
    } else if (convoluted.pattern == Deconvolution::Pattern::BCDE) {
        aVector = Map<VectorXd>(convoluted.oneLeft.data(),
                                convoluted.oneLeft.size());
        bVector = Map<VectorXd>(convoluted.oneRight.data(),
                                convoluted.oneRight.size());
        cVector = Map<VectorXd>(convoluted.twoRight.data(),
                                convoluted.twoRight.size());
    }
    VectorXd targetVector = Map<VectorXd>(convoluted.target.data(),
                                          convoluted.target.size());
    VectorXd trueVector = Map<VectorXd>(trueChromatogram.data(),
                                        trueChromatogram.size());

    double t_t = targetVector.squaredNorm();
    double a_a = aVector.squaredNorm();
    double b_b = bVector.squaredNorm();
    double c_c = cVector.squaredNorm();

    double t_a = targetVector.dot(aVector);
    double t_b = targetVector.dot(bVector);
    double t_c = targetVector.dot(cVector);

    double a_b = aVector.dot(bVector);
    double a_c = aVector.dot(cVector);

    double b_c = bVector.dot(cVector);

    typedef Matrix<double, 4, 4> Matrix4x4;
    Matrix4x4 matrix;
    matrix << t_t,  t_a,  t_b,  t_c,
              t_a,  a_a,  a_b,  a_c,
              t_b,  a_b,  b_b,  b_c,
              t_c,  a_c,  b_c,  c_c;
    Eigen::FullPivLU<Matrix4x4> lu(matrix);
    if (!lu.isInvertible()) {
        convoluted.pattern = Deconvolution::Pattern::BCD;
        return _deconvolutePatternTriple(convoluted, trueChromatogram);
    }

    double z_t = targetVector.dot(trueVector);
    double z_a = aVector.dot(trueVector);
    double z_b = bVector.dot(trueVector);
    double z_c = cVector.dot(trueVector);

    VectorXd z(4);
    z << z_t,
         z_a,
         z_b,
         z_c;
    double coefficient = lu.solve(z)(0);
    if (coefficient <= 0.0)
        return vector<float>(begin(convoluted.target), end(convoluted.target));

    VectorXd deconvolutedTarget = coefficient * targetVector;
    return vector<float>(deconvolutedTarget.data(),
                         deconvolutedTarget.data()
                         + deconvolutedTarget.rows()
                         * deconvolutedTarget.cols());
}

vector<float> _deconvolutePatternQuintuple(Deconvolution::SignalVector convoluted,
                                           vector<double> trueChromatogram)
{
    VectorXd aVector = Map<VectorXd>(convoluted.twoLeft.data(),
                                     convoluted.twoLeft.size());
    VectorXd bVector = Map<VectorXd>(convoluted.oneLeft.data(),
                                     convoluted.oneLeft.size());
    VectorXd cVector = Map<VectorXd>(convoluted.oneRight.data(),
                                     convoluted.oneRight.size());
    VectorXd dVector = Map<VectorXd>(convoluted.twoRight.data(),
                                     convoluted.twoRight.size());
    VectorXd targetVector = Map<VectorXd>(convoluted.target.data(),
                                          convoluted.target.size());
    VectorXd trueVector = Map<VectorXd>(trueChromatogram.data(),
                                        trueChromatogram.size());

    double t_t = targetVector.squaredNorm();
    double a_a = aVector.squaredNorm();
    double b_b = bVector.squaredNorm();
    double c_c = cVector.squaredNorm();
    double d_d = dVector.squaredNorm();

    double t_a = targetVector.dot(aVector);
    double t_b = targetVector.dot(bVector);
    double t_c = targetVector.dot(cVector);
    double t_d = targetVector.dot(dVector);

    double a_b = aVector.dot(bVector);
    double a_c = aVector.dot(cVector);
    double a_d = aVector.dot(dVector);

    double b_c = bVector.dot(cVector);
    double b_d = bVector.dot(dVector);

    double c_d = cVector.dot(dVector);

    typedef Matrix<double, 5, 5> Matrix5x5;
    Matrix5x5 matrix;
    matrix << t_t,  t_a,  t_b,  t_c,  t_d,
              t_a,  a_a,  a_b,  a_c,  a_d,
              t_b,  a_b,  b_b,  b_c,  b_d,
              t_c,  a_c,  b_c,  c_c,  c_d,
              t_d,  a_d,  b_d,  c_d,  d_d;
    Eigen::FullPivLU<Matrix5x5> lu(matrix);
    if (!lu.isInvertible()) {
        convoluted.pattern = Deconvolution::Pattern::BCD;
        return _deconvolutePatternTriple(convoluted, trueChromatogram);
    }

    double z_t = targetVector.dot(trueVector);
    double z_a = aVector.dot(trueVector);
    double z_b = bVector.dot(trueVector);
    double z_c = cVector.dot(trueVector);
    double z_d = dVector.dot(trueVector);

    VectorXd z(5);
    z << z_t,
         z_a,
         z_b,
         z_c,
         z_d;
    double coefficient = lu.solve(z)(0);
    if (coefficient <= 0.0)
        return vector<float>(begin(convoluted.target), end(convoluted.target));

    VectorXd deconvolutedTarget = coefficient * targetVector;
    return vector<float>(deconvolutedTarget.data(),
                         deconvolutedTarget.data()
                         + deconvolutedTarget.rows()
                         * deconvolutedTarget.cols());
}

vector<float> Deconvolution::execute(Deconvolution::SignalVector convoluted,
                                     vector<double> trueChromatogram)
{
    vector<float> deconvoluted;
    switch (convoluted.pattern) {
        case Pattern::BC:
            deconvoluted = _deconvolutePatternDouble(convoluted, trueChromatogram);
            break;
        case Pattern::CD:
            deconvoluted = _deconvolutePatternDouble(convoluted, trueChromatogram);
            break;
        case Pattern::ABC:
            deconvoluted = _deconvolutePatternTriple(convoluted, trueChromatogram);
            break;
        case Pattern::BCD:
            deconvoluted = _deconvolutePatternTriple(convoluted, trueChromatogram);
            break;
        case Pattern::CDE:
            deconvoluted = _deconvolutePatternTriple(convoluted, trueChromatogram);
            break;
        case Pattern::ABCD:
            deconvoluted = _deconvolutePatternQuadruple(convoluted, trueChromatogram);
            break;
        case Pattern::BCDE:
            deconvoluted = _deconvolutePatternQuadruple(convoluted, trueChromatogram);
            break;
        case Pattern::ABCDE:
            deconvoluted = _deconvolutePatternQuintuple(convoluted, trueChromatogram);
            break;
        default:
            return vector<float>(begin(convoluted.target),
                                 end(convoluted.target));
    }

    // clip all negative intensity values to zero
    for (size_t i = 0; i < deconvoluted.size(); ++i) {
        deconvoluted[i] = deconvoluted[i] < 0.0f ? 0.0f : deconvoluted[i];
    }

    return deconvoluted;
}
