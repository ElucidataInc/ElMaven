#ifndef PROJECTVERSIONING_H
#define PROJECTVERSIONING_H

#include <string>
#include <map>

using namespace std;

namespace ProjectVersioning {

/**
 * @brief The Version struct is used to represent a version number having
 * major, minor and patch levels (ref: htttps://semver.org). The most obvious
 * reason for the existence of this struct is to allow comparison between two
 * Version objects.
 */
struct Version {

    /**
     * @brief Create a version object from a version string of the form
     * "MAJOR.MINOR.PATCH". If the string is not of this form, the returned
     * Version object will most likely get an incorrect representation than what
     * was intended.
     * @param versionString String containing the version string to be converted
     * to a Version object of equivalent value.
     */
    Version(string versionString);

    /**
     * @brief Default constructor.
     */
    Version() = default;

    /**
     * @brief Increase major version number by 1.
     */
    void upMajor();

    /**
     * @brief Increase minor version number by 1.
     */
    void upMinor();

    /**
     * @brief Increase patch version number by 1.
     */
    void upPatch();

    /**
     * @brief Decrease major version number by 1. If current major version
     * number is 0, no decrement happens.
     * @return Whether a decrement occurred.
     */
    bool downMajor();

    /**
     * @brief Increase minor version number by 1. If current minor version
     * number is 0, no decrement happens.
     * @return Whether a decrement occurred.
     */
    bool downMinor();

    /**
     * @brief Increase patch version number by 1. If current patch version
     * number is 0, no decrement happens.
     * @return Whether a decrement occurred.
     */
    bool downPatch();

    /**
     * @brief Convert this object to its string representation.
     * @return String having the form "MAJOR.MINOR.PATCH".
     */
    string toString() const;

    int major() const { return _major; }
    int minor() const { return _minor; }
    int patch() const { return _patch; }

    bool operator < (const Version& rhs) const { return _compare(rhs) < 0; }
    bool operator > (const Version& rhs) const { return _compare(rhs) > 0; }
    bool operator <= (const Version& rhs) const { return _compare(rhs) <= 0; }
    bool operator >= (const Version& rhs) const { return _compare(rhs) >= 0; }
    bool operator == (const Version& rhs) const { return _compare(rhs) == 0; }
    bool operator != (const Version& rhs) const { return _compare(rhs) != 0; }
    friend ostream& operator << (ostream& os, const Version& v);

private:
    int _major;
    int _minor;
    int _patch;

    /**
     * @brief Compare Version object with another version object.
     * @param other Version object to compare against.
     * @return Positive integer if this object is of later version than `other`,
     * negative if this version is older than `other` and 0 if they represent
     * the same version.
     */
    int _compare(const Version& other) const;

};

/**
 * @brief This map is for storing db versions corresponding to the application
 * version they were meant to be used with. It should be made sure that this
 * mapping is up-to-date for every new release version of the application where
 * the database version changed. Failure to do so may cause irreversible
 * compatibility issues.
 */
extern map<Version, int> appDbVersionMap;

/**
 * @brief A map of versions mapping to SQL scripts. The script for database
 * version "1" can be used to upgrade a database having version "1" format to
 * one having version "2" format.
 */
extern map<int, string> dbVersionUpgradeScripts;

/**
 * @brief Extract the version string ("MAJOR.MINOR.PATCH") from a string that is
 * of the form "vMAJOR.MINOR.PATCH-xxx…". If the string also contains more
 * information, the beta or alpha stage of the build will also be extracted, if
 * present. This is temporarily useful for El-MAVEN's current version detection
 * setup.
 * @param tag Amalgamation of version and commit tag string obtained from
 * the application.
 * @param aheadBy Optional integer parameter which, if provided, will be set to
 * the number of commits the build might be ahead of from the tagged version.
 * This can be useful in telling whether the application is running as a dev
 * build.
 * @return A pair of strings. The first element is the version string (of form
 * "MAJOR.MINOR.PATCH") obtained from successful extraction. The second string
 * contains the alpha or beta buld stage of the running application, if found.
 * If extraction fails due to regex mismatch, both these strings are empty.
 */
pair<string, string> extractVersionInfoFromTag(const string tag,
                                               int* aheadBy=nullptr);

/**
 * @brief Fetches the database format version that will be compatible with a
 * given version.
 * @details The `appDbVersionMap` is a one-to-one mapping, i.e, only when the
 * database format changes, the structure is updated with the application
 * version (at which DB changes) and the incremented database version. This
 * means that for all application versions in between two consequtive records of
 * application version in the map, the same database format is used. To deduce
 * this however, the application version string should be of a definite form.
 * @param appVersion The version of application, as a string, for which database
 * version is to be fetched. This application version should be a string of the
 * form: "MAJOR.MINOR.PATCH" where MAJOR, MINOR and PATCH are integers
 * representing the major, minor and patch release numbers as dictated by the
 * semantic versioning document.
 * @return The database version as integer, which is compatible with the given
 * application version. Valid database versions are always positive integer
 * numbers. If no DB version can be deduced, -1 is returned, indicating failure.
 */
int getDbVersionForApp(const Version& currentVersion);

/**
 * @brief Obtain the latest database version available in `appDbVersionMap`.
 * @details This function should only be used to estimate the database version
 * required for development builds. For all release builds, `getDbVersionForApp`
 * function should be used instead.
 * @return The latest database version as integer.
 */
int getLatestDbVersion();

/**
 * @brief Generates a database upgrade SQL script for given starting version
 * and destination version.
 * @param fromVersion The current version of database that has to be upgraded.
 * @param toVersion The version of database to upgrade to.
 * @return A SQL script as a string, that is created by joining all SQL scripts
 * that are available to upgrade between every consequent DB version from
 * starting version to destination version, excluding the script for destination
 * version. Including the script for destination version would upgrade it by one
 * extra version.
 */
string generateUpgradeScript(const int fromVersion, const int toVersion);

/**
 * @brief Upgrade a SQLite database by executing an upgrade script and backup
 * the existing database.
 * @param dbFilename Absolute path of the database file to be backed up and
 * upgraded.
 * @param upgradeScript A valid SQL string that can be executed to mutate a
 * database to the new version without losing any more information than is
 * necessary.
 * @param appVersionString Version string for the application so that the
 * existing database can be backed up with a name that is suffixed with this
 * version string for appripriate identification.
 */
void upgradeDatabase(const string& dbFilename,
                     const string& upgradeScript,
                     const string& appVersionString);

/**
 * @brief Make a copy of a given file to another path.
 * @param originalFilepath Path of the file to be copied.
 * @param newFilepath Path at which the copied file should exist.
 * @return True if the backup was successful, false otherwise.
 */
bool backupFile(const string& originalFilepath, const string& newFilepath);

}

#endif // PROJECTVERSIONING_H
