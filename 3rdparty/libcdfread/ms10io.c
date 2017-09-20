/* -*-C-*-
*******************************************************************************
*
* File:         ms10io.c
* RCS:          $Header: $
* Description:  Public-domain implementation of the MS netCDF Data
*		Interchange Specification, Categories 1 and 2 data elements.
*		This file implements the netCDF file I/O functions of the
*		MS specification.
* Author:       David Stranz
*		Fisons Instruments
*		809 Sylvan Avenue, Suite 102
*		Modesto, CA  95350
*		Telephone:	(209) 521-0714
*		FAX:		(209) 521-9017
*		CompuServe:	70641,3057
*		Internet:	70641.3057@compuserve.com
* Created:      Thu Feb 20 15:00:16 1992
* Modified:     Wed Aug 25 12:51:32 1993 (David Stranz) dstranz@lfrg1
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

#include	<stddef.h>
#include	<stdlib.h>
#include	<string.h>
#include	"netcdf.h"
#include	"ms10.h"
#include	"ms10io.h"

#include	<stdio.h>

/*
********************************************************************************
*
*  This is one monster file; however, in exchange for size, it isolates
*  all netCDF I/O and simplifies the interface to I/O routines (basically,
*  makes the majority of the process table driven).
*
********************************************************************************
*/

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
*  FUNCTION:	format_to_datatype	(static)
*
*  DESCRIPTION:	Converts a mass, time, or intensity format to a netCDF
*		data type.
*
*  ARGUMENTS:	(ms_data_format_t) format specifier, (nc_type) default type
*
*  RETURNS:	(nc_type) datatype specifier
*
*  AUTHOR:	David Stranz Thu Oct 29 14:21:00 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
static nc_type
format_to_datatype( ms_data_format_t fmt, nc_type default_type )
#else	/* __STDC__ */
static nc_type
format_to_datatype( fmt, default_type )
   ms_data_format_t	fmt;
   nc_type		default_type;
#endif	/* not __STDC__ */
{
   int		i;

   for ( i = 0; i < nTypes; i++ )
      if ( fmt == ms_types[i].ms_fmt )
	 return ms_types[i].cdf_type;

   return default_type;

}  /* format_to_datatype */

/*
********************************************************************************
*
*  FUNCTION:	datatype_to_format	(static)
*
*  DESCRIPTION:	Returns the datatype given the nc_type
*
*  ARGUMENTS:	(nc_type) input type, (ms_data_format_t) default format
*
*  RETURNS:	(ms_data_format_t) corresponding format
*
*  AUTHOR:	David Stranz Tue Dec  1 08:12:47 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
static ms_data_format_t
datatype_to_format( nc_type typ, ms_data_format_t default_fmt )
#else	/* __STDC__ */
static ms_data_format_t
datatype_to_format( typ, default_fmt )
   nc_type		typ;
   ms_data_format_t	default_fmt;
#endif	/* not __STDC__ */
{
   int	i;

   for ( i = 0; i < nTypes; i++ )
      if ( typ == ms_types[i].cdf_type )
	 return ms_types[i].ms_fmt;

   return default_fmt;

}  /* datatype_to_format */

/*
********************************************************************************
*
*  FUNCTION:	put_default	(static)
*
*  DESCRIPTION:	Utility routine to cast and load a default numeric data value
*
*  ARGUMENTS:	(nc_type) datatype, (void *) address of variable, (int) array
*		index
*
*  RETURNS:	(void) 
*
*  AUTHOR:	David Stranz Thu Nov  5 11:11:22 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
static void
put_default( nc_type datatype, void * value, int offset )
#else	/* __STDC__ */
static void
put_default( datatype, value, offset )
   nc_type	datatype;
   void *	value;
   int		offset;
#endif	/* not __STDC__ */
{

   switch (datatype ) {
    case NC_BYTE:
      *((char *)value + offset) = (char)MS_NULL_BYTE;
      break;

    case NC_SHORT:
      *((short *)value + offset) = (short)MS_NULL_INT;
      break;

    case NC_LONG:
      *((long *)value + offset) = (long)MS_NULL_INT;
      break;

    case NC_FLOAT:
      *((float *)value + offset) = (float)MS_NULL_FLT;
      break;

    case NC_DOUBLE:
      *((double *)value + offset) = (double)MS_NULL_FLT;
      break;

    default:
      break;
   }
   return;

}  /* put_default */

/*
********************************************************************************
*
*  FUNCTION:	make_array	(static)
*
*  DESCRIPTION:	Allocates an array of the appropriate length and data type
*
*  ARGUMENTS:	(long) number of points, (nc_type) datatype
*
*  RETURNS:	(void *) pointer to array (NULL if error)
*
*  AUTHOR:	David Stranz Thu Nov  5 14:23:53 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
static void *
make_array( long npts, nc_type datatype )
#else	/* __STDC__ */
static void *
make_array( npts, datatype )
   long		npts;
   nc_type	datatype;
#endif	/* not __STDC__ */
{
   long		size;

   switch ( datatype ) {
    case NC_SHORT:
      size = sizeof( short );
      break;

    case NC_LONG:
      size = sizeof( long );
      break;

    case NC_FLOAT:
      size = sizeof( float );
      break;

    case NC_DOUBLE:
      size = sizeof( double );
      break;

    default:
      return NULL;
   }

   return (void *) malloc( (unsigned long)(npts * size) );

}  /* make_array */

/*
********************************************************************************
*
*  FUNCTION:	init_indexed_variables	(static)
*
*  DESCRIPTION:	Initializes (and optionally clears) variables which are
*		present on a per-scan or per-instrument basis.
*
*  ARGUMENTS:	(char *) values data structure pointer,
*		(int) number of variables,
*		(MS_Variable_Data *) variable definitions array	pointer,
*		(int) clear flag
*
*  RETURNS:	(void) 
*
*  AUTHOR:	David Stranz Fri Nov 13 09:08:08 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
static void
init_indexed_variables( char * values, int nDefs, MS_Variable_Data * defs,
		       int clear )
#else	/* __STDC__ */
static void
init_indexed_variables( values, nDefs, defs, clear )
   char *		values;
   int			nDefs;
   MS_Variable_Data *	defs;
   int			clear;
#endif	/* not __STDC__ */
{
   int		i;

   if ( NULL == values || NULL == defs )
      return;

   for ( i = 0; i < nDefs; i++ ) {

      if ( 0 == defs[i].ndim ) {		/* not possible		*/
	 continue;
      }
      else if ( 1 == defs[i].ndim ) {	/* single dimension		*/
	 if ( NC_CHAR == defs[i].datatype ) {
	    continue;			/* not possible			*/
	 }
	 else {
	    put_default( defs[i].datatype,
			(void *)(values + defs[i].data_offset), 0 );
	 }
      }
      else if ( 2 == defs[i].ndim ) {
	 if ( NC_CHAR == defs[i].datatype ) {
	    if ( clear && (*((char * *)(values + defs[i].data_offset))) )
	       free( *((char * *)(values + defs[i].data_offset)) );
	    *((char * *)(values + defs[i].data_offset)) = NULL;
	 }
	 else
	    continue;
      }
      else
	 continue;

   }	/* for ( i = ... )	*/

   return;

}  /* init_indexed_variables */

/*
********************************************************************************
*
*  FUNCTION:	write_variables	(static)
*
*  DESCRIPTION:	Defines netCDF variables given an array of definitions
*
*  ARGUMENTS:	(int) netcdf file id, (int) number of variables,
*		(MS_Variables *) variable ids data structure,
*		(MS_Dimensions *) dimension ids data structure
*		(MS_Variable_Data *) variable data array
*
*  RETURNS:	(int) error code (MS_ERROR / MS_NO_ERROR)
*
*  AUTHOR:	David Stranz Thu Oct 29 13:09:07 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
static int
write_variables( int cdfid, int nDefs, MS_Variables * varbls,
		MS_Dimensions * dimens, MS_Variable_Data * defs )
#else	/* __STDC__ */
static int
write_variables( cdfid, nDefs, varbls, dimens, defs )
   int			cdfid;
   int			nDefs;
   MS_Variables *	varbls;
   MS_Dimensions *	dimens;
   MS_Variable_Data *	defs;
#endif	/* not __STDC__ */
{
   int		i;			/* index into variable data	*/
   int		j;			/* index into dimension array	*/
   int		dim_ids[3];		/* dimension ids		*/

   /*  Error checking: nDefs > 0; varbls, dimens, defs must be non-NULL	*/

   if ( nDefs <= 0 || NULL == varbls || NULL == dimens || NULL == defs )
      return MS_ERROR;

   /* For each definition in the array, create a new netCDF variable	*/

   for ( i = 0; i < nDefs; i++ ) {

      /* Load dimension ids into argument array for ncvardef call	*/

      for ( j = 0; j < defs[i].ndim; j++ )
	 dim_ids[j] = ((ms_dim *)((char *)dimens + defs[i].dim[j]))->id;

      if ( MS_ERROR ==
	  (*((int *)(((char *)varbls) + defs[i].offset)) =
	   ncvardef( cdfid, defs[i].name, defs[i].datatype, defs[i].ndim,
		    dim_ids )) )
	 return MS_ERROR;
   }

   return MS_NO_ERROR;
   
}  /* write_variables */

/*
********************************************************************************
*
*  FUNCTION:	read_variables	(static)
*
*  DESCRIPTION:	Reads netCDF variable ids into the definitions array
*
*  ARGUMENTS:	(int) netCDF file id, (int) number of variables,
*		(MS_Variables *) variables data structure pointer,
*		(MS_Dimensions *) dimensions data structure pointer,
*		(MS_Variable_Data *) pointer to array of variable definitions
*
*  RETURNS:	(int) 
*
*  AUTHOR:	David Stranz Wed Nov  4 15:02:56 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
static int
read_variables( int cdfid, int nDefs, MS_Variables * varbls,
	       MS_Dimensions * dimens, MS_Variable_Data * defs )
#else	/* __STDC__ */
static int
read_variables( cdfid, nDefs, varbls, dimens, defs )
   int			cdfid;
   int			nDefs;
   MS_Variables *	varbls;
   MS_Dimensions *	dimens;
   MS_Variable_Data *	defs;
#endif	/* not __STDC__ */
{
   int		i;

   /* Error checking: nDefs > 0; varbls, dimens, defs non-NULL		*/

   if ( nDefs <= 0 || NULL == varbls || NULL == dimens || NULL == defs )
      return MS_ERROR;

   for ( i = 0; i < nDefs; i++ ) {
      *((int *)(((char *)varbls) + defs[i].offset)) =
	 ncvarid( cdfid, defs[i].name );
   }

   return MS_NO_ERROR;

}  /* read_variables */

/*
********************************************************************************
*
*  FUNCTION:	init_attributes	(static)
*
*  DESCRIPTION:	Initializes an attribute data structure to default values.
*		Optionally (clear flag == TRUE), will also free non-NULL
*		strings.
*
*  ARGUMENTS:	(char *) cast pointer to attribute data structure,
*		(int) number of attributes,
*		(MS_Attribute_Data *) pointer to attribute values structure,
*		(int) clear flag
*
*  RETURNS:	(void) 
*
*  AUTHOR:	David Stranz Wed Nov 11 13:24:23 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
static void
init_attributes( char * values, int nAtts, MS_Attribute_Data * atts, int clear )
#else	/* __STDC__ */
static void
init_attributes( values, nAtts, atts, clear )
   char *		values;
   int			nAtts;
   MS_Attribute_Data *	atts;
   int			clear;
#endif	/* not __STDC__ */
{
   int		i;

   /* Error check: pointers must be non-NULL				*/

   if ( NULL == atts || NULL == values )
      return;

   /* Enumerated attributes are stored in the file as NC_CHAR, but are
      represented in the data structures as integers, so they receive
      special treatment.  In addition, if the clear flag is set, then
      any non-NULL character string pointers are freed before being
      set to NULL.							*/

   for ( i = 0; i < nAtts; i++ ) {
      if ( NC_CHAR == atts[i].datatype ) {
	 if ( atts[i].enumerated )
	    *((int *)(values + atts[i].offset)) = atts[i].default_value;
	 else {
	    if ( clear && (*((char * *)(values + atts[i].offset))) )
	       free( *((char * *)(values + atts[i].offset)) );
	    *((char * *)(values + atts[i].offset)) = NULL;
	 }
      }
      else
	 put_default( atts[i].datatype, (void *)(values + atts[i].offset), 0 );
   }
	 
}  /* init_attributes */

