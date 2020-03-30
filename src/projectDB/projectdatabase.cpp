#include <sstream>
#include <unordered_map>
#include <boost/filesystem.hpp>
#include "projectdatabase.h"
#include "Compound.h"
#include "connection.h"
#include "cursor.h"
#include "datastructures/adduct.h"
#include "mzMassCalculator.h"
#include "mzAligner.h"
#include "mzSample.h"
#include "projectversioning.h"
#include "Scan.h"
#include "schema.h"

#define BINT(x) boost::get<int>(x)
#define BFLOAT(x) boost::get<float>(x)
#define BDOUBLE(x) boost::get<double>(x)
#define BSTRING(x) boost::get<string>(x)

ProjectDatabase::ProjectDatabase(const string& dbFilename,
                                 const string& version)
{
    _connection = new Connection(dbFilename);

    // figure out whether this database needs upgrade
    using namespace ProjectVersioning;
    auto aheadBy = 0;
    auto versionInfo = extractVersionInfoFromTag(version, &aheadBy);
    auto appVersion = Version(versionInfo.first);
    auto requiredDbVersion = getDbVersionForApp(appVersion);
    auto currentDbVersion = this->version();

    // if build is ahead by non-zero number of commits, then its a dev build
    if (aheadBy > 0)
        requiredDbVersion = getLatestDbVersion();

    // if current version is greater than what is required, this is probably a
    // project created by a future release of the application and is, therefore,
    // unsafe read or write to this file.
    if (currentDbVersion > requiredDbVersion) {
        delete _connection;
        _connection = nullptr;
    } else {
        if (currentDbVersion != requiredDbVersion && !this->isEmpty()) {
            // upgrade needed, close existing connection
            delete _connection;

            string upgradeScript = generateUpgradeScript(currentDbVersion,
                                                         requiredDbVersion);
            cout << "Debug: Upgrading database from v"
                 << currentDbVersion
                 << " to v"
                 << requiredDbVersion
                 << endl;
            upgradeDatabase(dbFilename, upgradeScript, version);

            // upgrade complete, re-establish connection
            _connection = new Connection(dbFilename);
        }

        _setVersion(requiredDbVersion);
    }
}

ProjectDatabase::~ProjectDatabase()
{
    delete _connection;
}

void ProjectDatabase::saveSamples(const vector<mzSample*>& samples)
{
    if (!_connection->prepare(CREATE_SAMPLES_TABLE)->execute()) {
        cerr << "Error: failed to create samples table" << endl;
        return;
    }

    // assign IDs before proceeding with save operation
    _assignSampleIds(samples);

    auto samplesQuery = _connection->prepare(
        "REPLACE INTO samples         \
               VALUES ( :sample_id    \
                      , :name         \
                      , :filename     \
                      , :set_name     \
                      , :sample_order \
                      , :is_blank     \
                      , :is_selected  \
                      , :color_red    \
                      , :color_green  \
                      , :color_blue   \
                      , :color_alpha  \
                      , :norml_const  \
                      , :transform_a0 \
                      , :transform_a1 \
                      , :transform_a2 \
                      , :transform_a4 \
                      , :transform_a5 )");

    _connection->begin();

    for (auto s : samples) {
        samplesQuery->bind(":sample_id", s->getSampleId());
        samplesQuery->bind(":name", s->getSampleName());
        samplesQuery->bind(":filename", s->fileName);
        samplesQuery->bind(":set_name", s->getSetName());
        samplesQuery->bind(":sample_order", s->getSampleOrder());
        samplesQuery->bind(":is_blank", s->isBlank);
        samplesQuery->bind(":is_selected", s->isSelected);

        samplesQuery->bind(":color_red", s->color[0]);
        samplesQuery->bind(":color_green", s->color[1]);
        samplesQuery->bind(":color_blue", s->color[2]);
        samplesQuery->bind(":color_alpha", s->color[3]);

        samplesQuery->bind(":norml_const", s->getNormalizationConstant());

        samplesQuery->bind(":transform_a0", 0);
        samplesQuery->bind(":transform_a1", 0);
        samplesQuery->bind(":transform_a2", 0);
        samplesQuery->bind(":transform_a4", 0);
        samplesQuery->bind(":transform_a5", 0);

        if (!samplesQuery->execute()) {
            cerr << "Error: failed to save sample " << s->getSampleName()
                 << endl;
        }
    }
    _connection->commit();
}

void ProjectDatabase::saveGroups(const vector<PeakGroup*>& groups,
                                 const string& tableName)
{
    _connection->begin();

    for (const auto group : groups)
        saveGroupAndPeaks(group, 0, tableName);

    _connection->commit();
}

int ProjectDatabase::saveGroupAndPeaks(PeakGroup* group,
                                       const int parentGroupId,
                                       const string& tableName)
{
    if (!group)
        return -1;

    // skip deleted groups
    if (group->deletedFlag)
        return -1;

    if (!_connection->prepare(CREATE_PEAK_GROUPS_TABLE)->execute()) {
        cerr << "Error: failed to create peakgroups table" << endl;
        return -1;
    }

    auto groupsQuery = _connection->prepare(
        "INSERT INTO peakgroups                            \
              VALUES ( :group_id                           \
                     , :parent_group_id                    \
                     , :meta_group_id                      \
                     , :tag_string                         \
                     , :expected_mz                        \
                     , :expected_abundance                 \
                     , :expected_rt_diff                   \
                     , :group_rank                         \
                     , :label                              \
                     , :type                               \
                     , :srm_id                             \
                     , :ms2_event_count                    \
                     , :ms2_score                          \
                     , :adduct_name                        \
                     , :compound_id                        \
                     , :compound_name                      \
                     , :compound_db                        \
                     , :table_name                         \
                     , :min_quality                        \
                     , :fragmentation_fraction_matched     \
                     , :fragmentation_mz_frag_error        \
                     , :fragmentation_hypergeom_score      \
                     , :fragmentation_mvh_score            \
                     , :fragmentation_dot_product          \
                     , :fragmentation_weighted_dot_product \
                     , :fragmentation_spearman_rank_corr   \
                     , :fragmentation_tic_matched          \
                     , :fragmentation_num_matches          \
                     , :sample_ids                         \
                     , :slice_mz_min                       \
                     , :slice_mz_max                       \
                     , :slice_rt_min                       \
                     , :slice_rt_max                       \
                     , :slice_ion_count                    \
                     , :table_group_id                     )");

    groupsQuery->bind(":parent_group_id", parentGroupId);
    groupsQuery->bind(":table_group_id", group->groupId);
    groupsQuery->bind(":meta_group_id", group->metaGroupId);
    groupsQuery->bind(":tag_string", group->tagString);
    groupsQuery->bind(":expected_mz", group->expectedMz);
    groupsQuery->bind(":expected_rt_diff", group->expectedRtDiff()); // do we need this anymore?
    groupsQuery->bind(":expected_abundance", group->expectedAbundance);
    groupsQuery->bind(":group_rank", group->groupRank);
    groupsQuery->bind(":label", string(1, group->label));
    groupsQuery->bind(":type", static_cast<int>(group->type()));
    groupsQuery->bind(":srm_id", group->srmId);

    groupsQuery->bind(":ms2_event_count", group->ms2EventCount);
    groupsQuery->bind(":ms2_score", group->fragMatchScore.mergedScore);

    groupsQuery->bind(":fragmentation_fraction_matched",
                      group->fragMatchScore.fractionMatched);
    groupsQuery->bind(":fragmentation_mz_frag_error",
                      group->fragMatchScore.mzFragError);
    groupsQuery->bind(":fragmentation_hypergeom_score",
                      group->fragMatchScore.hypergeomScore);
    groupsQuery->bind(":fragmentation_mvh_score",
                      group->fragMatchScore.mvhScore);
    groupsQuery->bind(":fragmentation_dot_product",
                      group->fragMatchScore.dotProduct);
    groupsQuery->bind(":fragmentation_weighted_dot_product",
                      group->fragMatchScore.weightedDotProduct);
    groupsQuery->bind(":fragmentation_spearman_rank_corr",
                      group->fragMatchScore.spearmanRankCorrelation);
    groupsQuery->bind(":fragmentation_tic_matched",
                      group->fragMatchScore.ticMatched);
    groupsQuery->bind(":fragmentation_num_matches",
                      group->fragMatchScore.numMatches);

    groupsQuery->bind(":adduct_name", group->getAdduct()
                                          ? group->getAdduct()->getName() : "");

    groupsQuery->bind(":compound_id",
                      group->getCompound() ? group->getCompound()->id() : "");
    groupsQuery->bind(":compound_name",
                      group->getCompound() ? group->getCompound()->name() : "");
    groupsQuery->bind(":compound_db",
                      group->getCompound() ? group->getCompound()->db() : "");

    groupsQuery->bind(":table_name", tableName);
    groupsQuery->bind(":min_quality", group->minQuality);

    groupsQuery->bind(":slice_mz_min", group->getSlice().mzmin);
    groupsQuery->bind(":slice_mz_max", group->getSlice().mzmax);
    groupsQuery->bind(":slice_rt_min", group->getSlice().rtmin);
    groupsQuery->bind(":slice_rt_max", group->getSlice().rtmax);
    groupsQuery->bind(":slice_ion_count", group->getSlice().ionCount);

    string sample_ids = "";
    if (group->samples.size() > 0) {
        sample_ids = accumulate(next(begin(group->samples)),
                                end(group->samples),
                                to_string(group->samples[0]->getSampleId()),
                                [](string current, mzSample* s) {
                                    return move(current)
                                           + ';'
                                           + to_string(s->getSampleId());
                                });
    }
    groupsQuery->bind(":sample_ids", sample_ids);

    if (!groupsQuery->execute())
        cerr << "Error: failed to save peak group" << endl;

    int lastInsertedGroupId = _connection->lastInsertId();
    saveGroupPeaks(group, lastInsertedGroupId);

    for (auto child: group->children)
        saveGroupAndPeaks(&child, lastInsertedGroupId, tableName);

    return lastInsertedGroupId;
}

