/* -*-C-*-
*******************************************************************************
*
* File:         ms10io.h
* RCS:          $Header: $
* Description:  Public-domain implementation of the AIA MS Version 1.0.1 Data
*		Interchange Specification, Categories 1 and 2 data elements.
*		This file is a header file for the netCDF file I/O functions
*		of the MS specification.
* Author:       David Stranz
*		Fisons Instruments
*		809 Sylvan Avenue, Suite 102
*		Modesto, CA  95350
*		Telephone:	(209) 521-0714
*		FAX:		(209) 521-9017
*		CompuServe:	70641,3057
*		Internet:	70641.3057@compuserve.com
* Created:      Thu Feb 20 15:00:16 1992
* Modified:     Wed Jul 14 11:50:38 1993 (David Stranz) dstranz@lfrg1
* Language:     C
* Package:      N/A
* Status:       Public Domain (Distribute with Copyright Notice)
*
*      (C) Copyright 1992, 1993, Analytical Instrument Association
*		           All rights reserved.
*
*******************************************************************************
*/

/*
********************************************************************************
*
*  COPYRIGHT NOTICE
*
*  (C) Copyright 1992, 1993  Analytical Instrument Association
*      All rights reserved.
*
*  The source code in this file implements the public-domain portion of the
*  AIA MS Data Interchange Specification for Mass Spectrometry, Version 1.0.1.
*  This code, although placed in the public domain, is copyright by the
*  Analytical Instrument Association.  It may be freely distributed, but any
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

#ifndef	MS10_IO_INCLUDED
#define	MS10_IO_INCLUDED	1

/* THIS FILE SHOULD BE INCLUDED *ONLY* BY FILE ms10io.c !!		*/

/*
********************************************************************************
*
*  Global, static variables
*
********************************************************************************
*/

static char *	ms_completeness_att	= "C1+C2";
static char *	ms_template_att		= "1.0.1";
static char *	ms_netcdf_att		= "2.3.2";
static char *	ms_languages_att	= "English";

/*  This definition is for those compilers which do not supply one (usually
    supplied in stddef.h, but it may be absent in non-ANSI compilers).*/

#ifndef	offsetof
#define	offsetof(s_name,m_name)		(long)(&(((s_name*)0))->m_name)
#endif

/*
********************************************************************************
*
*  MS_Dimensions data structure:  This keeps track of all the dimension id
*  numbers used when reading or writing netCDF files.
*
********************************************************************************
*/

typedef struct {
   int	id;				/* dimension id			*/
   long	size;				/* dimension size		*/
} ms_dim;

typedef	struct {
   ms_dim	_2_byte_dim;
   ms_dim	_4_byte_dim;
   ms_dim	_8_byte_dim;
   ms_dim	_16_byte_dim;
   ms_dim	_32_byte_dim;
   ms_dim	_64_byte_dim;
   ms_dim	_128_byte_dim;
   ms_dim	_255_byte_dim;
   ms_dim	range_dim;
   ms_dim	scan_number_dim;
   ms_dim	point_number_dim;
   ms_dim	instrument_number_dim;
   ms_dim	group_number_dim;
   ms_dim	group_max_masses_dim;
   ms_dim	error_number_dim;
} MS_Dimensions;

/*
********************************************************************************
*
*  MS_Variables data structure:  Keeps track of the netCDF variable ids.
*  The data structure member names might seem a bit long, but they are what
*  the "ncgen" utility would produce from the CDL file.
*
********************************************************************************
*/

typedef struct {
   int	error_log_id;
   int	instrument_name_id;
   int	instrument_id_id;
   int	instrument_mfr_id;
   int	instrument_model_id;
   int	instrument_serial_no_id;
   int	instrument_sw_version_id;
   int	instrument_fw_version_id;
   int	instrument_os_version_id;
   int	instrument_app_version_id;
   int	instrument_comments_id;
   int	scan_index_id;
   int	point_count_id;
   int	flag_count_id;
   int	mass_values_id;
   int	time_values_id;
   int	intensity_values_id;
   int	a_d_sampling_rate_id;
   int	a_d_coaddition_factor_id;
   int	scan_acquisition_time_id;
   int	scan_duration_id;
   int	inter_scan_time_id;
   int	resolution_id;
   int	actual_scan_id;
   int	total_intensity_id;
   int	mass_range_min_id;
   int	mass_range_max_id;
   int	time_range_min_id;
   int	time_range_max_id;
   int	group_mass_count_id;
   int	group_start_id;
   int	group_masses_id;
   int	group_samplings_id;
   int	group_delays_id;
   int	entry_name_id;
   int	entry_id_id;
   int	entry_number_id;
   int	source_data_file_id;
   int	CAS_name_id;
   int	CAS_number_id;
   int	other_name_0_id;
   int	other_name_1_id;
   int	other_name_2_id;
   int	other_name_3_id;
   int	chemical_formula_id;
   int	wiswesser_id;
   int	smiles_id;
   int	molfile_id;
   int	other_structure_id;
   int	retention_index_id;
   int	retention_type_id;
   int	absolute_retention_id;
   int	relative_retention_id;
   int	retention_reference_name_id;
   int	retention_reference_CAS_id;
   int	melting_point_id;
   int	boiling_point_id;
   int	chemical_mass_id;
   int	nominal_mass_id;
   int	accurate_mass_id;
   int	entry_other_information_id;
} MS_Variables;

