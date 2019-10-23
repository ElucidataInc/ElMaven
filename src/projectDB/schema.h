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
                                      , label                   INTEGER                           \
                                      , peak_spline_area        REAL                              );"

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
                                           , min_quality                        REAL                              \
                                           , fragmentation_fraction_matched     REAL                              \
                                           , fragmentation_mz_frag_error        REAL                              \
                                           , fragmentation_hypergeom_score      REAL                              \
                                           , fragmentation_mvh_score            REAL                              \
                                           , fragmentation_dot_product          REAL                              \
                                           , fragmentation_weighted_dot_product REAL                              \
                                           , fragmentation_spearman_rank_corr   REAL                              \
                                           , fragmentation_tic_matched          REAL                              \
                                           , fragmentation_num_matches          REAL                              \
                                           , sample_ids                         TEXT                              \
                                           , slice_mz_min                       REAL                              \
                                           , slice_mz_max                       REAL                              \
                                           , slice_rt_min                       REAL                              \
                                           , slice_rt_max                       REAL                              \
                                           , slice_ion_count                    REAL                              \
                                           , table_group_id                     INTEGER                           );"

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
                                          , note                  TEXT               \
                                          , PRIMARY KEY (compound_id, name, db_name) );"

#define CREATE_ALIGNMENT_TABLE \
    "CREATE TABLE IF NOT EXISTS alignment_rts ( sample_id   INTEGER NOT NULL \
                                              , scannum     INTEGER NOT NULL \
                                              , rt_original REAL    NOT NULL \
                                              , rt_updated  REAL    NOT NULL );"

#define CREATE_SETTINGS_TABLE \
    "CREATE TABLE IF NOT EXISTS user_settings ( ionization_mode                  INTEGER \
                                              , ionization_type                  INTEGER \
                                              , instrument_type                  INTEGER \
                                              , q1_accuracy                      REAL    \
                                              , q3_accuracy                      REAL    \
                                              , filterline                       INTEGER \
                                              , centroid_scans                   INTEGER \
                                              , scan_filter_polarity             INTEGER \
                                              , scan_filter_ms_level             INTEGER \
                                              , scan_filter_min_quantile         INTEGER \
                                              , scan_filter_min_intensity        INTEGER \
                                              , upload_multiprocessing           INTEGER \
                                              , eic_smoothing_algorithm          INTEGER \
                                              , eic_smoothing_window             INTEGER \
                                              , max_rt_difference_bw_peaks       REAL    \
                                              , asls_baseline_mode               INTEGER \
                                              , baseline_quantile                INTEGER \
                                              , baseline_smoothing_window        INTEGER \
                                              , asls_smoothness                  INTEGER \
                                              , asls_asymmetry                   INTEGER \
                                              , isotope_filter_equal_peak        INTEGER \
                                              , min_signal_baseline_diff         REAl    \
                                              , min_peak_quality                 REAL    \
                                              , isotope_min_signal_baseline_diff REAL    \
                                              , isotope_min_peak_quality         REAL    \
                                              , d2_label_bpe                     INTEGER \
                                              , c13_label_bpe                    INTEGER \
                                              , n15_label_bpe                    INTEGER \
                                              , s34_label_bpe                    INTEGER \
                                              , min_isotope_parent_correlation   REAL    \
                                              , max_isotope_scan_diff            INTEGER \
                                              , abundance_threshold              REAL    \
                                              , max_natural_abundance_error      REAL    \
                                              , correct_c13_isotope_abundance    INTEGER \
                                              , eic_type                         INTEGER \
                                              , use_overlap                      INTEGER \
                                              , dist_x_weight                    INTEGER \
                                              , dist_y_weight                    INTEGER \
                                              , overlap_weight                   INTEGER \
                                              , consider_delta_rt                INTEGER \
                                              , quality_weight                   INTEGER \
                                              , intensity_weight                 INTEGER \
                                              , delta_rt_weight                  INTEGER \
                                              , mass_cutoff_type                 TEXT    \
                                              , automated_detection              INTEGER \
                                              , mass_domain_resolution           REAL    \
                                              , time_domain_resolution           INTEGER \
                                              , min_mz                           REAL    \
                                              , max_mz                           REAL    \
                                              , min_rt                           REAL    \
                                              , max_rt                           REAL    \
                                              , min_intensity                    REAL    \
                                              , max_intensity                    REAL    \
                                              , database_search                  INTEGER \
                                              , compound_extraction_window       REAL    \
                                              , match_rt                         INTEGER \
                                              , compound_rt_window               REAL    \
                                              , limit_groups_per_compound        INTEGER \
                                              , match_fragmentation              INTEGER \
                                              , min_frag_match_score             REAL    \
                                              , fragment_tolerance               REAL    \
                                              , min_frag_match                   REAL    \
                                              , report_isotopes                  INTEGER \
                                              , peak_quantitation                INTEGER \
                                              , min_group_intensity              REAL    \
                                              , intensity_quantile               INTEGER \
                                              , min_group_quality                REAL    \
                                              , quality_quantile                 INTEGER \
                                              , min_signal_blank_ratio           REAL    \
                                              , signal_blank_ratio_quantile      INTEGER \
                                              , min_signal_baseline_ratio        REAL    \
                                              , signal_baseline_ratio_quantile   INTEGER \
                                              , min_peak_width                   INTEGER \
                                              , min_good_peak_count              INTEGER \
                                              , peak_classifier_file             TEXT    \
                                              , alignment_good_peak_count        INTEGER \
                                              , alignment_limit_group_count      INTEGER \
                                              , alignment_peak_grouping_window   INTEGER \
                                              , alignemnt_wrt_expected_rt        INTEGER \
                                              , alignment_min_peak_intensity     REAL    \
                                              , alignment_min_signal_noise_ratio INTEGER \
                                              , alignment_min_peak_width         INTEGER \
                                              , alignment_peak_detection         INTEGER \
                                              , poly_fit_num_iterations          INTEGER \
                                              , poly_fit_polynomial_degree       INTEGER \
                                              , obi_warp_reference_sample        INTEGER \
                                              , obi_warp_show_advance_params     INTEGER \
                                              , obi_warp_score                   INTEGER \
                                              , obi_warp_response                REAL    \
                                              , obi_warp_bin_size                REAL    \
                                              , obi_warp_gap_init                REAL    \
                                              , obi_warp_gap_extend              REAL    \
                                              , obi_warp_factor_diag             REAL    \
                                              , obi_warp_factor_gap              REAL    \
                                              , obi_warp_no_standard_normal      REAL    \
                                              , obi_warp_local                   REAL    \
                                              , main_window_selected_db_name     TEXT    \
                                              , main_window_charge               INTEGER \
                                              , main_window_peak_quantitation    INTEGER \
                                              , main_window_mass_resolution      REAL    \
                                              , must_have_fragmentation          INTEGER \
                                              , identification_match_rt          INTEGER \
                                              , identification_rt_window         REAL    \
                                              , search_adducts                   INTEGER \
                                              , adduct_search_window             REAL    \
                                              , adduct_percent_correlation       REAL    );"

#define CREATE_COMPOUNDS_DB_INDEX \
    "CREATE INDEX IF NOT EXISTS compounds_db_idx    \
                             ON compounds ( db_name );"

#define CREATE_PEAKS_GROUP_INDEX \
    "CREATE INDEX IF NOT EXISTS peaks_group_idx  \
                             ON peaks ( group_id );"

#endif  // SCHEMA_H