void ProjectDatabase::saveGroupPeaks(PeakGroup* group, const int databaseId)
{
    if (!_connection->prepare(CREATE_PEAKS_TABLE)->execute()) {
        cerr << "Error: failed to create peaks table" << endl;
        return;
    }

    auto peaksQuery = _connection->prepare(
        "INSERT INTO peaks                      \
              VALUES ( :peak_id                 \
                     , :group_id                \
                     , :sample_id               \
                     , :pos                     \
                     , :minpos                  \
                     , :maxpos                  \
                     , :rt                      \
                     , :rtmin                   \
                     , :rtmax                   \
                     , :mzmin                   \
                     , :mzmax                   \
                     , :scan                    \
                     , :minscan                 \
                     , :maxscan                 \
                     , :peak_area               \
                     , :peak_area_corrected     \
                     , :peak_area_top           \
                     , :peak_area_top_corrected \
                     , :peak_area_fractional    \
                     , :peak_rank               \
                     , :peak_intensity          \
                     , :peak_baseline_level     \
                     , :peak_mz                 \
                     , :median_mz               \
                     , :base_mz                 \
                     , :quality                 \
                     , :width                   \
                     , :gauss_fit_sigma         \
                     , :gauss_fit_r2            \
                     , :no_noise_obs            \
                     , :no_noise_fraction       \
                     , :symmetry                \
                     , :signal_baseline_ratio   \
                     , :group_overlap           \
                     , :group_overlap_frac      \
                     , :local_max_flag          \
                     , :from_blank_sample       \
                     , :label                   \
                     , :peak_spline_area        )");

    for (Peak p : group->peaks) {
        peaksQuery->bind(":group_id", databaseId);
        peaksQuery->bind(":sample_id", p.getSample()->getSampleId());
        peaksQuery->bind(":pos", static_cast<int>(p.pos));
        peaksQuery->bind(":minpos", static_cast<int>(p.minpos));
        peaksQuery->bind(":maxpos", static_cast<int>(p.maxpos));
        peaksQuery->bind(":rt", p.rt);
        peaksQuery->bind(":rtmin", p.rtmin);
        peaksQuery->bind(":rtmax", p.rtmax);
        peaksQuery->bind(":mzmin", p.mzmin);
        peaksQuery->bind(":mzmax", p.mzmax);
        peaksQuery->bind(":scan", static_cast<int>(p.scan));
        peaksQuery->bind(":minscan", static_cast<int>(p.minscan));
        peaksQuery->bind(":maxscan", static_cast<int>(p.maxscan));
        peaksQuery->bind(":peak_area", p.peakArea);
        peaksQuery->bind(":peak_spline_area", p.peakSplineArea);
        peaksQuery->bind(":peak_area_corrected", p.peakAreaCorrected);
        peaksQuery->bind(":peak_area_top", p.peakAreaTop);
        peaksQuery->bind(":peak_area_top_corrected", p.peakAreaTopCorrected);
        peaksQuery->bind(":peak_area_fractional", p.peakAreaFractional);
        peaksQuery->bind(":peak_rank", p.peakRank);
        peaksQuery->bind(":peak_intensity", p.peakIntensity);
        peaksQuery->bind(":peak_baseline_level", p.peakBaseLineLevel);
        peaksQuery->bind(":peak_mz", p.peakMz);
        peaksQuery->bind(":median_mz", p.medianMz);
        peaksQuery->bind(":base_mz", p.baseMz);
        peaksQuery->bind(":quality", p.quality);
        peaksQuery->bind(":width", static_cast<int>(p.width));
        peaksQuery->bind(":gauss_fit_sigma", p.gaussFitSigma);
        peaksQuery->bind(":gauss_fit_r2", p.gaussFitR2);
        peaksQuery->bind(":no_noise_obs", static_cast<int>(p.noNoiseObs));
        peaksQuery->bind(":no_noise_fraction", p.noNoiseFraction);
        peaksQuery->bind(":symmetry", p.symmetry);
        peaksQuery->bind(":signal_baseline_ratio", p.signalBaselineRatio);
        peaksQuery->bind(":group_overlap", p.groupOverlap);
        peaksQuery->bind(":group_overlap_frac", p.groupOverlapFrac);
        peaksQuery->bind(":local_max_flag", p.localMaxFlag);
        peaksQuery->bind(":from_blank_sample", p.fromBlankSample);
        peaksQuery->bind(":label", string(1, p.label));

        if (!peaksQuery->execute())
            cerr << "Error: failed to write peak" << endl;
    }
}

void ProjectDatabase::saveCompounds(const vector<PeakGroup>& groups)
{
    set<Compound*> seenCompounds;

    //find linked compounds (store only unique ones)
    for(auto group: groups) {
        if (group.getCompound())
            seenCompounds.insert(group.getCompound());
    }

    saveCompounds(seenCompounds);
}

void ProjectDatabase::saveCompounds(const set<Compound*>& seenCompounds)
{
    if (!_connection->prepare(CREATE_COMPOUNDS_TABLE)->execute()) {
        cerr << "Error: failed to create compounds table" << endl;
        return;
    }

    if (!_connection->prepare(CREATE_COMPOUNDS_DB_INDEX)->execute())
        cerr << "Warning: failed to create index on compounds table" << endl;

    auto compoundsQuery = _connection->prepare(
        "REPLACE INTO compounds                \
               VALUES ( :compound_id           \
                      , :db_name               \
                      , :name                  \
                      , :formula               \
                      , :smile_string          \
                      , :srm_id                \
                      , :mass                  \
                      , :charge                \
                      , :expected_rt           \
                      , :precursor_mz          \
                      , :product_mz            \
                      , :collision_energy      \
                      , :log_p                 \
                      , :virtual_fragmentation \
                      , :ionization_mode       \
                      , :category              \
                      , :fragment_mzs          \
                      , :fragment_intensity    \
                      , :fragment_ion_types    \
                      , :note                  )");

    _connection->begin();

    for (Compound* c : seenCompounds) {
        stringstream categories;
        auto category = c->category();
        for (string s : category) {
            categories << s.c_str() << ";";
        }
        string catStr = categories.str();
        catStr = catStr.substr(0, catStr.size() - 1);

        stringstream fragMz;
        stringstream fragIntensity;
        stringstream fragIonType;
        size_t numFragments = c->fragmentMzValues().size();
        if (numFragments != 0
            && (numFragments == c->fragmentIntensities().size())
            && (numFragments == c->fragmentIonTypes().size())) {
            auto fragmentMzValues = c->fragmentMzValues();
            auto fragmentIonTypes = c->fragmentIonTypes();
            auto fragmentIntensities = c->fragmentIntensities();
            for (size_t i = 0; i < numFragments - 1; ++i) {
                // presumption: all three containers are of the same size
                auto mz = fragmentMzValues[i];
                auto intensity = fragmentIntensities[i];
                auto ionType = fragmentIonTypes[i];

                fragMz << fixed << setprecision(10) << mz << ";";
                fragIntensity << fixed << setprecision(10) << intensity << ";";
                fragIonType << ionType << ";";
            }
            fragMz << fixed
                   << setprecision(10)
                   << fragmentMzValues.back();
            fragIntensity << fixed
                          << setprecision(10)
                          << fragmentIntensities.back();
            fragIonType << fragmentIonTypes.rbegin()->second;
        }

        compoundsQuery->bind(":compound_id", c->id());
        compoundsQuery->bind(":db_name", c->db());
        compoundsQuery->bind(":name", c->name());
        compoundsQuery->bind(":formula", c->formula());
        compoundsQuery->bind(":smile_string", c->smileString());
        compoundsQuery->bind(":srm_id", c->srmId());

        compoundsQuery->bind(":mass", c->mz());
        compoundsQuery->bind(":charge", c->charge());
        compoundsQuery->bind(":expected_rt", c->expectedRt());
        compoundsQuery->bind(":precursor_mz", c->precursorMz());
        compoundsQuery->bind(":product_mz", c->productMz());

        compoundsQuery->bind(":collision_energy", c->collisionEnergy());
        compoundsQuery->bind(":log_p", c->logP());
        compoundsQuery->bind(":virtual_fragmentation", c->virtualFragmentation());

        int ionizationMode;
        if(c->ionizationMode == Compound::IonizationMode::Positive)
            ionizationMode = +1;
        else if(c->ionizationMode == Compound::IonizationMode::Negative)
            ionizationMode = -1;
        else
            ionizationMode = 0;
        compoundsQuery->bind(":ionization_mode", ionizationMode);

        compoundsQuery->bind(":category", catStr);
        compoundsQuery->bind(":fragment_mzs", fragMz.str());
        compoundsQuery->bind(":fragment_intensity", fragIntensity.str());
        compoundsQuery->bind(":fragment_ion_types", fragIonType.str());

        compoundsQuery->bind(":note", c->note());
        if (!compoundsQuery->execute())
            cerr << "Error: failed to save compound " << c->name() << endl;
    }

    _connection->commit();
}

