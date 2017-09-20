/* -*-C-*-
*******************************************************************************
*
* File:         ms10aux.c
* RCS:          $Header: $
* Description:  Public-domain implementation of the MS netCDF Data
*		Interchange Specification, Categories 1 and 2 data elements.
*		This file contains auxilliary functions to support the MS
*		implementation.
* Author:       David Stranz
*		Fisons Instruments
*		809 Sylvan Avenue, Suite 102
*		Modesto, CA  95350
*		Telephone:	(209) 521-0714
*		FAX:		(209) 521-9017
*		CompuServe:	70641,3057
*		Internet:	70641.3057@compuserve.com
* Created:      Thu Feb 20 15:00:16 1992
* Modified:     Wed Jun 30 16:07:37 1993 (David Stranz) dstranz@lfrg1
* Language:     C
* Package:      N/A
* Status:       Public Domain (Distribute with Copyright Notice)
*
*      (C) Copyright 1992, Analytical Instrument Association
*		           All rights reserved.
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

#include	<stdlib.h>
#include	<string.h>
#include	"netcdf.h"
#include	"ms10.h"

/*
********************************************************************************
*
*  Internal (static) functions
*
********************************************************************************
*/

/*
********************************************************************************
*
*  FUNCTION:	put_number	(static)
*
*  DESCRIPTION:	Formats an integer number into a character string, with
*		leading zeros.  This function will fail if the count exceeds
*		the number of digits (i.e. the power of ten) which can be
*		represented in an integer.  No assumption is made that the
*		string is NULL terminated on entry, and it is not NULL
*		terminated on exit.  In other words, the arguments had
*		better be valid.
*
*  ARGUMENTS:	(char *) string, (int) starting string index,
*		(int) number of characters required, (int) integer value
*
*  RETURNS:	(int) error code (MS_ERROR / MS_NO_ERROR)
*
*  AUTHOR:	David Stranz Mon Nov 16 10:31:41 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
static int
put_number( char * string, int start, int count, int value )
#else	/* __STDC__ */
static int
put_number( string, start, count, value )
   char *	string;
   int		start;
   int		count;
   int		value;
#endif	/* not __STDC__ */
{
   int		index;	
      
   if ( NULL == string )
      return MS_ERROR;

   if ( value < 0 )
      value = -value;

   index = start + count - 1;
   while ( value ) {
      string[index--] = '0' + (char)(value % 10);
      value /= 10;
   }
   while( index >= start )		/* add leading zeros		*/
      string[index--] = '0';

   return MS_NO_ERROR;

}  /* put_number */

/*
********************************************************************************
*
*  FUNCTION:	get_number	(static)
*
*  DESCRIPTION:	Retrieves an integer number from a character string.  This
*		function will also fail if the character count exceeds the
*		the power of ten which can be represented by an integer.
*		The same assumptions as for put_number (above) apply.
*
*  ARGUMENTS:	(char *) string, (int) starting index of number in string,
*		(int) number of characters to decode
*
*  RETURNS:	(int) unsigned integer value (MS_ERROR on error)
*
*  AUTHOR:	David Stranz Mon Nov 16 10:43:07 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
static int
get_number( char * string, int start, int count )
#else	/* __STDC__ */
static int
get_number( string, start, count )
   char *	string;
   int		start;
   int		count;
#endif	/* not __STDC__ */
{
   int		ret_val = 0;

   if ( NULL == string )
      return MS_ERROR;

   while( count ) {
      ret_val *= 10;
      ret_val += (int)(string[start++] - '0');
      count--;
   }

   return ret_val;

}  /* get_number */

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
*  FUNCTION:	ms_read_enum_attribute
*
*  DESCRIPTION:	Reads the literal version of an enumerated attribute from
*		the netCDF file and returns the enumerated constant value
*
*  ARGUMENTS:	(int) netCDF id, (int) variable id, (char *) name
*
*  RETURNS:	(int) enumerated value (MS_ERROR if error)
*
*  AUTHOR:	David Stranz Mon Mar  2 10:28:25 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
int
ms_read_enum_attribute( int cdfid, int varid, char * name )
#else	/* __STDC__ */
int
ms_read_enum_attribute( cdfid, varid, name )
     int	cdfid;
     int	varid;
     char *	name;