/*
********************************************************************************
*
*  FUNCTION:	write_attributes	(static)
*
*  DESCRIPTION:	Writes attribute values to the netCDF file
*
*  ARGUMENTS:	(int) netCDF file id, (int) number of attributes,
*		(MS_Variables *) variables data structure pointer,
*		(MS_Attribute_Data *) attribute definition data structure pointer,
*		(char *) cast pointer to attribute values data structure
*
*  RETURNS:	(int) error code (MS_ERROR / MS_NO_ERROR)
*
*  AUTHOR:	David Stranz Fri Oct 30 09:14:00 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
static int
write_attributes( int cdfid, int nAtt, MS_Variables * varbls,
		 MS_Attribute_Data * atts, char * values )
#else	/* __STDC__ */
static int
write_attributes( cdfid, nAtt, varbls, atts, values )
   int			cdfid;
   int			nAtt;
   MS_Variables *	varbls;
   MS_Attribute_Data *	atts;
   char *		values;
#endif	/* not __STDC__ */
{
   int		i;
   int		len = 1;
   int		id;
   long		int_val;
   double	dbl_val;
   int		write_it;
   void *	value;

   /*  Error checking: nAtt > 0, all pointers valid			*/

   if ( nAtt <= 0 || NULL == varbls || NULL == atts || NULL == values )
      return MS_ERROR;

   /*  Loop through attribute definitions.  Depending on the type of each
       attribute, whether it is local or global, and whether it is an
       explicit or implicit value (i.e. needs to be retrieved via a lookup of
       the string representation of an enumerated value), extract the
       attribute value appropriately.  If the attribute has a default
       value (NULL, MS_NULL_FLT, or MS_NULL_INT), ignore the attribute.
       Otherwise, write it to the file.		*/

   for ( i = 0; i < nAtt; i++ ) {

      write_it = TRUE;

      /* Determine the datatype, and retrieve the attribute value	*/

      switch( atts[i].datatype ) {

	 /*  All enumerated variables convert to NC_CHAR strings, so NC_CHAR
	     attributes need special processing.			*/

       case NC_CHAR:

	 if ( atts[i].enumerated ) {
	    value = 
	       (void *)ms_enum_to_string( *((int *)(values + atts[i].offset)) );
	 }
	 else {
	    value = (void *)(*((char * *)(values + atts[i].offset)));
	 }
	 if ( NULL == value ) {
	    write_it = FALSE;
	    break;
	 }
	 len = (long)strlen( (char *) value ) + 1;
	 break;

	 /*  All other types are handled identically.			*/
	 
       case NC_BYTE:

	 len = 1;
	 value = (void *)(values + atts[i].offset);
	 int_val = (long)(*((char *)value));
	 if ( MS_NULL_INT == int_val )
	    write_it = FALSE;
	 break;
	 
       case NC_SHORT:
	 
	 len = 1;
	 value = (void *)(values + atts[i].offset);
	 int_val = (long)(*((short *)value));
	 if ( MS_NULL_INT == int_val )
	    write_it = FALSE;
	 break;

       case NC_LONG:

	 len = 1;
	 value = (void *)(values + atts[i].offset);
	 int_val = *((long *)value);
	 if ( MS_NULL_INT == int_val )
	    write_it = FALSE;
	 break;

       case NC_FLOAT:
	 
	 len = 1;
	 value = (void *)(values + atts[i].offset);
	 dbl_val = (double)(*((float *)value));
	 if ( (int)MS_NULL_FLT == (int)dbl_val )
	    write_it = FALSE;
	 break;
	 
       case NC_DOUBLE:

	 len = 1;
	 value = (void *)(values + atts[i].offset);
	 dbl_val = *((double *)value);
	 if ( (int)MS_NULL_FLT == (int)dbl_val )
	    write_it = FALSE;
	 break;

       default:
	 return MS_ERROR;		/* invalid datatype!		*/
      }

      if ( !write_it ) {
	 continue;
      }

      /*  Check for local or global attribute.  If local, retrieve variable
	  id from MS_Variables data structure				*/

      if ( NC_GLOBAL == atts[i].id )
	 id = NC_GLOBAL;
      else
	 id = *((int *)((char *)varbls + atts[i].id));

      /* Finally, write the attribute definition			*/

      if ( MS_ERROR ==
	  ncattput( cdfid, id, atts[i].name, atts[i].datatype, len, value ) )
	 return MS_ERROR;
   }

   return MS_NO_ERROR;

}  /* write_attributes */

/*
********************************************************************************
*
*  FUNCTION:	read_attributes	(static)
*
*  DESCRIPTION:	Reads attribute values from the netCDF file.  Numeric attribute
*		values are read directly into the fields for them; space is
*		first allocated for string attributes, then the values are
*		read in.  This space must be freed before the next read (or the
*		pointer copied so it can later be freed), or a memory leak
*		will occur.
*
*		Attributes which are defined but not present in the netCDF
*		file will be returned with default values (MS_NULL_INT,
*		MS_NULL_FLT, NULL, or the default enumerated type) as
*		appropriate.
*
*  ARGUMENTS:	(int) netCDF file id, (int) number of attributes,
*		(MS_Variables *) variables data structure pointer,
*		(MS_Attribute_Data *) attribute definition data structure pointer,
*		(char * *) cast pointer to attribute values data structure
*
*  RETURNS:	(int) error code (MS_ERROR / MS_NO_ERROR)
*
*  AUTHOR:	David Stranz Thu Nov  5 07:40:14 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
static int
read_attributes( int cdfid, int nAtts, MS_Variables * varbls,
		MS_Attribute_Data * atts, char * values )
#else	/* __STDC__ */
static int
read_attributes( cdfid, nAtts, varbls, atts, values )
   int			cdfid;
   int			nAtts;
   MS_Variables *	varbls;
   MS_Attribute_Data *	atts;
   char * 		values;
#endif	/* not __STDC__ */
{
   int		i;
   int		isPresent;
   int		varid;
   int		length;
   nc_type	datatype;
   char		buffer[MS_MAX_STRING_LENGTH];

   /* Error checking: all pointers must be non-NULL; nAtts > 0		*/

   if ( nAtts <= 0 || NULL == varbls || NULL == atts || NULL == values )
      return MS_ERROR;

   for ( i = 0; i < nAtts; i++ ) {

      /* Determine the datatype, and retrieve appropriately		*/

      if ( NC_GLOBAL == atts[i].id )
	 varid = NC_GLOBAL;
      else
	 varid = *((int *)((char *)varbls + atts[i].id));

      /* Inquire about the attribute.  An error return indicates the attribute
         was not found.							*/

      if ( MS_ERROR == 
	  ncattinq( cdfid, varid, atts[i].name, &datatype, &length ) )
	 isPresent = FALSE;
      else
	 isPresent = TRUE;
      
      /* Check for datatype mismatch.					*/

      if ( isPresent && ( datatype != atts[i].datatype ) )
	    return MS_ERROR;

      /* Check for missing mandatory attribute				*/

      if ( atts[i].mandatory && FALSE == isPresent ) {
	 return MS_ERROR;
      }

      switch ( atts[i].datatype ) {

       case NC_CHAR:			/* A character string		*/

	 /* Character strings can be one of two types: either a free-form
	    string value or the string form of an enumerated type.  Free-
	    form strings are retrieved into space allocated for them,
	    whereas enumerated types are retrieved as strings into a
	    buffer, then converted to the enumerated type.  Free-form
	    string attribute values which are not recorded in the file are
	    assigned NULL pointers; missing enumerated values are assigned
	    their defaults.						*/

	 if ( isPresent ) {

	    memset( buffer, 0, sizeof( buffer ) );
	    if ( MS_ERROR ==
		ncattget( cdfid, varid, atts[i].name, (void *)buffer ) ) {
	       return MS_ERROR;
	    }

	    if ( atts[i].enumerated ) {
	       *((int *)(values + atts[i].offset)) =
		  ms_string_to_enum( buffer );
	    }
	    else {
	       char *	p;
	       p = (char *)malloc((unsigned long)(length + 1) * sizeof( char ));
	       if ( NULL == p )
		  return MS_ERROR;

	       (void)strcpy( p, buffer );
	       *((char * *)(values + atts[i].offset)) = p;
	    }
	 }
	 else {
	    if ( atts[i].enumerated )
	       *((int *)(values + atts[i].offset)) = atts[i].default_value;
	    else
	       *((char * *)(values + atts[i].offset)) = NULL;
	 }
	 break;

       /* Numeric types are all handled in essentially the same way; the
	  MS implementation permits only single-valued numeric attributes
	  (i.e. numeric attribute arrays are not supported, although this
	  could be changed with a bit of effort).			*/

       case NC_BYTE: {
	  char	byte_val = (char) MS_NULL_BYTE;

	  if ( isPresent )
	     if ( MS_ERROR ==
		 ncattget( cdfid, varid, atts[i].name, (void *)&byte_val ) )
		return MS_ERROR;

	  *(values + atts[i].offset) = byte_val;
	  break;
       }

       case NC_SHORT: {
	  short	short_val = (short) MS_NULL_INT;
	 
	  if ( isPresent )
	     if ( MS_ERROR ==
		 ncattget( cdfid, varid, atts[i].name, (void *)&short_val ) )
		return MS_ERROR;

	  *((short *)(values + atts[i].offset)) = short_val;
	  break;
       }

       case NC_LONG: {
	  long	long_val = (long) MS_NULL_INT;

	  if ( isPresent )
	     if ( MS_ERROR ==
		 ncattget( cdfid, varid, atts[i].name, (void *)&long_val ) )
		return MS_ERROR;

	  *((long *)(values + atts[i].offset)) = long_val;
	  break;
       }

       case NC_FLOAT: {
	  float	float_val = (float) MS_NULL_FLT;

	  if ( isPresent )
	     if ( MS_ERROR ==
		 ncattget( cdfid, varid, atts[i].name, (void *)&float_val ) )
		return MS_ERROR;

	  *((float *)(values + atts[i].offset)) = float_val;
	  break;
       }

       case NC_DOUBLE: {
	  double	double_val = (double) MS_NULL_FLT;

	  if ( isPresent )
	     if ( MS_ERROR ==
		 ncattget( cdfid, varid, atts[i].name, (void *)&double_val ) )
		return MS_ERROR;

	  *((double *)(values + atts[i].offset)) = double_val;
	  break;
       }

       default:
	 return MS_ERROR;		/* undefined type		*/

      }  /* switch	*/
   }	/* for		*/

   return MS_NO_ERROR;

}  /* read_attributes */

/*
********************************************************************************
*
*  FUNCTION:	write_info	(static)
*
*  DESCRIPTION:	Writes variable values to the netCDF file
*
*  ARGUMENTS:	(int) netCDF file id, 
*		(MS_Variables *) variable ids data structure pointer,
*		(MS_Dimensions *) dimension ids data structure pointer,
*		(long) major dimension for array variables,
*		(int) number of data variables,
*		(char *) pointer to variable values data structure
*		(MS_Variable_Data *) pointer to MS_Variable_Data array
*
*  RETURNS:	(int) error code (MS_ERROR / MS_NO_ERROR)
*
*  AUTHOR:	David Stranz Mon Nov  2 14:16:57 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
static int
write_info( int cdfid, MS_Variables * varbls, MS_Dimensions * dimens,
	   long index, int nvals, char * values, MS_Variable_Data * var_data )
#else	/* __STDC__ */
static int
write_info( cdfid, varbls, dimens, index, nvals, values, var_data )
   int			cdfid;
   MS_Variables *	varbls;
   MS_Dimensions *	dimens;
   long			index;
   int			nvals;
   char *		values;
   MS_Variable_Data *	var_data;