void ProjectDatabase::saveAlignment(const vector<mzSample*>& samples)
{
    deleteAllAlignmentData();

    if (!_connection->prepare(CREATE_ALIGNMENT_TABLE)->execute())
        cerr << "Error: failed to create alignment table" << endl;

    // save new alignment
    auto alignmentQuery = _connection->prepare(
        "INSERT INTO alignment_rts  \
              VALUES ( :sample_id   \
                     , :scannum    \
                     , :rt_original \
                     , :rt_updated  )");

    _connection->begin();

    for (auto s : samples) {
        // ignore samples having MS2 scans
        if (s->ms1ScanCount() == 0)
            continue;

        for (int i = 0; i < s->scans.size(); i++) {
            // save rt for every 200th scan (and last scan)
            if (i % 200 == 0 || i == s->scans.size() - 1) {
                auto scan = s->scans[i];
                float rt_updated = scan->rt;
                float rt_original = scan->originalRt;
                alignmentQuery->bind(":sample_id", s->getSampleId());
                alignmentQuery->bind(":scannum", -1);
                alignmentQuery->bind(":rt_original", rt_original);
                alignmentQuery->bind(":rt_updated", rt_updated);
                if (!alignmentQuery->execute())
                    cerr << "Error: failed to write alignment data" << endl;
            }
        }
    }

    _connection->commit();
}

void ProjectDatabase::saveScans(const vector<mzSample*>& sampleSet)
{
    deleteAllScans();

    if(!_connection->prepare(CREATE_SCANS_TABLE)->execute()) {
        cerr << "Error: failed to create scans table" << endl;
        return;
    }

    auto scansQuery = _connection->prepare(
        "INSERT INTO scans               \
              VALUES ( :sample_id        \
                     , :scan             \
                     , :file_seek_start  \
                     , :file_seek_end    \
                     , :mslevel          \
                     , :rt               \
                     , :precursor_mz     \
                     , :precursor_charge \
                     , :precursor_ic     \
                     , :precursor_purity \
                     , :minmz            \
                     , :maxmz            \
                     , :data             )");

    _connection->begin();

    float ppm = 20;
    for (auto s : sampleSet) {
        for (auto scan : s->scans) {
            if (scan->mslevel == 1)
                continue;

            string scanData = _getScanSignature(scan, 2000);

            scansQuery->bind(":sample_id", s->getSampleId());
            scansQuery->bind(":scan", scan->scannum);
            scansQuery->bind(":file_seek_start", -1);
            scansQuery->bind(":file_seek_end", -1);
            scansQuery->bind(":mslevel", scan->mslevel);
            scansQuery->bind(":rt", scan->rt);
            scansQuery->bind(":precursor_mz", scan->precursorMz);
            scansQuery->bind(":precursor_charge", scan->precursorCharge);
            scansQuery->bind(":precursor_ic", scan->totalIntensity());
            scansQuery->bind(":precursor_purity", scan->getPrecursorPurity(ppm));
            scansQuery->bind(":minmz", scan->minMz());
            scansQuery->bind(":maxmz", scan->maxMz());
            scansQuery->bind(":data", scanData.c_str());

            if (!scansQuery->execute())
                cerr << "Error: failed to save scan" << endl;
        }
    }

    _connection->commit();
}