#endif	/* not __STDC__ */
{
  char *	enum_literal;
  int		enum_value;
  nc_type	datatype;
  int		len;

  if ( NULL == name )
    return MS_ERROR;

  if ( MS_ERROR == ncattinq( cdfid, varid, name, &datatype, &len ) )
    return MS_ERROR;

  if ( len <= 0 || datatype != NC_CHAR )
    return MS_ERROR;

  /* Allocate a buffer to hold the literal value of the enumerated
     constant.								*/

  if ( NULL == (enum_literal =
		(char *)malloc( (unsigned)(len + 1) * sizeof( char ) )) )
    return MS_ERROR;

  /* Read the literal value						*/

  if ( MS_ERROR == ncattget( cdfid, varid, name, enum_literal ) ) {
    free( enum_literal );
    return MS_ERROR;
  }
  enum_literal[len] = '\0';

  /* Convert to an enumerated value, then free the allocated string	*/

  enum_value = ms_string_to_enum( enum_literal );
  free( enum_literal );
  
  if ( MS_ERROR == enum_value )
    return MS_ERROR;

  return enum_value;

}  /* ms_read_enum_attribute */

/*
********************************************************************************
*
*  FUNCTION:	ms_read_string_variable
*
*  DESCRIPTION:	Reads the value of a string variable from the netCDF file.
*		Space is allocated for the string value.  The array index is
*		the first dimension for arrays of strings (the second dimension,
*		that of the string itself, is implicit); use -1 for non-array
*		string variables.
*
*  ARGUMENTS:	(int) netCDF id, (int) variable id, (long) array index 1,
*		(long) array index 2, (int) maximum size of string
*
*  RETURNS:	(char *) newly allocated string (NULL if error or empty)
*
*  AUTHOR:	David Stranz Tue Mar  3 10:52:59 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
char *
ms_read_string_variable( int cdfid, int varid, long arr_index1,
			long arr_index2, int max_size )
#else	/* __STDC__ */
char *
ms_read_string_variable( cdfid, varid, arr_index1,arr_index2, max_size )
   int		cdfid;
   int		varid;
   long		arr_index1;
   long		arr_index2;
   int		max_size;
#endif	/* not __STDC__ */
{
   long		tx_start[3];			/* Hyperslab corners	*/
   long		tx_count[3];			/* Hyperslab edges	*/
   char *	string;
   register int	i;
   unsigned int	len;

   /* Set up hyperslab indices and counts for multidimensional string
      variable access.							*/

   tx_start[0] = ( arr_index1 < 0L ? 0L : arr_index1 );
   tx_start[1] = ( arr_index2 < 0L ? 0L : arr_index2 );
   tx_start[2] = 0L;				/* in all cases		*/

   tx_count[0] = (arr_index1 < 0L ? (long) max_size : 1L );
   tx_count[1] = (arr_index2 < 0L ? (long) max_size : 1L );
   tx_count[1] = (long) max_size;		/* also in all cases	*/

   /* Allocate a string to hold the variable value			*/

   if ( NULL ==
       (string = (char *)malloc( (unsigned) max_size * sizeof( char ) )) ) {
      return NULL;
   }

   for ( i = 0; i < max_size; string[i++] = ' ' ) { }
   string[max_size - 1] = '\0';

   if ( MS_ERROR ==
       ncvarget( cdfid, varid, tx_start, tx_count, (void *) string ) ) {
      free( string );
      return NULL;
   }
  
   /* Put a terminating NULL, just in case...				*/

   string[max_size - 1] = '\0';

   /* If the string is entirely blank, then get rid of the allocated string
      and simply return a NULL pointer					*/

   if ( 0 == (int)(len =  strlen( string )) ) {
      free( string );
      return NULL;
   }

   for ( i = len; i >= 0; i-- ) {
      if ( ' ' != string[i] && '\0' != string[i] )
	 break;
      string[i] = '\0';
   }

   if ( 0 == (int) strlen( string ) ) {
      free( string );
      return NULL;
   }

   return string;

}  /* ms_read_string_variable */

/*
********************************************************************************
*
*  FUNCTION:	ms_write_string_variable
*
*  DESCRIPTION:	Writes the value of a string variable to the netCDF file.  NULL-
*		values strings are supported, and are written out with zero
*		length.  The array index is for the first dimension of string
*		array variables; use -1 for non-array strings
*
*  ARGUMENTS:	(int) netCDF id, (int) variable id, (long) first array index,
*		(long) second array index, (int) maximum length of string,
*		(char *) string
*
*  RETURNS:	(int) error code (MS_ERROR if error, 0 otherwise)
*
*  AUTHOR:	David Stranz Tue Mar  3 11:23:22 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
int
ms_write_string_variable( int cdfid, int varid, long arr_index1,
			 long arr_index2, int max_size, char * string )
#else	/* __STDC__ */
int
ms_write_string_variable( cdfid, varid, arr_index1, arr_index2, max_size,
			 string )
   int		cdfid;
   int		varid;
   long		arr_index1;
   long		arr_index2;
   int		max_size;
   char *	string;
