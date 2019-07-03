#ifndef MZROLLDBCONVERTER_H
#define MZROLLDBCONVERTER_H

#include "connection.h"

namespace MzrollDbConverter
{
    /**
     * @brief This function helps convert an mzrollDB file to an emDB file.
     * @details The file that is converted will not be modified, instead a new
     * emDB file is created into which the data is copied.
     * @param fromPath The path of an mzrollDB file (that has to be converted).
     * @param toPath The path of an emDB file (in which converted data will be
     * stored).
     */
    void convertLegacyToCurrent(const std::string& fromPath,
                                const std::string& toPath);

    /**
     * @brief Copy samples from a given mzrollDB to emDB.
     * @param mzrollDb Database connection to an mzrollDB file.
     * @param emDb Database connection to an emDB file.
     */
    void copySamples(Connection& mzrollDb, Connection& emDb);

    /**
     * @brief Copy scans from a given mzrollDB to emDB.
     * @param mzrollDb Database connection to an mzrollDB file.
     * @param emDb Database connection to an emDB file.
     */
    void copyScans(Connection& mzrollDb, Connection& emDb);

    /**
     * @brief Copy peak groups from a given mzrollDB to emDB.
     * @param mzrollDb Database connection to an mzrollDB file.
     * @param emDb Database connection to an emDB file.
     */
    void copyPeakgroups(Connection& mzrollDb, Connection& emDb);

    /**
     * @brief Copy peaks from a given mzrollDB to emDB.
     * @param mzrollDb Database connection to an mzrollDB file.
     * @param emDb Database connection to an emDB file.
     */
    void copyPeaks(Connection& mzrollDb, Connection& emDb);

    /**
     * @brief Copy compounds from a given mzrollDB to emDB.
     * @param mzrollDb Database connection to an mzrollDB file.
     * @param emDb Database connection to an emDB file.
     */
    void copyCompounds(Connection& mzrollDb, Connection& emDb);

    /**
     * @brief Copy alignment data from a given mzrollDB to emDB.
     * @param mzrollDb Database connection to an mzrollDB file.
     * @param emDb Database connection to an emDB file.
     */
    void copyAlignmentData(Connection& mzrollDb, Connection& emDb);

    /**
     * @brief Set user version of an emDB file.
     * @param emDb Database connection to an emDB file.
     * @param version The integer version to set user version to.
     */
    void setVersion(Connection& emDb, int version);
} // namespace MzrollDbConverter

#endif // MZROLLDBCONVERTER_H