void ProjectDatabase::saveSettings(const map<string, variant>& settingsMap)
{
    if (!_connection->prepare(CREATE_SETTINGS_TABLE)->execute()) {
        cerr << "Error: failed to create settings table" << endl;
        return;
    }

    auto settingsQuery = _connection->prepare(
        "REPLACE INTO user_settings                       \
               VALUES ( :ionization_mode                  \
                      , :ionization_type                  \
                      , :instrument_type                  \
                      , :q1_accuracy                      \
                      , :q3_accuracy                      \
                      , :filterline                       \
                      , :centroid_scans                   \
                      , :scan_filter_polarity             \
                      , :scan_filter_ms_level             \
                      , :scan_filter_min_quantile         \
                      , :scan_filter_min_intensity        \
                      , :upload_multiprocessing           \
                      , :eic_smoothing_algorithm          \
                      , :eic_smoothing_window             \
                      , :max_rt_difference_bw_peaks       \
                      , :asls_baseline_mode               \
                      , :baseline_quantile                \
                      , :baseline_smoothing_window        \
                      , :asls_smoothness                  \
                      , :asls_asymmetry                   \
                      , :isotope_filter_equal_peak        \
                      , :min_signal_baseline_diff         \
                      , :min_peak_quality                 \
                      , :isotope_min_signal_baseline_diff \
                      , :isotope_min_peak_quality         \
                      , :d2_label_bpe                     \
                      , :c13_label_bpe                    \
                      , :n15_label_bpe                    \
                      , :s34_label_bpe                    \
                      , :min_isotope_parent_correlation   \
                      , :max_isotope_scan_diff            \
                      , :abundance_threshold              \
                      , :max_natural_abundance_error      \
                      , :correct_c13_isotope_abundance    \
                      , :eic_type                         \
                      , :use_overlap                      \
                      , :dist_x_weight                    \
                      , :dist_y_weight                    \
                      , :overlap_weight                   \
                      , :consider_delta_rt                \
                      , :quality_weight                   \
                      , :intensity_weight                 \
                      , :delta_rt_weight                  \
                      , :mass_cutoff_type                 \
                      , :automated_detection              \
                      , :mass_domain_resolution           \
                      , :time_domain_resolution           \
                      , :min_mz                           \
                      , :max_mz                           \
                      , :min_rt                           \
                      , :max_rt                           \
                      , :min_intensity                    \
                      , :max_intensity                    \
                      , :database_search                  \
                      , :compound_extraction_window       \
                      , :match_rt                         \
                      , :compound_rt_window               \
                      , :limit_groups_per_compound        \
                      , :match_fragmentation              \
                      , :min_frag_match_score             \
                      , :fragment_tolerance               \
                      , :min_frag_match                   \
                      , :report_isotopes                  \
                      , :peak_quantitation                \
                      , :min_group_intensity              \
                      , :intensity_quantile               \
                      , :min_group_quality                \
                      , :quality_quantile                 \
                      , :min_signal_blank_ratio           \
                      , :signal_blank_ratio_quantile      \
                      , :min_signal_baseline_ratio        \
                      , :signal_baseline_ratio_quantile   \
                      , :min_peak_width                   \
                      , :min_good_peak_count              \
                      , :peak_classifier_file             \
                      , :alignment_good_peak_count        \
                      , :alignment_limit_group_count      \
                      , :alignment_peak_grouping_window   \
                      , :alignemnt_wrt_expected_rt        \
                      , :alignment_min_peak_intensity     \
                      , :alignment_min_signal_noise_ratio \
                      , :alignment_min_peak_width         \
                      , :alignment_peak_detection         \
                      , :poly_fit_num_iterations          \
                      , :poly_fit_polynomial_degree       \
                      , :obi_warp_reference_sample        \
                      , :obi_warp_show_advance_params     \
                      , :obi_warp_score                   \
                      , :obi_warp_response                \
                      , :obi_warp_bin_size                \
                      , :obi_warp_gap_init                \
                      , :obi_warp_gap_extend              \
                      , :obi_warp_factor_diag             \
                      , :obi_warp_factor_gap              \
                      , :obi_warp_no_standard_normal      \
                      , :obi_warp_local                   \
                      , :main_window_selected_db_name     \
                      , :main_window_charge               \
                      , :main_window_peak_quantitation    \
                      , :main_window_mass_resolution      \
                      , :must_have_fragmentation          \
                      , :identification_match_rt          \
                      , :identification_rt_window         \
                      , :search_adducts                   \
                      , :adduct_search_window             \
                      , :adduct_percent_correlation       )");

    settingsQuery->bind(":ionization_mode", BINT(settingsMap.at("ionizationMode")));
    settingsQuery->bind(":ionization_type", BINT(settingsMap.at("ionizationType")));
    settingsQuery->bind(":instrument_type", BINT(settingsMap.at("instrumentType")));
    settingsQuery->bind(":q1_accuracy", BDOUBLE(settingsMap.at("q1Accuracy")));
    settingsQuery->bind(":q3_accuracy", BDOUBLE(settingsMap.at("q3Accuracy")));
    settingsQuery->bind(":filterline", BINT(settingsMap.at("filterline")));

    settingsQuery->bind(":centroid_scans", BINT(settingsMap.at("centroidScans")));
    settingsQuery->bind(":scan_filter_polarity", BINT(settingsMap.at("scanFilterPolarity")));
    settingsQuery->bind(":scan_filter_ms_level", BINT(settingsMap.at("scanFilterMsLevel")));
    settingsQuery->bind(":scan_filter_min_quantile", BINT(settingsMap.at("scanFilterMinQuantile")));
    settingsQuery->bind(":scan_filter_min_intensity", BINT(settingsMap.at("scanFilterMinIntensity")));
    settingsQuery->bind(":upload_multiprocessing", BINT(settingsMap.at("uploadMultiprocessing")));

    settingsQuery->bind(":eic_smoothing_algorithm", BINT(settingsMap.at("eicSmoothingAlgorithm")));
    settingsQuery->bind(":eic_smoothing_window", BINT(settingsMap.at("eicSmoothingWindow")));
    settingsQuery->bind(":max_rt_difference_bw_peaks", BDOUBLE(settingsMap.at("maxRtDiffBetweenPeaks")));

    settingsQuery->bind(":asls_baseline_mode", BINT(settingsMap.at("aslsBaselineMode")));
    settingsQuery->bind(":baseline_quantile", BINT(settingsMap.at("baselineQuantile")));
    settingsQuery->bind(":baseline_smoothing_window", BINT(settingsMap.at("baselineSmoothing")));
    settingsQuery->bind(":asls_smoothness", BINT(settingsMap.at("aslsSmoothness")));
    settingsQuery->bind(":asls_asymmetry", BINT(settingsMap.at("aslsAsymmetry")));

    settingsQuery->bind(":isotope_filter_equal_peak", BINT(settingsMap.at("isotopeFilterEqualPeak")));
    settingsQuery->bind(":min_signal_baseline_diff", BDOUBLE(settingsMap.at("minSignalBaselineDifference")));
    settingsQuery->bind(":min_peak_quality", BDOUBLE(settingsMap.at("minPeakQuality")));
    settingsQuery->bind(":isotope_min_signal_baseline_diff", BDOUBLE(settingsMap.at("isotopeMinSignalBaselineDifference")));
    settingsQuery->bind(":isotope_min_peak_quality", BDOUBLE(settingsMap.at("isotopeMinPeakQuality")));

    settingsQuery->bind(":d2_label_bpe", BINT(settingsMap.at("D2LabelBPE")));
    settingsQuery->bind(":c13_label_bpe", BINT(settingsMap.at("C13LabelBPE")));
    settingsQuery->bind(":n15_label_bpe", BINT(settingsMap.at("N15LabelBPE")));
    settingsQuery->bind(":s34_label_bpe", BINT(settingsMap.at("S34LabelBPE")));

    settingsQuery->bind(":min_isotope_parent_correlation", BDOUBLE(settingsMap.at("minIsotopeParentCorrelation")));
    settingsQuery->bind(":max_isotope_scan_diff", BINT(settingsMap.at("maxIsotopeScanDiff")));
    settingsQuery->bind(":abundance_threshold", BDOUBLE(settingsMap.at("abundanceThreshold")));
    settingsQuery->bind(":max_natural_abundance_error", BDOUBLE(settingsMap.at("maxNaturalAbundanceError")));
    settingsQuery->bind(":correct_c13_isotope_abundance", BINT(settingsMap.at("correctC13IsotopeAbundance")));

    settingsQuery->bind(":eic_type", BINT(settingsMap.at("eicType")));

    settingsQuery->bind(":use_overlap", BINT(settingsMap.at("useOverlap")));
    settingsQuery->bind(":dist_x_weight", BINT(settingsMap.at("distXWeight")));
    settingsQuery->bind(":dist_y_weight", BINT(settingsMap.at("distYWeight")));
    settingsQuery->bind(":overlap_weight", BINT(settingsMap.at("overlapWeight")));

    settingsQuery->bind(":consider_delta_rt", BINT(settingsMap.at("considerDeltaRT")));
    settingsQuery->bind(":quality_weight", BINT(settingsMap.at("qualityWeight")));
    settingsQuery->bind(":intensity_weight", BINT(settingsMap.at("intensityWeight")));
    settingsQuery->bind(":delta_rt_weight", BINT(settingsMap.at("deltaRTWeight")));

    settingsQuery->bind(":mass_cutoff_type", BSTRING(settingsMap.at("massCutoffType")));

    settingsQuery->bind(":automated_detection", BINT(settingsMap.at("automatedDetection")));
    settingsQuery->bind(":mass_domain_resolution", BDOUBLE(settingsMap.at("massDomainResolution")));
    settingsQuery->bind(":time_domain_resolution", BDOUBLE(settingsMap.at("timeDomainResolution")));
    settingsQuery->bind(":min_mz", BDOUBLE(settingsMap.at("minMz")));
    settingsQuery->bind(":max_mz", BDOUBLE(settingsMap.at("maxMz")));
    settingsQuery->bind(":min_rt", BDOUBLE(settingsMap.at("minRt")));
    settingsQuery->bind(":max_rt", BDOUBLE(settingsMap.at("maxRt")));
    settingsQuery->bind(":min_intensity", BDOUBLE(settingsMap.at("minIntensity")));
    settingsQuery->bind(":max_intensity", BDOUBLE(settingsMap.at("maxIntensity")));
    settingsQuery->bind(":must_have_fragmentation", BINT(settingsMap.at("mustHaveFragmentation")));
    settingsQuery->bind(":identification_match_rt", BINT(settingsMap.at("identificationMatchRt")));
    settingsQuery->bind(":identification_rt_window", BDOUBLE(settingsMap.at("identificationRtWindow")));

    settingsQuery->bind(":database_search", BINT(settingsMap.at("databaseSearch")));
    settingsQuery->bind(":compound_extraction_window", BDOUBLE(settingsMap.at("compoundExtractionWindow")));
    settingsQuery->bind(":match_rt", BINT(settingsMap.at("matchRt")));
    settingsQuery->bind(":compound_rt_window", BDOUBLE(settingsMap.at("compoundRtWindow")));
    settingsQuery->bind(":limit_groups_per_compound", BINT(settingsMap.at("limitGroupsPerCompound")));
    settingsQuery->bind(":search_adducts", BINT(settingsMap.at("searchAdducts")));
    settingsQuery->bind(":adduct_search_window", BDOUBLE(settingsMap.at("adductSearchWindow")));
    settingsQuery->bind(":adduct_percent_correlation", BDOUBLE(settingsMap.at("adductPercentCorrelation")));

    settingsQuery->bind(":match_fragmentation", BINT(settingsMap.at("matchFragmentation")));
    settingsQuery->bind(":min_frag_match_score", BDOUBLE(settingsMap.at("minFragMatchScore")));
    settingsQuery->bind(":fragment_tolerance", BDOUBLE(settingsMap.at("fragmentTolerance")));
    settingsQuery->bind(":min_frag_match", BDOUBLE(settingsMap.at("minFragMatch")));

    settingsQuery->bind(":report_isotopes", BINT(settingsMap.at("reportIsotopes")));

    settingsQuery->bind(":peak_quantitation", BINT(settingsMap.at("peakQuantitation")));
    settingsQuery->bind(":min_group_intensity", BDOUBLE(settingsMap.at("minGroupIntensity")));
    settingsQuery->bind(":intensity_quantile", BINT(settingsMap.at("intensityQuantile")));
    settingsQuery->bind(":min_group_quality", BDOUBLE(settingsMap.at("minGroupQuality")));
    settingsQuery->bind(":quality_quantile", BINT(settingsMap.at("qualityQuantile")));
    settingsQuery->bind(":min_signal_blank_ratio", BDOUBLE(settingsMap.at("minSignalBlankRatio")));
    settingsQuery->bind(":signal_blank_ratio_quantile", BINT(settingsMap.at("signalBlankRatioQuantile")));
    settingsQuery->bind(":min_signal_baseline_ratio", BINT(settingsMap.at("minSignalBaselineRatio")));
    settingsQuery->bind(":signal_baseline_ratio_quantile", BINT(settingsMap.at("signalBaselineRatioQuantile")));
    settingsQuery->bind(":min_peak_width", BINT(settingsMap.at("minPeakWidth")));
    settingsQuery->bind(":min_good_peak_count", BINT(settingsMap.at("minGoodPeakCount")));
    settingsQuery->bind(":peak_classifier_file", BSTRING(settingsMap.at("peakClassifierFile")));

    settingsQuery->bind(":main_window_selected_db_name", BSTRING(settingsMap.at("mainWindowSelectedDbName")));
    settingsQuery->bind(":main_window_charge", BINT(settingsMap.at("mainWindowCharge")));
    settingsQuery->bind(":main_window_peak_quantitation", BINT(settingsMap.at("mainWindowPeakQuantitation")));
    settingsQuery->bind(":main_window_mass_resolution", BDOUBLE(settingsMap.at("mainWindowMassResolution")));

    if (!settingsQuery->execute()) {
        cerr << "Error: failed to save user settings." << endl;
    }
}

pair<vector<string>, vector<string>>
ProjectDatabase::getSampleNames(const vector<mzSample*> loaded)
{
    string projectPath = this->projectPath();
    auto samplesQuery = _connection->prepare("SELECT filename \
                                                FROM samples  ");

    vector<string> sampleNamesFound;
    vector<string> sampleNamesNotFound;
    while (samplesQuery->next()) {
        string filename = samplesQuery->stringValue("filename");

        // skip files that have been loaded already
        bool isLoaded = false;
        for (auto const loadedSample : loaded) {
            if (loadedSample->fileName == filename) {
                isLoaded = true;
                break;
            }
        }
        if (isLoaded)
            continue;

        vector<string> possiblePaths;
        possiblePaths.push_back(projectPath);
        possiblePaths.push_back("./");
        possiblePaths.push_back("../");
        string filepath = _locateSample(filename, possiblePaths);

        if (!filepath.empty()) {
            sampleNamesFound.push_back(filepath);
            cerr << "Debug: Found sample: " << filepath << endl;
        } else {
            sampleNamesNotFound.push_back(filename);
            cerr << "Error: Could not find sample: " << filename << endl;
        }
    }
    pair<vector<string>, vector<string>> sampleNames(sampleNamesFound,
                                                     sampleNamesNotFound);
    return sampleNames;
}

