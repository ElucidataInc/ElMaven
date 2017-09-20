/* vi:set syntax=c expandtab tabstop=4 shiftwidth=4:

 C D F R E A D . C

 This program implements the routines to read single mass spectra
 and mass chromatograms (with variable width) from data files in
 netCDF format.

 The graphical display of the data makes use of gnuplot. The
 "user interface" provided here was written mainly for testing
 purposes and is therefore very simple.
 --------------------------------------------------------------------

 Copyright (C) 2001...2009 by Joerg Hau. All rights reserved.

 This program is free software; you can redistribute it and/or
 modify it under the terms version 2 of the GNU General Public 
 License as published by the Free Software Foundation. See the
 file LICENSE for details.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 --------------------------------------------------------------------

 The CDF reader functions used here are based on sample code by
 David Stranz, (C) 1993 Analytical Instruments Association, which
 was placed into the Public Domain. The following COPYRIGHT NOTICE
 from the Analytical Instrument Association applies [Note from JHa:
 I have slightly changed the first paragraph of the original text,
 as it does not apply to the actual situation]:

 The source code in this file uses the public-domain portion of the
 MS netCDF Data Interchange Specification for Mass Spectrometry.
 This code, although placed in the public domain, is copyright by
 the Analytical Instrument Association. It may be freely distributed,
 but any copies or derived works must contain the copyright notice,
 along with these paragraphs.

 IMPORTANT:  Implementers of netCDF interchange programs using the code
 in this file should do so without modifying this code if at all possible.
 The names of dimensions, attributes, and variables, as well as the
 enumerated values of certain attributes and variables have been agreed
 upon by a consortium of manufacturers and end users from throughout
 the mass spectrometry community.  Many of the identifiers have been
 directly copied from other implementations by the Analytical Instrument
 Association (AIA) for chromatographic data interchange, and from the
 Analytical Data Interchange and Storage Standards (ADISS) Project for
 general analytical data.  Changing them will almost certainly result
 in incompatibility between files produced using differing implementations,
 and the inability to interchange data, which after all, is the whole
 reason behind doing this.

 Manufacturers or other implementers can *extend* the MS specification
 to accomodate individual needs.  This can be done by defining dimensions,
 attributes, or variables *in addition to* those defined by the MS
 specification.  It is strongly suggested that these additional identifiers
 have names which begin with a unique prefix, preferably something which
 indicates the source of the new identifier, such as:

    hp_dimension  or
    :fisons_attribute  or
    short finnigan_array(finnigan_dimension)  (you get the idea)

 Extending the MS specification in this way is harmless - the new
 identifiers are ignored by any application not looking for them, and
 using manufacturer-specific prefixes ensures that new names will not
 accidently be confused with identifiers added by someone else.

 Any changes to the MS specified part of this implementation *must*
 be reviewed and approved by the MS data interchange consortium.

--------------------------------------------------------------------

 Author:  Joerg Hau
 www:     http://cdfread.sourceforge.net/

 History: (adapt VERSION below when changing this!)

 2001-05-16, creation by JHa
 2001-09-18, more comments (JHa)
 2003-04-15, optional parameter for mouse in gnuplot
 2003-08-13, some additional error checking
 2003-09-15, additional error check on gnuplot start, mouse zoom 
             support, annotation with middle mouse button
 2003-09-17: Added "-i" command line switch.
 2003-09-25: Added GNU readline() and a "menu" (inspired by from comspari ;-).
             Toggle persistent/temporary mouse labels, clear mouse labels (JHa)
 2003-09-26: changed peakwidth from "w" to "p"; variables now in struct;
             misc streamlining of code (JHa)
 2003-10-06: added "?" command (JHa)
 2003-11-02: added WIN32 stuff (JHa)
 2004-08-06: added mouse feedback (EXPERIMENTAL) (JHa)
 2004-10-01: added warning for brokenmass range, frequently seen with 
             HP/Agilent Chemstation) (JHa)
 2004-10-28: bugfix in printing (might have restored the wrong terminal 
             settings after print; JHa)
 2006-11-15: updated information where to find the AIA header files; 
             no changes to the code as such (= same version). (JHa)
 2009-03-07: updated to reflect hosting on sourceforge (JHa).

 To use it, you need gnuplot installed on your system. I strongly
 recommend using gnuplot 4 or later (mouse support!).

 This should compile with any C compiler. To build, you need:

-  the AIA files to read netCDF MS datafiles, available at
   http://andi.sourceforge.net/. You need ms10.h, ms10aux.c,
   ms10enum.c, ms10io.c and ms10io.h.

 - the netCDF library, available from ftp.unidata.ucar.edu in the
   /pub/netcdf directory. A number of pre-built libraries for
   different OS are located in the /pub/netcdf/contrib tree.

 - optinally, the GNU readline library.

  How to proceed:

 - install the netCDF libraries, e.g. in /usr/local.
 - gcc -c -O3 ms10aux.c ms10enum.c ms10io.c
 - gcc -O3 -Wall -o cdfread cdfread.c ms10aux.o ms10enum.o ms10io.o \
       /usr/local/lib/libnetcdf.a

   (adapt paths to your needs)

   If you use GNU readline, add "-lreadline" in that line.

   The display using gnuplot will not necessarily work on any platform.
   We need POSIX pipes for that, which implies that the underlying operating
   system knows about processes and pipes, and that it uses them in a POSIX
   fashion. Since MS Windows does not respect this standard, we need to use
   'pgnuplot.exe' as a workaround on that platform.

   Thus, for MS Windows, add -DWIN32 and see the README file.

*/

