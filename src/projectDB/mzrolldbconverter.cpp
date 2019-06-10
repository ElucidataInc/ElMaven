#include <boost/filesystem.hpp>

#include "connection.h"
#include "cursor.h"
#include "mzrolldbconverter.h"
#include "mzUtils.h"
#include "schema.h"

using namespace std;

namespace bfs = boost::filesystem;

void MzrollDbConverter::convertLegacyToCurrent(const string& fromPath,
                                               const string& toPath)
{
    bfs::path originalPath(fromPath);
    bfs::path newPath(toPath);
    if (!bfs::exists(fromPath))
        return;

    cerr << "Converting provided mzrollDb to emDb…" << endl;

    Connection mzrollDb(fromPath);
    Connection emDb(toPath);

    copySamples(mzrollDb, emDb);
    copyScans(mzrollDb, emDb);
    copyPeakgroups(mzrollDb, emDb);
    copyPeaks(mzrollDb, emDb);
    copyCompounds(mzrollDb, emDb);
    copyAlignmentData(mzrollDb, emDb);
    setVersion(emDb, 1);
}

void MzrollDbConverter::copySamples(Connection &mzrollDb, Connection &emDb)
{
    if (!emDb.prepare(CREATE_SAMPLES_TABLE)->execute()) {
        cerr << "Error: failed to create samples table" << endl;
        return;
    }

    auto writeQuery = emDb.prepare(
        "INSERT INTO samples         \
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

    auto readQuery = mzrollDb.prepare("SELECT *       \
                                         FROM samples ");

    emDb.begin();
    while (readQuery->next()) {
        writeQuery->bind(":sample_id",
                         readQuery->integerValue("sampleId"));
        writeQuery->bind(":name",
                         mzUtils::cleanFilename(readQuery->stringValue("name")));
        writeQuery->bind(":filename",
                         readQuery->stringValue("filename"));
        writeQuery->bind(":set_name",
                         readQuery->stringValue("setName"));
        writeQuery->bind(":sample_order",
                         readQuery->integerValue("sampleOrder"));
        writeQuery->bind(":is_blank",
                         0);
        writeQuery->bind(":is_selected",
                         readQuery->integerValue("isSelected"));
        writeQuery->bind(":color_red",
                         readQuery->floatValue("color_red"));
        writeQuery->bind(":color_green",
                         readQuery->floatValue("color_green"));
        writeQuery->bind(":color_blue",
                         readQuery->floatValue("color_blue"));
        writeQuery->bind(":color_alpha",
                         readQuery->floatValue("color_alpha"));
        writeQuery->bind(":normal_const",
                         readQuery->floatValue("norml_const"));
        writeQuery->bind(":transform_a0",
                         readQuery->floatValue("transform_a0"));
        writeQuery->bind(":transform_a1",
                         readQuery->floatValue("transform_a1"));
        writeQuery->bind(":transform_a2",
                         readQuery->floatValue("transform_a2"));
        writeQuery->bind(":transform_a4",
                         readQuery->floatValue("transform_a4"));
        writeQuery->bind(":transform_a5",
                         readQuery->floatValue("transform_a5"));

        if (!writeQuery->execute())
            cerr << "Error: failed to save sample" << endl;
    }
    emDb.commit();
}

void MzrollDbConverter::copyScans(Connection &mzrollDb, Connection &emDb)
{
    if(!emDb.prepare(CREATE_SCANS_TABLE)->execute()) {
        cerr << "Error: failed to create scans table" << endl;
        return;
    }

    auto writeQuery = emDb.prepare(
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

    auto readQuery = mzrollDb.prepare("SELECT *     \
                                         FROM scans ");

    emDb.begin();
    while (readQuery->next()) {
        writeQuery->bind(":sample_id",
                         readQuery->integerValue("sampleId"));
        writeQuery->bind(":scan",
                         readQuery->integerValue("scan"));
        writeQuery->bind(":file_seek_start",
                         readQuery->integerValue("fileSeekStart"));
        writeQuery->bind(":file_seek_end",
                         readQuery->integerValue("fileSeekEnd"));
        writeQuery->bind(":mslevel",
                         readQuery->integerValue("mslevel"));
        writeQuery->bind(":rt",
                         readQuery->floatValue("rt"));
        writeQuery->bind(":precursor_mz",
                         readQuery->floatValue("precursorMz"));
        writeQuery->bind(":precursor_charge",
                         readQuery->integerValue("precursorCharge"));
        writeQuery->bind(":precursor_ic",
                         readQuery->floatValue("precursorIc"));
        writeQuery->bind(":precursor_purity",
                         readQuery->floatValue("precursorPurity"));
        writeQuery->bind(":minmz",
                         readQuery->floatValue("minmz"));
        writeQuery->bind(":maxmz",
                         readQuery->floatValue("maxmz"));
        writeQuery->bind(":data",
                         readQuery->stringValue("data"));

        if (!writeQuery->execute())
            cerr << "Error: failed to save scan" << endl;
    }
    emDb.commit();
}

void MzrollDbConverter::copyPeakgroups(Connection &mzrollDb, Connection &emDb)
{
    if(!emDb.prepare(CREATE_PEAK_GROUPS_TABLE)->execute()) {
        cerr << "Error: failed to create peakgroups table" << endl;
        return;
    }

    auto writeQuery = emDb.prepare(
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
                     , :sample_ids                         )");

    auto readQuery = mzrollDb.prepare("SELECT *          \
                                         FROM peakgroups ");

    emDb.begin();
    while (readQuery->next()) {
        writeQuery->bind(":group_id",
                         readQuery->integerValue("groupId"));
        writeQuery->bind(":parent_group_id",
                         readQuery->integerValue("parentGroupId"));
        writeQuery->bind(":meta_group_id",
                         readQuery->integerValue("metaGroupId"));
        writeQuery->bind(":tag_string",
                         readQuery->stringValue("tagString"));
        writeQuery->bind(":expected_mz",
                         0.0);
        writeQuery->bind(":expected_abundance",
                         0.0);
        writeQuery->bind(":expected_rt_diff",
                         readQuery->floatValue("expectedRtDiff"));
        writeQuery->bind(":group_rank",
                         readQuery->floatValue("groupRank"));
        writeQuery->bind(":label",
                         readQuery->stringValue("label"));
        writeQuery->bind(":type",
                         readQuery->integerValue("type"));
        writeQuery->bind(":srm_id",
                         readQuery->stringValue("srmId"));
        writeQuery->bind(":ms2_event_count",
                         readQuery->integerValue("ms2EventCount"));
        writeQuery->bind(":ms2_score",
                         readQuery->floatValue("ms2Score"));
        writeQuery->bind(":adduct_name",
                         readQuery->stringValue("adductName"));
        writeQuery->bind(":compound_id",
                         readQuery->stringValue("compoundId"));
        writeQuery->bind(":compound_name",
                         readQuery->stringValue("compoundName"));
        writeQuery->bind(":compound_db",
                         readQuery->stringValue("compoundDB"));
        writeQuery->bind(":table_name",
                         readQuery->stringValue("searchTableName"));

        if (!writeQuery->execute())
            cerr << "Error: failed to save peak group" << endl;
    }
    emDb.commit();
    emDb.prepare(CREATE_PEAKS_GROUP_INDEX)->execute();

    // rename all peak tables to be compatible with El-MAVEN
    auto tableReadQuery = emDb.prepare("SELECT DISTINCT table_name \
                                                   FROM peakgroups ");
    vector<string> tables;
    while (tableReadQuery->next()) {
        auto table = tableReadQuery->stringValue("table_name");
        if (!table.empty())
            tables.push_back(table);
    }
    auto tableWriteQuery = emDb.prepare("UPDATE peakgroups                   \
                                            SET table_name = :new_table_name \
                                          WHERE table_name = :old_table_name ");
    emDb.begin();
    int peakTableCount = 0;
    for (auto table : tables) {
        string newTable = "Peak Table " + to_string(++peakTableCount);
        tableWriteQuery->bind(":old_table_name", table);
        tableWriteQuery->bind(":new_table_name", newTable);
        tableWriteQuery->execute();
    }
    emDb.commit();
}

void MzrollDbConverter::copyPeaks(Connection &mzrollDb, Connection &emDb)
{
    if(!emDb.prepare(CREATE_PEAKS_TABLE)->execute()) {
        cerr << "Error: failed to create peaks table" << endl;
        return;
    }

    auto writeQuery = emDb.prepare(
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

    auto readQuery = mzrollDb.prepare("SELECT *     \
                                         FROM peaks ");

    emDb.begin();
    while (readQuery->next()) {
        writeQuery->bind(":peak_id",
                         readQuery->integerValue("peakId"));
        writeQuery->bind(":group_id",
                         readQuery->integerValue("groupId"));
        writeQuery->bind(":sample_id",
                         readQuery->integerValue("sampleId"));
        writeQuery->bind(":pos",
                         readQuery->integerValue("pos"));
        writeQuery->bind(":minpos",
                         readQuery->integerValue("minpos"));
        writeQuery->bind(":maxpos",
                         readQuery->integerValue("maxpos"));
        writeQuery->bind(":rt",
                         readQuery->floatValue("rt"));
        writeQuery->bind(":rtmin",
                         readQuery->floatValue("rtmin"));
        writeQuery->bind(":rtmax",
                         readQuery->floatValue("rtmax"));
        writeQuery->bind(":mzmin",
                         readQuery->floatValue("mzmin"));
        writeQuery->bind(":mzmax",
                         readQuery->floatValue("mzmax"));
        writeQuery->bind(":scan",
                         readQuery->integerValue("scan"));
        writeQuery->bind(":minscan",
                         readQuery->integerValue("minscan"));
        writeQuery->bind(":maxscan",
                         readQuery->integerValue("maxscan"));
        writeQuery->bind(":peak_area",
                         readQuery->floatValue("peakArea"));
        writeQuery->bind(":peak_area_corrected",
                         readQuery->floatValue("peakAreaCorrected"));
        writeQuery->bind(":peak_area_top",
                         readQuery->floatValue("peakAreaTop"));
        writeQuery->bind(":peak_area_top_corrected",
                         0.0);
        writeQuery->bind(":peak_area_fractional",
                         readQuery->floatValue("peakAreaFractional"));
        writeQuery->bind(":peak_rank",
                         readQuery->floatValue("peakRank"));
        writeQuery->bind(":peak_intensity",
                         readQuery->floatValue("peakIntensity"));
        writeQuery->bind(":peak_baseline_level",
                         readQuery->floatValue("peakBaseLineLevel"));
        writeQuery->bind(":peak_mz",
                         readQuery->floatValue("peakMz"));
        writeQuery->bind(":median_mz",
                         readQuery->floatValue("medianMz"));
        writeQuery->bind(":base_mz",
                         readQuery->floatValue("baseMz"));
        writeQuery->bind(":quality",
                         readQuery->floatValue("quality"));
        writeQuery->bind(":width",
                         readQuery->integerValue("width"));
        writeQuery->bind(":gauss_fit_sigma",
                         readQuery->floatValue("gaussFitSigma"));
        writeQuery->bind(":gauss_fit_r2",
                         readQuery->floatValue("gaussFitR2"));
        writeQuery->bind(":no_noise_obs",
                         readQuery->integerValue("noNoiseObs"));
        writeQuery->bind(":no_noise_fraction",
                         readQuery->floatValue("noNoiseFraction"));
        writeQuery->bind(":symmetry",
                         readQuery->floatValue("symmetry"));
        writeQuery->bind(":signal_baseline_ratio",
                         readQuery->floatValue("signalBaselineRatio"));
        writeQuery->bind(":group_overlap",
                         readQuery->floatValue("groupOverlap"));
        writeQuery->bind(":group_overlap_frac",
                         readQuery->floatValue("groupOverlapFrac"));
        writeQuery->bind(":local_max_flag",
                         readQuery->floatValue("localMaxFlag"));
        writeQuery->bind(":from_blank_sample",
                         readQuery->integerValue("fromBlankSample"));
        writeQuery->bind(":label",
                         readQuery->integerValue("label"));

        if (!writeQuery->execute())
            cerr << "Error: failed to save peak" << endl;
    }
    emDb.commit();
}

void MzrollDbConverter::copyCompounds(Connection &mzrollDb, Connection &emDb)
{
    if (!emDb.prepare(CREATE_COMPOUNDS_TABLE)->execute()) {
        cerr << "Error: failed to create compounds table" << endl;
        return;
    }

    auto writeQuery = emDb.prepare(
        "INSERT INTO compounds                 \
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

    auto readQuery = mzrollDb.prepare("SELECT *         \
                                         FROM compounds ");

    emDb.begin();
    while (readQuery->next()) {
        writeQuery->bind(":compound_id",
                         readQuery->stringValue("compoundId"));
        writeQuery->bind(":db_name",
                         readQuery->stringValue("dbName"));
        writeQuery->bind(":name",
                         readQuery->stringValue("name"));
        writeQuery->bind(":formula",
                         readQuery->stringValue("formula"));
        writeQuery->bind(":smile_string",
                         readQuery->stringValue("smileString"));
        writeQuery->bind(":srm_id",
                         readQuery->stringValue("srmId"));
        writeQuery->bind(":mass",
                         readQuery->floatValue("mass"));
        writeQuery->bind(":charge",
                         readQuery->integerValue("charge"));
        writeQuery->bind(":expected_rt",
                         readQuery->floatValue("expectedRt"));
        writeQuery->bind(":precursor_mz",
                         readQuery->floatValue("precursorMz"));
        writeQuery->bind(":product_mz",
                         readQuery->floatValue("productMz"));
        writeQuery->bind(":collision_energy",
                         readQuery->floatValue("collisionEnergy"));
        writeQuery->bind(":log_p",
                         readQuery->floatValue("logP"));
        writeQuery->bind(":virtual_fragmentation",
                         readQuery->integerValue("virtualFragmentation"));
        writeQuery->bind(":ionization_mode",
                         readQuery->integerValue("ionizationMode"));
        writeQuery->bind(":category",
                         readQuery->stringValue("category"));
        writeQuery->bind(":fragment_mzs",
                         readQuery->stringValue("fragment_mzs"));
        writeQuery->bind(":fragment_intensity",
                         readQuery->stringValue("fragment_intensity"));

        if (!writeQuery->execute())
            cerr << "Error: failed to save compound" << endl;
    }
    emDb.commit();
    emDb.prepare(CREATE_COMPOUNDS_DB_INDEX)->execute();
}

void MzrollDbConverter::copyAlignmentData(Connection &mzrollDb,
                                          Connection &emDb)
{
    if (!emDb.prepare(CREATE_ALIGNMENT_TABLE)->execute()) {
        cerr << "Error: failed to create alignment_rts table" << endl;
        return;
    }

    auto writeQuery = emDb.prepare(
        "INSERT INTO alignment_rts   \
              VALUES ( :sample_id    \
                     , :scannum      \
                     , :rt_original  \
                     , :rt_updated   )");

    auto readQuery = mzrollDb.prepare("SELECT *             \
                                         FROM rt_update_key ");

    emDb.begin();
    while (readQuery->next()) {
        writeQuery->bind(":sample_id",
                         readQuery->integerValue("sampleId"));
        writeQuery->bind(":scannum",
                         -1);
        writeQuery->bind(":rt_original",
                         readQuery->floatValue("rt"));
        writeQuery->bind(":rt_updated",
                         readQuery->floatValue("rt_update"));

        if (!writeQuery->execute())
            cerr << "Error: failed to save alignment values" << endl;
    }
    emDb.commit();
}

void MzrollDbConverter::setVersion(Connection &emDb, int version)
{
    auto query = emDb.prepare("PRAGMA user_version = " + to_string(version));
    query->execute();
}