void ProjectDatabase::updateSamples(const vector<mzSample*> freshlyLoaded)
{
    string projectPath = this->projectPath();
    auto samplesQuery = _connection->prepare("SELECT *      \
                                                FROM samples");

    while (samplesQuery->next()) {
        string sampleName = samplesQuery->stringValue("name");
        string setName = samplesQuery->stringValue("set_name");
        int sampleId = samplesQuery->integerValue("sample_id");

        int sampleOrder = samplesQuery->integerValue("sample_order");
        int isBlank = samplesQuery->integerValue("is_blank");
        int isSelected = samplesQuery->integerValue("is_selected");
        float color_red = samplesQuery->floatValue("color_red");
        float color_blue = samplesQuery->floatValue("color_blue");
        float color_green = samplesQuery->floatValue("color_green");
        float color_alpha = samplesQuery->floatValue("color_alpha");
        float norml_const = samplesQuery->floatValue("norml_const");

        if (norml_const == 0.0f)
            norml_const = 1.0f;

        // find current sample based on its sample name
        mzSample* sample = nullptr;
        for (auto const loadedSample : freshlyLoaded) {
            if (loadedSample->sampleName == sampleName) {
                sample = loadedSample;
                break;
            }
        }
        if (sample) {
            sample->setSampleId(sampleId);
            sample->setSetName(setName);
            sample->setSampleOrder(sampleOrder);
            sample->isBlank = static_cast<bool>(isBlank);
            sample->isSelected = static_cast<bool>(isSelected);
            sample->color[0] = color_red;
            sample->color[1] = color_green;
            sample->color[2] = color_blue;
            sample->color[3] = color_alpha;
            sample->setNormalizationConstant(norml_const);
        }
    }
}

vector<PeakGroup*> ProjectDatabase::loadGroups(const vector<mzSample*>& loaded)
{
    _connection->prepare(CREATE_PEAKS_GROUP_INDEX)->execute();
    auto groupsQuery = _connection->prepare("SELECT *         \
                                               FROM peakgroups");

    vector<PeakGroup*> groups;
    map<int, PeakGroup*> databaseIdForGroups;
    map<PeakGroup*, int> childParentMap;
    while (groupsQuery->next()) {
        PeakGroup* group = new PeakGroup();
        int databaseId = groupsQuery->integerValue("group_id");
        group->groupId = groupsQuery->integerValue("table_group_id");
        int parentGroupId = groupsQuery->integerValue("parent_group_id");
        group->tagString = groupsQuery->stringValue("tag_string");
        group->metaGroupId = groupsQuery->integerValue("meta_group_id");
        group->expectedMz = groupsQuery->floatValue("expected_mz");
        group->expectedAbundance =
            groupsQuery->floatValue("expected_abundance");
        group->groupRank = groupsQuery->floatValue("group_rank");
        group->label = groupsQuery->stringValue("label")[0];
        group->ms2EventCount = groupsQuery->integerValue("ms2_event_count");
        group->fragMatchScore.mergedScore =
            groupsQuery->doubleValue("ms2_score");
        group->fragMatchScore.fractionMatched =
            groupsQuery->doubleValue("fragmentation_fraction_matched");
        group->fragMatchScore.mzFragError =
            groupsQuery->doubleValue("fragmentation_mz_frag_error");
        group->fragMatchScore.hypergeomScore =
            groupsQuery->doubleValue("fragmentation_hypergeom_score");
        group->fragMatchScore.mvhScore =
            groupsQuery->doubleValue("fragmentation_mvh_score");
        group->fragMatchScore.dotProduct =
            groupsQuery->doubleValue("fragmentation_dot_product");
        group->fragMatchScore.weightedDotProduct =
            groupsQuery->doubleValue("fragmentation_weighted_dot_product");
        group->fragMatchScore.spearmanRankCorrelation =
            groupsQuery->doubleValue("fragmentation_spearman_rank_corr");
        group->fragMatchScore.ticMatched =
            groupsQuery->doubleValue("fragmentation_tic_matched");
        group->fragMatchScore.numMatches =
            groupsQuery->doubleValue("fragmentation_num_matches");

        group->setType(static_cast<PeakGroup::GroupType>(groupsQuery->integerValue("type")));
        group->setTableName(groupsQuery->stringValue("table_name"));
        group->minQuality = groupsQuery->doubleValue("min_quality");

        string compoundId = groupsQuery->stringValue("compound_id");
        string compoundDB = groupsQuery->stringValue("compound_db");
        string compoundName = groupsQuery->stringValue("compound_name");
        string adductName = groupsQuery->stringValue("adduct_name");

        string srmId = groupsQuery->stringValue("srm_id");
        if (!srmId.empty())
            group->setSrmId(srmId);

        if (!adductName.empty()) {
            group->setAdduct(_findAdductByName(adductName));
        } else {
            group->setAdduct(nullptr);
        }

        if (!compoundId.empty()) {
            Compound* compound = _findSpeciesByIdAndName(compoundId,
                                                         compoundName,
                                                         compoundDB);
            if (compound)
                group->setCompound(compound);

        } else if (!compoundName.empty() && !compoundDB.empty()) {
            vector<Compound*> matches = _findSpeciesByName(compoundName,
                                                           compoundDB);
            if (matches.size() > 0)
                group->setCompound(matches[0]);
        }

        vector<string> sample_ids;
        mzUtils::split(groupsQuery->stringValue("sample_ids"), ';', sample_ids);
        for (auto idString : sample_ids) {
            if (idString.empty())
                continue;

            int sampleId = stoi(idString);
            auto sampleIter = find_if(begin(loaded),
                                      end(loaded),
                                      [sampleId](mzSample* s) {
                                          return sampleId == s->getSampleId();
                                      });
            if (sampleIter != end(loaded)) {
                group->samples.push_back(*sampleIter);
            }
        }

        float sliceMzMin = groupsQuery->doubleValue("slice_mz_min");
        float sliceMzMax = groupsQuery->doubleValue("slice_mz_max");
        float sliceRtMin = groupsQuery->doubleValue("slice_rt_min");
        float sliceRtMax = groupsQuery->doubleValue("slice_rt_max");
        float sliceIonCount = groupsQuery->doubleValue("slice_ion_count");
        mzSlice slice(sliceMzMin, sliceMzMax, sliceRtMin, sliceRtMax);
        slice.ionCount = sliceIonCount;
        slice.srmId = group->srmId;
        slice.compound = group->getCompound();
        group->setSlice(slice);

        loadGroupPeaks(group, databaseId, loaded);
        group->groupStatistics();

        if (parentGroupId == 0) {
            groups.push_back(group);
        } else {
            childParentMap[group] = parentGroupId;
        }
        databaseIdForGroups[databaseId] = group;
    }

    // assign parents for child groups
    for (auto pair : childParentMap) {
        bool foundParent = false;
        auto child = pair.first;
        for (auto idGroupPair : databaseIdForGroups) {
            if (idGroupPair.first == pair.second) {
                idGroupPair.second->addChild(*child);
                foundParent = true;
                break;
            }
        }
        // failed to find a parent group, become a parent
        if (!foundParent)
            groups.push_back(child);
    }


    cerr << "Debug: Read in " << groups.size() << " groups" << endl;
    return groups;
}

