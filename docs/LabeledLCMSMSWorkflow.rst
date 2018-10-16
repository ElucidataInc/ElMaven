Labeled LC-MS/MS Workflow
=========================

.. All widget icons are referenced here

.. |options| image:: /image/Widget_1.png
.. |mark sample as blank| image:: /image/Widget_10.png
.. |export to csv| image:: /image/Widget_36.png
.. |generate pdf| image:: /image/Widget_35.png
.. |export to json| image:: /image/Widget_37.png

.. |LCMSMS01| image:: /image/LCMSMS_1.png
.. |LCMSMS02| image:: /image/LCMSMS_2.png
.. |LCMSMS03| image:: /image/LCMSMS_3.png
.. |LCMSMS04| image:: /image/LCMSMS_4.png
.. |LCMSMS05| image:: /image/LCMSMS_5.png
.. |LCMSMS06| image:: /image/LCMSMS_6.png
.. |LCMSMS07| image:: /image/LCMSMS_7.png
.. |LCMSMS08| image:: /image/LCMSMS_8.png
.. |LCMSMS09| image:: /image/LCMSMS_9.png 
.. |LCMSMS10| image:: /image/LCMSMS_10.png
.. |LCMSMS11| image:: /image/LCMSMS_11.png
.. |LCMSMS12| image:: /image/LCMSMS_12.png

.. **Contents**

    * Preprocessing
    * Launch El-MAVEN
    * Adjust Global Settings
    * Load Samples
    * Load Compound Database
    * Mark Blanks
    * Alignment
    * Peak Grouping
    * Baseline
    * Mass Spectra
    * Peak Curation
    * Guidelines for Peak Picking
    * Export options

This is a tutorial for processing LC/MSMS data files through El-MAVEN.

Preprocessing
-------------

msConvert is a command-line/GUI tool that is used to convert between various mass spectroscopy data formats, developed and maintained by proteoWizard. Raw data files obtained from mass spectrometers need to be converted to certain acceptable formats before processing in El-MAVEN.

**Input**

msConvert supports the following formats:

    * .mzXML
    * .mzML
    * .RAW ThermoFisher
    * .RAW Walters
    * .d Agilent
    * .wiff ABSciex

The settings used for msConvert as a GUI tool are captured in the following screenshot: 

|LCMSMS01|

.. note::

 It is important that zlib compression is enabled by default in msConvert. El-MAVEN in its current form does not support zlib compression. Make sure to uncheck "Use zlib compression" box.

**Output**

msConvert can convert to an array of different formats but El-MAVEN primarily uses .mzXML and .mzML formats.

Launch El-MAVEN
---------------

Once sample files are ready for processing, launch El-MAVEN. 

|LCMSMS02|

Adjust Global Settings
----------------------

Global Settings can be changed from the *Options* dialog |options|. There are 9 tabs in the dialog. Each of these tabs has parameters related to a different module in El-MAVEN. For example, a tab for Instrumentation details, a tab for file import settings, etc.

|LCMSMS03|

To know more about the functionality of different tabs and their settings, users can see the `Widgets page <https://elmaven.readthedocs.io/en/develop/IntroductiontoElMAVENUI.html#global-settings>`_. Please be sure to set the desired settings before processing an input file.

Load Samples
------------

Users can go to File in the Menu, click on *Load Samples|Projects|Peaks* option in the File menu. Then navigate to the folder containing the sample data, select all .mzXML or .mzml files and click Open. A loading bar displays the progress at the bottom. 

|LCMSMS04|

When the samples have loaded, users should see a sample panel on the left side. If it is not displayed automatically, click on the *Show Samples Widget* button on the toolbar. El-MAVEN automatically assigns a color to every sample. Users can select/deselect any sample by clicking the checkbox on the left of the sample name. 

|LCMSMS05|

Load Compound Database
----------------------

Users can click on *Compounds* option in the leftmost menu, navigate to the folder containing the standard database file, select the appropriate .csv file and click *Open*. Alternatively, users may use any of the default files loaded on start-up.

|LCMSMS06|

This is a sample Compound Database: 

|LCMSMS07|

It lists all metabolite names, chemical formula, HMDB ID, and the class/category of compounds they belong to (if known).

Mark Blanks
-----------

Users can mark the blanks by selecting the blank samples from the list on screen, and clicking on the *Set as a Blank Sample* icon |mark sample as blank| in Samples menu.

|LCMSMS08|

Multiple blanks can be marked together. The blanks will appear black as shown in the image below: 

|LCMSMS09|

Alignment
---------

(missing)

Export
------

There are multiple export options available for storing marked peak data. Users can either generate a PDF report to save the EIC for every metabolite, export data for a particular group in .csv format, or export the EICs to a Json file as shown below. 

|LCMSMS10|

Users can select *All, Good, Bad or Selected* peaks to export.

|LCMSMS11|

The *Export Groups to CSV* option |export to csv| lets the users save the 'good'/'bad' labels along with the peak table. Users also have the option to filter out rows that have a certain label while exporting the table. 

*Generate PDF Report* option |generate pdf| saves all EICs with their corresponding bar plots in a PDF file.

*Export EICs to Json* option |export to json| exports all EICs to a Json file.

Another option is to export the peak data in .mzroll format that can be directly loaded into El-MAVEN by clicking on the Load *Samples|Projects|Peaks* option in the File menu. For this, go to the File option in the menu bar, and click on '*Save Project*'.

|LCMSMS12|