#endif	/* not __STDC__ */
{
   int		i;
   int		j;
   int		varid;			/* variable id			*/
   long		dt_count[2];		/* size of array or string	*/
   long		dt_start[2];		/* indices into hyperslab	*/
   void *	value;			/* address of value to be written */
   int		nminor;

   /* Error checking: varbls, dimens, values, var_data non NULL		*/

   if ( NULL == varbls || NULL == dimens || NULL == values || NULL == var_data )
      return MS_ERROR;

   /* There are two cases to cover here:

      Case 1:  If index < 0, then we are being asked to write a variable which
               occurs in one chunk (not on a per-scan basis, say).  

        (1a):  If it has zero dimensionality, then it is a single-valued
	       variable.
	(1b):  If the dimensionality is one and it is of type NC_CHAR,
	       then it is a single character string.  If it is of another type,
	       then it represents an array of those types.
	(1c):  If the dimensionality is two, then the only allowed type
	       is NC_CHAR, and it represents an array of strings.
	(1d):  Dimensionality > 2 is an error.

      Case 2:  If index >= 0, then we are being asked to write one slab of 
               a variable which occurs as a dimensioned array.

	(2a):  Dimensionality zero is an error.
	(2b):  Dimensionality one with type NC_CHAR is an error, but 
	       otherwise it represents one value of an arrayed set.
	(2c):  Dimensionality two represents a hyperslab - either a single
	       string or an array of numbers (such as a scan range) which
	       varies with the major index.
	(2d):  Dimensionality > 2 is not supported.

      So, all these choices make for a somewhat long and complicated
      bit of logic.   It is further complicated by the fact that we are
      looping over all the fields in a data structure, and the logic
      must be applied separately to each member (although the indexing
      over major index must be the same for all elements - a data structure
      cannot have some elements which vary on a per-scan basis, and
      some which do not).						*/

   for ( i = 0; i < nvals; i++ ) {	/* loop over all data fields	*/
      
      varid = *((int *)((char *)varbls + var_data[i].offset));

      if ( index < 0 ) {			/* case 1		*/
	 if ( 0 == var_data[i].ndim ) {		/* case 1a		*/

	    if ( NC_CHAR == var_data[i].datatype )
	       return MS_ERROR;		/* error - strings are arrays	*/

	    /* Write a non-dimensioned variable value			*/

	    dt_start[0] = 0L;
	    value = (void *)((char *)values + var_data[i].data_offset);
	    if ( MS_ERROR ==
		ncvarput1( cdfid,  varid, dt_start, value ) )
	       return MS_ERROR;
	 }

	 else if ( 1 == var_data[i].ndim ) {	/* case 1b		*/
	    if ( NC_CHAR == var_data[i].datatype ) {	/* put a string	*/
	       dt_count[0] =
		  ((ms_dim *)((char *)dimens + var_data[i].dim[0]))->size;
	       if ( MS_ERROR ==
		   ms_write_string_variable( cdfid, varid, index, -1, 
					    dt_count[0],
					    *((char * *)
					      (values + 
					       var_data[i].data_offset)) ) )
		  return MS_ERROR;
	    }
	    else {					/* put an array	*/
	       dt_start[0] = 0L;
	       dt_count[0] =
		  ((ms_dim *)((char *)dimens + var_data[i].dim[0]))->size;
	       value = (void *)((char *)values + var_data[i].data_offset);
	       if ( MS_ERROR ==
		   ncvarput( cdfid, varid, dt_start, dt_count, value ) )
		  return MS_ERROR;
	    }
	 }
	 else if ( 2 == var_data[i].ndim ) {	/* case 1c		*/
	    if ( NC_CHAR != var_data[i].datatype )
	       return MS_ERROR;			/* error - no 2D arrays	*/

	    /* Put each string separately				*/

	    nminor = ((ms_dim *)((char *)dimens + var_data[i].dim[0]))->size;
	    for ( j = 0; j < nminor; j++ ) {
	       dt_count[0] =
		  ((ms_dim *)((char *)dimens + var_data[i].dim[1]))->size;
	       if ( MS_ERROR ==
		   ms_write_string_variable( cdfid, varid, (long)j, -1, 
					    dt_count[0],
					    *((char * *)
					      (values + var_data[i].data_offset)
					      + j) ) )
		  return MS_ERROR;
	    }
	 }
	 else					/* case 1d - error	*/
	    return MS_ERROR;
      }						/* if ( index < 0 )	*/

      else {					/* case 2		*/
	 if ( 0 == var_data[i].ndim )		/* case 2a - error	*/
	    return MS_ERROR;

	 if ( 1 == var_data[i].ndim ) {		/* case 2b		*/
	    if ( NC_CHAR == var_data[i].datatype )
	       return MS_ERROR;			/* error		*/

	    dt_start[0] = (long)index;
	    value = (void *)((char *)values + var_data[i].data_offset);

	    if ( MS_ERROR ==
		ncvarput1( cdfid, varid, dt_start, value ) )
     	       return MS_ERROR;
	 }
	 else if ( 2 == var_data[i].ndim ) {	/* case 2c		*/
	    dt_count[0] = 1L;
	    dt_count[1] =
	       ((ms_dim *)((char *)dimens + var_data[i].dim[1]))->size;
	    if ( NC_CHAR == var_data[i].datatype ) {
	       if ( MS_ERROR ==
		   ms_write_string_variable( cdfid, varid, index, -1,
					    dt_count[1],
					    *((char * *)
					      (values + 
					       var_data[i].data_offset)) ) )
		  return MS_ERROR;
	    }
	    else {
	       dt_start[0] = (long) index;
	       dt_start[1] = 0L;
	       value = 
		  (void *)(*((char * *)(values + var_data[i].data_offset)));
	       if ( MS_ERROR ==
		   ncvarput( cdfid, varid, dt_start, dt_count, value ) )
		  return MS_ERROR;
	    }
	 }
	 else					/* case 2d		*/
	    return MS_ERROR;
      }
   }					/* for ( i = 0; ... )		*/

   return MS_NO_ERROR;

}  /* write_info */

/*
********************************************************************************
*
*  FUNCTION:	read_info	(static)
*
*  DESCRIPTION: Reads variable values from the netCDF file.
*
*  ARGUMENTS:	(int) netCDF file id,
*		(MS_Variables *) variables data structure pointer,
*		(MS_Dimensions *) dimensions data structure pointer,
*		(long) major dimension for array variables,
*		(int) number of data variables,
*		(char *) pointer to variable values data structure
*		(MS_Variable_Data *) pointer to MS_Variable_Data array
*
*  RETURNS:	(int) error code (MS_ERROR / MS_NO_ERROR)
*
*  AUTHOR:	David Stranz Thu Nov  5 08:53:28 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
static int
read_info( int cdfid, MS_Variables * varbls, MS_Dimensions * dimens,
	  long index, int nvals, char * values, MS_Variable_Data * var_data )
#else	/* __STDC__ */
static int
read_info( cdfid, varbls, dimens, index, nvals, values, var_data )
   int			cdfid;
   MS_Variables *	varbls;
   MS_Dimensions *	dimens;
   long			index;
   int			nvals;
   char *		values;
   MS_Variable_Data *	var_data;
#endif	/* not __STDC__ */
{
   int		i;
   long		j;
   int		varid;
   nc_type	datatype;
   int		nAtts;
   int		ndim;
   int		dim_id[3];
   int		isPresent;
   long		dt_start[2];
   long		dt_count[2];
   void *	value;

   /* Error check: all pointers must be non-NULL; nvals > 0		*/

   if ( nvals <= 0 || NULL == varbls || NULL == dimens || NULL == values ||
       NULL == var_data ) {
      return MS_ERROR;
   }

   /* As in writing variable values, there are two major cases:

      Case 1:	If index < 0, we are being asked to read a variable which
      		is non-indexed.

	(1a):	If it has zero dimensionality, it is single-valued (and
		cannot be of type NC_CHAR).
	(1b):	If it ha dimensionality one and is of type NC_CHAR, then
	        it is a simple string, otherwise it is a numeric array.
	(1c):	If the dimensionality is two, then it can only be of type
		NC_CHAR, since 2-D arrays of numerics are not supported as
		non-indexed variables in this implementation.
	(1d):	Dimensionality > 2 is not supported and is an error.

      Case 2:	If index >= 0, then we are being asked to retrieve an
      		indexed variable.

	(2a):	Dimensionality zero is an error.
	(2b):	Dimensionality one with type NC_CHAR is also an error.
		Otherwise, it represent one numeric value of an indexed set.
	(2c):	Dimensionality two is a hyperslab - either a single indexed
		string or an array of numeric values.
	(2d):	Dimensionality > 2 is not supported and is an error.

     In both cases, space is allocated for strings and must be freed by the
     caller after use.  Numeric values are copied directly into their
     storage locations.  For values which occur as arrays, the arrays are
     presumed to be predefined, and the values are stored in the appropriate
     offsets (string pointers or numeric values).			*/

   for ( i = 0; i < nvals; i++ ) {	/* loop over all data fields	*/

      varid = *((int *)((char *)varbls + var_data[i].offset));

      /* Inquire about the variable; this is primarily to determine that
	 the variable is present in the file.  If it is not, we will
	 assign a default value to it.					*/

      if ( MS_ERROR ==
	  ncvarinq( cdfid, varid, (char *)0, &datatype, &ndim, dim_id, &nAtts ) )
	 isPresent = FALSE;
      else
	 isPresent = TRUE;

      /* Check for datatype and dimensionality mismatch.		*/

      if ( isPresent ) {
	 if ( datatype != var_data[i].datatype ) {
	    return MS_ERROR;
	 }
	 if ( ndim != var_data[i].ndim ) {
	    return MS_ERROR;
	 }
      }

      if ( index < 0 ) {			/* case 1		*/
	 if ( 0 == var_data[i].ndim ) {		/* case 1a		*/

	    if ( NC_CHAR == var_data[i].datatype )
	       return MS_ERROR;		/* error - strings are arrays	*/

	    /* Read a non-dimensioned numeric value			*/

	    if ( isPresent ) {
	       dt_start[0] = 0L;
	       value = (void *)((char *)values + var_data[i].data_offset);
	       if ( MS_ERROR ==
		   ncvarget1( cdfid, varid, dt_start, value ) ) {
		  return MS_ERROR;
	       }
	    }
	    else {
	       put_default( var_data[i].datatype, 
			 (void *)
			   ((char *)values + var_data[i].data_offset), 0 );
	    }
	 }
	 else if ( 1 == var_data[i].ndim ) {	/* case 1b		*/
	    dt_count[0] =
	       ((ms_dim *)((char *)dimens + var_data[i].dim[0]))->size;

	    if ( NC_CHAR == var_data[i].datatype ) {	/* get a string	*/
	       *((char * *)(values + var_data[i].data_offset)) =
		  ms_read_string_variable( cdfid, varid, index, -1,
					  dt_count[0] );
	    }
	    else {				/* get an array		*/
	       if ( isPresent ) {
		  dt_start[0] = 0L;
		  if ( MS_ERROR ==
		      ncvarget( cdfid, varid, dt_start, dt_count,
			       (void *)((char *)values + 
					var_data[i].data_offset) ) ) {
		     return MS_ERROR;
		  }
	       }
	       else {				/* assign default	*/
		  for ( j = 0; j < dt_count[0]; j++ ) {
		     put_default( var_data[i].datatype,
			       (void *)(((char *)values + 
				   var_data[i].data_offset)), j );
		  }
	       }
	    }
	 }
	 else if ( 2 == var_data[i].ndim ) {	/* case 1c		*/
	    if ( NC_CHAR != var_data[i].datatype ) {
	       return MS_ERROR;
	    }

	    /* Read an array of strings					*/

	    dt_count[0] =
	       ((ms_dim *)((char *)dimens + var_data[i].dim[0]))->size;
	    for ( j = 0; j < dt_count[0]; j++ ) {
	       *((char * *)(values + var_data[i].data_offset) + j) =
		  ms_read_string_variable( cdfid, varid, j, -1, dt_count[0] );
	    }
	 }
	 else {
	    return MS_ERROR;			/* case 1d		*/
	 }
      }						/* if ( index < 0 )	*/

      else {					/* case 2		*/
	 if ( 0 == var_data[i].ndim )		/* case 2a - error	*/
	    return MS_ERROR;
	 else if ( 1 == var_data[i].ndim ) {	/* case 2b		*/
	    if ( NC_CHAR == var_data[i].datatype ) {
	       return MS_ERROR;			/* no indexed 1D strings*/
	    }

	    dt_start[0] = (long) index;
	    if ( isPresent ) {
	       if ( MS_ERROR ==
		   ncvarget1( cdfid, varid, dt_start,
			     (void *)((values + var_data[i].data_offset)) ) ) {
		  return MS_ERROR;
	       }
	    }
	    else {
	       put_default ( var_data[i].datatype,
			  (void *)
			    ((char *)values + var_data[i].data_offset), 0 );
	    }
	 }
	 else if ( 2 == var_data[i].ndim ) {	/* case 2c		*/
	    dt_count[0] = 1L;
	    dt_count[1] =
	       ((ms_dim *)((char *)dimens + var_data[i].dim[1]))->size;
	    if ( NC_CHAR == var_data[i].datatype ) {
	       *((char * *)(values + var_data[i].data_offset)) =
		  ms_read_string_variable( cdfid, varid, index, -1,
					  dt_count[1] );
	    }
	    else {
	       if ( isPresent ) {
		  dt_start[0] = (long)index;
		  dt_start[1] = 0L;

		  value =
		     (void *)(*((char * *)(values + var_data[i].data_offset)));

		  if ( MS_ERROR ==
		      ncvarget( cdfid, varid, dt_start, dt_count, value ) ) {
		     return MS_ERROR;
		  }
	       }
	       else {
		  for ( j = 0; j < dt_count[1]; j++ ) {
		     put_default( var_data[i].datatype,
				 (void *)
				 ((char *)(values + 
					   var_data[i].data_offset)), j );
		  }
	       }
	    }
	 }
	 else {					/* case 2d		*/
	    return MS_ERROR;
	 }
      }	/* else		*/
   }	/* for ( i = 0; ... )	*/

   return MS_NO_ERROR;

}  /* read_info */