void ProjectDatabase::loadGroupPeaks(PeakGroup* parentGroup,
                                     int databaseId,
                                     const vector<mzSample*>& loaded)
{
    auto peaksQuery = _connection->prepare(
                "SELECT peaks.*                             \
                      , samples.name AS sample_name         \
                   FROM peaks                               \
                      , samples                             \
                  WHERE peaks.sample_id = samples.sample_id \
                    AND peaks.group_id = :parent_group_id   ");
    peaksQuery->bind(":parent_group_id", databaseId);

    while (peaksQuery->next()) {
        Peak peak;
        peak.pos = static_cast<unsigned int>(peaksQuery->integerValue("pos"));
        peak.minpos =
            static_cast<unsigned int>(peaksQuery->integerValue("minpos"));
        peak.maxpos =
            static_cast<unsigned int>(peaksQuery->integerValue("maxpos"));
        peak.rt = peaksQuery->floatValue("rt");
        peak.rtmin = peaksQuery->floatValue("rtmin");
        peak.rtmax = peaksQuery->floatValue("rtmax");
        peak.mzmin = peaksQuery->floatValue("mzmin");
        peak.mzmax = peaksQuery->floatValue("mzmax");
        peak.scan = static_cast<unsigned int>(peaksQuery->integerValue("scan"));
        peak.minscan =
            static_cast<unsigned int>(peaksQuery->integerValue("minscan"));
        peak.maxscan =
            static_cast<unsigned int>(peaksQuery->integerValue("maxscan"));
        peak.peakArea = peaksQuery->floatValue("peak_area");
        peak.peakSplineArea = peaksQuery->floatValue("peak_spline_area");
        peak.peakAreaCorrected = peaksQuery->floatValue("peak_area_corrected");
        peak.peakAreaTop = peaksQuery->floatValue("peak_area_top");
        peak.peakAreaTopCorrected =
            peaksQuery->floatValue("peak_area_top_corrected");
        peak.peakAreaFractional =
            peaksQuery->floatValue("peak_area_fractional");
        peak.peakRank = peaksQuery->floatValue("peak_rank");
        peak.peakIntensity = peaksQuery->floatValue("peak_intensity");
        peak.peakBaseLineLevel = peaksQuery->floatValue("peak_baseline_level");
        peak.peakMz = peaksQuery->floatValue("peak_mz");
        peak.medianMz = peaksQuery->floatValue("median_mz");
        peak.baseMz = peaksQuery->floatValue("base_mz");
        peak.quality = peaksQuery->floatValue("quality");
        peak.width =
            static_cast<unsigned int>(peaksQuery->integerValue("width"));
        peak.gaussFitSigma = peaksQuery->floatValue("gauss_fit_sigma");
        peak.gaussFitR2 = peaksQuery->floatValue("gauss_fit_r2");
        peak.noNoiseObs =
            static_cast<unsigned int>(peaksQuery->integerValue("no_noise_obs"));
        peak.noNoiseFraction = peaksQuery->floatValue("no_noise_fraction");
        peak.symmetry = peaksQuery->floatValue("symmetry");
        peak.signalBaselineRatio =
            peaksQuery->floatValue("signal_baseline_ratio");
        peak.groupOverlap = peaksQuery->floatValue("group_overlap");
        peak.groupOverlapFrac = peaksQuery->floatValue("group_overlap_frac");
        peak.localMaxFlag = peaksQuery->integerValue("local_max_flag");
        peak.fromBlankSample = peaksQuery->integerValue("from_blank_sample");
        peak.label = peaksQuery->stringValue("label")[0];

        string sampleName = peaksQuery->stringValue("sample_name");

        for (auto sample : loaded) {
            if (sample->sampleName == sampleName) {
                peak.setSample(sample);
                break;
            }
        }
        parentGroup->addPeak(peak);
    }
}

vector<Compound*> ProjectDatabase::loadCompounds(const string databaseName)
{
    vector<Compound*> compounds;
    if (!databaseName.empty() && _compoundDatabaseLoaded(databaseName)) {
        cerr << "Debug: already loaded database " << databaseName << endl;
        return compounds;
    }

    string selectStatement = "SELECT *                      \
                                FROM compounds              ";
    if (!databaseName.empty())
        selectStatement += "   WHERE db_name= :database_name";

    auto compoundsQuery = _connection->prepare(selectStatement);
    compoundsQuery->bind(":database_name", databaseName);

    MassCalculator mcalc;
    int loadCount = 0;
    while (compoundsQuery->next()) {
        string id = compoundsQuery->stringValue("compound_id");
        string name = compoundsQuery->stringValue("name");
        string formula = compoundsQuery->stringValue("formula");
        int charge = compoundsQuery->integerValue("charge");
        float mass = compoundsQuery->floatValue("mass");
        string db = compoundsQuery->stringValue("db_name");
        float expectedRt = compoundsQuery->floatValue("expected_rt");

        // skip if compound already exists in internal database
        if (_compoundIdMap.find(id + name + db) != end(_compoundIdMap))
            continue;

        // the neutral mass is computed automatically inside the constructor
        Compound* compound = new Compound(id, name, formula, charge);
        compound->setDb (db);
        compound->setExpectedRt( expectedRt);

        if (formula.empty()) {
            if (mass > 0)
                compound->setMz( mass);
        } else {
            compound->setMz
                    (static_cast<float>(mcalc.computeNeutralMass(formula)));
        }

        compound->setPrecursorMz ( compoundsQuery->floatValue("precursor_mz"));
        compound->setProductMz (compoundsQuery->floatValue("product_mz"));
        compound->setCollisionEnergy
                (compoundsQuery->floatValue("collision_energy"));
        compound->setSmileString (compoundsQuery->stringValue("smile_string"));
        compound->setLogP (compoundsQuery->floatValue("log_p"));

        int ionizationMode;
        ionizationMode = compoundsQuery->floatValue("ionization_mode");
        if (ionizationMode > 0)
            compound->ionizationMode = Compound::IonizationMode::Positive;
        else if (ionizationMode < 0)
            compound->ionizationMode = Compound::IonizationMode::Negative;
        else
            compound->ionizationMode = Compound::IonizationMode::Neutral;

        compound->setNote (compoundsQuery->stringValue("note"));

        // mark compound as decoy if names contains DECOY string
        if (compound->name().find("DECOY") != string::npos)
            compound->setIsDecoy (true);

        // lambda function to split a string using given delimiters
        auto split = [](string str, const char delim) {
            vector<string> separated;
            stringstream ss(str);
            string item;
            while(getline(ss, item, delim))
                separated.push_back(item);
            return separated;
        };

        string categories = compoundsQuery->stringValue("category");
        vector<string> categoryVect;
        for (auto category : split(categories, ';')) {
            if (!category.empty())
                categoryVect.push_back(category);
        }
        compound->setCategory(categoryVect);

        string fragmentMzValues = compoundsQuery->stringValue("fragment_mzs");
        vector<float> mzValues;
        for (string fragMz : split(fragmentMzValues, ';')) {
            if (!fragMz.empty())
                mzValues.push_back(stof(fragMz));
        }
        compound->setFragmentMzValues(mzValues);

        string fragmentIntensities =
                compoundsQuery->stringValue("fragment_intensity");
        vector<float> intensities;
        for (string fragIntensity : split(fragmentIntensities, ';')) {
            if (!fragIntensity.empty())
                intensities.push_back(stof(fragIntensity));
        }
        compound->setFragmentIntensities(intensities);

        vector<string> fragmentIonTypes =
            split(compoundsQuery->stringValue("fragment_ion_types"), ';');
        map<int, string> ionTypes;
        for (size_t i = 0; i < fragmentIonTypes.size(); ++i) {
            string fragIonType = fragmentIonTypes[i];
            if (!fragIonType.empty())
                ionTypes[i] = fragIonType;
        }
        compound->setFragmentIonTypes(ionTypes);

        _compoundIdMap[compound->id()  + compound->name() + compound->db()] = compound;
        compounds.push_back(compound);
        loadCount++;
    }

    sort(compounds.begin(), compounds.end(), Compound::compMass);
    cerr << "Loaded: " << loadCount << " compounds" << endl;
    if (loadCount > 0 and !databaseName.empty())
        _loadedCompoundDatabases.push_back(databaseName);

    return compounds;
}

void ProjectDatabase::loadAndPerformAlignment(const vector<mzSample*>& loaded)
{
    auto alignmentQuery = _connection->prepare(
        "SELECT samples.name AS sample_name                \
              , alignment_rts.*                            \
           FROM alignment_rts                              \
              , samples                                    \
          WHERE samples.sample_id = alignment_rts.sample_id");

    unordered_map<int, unordered_map<int, Scan*>> sampleScanMap;
    for (auto sample : loaded) {
        // ignore samples having MS2 scans
        if (sample->ms1ScanCount() == 0)
            continue;

        unordered_map<int, Scan*> scanMap;
        for (auto scan : sample->scans) {
            if (scan->mslevel > 1)
                continue;

            scanMap[scan->scannum] = scan;
        }
        sampleScanMap[sample->getSampleId()] = scanMap;
    }

    Aligner aligner;
    aligner.setSamples(loaded);
    AlignmentSegment* lastSegment = nullptr;
    int segCount = 0;

    while (alignmentQuery->next()) {
        string sampleName = alignmentQuery->stringValue("sample_name");
        int sampleId = alignmentQuery->integerValue("sample_id");
        if (!sampleScanMap.count(sampleId)) {
            cerr << "Error: no sample with id " << sampleId << " found" << endl;
            continue;
        }

        int scannum = alignmentQuery->integerValue("scannum");
        if (scannum != -1) {
            // perform regular alignment
            auto scanMap = sampleScanMap[sampleId];
            if (!scanMap.count(scannum)) {
                cerr << "Error: no scan with scannum " << sampleId << endl;
                continue;
            }

            Scan* scan = scanMap[scannum];
            scan->rt = alignmentQuery->floatValue("rt_updated");
            scan->originalRt = alignmentQuery->floatValue("rt_original");
        } else {
            // perform segmented alignment
            segCount++;
            AlignmentSegment* seg = new AlignmentSegment();
            seg->sampleName = sampleName;
            seg->segStart = 0;
            seg->segEnd   = alignmentQuery->floatValue("rt_original");
            seg->newStart = 0;
            seg->newEnd   = alignmentQuery->floatValue("rt_updated");

            if (lastSegment and lastSegment->sampleName == seg->sampleName) {
                seg->segStart = lastSegment->segEnd;
                seg->newStart = lastSegment->newEnd;
            }
            auto alignmentSegments = aligner.alignmentSegments();
            aligner.addSegment(sampleName, *seg, alignmentSegments);
            lastSegment = seg;
            aligner.setAlignmentSegment(alignmentSegments);
        }
    }

    if (segCount > 0)
        aligner.performSegmentedAlignment();
}