#define VERSION "V20090306"     /* String! */

//#define READLINE                /* the GNU Readline Library */

#define MAXLEN 81     		    /* buffers etc. */
#define CHRO	0		        /* display mode */
#define SPEC 	1

#define GPFIFO "./gpio"         /* string */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>      /* isspace() */
#include <unistd.h>		/* unlink() */
#include <sys/types.h>
#include <sys/stat.h>
#include "ms10.h"

#ifndef __GNUC__                /* if this is NOT the GNU C compiler */
#include <io.h>                 /* wg. 'access()' */
#else
#include <unistd.h>             /* wg. 'access()' */
#endif

#ifdef READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

/* --- Some compiler-specific options ---- */
/* --- FIXME: Add stuff for other compilers (MinGW, ...) ---- */

#ifdef __MSVC__          /* MS C */
#define WIN32
#define popen     _popen
#define pclose    _pclose
#endif

/* --- Some platform-specific options. NOTE: Most should be automagically
       invoked through the compiler-specific options above --- */

#ifdef WIN32
#define GNUPLOT     "pgnuplot.exe"  /* MS Windows requires pgnuplot.exe */
#else
#define GNUPLOT     "gnuplot"       /* all other OS use "normal" gnuplot */
#endif


/* --- structures to hold miscellaneous params --- */

typedef struct
{
char   *fn;                 /* actual data file name */
long   minscan, maxscan,	/* first and last scan to use */
       minmass, maxmass,	/* lowest and highest m/z to use */
       scan;			    /* actual scan */
double mz, pw;			    /* actual mz value; peak width */
int    mode,                /* CHRO, SPEC */
       verbose,	    	    /* flag for reporting mode (0 = off, else on) */
       mouse,	    		/* flag for mouse use (0 = off, else on) */
       p_label;	    	    /* persistent labels */
}
settingstype;


/* --- Function prototypes ---- */

int     open_cdf_ms (char *fname, int is_verbose);
void    close_cdf_ms (const int handle);
int     readscan (const int handle, const long scan, const char *fnam);
int     readchro (const int cdf, const double lo, const double hi, const char *fnam);
char 	*read_line(char *prompt, int maxlen);
int     strclean (char *buf);
int     fifo_changed (const char *fifo);
int     GetOpt(int argc, char *argv[], char *optionS);


/* --- Global variables --- */

settingstype set;		/* to hold misc parameters (see above) */

int optind = 1;        /* global: index of which argument is next. Is used
                                 as a global variable for collection of further
                                 arguments (= not options) via argv pointers. */


/* --- Global static variables --- */

static MS_Admin_Data            admin_data;
static MS_Sample_Data           sample_data;
static MS_Test_Data             test_data;
static MS_Instrument_Data       inst_data;
static MS_Raw_Data_Global       raw_global_data;
static MS_Raw_Per_Scan          raw_data;
extern int ncopts;              /* from "netcdf.h" */
char   *optarg;                /* global: pointer to argument of current option */


