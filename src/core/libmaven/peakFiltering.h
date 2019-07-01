#ifndef PEAKFILTERING_H
#define PEAKFILTERING_H

#include <iostream>

class Peak;
class EIC;
class MavenParameters;

using namespace std;

class PeakFiltering
{
  public:
	/**
	 * @brief Constructor of class PeakFiltering
	 * @param mavenParameters Pointer to class MavenParameters
	 * @see MavenParameters
	 * @see Peak
	 */
	PeakFiltering(MavenParameters *mavenParameters, bool isIsotope);

	/**
	 * @brief Filter peaks in vector of EICs
	 * @param eics Vector of pointers to class EIC
	 * @see EIC
	 * @see Peak
	 */
	void filter(vector<EIC*> &eics);

	/**
	 * @brief Filter peaks in an EIC
	 * @param eic Pointer to class EIC
	 * @see EIC
	 */
	void filter(EIC *eic);

	/**
	 * @brief filter vector of peaks
	 * @param peaks vector of pointers to class Peak
	 */
	void filter(vector<Peak> &peaks);

	/**
	 * @brief Returns true if peak is filtered
	 * @param peak Reference to class Peak
	 * @return bool True if peak is filtered
	 */
	bool filter(Peak &peak);

  private:
	bool _isIsotope;
	MavenParameters *_mavenParameters;
};

#endif //PEAKFILTERING_H