#endif	/* not __STDC__ */
{
   long		tx_start[3];			/* Hyperslab corners	*/
   long		tx_count[3];			/* Hyperslab edges	*/
   register int	i;
   char *	ms_buffer;
   
   tx_start[0] = ( arr_index1 < 0L ? 0L : arr_index1 );
   tx_start[1] = ( arr_index2 < 0L ? 0L : arr_index2 );
   tx_start[2] = 0L;				/* in all cases		*/

   tx_count[0] = (arr_index1 < 0L ? (long) max_size : 1L );
   tx_count[1] = (arr_index2 < 0L ? (long) max_size : 1L );
   tx_count[2] = (long) max_size;
   
   /* Allocate a buffer to hold the string to be written		*/

   if ( NULL == 
       (ms_buffer = (char *)malloc( (unsigned int)max_size * sizeof( char ) )) )
      return MS_ERROR;

   for ( i = 0; i < max_size; ms_buffer[i++] = ' ' ) { }
   
   if ( string != NULL )
      (void) strncpy( ms_buffer, string, (unsigned int)max_size );
   ms_buffer[max_size - 1] = '\0';
   
   if ( MS_ERROR ==
       ncvarput( cdfid, varid, tx_start, tx_count, (void *) ms_buffer ) ) {
      free( ms_buffer );
      return MS_ERROR;
   }
   
   free( ms_buffer );
   return MS_NO_ERROR;

}  /* ms_write_string_variable */

/*
********************************************************************************
*
*  FUNCTION:	ms_open_write
*
*  DESCRIPTION:	Opens a netCDF file for writing; any existing file is
*		overwritten.
*
*  ARGUMENTS:	(char *) filename, (ms_admin_expt_t) experiment type,
*		(long) number of scans, (long) number of instrument components,
*		(ms_data_format_t) mass data format,
*		(ms_data_format_t) time data format,
*		(ms_data_format_t) intensity data format,
*		(int) masses flag (TRUE if mass values will be written)
*		(int) times flag (TRUE if time values will be written)
*
*  RETURNS:	(int) fileID (MS_ERROR if error, id >= 0 if successful)
*
*  AUTHOR:	David Stranz Thu Nov 12 10:16:55 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
int
ms_open_write( char * filename, ms_admin_expt_t expt_type, long nscans,
	      long ninst, ms_data_format_t mass_type, ms_data_format_t time_type,
	      ms_data_format_t inty_type, int do_masses, int do_times )
#else	/* __STDC__ */
int
ms_open_write( filename, expt_type, nscans, ninst, mass_type, time_type,
	      inty_type, do_masses, do_times )
   char *		filename;
   ms_admin_expt_t	expt_type;
   long			nscans;
   long			ninst;
   ms_data_format_t	mass_type;
   ms_data_format_t	time_type;
   ms_data_format_t	inty_type;
   int			do_masses;
   int			do_times;
#endif	/* not __STDC__ */
{
   int		cdfid;

   if ( NULL == filename || 0 == strlen( filename ) )
      return MS_ERROR;

   if ( MS_ERROR == (cdfid = nccreate( filename, NC_CLOBBER )) )
      return MS_ERROR;

   if ( MS_ERROR == ms_associate_id( cdfid ) ) {
      (void)ncclose( cdfid );
      return MS_ERROR;
   }
     
   if ( MS_ERROR ==
       ms_write_dimensions( cdfid, nscans, ninst ) ) {
      (void)ncclose( cdfid );
      return MS_ERROR;
   }

   if ( MS_ERROR ==
       ms_write_variables( cdfid, expt_type, mass_type, time_type, inty_type,
			  do_masses, do_times ) ) {
      (void)ncclose( cdfid );
      return MS_ERROR;
   }

   /* Place the netCDF file into "data mode".  This permits data to be written
      prior to writing the global information.  Some applications may not
      know all global data prior to reading through the input file, so
      allowing global data to be written whenever it is convenient alleviates
      any need to make more than one pass thorugh the input file.

      We ignore the error return from this call.			*/

   (void)ncendef( cdfid );

   return cdfid;

}  /* ms_open_write */

/*
********************************************************************************
*
*  FUNCTION:	ms_open_read
*
*  DESCRIPTION:	Opens a netCDF file for reading.
*
*  ARGUMENTS:	(char *) filename
*
*  RETURNS:	(int) file ID (MS_ERROR if error, id >= 0 if successful)
*
*  AUTHOR:	David Stranz Thu Nov 12 10:27:13 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
int
ms_open_read( char * filename )
#else	/* __STDC__ */
int
ms_open_read( filename )
   char *	filename;
