/* -*-C-*-
*******************************************************************************
*
* File:         ms10enum.c
* RCS:          $Header: $
* Description:  Enumerated value <-> string literal translations for the MS
*		netCDF Data Interchange Specification, Categories 1 & 2 data
* Author:       David Stranz
*		Fisons Instruments
*		809 Sylvan Avenue, Suite 102
*		Modesto, CA  95350
*		Telephone:	(209) 521-0714
*		FAX:		(209) 521-9017
*		CompuServe:	70641,3057
*		Internet:	70641.3057@compuserve.com
* Created:      Thu Feb 27 13:23:19 1992
* Modified:     Mon Jun 28 09:47:13 1993 (David Stranz) dstranz@lfrg1
* Language:     C
* Package:      N/A
* Status:       Public Domain (Distribute with Copyright Notice)
*
*      (C) Copyright 1992, Analytical Instrument Association
*			   All rights reserved.
*
*******************************************************************************
*/

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

#include	<stddef.h>
#include	<ctype.h>
#include	"ms10.h"

/*
********************************************************************************
*
*  Enumerated set enum to string correspondences
*
*  NOTE: The strings below are sorted in case-insensitive ascending alphabetical
*  order.  If you add new strings, ensure that order is maintained!  It is also
*  a requirement that all string literals be unique.
*
*  A binary search is used to find string literals in this array; that is
*  the reason for the sorting and uniqueness requirements.
*
********************************************************************************
*/

typedef struct {
  char *	literal_value;
  int		enum_value;
} EnumLiteral;

static EnumLiteral	ms_enums[] = {
{ "Arbitrary Mass Units",			(int) mass_arbitrary },
{ "Arbitrary Intensity Units",			(int) intensity_arbitrary },
{ "Arbitrary Time Units",			(int) time_arbitrary },
{ "Atmospheric Pressure Chemical Ionization",	(int) ionization_apci },
{ "Atmospheric Pressure Chemical Ionization Inlet",	(int) inlet_apci },
{ "Capillary Direct",				(int) inlet_capillary },
{ "Capillary Zone Electrophoresis",		(int) separation_cze },
{ "Centroided Mass Spectrum",			(int) expt_centroid },
{ "Chemical Ionization",			(int) ionization_ci },
{ "Constant Resolution",			(int) resolution_constant },
{ "Continuum Mass Spectrum",			(int) expt_continuum },
{ "Conversion Dynode Electron Multiplier",	(int) detector_dynode_em },
{ "Conversion Dynode Photomultiplier",		(int) detector_dynode_pm },
{ "Counts Per Second",				(int) intensity_cps },
{ "Current",					(int) intensity_current },
{ "Direct Inlet Probe",				(int) inlet_direct },
{ "Double",					(int) data_double },
{ "Down",					(int) direction_down },
{ "Electron Impact",				(int) ionization_ei },
{ "Electron Multiplier",			(int) detector_em },
{ "Electrospray Inlet",				(int) inlet_es },
{ "Electrospray Ionization",			(int) ionization_es },
{ "Exponential",				(int) law_exponential },
{ "Faraday Cup",				(int) detector_cup },
{ "Fast Atom Bombardment",			(int) ionization_fab },
{ "Field Desorption",				(int) ionization_fd },
{ "Field Flow Fractionation",			(int) separation_fff },
{ "Field Ionization",				(int) ionization_fi },
{ "Float",					(int) data_float },
{ "Flow Injection Analysis",			(int) inlet_fia },
{ "Focal Plane Array",				(int) detector_focal },
{ "Gas",					(int) state_gas },
{ "Gas-Liquid Chromatography",			(int) separation_glc },
{ "Gas-Solid Chromatography",			(int) separation_gsc },
{ "Infusion",					(int) inlet_infusion },
{ "Ion Exchange Liquid Chromatography",		(int) separation_ielc },
{ "Ion Pair Liquid Chromatography",		(int) separation_iplc },
{ "Jet Separator",				(int) inlet_jet },
{ "Laser Desorption",				(int) ionization_ld },
{ "Library Mass Spectrum",			(int) expt_library },
{ "Linear",					(int) law_linear },
{ "Liquid",					(int) state_liquid },
{ "Long",					(int) data_long },
{ "M/Z",					(int) mass_m_z },
{ "Mass Scan",					(int) function_scan },
{ "Membrane Separator",				(int) inlet_membrane },
{ "Moving Belt",				(int) inlet_belt },
{ "Multicollector",				(int) detector_multicoll },
{ "Negative Polarity",				(int) polarity_minus },
{ "No Chromatography",				(int) separation_none },
{ "Normal Phase Liquid Chromatography",		(int) separation_nplc },
{ "Open Split",					(int) inlet_opensplit },
{ "Other Chromatography",			(int) separation_other },
{ "Other Detector",				(int) detector_other },
{ "Other Direction",				(int) direction_other },
{ "Other Function",				(int) function_other },
{ "Other Inlet",				(int) inlet_other },
{ "Other Intensity",				(int) intensity_other },
{ "Other Ionization",				(int) ionization_other },
{ "Other Law",					(int) law_other },
{ "Other Liquid Chromatography",		(int) separation_olc },
{ "Other Mass",					(int) mass_other },
{ "Other Probe",				(int) inlet_probe },
{ "Other State",				(int) state_other },
{ "Other Time",					(int) time_other },
{ "Particle Beam",				(int) inlet_pb },
{ "Photomultiplier",				(int) detector_pm },
{ "Plasma",					(int) state_plasma },
{ "Plasma Desorption",				(int) ionization_pd },
{ "Positive Polarity",				(int) polarity_plus },
{ "Proportional Resolution",			(int) resolution_proportional },
{ "Quadratic",					(int) law_quadratic },
{ "Reservoir",					(int) inlet_reservoir },
{ "Reverse Phase Liquid Chromatography",	(int) separation_rplc },
{ "Seconds",					(int) time_seconds },
{ "Selected Ion Detection",			(int) function_sid },
{ "Septum",					(int) inlet_septum },
{ "Short",					(int) data_short },
{ "Size Exclusion Liquid Chromatography",	(int) separation_selc },
{ "Solid",					(int) state_solid },
{ "Spark Ionization",				(int) ionization_spark },
{ "Supercritical Fluid Chromatography",		(int) separation_sfc },
{ "Supercritical Fluid",			(int) state_supercrit },
{ "Thermal Ionization",				(int) ionization_thermal },
{ "Thermospray Inlet",				(int) inlet_ts },
{ "Thermospray Ionization",			(int) ionization_ts },
{ "Thin Layer Chromatography",			(int) separation_tlc },
{ "Total Counts",				(int) intensity_counts },
{ "Up",						(int) direction_up },
{ "Volts",					(int) intensity_volts }
};