/****************************************************************
* Function:     main -- Main program part.                      *
* Agruments:    Filename (incl. extension, via gp line).       *
* Returns:      One of the following:                           *
*               0       OK                                      *
*               1       no file specified, or wrong option      *
*               2       problem reading the cdf file            *
*               3       problem communicating with gnuplot      *
*               4       internal error (calloc...)              *
****************************************************************/
int main(int argc, char *argv[])
{
static char *disclaimer =
"\ncdfread - read data from MS-netCDF files. %s.\n"
"Copyright (C) 2001...2009 by Joerg Hau.\n\n"
"This program is free software; you can redistribute it and/or modify it under\n"
"the terms of v2 of the GNU General Public License as published by the Free\n"
"Software Foundation.\n\n"
"This program is distributed in the hope that it will be useful, but WITHOUT ANY\n"
"WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A\n"
"PARTICULAR PURPOSE. See the GNU General Public License for details.\n\n";

static char *msg=
"Syntax: cdfread [-h] [-v] [-i] [-m] [-p peakwidth] [-g path/to/gnuplot] filename.cdf"
"\n        -h       this help screen"
"\n        -v       be verbose"
"\n        -i       show only file information"
"\n        -m       disable mouse"
"\n        -p x     read chromatograms with a window of +- 'x'/2 amu"
"\n        -g       specify path/to/[p]gnuplot (if not in your current PATH)\n\n";

extern settingstype set;

int     cdf,                /* file handle */
        tmp = 0;
FILE    *gp, *gpin;         /* for communication w/ gnuplot */
char    key = 0,	        /* input */
	    *buf=(char *) NULL, /* text buffer for readline() */
        gnuplot[MAXLEN],
        temp_name[MAXLEN],  /* for temporary file */
	    info_only = 0;	    /* flag for info-only mode (0 = off, else on) */

set.verbose = 0;        /* silent */
set.mouse   = 1;        /* mouse on */
set.p_label = 1;        /* persistent labels */
set.pw      = 1.0;      /* peak width */

/* --- set the executable --- */

sprintf (gnuplot, "%s", GNUPLOT);

/* --- show the usual text --- */

fprintf(stderr, disclaimer, VERSION);


/* --- decode and read the command line --- */

while ((tmp = GetOpt(argc, argv, "hvimg:p:")) != EOF)
    switch (tmp)
        {
        case 'h':                    /* help me */
            fprintf (stderr, msg);
            return 0;
        case 'v':                    /* be verbose */
            set.verbose = 1;
            continue;
        case 'i':                    /* show info only, no graphics */
            info_only = 1;
            continue;
        case 'm':                    /* disable mouse in gnuplot */
            set.mouse = 0;
            continue;
        case 'g':
            sscanf (optarg, "%80s", gnuplot);
            continue;
         case 'p':                    /* read window width */
            sscanf(optarg, "%3lf", &set.pw);
		    if ((set.pw < 0.0001) || (set.pw > 10000.0))
		        {
		        fprintf (stderr, "Invalid peakwidth, using default.\n");
		    	set.pw = 1.0;
			    }
            continue;
        case '~':                    /* invalid arg */
        default:
		    fprintf (stderr, "'%s -h' for help.\n\n", argv[0]);
            return 1;
        }

if (argv[optind] == NULL)         /* no remaining parameter on gp line? */
    {
    fprintf (stderr, msg);
#ifdef WIN32
    system("pause"); /* MS-Windows only: avoid that the gp box closes immediately.*/
#endif
    return 1;
    }

set.fn = argv[optind];
ncopts = 0;

/* create a temporary filename. As this will be printed in the graphics,
   use the name of the original data file as "base". */

#ifdef WIN32
    strcpy (temp_name, "cdfXXXXXX");
    if (NULL == mktemp(temp_name))
        {
        perror("Error while creating temporary filename!");
        return 4;
        }
#else
    strcpy (temp_name, "/tmp/cdf_XXXXXX");
    if (-1 == mkstemp(temp_name))
        {
        perror("Error while creating temporary filename!");
        return 4;
        }
#endif

if (info_only)
	set.verbose = 1;		/* reuse this flag */

/* get cdf file header etc., prepare for reading */

printf("%s\n", set.fn);	/* show filename */

if (0 == (cdf = open_cdf_ms(set.fn, set.verbose)))
    {
    fprintf (stderr, "\nError opening datafile %s!\n", set.fn);
    return 2;
    }

if (info_only)			/* if only info was desired, we can quit here */
	{
	close_cdf_ms(cdf);
	unlink(temp_name);
	printf("\n");
	return 0;
	}

#ifndef WIN32
/* create FIFO for reading back from gnuplot */
/* This is not used yet */

if (mkfifo(GPFIFO, 0600))   /* create FIFO */
    if (errno != EEXIST)	/* not a real problem */
        {
        perror(GPFIFO);
	    unlink(GPFIFO);
	    return 1;
	    }
#endif

/* prepare gnuplot for action */

if (NULL == (gp = popen(gnuplot,"w")))
	{
	fprintf(stderr, "\nCannot start gnuplot!\n") ;
	fflush(stderr);
	close_cdf_ms(cdf);
	pclose(gp);
	return 3;
	}

/* arrive here if OK: send some initial stuff to gnuplot */

fprintf(gp, "set border 3;set tics out;set xtics nomirror;set ytics nomirror\n");
fprintf(gp, "set autoscale xfixmin;set autoscale xfixmax\n");
if (set.mouse)
	fprintf(gp, "set mouse;set mouse labels;\n");
fprintf(gp, "set print \"%s\"\n", GPFIFO);

/* seems we should print an initial linefeed from gnuplot,
   otherwise something is wrong with the FIFO ?!
   Try commenting this line out, sometimes it hangs then */
fprintf(gp, "print \"\\n\"\n");
fflush(gp);

/* Open the FIFO (where gnuplot is writing to) for reading. */
#ifndef WIN32
if (NULL == (gpin = fopen(GPFIFO,"r")))
    {
	perror(GPFIFO);
	pclose(gp);
	return 1;
	}
#endif

/* --- prepare and run the dialog --- */

set.scan = 0;       /* some init value to avoid printing nonsense */
set.mz = raw_global_data.mass_axis_global_min + \
         (raw_global_data.mass_axis_global_max+raw_global_data.mass_axis_global_min)/2.0;


while (key >= 0)		/* (almost) endless loop */
    {
    fprintf(gp, "\nset mouse %slabels;set mouse mouseformat \"%%5.1f\"\n", set.p_label ? "" : "no");

    buf = read_line(": ", MAXLEN);      /* read input */
    while (isspace(*buf))               /* skip whitespace */
    	buf++;

    switch (*buf)
        {
	    case 'h':		/* help */
	    case '?':		/* also help */
	        printf ("This is cdfread, %s.\n"
		        //"Enter) next spectrum or chromatogram\n"
		        "s)can [number]\n"
		        "c)hromatogram [m/z]\n"
		        "p)eak width (\"p 1\" is +/- 0.5 m/z)\n"
		        "o)utput (e.g., \"o plot.ps\", or \"o |lpr -Plp1\"\n"
		        "g)nuplot internal command (e.g., \"g set xrange [0:20]\")\n"
		        "clear l)abels\n"
		        "toggle if L)abels are persistant\n"
		        "reset x) axis to default\n"
		        "h)elp (this fine menu ;-)\n"
		        "q)uit\n", VERSION);
	        break;
	    case 'c':		/* chro */
	        set.mode = CHRO;
            buf++;
            sscanf(buf,"%8lf",&set.mz);
	        if (!readchro(cdf, (double)set.mz-(set.pw/2.0), (double)set.mz+(set.pw/2.0), temp_name))
                {
                /* print only error msg, do not abort */
                fprintf(stderr, "\nProblem reading m/z %.1f!\n", set.mz);
                continue;
                }
            fprintf(gp, "set title '%s, m/z %.2f>%.2f';set nokey;set xlabel 'scan'\n",
                set.fn, (double)set.mz-(set.pw/2.0), (double)set.mz+(set.pw/2.0));
            fprintf(gp,"plot '%s' with lines\n", temp_name);
            fflush(gp);
	        break;
	    case 'p':		/* chro peak width */
            buf++;
            sscanf(buf,"%5lf",&set.pw);
	        if ((set.pw < 0.0001) || (set.pw > 10000))
		        {
		        fprintf (stderr, "Invalid peakwidth, using default.\n");
		        set.pw = 1.0;
		        }
	        break;
	    case 's':		/* spec */
	        set.mode = SPEC;
            buf++;
            sscanf(buf,"%8ld",&set.scan);
            if (!readscan(cdf, set.scan, temp_name))
                {
                /* print only error msg, do not abort */
                fprintf(stderr, "\nProblem reading scan %ld!\n", set.scan);
                continue;
                }
            fprintf(gp,"set title '%s, scan %ld';set nokey;set xlabel 'm/z'\n", set.fn, set.scan);
            fprintf(gp,"plot '%s' with %s\n", temp_name, \
		    (admin_data.experiment_type ? "lines" : "impulses"));
            fflush(gp);
	        break;
	    case 'o':		/* PS output */
            for(buf++; isspace(*buf); buf++);
	        if (strclean (buf))	/* FIXME: safety check */
	            {
	            printf ("Printing ...");
	            fprintf (gp, "set term push\n");
	            fprintf (gp,
			    "set term postscript eps \"Times\" color solid\n");
	            fprintf (gp, "set nogrid\nset xlabel\nset ylabel\n");
	            fprintf (gp, "set output \"%s\"\n", buf);
	            fprintf (gp, "replot\n");
	            fflush (gp);
	            printf (" done. Restoring terminal settings ... ");
	            fprintf (gp, "set term pop\nset output\nreplot\n");
	            fflush (gp);
	            printf (" done.\n");
	            }
	        break;
        case 'g':
            for(buf++; isspace(*buf); buf++);
	        if (strclean (buf))	    /* FIXME: safety check */
	            fprintf (gp, "%s\n", buf);
	        fflush (gp);
	        break;
        case 'x':		/* reset x range */
	        if (set.mode == SPEC)
	    	    fprintf(gp, "set xrange [%ld:%ld];set yrange [0:*]\n",
                    (long)raw_global_data.mass_axis_global_min,
                    (long)raw_global_data.mass_axis_global_max);
	        else 		/* mode == CHRO */
		        fprintf(gp, "set xrange [0:%ld]; set yrange [0:*]\n",
		            (long)raw_global_data.nscans);
            fprintf (gp, "replot\n");
            fflush (gp);
    	    break;
	 case 'l':		/* clear all labels */
	        fprintf (gp, "unset label;replot\n");
	        fflush (gp);
	        break;
	 case 'L':		/* labels persistent or not */
	        set.p_label = !(set.p_label);
	        printf ("Labels are%s persistent.\n", set.p_label ? "": " not");
	        break;
	 case 'q':		/* quit */
	        key = -1;
	        break;
	 default:
            fprintf(stderr, "Invalid command. Type 'h' or '?' for help.\n");
	        break;
      }				/* switch() */
   }				/* while (key >= 0) */


/* --- clean up before we leave --- */

close_cdf_ms(cdf);
pclose(gp);
unlink (GPFIFO);
unlink(temp_name);

return 0;
}