/*
********************************************************************************
*
*  FUNCTION:	client_index	(static)
*
*  DESCRIPTION:	Looks up the index of the client in the ms_clients array given
*		the netCDF file id.
*
*  ARGUMENTS:	(int) netCDF id
*
*  RETURNS:	(int) client index (MS_ERROR on error)
*
*  AUTHOR:	David Stranz Mon Nov 30 08:47:08 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
static int
client_index( int cdfid )
#else	/* __STDC__ */
static int
client_index( cdfid )
   int		cdfid;
#endif	/* not __STDC__ */
{
   int	index;

   if ( NULL == ms_clients || 0 == ms_client_count )
      return MS_ERROR;

   for ( index = 0; index < ms_client_count; index++ ) {
      if ( cdfid == ms_clients[index]->cdfid )
	 return index;
   }

   return MS_ERROR;

}  /* client_index */

/*
********************************************************************************
*
*  Exported (global) functions
*
********************************************************************************
*/

/*
********************************************************************************
*
*  FUNCTION:	ms_write_dimensions
*
*  DESCRIPTION:	Creates dimension definitions in the output netCDF file
*
*  ARGUMENTS:	(int) netCDF file id, (long) number of scans, 
*		(long) instrument component count,
*
*  RETURNS:	(int) error code (MS_ERROR if error, MS_NO_ERROR otherwise)
*
*  AUTHOR:	David Stranz Thu Oct 29 10:12:59 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
int
ms_write_dimensions( int cdfid, long nscans, long instrument_count )
#else	/* __STDC__ */
int
ms_write_dimensions( cdfid, nscans, instrument_count )
   int			cdfid;
   long			nscans;
   long			instrument_count;
#endif	/* not __STDC__ */
{
   int			i;
   int			index;
   MS_Dimensions *	dims;

   /* Error check: nscans must be > 0					*/

   if ( nscans <= 0 )
      return MS_ERROR;

   if ( MS_ERROR == (index = client_index( cdfid )) )
      return MS_ERROR;

   dims = &(ms_clients[index]->dims);

   /* Define the dimensions in the data array first			*/

   for ( i = 0; i < nDims; i++ ) {
      if ( MS_ERROR ==
	  (((ms_dim *)((char *)dims + ms_dimensions[i].offset))->id =
	   ncdimdef( cdfid, ms_dimensions[i].name, ms_dimensions[i].size )) )
	 return MS_ERROR;
      ((ms_dim *)((char *)dims + ms_dimensions[i].offset))->size =
	 ms_dimensions[i].size;
   }

   /* Next, define the scan_number and (if applicable) the instrument_number
      dimensions.							*/

   if ( MS_ERROR ==
       (dims->scan_number_dim.id = ncdimdef( cdfid, "scan_number", nscans )) )
      return MS_ERROR;
   dims->scan_number_dim.size = nscans;

   if ( instrument_count > 0 ) {
      if ( MS_ERROR ==
	  (dims->instrument_number_dim.id = 
	   ncdimdef( cdfid, "instrument_number", instrument_count )) )
	 return MS_ERROR;
      dims->instrument_number_dim.size = instrument_count;
   }
   else {
      dims->instrument_number_dim.id   = MS_ERROR;
      dims->instrument_number_dim.size = 0L;
   }

   return MS_NO_ERROR;

}  /* ms_write_dimensions */

/*
********************************************************************************
*
*  FUNCTION:	ms_read_dimensions
*
*  DESCRIPTION:	Reads dimension definitions from the netCDF file
*
*  ARGUMENTS:	(int) netCDF file id
*
*  RETURNS:	(int) error code (MS_ERROR / MS_NO_ERROR)
*
*  AUTHOR:	David Stranz Wed Nov  4 13:44:07 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
int
ms_read_dimensions( int cdfid )
#else	/* __STDC__ */
int
ms_read_dimensions( cdfid )
   int			cdfid;
#endif	/* not __STDC__ */
{
   int			i;
   int			id;
   long			size;
   int			index;
   MS_Dimensions *	dims;

   if ( MS_ERROR == (index = client_index( cdfid )) )
      return MS_ERROR;

   dims = &(ms_clients[index]->dims);

   /* Read all dimensions from the dimensions array first.		*/

   for ( i = 0; i < nDims; i++ ) {
      size = 0L;
      id = ncdimid( cdfid, ms_dimensions[i].name );
      if ( -1 != id )
	 (void)ncdiminq( cdfid, id, (char *)0, &size );
      ((ms_dim *)((char *)dims + ms_dimensions[i].offset))->id = id;
      ((ms_dim *)((char *)dims + ms_dimensions[i].offset))->size = size;
   }

   /* Then get the scan_number and instrument_number dimenensions	*/

   dims->scan_number_dim.id = ncdimid( cdfid, "scan_number" );
   if ( -1 != dims->scan_number_dim.id )
      (void)ncdiminq( cdfid, dims->scan_number_dim.id, (char *)0,
		     &(dims->scan_number_dim.size) );

   dims->instrument_number_dim.id = ncdimid( cdfid, "instrument_number" );
   if ( -1 != dims->instrument_number_dim.id )
      (void)ncdiminq( cdfid, dims->instrument_number_dim.id, (char *)0,
		     &(dims->instrument_number_dim.size) );

   /* Get the scan group dimensions, if any				*/

   dims->group_number_dim.id = ncdimid( cdfid, "group_number" );
   if ( -1 != dims->group_number_dim.id )
      (void) ncdiminq( cdfid, dims->group_number_dim.id, (char *)0,
		      &(dims->group_number_dim.size) );

   dims->group_max_masses_dim.id = ncdimid( cdfid, "group_max_masses" );
   if ( -1 != dims->group_max_masses_dim.id )
      (void) ncdiminq( cdfid, dims->group_max_masses_dim.id, (char *)0,
		      &(dims->group_max_masses_dim.size) );

   return MS_NO_ERROR;

}  /* ms_read_dimensions */

/*
********************************************************************************
*
*  FUNCTION:	ms_write_variables
*
*  DESCRIPTION: Defines all appropriate variables to the netCDF file.  If
*		the experiment type is library, library definitions are
*		written.
*
*  ARGUMENTS:	(int) netCDF file id, (ms_admin_expt_t) experiment type,
*		(ms_data_format_t) mass format,
*		(ms_data_format_t) time format,
*		(ms_data_format_t) intensity format,
*		(int) TRUE if mass data is present,
*		(int) TRUE if time data is present (one or both of these flags
*		      must be TRUE),
*
*  RETURNS:	(int) error code (MS_ERROR / MS_NO_ERROR)
*
*  AUTHOR:	David Stranz Thu Oct 29 14:01:26 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
int
ms_write_variables( int cdfid, ms_admin_expt_t expt_type,
		       ms_data_format_t mass_fmt, ms_data_format_t time_fmt,
		       ms_data_format_t intensity_fmt,
		       int do_masses, int do_times )
#else	/* __STDC__ */
int
ms_write_variables( cdfid, expt_type, mass_fmt, time_fmt, intensity_fmt,
		        do_masses, do_times )
   int			cdfid;
   ms_admin_expt_t	expt_type;
   ms_data_format_t	mass_fmt;
   ms_data_format_t	time_fmt;
   ms_data_format_t	intensity_fmt;
   int			do_masses;
   int			do_times;
#endif	/* not __STDC__ */
{
   int			dim_ids[2];
   nc_type		datatype;
   int			index;
   MS_Dimensions *	dims;
   MS_Variables *	vars;

   /* Error check: one or both of the flags must be TRUE		*/

   if ( !(do_masses || do_times) ) {
      return MS_ERROR;
   }

   if ( MS_ERROR == (index = client_index( cdfid )) )
      return MS_ERROR;

   dims = &(ms_clients[index]->dims);
   vars = &(ms_clients[index]->vars);

   ms_clients[index]->expt_type = expt_type;
   ms_clients[index]->mass_type = mass_fmt;
   ms_clients[index]->time_type = time_fmt;
   ms_clients[index]->inty_type = intensity_fmt;
   ms_clients[index]->has_masses = do_masses;
   ms_clients[index]->has_times = do_times;

   /*  Write the general variable definitions				*/

   if ( MS_ERROR ==
       write_variables( cdfid, nError, vars, dims, error_variables ) )
      return MS_ERROR;

   if ( MS_ERROR ==
       write_variables( cdfid, nRawP, vars, dims, raw_variables ) )
      return MS_ERROR;

   /* Three variables are used to keep track of the per-scan data:
      scan_index provides the offset into the mass, time, and intensity
      	arrays for the start of each scan.
      point_count keeps a count of the number of peaks in each scan.
      flag_count keeps a count of the number of flags in each scan.	*/

   dim_ids[0] = dims->scan_number_dim.id;
   if ( MS_ERROR ==
       (vars->scan_index_id =
	ncvardef( cdfid, "scan_index", NC_LONG, 1, dim_ids )) )
      return( MS_ERROR );

   if ( MS_ERROR ==
       (vars->point_count_id =
	ncvardef( cdfid, "point_count", NC_LONG, 1, dim_ids )) )
      return( MS_ERROR );

   if ( MS_ERROR ==
       (vars->flag_count_id =
	ncvardef( cdfid, "flag_count", NC_LONG, 1, dim_ids )) )
      return( MS_ERROR );

   /*  Write the mass, time, and intensity value variables.  These have
       no predetermined data type, so must be defined at run time	*/

   if ( do_masses ) {
      datatype = format_to_datatype( ms_clients[index]->mass_type, NC_SHORT );
      dim_ids[0] = dims->point_number_dim.id;
      if ( MS_ERROR ==
	  (vars->mass_values_id =
	   ncvardef( cdfid, "mass_values", datatype, 1, dim_ids )) )
	 return( MS_ERROR );
   }
   else
      vars->mass_values_id = -1;

   if ( do_times ) {
      datatype = format_to_datatype( ms_clients[index]->time_type, NC_SHORT );
      dim_ids[0] = dims->point_number_dim.id;
      if ( MS_ERROR ==
	  (vars->time_values_id =
	   ncvardef( cdfid, "time_values", datatype, 1, dim_ids )) )
	 return( MS_ERROR );
   }
   else
      vars->time_values_id = -1;

   datatype = format_to_datatype( ms_clients[index]->inty_type, NC_LONG );
   dim_ids[0] = dims->point_number_dim.id;
   if ( MS_ERROR ==
       (vars->intensity_values_id =
	ncvardef( cdfid, "intensity_values", datatype, 1, dim_ids )) )
      return( MS_ERROR );

   /* If appropriate, write the instrument and library variables definitions.
      Instrument variables are written if the instrument_number dimension id
      has been defined (i.e. != -1).  Library variables are written if
      the experiment type is "expt_library".				*/

   if ( MS_ERROR != dims->instrument_number_dim.id ) {
      if ( MS_ERROR == 
	  write_variables( cdfid, nInst, vars, dims, instrument_variables ) )
	 return MS_ERROR;
   }

   if ( expt_library == expt_type ) {
      if ( MS_ERROR == 
	  write_variables( cdfid, nLib, vars, dims, library_variables ) )
	 return MS_ERROR;
   }

   /* All done, all OK!							*/

   return MS_NO_ERROR;

}  /* ms_write_variables */

