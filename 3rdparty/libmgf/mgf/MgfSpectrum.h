#ifndef __MGFP_INCLUDE_MGFSPECTRUM_H__
#define __MGFP_INCLUDE_MGFSPECTRUM_H__

#include "config.h"

#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "Collection.h"
#include "MassAbundancePair.h"

namespace mgf {

/** An MgfSpectrum holds a fragment ion series and the corresponding (local)
 * search parameters. The parameter set, their values and syntax are taken from
 * the MatrixScience format help file at
 * http://www.matrixscience.com/help/data_file_help.html#GEN
 */
class MGFP_EXPORT MgfSpectrum : public Collection<MassAbundancePair>
{
  public:
    /** Default constructor. Constructs an empty MgfSpectrum.
     */
    MgfSpectrum();

    /** Get the precursor charges for which a search is requested.
     *  @see http://www.matrixscience.com/help/search_field_help.html#TWO
     *  @param[out] charges The charge vector.
     */
    std::vector<int> getCHARGE(void) const;

    /** Set the precursor charges for which a search is requested.
     *  @see http://www.matrixscience.com/help/search_field_help.html#TWO
     *  @param[in] charges The charge vector.
     */
    void setCHARGE(const std::vector<int>& charges);

    /** Get amino acid composition information.
     *  @see http://www.matrixscience.com/help/sq_help.html#COMP
     *  @return An amino acid composition string.
     */
    std::string getCOMP(void) const;

    /** Set the amino acid composition information.
     *  @see http://www.matrixscience.com/help/sq_help.html#COMP
     *  @param[in] An amino acid composition string.
     */
    void setCOMP(const std::string& comp);

    /** Get 
     *  @see 
     *  @return
     */
    std::string getETAG(void) const;

    /** Set 
     *  @see 
     *  @param[in] 
     */
    void setETAG(const std::string& etag);

    /** Get the instrument type/fragmentation rules. Based on the definitions in
     * the Mascot \c fragmentation_rules configuration files, this defines the
     * type of instrument Mascot expects and the fragmentation scheme used to
     * derive the theoretical fragementation spectra.
     *  @return The.
     */
    std::string getINSTRUMENT(void) const;

    /** Set the instrument type/fragementation rules. Valid values are defined
     * in the Mascot \c fragmentation_rules configuration file. There is no way
     * for \a libmgf to validate the user input, hence the responsibility lies
     * with the user.
     *  @param[in] instrument The
     */
    void setINSTRUMENT(const std::string& instrument);

    /** Get 
     *  @see 
     *  @return
     */
    void getIONS(std::vector<MassAbundancePair>& ions) const;

    /** Set 
     *  @see 
     *  @param[in] 
     */
    void setIONS(const std::vector<MassAbundancePair>& ions);

    /** Get the string representation of the variable modifications. The
     * modification string specifies which variable post-translational
     * modifications are searched in the Mascot query associated with the
     * current MgfSpectrum..
     *  @return A string of variable modifications.
     */
    std::string getIT_MODS(void) const;

    /** Set the variable modification string. The modfication string specifies which
     * variable post-translational modifications are searched in the Mascot
     * query associated with the current MgfSpectrum.
     * Strings should conform to UniMod standards.
     *  @param[in] it_mods The modification string.
     */
    void setIT_MODS(const std::string& it_mods);

    /** Get the peptide mass and (optionally) the abundance of the precursor.
     *  The \c .first part of the return value holds the precursor mass, in \a
     *  m/z. To determine relative (i.e. uncharged) mass, consult the precursor
     *  charges, available via getCHARGES(). If specified in the MGF file, the
     *  \c .second element of the return value holds the precursor abundance. If
     *  no precursor abundance was specified, then <tt> .second == 0</tt>.
     *  @return A pair of precursor mass and (optionally) abundance.
     */
    std::pair<double, double> getPEPMASS(void) const;