/************************************************************************
* Function:     open_cdf_ms                                             *
* Description:  Reads through a CDF file and gets some key parameters   *
* Arguments:    filename to read from; flag if verbose                  *
* Returns:      nr of cdf file handle if OK, 0 if error.                *
* Author:       Joerg Hau <joerg.hau(at)dplanet.ch>, 2001-05-16         *
*               based on example code from David Stranz                 *
*************************************************************************/
int open_cdf_ms (char *filename, int is_verbose)
{
int     cdf, errflag = 0;
long    i, j, nscans, ninst;

/* Open the interchange file */

cdf = ms_open_read( filename );
if ( -1 == cdf )
    {
    fprintf( stderr, "\nopen_cdf_ms: ms_open_read failed!" );
    return 0;
    }

/* Initialize attribute data structures */

ms_init_global( FALSE, &admin_data, &sample_data, &test_data,
                &raw_global_data );


/* Read global information */

if (MS_ERROR == ms_read_global( cdf, &admin_data, &sample_data,
                       &test_data, &raw_global_data))
    {
    fprintf( stderr, "\nopen_cdf_ms: ms_read_global failed!" );
    ms_init_global( TRUE, &admin_data, &sample_data,
                    &test_data, &raw_global_data);
    ms_close(cdf);
    return 0;
    }

nscans = raw_global_data.nscans;

if (is_verbose)
    {
    printf ("\n-- Administrative Information --");
    printf ("\nDataset Completeness\t%s", admin_data.dataset_completeness);
    printf ("\nMS Template Revision\t%s", admin_data.ms_template_revision);
    printf ("\nnetCDF DateTime\t\t%s", admin_data.netcdf_date_time);
    printf ("\nnetCDF Revision\t\t%s", admin_data.netcdf_revision);

    printf ("\nSource file ref.\t%s", admin_data.source_file_reference);
    printf ("\nSource file format\t%s", admin_data.source_file_format);
    printf ("\nDataset Origin\t\t%s", admin_data.dataset_origin);
    printf ("\nDataset Owner\t\t%s", admin_data.dataset_owner);
    printf ("\nOperator\t\t%s", admin_data.operator_name);

    printf ("\nExperiment Title\t%s", admin_data.experiment_title);
    printf ("\nExperiment DateTime\t%s", admin_data.experiment_date_time);
    printf ("\nExperiment Type\t\t");
    switch (admin_data.experiment_type)
        {
        case 0:
                printf ("Centroid");
                break;
        case 1:
                printf ("Continuum");
                break;
        case 2:
                printf ("Library");
                break;
        default:
                printf ("Unknown: '%d'", admin_data.experiment_type);
                break;
        }

    printf ("\n\n-- Instrument Information --");
    ninst = admin_data.number_instrument_components;
    printf ("\nNumber_inst_comp\t%ld", ninst);

    if (ninst > 0)
        {
        ms_init_instrument( FALSE, &inst_data );
        for ( i = 0; i < ninst; i++ )
            {
            inst_data.inst_no = (long)i;
            if (MS_ERROR == ms_read_instrument(cdf, &inst_data))
                {
                fprintf(stdout,
                "\nopen_cdf_ms: ms_read_instrument failed on component %ld!",
                i);
                errflag = 1;
                ms_init_instrument(TRUE, &inst_data);
                break;
                }

            printf("\n [%ld] Name:\t\t%s", i, inst_data.name);
            printf("\n [%ld] ID\t\t\t%s", i, inst_data.id);
            printf("\n [%ld] Manufacturer\t%s", i, inst_data.manufacturer);
            printf("\n [%ld] Serial #\t\t%s", i, inst_data.serial_number);
            printf("\n [%ld] Software\t\t%s", i, inst_data.software_version);
            printf("\n [%ld] Firmware\t\t%s", i, inst_data.firmware_version);
            printf("\n [%ld] Operating System\t%s",
                i, inst_data.operating_system );
            printf("\n [%ld] Application\t%s",
                 i, inst_data.application_software );
            printf("\n [%ld] Comments\t\t%s", i, inst_data.comments);

            ms_init_instrument( TRUE, &inst_data );
            }   /* component loop */
        }       /* if ninst > 0 ... */

    printf ("\n\n-- Sample Information --");

/* I have commented out a good part of information that is apparently
   not used by the manufacturers right now */

    printf ("\nInternal ID\t\t%s", sample_data.internal_id);
    printf ("\nExternal ID\t\t%s",sample_data.external_id);
    printf ("\nReceipt  \t\t%s",sample_data.receipt_date_time);
    printf ("\nOwner    \t\t%s",sample_data.owner);
    printf ("\nProcedure\t\t%s", sample_data.procedure_name);
    printf ("\nMatrix  \t\t%s", sample_data.matrix);
    printf ("\nStorage \t\t%s", sample_data.storage);
    printf ("\nDisposal\t\t%s", sample_data.disposal);
    printf ("\nHistory \t\t%s", sample_data.history);
    printf ("\nPreparation\t\t%s", sample_data.prep_procedure);
    printf ("\nPrep Comments\t\t%s", sample_data.prep_comments);
    printf ("\nHandling\t\t%s", sample_data.manual_handling);
    printf ("\nSample Comments\t\t%s", sample_data.comments);

    printf ("\n\n-- Raw Data Information --");
    printf ("\nNumber of scans\t\t%ld", nscans);

    printf ("\nMass Range\t\t%.2f > %.2f",
        raw_global_data.mass_axis_global_min,
        raw_global_data.mass_axis_global_max);

    printf ("\nInty Range\t\t%.2f > %.2f",
        raw_global_data.intensity_axis_global_min,
        raw_global_data.intensity_axis_global_max);

    printf ("\nTime Range\t\t%.2f > %.2f",
        raw_global_data.time_axis_global_min,
        raw_global_data.time_axis_global_max);

    printf ("\nActual Run Time\t\t%.2f (%.2f min)",
        raw_global_data.run_time, raw_global_data.run_time/60.0);
    printf ("\nComments \t\t%s", raw_global_data.comments);

    printf ("\n");		/* final linefeed */
    }   /* ... is_verbose ... */


if (2 == admin_data.experiment_type)
        {
        fprintf (stderr, "\n*** Error: Cannot convert Library data!");
        errflag = 1;
        }

if (admin_data.experiment_type > 2)
        {
        fprintf (stderr,"\n*** Error: Illegal experiment_type entry '%d'!",
                admin_data.experiment_type);
        errflag = 1;
        }

if (0 == raw_global_data.has_masses)
        {
        fprintf (stderr, "\n*** Error: Cannot convert data without masses!");
        errflag = 1;
        }

     /* Read group data */

   if ( MS_ERROR == ms_read_group_global( cdf, &i, &j ))
        {
        fprintf( stderr, "\nopen_cdf_ms: ms_read_group_global failed." );
        errflag = 1;
        }
   else
        if (i > 0)
            {
            fprintf (stderr,
                "\n*** Conversion of group data is not implemented!");
            errflag = 1;
            }

if (errflag)                    /* if error occurred, clean up and leave */
        {
        ms_init_global( TRUE, &admin_data, &sample_data, &test_data,
                  &raw_global_data );
        ms_close( cdf );
        return 0;
        }

/* Check to see if scale factors and offsets are set to "NULL"
   values; if so, correct them for use below */

if ((int)MS_NULL_FLT == (int)raw_global_data.mass_factor)
      raw_global_data.mass_factor = 1.0;

if ((int)MS_NULL_FLT == (int)raw_global_data.time_factor)
      raw_global_data.time_factor = 1.0;

if ((int)MS_NULL_FLT == (int)raw_global_data.intensity_factor)
      raw_global_data.intensity_factor = 1.0;

if ((int)MS_NULL_FLT == (int)raw_global_data.intensity_offset)
      raw_global_data.intensity_offset = 0.0;

if ((raw_global_data.mass_axis_global_min < 0) || (raw_global_data.mass_axis_global_max < 0))
    {
    /* this bug is frequently observed with files from HP/Agilent ChemStation */
    fprintf (stderr, "\n*** WARNING: Negative mass reported! Use '-v' for details.\n\n");
    }

return cdf;
}