/*
********************************************************************************
*
*  Client data structure: each time an AIA MS file is opened, a new "client"
*  is registered for that file, associating the netCDF file ID with dimension,
*  variable, data format, and other information.
*
*  The array, ms_clients, and counter (ms_client_count) keep track of
*  client information and are dynamically adjusted.  The function
*  ms_associate_id() adds a new client, and ms_dissociate_id() removes it.
*
********************************************************************************
*/

typedef struct {
   int			cdfid;
   ms_admin_expt_t	expt_type;
   ms_data_format_t	mass_type;
   ms_data_format_t	time_type;
   ms_data_format_t	inty_type;
   int			has_masses;
   int			has_times;
   MS_Dimensions	dims;
   MS_Variables		vars;
   long			total_count;
}	MS_Client_Data;

static	MS_Client_Data * *	ms_clients = NULL;
static	int			ms_client_count = 0;

/*
********************************************************************************
*
*  Datatype to format conversion table
*
********************************************************************************
*/

static struct {
   nc_type		cdf_type;
   ms_data_format_t	ms_fmt;
} ms_types[] = { { NC_SHORT,	data_short	},
		 { NC_LONG,	data_long	},
		 { NC_FLOAT,	data_float	},
		 { NC_DOUBLE,	data_double	} };

static int	nTypes = 4;

/*
********************************************************************************
*
*  Data structures
*
*  Most netCDF dimension, variable, and attribute definitions are presented
*  as arrays of data, defined by the three structures below.  This affords more
*  easily maintainable code and avoids lengthy subroutines with explicit
*  calls to create or retrieve each element.
*
*  There are a few exceptions to this table-driven algorithm; these are clearly
*  identified in the code.
*
********************************************************************************
*/

typedef struct {
   unsigned long	offset;		/* offset of dimension id	*/
   char *		name;		/* dimension name		*/
   long			size;		/* size of dimension		*/
} MS_Dimension_Data;

typedef struct {
   unsigned long	offset;		/* offset of variable id	*/
   char *		name;		/* name of variable		*/
   nc_type		datatype;	/* type of variable		*/
   long			data_offset;	/* offset of data element	*/
   int			ndim;		/* number of dimensions	(0 - 2)	*/
   long			dim[2];		/* dimensions ids		*/
} MS_Variable_Data;

typedef	struct {
   unsigned long	offset;		/* offset of data field		*/
   char *		name;		/* field name			*/
   long			id;		/* variable id (or NC_GLOBAL)	*/
   nc_type		datatype;	/* variable data type		*/
   int			mandatory;	/* non-zero if a required field	*/
   int			enumerated;	/* non-zero if an enum type	*/
   int			default_value;	/* for enum types only		*/
} MS_Attribute_Data;

/*
********************************************************************************
*
*  MS_Dimension_Data arrays
*
********************************************************************************
*/

/*  Dimensions with implementation-defined sizes are included in this table.
    Those which are data-dependent (e.g. number of scans) are explicitly
    created in code.							*/

static MS_Dimension_Data ms_dimensions[] = {
{ offsetof( MS_Dimensions,   _2_byte_dim ),   "_2_byte_string",   2L },
{ offsetof( MS_Dimensions,   _4_byte_dim ),   "_4_byte_string",   4L },
{ offsetof( MS_Dimensions,   _8_byte_dim ),   "_8_byte_string",   8L },
{ offsetof( MS_Dimensions,  _16_byte_dim ),  "_16_byte_string",  16L },
{ offsetof( MS_Dimensions,  _32_byte_dim ),  "_32_byte_string",  32L },
{ offsetof( MS_Dimensions,  _64_byte_dim ),  "_64_byte_string",  64L },
{ offsetof( MS_Dimensions, _128_byte_dim ), "_128_byte_string", 128L },
{ offsetof( MS_Dimensions, _255_byte_dim ), "_255_byte_string", 255L },
{ offsetof( MS_Dimensions, range_dim ), "range", 2L },
{ offsetof( MS_Dimensions, point_number_dim ), "point_number", NC_UNLIMITED },
{ offsetof( MS_Dimensions, error_number_dim ), "error_number", 1L }
};

static int nDims = (sizeof( ms_dimensions ) / sizeof( MS_Dimension_Data ));

/*
********************************************************************************
*
*  MS Variable data arrays
*
********************************************************************************
*/

/*  Error log								*/

