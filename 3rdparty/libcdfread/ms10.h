/* -*-C-*-
*******************************************************************************
*
* File:         ms10.h
* RCS:          $Header: $
* Description:  Header file for the public-domain implementation of the
*		MS netCDF Data Interchange Specification, Categories 1 & 2 
*		data elements
* Author:       David Stranz
*		Fisons Instruments
*		809 Sylvan Avenue, Suite 102
*		Modesto, CA  95350
*		Telephone:	(209) 521-0714
*		FAX:		(209) 521-9017
*		CompuServe:	70641,3057
*		Internet:	70641.3057@compuserve.com
* Created:      Tue Feb 18 13:03:15 1992
* Modified:     Tue Jul 20 10:58:05 1993 (David Stranz) dstranz@lfrg1
* Language:     C
* Package:      N/A
* Status:       Public Domain (Distribute with Copyright Notice)
*
*      (C) Copyright 1992, Analytical Instrument Association
*		           All rights reserved.
*
*******************************************************************************
*/

#ifndef	MS10_INCLUDED
#define	MS10_INCLUDED	1

/*
********************************************************************************
*
*  COPYRIGHT NOTICE
*
*  (C) Copyright 1992, Analytical Instrument Association
*      All rights reserved.
*
*  The source code in this file implements the public-domain portion of the
*  MS netCDF Data Interchange Specification for Mass Spectrometry.  This code,
*  although placed in the public domain, is copyright by the Analytical
*  Instrument Association.  It may be freely distributed, but any
*  copies or derived works must contain the copyright notice, along with these
*  paragraphs.
*
*  IMPORTANT:  Implementers of netCDF interchange programs using the code
*  in this file should do so without modifying this code if at all possible.
*  The names of dimensions, attributes, and variables, as well as the enumerated
*  values of certain attributes and variables have been agreed upon by a
*  consortium of manufacturers and end users from throughout the mass 
*  spectrometry community.  Many of the identifiers have been directly copied
*  from other implementations by the Analytical Instrument Association (AIA)
*  for chromatographic data interchange, and from the Analytical Data 
*  Interchange and Storage Standards (ADISS) Project for general analytical
*  data.  Changing them will almost certainly result in incompatibility between
*  files produced using differing implementations, and the inability to
*  interchange data, which after all, is the whole reason behind doing this.
*
*  Manufacturers or other implementers can *extend* the MS specification
*  to accomodate individual needs.  This can be done by defining dimensions,
*  attributes, or variables *in addition to* those defined by the MS
*  specification.  It is strongly suggested that these additional identifiers
*  have names which begin with a unique prefix, preferably something which
*  indicates the source of the new identifier, such as:
*
*	hp_dimension  or
*	:fisons_attribute  or
*	short finnigan_array(finnigan_dimension)  (you get the idea)
*
*  Extending the MS specification in this way is harmless - the new
*  identifiers are ignored by any application not looking for them, and
*  using manufacturer-specific prefixes ensures that new names will not
*  accidently be confused with identifiers added by someone else.
*
*  Any changes to the MS specified part of this implementation *must*
*  be reviewed and approved by the MS data interchange consortium.  Such
*  requests should be submitted to David Stranz, Fisons Instruments,
*  809 Sylvan Avenue, Suite 102, Modesto, CA  95350.
*
********************************************************************************
*/

/*
********************************************************************************
*
*  Standard definitions for the MS Interchange
*
********************************************************************************
*/

/* Pointer, floating point, and integer elements in data structures are
   initialized to these values for output.  On output, any field which
   contains one of these values will not be written to the netCDF file.

   For input, these values in a data field indicate that the netCDF file 
   did not contain a value for the element.  

   In other words, they are the default NULL values for (char *) pointers,
   (float) or (double) variables, or (int) or (long) variables, respectively. */

#ifndef	NULL
#define	NULL			((void *) 0)
#endif
#define	MS_NULL_FLT		((float) -9999.0)
#define	MS_NULL_INT		(-9999)
#define	MS_NULL_BYTE		(255)