    /** Set the peptide mass and (optionally) the abundance of the precursor.
     *  \c pepmass.first must hold the precursor mass, in \a
     *  m/z. Optionally, \c pepmass.second may hold the precursor abundance. If
     *  no abundance value is available, \c pepmass.second must be set to zero.
     *  @param[in] pepmass A pair of precursor mass and (optionally) abundance.
     */
    void setPEPMASS(const std::pair<double, double> pepmass);

    /** Get the retention time range of the precursor scans. 
     *  @return The retention time range of the precursor scans, in seconds.
     *          If the precursors stem from a single scan, the .second entry
     *          of the returned std::pair equals -1.0.
     */
    std::pair<double,double> getRTINSECONDS(void) const;

    /** Set the retention time range of the precursor scans.
     *  @param[in] rtinseconds The retention time of the precursor scan(s), 
     *               in seconds. For single scans, use -1.0 as .second value
     *               or the alternate setRTINSECONDS function that takes a
     *               single double.
     */
    void setRTINSECONDS(const std::pair<double,double>& rtinseconds);

    /** Set the retention time range of a single precursor scan.
     *  @param[in] rtinseconds The retention time of the precursor scan, 
     *               in seconds. 
     */
    void setRTINSECONDS(const double rtinseconds);

    /** Get the scan number or scan number range from which this fragmentation
     *  spectrum was generated.
     *  @see http://www.matrixscience.com/help/data_file_help.html
     *  @return A pair of scan numbers. For fragmentation spectra of single 
     *          scans, the .second value of the return value equals -1.
     */
    std::pair<int, int> getSCANS(void) const;

    /** Set the scan number or scan number range from which this fragmentation
     *  spectrum was generated.
     *  @see http://www.matrixscience.com/help/data_file_help.html
     *  @param[in] A pair [start, end] of scan numbers. For single scans
     *             use [scanNumber, -1].
     */
    void setSCANS(const std::pair<int, int>& scans);

    /** Set the scan number from which this fragmentation
     *  spectrum was generated. This is just a convenience function wrapper
     *  for single scans.
     *  @see http://www.matrixscience.com/help/data_file_help.html
     *  @param[in] A scan number.
     */
    void setSCANS(const int scans);

    /** Get the amino acid sequence. Please consult the detailed description in
     * the Mascot documentation (see link).
     *  @see http://www.matrixscience.com/help/sq_help.html#SEQ
     *  @return The amino acid sequence.
     */
    std::string getSEQ(void) const;

    /** Get the amino acid sequence. Please consult the detailed description in
     * the Mascot documentation (see link).
     *  @see http://www.matrixscience.com/help/sq_help.html#SEQ
     *  @param[in] seq The amino acid sequence.
     */
    void setSEQ(const std::string& seq);

    /** Get the sequence tag. Please consult the detailed description in the
     *  Mascot documentation (see link).
     *  @see http://www.matrixscience.com/help/sq_help.html#TAG
     *  @return The sequence tag.
     */
    std::string getTAG(void) const;

    /** Set the sequence tag. Please consult the detailed description in the
     *  Mascot documentation (see link).
     *  @see http://www.matrixscience.com/help/sq_help.html#TAG
     *  @param[in] tag A sequence tag string.
     */
    void setTAG(const std::string& tag);

    /** Get the query title. This returns the title of the query that
     *  corresponds to the spectrum at hand.
     *  @see http://www.matrixscience.com/help/data_file_help.html
     *  @return The query title.
     */
    std::string getTITLE(void) const;

    /** Set the query title. Set the title of the query that corresponds to the
     * spectrum at hand.
     *  @see http://www.matrixscience.com/help/data_file_help.html
     *  @param[in] title The query title.
     */
    void setTITLE(const std::string& title);

    /** Get the search tolerance. The return value is unit-dependent.  The unit
     *  is available via getTOLU().
     *  @see http://www.matrixscience.com/help/search_field_help.html#TOL
     *  @return The search tolerance.
     */
    double getTOL(void) const;

    /** Set the search tolerance. The tolerance value depends on the tolerance
     *  unit which can be set using setTOLU().
     *  @see http://www.matrixscience.com/help/search_field_help.html#TOL
     *  @param[in] tol The
     */
    void setTOL(const double tol);

