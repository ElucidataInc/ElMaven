Labeled LC-MS/MS Workflow
=========================

This is a tutorial for processing LC/MSMS data files through El-MAVEN.
Convert files to mzml/mzxml format using msConvert

**Contents**

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

**Preprocessing**

msConvert is a command-line/ GUI tool that is used to convert between various mass spectroscopy 
data formats, developed and maintained by proteoWizard.

Input

msConvert supports the following formats:

    * .mzXML
    * .mzML
    * .RAW Thermofisher
    * .RAW Walters
    * .d Agilent
    * .wiff format ABSciex

The settings used for msConvert as a GUI tool are captured in the following screenshots: 

Screenshot 1

NOTE: It is important that Zlib compression is enabled by default in msConvert. El-MAVEN 
in its current form does not support Zlib compression. Make sure to uncheck "Use Zlib compression" box.

Output

msConvert can convert to an array of different formats but El-MAVEN primarily uses .mzXML and .mzML formats.

**Launch El-MAVEN**

Once sample files are ready for processing, launch El-MAVEN. 

Screenshot 2

**Adjust Global Settings**

Global Settings can be changed from the *Options* dialog (Insert icon). There are 9 tabs in the dialog. Each of these 
tabs has parameters related to a different module in El-MAVEN. For example, a tab for Instrumentation 
details, a tab for file import settings, et cetera.

Screenshot 3

To know more about the functionality of different tabs and their settings, user can see the 
`Widgets page <https://github.com/ElucidataInc/El-MAVEN/wiki/Introduction-to-El-MAVEN-UI#2-global-settings>`_. 
Please be sure to set the desired settings before processing an input file.

**Load Samples**

User can go to File in the Menu, click on *Load Samples|Projects|Peaks* option in the File menu. 
Then navigate to the folder containing the sample data, select all .mzXML or .mzml files and click Open. 
A loading bar displays the progress at the bottom. 

Screenshot 4

When the samples have loaded, user should see a sample panel on the left side. If it is not displayed 
automatically, click on the Show Samples Widget button on the toolbar. El-MAVEN automatically assigns a 
color to every sample. User can select/deselect any sample by clicking the checkbox on the left of the 
sample name. 

Screenshot 5

**Load Compound Database**

User can click on *Compounds* option in the leftmost menu, navigate to the folder containing the 
standard database file, select the appropriate .csv file and click *Open*. 

Screenshot 6

This is a sample Compound Database: 

Screenshot 7

It lists all metabolite names, chemical formula, HMDB ID, and the class/category of compounds they 
belong to (if known).

**Mark Blanks**

The user can mark the blanks by selecting the blank samples from the list on screen, and clicking 
on the *Set as a Blank Sample* icon (Insert icon) in Samples menu.

Screenshot 8

Multiple blanks can be marked together. The blanks will appear black as shown in the image below: 

Screenshot 9