map<string, variant> ProjectDatabase::loadSettings()
{
    map<string, variant> settingsMap;

    auto settingsQuery = _connection->prepare("SELECT *            \
                                                 FROM user_settings");
    while (settingsQuery->next()) {
        settingsMap["ionizationMode"] = variant(settingsQuery->integerValue("ionization_mode"));
        settingsMap["ionizationType"] = variant(settingsQuery->integerValue("ionization_type"));
        settingsMap["instrumentType"] = variant(settingsQuery->integerValue("instrument_type"));
        settingsMap["q1Accuracy"] = variant(settingsQuery->doubleValue("q1_accuracy"));
        settingsMap["q3Accuracy"] = variant(settingsQuery->doubleValue("q3_accuracy"));
        settingsMap["filterline"] = variant(settingsQuery->integerValue("filterline"));

        settingsMap["centroidScans"] = variant(settingsQuery->integerValue("centroid_scans"));
        settingsMap["scanFilterPolarity"] = variant(settingsQuery->integerValue("scan_filter_polarity"));
        settingsMap["scanFilterMsLevel"] = variant(settingsQuery->integerValue("scan_filter_ms_level"));
        settingsMap["scanFilterMinQuantile"] = variant(settingsQuery->integerValue("scan_filter_min_quantile"));
        settingsMap["scanFilterMinIntensity"] = variant(settingsQuery->integerValue("scan_filter_min_intensity"));
        settingsMap["uploadMultiprocessing"] = variant(settingsQuery->integerValue("upload_multiprocessing"));

        settingsMap["eicSmoothingAlgorithm"] = variant(settingsQuery->integerValue("eic_smoothing_algorithm"));
        settingsMap["eicSmoothingWindow"] = variant(settingsQuery->integerValue("eic_smoothing_window"));
        settingsMap["maxRtDiffBetweenPeaks"] = variant(settingsQuery->doubleValue("max_rt_difference_bw_peaks"));

        settingsMap["aslsBaselineMode"] = variant(settingsQuery->integerValue("asls_baseline_mode"));
        settingsMap["baselineQuantile"] = variant(settingsQuery->integerValue("baseline_quantile"));
        settingsMap["baselineSmoothingWindow"] = variant(settingsQuery->integerValue("baseline_smoothing_window"));
        settingsMap["aslsSmoothness"] = variant(settingsQuery->integerValue("asls_smoothness"));
        settingsMap["aslsAsymmetry"] = variant(settingsQuery->integerValue("asls_asymmetry"));

        settingsMap["isotopeFilterEqualPeak"] = variant(settingsQuery->integerValue("isotope_filter_equal_peak"));
        settingsMap["minSignalBaselineDifference"] = variant(settingsQuery->doubleValue("min_signal_baseline_diff"));
        settingsMap["minPeakQuality"] = variant(settingsQuery->doubleValue("min_peak_quality"));
        settingsMap["isotopeMinSignalBaselineDifference"] = variant(settingsQuery->doubleValue("isotope_min_signal_baseline_diff"));
        settingsMap["isotopeMinPeakQuality"] = variant(settingsQuery->doubleValue("isotope_min_peak_quality"));

        settingsMap["D2LabelBPE"] = variant(settingsQuery->integerValue("d2_label_bpe"));
        settingsMap["C13LabelBPE"] = variant(settingsQuery->integerValue("c13_label_bpe"));
        settingsMap["N15LabelBPE"] = variant(settingsQuery->integerValue("n15_label_bpe"));
        settingsMap["S34LabelBPE"] = variant(settingsQuery->integerValue("s34_label_bpe"));

        settingsMap["minIsotopeParentCorrelation"] = variant(settingsQuery->doubleValue("min_isotope_parent_correlation"));
        settingsMap["maxIsotopeScanDiff"] = variant(settingsQuery->integerValue("max_isotope_scan_diff"));
        settingsMap["abundanceThreshold"] = variant(settingsQuery->doubleValue("abundance_threshold"));
        settingsMap["maxNaturalAbundanceError"] = variant(settingsQuery->doubleValue("max_natural_abundance_error"));
        settingsMap["correctC13IsotopeAbundance"] = variant(settingsQuery->integerValue("correct_c13_isotope_abundance"));

        settingsMap["eicType"] = variant(settingsQuery->integerValue("eic_type"));

        settingsMap["useOverlap"] = variant(settingsQuery->integerValue("use_overlap"));
        settingsMap["distXWeight"] = variant(settingsQuery->doubleValue("dist_x_weight"));
        settingsMap["distYWeight"] = variant(settingsQuery->doubleValue("dist_y_weight"));
        settingsMap["overlapWeight"] = variant(settingsQuery->doubleValue("overlap_weight"));

        settingsMap["considerDeltaRT"] = variant(settingsQuery->integerValue("consider_delta_rt"));
        settingsMap["qualityWeight"] = variant(settingsQuery->integerValue("quality_weight"));
        settingsMap["intensityWeight"] = variant(settingsQuery->integerValue("intensity_weight"));
        settingsMap["deltaRTWeight"] = variant(settingsQuery->integerValue("delta_rt_weight"));

        settingsMap["massCutoffType"] = variant(settingsQuery->stringValue("mass_cutoff_type"));

        settingsMap["automatedDetection"] = variant(settingsQuery->integerValue("automated_detection"));
        settingsMap["massDomainResolution"] = settingsQuery->doubleValue("mass_domain_resolution");
        settingsMap["timeDomainResolution"] = variant(settingsQuery->integerValue("time_domain_resolution"));
        settingsMap["minMz"] = variant(settingsQuery->doubleValue("min_mz"));
        settingsMap["maxMz"] = variant(settingsQuery->doubleValue("max_mz"));
        settingsMap["minRt"] = settingsQuery->doubleValue("min_rt");
        settingsMap["maxRt"] = variant(settingsQuery->doubleValue("max_rt"));
        settingsMap["minIntensity"] = variant(settingsQuery->doubleValue("min_intensity"));
        settingsMap["maxIntensity"] = variant(settingsQuery->doubleValue("max_intensity"));
        settingsMap["mustHaveFragmentation"] = variant(settingsQuery->integerValue("must_have_fragmentation"));
        settingsMap["identificationMatchRt"] = variant(settingsQuery->integerValue("identification_match_rt"));
        settingsMap["identificationRtWindow"] = variant(settingsQuery->doubleValue("identification_rt_window"));

        settingsMap["databaseSearch"] = variant(settingsQuery->integerValue("database_search"));
        settingsMap["compoundExtractionWindow"] = settingsQuery->doubleValue("compound_extraction_window");
        settingsMap["matchRt"] = variant(settingsQuery->integerValue("match_rt"));
        settingsMap["compoundRtWindow"] = variant(settingsQuery->doubleValue("compound_rt_window"));
        settingsMap["limitGroupsPerCompound"] = variant(settingsQuery->integerValue("limit_groups_per_compound"));
        settingsMap["searchAdducts"] = variant(settingsQuery->integerValue("search_adducts"));
        settingsMap["adductSearchWindow"] = variant(settingsQuery->doubleValue("adduct_search_window"));
        settingsMap["adductPercentCorrelation"] = variant(settingsQuery->doubleValue("adduct_percent_correlation"));

        settingsMap["matchFragmentation"] = settingsQuery->doubleValue("match_fragmentation");
        settingsMap["minFragMatchScore"] = variant(settingsQuery->integerValue("min_frag_match_score"));
        settingsMap["fragmentTolerance"] = variant(settingsQuery->doubleValue("fragment_tolerance"));
        settingsMap["minFragMatch"] = variant(settingsQuery->integerValue("min_frag_match"));

        settingsMap["reportIsotopes"] = variant(settingsQuery->integerValue("report_isotopes"));

        settingsMap["peakQuantitation"] = variant(settingsQuery->integerValue("peak_quantitation"));
        settingsMap["minGroupIntensity"] = variant(settingsQuery->doubleValue("min_group_intensity"));
        settingsMap["intensityQuantile"] = variant(settingsQuery->integerValue("intensity_quantile"));
        settingsMap["minGroupQuality"] = variant(settingsQuery->doubleValue("min_group_quality"));
        settingsMap["qualityQuantile"] = variant(settingsQuery->integerValue("quality_quantile"));
        settingsMap["minSignalBlankRatio"] = variant(settingsQuery->doubleValue("min_signal_blank_ratio"));
        settingsMap["signalBlankRatioQuantile"] = variant(settingsQuery->integerValue("signal_blank_ratio_quantile"));
        settingsMap["minSignalBaselineRatio"] = variant(settingsQuery->doubleValue("min_signal_baseline_ratio"));
        settingsMap["signalBaselineRatioQuantile"] = settingsQuery->integerValue("signal_baseline_ratio_quantile");
        settingsMap["minPeakWidth"] = variant(settingsQuery->integerValue("min_peak_width"));
        settingsMap["minGoodPeakCount"] = variant(settingsQuery->integerValue("min_good_peak_count"));
        settingsMap["peakClassifierFile"] = variant(settingsQuery->stringValue("peak_classifier_file"));

        settingsMap["mainWindowSelectedDbName"] = settingsQuery->stringValue("main_window_selected_db_name");
        settingsMap["mainWindowCharge"] = settingsQuery->integerValue("main_window_charge");
        settingsMap["mainWindowPeakQuantitation"] = settingsQuery->integerValue("main_window_peak_quantitation");
        settingsMap["mainWindowMassResolution"] = settingsQuery->doubleValue("main_window_mass_resolution");
    }

    return settingsMap;
}