    /** Get the unit in which the tolerance is specified. Valid values are \a %,
     * \a ppm, \a mmu and \a Da).
     *  @return A string specifying the tolerance unit.
     */
    std::string getTOLU(void) const;

    /** Set the unit in which the tolerance is specified. Valid values are \a %,
     * \a ppm, \a mmu and \a Da).
     *  @param[in] tolu The tolerance unit.
     */
    void setTOLU(const std::string& tolu);

    /** Clear all data from the MgfSpectrum.
     */
    void clear();

    ///////////// Custom fields added for compliance with GNPS MGF /////////////

    /** Get the MS level of the experiment from which the data was recorded.
     *  @return An integer specifying the MS level.
     */
    int getMSLEVEL(void) const;

    /** Set the MS level of the experiment from which the data was recorded.
     *  @param[in] mslevel The MS level.
     */
    void setMSLEVEL(const int mslevel);

    /** Get name of the file which was used to extract data.
     *  @return A string specifying the file name.
     */
    std::string getFILENAME(void) const;

    /** Set name of the file which was used to extract data.
     *  @param[in] filename The file name.
     */
    void setFILENAME(const std::string& filename);

    /** Get ionization mode of the precursor ions, negative or positive.
     *  @return A string denoting the ionization mode.
     */
    std::string getIONMODE(void) const;

    /** Set ionization mode of the precursor ions, negative or positive.
     *  @param[in] ionmode The ionization mode.
     */
    void setIONMODE(const std::string& ionmode);

    /** Get identifier for organism from which the sample was extracted.
     *  @return A string specifying the organism.
     */
    std::string getORGANISM(void) const;

    /** Set identifier for organism from which the sample was extracted.
     *  @param[in] organism The organism.
     */
    void setORGANISM(const std::string& organism);

    /** Get SMILE string for the entry.
     *  @return A string storing the SMILE string.
     */
    std::string getSMILES(void) const;

    /** Set SMILE string for the entry.
     *  @param[in] smileString The SMILE string.
     */
    void setSMILES(const std::string& smileString);

    /** Get INCHI string identifier for the entry.
     *  @return A string storing the INCHI key.
     */
    std::string getINCHI(void) const;

    /** Set INCHI string identifier for the entry.
     *  @param[in] inchi The INCHI key.
     */
    void setINCHI(const std::string& inchi);

    /** Get identifier to a PubMed study associated with this data.
     *  @return A string storing the PubMed identifier.
     */
    int getPUBMED(void) const;

    /** Get identifier to a PubMed study associated with this data.
     *  @param[in] pubmed The PubMed ID/Link.
     */
    void setPUBMED(const int& pubmed);

    /** Get a unique identifier for this spectral entry.
     *  @return A string specifying the spectrum ID.
     */
    std::string getSPECTRUMID(void) const;

    /** Set a unique identifier for this spectral entry.
     *  @param[in] spectrumId The unique spectrum ID.
     */
    void setSPECTRUMID(const std::string& spectrumId);

  private:
    friend std::ostream& operator<<(std::ostream& os, const MgfSpectrum& mgf);
    std::vector<int> charges_;
    std::string comp_, etag_, instrument_, it_mods_;
    std::pair<double, double> pepmass_;
    std::pair<double, double> rtinseconds_;
    std::pair<int, int> scans_;
    std::string seq_, tag_, title_;
    double tol_;
    std::string tolu_;

    ///////////// Custom fields added for compliance with GNPS MGF /////////////
    int mslevel_;
    std::string filename_;
    std::string ionmode_;
    std::string organism_;
    std::string smiles_;
    std::string inchi_;
    int pubmed_;
    std::string spectrumId_;
};

/** An stream operator to output the fragment ion spectrum in MGF. This includes
 * all entries between <tt>BEGIN IONS</tt> and <tt>END IONS</tt> as well as all
 * local parameter key/value-pairs.
 */
std::ostream& operator<<(std::ostream& os, const MgfSpectrum& mgf);

} // namespace mgf

#endif