/*
********************************************************************************
*
*  FUNCTION:	ms_read_variables
*
*  DESCRIPTION:	Reads variable definitions from the netCDF file
*
*  ARGUMENTS:	(int) netCDF file id
*
*  RETURNS:	(int) error code (MS_ERROR / MS_NO_ERROR)
*
*  AUTHOR:	David Stranz Mon Nov  9 12:40:11 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
int
ms_read_variables( int cdfid )
#else	/* __STDC__ */
int
ms_read_variables( cdfid )
   int		cdfid;
#endif	/* not __STDC__ */
{
   int			expt_type;
   int			index;
   MS_Dimensions *	dims;
   MS_Variables *	vars;
   int			ndims;
   int			natts;
   nc_type		inq_type;
   int			dim_id[5];

   if ( MS_ERROR == (index = client_index( cdfid )) )
      return MS_ERROR;

   dims = &(ms_clients[index]->dims);
   vars = &(ms_clients[index]->vars);

   /* Read array variables first					*/

   if ( MS_ERROR ==
       read_variables( cdfid, nError, vars, dims, error_variables ) )
      return MS_ERROR;

   if ( MS_ERROR ==
       read_variables( cdfid, nRawP, vars, dims, raw_variables ) )
      return MS_ERROR;

   /* Read scan data variables						*/

   vars->scan_index_id		= ncvarid( cdfid, "scan_index" );
   vars->point_count_id		= ncvarid( cdfid, "point_count" );
   vars->flag_count_id		= ncvarid( cdfid, "flag_count" );

   inq_type = NC_SHORT;
   if ( MS_ERROR != (vars->mass_values_id = ncvarid( cdfid, "mass_values" )) ) {
      ms_clients[index]->has_masses = TRUE;
      ncvarinq( cdfid, vars->mass_values_id, (char *)0, &inq_type,
	       &ndims, dim_id, &natts );
   }
   ms_clients[index]->mass_type = datatype_to_format( inq_type, data_short );
      
   inq_type = NC_SHORT;
   if ( MS_ERROR != (vars->time_values_id = ncvarid( cdfid, "time_values" )) ) {
      ms_clients[index]->has_times = TRUE;
      ncvarinq( cdfid, vars->time_values_id, (char *)0, &inq_type,
	       &ndims, dim_id, &natts );
   }
   ms_clients[index]->time_type = datatype_to_format( inq_type, data_short );

   inq_type = NC_LONG;
   if ( MS_ERROR != 
       (vars->intensity_values_id = ncvarid( cdfid, "intensity_values" )) ) {
      ncvarinq( cdfid, vars->intensity_values_id, (char *)0, &inq_type,
	       &ndims, dim_id, &natts );
   }
   ms_clients[index]->inty_type = datatype_to_format( inq_type, data_long );

   /* If the instrument_number_dim dimension has been defined, then instrument
      data is present in the file.  Retrieve the ids for the variables.	*/

   if ( -1 != dims->instrument_number_dim.id ) {
      if ( MS_ERROR ==
	  read_variables( cdfid, nInst, vars, dims, instrument_variables ) )
	 return MS_ERROR;
   }

   /* Check the experiment type; if it is a library type, then we need to
      retrieve the library variables also.				*/

   if ( MS_ERROR == (expt_type =
		     ms_read_enum_attribute( cdfid, NC_GLOBAL,
					    "experiment_type" )) )
      return MS_ERROR;

   ms_clients[index]->expt_type = (ms_admin_expt_t)expt_type;
   if ( expt_library == ms_clients[index]->expt_type ) {
      if ( MS_ERROR ==
	  read_variables( cdfid, nLib, vars, dims, library_variables ) )
	 return MS_ERROR;
   }

   return MS_NO_ERROR;
   
}  /* ms_read_variables */

/*
********************************************************************************
*
*  FUNCTION:	ms_write_global
*
*  DESCRIPTION:	Writes global and local attribute values to the netCDF file
*
*  ARGUMENTS:	(int) netCDF file id,
*		(MS_Admin_Data *) administrative data structure pointer,
*		(MS_Sample_Data *) sample data structure pointer,
*		(MS_Test_Data *) test method data structure pointer,
*		(MS_Raw_Data_Global *) raw data global data structure pointer
*
*  RETURNS:	(int) error code (MS_ERROR / MS_NO_ERROR)
*
*  AUTHOR:	David Stranz Fri Oct 30 09:02:17 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
int
ms_write_global( int cdfid, MS_Admin_Data * admin_data,
		    MS_Sample_Data * sample_data, MS_Test_Data * test_data,
		    MS_Raw_Data_Global * raw_data )
#else	/* __STDC__ */
int
ms_write_global( cdfid, admin_data, sample_data, test_data, raw_data )
   int			cdfid;
   MS_Admin_Data *	admin_data;
   MS_Sample_Data *	sample_data;
   MS_Test_Data *	test_data;
   MS_Raw_Data_Global *	raw_data;
#endif	/* not __STDC__ */
{
   int			err_code;
   int			index;
   MS_Dimensions *	dims;
   MS_Variables *	vars;
   extern int		ncopts;
   int			saveOpts;

   if ( NULL == admin_data || NULL == sample_data || NULL == test_data ||
       NULL == raw_data )
      return MS_ERROR;

   if ( MS_ERROR == (index = client_index( cdfid )) )
      return MS_ERROR;

   dims = &(ms_clients[index]->dims);
   vars = &(ms_clients[index]->vars);

   /* Put the netCDF file into "define mode" (even though it may already be
      there).  If it is already in define mode, this call returns an error,
      so we ignore it.							*/

   saveOpts = ncopts;
   ncopts = 0;
   (void)ncredef( cdfid );
   ncopts = saveOpts;

   /* The values of the first four Administrative-ID attributes
      are a property of the implementation revision level, and are
      defined as static character strings in the header file.
      Because of the mechanism used to initialize attribute data structures,
      they are loaded, written to the file, then the attribute pointers are
      set to NULL.  This will avoid errors later when the MS_Admin_Structure
      is cleared.							*/

   admin_data->dataset_completeness = ms_completeness_att;
   admin_data->ms_template_revision = ms_template_att;
   admin_data->netcdf_revision = ms_netcdf_att;
   admin_data->languages = ms_languages_att;

   err_code = write_attributes( cdfid, nAdminA, vars, admin_attributes, 
			       (char *)admin_data );

   admin_data->dataset_completeness = NULL;
   admin_data->ms_template_revision = NULL;
   admin_data->netcdf_revision = NULL;
   admin_data->languages = NULL;

   if ( MS_ERROR == err_code )
      return MS_ERROR;

   if ( MS_ERROR == 
       write_attributes( cdfid, nSamp, vars, sample_attributes, 
			(char *)sample_data ) )
      return MS_ERROR;

   if ( MS_ERROR == 
       write_attributes( cdfid, nTest, vars, test_attributes, 
			(char *)test_data ) )
      return MS_ERROR;

   if ( MS_ERROR == 
       write_attributes( cdfid, nRaw, vars, raw_data_attributes,
			(char *)raw_data ) )
      return MS_ERROR;

   /* Now, place the file in data mode					*/

   saveOpts = ncopts;
   ncopts = 0;
   (void)ncendef( cdfid );
   ncopts = saveOpts;

   /* Write out the error variable					*/

   if ( MS_ERROR ==
       write_info( cdfid, vars, dims, (long) -1, nError, (char *)admin_data,
		  error_variables ) )
      return MS_ERROR;

   return MS_NO_ERROR;

}  /* ms_write_global */

/*
********************************************************************************
*
*  FUNCTION:	ms_read_global
*
*  DESCRIPTION:	Reads attribute values from the netCDF file
*
*  ARGUMENTS:	(int) netCDF file id,
*		(MS_Admin_data *) administrative data structure pointer,
*		(MS_Sample_Data *) sample data structure pointer,
*		(MS_Test_Data *) test method data structure pointer,
*		(MS_Raw_Data_Global *) raw data global data structure pointer
*
*  RETURNS:	(int) error code (MS_ERROR / MS_NO_ERROR)
*
*  AUTHOR:	David Stranz Tue Nov 10 10:00:49 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
int
ms_read_global( int cdfid, MS_Admin_Data * admin_data, 
		   MS_Sample_Data * sample_data, MS_Test_Data * test_data,
		   MS_Raw_Data_Global * raw_data )
#else	/* __STDC__ */
int
ms_read_global( cdfid, admin_data, sample_data, test_data, raw_data )
   int			cdfid;
   MS_Admin_Data *	admin_data;
   MS_Sample_Data *	sample_data;
   MS_Test_Data *	test_data;
   MS_Raw_Data_Global *	raw_data;
#endif	/* not __STDC__ */
{
   int			index;
   MS_Dimensions *	dims;
   MS_Variables *	vars;

   if ( NULL == admin_data || NULL == sample_data || NULL == test_data ||
       NULL == raw_data )
      return MS_ERROR;

   if ( MS_ERROR == (index = client_index( cdfid )) )
      return MS_ERROR;

   dims = &(ms_clients[index]->dims);
   vars = &(ms_clients[index]->vars);

   if ( MS_ERROR ==
       read_attributes( cdfid, nAdminA, vars, admin_attributes, 
		       (char *)admin_data ) )
      return MS_ERROR;

   if ( MS_ERROR ==
       read_attributes( cdfid, nSamp, vars, sample_attributes, 
		       (char *)sample_data ) )
      return MS_ERROR;

   if ( MS_ERROR ==
       read_attributes( cdfid, nTest, vars, test_attributes, 
		       (char *)test_data ) )
      return MS_ERROR;

   if ( MS_ERROR ==
       read_attributes( cdfid, nRaw, vars, raw_data_attributes, 
		       (char *)raw_data ) )
      return MS_ERROR;

   /* Now, read in the error variable value				*/

   if ( MS_ERROR ==
       read_info( cdfid, vars, dims, (long) -1, nError, (char *)admin_data,
		 error_variables ) )
      return MS_ERROR;

   /* Finally, put miscellaneous information into the admin_data and
      raw_data structures.						*/

   raw_data->nscans = dims->scan_number_dim.size;
   admin_data->number_instrument_components = dims->instrument_number_dim.size;

   raw_data->has_masses = ms_clients[index]->has_masses;
   raw_data->has_times = ms_clients[index]->has_times;

   return MS_NO_ERROR;

}  /* ms_read_global */

/*
********************************************************************************
*
*  FUNCTION:	ms_write_instrument
*
*  DESCRIPTION:	Writes the Instrument-ID information for the nth instrument
*		to the netCDF file.
*
*  ARGUMENTS:	(int) netCDF file id,
*		(MS_Instrument_Data *) instrument data structure pointer
*
*  RETURNS:	(int) error code (MS_ERROR / MS_NO_ERROR)
*
*  AUTHOR:	David Stranz Mon Nov  2 08:35:20 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
int
ms_write_instrument( int cdfid, MS_Instrument_Data * inst_data )
#else	/* __STDC__ */
int
ms_write_instrument( cdfid, inst_data )
   int			cdfid;
   MS_Instrument_Data *	inst_data;
#endif	/* not __STDC__ */
{
   int			index;
   MS_Dimensions *	dims;
   MS_Variables *	vars;

   if ( NULL == inst_data )
      return MS_ERROR;

   if ( MS_ERROR == (index = client_index( cdfid )) )
      return MS_ERROR;

   dims = &(ms_clients[index]->dims);
   vars = &(ms_clients[index]->vars);

   if ( MS_ERROR ==
       write_info( cdfid, vars, dims, inst_data->inst_no, nInst, 
		  (char *)inst_data, instrument_variables ) )
      return MS_ERROR;

   return MS_NO_ERROR;

}  /* ms_write_instrument */

