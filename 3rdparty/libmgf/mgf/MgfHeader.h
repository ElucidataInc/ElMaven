#ifndef __MGFP_INCLUDE_MGFHEADER_H__
#define __MGFP_INCLUDE_MGFHEADER_H__

#include "config.h"

#include <map>
#include <ostream>
#include <string>
#include <vector>

#include "utils.h" // for existsOrEmpty()

namespace mgf {

/** Mascot Generic Format file header (global parameters) information. MGF files
 * hold global and local parameters. Whereas local parameters are stored with
 * the respective MgfSpectrum, MgfHeader is where the global variables reside.
 * The description and values of the variables are taken from the official
 * MatrixScience format information page at
 * http://www.matrixscience.com/help/data_file_help.html#GEN..
 */
class MGFP_EXPORT MgfHeader
{
  public:
    
    /** Get the name of the enzyme used for digestion.
     *  @see http://www.matrixscience.com/help/search_field_help.html#CLE
     *  @return The enzyme name (e.g. \a Trypsin).
     */
    std::string getCLE(void) const;

    /** Set the name of the enzyme used for digestion. If the MGF file is to be
     * searched, then the name needs to correspond to an enzyme name specified
     * in the Mascot enzyme file. Because there is no way for \a mgf to check
     * the validity of the string, the responsibility for valid enzyme names
     * lies with the user.
     *  @see http://www.matrixscience.com/help/search_field_help.html#CLE
     *  @param[in] cle The enzyme string (e.g. \a Trypsin).
     */
    void setCLE(const std::string& cle);
    
    /** Get the search title.
     *  @see http://www.matrixscience.com/help/search_field_help.html#COM
     *  @return The search title.
     */
    std::string getCOM(void) const;

    /** Set the search title.
     *  @see http://www.matrixscience.com/help/search_field_help.html#COM
     *  @param[in] com The search title.
     */
    void setCOM(const std::string& com);
    
    /** Get the database name agains which Mascot should search.
     *  @see http://www.matrixscience.com/help/search_field_help.html#DB
     *  @return The database name.
     */
    std::string getDB(void) const;

    /** Set the sequence database name that specifies the database against which
     *  searches should be carreid out.
     *  @see http://www.matrixscience.com/help/search_field_help.html#DB
     *  @param[in] db The database name.
     */
    void setDB(const std::string& db);
    
    /** Get the MS/MS data file format. Valid values are Mascot generic (the
     * default), Sequest (.DTA), Finnigan (.ASC), Micromass (.PKL), PerSeptive
     * (.PKS), Sciex API III, Bruker (.XML), mzData (.XML).
     *  @return The data file format.
     */
    std::string getFORMAT(void) const;

    /** Set the MS/MS data file format. Valid values are Mascot generic (the
     * default), Sequest (.DTA), Finnigan (.ASC), Micromass (.PKL), PerSeptive
     * (.PKS), Sciex API III, Bruker (.XML), mzData (.XML).
     *  @param[in] format Data file format string.
     */
    void setFORMAT(const std::string& format);
    
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
    
    /** Get the string representation of the variable modifications. The
     * modification string specifies which variable post-translational
     * modifications are searched in a Mascot query.
     *  @return A string of variable modifications.
     */
    std::string getIT_MODS(void) const;

    /** Set the variable modification string. The modfication string specifies which
     * variable post-translational modifications are searched in a Mascot query.
     * Strings should conform to UniMod standards.
     *  @param[in] it_mods The modification string.
     */
    void setIT_MODS(const std::string& it_mods);
    
    /** Get the unit used to specify the fragmentation ion tolerance. Valid
     * values are \a mmu and \a Da. 
     *  @return The.
     */
    std::string getITOLU(void) const;

    /** Set the unit used to specify the fragmentation ion tolerance. Valid
     * values are \a mmu and \a Da. Currently, the responsibility for valid
     * input lies with the user.
     *  @param[in] itolu The fragmentation ion tolerance unit.
     */
    void setITOLU(const std::string& itolu);
    
    /** Get the type of mass reported for the parent ion: \a monoisotopic or \a average.
     *  @return The 
     */
    std::string getMASS(void) const;

    /** Set the parent io mass type: \a monoisotopic or \a average.
     *  @param[in] mass The
     */
    void setMASS(const std::string& mass);
    
    /** Get the string representation of fixed modifications. The modification
     * string specifies which fixed post-translational modifications are
     * searched in all Mascot queries in the MgfFile object. The setting may be
     * overridden in individual MgfSpectrum instances.
     *  @return A string representation of the fixed modifications specified for
     *          the current MgfFile.
     */
    std::string getMODS(void) const;