static MS_Variable_Data error_variables[] = {
{ offsetof( MS_Variables, error_log_id ), "error_log", NC_CHAR, 
     (long)offsetof( MS_Admin_Data, error_log ), 2,
  {  (long)offsetof( MS_Dimensions, error_number_dim ),
	(long)offsetof( MS_Dimensions, _64_byte_dim ) }}
};

static int nError = sizeof( error_variables ) / sizeof( MS_Variable_Data );

/*  Raw data per-scan variables						*/

static MS_Variable_Data raw_variables[] = {
{ offsetof( MS_Variables, a_d_sampling_rate_id ), 
     "a_d_sampling_rate", NC_DOUBLE,
     (long)offsetof( MS_Raw_Per_Scan, a_d_rate ), 1,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ), -1 } },
{ offsetof( MS_Variables, a_d_coaddition_factor_id ), 
     "a_d_coaddition_factor", NC_SHORT,
     (long)offsetof( MS_Raw_Per_Scan, a_d_coadditions ), 1,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ), -1 } },
{ offsetof( MS_Variables, scan_acquisition_time_id ), 
     "scan_acquisition_time", NC_DOUBLE,
     (long)offsetof( MS_Raw_Per_Scan, scan_acq_time ), 1,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ), -1 } },
{ offsetof( MS_Variables, scan_duration_id ), 
     "scan_duration", NC_DOUBLE, 
     (long)offsetof( MS_Raw_Per_Scan, scan_duration ), 1,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ), -1 } },
{ offsetof( MS_Variables, inter_scan_time_id ), 
     "inter_scan_time", NC_DOUBLE, 
     (long)offsetof( MS_Raw_Per_Scan, inter_scan_time ), 1,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ), -1 } },
{ offsetof( MS_Variables, resolution_id ), 
     "resolution", NC_DOUBLE, 
     (long)offsetof( MS_Raw_Per_Scan, resolution ), 1,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ), -1 } },
{ offsetof( MS_Variables, actual_scan_id ), 
     "actual_scan_number", NC_LONG, 
     (long)offsetof( MS_Raw_Per_Scan, actual_scan_no ), 1,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ), -1 } },
{ offsetof( MS_Variables, total_intensity_id ), 
     "total_intensity", NC_DOUBLE, 
     (long)offsetof( MS_Raw_Per_Scan, total_intensity ), 1,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ), -1 } },
{ offsetof( MS_Variables, mass_range_min_id ), "mass_range_min", NC_DOUBLE, 
     (long)offsetof( MS_Raw_Per_Scan, mass_range_min ), 1,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ), -1 } },
{ offsetof( MS_Variables, mass_range_max_id ), "mass_range_max", NC_DOUBLE, 
     (long)offsetof( MS_Raw_Per_Scan, mass_range_max ), 1,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ), -1 } },
{ offsetof( MS_Variables, time_range_min_id ), "time_range_min", NC_DOUBLE, 
     (long)offsetof( MS_Raw_Per_Scan, time_range_min ), 1,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ), -1 } },
{ offsetof( MS_Variables, time_range_max_id ), "time_range_max", NC_DOUBLE, 
     (long)offsetof( MS_Raw_Per_Scan, time_range_max ), 1,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ), -1 } }
};

static int nRawP = sizeof( raw_variables ) / sizeof( MS_Variable_Data );

/* Raw data per scan group variables					*/

static MS_Variable_Data group_variables[] = {
{ offsetof( MS_Variables, group_mass_count_id ), 
     "group_mass_count", NC_LONG,
     (long)offsetof( MS_Raw_Per_Group, mass_count ), 1,
  {  (long)offsetof( MS_Dimensions, group_number_dim ), -1 } },
{ offsetof( MS_Variables, group_start_id ), 
     "group_starting_scan", NC_LONG,
     (long)offsetof( MS_Raw_Per_Group, starting_scan ), 1,
  {  (long)offsetof( MS_Dimensions, group_number_dim ), -1 } },
{ offsetof( MS_Variables, group_masses_id ), 
     "group_masses", NC_DOUBLE,
     (long)offsetof( MS_Raw_Per_Group, masses ), 2,
  {  (long)offsetof( MS_Dimensions, group_number_dim ),
     (long)offsetof( MS_Dimensions, group_max_masses_dim ) } },
{ offsetof( MS_Variables, group_samplings_id ), 
     "group_sampling_times", NC_DOUBLE,
     (long)offsetof( MS_Raw_Per_Group, sampling_times ), 2,
  {  (long)offsetof( MS_Dimensions, group_number_dim ),
     (long)offsetof( MS_Dimensions, group_max_masses_dim ) } },
{ offsetof( MS_Variables, group_delays_id ), 
     "group_delay_times", NC_DOUBLE,
     (long)offsetof( MS_Raw_Per_Group, delay_times ), 2,
  {  (long)offsetof( MS_Dimensions, group_number_dim ),
     (long)offsetof( MS_Dimensions, group_max_masses_dim ) } }
};

