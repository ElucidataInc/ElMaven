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
    {Version("0.7.0"), 1},
    {Version("0.8.0"), 2},
    {Version("0.9.0"), 3},
    {Version("0.10.0"), 4}
};

/**
 * Update this for every release where DB format changes. Please only add the
 * absolute minimum change that would be needed for moving from one database
 * format to the next. Also, please ensure that each SQL statement ends with a
 * semi-colon (;), as it would according to SQLite standard syntax.
 */
map<int, string> dbVersionUpgradeScripts = {
    {
        0,
        "BEGIN TRANSACTION;"

        // changes to peakgroups table
        "ALTER TABLE peakgroups ADD COLUMN min_quality REAL;"
        "ALTER TABLE peakgroups ADD COLUMN fragmentation_fraction_matched REAL;"
        "ALTER TABLE peakgroups ADD COLUMN fragmentation_mz_frag_error REAL;"
        "ALTER TABLE peakgroups ADD COLUMN fragmentation_hypergeom_score REAL;"
        "ALTER TABLE peakgroups ADD COLUMN fragmentation_mvh_score REAL;"
        "ALTER TABLE peakgroups ADD COLUMN fragmentation_dot_product REAL;"
        "ALTER TABLE peakgroups ADD COLUMN fragmentation_weighted_dot_product REAL;"
        "ALTER TABLE peakgroups ADD COLUMN fragmentation_spearman_rank_corr REAL;"
        "ALTER TABLE peakgroups ADD COLUMN fragmentation_tic_matched REAL;"
        "ALTER TABLE peakgroups ADD COLUMN fragmentation_num_matches REAL;"
        "ALTER TABLE peakgroups ADD COLUMN sample_ids TEXT;"

        // changes to peaks table
        "ALTER TABLE peaks ADD COLUMN peak_spline_area REAL;"

        // due to change in primary key, the entire table needs to be recreated
        "ALTER TABLE compounds RENAME TO compounds_old;"
        "CREATE TABLE compounds ( compound_id           TEXT                 "
        "                       , db_name               TEXT                 "
        "                       , name                  TEXT                 "
        "                       , formula               TEXT                 "
        "                       , smile_string          TEXT                 "
        "                       , srm_id                TEXT                 "
        "                       , mass                  REAL                 "
        "                       , charge                INTEGER              "
        "                       , expected_rt           REAL                 "
        "                       , precursor_mz          REAL                 "
        "                       , product_mz            REAL                 "
        "                       , collision_energy      REAL                 "
        "                       , log_p                 REAL                 "
        "                       , virtual_fragmentation INTEGER              "
        "                       , ionization_mode       INTEGER              "
        "                       , category              TEXT                 "
        "                       , fragment_mzs          TEXT                 "
        "                       , fragment_intensity    TEXT                 "
        "                       , fragment_ion_types    TEXT                 "
        "                       , note                  TEXT                 "
        "                       , PRIMARY KEY (compound_id, name, db_name) );"
        "INSERT INTO compounds ( compound_id           "
        "                      , db_name               "
        "                      , name                  "
        "                      , formula               "
        "                      , smile_string          "
        "                      , srm_id                "
        "                      , mass                  "
        "                      , charge                "
        "                      , expected_rt           "
        "                      , precursor_mz          "
        "                      , product_mz            "
        "                      , collision_energy      "
        "                      , log_p                 "
        "                      , virtual_fragmentation "
        "                      , ionization_mode       "
        "                      , category              "
        "                      , fragment_mzs          "
        "                      , fragment_intensity   )"
        "                 SELECT compound_id           "
        "                      , db_name               "
        "                      , name                  "
        "                      , formula               "
        "                      , smile_string          "
        "                      , srm_id                "
        "                      , mass                  "
        "                      , charge                "
        "                      , expected_rt           "
        "                      , precursor_mz          "
        "                      , product_mz            "
        "                      , collision_energy      "
        "                      , log_p                 "
        "                      , virtual_fragmentation "
        "                      , ionization_mode       "
        "                      , category              "
        "                      , fragment_mzs          "
        "                      , fragment_intensity    "
        "                   FROM compounds_old        ;"
        "DROP TABLE compounds_old;"

        "COMMIT;"
    },
    {
        1,
        "BEGIN TRANSACTION;"
        "ALTER TABLE user_settings ADD COLUMN must_have_fragmentation INTEGER;"
        "COMMIT;"
    },
    {
        2,
        "BEGIN TRANSACTION;"

        "ALTER TABLE peakgroups ADD COLUMN slice_mz_min REAL;"
        "ALTER TABLE peakgroups ADD COLUMN slice_mz_max REAL;"
        "ALTER TABLE peakgroups ADD COLUMN slice_rt_min REAL;"
        "ALTER TABLE peakgroups ADD COLUMN slice_rt_max REAL;"
        "ALTER TABLE peakgroups ADD COLUMN slice_ion_count REAL;"
        "ALTER TABLE peakgroups ADD COLUMN table_group_id INTEGER;"

        "ALTER TABLE user_settings RENAME TO user_settings_old;"
        "CREATE TABLE user_settings ( ionization_mode                  INTEGER "
        "                           , ionization_type                  INTEGER "
        "                           , instrument_type                  INTEGER "
        "                           , q1_accuracy                      REAL    "
        "                           , q3_accuracy                      REAL    "
        "                           , filterline                       INTEGER "
        "                           , centroid_scans                   INTEGER "
        "                           , scan_filter_polarity             INTEGER "
        "                           , scan_filter_ms_level             INTEGER "
        "                           , scan_filter_min_quantile         INTEGER "
        "                           , scan_filter_min_intensity        INTEGER "
        "                           , upload_multiprocessing           INTEGER "
        "                           , eic_smoothing_algorithm          INTEGER "
        "                           , eic_smoothing_window             INTEGER "
        "                           , max_rt_difference_bw_peaks       REAL    "
        "                           , asls_baseline_mode               INTEGER "
        "                           , baseline_quantile                INTEGER "
        "                           , baseline_smoothing_window        INTEGER "
        "                           , asls_smoothness                  INTEGER "
        "                           , asls_asymmetry                   INTEGER "
        "                           , isotope_filter_equal_peak        INTEGER " 
        "                           , min_signal_baseline_diff         REAl    " 
        "                           , min_peak_quality                 REAL    " 
        "                           , isotope_min_signal_baseline_diff REAL    " 
        "                           , isotope_min_peak_quality         REAL    " 
        "                           , d2_label_bpe                     INTEGER " 
        "                           , c13_label_bpe                    INTEGER " 
        "                           , n15_label_bpe                    INTEGER " 
        "                           , s34_label_bpe                    INTEGER " 
        "                           , min_isotope_parent_correlation   REAL    " 
        "                           , max_isotope_scan_diff            INTEGER " 
        "                           , abundance_threshold              REAL    " 
        "                           , max_natural_abundance_error      REAL    " 
        "                           , correct_c13_isotope_abundance    INTEGER " 
        "                           , eic_type                         INTEGER " 
        "                           , use_overlap                      INTEGER " 
        "                           , dist_x_weight                    INTEGER " 
        "                           , dist_y_weight                    INTEGER " 
        "                           , overlap_weight                   INTEGER "  
        "                           , consider_delta_rt                INTEGER "
        "                           , quality_weight                   INTEGER "
        "                           , intensity_weight                 INTEGER "
        "                           , delta_rt_weight                  INTEGER "
        "                           , mass_cutoff_type                 TEXT    "
        "                           , automated_detection              INTEGER "
        "                           , mass_domain_resolution           REAL    "
        "                           , time_domain_resolution           INTEGER "
        "                           , min_mz                           REAL    "
        "                           , max_mz                           REAL    "
        "                           , min_rt                           REAL    "
        "                           , max_rt                           REAL    "
        "                           , min_intensity                    REAL    "
        "                           , max_intensity                    REAL    "
        "                           , database_search                  INTEGER "
        "                           , compound_extraction_window       REAL    "
        "                           , match_rt                         INTEGER "
        "                           , compound_rt_window               REAL    "
        "                           , limit_groups_per_compound        INTEGER "
        "                           , match_fragmentation              INTEGER "
        "                           , min_frag_match_score             REAL    " 
        "                           , fragment_tolerance               REAL    "
        "                           , min_frag_match                   REAL    "
        "                           , report_isotopes                  INTEGER "
        "                           , peak_quantitation                INTEGER "
        "                           , min_group_intensity              REAL    "
        "                           , intensity_quantile               INTEGER "
        "                           , min_group_quality                REAL    "
        "                           , quality_quantile                 INTEGER "
        "                           , min_signal_blank_ratio           REAL    "
        "                           , signal_blank_ratio_quantile      INTEGER "
        "                           , min_signal_baseline_ratio        REAL    "
        "                           , signal_baseline_ratio_quantile   INTEGER "
        "                           , min_peak_width                   INTEGER "
        "                           , min_good_peak_count              INTEGER "
        "                           , peak_classifier_file             TEXT    "
        "                           , alignment_good_peak_count        INTEGER "
        "                           , alignment_limit_group_count      INTEGER "
        "                           , alignment_peak_grouping_window   INTEGER "
        "                           , alignemnt_wrt_expected_rt        INTEGER "
        "                           , alignment_min_peak_intensity     REAL    "
        "                           , alignment_min_signal_noise_ratio INTEGER "
        "                           , alignment_min_peak_width         INTEGER "
        "                           , alignment_peak_detection         INTEGER "
        "                           , poly_fit_num_iterations          INTEGER "
        "                           , poly_fit_polynomial_degree       INTEGER "
        "                           , obi_warp_reference_sample        INTEGER "
        "                           , obi_warp_show_advance_params     INTEGER "
        "                           , obi_warp_score                   INTEGER "
        "                           , obi_warp_response                REAL    "
        "                           , obi_warp_bin_size                REAL    "
        "                           , obi_warp_gap_init                REAL    "
        "                           , obi_warp_gap_extend              REAL    "
        "                           , obi_warp_factor_diag             REAL    "
        "                           , obi_warp_factor_gap              REAL    "
        "                           , obi_warp_no_standard_normal      REAL    "
        "                           , obi_warp_local                   REAL    "
        "                           , main_window_selected_db_name     TEXT    "
        "                           , main_window_charge               INTEGER "
        "                           , main_window_peak_quantitation    INTEGER "
        "                           , main_window_mass_resolution      REAL    "
        "                           , must_have_fragmentation          INTEGER );"
        "INSERT INTO user_settings ( ionization_mode                  "
        "                          , ionization_type                  "
        "                          , instrument_type                  "
        "                          , q1_accuracy                      "
        "                          , q3_accuracy                      "
        "                          , filterline                       "
        "                          , centroid_scans                   "
        "                          , scan_filter_polarity             "
        "                          , scan_filter_ms_level             "
        "                          , scan_filter_min_quantile         "
        "                          , scan_filter_min_intensity        "
        "                          , upload_multiprocessing           "
        "                          , eic_smoothing_algorithm          "
        "                          , eic_smoothing_window             "
        "                          , max_rt_difference_bw_peaks       "
        "                          , asls_baseline_mode               "
        "                          , baseline_quantile                "
        "                          , baseline_smoothing_window        "
        "                          , asls_smoothness                  "
        "                          , asls_asymmetry                   "
        "                          , isotope_filter_equal_peak        " 
        "                          , min_signal_baseline_diff         " 
        "                          , min_peak_quality                 " 
        "                          , isotope_min_signal_baseline_diff " 
        "                          , isotope_min_peak_quality         " 
        "                          , d2_label_bpe                     " 
        "                          , c13_label_bpe                    " 
        "                          , n15_label_bpe                    " 
        "                          , s34_label_bpe                    " 
        "                          , min_isotope_parent_correlation   " 
        "                          , max_isotope_scan_diff            " 
        "                          , abundance_threshold              " 
        "                          , max_natural_abundance_error      " 
        "                          , correct_c13_isotope_abundance    " 
        "                          , eic_type                         " 
        "                          , use_overlap                      " 
        "                          , dist_x_weight                    " 
        "                          , dist_y_weight                    " 
        "                          , overlap_weight                   "  
        "                          , consider_delta_rt                "
        "                          , quality_weight                   "
        "                          , intensity_weight                 "
        "                          , delta_rt_weight                  "
        "                          , mass_cutoff_type                 "
        "                          , automated_detection              "
        "                          , mass_domain_resolution           "
        "                          , time_domain_resolution           "
        "                          , min_mz                           "
        "                          , max_mz                           "
        "                          , min_rt                           "
        "                          , max_rt                           "
        "                          , min_intensity                    "
        "                          , max_intensity                    "
        "                          , database_search                  "
        "                          , compound_extraction_window       "
        "                          , match_rt                         "
        "                          , compound_rt_window               "
        "                          , limit_groups_per_compound        "
        "                          , match_fragmentation              "
        "                          , min_frag_match_score             " 
        "                          , fragment_tolerance               "
        "                          , min_frag_match                   "
        "                          , report_isotopes                  "
        "                          , peak_quantitation                "
        "                          , min_group_intensity              "
        "                          , intensity_quantile               "
        "                          , min_group_quality                "
        "                          , quality_quantile                 "
        "                          , min_signal_blank_ratio           "
        "                          , signal_blank_ratio_quantile      "
        "                          , min_signal_baseline_ratio        "
        "                          , signal_baseline_ratio_quantile   "
        "                          , min_peak_width                   "
        "                          , min_good_peak_count              "
        "                          , peak_classifier_file             "
        "                          , alignment_good_peak_count        "
        "                          , alignment_limit_group_count      "
        "                          , alignment_peak_grouping_window   "
        "                          , alignemnt_wrt_expected_rt        "
        "                          , alignment_min_peak_intensity     "
        "                          , alignment_min_signal_noise_ratio "
        "                          , alignment_min_peak_width         "
        "                          , alignment_peak_detection         "
        "                          , poly_fit_num_iterations          "
        "                          , poly_fit_polynomial_degree       "
        "                          , obi_warp_reference_sample        "
        "                          , obi_warp_show_advance_params     "
        "                          , obi_warp_score                   "
        "                          , obi_warp_response                "
        "                          , obi_warp_bin_size                "
        "                          , obi_warp_gap_init                "
        "                          , obi_warp_gap_extend              "
        "                          , obi_warp_factor_diag             "
        "                          , obi_warp_factor_gap              "
        "                          , obi_warp_no_standard_normal      "
        "                          , obi_warp_local                   "
        "                          , main_window_selected_db_name     "
        "                          , main_window_charge               "
        "                          , main_window_peak_quantitation    "
        "                          , main_window_mass_resolution      "
        "                          , must_have_fragmentation          )"
        "                     SELECT ionization_mode                  "
        "                          , ionization_type                  "
        "                          , instrument_type                  "
        "                          , q1_accuracy                      "
        "                          , q3_accuracy                      "
        "                          , filterline                       "
        "                          , centroid_scans                   "
        "                          , scan_filter_polarity             "
        "                          , scan_filter_ms_level             "
        "                          , scan_filter_min_quantile         "
        "                          , scan_filter_min_intensity        "
        "                          , upload_multiprocessing           "
        "                          , eic_smoothing_algorithm          "
        "                          , eic_smoothing_window             "
        "                          , max_rt_difference_bw_peaks       "
        "                          , asls_baseline_mode               "
        "                          , baseline_quantile                "
        "                          , baseline_smoothing_window        "
        "                          , asls_smoothness                  "
        "                          , asls_asymmetry                   "
        "                          , isotope_filter_equal_peak        " 
        "                          , min_signal_baseline_diff         " 
        "                          , min_peak_quality                 " 
        "                          , isotope_min_signal_baseline_diff " 
        "                          , isotope_min_peak_quality         " 
        "                          , d2_label_bpe                     " 
        "                          , c13_label_bpe                    " 
        "                          , n15_label_bpe                    " 
        "                          , s34_label_bpe                    " 
        "                          , min_isotope_parent_correlation   " 
        "                          , max_isotope_scan_diff            " 
        "                          , abundance_threshold              " 
        "                          , max_natural_abundance_error      " 
        "                          , correct_c13_isotope_abundance    " 
        "                          , eic_type                         " 
        "                          , use_overlap                      " 
        "                          , dist_x_weight                    " 
        "                          , dist_y_weight                    " 
        "                          , overlap_weight                   "  
        "                          , consider_delta_rt                "
        "                          , quality_weight                   "
        "                          , intensity_weight                 "
        "                          , delta_rt_weight                  "
        "                          , mass_cutoff_type                 "
        "                          , automated_detection              "
        "                          , mass_domain_resolution           "
        "                          , time_domain_resolution           "
        "                          , min_mz                           "
        "                          , max_mz                           "
        "                          , min_rt                           "
        "                          , max_rt                           "
        "                          , min_intensity                    "
        "                          , max_intensity                    "
        "                          , database_search                  "
        "                          , compound_extraction_window       "
        "                          , match_rt                         "
        "                          , compound_rt_window               "
        "                          , limit_groups_per_compound        "
        "                          , match_fragmentation              "
        "                          , min_frag_match_score             " 
        "                          , fragment_tolerance               "
        "                          , min_frag_match                   "
        "                          , report_isotopes                  "
        "                          , peak_quantitation                "
        "                          , min_group_intensity              "
        "                          , intensity_quantile               "
        "                          , min_group_quality                "
        "                          , quality_quantile                 "
        "                          , min_signal_blank_ratio           "
        "                          , signal_blank_ratio_quantile      "
        "                          , min_signal_baseline_ratio        "
        "                          , signal_baseline_ratio_quantile   "
        "                          , min_peak_width                   "
        "                          , min_good_peak_count              "
        "                          , peak_classifier_file             "
        "                          , alignment_good_peak_count        "
        "                          , alignment_limit_group_count      "
        "                          , alignment_peak_grouping_window   "
        "                          , alignemnt_wrt_expected_rt        "
        "                          , alignment_min_peak_intensity     "
        "                          , alignment_min_signal_noise_ratio "
        "                          , alignment_min_peak_width         "
        "                          , alignment_peak_detection         "
        "                          , poly_fit_num_iterations          "
        "                          , poly_fit_polynomial_degree       "
        "                          , obi_warp_reference_sample        "
        "                          , obi_warp_show_advance_params     "
        "                          , obi_warp_score                   "
        "                          , obi_warp_response                "
        "                          , obi_warp_bin_size                "
        "                          , obi_warp_gap_init                "
        "                          , obi_warp_gap_extend              "
        "                          , obi_warp_factor_diag             "
        "                          , obi_warp_factor_gap              "
        "                          , obi_warp_no_standard_normal      "
        "                          , obi_warp_local                   "
        "                          , main_window_selected_db_name     "
        "                          , main_window_charge               "
        "                          , main_window_peak_quantitation    "
        "                          , main_window_mass_resolution      "
        "                          , must_have_fragmentation          "
        "                       FROM user_settings_old               ;"
        "DROP TABLE user_settings_old;"

        "ALTER TABLE user_settings ADD COLUMN search_adducts INTEGER;"
        "ALTER TABLE user_settings ADD COLUMN adduct_search_window REAL;"
        "ALTER TABLE user_settings ADD COLUMN adduct_percent_correlation REAL;"
        "COMMIT;"
    },
    {
        3,
        "BEGIN TRANSACTION;"
        "ALTER TABLE user_settings ADD COLUMN identification_match_rt  INTEGER;"
        "ALTER TABLE user_settings ADD COLUMN identification_rt_window REAL;"
        "COMMIT;"
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
        auto lastElement = appDbVersionMap.rbegin();
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
    if (bfs::exists(newPath))
        return false;

    try {
        bfs::copy_file(originalPath, newPath);
    } catch(boost::system::error_code) {
        cerr << "Error: failed to copy file" << endl;
        return false;
    }

    return true;
}

} // namespace ProjectVersioning
