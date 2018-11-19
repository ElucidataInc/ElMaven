#ifndef PROJECTDATABASE_H
#define PROJECTDATABASE_H

#include <iostream>
#include <map>
#include <set>
#include <vector>

class Adduct;
class Compound;
class Connection;
class mzSample;
class PeakGroup;
class Scan;

using namespace std;

/**
 * @brief The ProjectDatabase class is the main interface meant to be used by
 * applications to save/load project (session) data.
 */
class ProjectDatabase {

public:
    /**
     * @brief Create a ProjectDatabase instance and connect to the database.
     * @param dbFilename Absolute filename for the database file to be used.
     */
    ProjectDatabase(const string& dbFilename);

    /**
      * @brief Destroy the object and close database connection.
      */
    ~ProjectDatabase();

    /**
     * @brief Save information for a given set of samples.
     * @details Consequently, each sample saved is also assigned a unique ID.
     * Only samples with this ID should be used further to save peak groups,
     * peaks, scans and alignment information, since they are inherently
     * related to these samples and the relation will be stored as such.
     * @param samples A vector of ponters to mzSample objects.
     */
    void saveSamples(const vector<mzSample*>& samples);

    /**
     * @brief Save a given ser of peak groups.
     * @details For each group, this method calls `saveGroupAndPeaks. The peaks
     * associated with every group are also saved.
     * @param groups A vector of pointers to PeakGroup objects to be saved.
     * @param tableName An optional parameter to save table name for groups.
     */
    void saveGroups(const vector<PeakGroup*>& groups,
                    const string& tableName="");

    /**
     * @brief Save the given peak group, its sub-groups and their peaks
     * @details After calling this method with a peak group, the user does not
     * need to make extra calls to `saveGroupPeaks` for saving peaks of the
     * peak group. Similarly, the user need not call this again for saving
     * child groups for a group. The method recursively calls itself to save
     * the sub-groups (and their peaks) of the parent group provided.
     * @param group The PeakGroup which has to be saved, along with its
     * children and their collective set of Peak objects.
     * @param parentGroupId The group ID of the parent group, if any. Default
     * value of this parameter is 0 (for top-level groups).
     * @param tableName An optional parameter to save table name for the group.
     * @return An integer ID for the group saved.
     */
    int saveGroupAndPeaks(PeakGroup* group,
                          const int parentGroupId=0,
                          const string& tableName="");

    /**
     * @brief Save peaks for the given group.
     * @param group The peak group whose peaks need to be saved.
     * @param groupId The group ID for the group (as saved in the database).
     */
    void saveGroupPeaks(PeakGroup* group, const int groupId);

    /**
     * @brief Save compounds linked to a given set of groups.
     * @details This method filters out the total pool of Compound objects from
     * the given peak groups ans sends only the unique ones to be saved by the
     * an overloaded counterpart that takes in a set of compounds.
     * @param groups A vector of pointers to PeakGroup objects.
     */
    void saveCompounds(const vector<PeakGroup>& groups);

    /**
     * @brief Save a set of Compound objets.
     * @details This method uses a "REPLACE" SQL statement to ensure that
     * repeated primary keys are replaced and only one entry for a unique
     * Compound is saved.
     * @param seenCompounds A set of pointers to Compound objects.
     */
    void saveCompounds(const set<Compound*>& seenCompounds);

    /**
     * @brief Save alignment data for all scans in all given samples.
     * @param samples A vector of pointers to mzSample objects whose alignment
     * data needs to be saved. These samples should already have a unique ID.
     */
    void saveAlignment(const vector<mzSample*>& samples);

    /**
     * @brief Save some information about the scans of a set of samples.
     * @details Although there is a save method for scans information, there is
     * no load counterpart. So its still unclear, but this is probably used by
     * the author (Eugene) for their downstream work using the database file.
     * @param sampleSet A vector of pointers to mzSample objects whose scans
     * need to be saved. These samples should already have their uniqe ID set.
     */
    void saveScans(const vector<mzSample*>& sampleSet);