/************************************************************************
* Function:     close_cdf_ms                                            *
* Description:  closes a previosly opened CDF file                      *
* Arguments:    file handle to close                                    *
* Returns:      void                                                    *
* Author:       Joerg Hau <joerg.hau(at)dplanet.ch>, 2001-05-16         *
*               based on example code from David Stranz                 *
************************************************************************/
void close_cdf_ms (const int cdf)
{
ms_init_global( TRUE, &admin_data, &sample_data, &test_data,
                  &raw_global_data );
ms_close( cdf );
return;
}


/************************************************************************
* Function:     readscan                                                *
* Description:  Reads a given scan from a netCDF file into a file       *
* Arguments:    - file handle (must be open)                            *
*               - scan nr to read                                       *
*               - name of file to write to                              *
* Returns:      1 if OK, 0 if error.                                    *
* Author:       Joerg Hau <joerg.hau(at)dplanet.ch>, 2001-05-16         *
*               based on example code from David Stranz                 *
*************************************************************************/
int readscan(const int cdf, const long scan, const char *fnam)
{
FILE *file;
long j;
double mass_pt = 0.0, inty_pt = 0.0;

if (!(file = fopen(fnam, "w")))
        {
        fprintf(stderr,
        "\nreadscan: cannot open intermediate file!");
        return 0;
        }

ms_init_per_scan(FALSE, &raw_data, NULL);
raw_data.scan_no = (long) scan;

if (MS_ERROR == ms_read_per_scan(cdf, &raw_data, NULL))
        {               /* free allocated memory before leaving */
        fprintf(stderr,
        "\nreadscan: ms_read_per_scan failed (scan %ld)!", scan);
        ms_init_per_scan(TRUE, &raw_data, NULL);
        return 0;
        }

if ( raw_data.points > 0 )
        {
        for ( j = 0; j < raw_data.points; j++ )
            {
            switch( raw_global_data.mass_format )
                {
                case data_short:
                    mass_pt = (double) ((short *)raw_data.masses)[j];
                    break;

                case data_long:
                    mass_pt = (double) ((long *)raw_data.masses)[j];
                    break;

                case data_float:
                    mass_pt = (double) ((float *)raw_data.masses)[j];
                    break;

                case data_double:
                    mass_pt = ((double *)raw_data.masses)[j];
                    break;
                }
            mass_pt *= raw_global_data.mass_factor;

            switch( raw_global_data.intensity_format )
                {
                case data_short:
                    inty_pt = (double) ((short *)raw_data.intensities)[j];
                    break;

                case data_long:
                    inty_pt = (double) ((long *)raw_data.intensities)[j];
                    break;

                case data_float:
                    inty_pt = (double) ((float *)raw_data.intensities)[j];
                    break;

                case data_double:
                    inty_pt = (double) ((double *)raw_data.intensities)[j];
                    break;
                }

            inty_pt = inty_pt * raw_global_data.intensity_factor +
                       raw_global_data.intensity_offset;

            if (!fprintf(file, "%g\t%g\n", mass_pt, inty_pt))
                {
                fprintf(stderr,
                "\nreadscan: cannot write intermediate data!");
                ms_init_per_scan( TRUE, &raw_data, NULL );
                fclose(file);
                return 0;
                }

            if (raw_data.flags > 0)
               printf("\nWarning: There are flags in scan %ld (ignored).", scan);
         } /* for all data points in one scan */
       }    /* if there are data points */
ms_init_per_scan( TRUE, &raw_data, NULL );
fclose(file);

return 1;
}