/*
********************************************************************************
*
*  FUNCTION:	ms_read_instrument
*
*  DESCRIPTION:	Reads Instrument-ID information for the nth instrument from
*		the netCDF file.  The MS_Instrument_Data structure field
*		"inst_no" must be pre-loaded with the instrument number
*		desired.
*
*  ARGUMENTS:	(int) netCDF file id
*		(MS_Instrument_Data *) instrument data structure pointer
*
*  RETURNS:	(int) error code (MS_ERROR / MS_NO_ERROR)
*
*  AUTHOR:	David Stranz Tue Nov 10 10:09:21 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
int
ms_read_instrument( int cdfid, MS_Instrument_Data * inst_data )
#else	/* __STDC__ */
int
ms_read_instrument( cdfid, inst_data )
   int			cdfid;
   MS_Instrument_Data *	inst_data;
#endif	/* not __STDC__ */
{
   int			index;
   MS_Dimensions *	dims;
   MS_Variables *	vars;

   if ( NULL == inst_data )
      return MS_ERROR;

   if ( MS_ERROR == (index = client_index( cdfid )) )
      return MS_ERROR;

   dims = &(ms_clients[index]->dims);
   vars = &(ms_clients[index]->vars);

   if ( inst_data->inst_no >= dims->instrument_number_dim.size )
      return MS_ERROR;

   if ( MS_ERROR ==
       read_info( cdfid, vars, dims, inst_data->inst_no, nInst,
		 (char *)inst_data, instrument_variables ) )
      return MS_ERROR;

   return MS_NO_ERROR;

}  /* ms_read_instrument */

/*
********************************************************************************
*
*  FUNCTION:	ms_write_per_scan
*
*  DESCRIPTION:	Writes raw and library per-scan information to the data file.
*		Either data stucture pointer can be NULL, in which case it
*		is ignored.
*
*  ARGUMENTS:	(int) netCDF file id,
*		(MS_Raw_Per_Scan *) raw data per scan data structure pointer
*		(MS_Raw_Library *) library per scan data structure pointer
*
*  RETURNS:	(int) error code (MS_ERROR / MS_NO_ERROR)
*
*  AUTHOR:	David Stranz Wed Nov  4 09:29:08 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
int
ms_write_per_scan( int cdfid, MS_Raw_Per_Scan * raw_data, 
		       MS_Raw_Library * lib_data )
#else	/* __STDC__ */
int
ms_write_per_scan( cdfid, raw_data, lib_data )
   int			cdfid;
   MS_Raw_Per_Scan *	raw_data;
   MS_Raw_Library *	lib_data;
#endif	/* not __STDC__ */
{
   int			varid;
   long			dt_start[2];
   long			dt_count[2];
   void *		flags;
   nc_type		flag_type;
   int			index;
   MS_Dimensions *	dims;
   MS_Variables *	vars;

   if ( MS_ERROR == (index = client_index( cdfid )) )
      return MS_ERROR;

   dims = &(ms_clients[index]->dims);
   vars = &(ms_clients[index]->vars);

   if ( raw_data ) {

      /* First, we write out the type-invariant data to the file.  These are
	 the fields defined in the MS_Variable_Data raw_variables array.*/

      if ( MS_ERROR ==
	  write_info( cdfid, vars, dims, raw_data->scan_no, nRawP, 
		     (char *)raw_data, raw_variables ) )
	 return MS_ERROR;

      /* Next, write out the mass, time, and intensity data.  For all scans
	 in the file, either both masses and times, or only one of them may
	 be present; in all cases, the same arrays MUST be present for
	 all scans.

	 NULL pointers must be passed for mass or time arrays which
	 are not used.

	 If there are no peaks for a given scan, then NULL pointers should be
	 passed for all arrays, and points and flags variables set to zero.*/

      /* Write out the number of points and number of flags to the point_count
	 and flag_count arrays, respectively.				*/

      dt_start[0] = raw_data->scan_no;
      if ( MS_ERROR ==
	  ncvarput1( cdfid, vars->point_count_id, dt_start,
		    (void *)&(raw_data->points) ) )
	 return MS_ERROR;

      if ( MS_ERROR ==
	  ncvarput1( cdfid, vars->flag_count_id, dt_start,
		    (void *)&(raw_data->flags) ) )
	 return MS_ERROR;

      /* Write out the start-of-scan index.				*/

      if ( MS_ERROR ==
	  ncvarput1( cdfid, vars->scan_index_id, dt_start,
		    (void *)&(ms_clients[index]->total_count) ) )
	 return MS_ERROR;

      /* Next, write out the point data; if there are no points,
	 then we are done with this scan.				*/

      if ( raw_data->points > 0L ) {
	 dt_start[0] = ms_clients[index]->total_count;	/* offset of this scan */
	 dt_count[0] = raw_data->points;	/* number of points	*/

	 if ( MS_ERROR ==
	     ncvarput( cdfid, vars->intensity_values_id, dt_start, dt_count,
		      (void *) raw_data->intensities ) )
	    return MS_ERROR;

	 if ( raw_data->masses && ms_clients[index]->has_masses ) {
	    if ( MS_ERROR ==
		ncvarput( cdfid, vars->mass_values_id, dt_start, dt_count,
			 (void *)raw_data->masses ) )
	       return MS_ERROR;
	 }

	 if ( raw_data->times && ms_clients[index]->has_times ) {
	    if ( MS_ERROR ==
		ncvarput( cdfid, vars->time_values_id, dt_start, dt_count,
			 (void *)raw_data->times ) )
	       return MS_ERROR;
	 }

	 /* Update point count						*/

	 ms_clients[index]->total_count += raw_data->points;

	 if ( raw_data->flags > 0L ) {
	    dt_start[0] = ms_clients[index]->total_count; /* offset of flags */
	    dt_count[0] = raw_data->flags;	/* number of flags	*/
	 
	    /* If masses array is present, then tack flag_peaks onto the end
	       of the masses array; otherwise, stick it onto the times	*/

	    if ( raw_data->masses ) {
	       varid = vars->mass_values_id;
	       flag_type = format_to_datatype( ms_clients[index]->mass_type,
					      NC_SHORT );
	    }
	    else {
	       varid = vars->time_values_id;
	       flag_type = format_to_datatype( ms_clients[index]->time_type,
					       NC_SHORT );
	    }

	    /* Make a temporary array (if required) to effect the cast from
	       NC_LONG to whatever the flag type is in the file.  If the
	       flag type is already NC_LONG, then do not make a copy.	*/

	    if ( flag_type != NC_LONG ) {
	       if ( NULL == (flags = make_array( raw_data->flags, flag_type )) )
		  return MS_ERROR;
	       ms_copy_array( raw_data->flag_peaks, data_long, raw_data->flags,
			     flags, datatype_to_format( flag_type, data_long ),
			     FALSE );
	    }
	    else
	       flags = raw_data->flag_peaks;

	    if ( MS_ERROR ==
		ncvarput( cdfid, varid, dt_start, dt_count, (void *)flags ) ) {
	       if ( flag_type != NC_LONG )
		  free( flags );
	       return MS_ERROR;
	    }

	    if ( flag_type != NC_LONG )
	       free( flags );

	    /* Stick the flags onto the end of the intensities array	*/

	    if ( ms_clients[index]->inty_type != data_short ) {
	       if ( NULL == 
		   (flags = 
		    make_array( raw_data->flags,
			       format_to_datatype( ms_clients[index]->inty_type,
						  NC_SHORT ) )) )
		  return MS_ERROR;
	       ms_copy_array( raw_data->flag_values, data_short,
			     raw_data->flags, flags,
			     ms_clients[index]->inty_type, FALSE );
	    }
	    else
	       flags = raw_data->flag_values;

	    if ( MS_ERROR ==
		ncvarput( cdfid, vars->intensity_values_id, dt_start, dt_count,
			 (void *)flags ) ) {
	       if ( ms_clients[index]->inty_type != data_short )
		  free( flags );
	       return MS_ERROR;
	    }

	    if ( ms_clients[index]->inty_type != data_short )
	       free( flags );
    
	    /* Update total point count to reflect addition of flags.	*/

	    ms_clients[index]->total_count += raw_data->flags;
	 }	/* if ( raw_data->flags > 0L )	*/
      }		/* if (raw_data->points > 0L )	*/
   }		/* if ( raw_data )		*/

   /* Write out library data, if present				*/

   if ( lib_data && (expt_library == ms_clients[index]->expt_type) ) {
      if ( MS_ERROR ==
	  write_info( cdfid, vars, dims, lib_data->scan_no, nLib, 
		     (char *)lib_data, library_variables ) )
	 return MS_ERROR;
   }

   return MS_NO_ERROR;

}  /* ms_write_per_scan */

/*
********************************************************************************
*
*  FUNCTION:	ms_read_per_scan
*
*  DESCRIPTION:	Reads raw and library per-scan data from the netCDF file.  If
*		a NULL pointer is passed in for either data structure, that
*		data type will be ignored.
*
*  ARGUMENTS:	(int) netCDF file id, 
*		(MS_Raw_Per_Scan *) raw data per-scan data structure pointer
*		(MS_Reaw_Library *) library per-scan data structure pointer
*
*  RETURNS:	(int) error code (MS_ERROR / MS_NO_ERROR)
*
*  AUTHOR:	David Stranz Thu Nov  5 13:09:33 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
int
ms_read_per_scan( int cdfid, MS_Raw_Per_Scan * raw_data, 
		 MS_Raw_Library * lib_data )
#else	/* __STDC__ */
int
ms_read_per_scan( cdfid, raw_data, lib_data )
   int			cdfid;
   MS_Raw_Per_Scan *	raw_data;
   MS_Raw_Library *	lib_data;
