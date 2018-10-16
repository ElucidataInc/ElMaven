Release History
===============

|

Current Release
---------------

`Version 0.4.1 <https://zenodo.org/record/1332034>`_

Publication Date: August 4\ :sup:`th`, 2018

* Optimisations

   * Faster sample upload
   * Checking/unchecking a sample in the presence of a large peak table is faster `(#723) <https://github.com/ElucidataInc/ElMaven/issues/723>`_
   * No lag in switching between two large peak tables

* UI Changes

   * Added a close button for Scatter Plot widget
   * Removed 'Load Peaks' from Peak tables
   * 'Gallery' widget is only available on the side bar panel
   * Removed 'Ratio' and 'p-value' columns from Peak tables (available in Scatter Plot Peak Table)
   * Removed 'Good' and 'Bad' marking options from EIC widget
   * Polly upload button is not clickable during the upload process `(#805) <https://github.com/ElucidataInc/ElMaven/issues/805>`_
   * Single button for exporting EIC on EIC widget
   * Removed 'Scatter Plot' button from side bar panel

* Bugs

   * Isotopic peak filtering based on peak quality has been fixed `(#772) <https://github.com/ElucidataInc/ElMaven/issues/772>`_
   * Crash in bookmark button has been fixed `(#768) <https://github.com/ElucidataInc/ElMaven/issues/768>`_
   * Empty tables will no longer be uploaded to Polly `(#777) <https://github.com/ElucidataInc/ElMaven/issues/777>`_
   * Only a single compound database file (.csv) will be uploaded to Polly

Past Releases
-------------

.. `Version 0.4.0-beta.1 <https://zenodo.org/record/1313542>`_

.. Publication Date: July 17\ :sup:`th`, 2018

   * Fixed Isotopic detection in shift-drag integration `(#781) <https://github.com/ElucidataInc/ElMaven/issues/781>`_
   * Groups with zero peaks will not be bookmarked on shift-drag integration
   * Fixed a bug in changing compound database from the drop-down
   * Getting Started window will not open behind the main application `(#775) <https://github.com/ElucidataInc/ElMaven/issues/775>`_

`Version 0.4.0-beta.1 <https://zenodo.org/record/1312704>`_

Publication Date: July 16\ :sup:`th`, 2018

   * Fixed Isotopic detection in shift-drag integration `(#781) <https://github.com/ElucidataInc/ElMaven/issues/781>`_
   * Groups with zero peaks will not be bookmarked on shift-drag integration
   * Fixed a bug in changing compound database from the drop-down
   * Getting Started window will not open behind the main application `(#775) <https://github.com/ElucidataInc/ElMaven/issues/775>`_

`Version 0.4.0-beta <https://zenodo.org/record/1305465>`_

Publication Date: July 5\ :sup:`th`, 2018 

`Version 0.3.2 <https://zenodo.org/record/1248658>`_

Publication Date: May 17\ :sup:`th`, 2018

`Version 0.3.1 <https://zenodo.org/record/1232373>`_

Publication Date: April 27\ :sup:`th`, 2018

* Bug Fixes:

   * Fixed compound name in CSV export `(#635) <https://github.com/ElucidataInc/ElMaven/issues/635>`_
   * Fixed automated detection of high ranking groups
   * Fix installer issue on Mac (Installer not working due to netcdf addition)

* UI:

   * Change default tab for El-MAVEN - Polly Integration dialog

`Version 0.3.1 <https://zenodo.org/record/1230370>`_

Publication Date: April 26\ :sup:`th`, 2018

`Version 0.3.0 <https://zenodo.org/record/1228065>`_

Publication Date: April 24\ :sup:`th`, 2018

`Version 0.3.0 <https://zenodo.org/record/1227187>`_

Publication Date: April 23\ :sup:`rd`, 2018

`Version 0.3.0-beta <https://zenodo.org/record/1216928>`_

Publication Date: April 11\ :sup:`th`, 2018

* Features

   * Export Scatter plot groups into a peak table
   * Introduced Exception handling in sample upload process
   * Send logs to sentry in the event of a crash
   * Upload data to Polly through CLI and GUI
   * Download project settings from Polly
   * New Alignment algorithm: Obi-warp
   * Support for cdf files on Windows
   * Highlight detected compounds in the database

* Enhancement
   
   * Consistent sample ordering across widgets
   * Append Sample number to Sample Name for mzML files

* Refactor
 
   * Isotope Widget
   * Isotope detection

* Bug fixes
 
   * Fixed Isotopes expected m/z in mzroll
   * Blank samples will be visible in sample widget
   * Fix trailing delimiter in peak detailed format
   * Isotopic intensities are consistent between isotope widget and peaks table

`Version 0.2.4 <https://zenodo.org/record/1168226>`_

Publication Date: February 7\ :sup:`th`, 2018

`Version 0.2.4 <https://zenodo.org/record/1165654>`_

Publication Date: February 5\ :sup:`th`, 2018

`Version 0.2.4 Beta <https://zenodo.org/record/1158577>`_

Publication Date: January 24\ :sup:`th`, 2018

`Version 0.2.3 <https://zenodo.org/record/1157953>`_

Publication Date: January 23\ :sup:`rd`, 2018

`Version 0.2.2 <https://zenodo.org/record/1133506>`_

Publication Date: December 28\ :sup:`th`, 2017