#ifndef	TRUE
#define	TRUE			(1)
#endif

#ifndef	FALSE
#define	FALSE			(0)
#endif

#define	MS_ERROR		(-1)	/* Returned for all errors	*/
#define	MS_NO_ERROR		(0)	/* Returned when successful	*/

#define	MS_INST_LENGTH		(32)	/* Length of INSTRUMENT-ID data
					   strings, including NULL	*/

#define	MS_MAX_STRING_LENGTH	(255)	/* Maximum allowed length of string
					   variables and attributes	*/

#define	MS_STAMP_LENGTH		(20)	/* Length of a time stamp string,
					   including NULL		*/

/*
********************************************************************************
*
*  MS enumerated types:  Each MS data element which has an enumerated set
*  of possible values has a type defined to represent that set.  Enumerated
*  constants select among the various members of the set.
*
*  So that lookup and translation of enumerated constants to string literals
*  can be easily supported, the enumerated constants are sequential, beginning
*  with zero.  This requires an ordering of the typedefs which appear below;
*  do not change the order without revising the initializations of constant
*  values.
*
********************************************************************************
*/

typedef enum {		/* Experiment types				*/
   expt_centroid = 0,			/* Centroided Mass Spectrum	*/
   expt_continuum,			/* Continuum Mass Spectrum	*/
   expt_library				/* Library Mass Spectrum	*/
   } ms_admin_expt_t;

typedef	enum {		/* Sample states				*/
   state_solid = (int) expt_library + 1,	/* Solid		*/
   state_liquid,			/* Liquid			*/
   state_gas,				/* Gas				*/
   state_supercrit,			/* Supercritical Fluid		*/
   state_plasma,			/* Plasma			*/
   state_other				/* Other			*/
   } ms_sample_state_t;

typedef enum {		/* Separation experiments			*/
   separation_glc = (int) state_other + 1,
   /* Gas-Liquid Chromatography		*/
   separation_gsc,		/* Gas-Solid Chromatography		*/
   separation_nplc,		/* Normal Phase Liquid Chromatography	*/
   separation_rplc,		/* Reverse Phase Liquid Chromatography	*/
   separation_ielc,		/* Ion Exchange Liquid Chromatography	*/
   separation_selc,		/* Size Exclusion Liquid Chromatography	*/
   separation_iplc,		/* Ion Pair Liquid Chromatography	*/
   separation_olc,		/* Other Liquid Chromatography		*/
   separation_sfc,		/* Supercritical Fluid Chromatography	*/
   separation_tlc,		/* Thin Layer Chromatography		*/
   separation_fff,		/* Field Flow Fractionation		*/
   separation_cze,		/* Capillary Zone Electrophoresis	*/
   separation_other,		/* Other Chromatography			*/
   separation_none		/* No Chromatography			*/
   } ms_test_separation_t;


typedef enum {		/* Mass spectrometer inlet types		*/
   inlet_membrane = (int) separation_none + 1,
   				/* Membrane Separator			*/
   inlet_capillary,		/* Capillary Direct			*/
   inlet_opensplit,		/* Open Split				*/
   inlet_jet,			/* Jet Separator			*/
   inlet_direct,		/* Direct Inlet Probe			*/
   inlet_septum,		/* Septum				*/
   inlet_pb,			/* Particle Beam			*/
   inlet_reservoir,		/* Reservoir				*/
   inlet_belt,			/* Moving Belt				*/
   inlet_apci,			/* Atmospheric Pressure Chemical Ionization */
   inlet_fia,			/* Flow Injection Analysis		*/
   inlet_es,			/* Electrospray				*/
   inlet_infusion,		/* Infusion				*/
   inlet_ts,			/* Thermospray				*/
   inlet_probe,			/* Other Probe				*/
   inlet_other			/* Other				*/
   } ms_test_inlet_t;