#endif	/* not __STDC__ */
{
   long			scan_index;
   long			dt_start[3];
   long			dt_count[3];
   nc_type		mass_type;
   nc_type		time_type;
   nc_type		inty_type;
   nc_type		flag_type = NC_LONG;	/* to keep lint happy	*/
   int			flag_id;
   int			index;
   MS_Dimensions *	dims;
   MS_Variables *	vars;
   void *		flags;

   if ( MS_ERROR == (index = client_index( cdfid )) )
      return MS_ERROR;

   dims = &(ms_clients[index]->dims);
   vars = &(ms_clients[index]->vars);

   flag_id = vars->time_values_id;	/* also for lint	*/
   mass_type = format_to_datatype( ms_clients[index]->mass_type, NC_SHORT );
   time_type = format_to_datatype( ms_clients[index]->time_type, NC_SHORT );
   inty_type = format_to_datatype( ms_clients[index]->inty_type, NC_LONG  );

   if ( raw_data ) {

   /* Retrieve the type-invariant data from the file			*/

      if ( MS_ERROR ==
	  read_info( cdfid, vars, dims, raw_data->scan_no, nRawP,
		    (char *)raw_data, raw_variables ) ) {
	 return MS_ERROR;
      }

      /* Next, retrieve the mass, time, and intensity data.  Because this
	 is of variable type, we have to create arrays of the apropriate
	 type to return to the caller.					*/

      /* First, get the scan offset, point count, and flag count	*/

      dt_start[0] = raw_data->scan_no;
      if ( MS_ERROR ==
	  ncvarget1( cdfid, vars->scan_index_id, dt_start,
		    (void *)&scan_index ) ) {
	 return MS_ERROR;
      }

      if ( MS_ERROR ==
	  ncvarget1( cdfid, vars->point_count_id, dt_start,
		    (void *)&(raw_data->points) ) ) {
	 return MS_ERROR;
      }

      if ( MS_ERROR ==
	  ncvarget1( cdfid, vars->flag_count_id, dt_start,
		    (void *)&(raw_data->flags) ) ) {
	 return MS_ERROR;
      }

      /* Do some initialization						*/

      raw_data->masses		= NULL;
      raw_data->times		= NULL;
      raw_data->intensities	= NULL;
      raw_data->flag_peaks	= NULL;
      raw_data->flag_values	= NULL;

      /* If no points, then we simply return the NULL arrays		*/

      if ( raw_data->points > 0 ) {

	 /*  Allocate mass, time and intensity arrays, then read in
	     the data							*/

	 dt_start[0] = scan_index;
	 dt_count[0] = raw_data->points;

	 if ( ms_clients[index]->has_times ) {
	    flag_type = format_to_datatype( ms_clients[index]->time_type,
					   NC_SHORT );
	    flag_id = vars->time_values_id;

	    if ( NULL ==
		(raw_data->times = make_array( raw_data->points, time_type )) )
	       return MS_ERROR;

	    if ( MS_ERROR ==
		ncvarget( cdfid, vars->time_values_id, dt_start, dt_count,
			 raw_data->times ) )
	       return MS_ERROR;
	 }

	 if ( ms_clients[index]->has_masses ) {
	    flag_type = mass_type;	/* Note: may override previous type */
	    flag_id = vars->mass_values_id;

	    if ( NULL == 
		(raw_data->masses = make_array( raw_data->points, mass_type )) )
	       return MS_ERROR;

	    if ( MS_ERROR ==
		ncvarget( cdfid, vars->mass_values_id, dt_start, dt_count,
			 raw_data->masses ) )
	       return MS_ERROR;
	 }

	 if ( NULL ==
	     (raw_data->intensities =
	      make_array( raw_data->points, inty_type )) )
	    return MS_ERROR;

	 if ( MS_ERROR ==
	     ncvarget( cdfid, vars->intensity_values_id, dt_start, dt_count,
		      raw_data->intensities ) )
	    return MS_ERROR;

	 /* Retrieve the flags, if any.  If masses are present, the type of the
	    flag_peaks is the same as that of the masses; otherwise it is the
	    same as that of the times.  The type of flag_values is always the
	    same as that of the intensities.				*/

	 if ( raw_data->flags > 0 ) {
	    dt_start[0] = scan_index + raw_data->points;
	    dt_count[0] = raw_data->flags;
	    if ( NULL ==
		(raw_data->flag_peaks = (long *) 
		 make_array( raw_data->flags, NC_LONG )) )
	       return MS_ERROR;
	    if ( NULL == (flags = make_array( raw_data->flags, flag_type )) )
	       return MS_ERROR;

	    if ( MS_ERROR ==
		ncvarget( cdfid, flag_id, dt_start, dt_count, flags ) ) {
	       free( flags );
	       return MS_ERROR;
	    }
	    ms_copy_array( flags, datatype_to_format( flag_type, data_long ),
			  raw_data->flags, raw_data->flag_peaks, data_long,
			  FALSE );
	    free( flags );

	    if ( NULL ==
		(raw_data->flag_values = (short *)
		 make_array( raw_data->flags, NC_SHORT )) )
	       return MS_ERROR;

	    if ( NULL == (flags = make_array( raw_data->flags, inty_type )) )
	       return MS_ERROR;

	    if ( MS_ERROR ==
		ncvarget( cdfid, vars->intensity_values_id, dt_start, dt_count,
			 flags ) ) {
	       free( flags );
	       return MS_ERROR;
	    }
	    ms_copy_array( flags, datatype_to_format( inty_type, data_short ),
			  raw_data->flags, raw_data->flag_values, data_short,
			  FALSE );
	    free( flags );

	 }	/* if ( raw_data->flags > 0 )	*/
      }		/* if( raw_data->points > 0 )	*/ 
   }		/* if ( raw_data )		*/

   /* Retrieve library per-scan data if pointer is non-NULL		*/

   if ( lib_data && (expt_library == ms_clients[index]->expt_type) ) {
      if ( MS_ERROR ==
	  read_info( cdfid, vars, dims, lib_data->scan_no, nLib,
		    (char *)lib_data, library_variables ) )
	 return MS_ERROR;
   }

   return MS_NO_ERROR;

}  /* ms_read_per_scan */

/*
********************************************************************************
*
*  FUNCTION:	ms_read_TIC
*
*  DESCRIPTION:	A convenience function to read the total ion chromatogram.  The
*		netCDF file must have been opened using ms_open_read(), and
*		the global information must have been read in (using
*		ms_read_global()) prior to calling this function.  The TIC
*		arrays returned by this function are new memory, and must be
*		freed by the caller (using free()) after use.
*
*  ARGUMENTS:	(int) netCDF file id, (long *) number of points in TIC,
*		(double * *) array of TIC values, (double * *) array of
*		TIC times
*
*  RETURNS:	(int) error code (MS_ERROR / MS_NO_ERROR)
*
*  AUTHOR:	David Stranz Mon Jun 28 09:13:58 1993
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
int
ms_read_TIC( int cdfid, long * nTic, double * * tic, double * * rTime )
#else	/* __STDC__ */
int
ms_read_TIC( cdfid, nTic, tic, rTime )
   int		cdfid;
   long *	nTic;
   double * *	tic;
   double * *	rTime;
#endif	/* not __STDC__ */
{
   int			index;
   MS_Dimensions *	dims;
   MS_Variables *	vars;
   long			dt_start[2];
   long			dt_count[2];

   /*  Some error checking						*/

   if ( NULL == nTic || NULL == tic || NULL == rTime )
      return MS_ERROR;

   if ( MS_ERROR == (index = client_index( cdfid )) )
      return MS_ERROR;

   /*  Get the number of scans (same as number of TIC points)		*/

   dims = &(ms_clients[index]->dims);
   vars = &(ms_clients[index]->vars);

   *nTic = dims->scan_number_dim.size;

   /*  Make the array to return the TIC values				*/

   if ( NULL == (*tic = (double *)make_array( *nTic, NC_DOUBLE )) ) {
      *nTic = 0L;
      return MS_ERROR;
   }

   /*  Make the array to hold the retention time values			*/

   if ( NULL == (*rTime = (double *)make_array( *nTic, NC_DOUBLE )) ) {
      free( *tic );
      *nTic = 0L;
      *tic = NULL;
      return MS_ERROR;
   }

   /*  Now read in the TIC values.  Set up for hyperslab access, then read
       them in.								*/
   
   dt_start[0] = 0L;			/* start with first point	*/
   dt_count[0] = *nTic;

   if ( MS_ERROR ==
       ncvarget( cdfid, vars->total_intensity_id, dt_start, dt_count,
		(void *)(*tic) ) ) {
      free( *tic );
      free( *rTime );
      *tic = NULL;
      *rTime = NULL;
      *nTic = 0L;
      return MS_ERROR;
   }

   if ( MS_ERROR ==
       ncvarget( cdfid, vars->scan_acquisition_time_id, dt_start, dt_count,
		(void *)(*rTime) ) ) {
      free( *tic );
      free( *rTime );
      *tic = NULL;
      *rTime = NULL;
      *nTic = 0L;
      return MS_ERROR;
   }

   return MS_NO_ERROR;
   
}  /* ms_read_TIC */

/*
********************************************************************************
*
*  FUNCTION:	ms_write_group_global
*
*  DESCRIPTION:	Writes scan group global information.
*
*  ARGUMENTS:	(int) netCDF file id, (long) number of scan groups,
*		(long) maximum number of masses in a group
*
*  RETURNS:	(int) error code (MS_ERROR / MS_NO_ERROR)
*
*  AUTHOR:	David Stranz Mon Jun 28 13:55:01 1993
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
int
ms_write_group_global( int cdfid, long nGroups, long maxGroup )
#else	/* __STDC__ */
int
ms_write_group_global( cdfid, nGroups, maxGroup )
   int		cdfid;
   long		nGroups;
   long		maxGroup;
#endif	/* not __STDC__ */
{
   int			index;
   MS_Dimensions *	dims;
   MS_Variables *	vars;
   extern int		ncopts;
   int			saveOpts;

   /*  Error checking							*/

   if ( MS_ERROR == (index = client_index( cdfid )) )
      return MS_ERROR;

   if ( nGroups < 1 || maxGroup < 1 )
      return MS_ERROR;

   dims = &(ms_clients[index]->dims);
   vars = &(ms_clients[index]->vars);

   /* Put the netCDF file into "define mode".  Ignore any error, since the
      file might already be in define mode and we have no way to detect it */

   saveOpts = ncopts;
   ncopts = 0;
   (void)ncredef( cdfid );
   ncopts = saveOpts;

   /* Define the dimensions						*/

   if ( MS_ERROR ==
       (dims->group_number_dim.id = ncdimdef( cdfid, "group_number", nGroups )) )
      return MS_ERROR;
   dims->group_number_dim.size = nGroups;

   if ( MS_ERROR ==
       (dims->group_max_masses_dim.id =
	ncdimdef( cdfid, "group_max_masses", maxGroup )) )
      return MS_ERROR;
   dims->group_max_masses_dim.size = maxGroup;

   /* Define the variables						*/

   if ( MS_ERROR == 
       write_variables( cdfid, nGroupP, vars, dims, group_variables ) )
      return MS_ERROR;

   /* Return to "data mode"						*/

   saveOpts = ncopts;
   ncopts = 0;
   (void)ncendef( cdfid );
   ncopts = saveOpts;

   return MS_NO_ERROR;

}  /* ms_write_group_global */

/*
********************************************************************************
*
*  FUNCTION:	ms_read_group_global
*
*  DESCRIPTION:	"Reads" scan group dimensions.  These were, in fact, cached
*		during the call to ms_read_dimensions() (during ms_open_read()),
*		so this function simply checks to see if the dimension ids are
*		valid and fills the return variables if so.
*
*  ARGUMENTS:	(int) netCDF file id, (long *) number og groups,
*		(long *) maximum number of masses in any group
*
*  RETURNS:	(int) error code (MS_ERROR / MS_NO_ERROR)
*
*  AUTHOR:	David Stranz Mon Jun 28 14:14:18 1993
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
int
ms_read_group_global( int cdfid, long * nGroups, long * maxGroup )
#else	/* __STDC__ */
int
ms_read_group_global( cdfid, nGroups, maxGroup )
   int		cdfid;
   long *	nGroups;
   long *	maxGroup;
#endif	/* not __STDC__ */
{
   int			index;
   MS_Dimensions *	dims;
   MS_Variables *	vars;

   /* Do some error checking						*/

   if ( NULL == nGroups || NULL == maxGroup )
      return MS_ERROR;

   if ( MS_ERROR == (index = client_index( cdfid )) )
      return MS_ERROR;

   dims = &(ms_clients[index]->dims);
   vars = &(ms_clients[index]->vars);

   /* Initialize the return variables.  If dimensions are not defined for group
      information, then this is not an error, but simply indication that no
      group information is present in the file.	 If dimensions are defined,
      pass their sizes back in the calling arguments.			*/

   *nGroups = 0L;
   *maxGroup = 0L;
   if ( -1 == dims->group_number_dim.id || -1 == dims->group_max_masses_dim.id )
      return MS_NO_ERROR;

   *nGroups = dims->group_number_dim.size;
   *maxGroup = dims->group_max_masses_dim.size;

   /* Read variables ids						*/

   if ( MS_ERROR ==
       read_variables( cdfid, nGroupP, vars, dims, group_variables ) )
      return MS_ERROR;

   return MS_NO_ERROR;

}  /* ms_read_group_global */

/*
********************************************************************************
*
*  FUNCTION:	ms_write_per_group
*
*  DESCRIPTION:	Writes per-scan-group data to the netCDF file.
*
*  ARGUMENTS:	(int) netCDF file id, (MS_Raw_Per_Group *) per-group data
*
*  RETURNS:	(int) error code (MS_ERROR / MS_NO_ERROR)
*
*  AUTHOR:	David Stranz Mon Jun 28 14:24:58 1993
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
int
ms_write_per_group( int cdfid, MS_Raw_Per_Group * group_data )
#else	/* __STDC__ */
int
ms_write_per_group( cdfid, group_data )
   int			cdfid;
   MS_Raw_Per_Group *	group_data;
#endif	/* not __STDC__ */
{
   int			index;
   MS_Dimensions *	dims;
   MS_Variables *	vars;

   /* Error checking: must be a valid client, data structure must be valid,
      and must have an appropriate group number.			*/

   if ( MS_ERROR == (index = client_index( cdfid )) )
      return MS_ERROR;

   if ( NULL == group_data )
      return MS_ERROR;

   dims = &(ms_clients[index]->dims);
   vars = &(ms_clients[index]->vars);

   if ( group_data->group_no >= dims->group_number_dim.size )
      return MS_ERROR;

   /* Write it out							*/

   if ( MS_ERROR ==
       write_info( cdfid, vars, dims, group_data->group_no, nGroupP,
		  (char *)group_data, group_variables ) )
      return MS_ERROR;

   return MS_NO_ERROR;

}  /* ms_write_per_group */