static int nGroupP = sizeof( group_variables ) / sizeof( MS_Variable_Data );

/* INSTRUMENT-ID variables						*/

static MS_Variable_Data instrument_variables[] = {
{ offsetof( MS_Variables, instrument_name_id ), 
     "instrument_name", NC_CHAR, 
     (long)offsetof( MS_Instrument_Data, name ), 2,
  {  (long)offsetof( MS_Dimensions, instrument_number_dim ),
     (long)offsetof( MS_Dimensions, _32_byte_dim ) } },
{ offsetof( MS_Variables, instrument_id_id ),
     "instrument_id", NC_CHAR, 
     (long)offsetof( MS_Instrument_Data, id ), 2,
  {  (long)offsetof( MS_Dimensions, instrument_number_dim ),
     (long)offsetof( MS_Dimensions, _32_byte_dim ) } },
{ offsetof( MS_Variables, instrument_mfr_id ),
     "instrument_mfr", NC_CHAR, 
     (long)offsetof( MS_Instrument_Data, manufacturer ), 2,
  {  (long)offsetof( MS_Dimensions, instrument_number_dim ),
     (long)offsetof( MS_Dimensions, _32_byte_dim ) } },
{ offsetof( MS_Variables, instrument_model_id ), 
     "instrument_model", NC_CHAR, 
     (long)offsetof( MS_Instrument_Data, model_number ), 2,
  {  (long)offsetof( MS_Dimensions, instrument_number_dim ),
     (long)offsetof( MS_Dimensions, _32_byte_dim ) } },
{ offsetof( MS_Variables, instrument_serial_no_id ), 
     "instrument_serial_no", NC_CHAR, 
     (long)offsetof( MS_Instrument_Data, serial_number ), 2,
  {  (long)offsetof( MS_Dimensions, instrument_number_dim ),
     (long)offsetof( MS_Dimensions, _32_byte_dim ) } },
{ offsetof( MS_Variables, instrument_sw_version_id ),
     "instrument_sw_version", NC_CHAR, 
     (long)offsetof( MS_Instrument_Data, software_version ), 2,
  {  (long)offsetof( MS_Dimensions, instrument_number_dim ),
     (long)offsetof( MS_Dimensions, _32_byte_dim ) } },
{ offsetof( MS_Variables, instrument_fw_version_id ),
     "instrument_fw_version", NC_CHAR, 
     (long)offsetof( MS_Instrument_Data, firmware_version ), 2,
  { (long)offsetof( MS_Dimensions, instrument_number_dim ),
     (long)offsetof( MS_Dimensions, _32_byte_dim ) } },
{ offsetof( MS_Variables, instrument_os_version_id ), 
     "instrument_os_version", NC_CHAR, 
     (long)offsetof( MS_Instrument_Data, operating_system ), 2,
  {   (long)offsetof( MS_Dimensions, instrument_number_dim ),
     (long)offsetof( MS_Dimensions, _32_byte_dim ) } },
{ offsetof( MS_Variables, instrument_app_version_id ),
     "instrument_app_version", NC_CHAR, 
     (long)offsetof( MS_Instrument_Data, application_software ), 2,
  {  (long)offsetof( MS_Dimensions, instrument_number_dim ),
     (long)offsetof( MS_Dimensions, _32_byte_dim ) } },
{ offsetof( MS_Variables, instrument_comments_id ),
     "instrument_comments", NC_CHAR, 
     (long)offsetof( MS_Instrument_Data, comments ), 2,
  {  (long)offsetof( MS_Dimensions, instrument_number_dim ),
     (long)offsetof( MS_Dimensions, _32_byte_dim ) } }
};

static int nInst = sizeof( instrument_variables ) / sizeof( MS_Variable_Data );

/*  LIBRARY DATA PER-SCAN variables					*/