typedef enum {		/* Ionization modes				*/
   ionization_ei = (int) inlet_other + 1,
   				/* Electron Impact			*/
   ionization_ci,		/* Chemical Ionization			*/
   ionization_fab,		/* Fast Atom Bombardment		*/
   ionization_fd,		/* Field Desorption			*/
   ionization_fi,		/* Field Ionization			*/
   ionization_es,		/* Electrospray				*/
   ionization_ts,		/* Thermospray				*/
   ionization_apci,		/* Atmospheric Pressure Chemical Ionization */
   ionization_pd,		/* Plasma Desorption			*/
   ionization_ld,		/* Laser Desorption			*/
   ionization_spark,		/* Spark Ionization			*/
   ionization_thermal,		/* Thermal Ionization			*/
   ionization_other		/* Other				*/
   } ms_test_ioniz_t;

typedef enum {		/* Ionization polarities			*/
   polarity_plus = (int) ionization_other + 1,
				   /* Positive				*/
   polarity_minus		/* Negative				*/
   } ms_test_polarity_t;

typedef enum {		/* Detector types				*/
   detector_em = (int) polarity_minus + 1,
				/* Electron Multiplier			*/
   detector_pm,			/* Photomultplier			*/
   detector_focal,		/* Focal Plane Array			*/
   detector_cup,		/* Faraday Cup				*/
   detector_dynode_em,		/* Conversion Dynode Electron Multiplier */
   detector_dynode_pm,		/* Conversion dynode Photomultiplier	*/
   detector_multicoll,		/* Multicollector			*/
   detector_other		/* Other				*/
   } ms_test_detector_t;

/* Resolution types; only these two are permitted.  Constant resolution
   implies constant absolute mass resolution across the mass axis, such
   as is found in quadrupoles; Proportional resolution implies constant
   proportional (mass / delta mass) resolution across the mass axis, as
   is found in magnetic sectors.					*/

typedef enum {
   resolution_constant = (int) detector_other + 1,
   				/* Constant				*/
   resolution_proportional	/* Proportional				*/
   } ms_test_res_t;

typedef enum {		/* Scan types					*/
   function_scan = (int) resolution_proportional + 1,
				/* Mass Scan				*/
   function_sid,		/* Selected Ion Detection		*/
   function_other		/* Other				*/
   } ms_test_function_t;

/* Scan direction; "other" should be used for non-monotonic scans.  Up
   means from low to high mass						*/

typedef enum {
   direction_up = (int) function_other + 1,
   				/* Up					*/
   direction_down,		/* Down					*/
   direction_other		/* Other				*/
   } ms_test_direction_t;

typedef enum {		/* Scan laws					*/
   law_linear = (int) direction_other + 1,
   				/* Linear				*/
   law_exponential,		/* Exponential				*/
   law_quadratic,		/* Quadratic				*/
   law_other			/* Other				*/
   } ms_test_law_t;

/* The mass, time, and intensity axes can each be stored in one of 
   four formats to best match the original precision and
   conserve space in the netCDF file.
   */

typedef enum {		/* Raw data format				*/
   data_short = (int) law_other + 1,
   				/* Short  (16-bit integer)		*/
   data_long,			/* Long	  (32-bit integer)		*/
   data_float,			/* Float  (IEEE 32-bit float)		*/
   data_double			/* Double (IEEE 64-bit float)		*/
   } ms_data_format_t;

/* Mass, time and intensity axes units have not been formally defined 
   in the MS specification.  These are some suggestions, and may change
   in future revisions.							*/

typedef enum {		/* Units for the mass axis			*/
   mass_m_z = (int) data_double + 1,
   				/* M/Z					*/
   mass_arbitrary,		/* Arbitrary units			*/
   mass_other			/* Other				*/
   } ms_data_mass_t;

typedef enum {		/* Units for the time axis			*/
   time_seconds = (int) mass_other + 1,
   				/* Seconds				*/
   time_arbitrary,		/* Arbitrary units			*/
   time_other			/* Other				*/
   } ms_data_time_t;