/*
********************************************************************************
*
*  FUNCTION:	ms_read_per_group
*
*  DESCRIPTION:	Reads per-scan-group data from the file.  The data arrays are
*		allocated during this call and must be freed by the caller after
*		use.  The arrays are allocated to the maximum group size;
*		however, only the number of elements corresponding to the
*		actual number of masses in the group is used.  The remainder
*		are set to the default value.  The "group_no" member of the
*		MS_Raw_Per_Group structure must be set to the desired group
*		number prior to the call.
*
*  ARGUMENTS:	(int) netCDF file id, (MS_Raw_Per_Group *) group data
*
*  RETURNS:	(int) error code (MS_ERROR / MS_NO_ERROR)
*
*  AUTHOR:	David Stranz Mon Jun 28 14:34:52 1993
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
int
ms_read_per_group( int cdfid, MS_Raw_Per_Group * group_data )
#else	/* __STDC__ */
int
ms_read_per_group( cdfid, group_data )
   int			cdfid;
   MS_Raw_Per_Group *	group_data;
#endif	/* not __STDC__ */
{
   int			index;
   MS_Dimensions *	dims;
   MS_Variables *	vars;
   long			maxGroup;
   long			j;

   /* Error checking: must be a valid client, data structure must be valid,
      and must have an appropriate group number.			*/

   if ( MS_ERROR == (index = client_index( cdfid )) )
      return MS_ERROR;

   if ( NULL == group_data )
      return MS_ERROR;

   dims = &(ms_clients[index]->dims);
   vars = &(ms_clients[index]->vars);

   if ( group_data->group_no >= dims->group_number_dim.size )
      return MS_ERROR;

   /* Create the data arrays						*/

   maxGroup = dims->group_max_masses_dim.size;
   if ( NULL ==
       (group_data->masses = (double *)malloc( maxGroup * sizeof( double ) )) )
      return MS_ERROR;

   if ( NULL ==
       (group_data->sampling_times =
	(double *)malloc( maxGroup * sizeof( double ) )) ) {
      free( group_data->masses );
      group_data->masses = NULL;
      return MS_ERROR;
   }

   if ( NULL ==
       (group_data->delay_times =
	(double *)malloc( maxGroup * sizeof( double ) )) ) {
      free( group_data->masses );
      free( group_data->sampling_times );
      group_data->masses = NULL;
      group_data->sampling_times = NULL;
      return MS_ERROR;
   }

   /* Read in the group							*/

   if ( MS_ERROR ==
       read_info( cdfid, vars, dims, group_data->group_no, nGroupP,
		 (char *)group_data, group_variables ) ) {
      free( group_data->masses );
      free( group_data->sampling_times );
      free( group_data->delay_times );
      group_data->masses = NULL;
      group_data->sampling_times = NULL;
      group_data->delay_times = NULL;
      group_data->mass_count = 0L;
      return MS_ERROR;
   }

   for ( j = group_data->mass_count; j < maxGroup; j++ ) {
      group_data->masses[j]		= MS_NULL_FLT;
      group_data->sampling_times[j]	= MS_NULL_FLT;
      group_data->delay_times[j]	= MS_NULL_FLT;
   }

   return MS_NO_ERROR;

}  /* ms_read_per_group */

/*
********************************************************************************
*
*  FUNCTION:	ms_init_global
*
*  DESCRIPTION:	Initializes (and optionally clears) attribute data structures
*		to default values.
*		NULL pointers to structures are ignored.
*
*		If the clear flag is TRUE, then non-NULL character string
*		attribute values are freed (since these are assumed to be
*		allocated memory), then set to NULL.
*
*  ARGUMENTS:	(int) clear flag
*		(MS_Admin_Data *) administrative data structure pointer,
*		(MS_Sample_Data *) sample data structure pointer
*		(MS_Test_Data *) test method data structure pointer
*		(MS_Raw_Data_Global *) raw data global data structure pointer
*
*  RETURNS:	(void) 
*
*  AUTHOR:	David Stranz Wed Nov 11 10:34:15 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
void
ms_init_global( int clear, MS_Admin_Data * admin_data,
	      MS_Sample_Data * sample_data, MS_Test_Data * test_data,
	      MS_Raw_Data_Global * raw_data )
#else	/* __STDC__ */
void
ms_init_global( clear, admin_data, sample_data, test_data, raw_data )
   int			clear;
   MS_Admin_Data *	admin_data;
   MS_Sample_Data *	sample_data;
   MS_Test_Data *	test_data;
   MS_Raw_Data_Global *	raw_data;
#endif	/* not __STDC__ */
{

   if ( admin_data )
      init_attributes( (char *)admin_data, nAdminA, admin_attributes, clear );

   if ( sample_data )
      init_attributes( (char *)sample_data, nSamp, sample_attributes, clear );

   if ( test_data )
      init_attributes( (char *)test_data, nTest, test_attributes, clear );

   if ( raw_data )
      init_attributes( (char *)raw_data, nRaw, raw_data_attributes, clear );
   
}  /* ms_init_global */

/*
********************************************************************************
*
*  FUNCTION:	ms_init_instrument
*
*  DESCRIPTION:	Initializes (and optionally clears) instrument variables
*
*  ARGUMENTS:	(int) clear flag,
*		(MS_Instrument_Data *) instrument data structure pointer
*
*  RETURNS:	(void) 
*
*  AUTHOR:	David Stranz Thu Nov 12 10:43:19 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
void
ms_init_instrument( int clear, MS_Instrument_Data * inst_data )
#else	/* __STDC__ */
void
ms_init_instrument( clear, inst_data )
   int			clear;
   MS_Instrument_Data *	inst_data;
#endif	/* not __STDC__ */
{

   if ( inst_data )
      init_indexed_variables( (char *)inst_data, nInst, instrument_variables,
			     clear );

   return;

}  /* ms_init_instrument */

/*
********************************************************************************
*
*  FUNCTION:	ms_init_per_scan
*
*  DESCRIPTION:	Initializes (and optionally clears) raw and library per scan
*		data structures.  If either data structure pointer is NULL,
*		that structure is ignored.
*
*  ARGUMENTS:	(int) clear flag,
*		(MS_Raw_Per_Scan *) raw per scan data structure
*		pointer, (MS_Raw_Library *) library per scan data structure
*		pointer
*
*  RETURNS:	(void) 
*
*  AUTHOR:	David Stranz Thu Nov 12 10:51:50 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
void
ms_init_per_scan( int clear, MS_Raw_Per_Scan * raw_data,
		 MS_Raw_Library * lib_data )
#else	/* __STDC__ */
void
ms_init_per_scan( clear, raw_data, lib_data )
   int			clear;
   MS_Raw_Per_Scan *	raw_data;
   MS_Raw_Library *	lib_data;
#endif	/* not __STDC__ */
{

   if ( raw_data ) {
      init_indexed_variables( (char *)raw_data, nRawP, raw_variables,
			     clear );

      raw_data->scan_no = -1L;
      raw_data->points = 0L;
      raw_data->flags = 0L;

      if ( clear && raw_data->masses )
	 free( raw_data->masses );
      raw_data->masses = NULL;

      if ( clear && raw_data->times )
	 free( raw_data->times );
      raw_data->times = NULL;

      if ( clear && raw_data->intensities )
	 free( raw_data->intensities );
      raw_data->intensities = NULL;

      if ( clear && raw_data->flag_peaks )
	 free( raw_data->flag_peaks );
      raw_data->flag_peaks = NULL;

      if ( clear && raw_data->flag_values )
	 free( raw_data->flag_values );
      raw_data->flag_values = NULL;

   }

   if ( lib_data ) {
      init_indexed_variables( (char *)lib_data, nLib, library_variables,
			     clear );
      lib_data->scan_no = -1L;
   }

   return;

}  /* ms_init_per_scan */

/*
********************************************************************************
*
*  FUNCTION:	ms_init_per_group
*
*  DESCRIPTION:	Initializes (and optionally clears) per-scan-group data
*
*  ARGUMENTS:	(int) clear flag, (MS_Raw_Per_Group *) group data
*
*  RETURNS:	(void) 
*
*  AUTHOR:	David Stranz Mon Jun 28 14:50:16 1993
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
void
ms_init_per_group( int clear, MS_Raw_Per_Group * group_data )
#else	/* __STDC__ */
void
ms_init_per_group( clear, group_data )
   int			clear;
   MS_Raw_Per_Group *	group_data;
#endif	/* not __STDC__ */
{

   if ( group_data ) {
      if ( clear ) {
	 if ( group_data->masses )
	    free( group_data->masses );
	 if ( group_data->sampling_times )
	    free( group_data->sampling_times );
	 if ( group_data->delay_times )
	    free( group_data->delay_times );
      }

      group_data->group_no		= (long)MS_NULL_INT;
      group_data->mass_count		= (long)MS_NULL_INT;
      group_data->starting_scan		= (long)MS_NULL_INT;
      group_data->masses		= NULL;
      group_data->sampling_times	= NULL;
      group_data->delay_times		= NULL;

   }
   return;

}  /* ms_init_per_group */

/*
********************************************************************************
*
*  FUNCTION:	ms_associate_id
*
*  DESCRIPTION:	Associates a netCDF id with internal data structures
*
*  ARGUMENTS:	(int) netCDF id
*
*  RETURNS:	(int) error code (MS_ERROR / MS_NO_ERROR)
*
*  AUTHOR:	David Stranz Mon Nov 30 08:18:35 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
int
ms_associate_id( int cdfid )
#else	/* __STDC__ */
int
ms_associate_id( cdfid )
   int		cdfid;
#endif	/* not __STDC__ */
{
   int		index;

   /* First, look up the netCDF id.  If it is a duplicate, do nothing.	*/

   if ( MS_ERROR != client_index( cdfid ) )
      return MS_NO_ERROR;

   /* It must be unique, so extend the clients array, then make a new
      client data structure.						*/

   if ( ms_client_count ) {
      if ( NULL == 
	  (ms_clients =
	   (MS_Client_Data * *) realloc( (void *)ms_clients,
					(unsigned int)(ms_client_count + 1) *
					sizeof( MS_Client_Data *) )) )
	 return MS_ERROR;
   }
   else {
      if ( NULL ==
	  (ms_clients =
	   (MS_Client_Data * *) malloc( sizeof( MS_Client_Data *) )) )
	 return MS_ERROR;
   }
   index = ms_client_count;

   if ( NULL ==
       (ms_clients[index] =
	(MS_Client_Data *) malloc( sizeof( MS_Client_Data ) )) )
      return MS_ERROR;

   ms_client_count++;

   ms_clients[index]->cdfid = cdfid;
   ms_clients[index]->mass_type = data_short;
   ms_clients[index]->time_type = data_short;
   ms_clients[index]->inty_type = data_long;
   ms_clients[index]->has_masses = FALSE;
   ms_clients[index]->has_times = FALSE;
   ms_clients[index]->total_count = 0L;

   return MS_NO_ERROR;

}  /* ms_associate_id */

/*
********************************************************************************
*
*  FUNCTION:	ms_dissociate_id
*
*  DESCRIPTION:	Removes the association between a netCDF id and internal data
*		structures.  Frees all allocated storage.
*
*  ARGUMENTS:	(int) netCDF id
*
*  RETURNS:	(int) error code (MS_ERROR / MS_NO_ERROR)
*
*  AUTHOR:	David Stranz Mon Nov 30 08:30:47 1992
*
*  REVISIONS:	
*
********************************************************************************
*/

#ifdef	__STDC__
int
ms_dissociate_id( int cdfid )
#else	/* __STDC__ */
int
ms_dissociate_id( cdfid )
   int		cdfid;
#endif	/* not __STDC__ */
{
   int	index;
   int	i;

   /* Do a lookup - if not found, that is an error			*/

   if ( MS_ERROR == (index = client_index( cdfid )) )
      return MS_ERROR;

   /* Free the data structure						*/

   free( ms_clients[index] );

   /* Compact the array							*/

   for ( i = index; i < ms_client_count - 1; i++ )
      ms_clients[i] = ms_clients[i+1];

   ms_client_count--;

   if ( ms_client_count ) {
      if ( NULL ==
	  (ms_clients = 
	   (MS_Client_Data * *)realloc( (void *) ms_clients,
				       (unsigned int)ms_client_count * 
				       sizeof( MS_Client_Data * ) )) )
	 return MS_ERROR;
   }
   else
      ms_clients = NULL;

   return MS_NO_ERROR;
   
}  /* ms_dissociate_id */