static MS_Variable_Data library_variables[] = {
{ offsetof( MS_Variables, entry_name_id ), 
     "entry_name", NC_CHAR, 
     (long)offsetof( MS_Raw_Library, entry_name ), 2,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ),
     (long)offsetof( MS_Dimensions, _255_byte_dim ) } },
{ offsetof( MS_Variables, entry_id_id ), 
     "entry_id", NC_CHAR, 
     (long)offsetof( MS_Raw_Library, entry_id ), 2,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ),
     (long)offsetof( MS_Dimensions, _32_byte_dim ) } },
{ offsetof( MS_Variables, entry_number_id ), 
     "entry_number", NC_LONG, 
     (long)offsetof( MS_Raw_Library, entry_number ), 1,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ), -1 } },
{ offsetof( MS_Variables, source_data_file_id ), 
     "source_data_file_reference", NC_CHAR, 
     (long)offsetof( MS_Raw_Library, source_data_file_reference ), 2,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ),
     (long)offsetof( MS_Dimensions, _32_byte_dim ) } },
{ offsetof( MS_Variables, CAS_name_id ),
     "CAS_name", NC_CHAR, 
     (long)offsetof( MS_Raw_Library, cas_name ), 2,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ),
     (long)offsetof( MS_Dimensions, _255_byte_dim ) } },
{ offsetof( MS_Variables, CAS_number_id ), 
     "CAS_number", NC_LONG, 
     (long)offsetof( MS_Raw_Library, cas_number ), 1,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ), -1 } },
{ offsetof( MS_Variables, other_name_0_id ), 
     "other_name_0", NC_CHAR, 
     (long)offsetof( MS_Raw_Library, other_name_0 ), 2,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ),
     (long)offsetof( MS_Dimensions, _255_byte_dim ) } },
{ offsetof( MS_Variables, other_name_1_id ), 
     "other_name_1", NC_CHAR, 
     (long)offsetof( MS_Raw_Library, other_name_1 ), 2,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ),
     (long)offsetof( MS_Dimensions, _255_byte_dim ) } },
{ offsetof( MS_Variables, other_name_2_id ), 
     "other_name_2", NC_CHAR, 
     (long)offsetof( MS_Raw_Library, other_name_2 ), 2,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ),
     (long)offsetof( MS_Dimensions, _255_byte_dim ) } },
{ offsetof( MS_Variables, other_name_3_id ), 
     "other_name_3", NC_CHAR, 
     (long)offsetof( MS_Raw_Library, other_name_3 ), 2,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ),
     (long)offsetof( MS_Dimensions, _255_byte_dim ) } },
{ offsetof( MS_Variables, chemical_formula_id ),
     "chemical_formula", NC_CHAR, 
     (long)offsetof( MS_Raw_Library, formula ), 2,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ),
     (long)offsetof( MS_Dimensions, _64_byte_dim ) } },
{ offsetof( MS_Variables, smiles_id ),
     "smiles", NC_CHAR, 
     (long)offsetof( MS_Raw_Library, smiles ), 2,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ),
     (long)offsetof( MS_Dimensions, _255_byte_dim ) } },
{ offsetof( MS_Variables, wiswesser_id ),
     "wiswesser", NC_CHAR, 
     (long)offsetof( MS_Raw_Library, wiswesser ), 2,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ),
     (long)offsetof( MS_Dimensions, _128_byte_dim ) } },
{ offsetof( MS_Variables, molfile_id ),
     "molfile_reference", NC_CHAR, 
     (long)offsetof( MS_Raw_Library, molfile_reference ), 2,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ),
     (long)offsetof( MS_Dimensions, _32_byte_dim ) } },
{ offsetof( MS_Variables, other_structure_id ),
     "other_structure", NC_CHAR, 
     (long)offsetof( MS_Raw_Library, other_structure ), 2,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ),
     (long)offsetof( MS_Dimensions, _128_byte_dim ) } },
{ offsetof( MS_Variables, retention_index_id ),
     "retention_index", NC_DOUBLE, 
     (long)offsetof( MS_Raw_Library, retention_index ), 1,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ), -1 } },
{ offsetof( MS_Variables, retention_type_id ),
     "retention_type", NC_CHAR, 
     (long)offsetof( MS_Raw_Library, retention_type ), 2,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ),
     (long)offsetof( MS_Dimensions, _32_byte_dim ) } },
{ offsetof( MS_Variables, relative_retention_id ),
     "relative_retention", NC_DOUBLE, 
     (long)offsetof( MS_Raw_Library, relative_retention ), 1,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ), -1 } },
{ offsetof( MS_Variables, absolute_retention_id ),
     "absolute_retention", NC_DOUBLE, 
     (long)offsetof( MS_Raw_Library, absolute_retention ), 1,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ), -1 } },
{ offsetof( MS_Variables, retention_reference_name_id ),
     "retention_reference_name", NC_CHAR, 
     (long)offsetof( MS_Raw_Library, retention_reference ), 2,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ),
     (long)offsetof( MS_Dimensions, _128_byte_dim ) } },
{ offsetof( MS_Variables, retention_reference_CAS_id ),
     "retention_reference_CAS", NC_LONG, 
     (long)offsetof( MS_Raw_Library, retention_cas ), 1,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ), -1 } },
{ offsetof( MS_Variables, melting_point_id ),
     "melting_point", NC_FLOAT, 
     (long)offsetof( MS_Raw_Library, mp ), 1,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ), -1 } },
{ offsetof( MS_Variables, boiling_point_id ),
     "boiling_point", NC_FLOAT, 
     (long)offsetof( MS_Raw_Library, bp ), 1,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ), -1 } },
{ offsetof( MS_Variables, chemical_mass_id ),
     "chemical_mass", NC_DOUBLE, 
     (long)offsetof( MS_Raw_Library, chemical_mass ), 1,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ), -1 } },
{ offsetof( MS_Variables, nominal_mass_id ),
     "nominal_mass", NC_LONG, 
     (long)offsetof( MS_Raw_Library, nominal_mass ), 1,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ), -1 } },
{ offsetof( MS_Variables, accurate_mass_id ),
     "accurate_mass", NC_DOUBLE, 
     (long)offsetof( MS_Raw_Library, accurate_mass ), 1,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ), -1 } },
{ offsetof( MS_Variables, entry_other_information_id ),
     "entry_other_information", NC_CHAR, 
     (long)offsetof( MS_Raw_Library, other_info ), 2,
  {  (long)offsetof( MS_Dimensions, scan_number_dim ),
     (long)offsetof( MS_Dimensions, _255_byte_dim ) } }
};