/************************************************************************
* Function:     readchro                                                *
* Description:  Reads mass chromatogram from a netCDF file into a file  *
* Arguments:    - file handle (must be open)                            *
*               - low & high mass limit to read                         *
*               - name of file to write to                              *
* Returns:      1 if OK, 0 if error.                                    *
* Author:       Joerg Hau <joerg.hau(at)dplanet.ch>, 2001-05-16         *
* Note:         This is pretty slow - unfortunately, we have to read    *
*               the whole CDF file scan-by-scan to reconstitute a mass  *
*               chromatogram. This is inherent to the file structure.   *
*************************************************************************/
int readchro
    (const int cdf, const double lomass, const double himass, const char *fnam)
{
FILE    *file;
long    i, scan, nscans;
double  mass_pt, inty_pt, inty;

nscans = raw_global_data.nscans;

if (!(file = fopen(fnam, "w")))
        {
        fprintf(stderr,
        "\nreadchro: cannot open intermediate file!");
        return 0;
        }

for (scan = 0; scan < nscans; scan++)
    {
    ms_init_per_scan(FALSE, &raw_data, NULL);
    raw_data.scan_no = (long) scan;
    mass_pt = inty_pt = inty = 0.0;                     /* init */

    if (MS_ERROR == ms_read_per_scan(cdf, &raw_data, NULL))
        {               /* free allocated memory before leaving */
        fprintf(stderr,
        "\nreadchro: ms_read_per_scan failed (scan %ld)!", scan);
        ms_init_per_scan(TRUE, &raw_data, NULL);
        return 0;
        }

    if (!raw_data.points)       /* empty scan? */
        break;
    else                        /* there are data points */
        for (i = 0; i < raw_data.points; i++)
            {
            switch( raw_global_data.mass_format )
                {
                case data_short:
                    mass_pt = (double) ((short *)raw_data.masses)[i];
                    break;

                case data_long:
                    mass_pt = (double) ((long *)raw_data.masses)[i];
                    break;

                case data_float:
                    mass_pt = (double) ((float *)raw_data.masses)[i];
                    break;

                case data_double:
                    mass_pt = ((double *)raw_data.masses)[i];
                    break;
                }

            mass_pt *= raw_global_data.mass_factor;

            /* m/z inside a netCDF file are in ascending order, so we can skip
               as soon as the recorded mass is above the upper window limit */

            if (mass_pt > himass)
                break;

            /* if mass is below himass, let's search for something inside
               the tolerance window */

            if (mass_pt > lomass)
                {
                switch( raw_global_data.intensity_format )
                    {
                    case data_short:
                        inty_pt = (double) ((short *)raw_data.intensities)[i];
                        break;

                    case data_long:
                        inty_pt = (double) ((long *)raw_data.intensities)[i];
                        break;

                    case data_float:
                        inty_pt = (double) ((float *)raw_data.intensities)[i];
                        break;

                    case data_double:
                        inty_pt = (double) ((double *)raw_data.intensities)[i];
                        break;
                    }

                inty_pt = inty_pt * raw_global_data.intensity_factor +
                       raw_global_data.intensity_offset;

                inty += inty_pt;        /* sum up */

                if (raw_data.flags > 0)
                   printf("\nWarning: There are flags in scan %ld (ignored).",
                        scan);
                }  /* mass_ptr > lomass ...*/
        }       /* i loop */

    ms_init_per_scan( TRUE, &raw_data, NULL );

    /* write to file */
    if (!fprintf(file, "%ld\t%g\n", scan, inty))
        {
        fprintf(stderr,
            "\nreadchro: cannot write intermediate data!");
        fclose(file);
        return 0;
        }

    }   /* scan loop */

fclose(file);
return 1;
}