    /**
     * @brief Load sample filenames saved in the DB from a previous session.
     * @details The filename of the sample is saved during the save process,
     * and that filename is used to load the sample back in (unless its already
     * loaded). If the file no longer exists, the sample file is searched for
     * in the project directory (i.e., path of the database file connected
     * through this interface). Additionally, two levels of parent paths
     * relative to the executable are also searched for the sample file. Once
     * the samples from these filenames are loaded in, the `updateSamples`
     * method should be used to read in attributes of the samples from the
     * previous session.
     * @param loaded A vector of samples that have already been loaded. These
     * samples' names will not be returned even if they are saved in the DB.
     * @return A vector of strings, each a filename from which a sample can be
     * loaded.
     */
    vector<string> loadSampleNames(const vector<mzSample*> loaded);

    /**
     * @brief Update sample attributes saved in the DB from a previous session.
     * @details This method should be called with the set of samples that were
     * successfully loaded from filenames returned by `loadSampleNames` method.
     * Equivalence will be checked on the basis of sample name, i.e., sample
     * name should be same as saved in DB for the sample to be assigned its
     * attributes. Attributes loaded will include a unique ID that can be used
     * to further load related data structures like peak groups, peaks, etc.
     * that depend on existence of these samples.
     * @param loaded A vector of samples that have been loaded. Attributes will
     * only be updated for these samples.
     * @return A vector of pointers to mzSample objects that were successfully
     * loaded.
     */
    void updateSamples(const vector<mzSample*> freshlyLoaded);

    /**
     * @brief Load a PeakGroup object its peaks.
     * @details This method will also attempt to find the parent group of the
     * loaded group and try to associate with it. If not found, the group will
     * be added as a top-level group itself.
     * @param loaded A vector of loaded samples which will be associated with
     * peaks.
     * @return A vector of PeakGroup objects that were successfully loaded.
     */
    vector<PeakGroup*> loadGroups(const vector<mzSample*>& loaded);

    /**
     * @brief Load peaks for a given peak group.
     * @param group The PeakGroup for which peaks are to be loaded.
     * @param loaded A vector of loaded mzSample objects that will be iterated
     * through to associate each peak with.
     */
    void loadGroupPeaks(PeakGroup* group,
                        const vector<mzSample*>& loaded);

    /**
     * @brief Load saved compounds from the database file.
     * @details Each compound is checked whether it was previously loaded
     * through another operation. If the user supplies the optional parameter
     * `databaseName` then only compounds belonging to this DB name will be
     * loaded (if not already loaded). Upon successful load, no compounds from
     * this database name will be loaded through an instance of this object.
     * @param databaseName An optional parameter to filter loading of only
     * compounds belonging to this database.
     * @return A vector of pointers to new Compound objects loaded.
     */
    vector<Compound*> loadCompounds(const string databaseName="");

    /**
     * @brief Load alignment data and perform alignment on given sameples.
     * @details This method needs to be supplied with a vector of loaded samples
     * and these samples must have all the samples present in the previous
     * session. This is because alignment saves sample ID and this will be used
     * to set the original and updated retention time values, thus preforming
     * alignment. Having incorrect set of samples might result in misaligned
     * peaks.
     * @param loaded A vector of pointers to loaded samples whose scans are
     * to be aligned. Typically, these samples will have been loaded through
     * `loadSamples` method to ensure they all have a unique ID.
     */
    void loadAndPerformAlignment(const vector<mzSample*>& loaded);

    /**
     * @brief Drop all tables, deleting all data stored by any of the save
     * methods.
     */
    void deleteAll();

    /**
     * @brief Drop 'samples' table, removing all information stored for samples.
     */
    void deleteAllSamples();

    /**
     * @brief Drop 'compounds' tables, removing all information stored about
     * compounds.
     */
    void deleteAllCompounds();