static int nLib = sizeof( library_variables ) / sizeof( MS_Variable_Data );

/*
********************************************************************************
*
*  MS Attribute data arrays
*
********************************************************************************
*/

/* Attribute data array for ADMINISTRATIVE INFORMATION attributes	*/

static MS_Attribute_Data admin_attributes[] = {
{ offsetof( MS_Admin_Data, dataset_completeness ),
     "dataset_completeness", NC_GLOBAL, NC_CHAR, 1, 0, 0 },
{ offsetof( MS_Admin_Data, ms_template_revision ),
     "ms_template_revision", NC_GLOBAL, NC_CHAR, 1, 0, 0 },
{ offsetof( MS_Admin_Data, netcdf_revision),
     "netcdf_revision", NC_GLOBAL, NC_CHAR, 1, 0, 0 },
{ offsetof( MS_Admin_Data, languages ),
     "languages", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Admin_Data, comments ),
     "administrative_comments", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Admin_Data, dataset_origin ),
     "dataset_origin", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Admin_Data, dataset_owner ),
     "dataset_owner", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Admin_Data, netcdf_date_time ),
     "netcdf_file_date_time_stamp", NC_GLOBAL, NC_CHAR, 1, 0, 0 },
{ offsetof( MS_Admin_Data, experiment_title ),
     "experiment_title", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Admin_Data, experiment_date_time ),
     "experiment_date_time_stamp", NC_GLOBAL, NC_CHAR, 1, 0, 0 },
{ offsetof( MS_Admin_Data, experiment_x_ref_0 ),
     "experiment_x_ref_0", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Admin_Data, experiment_x_ref_1 ),
     "experiment_x_ref_1", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Admin_Data, experiment_x_ref_2 ),
     "experiment_x_ref_2", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Admin_Data, experiment_x_ref_3 ),
     "experiment_x_ref_3", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Admin_Data, operator_name ),
     "operator_name", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Admin_Data, pre_expt_program_name ),
     "pre_experiment_program_name", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Admin_Data, post_expt_program_name ),
     "post_experiment_program_name", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Admin_Data, source_file_reference ),
     "source_file_reference", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Admin_Data, source_file_format ),
     "source_file_format", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Admin_Data, source_file_date_time ), 
     "source_file_date_time_stamp", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Admin_Data, external_file_ref_0 ),
     "external_file_ref_0", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Admin_Data, external_file_ref_1 ),
     "external_file_ref_1", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Admin_Data, external_file_ref_2 ),
     "external_file_ref_2", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Admin_Data, external_file_ref_3 ),
     "external_file_ref_3", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Admin_Data, calibration_history_0 ),
     "calibration_history_0", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Admin_Data, calibration_history_1 ),
     "calibration_history_1", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Admin_Data, calibration_history_2 ),
     "calibration_history_2", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Admin_Data, calibration_history_3 ),
     "calibration_history_3", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Admin_Data, experiment_type ),
     "experiment_type", NC_GLOBAL, NC_CHAR, 0, 1, (int)expt_centroid },
{ offsetof( MS_Admin_Data, number_times_processed ), 
     "number_of_times_processed", NC_GLOBAL, NC_LONG, 0, 0, 0 },
{ offsetof( MS_Admin_Data, number_times_calibrated ),
     "number_of_times_calibrated", NC_GLOBAL, NC_LONG, 0, 0, 0 }
};

static int nAdminA = sizeof( admin_attributes ) / sizeof( MS_Attribute_Data );

/*  Attribute data array for SAMPLE DESCRIPTION attributes		*/

static MS_Attribute_Data sample_attributes[] = {
{ offsetof( MS_Sample_Data, internal_id ),
     "sample_internal_id", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Sample_Data, external_id ),
     "sample_external_id", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Sample_Data, receipt_date_time ),
     "sample_receipt_date_time_stamp", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Sample_Data, owner ),
     "sample_owner", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Sample_Data, procedure_name ),
     "sample_procedure_name", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Sample_Data, matrix ),
     "sample_matrix", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Sample_Data, storage ),
     "sample_storage", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Sample_Data, disposal ),
     "sample_disposal", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Sample_Data, history ),
     "sample_history", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Sample_Data, prep_procedure ),
     "sample_prep_procedure", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Sample_Data, prep_comments ),
     "sample_prep_comments", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Sample_Data, manual_handling ),
     "sample_manual_handling", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Sample_Data, comments ),
     "sample_comments", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Sample_Data, state ),
     "sample_state", NC_GLOBAL, NC_CHAR, 0, 1, (int)state_other }
};

