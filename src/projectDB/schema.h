#ifndef SCHEMA_H
#define SCHEMA_H

#define CREATE_SAMPLES_TABLE \
    "CREATE TABLE IF NOT EXISTS samples ( sample_id    INTEGER PRIMARY KEY AUTOINCREMENT \
                                        , name         TEXT                              \
                                        , filename     TEXT                              \
                                        , set_name     TEXT                              \
                                        , sample_order INTEGER                           \
                                        , is_blank     INTEGER                           \
                                        , is_selected  INTEGER                           \
                                        , color_red    REAL                              \
                                        , color_green  REAL                              \
                                        , color_blue   REAL                              \
                                        , color_alpha  REAL                              \
                                        , norml_const  REAL                              \
                                        , transform_a0 REAL                              \
                                        , transform_a1 REAL                              \
                                        , transform_a2 REAL                              \
                                        , transform_a4 REAL                              \
                                        , transform_a5 REAL                              );"

#define CREATE_SCANS_TABLE \
    "CREATE TABLE IF NOT EXISTS scans ( id               INTEGER PRIMARY KEY AUTOINCREMENT \
                                      , sample_id        INTEGER NOT NULL                  \
                                      , scan             INTEGER NOT NULL                  \
                                      , file_seek_start  INTEGER NOT NULL                  \
                                      , file_seek_end    INTEGER NOT NULL                  \
                                      , mslevel          INTEGER NOT NULL                  \
                                      , rt               REAL    NOT NULL                  \
                                      , precursor_mz     REAL    NOT NULL                  \
                                      , precursor_charge INTEGER NOT NULL                  \
                                      , precursor_ic     REAL    NOT NULL                  \
                                      , precursor_purity REAL                              \
                                      , minmz            REAL    NOT NULL                  \
                                      , maxmz            REAL    NOT NULL                  \
                                      , data TEXT                                          );"

#define CREATE_PEAKS_TABLE \
    "CREATE TABLE IF NOT EXISTS peaks ( peak_id                 INTEGER PRIMARY KEY AUTOINCREMENT \
                                      , group_id                INTEGER                           \
                                      , sample_id               INTEGER                           \
                                      , pos                     INTEGER                           \
                                      , minpos                  INTEGER                           \
                                      , maxpos                  INTEGER                           \
                                      , rt                      REAL                              \
                                      , rtmin                   REAL                              \
                                      , rtmax                   REAL                              \
                                      , mzmin                   REAL                              \
                                      , mzmax                   REAL                              \
                                      , scan                    INTEGER                           \
                                      , minscan                 INTEGER                           \
                                      , maxscan                 INTEGER                           \
                                      , peak_area               REAL                              \
                                      , peak_area_corrected     REAL                              \
                                      , peak_area_top           REAL                              \
                                      , peak_area_top_corrected REAL                              \
                                      , peak_area_fractional    REAL                              \
                                      , peak_rank               REAL                              \
                                      , peak_intensity          REAL                              \
                                      , peak_baseline_level     REAL                              \
                                      , peak_mz                 REAL                              \
                                      , median_mz               REAL                              \
                                      , base_mz                 REAL                              \
                                      , quality                 REAL                              \
                                      , width                   INTEGER                           \
                                      , gauss_fit_sigma         REAL                              \
                                      , gauss_fit_r2            REAL                              \
                                      , no_noise_obs            INTEGER                           \
                                      , no_noise_fraction       REAL                              \
                                      , symmetry                REAL                              \
                                      , signal_baseline_ratio   REAL                              \
                                      , group_overlap           REAL                              \
                                      , group_overlap_frac      REAL                              \
                                      , local_max_flag          REAL                              \
                                      , from_blank_sample       INTEGER                           \
                                      , label                   INTEGER                           );"

#define CREATE_PEAK_GROUPS_TABLE \
    "CREATE TABLE IF NOT EXISTS peakgroups ( group_id                           INTEGER PRIMARY KEY AUTOINCREMENT \
                                           , parent_group_id                    INTEGER                           \
                                           , meta_group_id                      INTEGER                           \
                                           , tag_string                         TEXT                              \
                                           , expected_mz                        REAL                              \
                                           , expected_abundance                 REAL                              \
                                           , expected_rt_diff                   REAL                              \
                                           , group_rank                         REAL                              \
                                           , label                              TEXT                              \
                                           , type                               INTEGER                           \
                                           , srm_id                             TEXT                              \
                                           , ms2_event_count                    INTEGER                           \
                                           , ms2_score                          REAL                              \
                                           , adduct_name                        TEXT                              \
                                           , compound_id                        TEXT                              \
                                           , compound_name                      TEXT                              \
                                           , compound_db                        TEXT                              \
                                           , table_name                         TEXT                              \
                                           , fragmentation_fraction_matched     REAL                              \
                                           , fragmentation_mz_frag_error        REAL                              \
                                           , fragmentation_hypergeom_score      REAL                              \
                                           , fragmentation_mvh_score            REAL                              \
                                           , fragmentation_dot_product          REAL                              \
                                           , fragmentation_weighted_dot_product REAL                              \
                                           , fragmentation_spearman_rank_corr   REAL                              \
                                           , fragmentation_tic_matched          REAL                              \
                                           , fragmentation_num_matches          REAL                              );"

#define CREATE_COMPOUNDS_TABLE \
    "CREATE TABLE IF NOT EXISTS compounds ( compound_id           TEXT               \
                                          , db_name               TEXT               \
                                          , name                  TEXT               \
                                          , formula               TEXT               \
                                          , smile_string          TEXT               \
                                          , srm_id                TEXT               \
                                          , mass                  REAL               \
                                          , charge                INTEGER            \
                                          , expected_rt           REAL               \
                                          , precursor_mz          REAL               \
                                          , product_mz            REAL               \
                                          , collision_energy      REAL               \
                                          , log_p                 REAL               \
                                          , virtual_fragmentation INTEGER            \
                                          , ionization_mode       INTEGER            \
                                          , category              TEXT               \
                                          , fragment_mzs          TEXT               \
                                          , fragment_intensity    TEXT               \
                                          , fragment_ion_types    TEXT               \
                                          , PRIMARY KEY (compound_id, name, db_name) );"

#define CREATE_ALIGNMENT_TABLE \
    "CREATE TABLE IF NOT EXISTS alignment_rts ( sample_id   INTEGER NOT NULL \
                                              , scannum     INTEGER NOT NULL \
                                              , rt_original REAL    NOT NULL \
                                              , rt_updated  REAL    NOT NULL );"

#define CREATE_COMPOUNDS_DB_INDEX \
    "CREATE INDEX IF NOT EXISTS compounds_db_idx    \
                             ON compounds ( db_name );"

#define CREATE_PEAKS_GROUP_INDEX \
    "CREATE INDEX IF NOT EXISTS peaks_group_idx  \
                             ON peaks ( group_id );"

#endif  // SCHEMA_H