typedef enum {		/* Units for the intensity axis; these same units
			   also apply to total intensity		*/
   intensity_counts = (int) time_other + 1,
   				/* Total counts				*/
   intensity_cps,		/* Counts per second			*/
   intensity_volts,		/* Volts				*/
   intensity_current,		/* Current				*/
   intensity_arbitrary,		/* Arbitrary units			*/
   intensity_other		/* Other				*/
   } ms_data_intensity_t;

/*
********************************************************************************
*
*  Date/time stamp data structure; used for date/time conversions to
*  ISO 3307 format.
*
********************************************************************************
*/

typedef struct {
   char *		string;		/* input/output string		*/
   int			year;
   int			month;
   int			day;
   int			hour;
   int			minute;
   int			second;
   int			differential;
} MS_Date_Time;

/*
********************************************************************************
*
*  ADMINISTRATIVE-DATA Information Class - Category 1 & 2 Data Elements
*
********************************************************************************
*/

typedef struct {
   char *		dataset_completeness;
   char *		ms_template_revision;
   char *		netcdf_revision;
   char *		languages;
   char *		comments;
   char *		dataset_origin;
   char *		dataset_owner;
   char *		netcdf_date_time;
   char *		experiment_title;
   char *		experiment_date_time;
   char *		experiment_x_ref_0;
   char *		experiment_x_ref_1;
   char *		experiment_x_ref_2;
   char *		experiment_x_ref_3;
   char *		operator_name;
   char *		pre_expt_program_name;
   char *		post_expt_program_name;
   char *		source_file_reference;
   char *		source_file_format;
   char *		source_file_date_time;
   char *		external_file_ref_0;
   char *		external_file_ref_1;
   char *		external_file_ref_2;
   char *		external_file_ref_3;
   char *		calibration_history_0;
   char *		calibration_history_1;
   char *		calibration_history_2;
   char *		calibration_history_3;
   char *		error_log;
   ms_admin_expt_t	experiment_type;
   long			number_times_processed;
   long			number_times_calibrated;
   long			number_instrument_components;
} MS_Admin_Data;

/*
********************************************************************************
*
*  INSTRUMENT-ID Information Class - Category 1 through 5 Data Elements
*
********************************************************************************
*/

/* Note that the contents or format of the string fields below have
   not been defined in the MS proposal.  The structure definition has been
   provided for completeness only.					*/

typedef struct {
   long			inst_no;
   char *		name;
   char *		id;
   char *		manufacturer;
   char *		model_number;
   char *		serial_number;
   char *		software_version;
   char *		firmware_version;
   char *		operating_system;
   char *		application_software;
   char *		comments;
} MS_Instrument_Data;

/*
********************************************************************************
*
*  SAMPLE-DESCRIPTION Information Class - Category 1 & 2 Data Elements
*
********************************************************************************
*/

typedef	struct {
   char *		internal_id;
   char *		external_id;
   char *		receipt_date_time;
   char *		owner;
   char *		procedure_name;
   char *		matrix;
   char *		storage;
   char *		disposal;
   char *		history;
   char *		prep_procedure;
   char *		prep_comments;
   char *		manual_handling;
   char *		comments;
   ms_sample_state_t	state;
} MS_Sample_Data;

/*
********************************************************************************
*
*  TEST-METHOD Information Class - Category 1 & 2 Data Elements
*
********************************************************************************
*/