static int nSamp = sizeof( sample_attributes ) / sizeof( MS_Attribute_Data );

/* Attribute data array for TEST METHOD attributes			*/

static MS_Attribute_Data test_attributes[] = {
{ offsetof( MS_Test_Data, separation_type ),
     "test_separation_type", NC_GLOBAL, NC_CHAR, 0, 1, (int)separation_none  },
{ offsetof( MS_Test_Data, ms_inlet ),
     "test_ms_inlet", NC_GLOBAL, NC_CHAR, 0, 1, (int)inlet_direct },
{ offsetof( MS_Test_Data, ms_inlet_temperature ),
     "test_ms_inlet_temperature", NC_GLOBAL, NC_FLOAT, 0, 0, 0 },
{ offsetof( MS_Test_Data, ionization_mode ),
     "test_ionization_mode", NC_GLOBAL, NC_CHAR, 0, 1, (int)ionization_ei },
{ offsetof( MS_Test_Data, ionization_polarity ),
     "test_ionization_polarity", NC_GLOBAL, NC_CHAR, 0, 1, (int)polarity_plus },
{ offsetof( MS_Test_Data, electron_energy ),
     "test_electron_energy", NC_GLOBAL, NC_FLOAT, 0, 0, 0 },
{ offsetof( MS_Test_Data, laser_wavelength),
     "test_laser_wavelength", NC_GLOBAL, NC_FLOAT, 0, 0, 0 },
{ offsetof( MS_Test_Data, reagent_gas ),
     "test_reagent_gas", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Test_Data, reagent_gas_pressure ),
     "test_reagent_gas_pressure", NC_GLOBAL, NC_FLOAT, 0, 0, 0 },
{ offsetof( MS_Test_Data, fab_type ),
     "test_fab_type", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Test_Data, fab_matrix ),
     "test_fab_matrix", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Test_Data, source_temperature ),
     "test_source_temperature", NC_GLOBAL, NC_FLOAT, 0, 0, 0 },
{ offsetof( MS_Test_Data, filament_current ),
     "test_filament_current", NC_GLOBAL, NC_FLOAT, 0, 0, 0 },
{ offsetof( MS_Test_Data, emission_current ),
     "test_emission_current", NC_GLOBAL, NC_FLOAT, 0, 0, 0 },
{ offsetof( MS_Test_Data, accelerating_potential),
     "test_accelerating_potential", NC_GLOBAL, NC_FLOAT, 0, 0, 0 },
{ offsetof( MS_Test_Data, detector_type ),
     "test_detector_type", NC_GLOBAL, NC_CHAR, 0, 1, (int)detector_em },
{ offsetof( MS_Test_Data, detector_potential ),
     "test_detector_potential", NC_GLOBAL, NC_FLOAT, 0, 0, 0 },
{ offsetof( MS_Test_Data, detector_entrance_potential ),
     "test_detector_entrance_potential", NC_GLOBAL, NC_FLOAT, 0, 0, 0 },
{ offsetof( MS_Test_Data, resolution_type ), "test_resolution_type",
     NC_GLOBAL, NC_CHAR, 0, 1, (int)resolution_constant },
{ offsetof( MS_Test_Data, resolution_method ),
     "test_resolution_method", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Test_Data, scan_function ),
     "test_scan_function", NC_GLOBAL, NC_CHAR, 0, 1, (int)function_scan },
{ offsetof( MS_Test_Data, scan_direction ),
     "test_scan_direction", NC_GLOBAL, NC_CHAR, 0, 1, (int)direction_up },
{ offsetof( MS_Test_Data, scan_law ),
     "test_scan_law", NC_GLOBAL, NC_CHAR, 0, 1, (int)law_linear },
{ offsetof( MS_Test_Data, scan_time ),
     "test_scan_time", NC_GLOBAL, NC_FLOAT, 0, 0, 0 },
{ offsetof( MS_Test_Data, mass_calibration_file ),
     "mass_calibration_file", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Test_Data, external_reference_file ),
     "test_external_reference_file", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Test_Data, internal_reference_file ),
     "test_internal_reference_file", NC_GLOBAL, NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Test_Data, comments ),
     "test_comments", NC_GLOBAL, NC_CHAR, 0, 0, 0 }
};

static int nTest = sizeof( test_attributes ) / sizeof( MS_Attribute_Data );

/*  Attribute data array for RAW DATA GLOBAL attributes			*/