void ProjectDatabase::deleteAll()
{
    deleteAllSamples();
    deleteAllCompounds();
    deleteAllGroupsAndPeaks();
    deleteAllScans();
    deleteAllAlignmentData();
    deleteSettings();
}

void ProjectDatabase::deleteAllSamples()
{
    _connection->prepare("DROP TABLE samples")->execute();
    _connection->commit();
}

void ProjectDatabase::deleteAllCompounds()
{
    _connection->prepare("DROP TABLE compounds")->execute();
    _connection->commit();
}

void ProjectDatabase::deleteAllGroupsAndPeaks()
{
    _connection->prepare("DROP TABLE peaks")->execute();
    _connection->prepare("DROP TABLE peakgroups")->execute();
    _connection->commit();
}

void ProjectDatabase::deleteAllScans()
{
    _connection->prepare("DROP TABLE scans")->execute();
    _connection->commit();
}

void ProjectDatabase::deleteAllAlignmentData()
{
    _connection->prepare("DROP TABLE alignment_rts")->execute();
    _connection->commit();
}

void ProjectDatabase::deleteSettings()
{
    _connection->prepare("DROP TABLE user_settings")->execute();
    _connection->commit();
}

void ProjectDatabase::deleteCompoundsForDB(const string& dbName)
{
    // create index based on database name.
    _connection->prepare(CREATE_COMPOUNDS_DB_INDEX)->execute();

    auto compoundsQuery = _connection->prepare(
                "DELETE FROM compounds              \
                       WHERE db_name = :database_name");
    compoundsQuery->bind(":database_name", dbName);
    if (!compoundsQuery->execute())
        cerr << "Error: failed to delete compounds for db " << dbName << endl;

    _connection->commit();
}

void ProjectDatabase::deleteTableGroups(const string& tableName)
{
    auto failure = false;
    auto peaksQuery = _connection->prepare(
        "DELETE FROM peaks                                        \
               WHERE group_id IN (SELECT group_id                 \
                                    FROM peakgroups               \
                                   WHERE table_name = :table_name)");
    peaksQuery->bind(":table_name", tableName);
    if (!peaksQuery->execute()) {
        failure = true;
        cerr << "Error: failed to delete peaks for search table "
             << tableName << endl;
    }

    auto peakgroupsQuery = _connection->prepare(
        "DELETE FROM peakgroups             \
               WHERE table_name = :table_name");
    peakgroupsQuery->bind(":table_name", tableName);
    if(!peakgroupsQuery->execute()) {
        failure = true;
        cerr << "Error: failed to delete peak groups for search table "
             << tableName << endl;
    }

    // do not commit if peak or group deletion was unsuccessful
    if (failure) {
        _connection->rollback();
        return;
    }

    _connection->commit();
}

void ProjectDatabase::deletePeakGroup(PeakGroup* group)
{
    if (!group)
        return;

    string tableName = group->tableName();
    vector<int> selectedGroups;
    selectedGroups.push_back(group->groupId);
    for (const auto& child : group->children)
        selectedGroups.push_back(child.groupId);

    if (selectedGroups.size() == 0)
        return;

    auto peakgroupsQuery = _connection->prepare(
                "DELETE FROM peakgroups                 \
                       WHERE table_group_id = :group_id \
                         AND table_name = :table_name   ");

    auto peaksQuery = _connection->prepare(
                "DELETE FROM peaks                                          \
                       WHERE group_id IN (SELECT group_id                   \
                                            FROM peakgroups                 \
                                           WHERE table_group_id = :group_id \
                                             AND table_name = :table_name)  ");

    for (auto groupId : selectedGroups) {
        peakgroupsQuery->bind(":group_id", groupId);
        peakgroupsQuery->bind(":table_name", tableName);
        peaksQuery->bind(":group_id", groupId);
        peaksQuery->bind(":table_name", tableName);

        if (!peaksQuery->execute()) {
            cerr << "Error: while deleting peaks" << endl;
            _connection->rollback();
            return;
        }

        if (!peakgroupsQuery->execute()) {
            cerr << "Error: while deleting peakgroups" << endl;
            _connection->rollback();
            return;
        }
    }

    _connection->commit();
}

vector<string> ProjectDatabase::getTableNames()
{
    auto query = _connection->prepare(
        "SELECT DISTINCT table_name \
                    FROM peakgroups ");

    vector<string> dbNames;
    while (query->next()) {
        auto dbName = query->stringValue("table_name");
        if (!dbName.empty())
            dbNames.push_back(dbName);
    }
    return dbNames;
}

string ProjectDatabase::projectPath()
{
    string databaseFile = _connection->dbPath();
    boost::filesystem::path path(databaseFile);
    return path.parent_path().string();
}

string ProjectDatabase::projectName()
{
    string databaseFile = _connection->dbPath();
    boost::filesystem::path path(databaseFile);
    return path.filename().string();
}

int ProjectDatabase::version()
{
    auto query = _connection->prepare("PRAGMA user_version");
    auto version = 0;
    while (query->next())
        version = query->integerValue("user_version");
    return version;
}

bool ProjectDatabase::isEmpty()
{
    auto query = _connection->prepare(
        "SELECT COUNT(*) as table_count \
           FROM sqlite_master           \
          WHERE type = 'table'          ");
    auto tableCount = 0;
    while (query->next())
        tableCount = query->integerValue("table_count");
    return tableCount == 0;
}

void ProjectDatabase::vacuum()
{
    _connection->vacuum();
}

bool ProjectDatabase::openConnection()
{
    return _connection != nullptr;
}

void ProjectDatabase::_assignSampleIds(const vector<mzSample*>& samples) {
    int maxSampleId = -1;
    for (auto sample : samples)
        if (sample->getSampleId() > maxSampleId)
            maxSampleId = sample->getSampleId();

    // this is the first sample to be assigned
    if (maxSampleId == -1)
        maxSampleId = 0;

    for (auto sample : samples) {
        if (sample->getSampleId() == -1)
            sample->setSampleId(maxSampleId++);
        cerr << "Debug: assigned "
             << sample->sampleName
             << "\t with ID "
             << sample->getSampleId()
             << endl;
    }
}

Adduct* ProjectDatabase::_findAdductByName(string name)
{
    return new Adduct(name, 0, 0, 0.0f);
}

Compound* ProjectDatabase::_findSpeciesByIdAndName(string id,
                                                   string name,
                                                   string databaseName)
{
    // load compounds from database if not already loaded.
    if (!databaseName.empty() && !_compoundDatabaseLoaded(databaseName))
        loadCompounds(databaseName);

    if (_compoundIdMap.count(id + name + databaseName))
        return _compoundIdMap[id + name + databaseName];

    return nullptr;
}

vector<Compound*> ProjectDatabase::_findSpeciesByName(string name,
                                                      string databaseName)
{
    if (!databaseName.empty() && !_compoundDatabaseLoaded(databaseName))
        loadCompounds(databaseName);

    vector<Compound*> similarlyNamedCompounds;
    for (const auto& key : _compoundIdMap) {
        Compound* compound = key.second;
        if (compound->name() == name && compound->db() == databaseName)
            similarlyNamedCompounds.push_back(compound);
    }
    return similarlyNamedCompounds;
}

bool ProjectDatabase::_compoundDatabaseLoaded(string databaseName)
{
    auto beginning = begin(_loadedCompoundDatabases);
    auto ending = end(_loadedCompoundDatabases);
    if (find(beginning, ending, databaseName) != ending)
        return true;
    return false;
}

string ProjectDatabase::_getScanSignature(Scan* scan, int limitSize)
{
    stringstream signature;
    map<int, bool> seen;
    int mz_count = 0;
    for (auto posIndex : scan->intensityOrderDesc()) {
        size_t pos = static_cast<unsigned int>(posIndex);
        int mzround = static_cast<int>(scan->mz[pos]);
        if (!seen.count(mzround)) {
            signature << "[" << setprecision(9)
                      << scan->mz[pos] << ","
                      << scan->intensity[pos] << "]";
            seen[mzround] = true;
        }

        if (mz_count++ >= limitSize)
            break;
    }
    return signature.str();
}

string ProjectDatabase::_locateSample(const string filepath,
                                      const vector<string>& pathlist)
{
    // found file, all is good
    boost::filesystem::path sampleFile(filepath);
    if (boost::filesystem::exists(sampleFile))
        return filepath;

    // search for file
    string fileName = sampleFile.filename().string();
    for (auto& path : pathlist) {
        boost::filesystem::path possiblePath(path);
        auto filepath = possiblePath / fileName;
        if (boost::filesystem::exists(filepath))
            return filepath.string();
    }
    return "";
}

void ProjectDatabase::_setVersion(int version)
{
    // using this syntax, because SQLite does not support
    // binding for PRAGMA statements
    auto query = _connection->prepare("PRAGMA user_version = "
                                      + to_string(version));
    query->execute();
}