typedef	struct {
   ms_test_separation_t		separation_type;
   ms_test_inlet_t		ms_inlet;
   float			ms_inlet_temperature;
   ms_test_ioniz_t		ionization_mode;
   ms_test_polarity_t		ionization_polarity;
   float			electron_energy;
   float			laser_wavelength;
   char *			reagent_gas;
   float			reagent_gas_pressure;
   char *			fab_type;
   char *			fab_matrix;
   float			source_temperature;
   float			filament_current;
   float			emission_current;
   float			accelerating_potential;
   ms_test_detector_t		detector_type;
   float			detector_potential;
   float			detector_entrance_potential;
   ms_test_res_t		resolution_type;
   char *			resolution_method;
   ms_test_function_t		scan_function;
   ms_test_direction_t		scan_direction;
   ms_test_law_t		scan_law;
   float			scan_time;
   char *			mass_calibration_file;
   char *			external_reference_file;
   char *			internal_reference_file;
   char *			comments;
} MS_Test_Data;

/*
********************************************************************************
*
*  RAW-DATA Information Class - Category 1 & 2 Data Elements
*
********************************************************************************
*/

/* The "has_masses" and "has_times" flags are used only when reading netCDF
   files, and are set to TRUE if the respective data is present in the
   file.								*/

typedef struct {
   long			nscans;		/* Number of scans in data set	*/
   int			has_masses;	/* TRUE if masses present in file */
   int			has_times;	/* TRUE if times present in file  */
   double		mass_factor;
   double		time_factor;
   double		intensity_factor;
   double		intensity_offset;
   ms_data_mass_t	mass_units;
   ms_data_time_t	time_units;
   ms_data_intensity_t	intensity_units;
   ms_data_intensity_t	total_intensity_units;
   ms_data_format_t	mass_format;
   ms_data_format_t	time_format;
   ms_data_format_t	intensity_format;
   char *		mass_label;
   char *		time_label;
   char *		intensity_label;
   long			starting_scan_number;
   double		mass_axis_global_min;
   double		mass_axis_global_max;
   double		time_axis_global_min;
   double		time_axis_global_max;
   double		intensity_axis_global_min;
   double		intensity_axis_global_max;
   double		calibrated_mass_min;
   double		calibrated_mass_max;
   double		run_time;
   double		delay_time;
   short		uniform_flag;
   char *		comments;
} MS_Raw_Data_Global;

/* Note: this data is provided on a per-scan basis.  The arrays of mass,
   time, and intensity values must be the same size, and have a one-to-one
   correspondence between them (i.e. one mass, time, and intensity triplet
   for each datum point).  Mass or time data may be both present, or only mass
   or only time data.  WHATEVER IS PRESENT, THE SAME DATA MUST BE PRESENT
   FOR EVERY SCAN IN THE FILE.  If one abcissa type is missing, the pointer
   may be set to NULL.  The arrays are declared as void * to allow for
   variable typing (they may actually be short, long, float, or double,
   depending on the data format indicated in the MS_Raw_Data_Global fields.

   Flag_masses and flag_values arrays are optional.  If no peaks are flagged
   for a given scan, then NULL pointers may be passed.  Otherwise, these
   arrays are filled pairwise with the mass/time and flag value for those peaks
   which are flagged.  Peaks which have no flags need not be included.

   The following rules determine whether masses or times are passed in the
   flag_peaks array:
   (1) If both times and masses are recorded for scan data, then MASSES
       must be passed;
   (2) If times only are recorded, then TIMES must be passed; or,
   (3) If masses only are recorded, then MASSES must be passed.
   In every case, the flag_paeks array must be of the same data type as the
   masses or times array as appropriate.  The flag_values array MUST be
   of the same data type as the intensities array (since it is physically
   written to the netCDF file by concatenating it to the end of the
   intensities data for each scan, and therefore must be of the same type).

   Flags values are listed below.  If a peak has more than one flag,
   then a composite flag is constructed using the logical OR of the
   separate flags.  All of the peaks in the flag_peakss array MUST be present
   in the masses or times array for the scan; unpredictable results will occur
   if there is a mismatch.

   For each scan, the scan_no, points, and flags fields MUST have
   valid values.  If there are no peaks or flags for a given scan,
   pass zeros in those fields.  An entry must be made in the netCDF file
   for every scan, whether it has peaks or not.				*/