#endif	/* not __STDC__ */
{
   int		cdfid;

   if ( NULL == filename || 0 == strlen( filename ) )
      return MS_ERROR;

   if ( MS_ERROR == (cdfid = ncopen( filename, NC_NOWRITE )) )
      return MS_ERROR;

   if ( MS_ERROR == ms_associate_id( cdfid ) ) {
      (void)ncclose( cdfid );
      return MS_ERROR;
   }

   if ( MS_ERROR == ms_read_dimensions( cdfid ) ) {
      (void)ncclose( cdfid );
      return MS_ERROR;
   }

   if ( MS_ERROR == ms_read_variables( cdfid ) ) {
      (void)ncclose( cdfid );
      return MS_ERROR;
   }

   return cdfid;

}  /* ms_open_read */

/*
********************************************************************************
*
*  FUNCTION:	ms_close
*
*  DESCRIPTION:	Closes a netCDF file
*
*  ARGUMENTS:	(int) file id
*
*  RETURNS:	(void) 
*
*  AUTHOR:	David Stranz Thu Nov 12 10:40:45 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
void
ms_close( int cdfid )
#else	/* __STDC__ */
void
ms_close( cdfid )
   int	cdfid;
#endif	/* not __STDC__ */
{

   (void)ncclose( cdfid );
   (void)ms_dissociate_id( cdfid );

}  /* ms_close */

/*
********************************************************************************
*
*  FUNCTION:	ms_convert_date
*
*  DESCRIPTION:	Converts numeric data and time information to a ISO date/time
*		stamp string or vice-versa, depending on the flag.
*
*  ARGUMENTS:	(int) to_stamp flag - if TRUE, numeric data is converted to
*		a string, otherwise, the string is converted to numeric
*		data, (MS_Date_Time *) date/time data structure
*
*  RETURNS:	(int) error code (MS_ERROR / MS_NO_ERROR)
*
*  AUTHOR:	David Stranz Mon Nov 16 09:58:19 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
int
ms_convert_date( int flag, MS_Date_Time * dt )
#else	/* __STDC__ */
int
ms_convert_date( flag, dt )
   int			flag;
   MS_Date_Time *	dt;
#endif	/* not __STDC__ */
{

   if ( NULL == dt )
      return MS_ERROR;

   /* The ISO 3307 date format is: YYYYMMDDhhmmss+/-ffff, where

      YYYY	is the full four-digit year
      MM	is the month (January = 01)
      DD	is the day of the month
      hh	is the hour of day, on the 24-hour scale
      mm	is the minute of the hour
      ss	is the second within the minute
      +/-	is the sign of the time differential factor (either a
		plus or a minus sign character)
      ffff	is the difference (in hours and minutes) between Greenwich
      		Mean Time (GMT) and local time.

     For example, the time 12:30:23 pm, December 1, 1992, Eastern Standard
     Time (5 hours behind GMT) is represented as:  19921201123023-0500	*/

   if ( flag ) {			/* convert to string		*/

      if ( NULL == 
	  (dt->string = (char *)malloc( MS_STAMP_LENGTH * sizeof( char ) )) )
	 return MS_ERROR;

      (void)put_number( dt->string, 0,  4, dt->year );
      (void)put_number( dt->string, 4,  2, dt->month );
      (void)put_number( dt->string, 6,  2, dt->day );
      (void)put_number( dt->string, 8,  2, dt->hour );
      (void)put_number( dt->string, 10, 2, dt->minute );
      (void)put_number( dt->string, 12, 2, dt->second );

      dt->string[14] = (dt->differential >= 0 ? '+' : '-');

      (void)put_number( dt->string, 15, 4, dt->differential );
      dt->string[19] = '\0';

      return MS_NO_ERROR;
   }
   else {

      if ( NULL == dt->string || strlen( dt->string ) < MS_STAMP_LENGTH -1 )
	 return MS_ERROR;

      dt->year		= get_number( dt->string, 0,  4 );
      dt->month		= get_number( dt->string, 4,  2 );
      dt->day		= get_number( dt->string, 6,  2 );
      dt->hour		= get_number( dt->string, 8,  2 );
      dt->minute	= get_number( dt->string, 10, 2 );
      dt->second	= get_number( dt->string, 12, 2 );
      dt->differential	= get_number( dt->string, 15, 4 );

      if ( '-' == dt->string[14] )
	 dt->differential = -dt->differential;

      return MS_NO_ERROR;
   }

}  /* ms_convert_date */

