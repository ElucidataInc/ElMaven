#include <sstream>
#include <boost/filesystem.hpp>
#include "projectdatabase.h"
#include "Compound.h"
#include "connection.h"
#include "cursor.h"
#include "mzSample.h"
#include "schema.h"

ProjectDatabase::ProjectDatabase(const string& dbFilename)
{
    _connection = new Connection(dbFilename);
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

        samplesQuery->bind(":normal_const", s->getNormalizationConstant());

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
        "INSERT INTO peakgroups            \
              VALUES ( :group_id           \
                     , :parent_group_id    \
                     , :meta_group_id      \
                     , :tag_string         \
                     , :expected_mz        \
                     , :expected_rt_diff   \
                     , :expected_abundance \
                     , :group_rank         \
                     , :label              \
                     , :type               \
                     , :srm_id             \
                     , :ms2_event_count    \
                     , :ms2_score          \
                     , :adduct_name        \
                     , :compound_id        \
                     , :compound_name      \
                     , :compound_db        \
                     , :table_name         )");

    groupsQuery->bind(":parent_group_id", parentGroupId);
    groupsQuery->bind(":meta_group_id", group->metaGroupId);
    groupsQuery->bind(":tag_string", group->tagString);
    groupsQuery->bind(":expected_mz", group->expectedMz);
    groupsQuery->bind(":expected_rt_diff", group->expectedRtDiff);
    groupsQuery->bind(":expected_abundance", group->expectedAbundance);
    groupsQuery->bind(":group_rank", group->groupRank);
    groupsQuery->bind(":label", string(1, group->label));
    groupsQuery->bind(":type", group->type());
    groupsQuery->bind(":srm_id", group->srmId);

    groupsQuery->bind(":ms2_event_count", group->ms2EventCount);
    groupsQuery->bind(":ms2_score", group->fragMatchScore.mergedScore);
    groupsQuery->bind(":adduct_name", group->adduct ? group->adduct->name : "");

    groupsQuery->bind(":compound_id",
                      group->compound ? group->compound->id : "");
    groupsQuery->bind(":compound_name",
                      group->compound ? group->compound->name : "");
    groupsQuery->bind(":compound_db",
                      group->compound ? group->compound->db : "");

    groupsQuery->bind(":table_name", tableName);

    if (!groupsQuery->execute())
        cerr << "Error: failed to save peak group" << endl;

    int lastInsertedGroupId = _connection->lastInsertId();
    saveGroupPeaks(group, lastInsertedGroupId);

    for (auto child: group->children)
        saveGroupAndPeaks(&child, lastInsertedGroupId, tableName);

    return lastInsertedGroupId;
}