typedef struct {
   long			scan_no;	/* Index number of this scan	*/
   long			points;		/* Number of points in this scan*/
   long			flags;		/* Number of flagged peaks	*/
   long			actual_scan_no;	/* Data file scan number	*/
   double		total_intensity;
   double		a_d_rate;
   short		a_d_coadditions;
   double		scan_acq_time;
   double		scan_duration;
   double		inter_scan_time;
   double		mass_range_min;
   double		mass_range_max;
   double		time_range_min;
   double		time_range_max;
   double		resolution;
   void *		masses;		/* Array of masses		*/
   void *		times;		/* Array of times		*/
   void *		intensities;	/* Array of intensities		*/
   long *		flag_peaks;	/* Array of flagged peaks	*/
   short *		flag_values;	/* Array of flag values		*/
} MS_Raw_Per_Scan;

/* The following struct is used for SIR/SIM/MID-type data to record the
   scan group information.  The three arrays are parallel - that is, for
   each mass, there is a corresponding sampling time (how long that mass
   was monitored during the scan) and delay time (how long it took to
   get to the next monitored mass).  All times are in milliseconds.

   On writing, the arrays are assumed to be dimensioned to "mass_count"
   length.  On reading, the arrays are dynamically allocated to a length
   appropriate to the number of masses in the scan group being read.	*/

typedef struct {
   long			group_no;	/* Index number of this group	*/
   long			mass_count;	/* Number of masses in group	*/
   long			starting_scan;	/* Group starts with this scan	*/
   double *		masses;		/* Array of masses		*/
   double *		sampling_times;	/* Sampling time for each mass	*/
   double *		delay_times;	/* Inter-mass delay time	*/
} MS_Raw_Per_Group;

/* Note:  For library data, in addition to the per-scan raw data, the following
   data is provided for each library spectrum.				*/

/* Note:  The specification provides for any number of additional names;
   this is difficult to implement in netCDF, so in this present implementation,
   only four other additional names fields are provided.		*/

typedef struct {
   long			scan_no;
   char *		entry_name;
   char *		entry_id;
   long			entry_number;
   char *		source_data_file_reference;
   char *		cas_name;
   long			cas_number;
   char *		other_name_0;
   char *		other_name_1;
   char *		other_name_2;
   char *		other_name_3;
   char *		formula;
   char *		smiles;
   char *		wiswesser;
   char *		molfile_reference;
   char *		other_structure;
   double		retention_index;
   char *		retention_type;
   double		absolute_retention;
   double		relative_retention;
   char *		retention_reference;
   long			retention_cas;
   float		mp;
   float		bp;
   double		chemical_mass;
   long			nominal_mass;
   double		accurate_mass;
   char *		other_info;
} MS_Raw_Library;

/*
********************************************************************************
*
*  Peak flags - these may be "ORed" together to yield a composite flag
*
********************************************************************************
*/

#define	MS_FLAG_NOT_HRP		(1L << 0 )
#define	MS_FLAG_MISSED_REF	(1L << 1 )
#define	MS_FLAG_UNRESOLVED	(1L << 2 )
#define	MS_FLAG_DBL_CHARGED	(1L << 3 )
#define	MS_FLAG_REFERENCE	(1L << 4 )
#define	MS_FLAG_EXCEPTION	(1L << 5 )
#define	MS_FLAG_SATURATED	(1L << 6 )
#define	MS_FLAG_SIGNIFICANT	(1L << 7 )
#define	MS_FLAG_MERGED		(1L << 8 )
#define	MS_FLAG_FRAGMENTED	(1L << 9 )
#define	MS_FLAG_AREA_HEIGHT	(1L << 10)
#define	MS_FLAG_MATH_MODIFIED	(1L << 11)
#define	MS_FLAG_NEGATIVE	(1L << 12)
#define	MS_FLAG_EXTENDED	(1L << 13)
#define	MS_FLAG_CALCULATED	(1L << 14)
#define	MS_FLAG_LOCK_MASS	(1L << 15)