/************************************************************************
* Function:     read_line                                               *
* Description:  reads a line from the console                           *
* Arguments:    pointer to prompt, max number of chars to read          *
* Returns:      pointer to string, 0 if problem or empty                *
* Author:       Joerg Hau <joerg.hau(at)dplanet.ch>                     *
*               based on the readline() documentation                   *
*************************************************************************/
char *read_line(char *prompt, int maxlen)
{
#ifdef READLINE
static char *line_read = (char *)NULL;

/* If the buffer has already been allocated, return the memory to the free pool. */
if (line_read)
    {
    free (line_read);
    line_read = (char *)NULL;
    }

line_read = readline (prompt);  /* Get a line from the user. */

/* If there is any text, save it on the history. */
if (line_read && *line_read)
    add_history (line_read);

return (line_read);
#else
static char in[MAXLEN];

printf ("%s", prompt);
return (fgets(in, maxlen, stdin));
#endif
}


/************************************************************************
* Function:     strclean                                                *
* Description:  "cleans" a text buffer obtained by fgets()              *
* Arguments:    Pointer to text buffer                                  *
* Returns:      strlen of buffer                                        *
* Author:       Joerg Hau <joerg.hau(at)dplanet.ch>                     *
*************************************************************************/
int strclean (char *buf)
{
int i;

for (i = 0; i < strlen (buf); i++)	/* search for CR/LF */
    {
    if (buf[i] == '\n' || buf[i] == '\r')
        {
	    buf[i] = 0;		/* stop at CR or LF */
	    break;
        }
    }
return (strlen (buf));
}