/*
********************************************************************************
*
*  FUNCTION:	ms_copy_array
*
*  DESCRIPTION:	Copies (and casts) the contents of one array into another.
*		Both input and output arrays must exist, be of the appropriate
*		types, and be of proper length.  If the round flag is TRUE,
*		floating point values will be rounded (by adding 0.5, then
*		truncating) before being converted to integers.
*
*  ARGUMENTS:	(void *) input array, (ms_data_format_t) input data type,
*		(long) number of elements to copy, (void *) output array,
*		(ms_data_format_t) output array type, (int) round flag
*
*  RETURNS:	(void) 
*
*  AUTHOR:	David Stranz Mon Nov 30 14:07:50 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
void
ms_copy_array( void * input, ms_data_format_t in_type, long in_count,
	      void * output, ms_data_format_t out_type, int round_flag  )
#else	/* __STDC__ */
void
ms_copy_array( input, in_type, in_count, output, out_type, round_flag )
   void *		input;
   ms_data_format_t	in_type;
   long			in_count;
   void *		output;
   ms_data_format_t	out_type;
   int			round_flag;
#endif	/* not __STDC__ */
{
   long		index;
   short *	in_sp = NULL;
   short *	out_sp = NULL;
   long *	in_lp = NULL;
   long *	out_lp = NULL;
   float *	in_fp = NULL;
   float *	out_fp = NULL;
   double *	in_dp = NULL;
   double *	out_dp = NULL;

   if ( NULL == input || NULL == output || in_count <= 0L )
      return;

   
   switch( out_type ) {
    case data_short:
      out_sp = (short *) output;
      break;

    case data_long:
      out_lp = (long *) output;
      break;

    case data_float:
      out_fp = (float *) output;
      break;

    case data_double:
      out_dp = (double *) output;
      break;

    default:
      return;
   }

   switch( in_type ) {
    case data_short:
      in_sp = (short *) input;
      switch( out_type ) {
       case data_short:
	 for ( index = 0; index < in_count; index++ )
	    *out_sp++ = *in_sp++;
	 break;

       case data_long:
	 for ( index = 0; index < in_count; index++ )
	    *out_lp++ = (long) *in_sp++;
	 break;

       case data_float:
	 for ( index = 0; index < in_count; index++ )
	    *out_fp++ = (float) *in_sp++;
	 break;

       case data_double:
	 for ( index = 0; index < in_count; index++ )
	    *out_dp++ = (double) *in_sp++;
	 break;

       default:
	 break;
      }
      break;

    case data_long:
      in_lp = (long *) input;
      switch( out_type ) {
       case data_short:
	 for ( index = 0; index < in_count; index++ )
	    *out_sp++ = (short) *in_lp++;
	 break;

       case data_long:
	 for ( index = 0; index < in_count; index++ )
	    *out_lp++ = *in_lp++;
	 break;

       case data_float:
	 for ( index = 0; index < in_count; index++ )
	    *out_fp++ = (float) *in_lp++;
	 break;

       case data_double:
	 for ( index = 0; index < in_count; index++ )
	    *out_dp++ = (double) *in_lp++;
	 break;

       default:
	 break;
      }
      break;

    case data_float:
      in_fp = (float *) input;
      switch( out_type ) {
       case data_short:
	 for ( index = 0; index < in_count; index++ )
	    *out_sp++ = (short) (*in_fp++ + (round_flag ? 0.5 : 0.0));
	 break;

       case data_long:
	 for ( index = 0; index < in_count; index++ )
	    *out_lp++ = (long) (*in_fp++ + (round_flag ? 0.5 : 0.0));
	 break;

       case data_float:
	 for ( index = 0; index < in_count; index++ )
	    *out_fp++ = *in_fp++;
	 break;

       case data_double:
	 for ( index = 0; index < in_count; index++ )
	    *out_dp++ = (double) *in_fp++;
	 break;

       default:
	 break;
      }
      break;

    case data_double:
      in_dp = (double *) input;
      switch( out_type ) {
       case data_short:
	 for ( index = 0; index < in_count; index++ )
	    *out_sp++ = (short) (*in_dp++ + (round_flag ? 0.5 : 0.0));
	 break;

       case data_long:
	 for ( index = 0; index < in_count; index++ )
	    *out_lp++ = (long) (*in_dp++ + (round_flag ? 0.5 : 0.0));
	 break;

       case data_float:
	 for ( index = 0; index < in_count; index++ )
	    *out_fp++ = (float) *in_dp++;
	 break;

       case data_double:
	 for ( index = 0; index < in_count; index++ )
	    *out_dp++ = *in_dp++;
	 break;

       default:
	 break;
      }
      break;

    default:
      break;
   }

   return;

}  /* ms_copy_array */