/*
********************************************************************************
*
*  Exported functions
*
********************************************************************************
*/

#ifdef	__STDC__
#ifdef	__cplusplus
extern "C" {
#endif	/* __cplusplus */

/*  From ms10aux.c:							*/

int	ms_read_enum_attribute( int, int, char * );
char *	ms_read_string_variable( int, int, long, long, int );
int	ms_write_string_variable( int, int, long, long, int, char * );
int	ms_open_write( char *, ms_admin_expt_t, long, long,
		       ms_data_format_t, ms_data_format_t, ms_data_format_t,
		       int, int );
int	ms_open_read( char * );
void	ms_close( int );
int	ms_associate_id( int );
int	ms_dissociate_id( int );
int	ms_convert_date( int, MS_Date_Time * );
void	ms_copy_array( void *, ms_data_format_t, long, void *, 
		       ms_data_format_t, int );

/*  From ms10enum.c:							*/

char *	ms_enum_to_string( int );
int	ms_string_to_enum( char * );

/*  From ms10io.c:							*/

int	ms_write_dimensions( int, long, long );
int	ms_read_dimensions( int );

int	ms_write_variables( int, ms_admin_expt_t, ms_data_format_t,
			    ms_data_format_t, ms_data_format_t, int, int );
int	ms_read_variables( int );

int	ms_write_global( int, MS_Admin_Data *, MS_Sample_Data *,
			 MS_Test_Data *, MS_Raw_Data_Global * );
int	ms_read_global( int, MS_Admin_Data *, MS_Sample_Data *,
			 MS_Test_Data *, MS_Raw_Data_Global * );
void	ms_init_global( int, MS_Admin_Data *, MS_Sample_Data *,
			 MS_Test_Data *, MS_Raw_Data_Global * );

int	ms_write_instrument( int, MS_Instrument_Data * );
int	ms_read_instrument( int, MS_Instrument_Data * );
void	ms_init_instrument( int, MS_Instrument_Data * );

int	ms_write_per_scan( int, MS_Raw_Per_Scan *, MS_Raw_Library * );
int	ms_read_per_scan( int, MS_Raw_Per_Scan *, MS_Raw_Library * );
void	ms_init_per_scan( int, MS_Raw_Per_Scan *, MS_Raw_Library * );

int	ms_write_group_global( int, long, long );
int	ms_read_group_global( int, long *, long * );

int	ms_write_per_group( int, MS_Raw_Per_Group * );
int	ms_read_per_group( int, MS_Raw_Per_Group * );
void	ms_init_per_group( int, MS_Raw_Per_Group * );

int	ms_read_TIC( int, long *, double * *, double * * );

#ifdef	__cplusplus
}
#endif	/* __cplusplus */

#else	/* __STDC__ */

/*  From ms10aux.c:							*/

int	ms_read_enum_attribute();
char *	ms_read_string_variable();
int	ms_write_string_variable();
int	ms_open_write();
int	ms_open_read();
void	ms_close();
int	ms_associate_id();
int	ms_dissociate_id();
int	ms_convert_date();
void	ms_copy_array();

/*  From ms10enum.c:							*/

char *	ms_enum_to_string();
int	ms_string_to_enum();

/*  From ms10io.c:							*/

int	ms_write_dimensions();
int	ms_read_dimensions();

int	ms_write_variables();
int	ms_read_variables();

int	ms_write_global();
int	ms_read_global();
void	ms_init_global();

int	ms_write_instrument();
int	ms_read_instrument();
void	ms_init_instrument();

int	ms_write_per_scan();
int	ms_read_per_scan();
void	ms_init_per_scan();

int	ms_write_group_global();
int	ms_read_group_global();

int	ms_write_per_group();
int	ms_read_per_group();
void	ms_init_per_group();

int	ms_read_TIC();

#endif	/* __STDC__ */ 

#endif	/* MS10_INCLUDED */
/* DON'T ADD ANYTHING AFTER THIS #endif */