void ProjectDatabase::saveGroupPeaks(PeakGroup* group, const int groupId)
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
                     , :label                   )");

    for (Peak p : group->peaks) {
        peaksQuery->bind(":group_id", groupId);
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
        if (group.compound)
            seenCompounds.insert(group.compound);
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
                      , :fragment_intensity    )");

    _connection->begin();

    for (Compound* c : seenCompounds) {
        stringstream categories;
        for (string s : c->category) {
            categories << s.c_str() << ";";
        }
        string catStr = categories.str();
        catStr = catStr.substr(0, catStr.size() - 1);

        stringstream fragMz;
        for (float f : c->fragment_mzs) {
            fragMz << fixed << setprecision(5) << f << ";";
        }
        string fragMzStr = fragMz.str();
        fragMzStr = fragMzStr.substr(0, fragMzStr.size() - 1);

        stringstream fragIntensity;
        for (float f : c->fragment_intensity) {
            fragIntensity << fixed << setprecision(5) << f << ";";
        }
        string fragIntensityStr = fragIntensity.str();
        fragIntensityStr = fragIntensityStr.substr(0,
                                                   fragIntensityStr.size() - 1);

        compoundsQuery->bind(":compound_id", c->id);
        compoundsQuery->bind(":db_name", c->db);
        compoundsQuery->bind(":name", c->name);
        compoundsQuery->bind(":formula", c->formula);
        compoundsQuery->bind(":smile_string", c->smileString);
        compoundsQuery->bind(":srm_id", c->srmId);

        compoundsQuery->bind(":mass", c->mass);
        compoundsQuery->bind(":charge", c->charge);
        compoundsQuery->bind(":expected_rt", c->expectedRt);
        compoundsQuery->bind(":precursor_mz", c->precursorMz);
        compoundsQuery->bind(":product_mz", c->productMz);

        compoundsQuery->bind(":collision_energy", c->collisionEnergy);
        compoundsQuery->bind(":log_p", c->logP);
        compoundsQuery->bind(":virtual_fragmentation", c->virtualFragmentation);
        compoundsQuery->bind(":ionization_mode", c->ionizationMode);

        compoundsQuery->bind(":category", catStr);
        compoundsQuery->bind(":fragment_mzs", fragMzStr);
        compoundsQuery->bind(":fragment_intensity", fragIntensityStr);

        if (!compoundsQuery->execute())
            cerr << "Error: failed to save compound " << c->name << endl;
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
        for (auto scan : s->scans) {
            float rt_original = scan->originalRt;
            float rt_updated = scan->rt;
            alignmentQuery->bind(":sample_id", s->getSampleId());
            alignmentQuery->bind(":scannum", scan->scannum);
            alignmentQuery->bind(":rt_original", rt_original);
            alignmentQuery->bind(":rt_updated", rt_updated);

            if (!alignmentQuery->execute())
                cerr << "Error: failed to write alignment data" << endl;
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
    map<PeakGroup*, int> childParentMap;
    while (groupsQuery->next()) {
        PeakGroup* group = new PeakGroup();
        group->groupId = groupsQuery->integerValue("group_id");
        int parentGroupId = groupsQuery->integerValue("parent_group_id");
        group->tagString = groupsQuery->stringValue("tag_string");
        group->metaGroupId = groupsQuery->integerValue("meta_group_id");
        group->expectedMz = groupsQuery->floatValue("expected_mz");
        group->expectedRtDiff = groupsQuery->floatValue("expected_rt_diff");
        group->expectedAbundance =
            groupsQuery->floatValue("expected_abundance");
        group->groupRank = groupsQuery->floatValue("group_rank");
        group->label = groupsQuery->stringValue("label")[0];
        group->ms2EventCount = groupsQuery->integerValue("ms2_event_count");
        group->fragMatchScore.mergedScore =
            groupsQuery->doubleValue("ms2_score");
        group->setType(PeakGroup::GroupType(groupsQuery->integerValue("type")));
        group->searchTableName = groupsQuery->stringValue("table_name");

        string compoundId = groupsQuery->stringValue("compound_id");
        string compoundDB = groupsQuery->stringValue("compound_db");
        string compoundName = groupsQuery->stringValue("compound_name");
        string adductName = groupsQuery->stringValue("adduct_name");

        string srmId = groupsQuery->stringValue("srm_id");
        if (!srmId.empty())
            group->setSrmId(srmId);

        if (!adductName.empty())
            group->adduct = _findAdductByName(adductName);

        if (!compoundId.empty()) {
            Compound* compound = _findSpeciesById(compoundId, compoundDB);
            if (compound) {
                group->compound = compound;
            } else {
                group->tagString = compoundName
                                  + " | "
                                  + adductName
                                  + " | id="
                                  + compoundId;
            }
        } else if (!compoundName.empty() && !compoundDB.empty()) {
            vector<Compound*> matches = _findSpeciesByName(compoundName,
                                                           compoundDB);
            if (matches.size() > 0)
                group->compound = matches[0];
        }

        loadGroupPeaks(group, loaded);
        group->groupStatistics();

        if (parentGroupId == 0) {
            groups.push_back(group);
        } else {
            childParentMap[group] = parentGroupId;
        }
    }

    // assign parents for child groups
    for (auto pair : childParentMap) {
        bool foundParent = false;
        auto child = pair.first;
        for (auto parent : groups) {
            if (parent->groupId == pair.second) {
                parent->addChild(*child);
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
                                     const vector<mzSample*>& loaded)
{
    auto peaksQuery = _connection->prepare(
                "SELECT peaks.*                             \
                      , samples.name AS sample_name         \
                   FROM peaks                               \
                      , samples                             \
                  WHERE peaks.sample_id = samples.sample_id \
                    AND peaks.group_id = :parent_group_id   ");
    peaksQuery->bind(":parent_group_id", parentGroup->groupId);

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
        peak.groupNum = peaksQuery->integerValue("group_id");
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
        if (_compoundIdMap.find(id + db) != end(_compoundIdMap))
            continue;

        // the neutral mass is computed automatically inside the constructor
        Compound* compound = new Compound(id, name, formula, charge);
        compound->db = db;
        compound->expectedRt = expectedRt;

        if (formula.empty()) {
            if (mass > 0)
                compound->mass = mass;
        } else {
            compound->mass =
                    static_cast<float>(mcalc.computeNeutralMass(formula));
        }

        compound->precursorMz = compoundsQuery->floatValue("precursor_mz");
        compound->productMz = compoundsQuery->floatValue("product_mz");
        compound->collisionEnergy =
                compoundsQuery->floatValue("collision_energy");
        compound->smileString = compoundsQuery->stringValue("smile_string");

        // mark compound as decoy if names contains DECOY string
        if (compound->name.find("DECOY") > 0)
            compound->isDecoy = true;

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
        for (auto category : split(categories, ';')) {
            if (!category.empty())
                compound->category.push_back(category);
        }

        string fragment_mzs = compoundsQuery->stringValue("fragment_mzs");
        for (string frag_mz : split(fragment_mzs, ';')) {
            if (!frag_mz.empty())
                compound->fragment_mzs.push_back(stof(frag_mz));
        }

        string fragment_intensities =
                compoundsQuery->stringValue("fragment_intensity");
        for (string frag_intensity : split(fragment_intensities, ';')) {
            if (!frag_intensity.empty())
                compound->fragment_intensity.push_back(stof(frag_intensity));
        }

        _compoundIdMap[compound->id + compound->db] = compound;
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

    map<int, map<int, Scan*>> sampleScanMap;
    for (auto sample : loaded) {
        map<int, Scan*> scanMap;
        for (auto scan : sample->scans) {
            scanMap[scan->scannum] = scan;
        }
        sampleScanMap[sample->getSampleId()] = scanMap;
    }

    while (alignmentQuery->next()) {
        string sampleName = alignmentQuery->stringValue("sample_name");
        int sampleId = alignmentQuery->integerValue("sample_id");
        if (!sampleScanMap.count(sampleId)) {
            cerr << "Error: no sample with id " << sampleId << " found" << endl;
            continue;
        }

        int scannum = alignmentQuery->integerValue("scannum");
        auto scanMap = sampleScanMap[sampleId];
        if (!scanMap.count(scannum)) {
            cerr << "Error: no scan with scannum " << sampleId << endl;
            continue;
        }

        Scan* scan = scanMap[scannum];
        scan->rt = alignmentQuery->floatValue("rt_updated");
        scan->originalRt = alignmentQuery->floatValue("rt_original");
    }
}

void ProjectDatabase::deleteAll()
{
    deleteAllSamples();
    deleteAllCompounds();
    deleteAllGroupsAndPeaks();
    deleteAllScans();
    deleteAllAlignmentData();
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

    vector<int> selectedGroups;
    selectedGroups.push_back(group->groupId);
    for (const auto& child : group->children)
        selectedGroups.push_back(child.groupId);

    if (selectedGroups.size() == 0)
        return;

    auto peakgroupsQuery = _connection->prepare(
                "DELETE FROM peakgroups          \
                       WHERE group_id = :group_id");

    auto peaksQuery = _connection->prepare(
                "DELETE FROM peaks               \
                       WHERE group_id = :group_id");

    for (auto id : selectedGroups) {
        peakgroupsQuery->bind(":group_id", id);
        peaksQuery->bind(":group_id", id);

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

Adduct* ProjectDatabase::_findAdductByName(string id)
{
    if (id == "[M+H]+")
        return MassCalculator::PlusHAdduct;
    else if (id == "[M-H]-")
        return MassCalculator::MinusHAdduct;
    else if (id == "[M]")
        return MassCalculator::ZeroMassAdduct;
    return nullptr;
}

Compound* ProjectDatabase::_findSpeciesById(string id, string databaseName)
{
    // load compounds from database if not already loaded.
    if (!databaseName.empty() && !_compoundDatabaseLoaded(databaseName))
        loadCompounds(databaseName);

    if (_compoundIdMap.count(id + databaseName))
        return _compoundIdMap[id + databaseName];
    if (_compoundIdMap.count(id))
        return _compoundIdMap[id];
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
        if (compound->name == name && compound->db == databaseName)
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
