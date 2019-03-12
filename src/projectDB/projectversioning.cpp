#include <boost/filesystem.hpp>
#include <regex>

#include "connection.h"
#include "cursor.h"
#include "mzUtils.h"
#include "projectversioning.h"

namespace bfs = boost::filesystem;

namespace ProjectVersioning {

/**
 * Update this for every release where DB version changes. Intermediate
 * releases using the same DB version as the last mentioned release need not be
 * added.
 */
map<Version, int> appDbVersionMap = {
    {Version("0.6.0"), 0},
    {Version("0.7.0"), 1}
};

/**
 * Update this for every release where DB format changes. Please only add the
 * absolute minimum change that would be needed for moving from one database
 * format to the next. And DO NOT assume that any of the tables that should be
 * transformed already exist.
 */
map<int, string> dbVersionUpgradeScripts = {
    {
        0,
        "ALTER TABLE peakgroups ADD COLUMN fragmentation_fraction_matched REAL;"
        "ALTER TABLE peakgroups ADD COLUMN fragmentation_mz_frag_error REAL;"
        "ALTER TABLE peakgroups ADD COLUMN fragmentation_hypergeom_score REAL;"
        "ALTER TABLE peakgroups ADD COLUMN fragmentation_mvh_score REAL;"
        "ALTER TABLE peakgroups ADD COLUMN fragmentation_dot_product REAL;"
        "ALTER TABLE peakgroups ADD COLUMN fragmentation_weighted_dot_product REAL;"
        "ALTER TABLE peakgroups ADD COLUMN fragmentation_spearman_rank_corr REAL;"
        "ALTER TABLE peakgroups ADD COLUMN fragmentation_tic_matched REAL;"
        "ALTER TABLE peakgroups ADD COLUMN fragmentation_num_matches REAL;"
    }
};

////////////////////////////////////////////////////////////////////////////////

Version::Version(string version)
{
    string originalString = version;

    // remove spaces from the string, if any
    auto shuffled = std::remove_if(begin(version),
                                   end(version),
                                   [](unsigned char x) { return isspace(x); });
    version.erase(shuffled, end(version));

    // split on '.' character
    vector<string> versionVec;
    mzUtils::split(version, '.', versionVec);

    // get version numnbers
    _major = _minor = _patch = 0;
    try {
        if (versionVec.size() > 0)
            _major = stoi(versionVec.at(0));
        if (versionVec.size() > 1)
            _minor = stoi(versionVec.at(1));
        if (versionVec.size() > 2)
            _patch = stoi(versionVec.at(2));
    } catch(invalid_argument) {
        cerr << "Error: unexpected version string format - \""
             << originalString
             << "\""
             << endl;
    }
}

string Version::toString() const
{
    return to_string(_major)
           + '.'
           + to_string(_minor)
           + '.'
           + to_string(_patch);
}

ostream& operator << (ostream& os, const Version& v)
{
    os << v.toString();
    return os;
}

void Version::upMajor()
{
    ++_major;
}

void Version::upMinor()
{
    ++_minor;
}

void Version::upPatch()
{
    ++_patch;
}

bool Version::downMajor()
{
    if (_major != 0) {
        --_major;
        return true;
    }
    return false;
}

bool Version::downMinor()
{
    if (_minor != 0) {
        --_minor;
        return true;
    }
    return false;
}

bool Version::downPatch()
{
    if (_patch != 0) {
        --_patch;
        return true;
    }
    return false;
}

int Version::_compare(const Version& other) const
{
    int majorDiff = _major - other.major();
    int minorDiff = _minor - other.minor();
    int patchDiff = _patch - other.patch();

    if (majorDiff)
        return majorDiff;

    if (minorDiff)
        return minorDiff;

    return patchDiff;
}

////////////////////////////////////////////////////////////////////////////////

pair<string, string> extractVersionInfoFromTag(const string tag, int* aheadBy)
{
    regex rgx("v((\\d+\\.\\d+\\.\\d+)(-(beta|alpha))?(\\.(\\d)*)?(-(\\d+))?)");
    smatch matches;
    pair<string, string> found;

    if (regex_search(tag, matches, rgx)) {
        // captured group for version should exist on index 2
        found.first = matches[2].str();

        // captured groups for alpha/beta build stage should be on index 4 and 5
        found.second = matches[4].str() + matches[5].str();

        // captured group for commit count should be on index 8
        string commitCountStr = matches[8].str();
        if (aheadBy != nullptr && commitCountStr.size() > 0) {
            *aheadBy = stoi(commitCountStr);
        } else if (aheadBy != nullptr) {
            *aheadBy = 0;
        }
    } else {
        cerr << "Error: given tag string \""
             << tag
             << "\" does not have expected format"
             << endl;
    }

    return found;
}

int getDbVersionForApp(const Version& currentVersion)
{
    auto oldestVersion = begin(appDbVersionMap)->first;
    if (currentVersion < oldestVersion)
        return -1;

    Version olderVersion;
    Version laterVersion;
    for (const auto entry : appDbVersionMap) {
        auto version = entry.first;
        if (version < currentVersion) {
            olderVersion = version;
        } else {
            laterVersion = version;
            break;
        }
    }
    if (currentVersion == laterVersion)
        return appDbVersionMap.at(laterVersion);

    return appDbVersionMap.at(olderVersion);
}

int getLatestDbVersion()
{
    if (!appDbVersionMap.empty()) {
        auto lastElement = --end(appDbVersionMap);
        return lastElement->second;
    }
    return -1;
}

string generateUpgradeScript(const int fromVersion, const int toVersion)
{
    // the assumption here is that every database version must exist as a key
    if (!dbVersionUpgradeScripts.count(fromVersion)
        || !dbVersionUpgradeScripts.count(toVersion - 1)) {
        cerr << "No conversion script can be generated for: "
             << fromVersion << " → " << toVersion
             << endl;
        return "";
    }

    string mergedUpgradeScript;
    for (const auto entry : dbVersionUpgradeScripts) {
        int dbVersion = entry.first;
        string upgradeScript = entry.second;

        // relying on keys of map being sorted for generating the correct script
        if (dbVersion >= fromVersion && dbVersion < toVersion)
            mergedUpgradeScript += upgradeScript + "\n\n";
    }

    return mergedUpgradeScript;
}

void upgradeDatabase(const string& dbFilename,
                     const string& upgradeScript,
                     const string& appVersionString)
{
    bfs::path filepath(dbFilename);
    auto filename = filepath.stem().string();
    auto extension = filepath.extension().string();
    auto newFilename = filename + "(" + appVersionString + ")" + extension;
    auto newPath = filepath.parent_path() / bfs::path(newFilename);
    auto success = backupFile(filepath.string(), newPath.string());
    if (!success) {
        cerr << "Error: failed to backup original database; "
             << "continuing with the upgrade anyway …"
             << endl;
    }

    if (!upgradeScript.empty()) {
        Connection connection(dbFilename);
        connection.executeMulti(upgradeScript);
    }
}

bool backupFile(const string& originalFilepath, const string& newFilepath)
{
    cout << "Debug: backing up file "
         << originalFilepath
         << " as "
         << newFilepath
         << " …"
         << endl;

    bfs::path originalPath(originalFilepath);
    bfs::path newPath(newFilepath);
    try {
        bfs::copy_file(originalPath, newPath);
    } catch(boost::system::error_code) {
        cerr << "Error: failed to copy file" << endl;
        return false;
    }

    return true;
}

} // namespace ProjectVersioning