    /** Set the string representation of fixed modifications. The modification
     * string specifies which fixed post-translational modifications are
     * searched in all Mascot queries in the MgfFile object. The setting may be
     * overridden in individual MgfSpectrum instances. The modification string
     * mus conform to UniMod standards.
     *  @param[in] mods A string representation of fixed modifications used for
     *                  all spectra in the cuffrrent MfgFile object (may be
     *                  overridden in particular MgfSpectrum objects).
     */
    void setMODS(const std::string& mods);
    
    /** Get the strign that identifies the quantitation method. Method strings
     * are defined in Mascot's \c quantitation.xml file. Defined for MIS only.
     *  @return The quantitation identificatn string.
     */
    std::string getQUANTITATION(void) const;

    /** Set the strign that identifies the quantitation method. Method strings
     * are defined in Mascot's \c quantitation.xml file. Defined for MIS only.
     *  @param[in] quantitation The quantitation identification string.
     */
    void setQUANTITATION(const std::string& quantitation);
    
    /** Get the maximum number of hits that are being reported in the results.
     * Vaild values are \a AUTO or an integer.
     *  @return The maximum number of hits, as a string.
     */
    std::string getREPORT(void) const;

    /** Set the maximum number of hits that are being reported in the results.
     * Vaild values are \a AUTO or an integer.
     *  @param[in] report The maximum number of hits (as a \c std::string).
     */
    void setREPORT(const std::string& report);
    
    /** Get the report type identifier. Valid values are 
     * - \a protein      
     * - \a peptide (Default for MIS)
     * - \a archive (MIS only)
     * - \a concise (Default for PMF)
     * - \a select (MIS only)
     * - \a unassigned (MIS only)
     *  @see http://www.matrixscience.com/help/results_help.html
     *  @return A report identifier string.
     */
    std::string getREPTYPE(void) const;

    /** Get the report type identifier. Valid values are 
     * - \a protein      
     * - \a peptide (Default for MIS)
     * - \a archive (MIS only)
     * - \a concise (Default for PMF)
     * - \a select (MIS only)
     * - \a unassigned (MIS only)
     *
     * It is the user's responsibility to supply valid identifiers.
     *  @see http://www.matrixscience.com/help/results_help.html
     *  @param[in] reptype The report type identifier.
     */
    void setREPTYPE(const std::string& reptype);
    
    /** Get the type of search specified for the underlying MgfFile object.
     * Valid values are \a PMF, \a SQ, \a MIS.
     *  @return The search type string.
     */
    std::string getSEARCH(void) const;

    /** Set the type of search specified for the underlying MgfFile object.
     * Valid values are \a PMF, \a SQ, \a MIS. It is the user's responsibility
     * to supply a valid string.
     *  @param[in] search The search type string.
     */
    void setSEARCH(const std::string& search);
    
    /** Get the taxonomy (as specified in Mascot's taxonomy file).
     *  @see http://www.matrixscience.com/help/search_field_help.html#TAXONOMY
     *  @return The taxonomy.
     */
    std::string getTAXONOMY(void) const;

    /** Set the taxonomy (as specified in Mascot's taxonomy file).
     *  @see http://www.matrixscience.com/help/search_field_help.html#TAXONOMY
     *  @param[in] taxonomy The taxonomy.
     */
    void setTAXONOMY(const std::string& taxonomy);
    
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
    
    /** Get the user email address.
     *  @return A user email address string.
     */
    std::string getUSEREMAIL(void) const;

    /** Set the user email address.
     *  @param[in] useremail The user email address as \a abc@example.org.
     */
    void setUSEREMAIL(const std::string& useremail);
    
    /** Get the name of the user submitting the search.
     *  @return The.username.
     */
    std::string getUSERNAME(void) const;

    /** Set the name of the user submitting the search.
     *  @param[in] username The username.
     */
    void setUSERNAME(const std::string& username);
    
    /** Get the number of allowed missed cleavages. The range allowed by Mascot
     *  is [0-9], defaults to 1.
     *  @see http://www.matrixscience.com/help/search_field_help.html#PFA
     *  @return The number of missed cleavages used for the current MgfFile.
     */
    int getPFA(void) const;

    /** Set the number of allowed missed cleavages. The range allowd by Mascot
     * is [0-9], defaults to 1.
     *  @see http://www.matrixscience.com/help/search_field_help.html#PFA
     *  @param[in] pfa The number of missed cleavages.
     */
    void setPFA(const int pfa);
    
    /** Get the decoy search flag (0=false, 1=true).
     *  @return The decoy search flag.
     */
    int getDECOY(void) const;

    /** Set the decoy search flag (0=false, 1=true).
     *  @param[in] decoy The decoy search flag.
     */
    void setDECOY(const int decoy);
    
    /** Get the status of the error-tolerant search flag.
     *  @return The flag value (0=no error-tolerant search, 1=error-tolerant
     *          search)
     */
    int getERRORTOLERANT(void) const;