    /**
     * @brief Delete 'peakgroups' and 'peaks' tables together because they are
     * not very useful without the other.
     */
    void deleteAllGroupsAndPeaks();

    /**
     * @brief Drop 'scans' tables, deleting all stored scans data.
     */
    void deleteAllScans();

    /**
     * @brief Drop all alignment data stored.
     */
    void deleteAllAlignmentData();

    /**
     * @brief Delete compounds for the given database name.
     * @param dbName Name of the database whose compounds are to be deleted.
     */
    void deleteCompoundsForDB(const string& dbName);

    /**
     * @brief Delete peaks and peak groups associated with a peak group table.
     * @param tableName Name of the table whose results are to be deleted.
     */
    void deleteTableGroups(const string& tableName);

    /**
     * @brief Delete a given peak group and any peaks associated with it.
     * @param group
     */
    void deletePeakGroup(PeakGroup* group);

    /**
     * @brief Get the names of all the peak group tables present.
     * @return A vector of strings as names of search tables.
     */
    vector<string> getTableNames();

    /**
     * @brief Get the parent path for the connected database file.
     * @return Path of DB file as a string.
     */
    string projectPath();

    /**
     * @brief Get the file name for the connected database file.
     * @return Name of DB file as a string.
     */
    string projectName();

private:
    /**
     * @brief _connection A Connection object mediating connection with a SQLite
     * database.
     */
    Connection* _connection;

    /**
     * @brief _loadedCompoundDatabases A list of names of the compound databases
     * that have already been loaded.
     */
    vector<string> _loadedCompoundDatabases;

    /**
     * @brief _compoundIdMap A map of unique ID mapping to a Compound object.
     * This map is used to determine whether a compound has already been loaded
     * and need not be loaded again.
     */
    map<string, Compound*> _compoundIdMap;

    /**
     * @brief Assign each sample in the given vector with a unique ID.
     * @details This unique ID is extremely important in ensuring that other
     * object information such as peaks, peaks groups, scans and alignment
     * data are correctly stored relating to these samples.
     * @param samples A vector of mzSample objects to be assigned with an ID.
     */
    void _assignSampleIds(const vector<mzSample*>& samples);

    /**
     * @brief Tries to find an Adduct object for the given ID.
     * @param id An adduct ID for positive H, negative H or zero H adduct.
     * @return An adduct object.
     */
    Adduct* _findAdductByName(string id);

    /**
     * @brief Find compound with the given ID and from the given database
     * @param id A string ID for the compound (unique for a compound database)
     * @param db Name of the compound database.
     * @return A Compound object if found, otherwise nullptr.
     */
    Compound* _findSpeciesById(string id, string databaseName);

    /**
     * @brief Find a compound using its name.
     * @param name A string name of the compound.
     * @param databaseName Name of the database to search using compound name.
     * @return A vector of compounds by the name.
     */
    vector<Compound*> _findSpeciesByName(string name, string databaseName);

    /**
     * @brief Checks whether compound database of the given name has already
     * been loaded or not.
     * @param databaseName Name of the compound database.
     * @return true if database has already been loaded, false otherwise.
     */
    bool _compoundDatabaseLoaded(string databaseName);

    /**
     * @brief Attempt to create a unique scan signature for a given Scan object.
     * @param scan A Scan object for which signature needs to be created.
     * @param limitSize A limiting number on the length of the scan signature.
     * @return A scan signature of the Scan as a string.
     */
    string _getScanSignature(Scan* scan, int limitSize);

    /**
     * @brief Find a given sample within one of the possible paths.
     * @param filepath Full file path for the sample to be searched for.
     * @param pathlist A list of paths where the sample will be searched for.
     * @return The path of the sample if found or an empty string otherwise.
     */
    string _locateSample(const string filepath,
                         const vector<string>& pathlist);
};

#endif // PROJECTDATABASE_H