#define	ENUM_COUNT	(sizeof( ms_enums ) / sizeof( EnumLiteral ))

/*
********************************************************************************
*
*  Exported functions
*
********************************************************************************
*/

/*
********************************************************************************
*
*  FUNCTION:	ms_enum_to_string
*
*  DESCRIPTION:	Converts the enumerated constant (passed as an int) to the
*		corresponding string literal.  The caller MUST treat these
*		strings as read-only.
*
*  ARGUMENTS:	(int) enumerated constant
*
*  RETURNS:	(char *) pointer to string (NULL if not found)
*
*  AUTHOR:	David Stranz Thu Feb 27 14:52:25 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
char *
ms_enum_to_string( int enum_value )
#else	/* __STDC__ */
char *
ms_enum_to_string( enum_value )
     int	enum_value;
#endif	/* not __STDC__ */
{
  int	i;

  for ( i = 0; i < (int) ENUM_COUNT; i++ ) {
    if ( ms_enums[i].enum_value == enum_value ) {
       return ms_enums[i].literal_value;
    }
  }

  return NULL;

}  /* ms_enum_to_string */

/*
********************************************************************************
*
*  FUNCTION:	ms_string_to_enum
*
*  DESCRIPTION:	Converts the string value to the corresponding enumerated
*		constant, returned as an int.  Although the string literals are
*		upper/lower case, the comparisons are made case-insensitively.
*		
*		This routine performs a binary search of the enumerated string
*		list.  In order for it to work successfully, the string list
*		MUST be sorted in ascending, case-insensitive alphabetical order.
*		If any additions/modifications are made to the list, ensure
*		that sort order is maintained.
*
*  ARGUMENTS:	(char *) string literal
*
*  RETURNS:	(int) enumerated constant (MS_ERROR if error)
*
*  AUTHOR:	David Stranz Thu Feb 27 14:58:15 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
int
ms_string_to_enum( char * literal_value )
#else	/* __STDC__ */
int
ms_string_to_enum( literal_value )
     char *	literal_value;
#endif	/* not __STDC__ */
{
  int	left = 0;
  int	right = ENUM_COUNT - 1;
  int	pos;
  char	*s;
  char 	*t;
  int	diff;

  if ( (char *) NULL == literal_value )
    return MS_ERROR;

  /* The binary search of the string array uses a trick to speed it up.
     Rather than compare full strings at each step, the first letters of
     the target and search strings are compared.  Only if they match is
     a full string compare made.					*/

  while ( right >= left ) {
    pos = (right + left) / 2;
    t = literal_value;
    s = ms_enums[pos].literal_value;
    while ((0 == 
	    (diff = (int) (tolower( *t ) - tolower( *s )))) && *s && *t ) {
      s++;
      t++;
    }
    if ( diff < 0 )
      right = pos - 1;
    else if ( diff > 0 )
      left = pos + 1;
    else
       return ms_enums[pos].enum_value;
  }

  return MS_ERROR;

}  /* ms_string_to_enum */

#ifdef	TEST_ENUM

#include	<stdio.h>

/*
********************************************************************************
*
*  FUNCTION:	main
*
*  DESCRIPTION:	Test routine for string to enum conversion
*
*  ARGUMENTS:	(none)
*
*  RETURNS:	(int) return code
*
*  AUTHOR:	David Stranz Fri Feb 28 11:27:32 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
int
main()
#else	/* __STDC__ */
int
main()
#endif	/* not __STDC__ */
{
  int		enum_value;
  int		i;
  char *	literal_value;
  int		hits[2 * ENUM_COUNT];

  for ( i = 0; i < 2 * ENUM_COUNT; hits[i++] = 0 );

  /* Look up every string value, then use the enumerated value to find 
     the string.  Make sure there are no duplicate enumerated constants.
     An error in the search for the enumerated constant given the string
     means that there is an error in the sort order.			*/

  for ( i = 0; i < ENUM_COUNT; i++ ) {
    enum_value = ms_string_to_enum( ms_enums[i].literal_value );
    literal_value = ms_enum_to_string( enum_value );
    if ( hits[enum_value] != 0 )
      fprintf( stdout, "Duplicate enum value!!\n" );
    hits[enum_value] = 1;
  }

  return( 1 );

}  /* main */

#endif	/* TEST_ENUM */