/************************************************************************
* Function:     fifo_changed                                            *
* Description:  test if FIFO has been modified                          *
* Arguments:    Pointer to fifo (string)                                *
* Returns:      1 if FIFO was modified since last call to this function *
*               0 if not modified                                       *
*               -1 if error                                             *
* Author:       Joerg Hau <joerg.hau(at)dplanet.ch>                     *
*************************************************************************/
int fifo_changed (const char *fifo)
{
struct stat statinfo;
static time_t oldtime = 0;

if (stat(GPFIFO,&statinfo) == -1)
    return -1;

if (oldtime == statinfo.st_mtime)   /* not changed */
    return 0;
                                    /* else */
oldtime = statinfo.st_mtime;        /* update */
return 1;                           /* changed */
}



/***************************************************************************
* GETOPT: Command line parser, system V style.
*
*  Widely (and wildly) adapted from code published by Borland Intl. Inc.
*
*  Note that libc has a function getopt(), however this is not guaranteed
*  to be available for other compilers. Therefore we provide *this* function
*  (which does the same).
*
*  Standard option syntax is:
*
*    option ::= SW [optLetter]* [argLetter space* argument]
*
*  where
*    - SW is '-'
*    - there is no space before any optLetter or argLetter.
*    - opt/arg letters are alphabetic, not punctuation characters.
*    - optLetters, if present, must be matched in optionS.
*    - argLetters, if present, are found in optionS followed by ':'.
*    - argument is any white-space delimited string.  Note that it
*      can include the SW character.
*    - upper and lower case letters are distinct.
*
*  There may be multiple option clusters on a command line, each
*  beginning with a SW, but all must appear before any non-option
*  arguments (arguments not introduced by SW).  Opt/arg letters may
*  be repeated: it is up to the caller to decide if that is an error.
*
*  The character SW appearing alone as the last argument is an error.
*  The lead-in sequence SWSW ("--") causes itself and all the rest
*  of the line to be ignored (allowing non-options which begin
*  with the switch char).
*
*  The string *optionS allows valid opt/arg letters to be recognized.
*  argLetters are followed with ':'.  Getopt () returns the value of
*  the option character found, or EOF if no more options are in the
*  command line. If option is an argLetter then the global optarg is
*  set to point to the argument string (having skipped any white-space).
*
*  The global optind is initially 1 and is always left as the index
*  of the next argument of argv[] which getopt has not taken.  Note
*  that if "--" or "//" are used then optind is stepped to the next
*  argument before getopt() returns EOF.
*
*  If an error occurs, that is an SW char precedes an unknown letter,
*  then getopt() will return a '~' character and normally prints an
*  error message via perror().  If the global variable opterr is set
*  to false (zero) before calling getopt() then the error message is
*  not printed.
*
*  For example, if
*
*    *optionS == "A:F:PuU:wXZ:"
*
*  then 'P', 'u', 'w', and 'X' are option letters and 'A', 'F',
*  'U', 'Z' are followed by arguments. A valid command line may be:
*
*    aCommand  -uPFPi -X -A L someFile
*
*  where:
*    - 'u' and 'P' will be returned as isolated option letters.
*    - 'F' will return with "Pi" as its argument string.
*    - 'X' is an isolated option.
*    - 'A' will return with "L" as its argument.
*    - "someFile" is not an option, and terminates getOpt.  The
*      caller may collect remaining arguments using argv pointers.
***************************************************************************/
int GetOpt(int argc, char *argv[], char *optionS)
{
static char *letP = NULL;            /* remember next option char's location */
static char SW = '-';                /* switch character */

int opterr = 1;                      /* allow error message        */
unsigned char ch;
char *optP;

if (argc > optind)
        {
        if (letP == NULL)
                {
                if ((letP = argv[optind]) == NULL || *(letP++) != SW)
                        goto gopEOF;

                if (*letP == SW)
                        {
                        optind++;
                        goto gopEOF;
                        }
                }
        if (0 == (ch = *(letP++)))
                {
                optind++;
                goto gopEOF;
                }
        if (':' == ch  ||  (optP = strchr(optionS, ch)) == NULL)
                goto gopError;
        if (':' == *(++optP))
                {
                optind++;
                if (0 == *letP)
                        {
                        if (argc <= optind)
                                goto  gopError;
                        letP = argv[optind++];
                        }
                optarg = letP;
                letP = NULL;
        }
        else
        {
        if (0 == *letP)
                {
                optind++;
                letP = NULL;
                }
        optarg = NULL;
        }
        return ch;
}

gopEOF:
        optarg = letP = NULL;
        return EOF;

gopError:
        optarg = NULL;
        errno  = EINVAL;
        if (opterr)
                perror ("\nCommand line option");
        return ('~');
}