    /** Set the status of the error-tolerant search flag. Error-tolerant searchs
     * are not available for PMF queries.
     *  @param[in] errortolerant The desired value of the error-tolerant search
     *                           flag (0=off, 1=on). 
     */
    void setERRORTOLERANT(const int errortolerant);
    
    /** Get the status of the isotope error correction flag. The flag controls
     *  whether Mascot also attempts to find peptide-spectral matches for a
     *  precursor mass -1m/z. This counters situations in which the instrument
     *  picks the second isotope as parent mass.
     *  @return The.isotope error correction flag value (0=off, 1=on)
     */
    double getPEP_ISOTOPE_ERROR(void) const;

    /** Set/unset the peptide isotope error correction flag. The flag controls
     *  whether Mascot also attempts to find peptide-spectral matches for a
     *  precursor mass -1m/z. This counters situations in which the instrument
     *  picks the second isotope as parent mass.
     *  @param[in] pep_isotope_error The desired value of the peptide isotope
     *                               error flag.
     */
    void setPEP_ISOTOPE_ERROR(const double pep_isotope_error);
    
    /** Get the fragment ion tolerance. Keep in mind that the value is unit
     *  dependent and the unit is available via getITOLU().
     *  @see http://www.matrixscience.com/help/search_field_help.html#ITOL
     *  @return The fragment ion tolerance.
     */
    double getITOL(void) const;

    /** Set the fragment ion tolerance. Also make sure to use setITOLU() to
     *  specify the correct fragment ion tolerance unit.
     *  @see http://www.matrixscience.com/help/search_field_help.html#ITOL
     *  @param[in] itol The fragment ion tolerance.
     */
    void setITOL(const double itol);
    
    /** Get the scan precursor mass (in m/z).
     *  @see http://www.matrixscience.com/help/search_field_help.html#PRECURSOR
     *  @return The.mass of the precursor.
     */
    double getPRECURSOR(void) const;

    /** Set the precursor mass (in m/z).
     *  @see http://www.matrixscience.com/help/search_field_help.html#PRECURSOR
     *  @param[in] precursor The mass value of the precursor (in m/z).
     */
    void setPRECURSOR(const double precursor);
    
    /** Get the mass of the intact protein. The intacat mass is in Da, applied
     *  as a sliding windows. Follow the link for more information.
     *  @see http://www.matrixscience.com/help/search_field_help.html#SEG
     *  @return The protein mass in Da.
     */
    double getSEG(void) const;

    /** Set the mass of the intact protein. The intacat mass is in Da, applied
     *  as a sliding windows. Follow the link for more information and
     *  information on how to set the parameter correctly.
     *  @see http://www.matrixscience.com/help/search_field_help.html#SEG
     *  @see http://www.matrixscience.com/help/pmf_help.html#SEG
     *  @param[in] seg The
     */
    void setSEG(const double seg);
    
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
    
    /** Get the precursor charges for which a search is requested.
     *  @see http://www.matrixscience.com/help/search_field_help.html#TWO
     *  @param[out] charges The charge vector.
     */
    void getCHARGE(std::vector<int>& charges) const;

    /** Set the precursor charges for which a search is requested.
     *  @see http://www.matrixscience.com/help/search_field_help.html#TWO
     *  @param[in] charges The charge vector.
     */
    void setCHARGE(const std::vector<int>& charges);
    
    /** Get the type of NA translation. The default value is an array of
     *  [1, 2, 3, 4, 5, 6]. Documentation on this parameter and its exact use is
     *  sparse.
     *  @see http://www.matrixscience.com/help/data_file_help.html#PARAMS
     *  @return The list of translations (?).
     */
    void getFRAMES(std::vector<int>& frames) const;

    /** Get the type of NA translation. The default value is an array of
     *  [1, 2, 3, 4, 5, 6]. Documentation on this parameter and its exact use is
     *  sparse.
     *  @see http://www.matrixscience.com/help/data_file_help.html#PARAMS
     *  @param[in] frames The list of translations (?).
     */
    void setFRAMES(const std::vector<int>& frames);

    /** Clear all information from the current MgfHeader object.
     */
    void clear();

  private:
    /** Output operator. Defining the stream operator as a friend keeps the
     * header elements accessible. This allows us to iterate over the elemtents
     * instead of calling the \c get function for each entry and checking the
     * return value.
     */
    friend std::ostream& operator<<(std::ostream& os, const MgfHeader& mgf);

    /** Container for all string elements.
     */
    std::map<std::string, std::string> strings;
    
    /** Container for all integer elements.
     */
    std::map<std::string, int> ints;
    
    /** Container for all double elements.
     */
    std::map<std::string, double> doubles;

    /** Charge vector.
     */
    std::vector<int> charges_;
    
    /** Frame vector.
     */
    std::vector<int> frames_;
};

/** A stream operator to output the header information in MGF.
 */
std::ostream& operator<<(std::ostream& os, const MgfHeader& mgf);

} //namespace mgf

#endif