static MS_Attribute_Data raw_data_attributes[] = {
{ offsetof( MS_Raw_Data_Global, mass_format ),
     "raw_data_mass_format", NC_GLOBAL, NC_CHAR, 0, 1, (int)data_short },
{ offsetof( MS_Raw_Data_Global, time_format ),
     "raw_data_time_format", NC_GLOBAL, NC_CHAR, 0, 1, (int)data_short },
{ offsetof( MS_Raw_Data_Global, intensity_format ),
     "raw_data_intensity_format", NC_GLOBAL, NC_CHAR, 0, 1, (int)data_long },
{ offsetof( MS_Raw_Data_Global, mass_units ), "units",
     (long)offsetof( MS_Variables, mass_values_id ), NC_CHAR, 0, 1, 
     (int)mass_m_z },
{ offsetof( MS_Raw_Data_Global, time_units ), "units",
     (long)offsetof( MS_Variables, time_values_id ), NC_CHAR, 0, 1,
     (int)time_seconds },
{ offsetof( MS_Raw_Data_Global, intensity_units ), "units",
     (long)offsetof( MS_Variables, intensity_values_id ), NC_CHAR, 0, 1,
     (int)intensity_arbitrary },
{ offsetof( MS_Raw_Data_Global, intensity_offset ), "add_offset",
     (long)offsetof( MS_Variables, intensity_values_id ), NC_DOUBLE, 0, 0, 0 },
{ offsetof( MS_Raw_Data_Global, mass_factor ), "scale_factor",
     (long)offsetof( MS_Variables, mass_values_id ), NC_DOUBLE, 0, 0, 0 },
{ offsetof( MS_Raw_Data_Global, time_factor ), "scale_factor",
     (long)offsetof( MS_Variables, time_values_id ), NC_DOUBLE, 0, 0, 0 },
{ offsetof( MS_Raw_Data_Global, intensity_factor ), "scale_factor",
     (long)offsetof( MS_Variables, intensity_values_id ), NC_DOUBLE, 0, 0, 0 },
{ offsetof( MS_Raw_Data_Global, total_intensity_units ), "units",
     (long)offsetof( MS_Variables, total_intensity_id ), NC_CHAR, 0, 1,
     (int)intensity_arbitrary },
{ offsetof( MS_Raw_Data_Global, mass_label ), "long_name",
     (long)offsetof( MS_Variables, mass_values_id ), NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Raw_Data_Global, time_label ), "long_name",
     (long)offsetof( MS_Variables, time_values_id ), NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Raw_Data_Global, intensity_label ), "long_name",
     (long)offsetof( MS_Variables, intensity_values_id ), NC_CHAR, 0, 0, 0 },
{ offsetof( MS_Raw_Data_Global, starting_scan_number ), "starting_scan_number",
     NC_GLOBAL, NC_LONG, 0, 0, 0 },
{ offsetof( MS_Raw_Data_Global, mass_axis_global_min ), "global_mass_min",
     NC_GLOBAL, NC_DOUBLE, 0, 0, 0 },
{ offsetof( MS_Raw_Data_Global, mass_axis_global_max ), "global_mass_max",
     NC_GLOBAL, NC_DOUBLE, 0, 0, 0 },
{ offsetof( MS_Raw_Data_Global, time_axis_global_min ), "global_time_min",
     NC_GLOBAL, NC_DOUBLE, 0, 0, 0 },
{ offsetof( MS_Raw_Data_Global, time_axis_global_max ), "global_time_max",
     NC_GLOBAL, NC_DOUBLE, 0, 0, 0 },
{ offsetof( MS_Raw_Data_Global, intensity_axis_global_min ),
     "global_intensity_min", NC_GLOBAL, NC_DOUBLE, 0, 0, 0 },
{ offsetof( MS_Raw_Data_Global, intensity_axis_global_max ),
     "global_intensity_max", NC_GLOBAL, NC_DOUBLE, 0, 0, 0 },
{ offsetof( MS_Raw_Data_Global, calibrated_mass_min ), "calibrated_mass_min",
     NC_GLOBAL, NC_DOUBLE, 0, 0, 0 },
{ offsetof( MS_Raw_Data_Global, calibrated_mass_max ), "calibrated_mass_max",
     NC_GLOBAL, NC_DOUBLE, 0, 0, 0 },
{ offsetof( MS_Raw_Data_Global, run_time ), "actual_run_time_length",
     NC_GLOBAL, NC_DOUBLE, 0, 0, 0 },
{ offsetof( MS_Raw_Data_Global, delay_time ), "actual_delay_time",
     NC_GLOBAL, NC_DOUBLE, 0, 0, 0 },
{ offsetof( MS_Raw_Data_Global, uniform_flag ),
     "raw_data_uniform_sampling_flag", NC_GLOBAL, NC_SHORT, 0, 0, 0 },
{ offsetof( MS_Raw_Data_Global, comments ), 
     "raw_data_comments", NC_GLOBAL, NC_CHAR, 0, 0, 0 }
};

static int nRaw = sizeof( raw_data_attributes ) / sizeof( MS_Attribute_Data );

#endif	/* MS10_IO_INCLUDED */
/* DON'T ADD ANYTHING AFTER THIS #endif */
