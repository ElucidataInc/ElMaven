Labeled LC-MS Workflow
======================

This is a tutorial for processing Labeled LC/MS data files through El-MAVEN.

.. All widget icons are referenced here

.. |options| image:: /image/Widget_1.png
.. |mark sample as blank| image:: /image/Widget_10.png
.. |show isotope plot| image:: /image/Widget_23.png
.. |show alignment visualisation| image:: /image/Widget_26.png
.. |align| image:: /image/Widget_25.png
.. |mass spectra widget| image:: /image/Widget_38.png
.. |peaks| image:: /image/Widget_29.png
.. |export to csv| image:: /image/Widget_36.png
.. |generate pdf| image:: /image/Widget_35.png
.. |export to json| image:: /image/Widget_37.png

.. |LLCMS01| image:: /image/LLCMS_1.png
.. |LLCMS02| image:: /image/LLCMS_2.png
.. |LLCMS03| image:: /image/LLCMS_3.png
.. |LLCMS04| image:: /image/LLCMS_4.png
.. |LLCMS05| image:: /image/LLCMS_5.png
.. |LLCMS06| image:: /image/LLCMS_6.png
.. |LLCMS07| image:: /image/LLCMS_7.png
.. |LLCMS08| image:: /image/LLCMS_8.png
.. |LLCMS09| image:: /image/LLCMS_9.png 
.. |LLCMS10| image:: /image/LLCMS_10.png
.. |LLCMS11| image:: /image/LLCMS_11.png
.. |LLCMS12| image:: /image/LLCMS_12.png
.. |LLCMS13| image:: /image/LLCMS_13.png
.. |LLCMS14| image:: /image/LLCMS_14.png
.. |LLCMS15| image:: /image/LLCMS_15.png
.. |LLCMS16| image:: /image/LLCMS_16.png
.. |LLCMS17| image:: /image/LLCMS_17.png
.. |LLCMS18| image:: /image/LLCMS_18.png
.. |LLCMS19| image:: /image/LLCMS_19.png
.. |LLCMS20| image:: /image/LLCMS_20.png 
.. |LLCMS21| image:: /image/LLCMS_21.png
.. |LLCMS22| image:: /image/LLCMS_22.png
.. |LLCMS23| image:: /image/LLCMS_23.png
.. |LLCMS24| image:: /image/LLCMS_24.png
.. |LLCMS25| image:: /image/LLCMS_25.png
.. |LLCMS26| image:: /image/LLCMS_26.png
.. |LLCMS27| image:: /image/LLCMS_27.png
.. |LLCMS28| image:: /image/LLCMS_28.png
.. |LLCMS29| image:: /image/LLCMS_29.png
.. |LLCMS30| image:: /image/LLCMS_30.png
.. |LLCMS31| image:: /image/LLCMS_31.png 
.. |LLCMS32| image:: /image/LLCMS_32.png
.. |LLCMS33| image:: /image/LLCMS_33.png
.. |LLCMS34| image:: /image/LLCMS_34.png
.. |LLCMS35| image:: /image/LLCMS_35.png
.. |LLCMS36| image:: /image/LLCMS_36.png
.. |LLCMS37| image:: /image/LLCMS_37.png
.. |LLCMS38| image:: /image/LLCMS_38.png
.. |LLCMS39| image:: /image/LLCMS_39.png
.. |LLCMS40| image:: /image/LLCMS_40.png
.. |LLCMS41| image:: /image/LLCMS_41.png
.. |LLCMS42| image:: /image/LLCMS_42.png 
.. |LLCMS43| image:: /image/LLCMS_43.png
.. |LLCMS44| image:: /image/LLCMS_44.png
.. |LLCMS45| image:: /image/LLCMS_45.png
.. |LLCMS46| image:: /image/LLCMS_46.png
.. |LLCMS47| image:: /image/LLCMS_47.png
.. |LLCMS48| image:: /image/LLCMS_48.png
.. |LLCMS49| image:: /image/LLCMS_49.png
.. |LLCMS50| image:: /image/LLCMS_50.png
.. |LLCMS51| image:: /image/LLCMS_51.png
.. |LLCMS52| image:: /image/LLCMS_52.png
.. |LLCMS53| image:: /image/LLCMS_53.png 
.. |LLCMS54| image:: /image/LLCMS_54.png
.. |LLCMS55| image:: /image/LLCMS_55.png

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
   * .wiff format ABSciex

The settings used for msConvert as a GUI tool are captured in the following screenshots: 

|LLCMS01|

NOTE: zlib compression is enabled by default in msConvert. El-MAVEN in its current form does not support zlib compression. It is important to uncheck "Use zlib compression" box.

**Output**

msConvert can convert to an array of different formats but El-MAVEN primarily uses .mzXML and .mzML formats.

Launch El-MAVEN
---------------

Once sample files are ready for processing, launch El-MAVEN. 

|LLCMS02|

Adjust Global Settings
----------------------

Global Settings can be changed from the *Options* dialog |options|. There are 9 tabs in the dialog. Each of these tabs has parameters related to a different module in El-MAVEN. For example, a tab for Instrumentation details, a tab for File Import settings etc.

|LLCMS03|

The *m/z* option scans the groups to find any specific m/z value and plot its corresponding EIC. The +/- option to its right is to specify the expected mass resolution error in parts per million (ppm).

|LLCMS04|

To know more about the functionality of different tabs and their settings, users can see the `Widgets page <https://github.com/ElucidataInc/El-MAVEN/wiki/Introduction-to-El-MAVEN-UI#2-global-settings>`_. Please be sure to set the desired settings before processing an input file.

Load Samples
------------

Users can go to File in the Menu, click on *Load Samples|Projects|Peaks* option in the File menu. Then navigate to the folder containing the sample data, select all .mzXML or .mzML files and click Open. A loading bar displays the progress at the bottom. 

|LLCMS05|

When the samples have loaded, users should see a sample panel on the left side. If it is not displayed automatically, click on the *Show Samples Widget* button on the toolbar. El-MAVEN automatically assigns a color to every sample. Users can select/deselect any sample by clicking the checkbox on the left of the sample name. 

|LLCMS06|

Load Compound Database
----------------------

Users can click on *Compounds* option in the leftmost menu, navigate to the folder containing the standard database file, select the appropriate .csv file and click *Open*. 

|LLCMS07|

This is a sample Compound Database: 

|LLCMS08|

It lists all metabolite names, chemical formula, HMDB ID, and the class/category of compounds they belong to (if known).

Mark Blanks
-----------

Users can mark the blanks by selecting the blank samples from the list on screen, and clicking on the *Set as a Blank Sample* icon |mark sample as blank| in Samples menu.

|LLCMS09|

Multiple blanks can be marked together. The blanks will appear black as shown in the image below: 

|LLCMS10|

Alignment
---------

Prolonged use of the LC column can lead to a drift in retention time across samples. Alignment shifts the peak RTs in every sample to correct for this drift and brings the peaks closer to median RT of the group.

|LLCMS11|

In the above image, EIC for a UTP group is displayed. If the samples were aligned, all peaks would lie at the same RT. Since this is not the case the samples need to be aligned.

*Alignment visualization* |show alignment visualisation| can be used to judge the extent of deviation from median RT. 

|LLCMS12|

In the above visualization, each box represents a peak from the selected group at its current RT. Samples are said to be perfectly aligned when all peak boxes lie on the same vertical axis. The peaks are considerably scattered in the above image and therefore the samples should be aligned for better grouping of peaks.

**Perform Alignment**

*Alignment settings* can be adjusted using the Align button |align|. This example was aligned with *Poly fit* algorithm with default parameters. 

|LLCMS13|

Post-alignment the peaks in the group should appear closer to the median RT of the group.

|LLCMS14|

|LLCMS15|

Pre-alignment, the peaks were considerably scattered while the aligned peaks lie nearly on the same axis. Users can run alignment again with different parameters if required (or with a different algorithm). Further details on Alignment settings are available on the `Widgets page <https://github.com/ElucidataInc/El-MAVEN/wiki/Introduction-to-El-MAVEN-UI#2-global-settings>`_.

Peak Grouping
-------------

Peak grouping is an integral part of the El-MAVEN workflow that categorizes all detected peaks into groups on the basis of certain user-controlled parameters. A group score is calculated for every peak during the process. The formula for this score takes into account the difference in RT, intensities between peaks (smaller difference leads to a better score) and any existing overlap between them (higher extent of overlap leads to better score). All three parameters have certain weights attached to them that can be controlled by the users. The formula for the score is shown in the image. More details on it can be found on the `Widgets page <https://github.com/ElucidataInc/El-MAVEN/wiki/Introduction-to-El-MAVEN-UI#2-global-settings>`_.

|LLCMS16|

|LLCMS17|

The above image shows two groups in the EIC window. The highlighted (solid circles) peaks belong to group A, the peaks to its left with empty circles belong to another group B. The short peaks in group A that are close to the baseline and peaks in group B come from the same samples. Additionally, the high intensity peaks of group A have a similar peak shape to group B peaks. These peaks might have been wrongly classified into separate groups because of the difference in Rt range of the two sets of peaks. The weights attached to difference in Rt and intensities, and extent of overlap can be adjusted for better grouping.

Grouping parameters can be changed from the Options dialog |options|.

|LLCMS18|

|LLCMS19|

Giving less priority to difference in RTs and intensities results in the two groups being merged into a single  group while the peaks that lay close to the baseline are no longer classified as valid peaks.

.. Screenshots 17 and 19 regarding the grouping of sarcosine are from the old documentation. Peak grouping was not replicated successfully hence the screenshots were re-used.

Baseline
--------

When measuring a number of peaks, it is often more effective to subtract an estimated baseline from the data. This baseline should be set where ideally no peaks occur. Although sometimes the program sets a particular baseline such that one or more peaks occur below that baseline value. In the following image, the dashed line represents each baseline:

|LLCMS20|

The corresponding peaks are indicated with solid circles: 

|LLCMS21|

The baseline correction can be done in the *Peak Detection* tab by clicking on *Options* button: 

|LLCMS22|

Further details on settings can be accessed `here <https://github.com/ElucidataInc/El-MAVEN/wiki/Introduction-to-El-MAVEN-UI#peak-detection>`_.

Isotope Detection
-----------------

**Samples are labeled?**

The *Peaks* dialog |peaks| can be used to detect labeled peaks along with the unlabeled ones in the Peaks Table. 

|LLCMS23|

On opening the *Feature Detection Selection* tab, the *Report Isotopic Peaks* box must be checked. Clicking on the *Isotope Detection Options* gives the following window. Alternately, these settings can also be accessed from the *Options* dialog. 

|LLCMS24|

* *Bookmarks, peak detection, file export*: To select the labeled atoms that should be used in bookmarking, peak detection and export. D2: Deuterium, C13: Labeled carbon, N15: Labeled nitrogen, S34: Labeled sulphur.

* *Isotopic widget*: To select the labeled atoms that should be displayed in the isotopic widget. D2: Deuterium, C13: Labeled carbon, N15: Labeled nitrogen, S34: Labeled sulphur.

* *Number of M+n isotopes*: To set the maximum number of labeled atoms per ion in the experiment.

* *Abundance Threshold*: To set the minimum threshold for isotopic abundance. Isotopic abundance is the ratio of intensity of isotopic peak over the parent peak.

**Filter Isotopic Peaks based on these criteria**

* *Minimum Isotope-Parent Correlation*: To set the minimum threshold for isotope-parent peak correlation. This correlation is a measure of how often they appear together.

* *Isotope is within [X] scans of parent*: To set the maximum scan difference between isotopic and parent peaks. This is a measure of how closely they appear together on the RT scale.

* *Maximum % Error to Natural Abundance*: To set the maximum natural abundance error expected. Natural abundance of an isotope is the expected ratio of amount of isotope over the amount of parent molecule in nature. Error is the difference between observed and natural abundance as a fraction of natural abundance.

* *Correct for Natural C13 Isotope Abundance*: The box should be checked to correct for natural C13 abundance.

In the image below, Peak Table 3 has a drop down button with metabolites that shows all labeled isotopologues of that particular metabolite.

|LLCMS25|

|LLCMS26|

|LLCMS27| 

**Show Isotope Plots**

This icon |show isotope plot| on top displays the isotope plots for a group. The red colored bar plot for UTP group is shown below. Each bar in the plot represents the relative percentage of different isotopic species for the selected group in a sample.

|LLCMS28| 

Mass Spectra
------------

Mass Spectra Widget |mass spectra widget| displays each peak, its mass, and intensity for a scan. As the widget shows all detected masses in a scan, the ppm window for the EIC and consequently grouping can be adjusted accordingly. This feature is especially useful for MS/MS data and isotopic detection. 

|LLCMS29|

Peak Curation
-------------

Generally there are two broad workflows to curate peaks in El-MAVEN:

   * Manual Peak Curation using Compound DB widget

   * Automated Peak Curation

Although, for labeled data Automatic Peak Curation is not meaningful because it will not curate any labeled groups.

**Manual Peak Curation using Compound DB widget**

Clicking the *Peaks* icon |peaks| on the top opens the settings dialog.

|LLCMS30|

The user must check the box for *Report Isotopic Peaks* in the *Group Filtering* tab.

NOTE: The user should not click on *Find Peaks* after checking the box for manual curation. Clicking on that option would start Automatic Peak Detection. For adjusting other settings the user can access them through the Options |options| dialog .

For more details on how to access Peak Detection settings, read this `Widgets page <https://github.com/ElucidataInc/ElMaven/wiki/Introduction-to-ElMaven-UI#peak-detection>`_.

To use manual curation using the compound DB widget, the user has to iterate over all the compounds in the compound DB on the extreme left of the window, as highlighted in the images below. 

|LLCMS31|

|LLCMS32|

Once on a compound, El-MAVEN shows the highest ranked group for that m/z. The user can now choose a group or reject it. There are two ways to do this.

In the first workflow, the user needs to double click on the peak group of his choice. This will get the RT line to the median of the group and also add the metabolite to the bookmarks table (as shown in the image below). User can read more about the bookmarks table `here <https://github.com/ElucidataInc/ElMaven/wiki/Introduction-to-ElMaven-UI#5-eic-window>`_.

|LLCMS33|

|LLCMS34|

When the user selects the first group they would be asked if they would like to auto-save the state of the application. This feature allows the user to go back to his curated peaks if they so wishes in future. 

|LLCMS35|

The user can then use dropdown arrow on bookmarked group to mark all its isotopologues as good or bad. 

|LLCMS36|

After marking all the groups for a compound, the user can scroll down to the next compound and decide on the basis of shown EIC if the group should be marked for curation. 

|LLCMS37|

Double clicking on any peak (solid coloured circle) moves the RT line along the group. And the group moves to the bookmark table. 

|LLCMS38|

Qualifying peaks as good or bad is explained in the next section.

Guidelines for Peak Picking
---------------------------

* A peak’s width and shape are two very crucial things to look at while classifying a peak as good or bad. The peak’s shape should have a Gaussian distribution and width should not be spread across a wide range of RT. 

|LLCMS39|

* Peak Intensities for a group are plotted as bar plots for all the samples. These bar plots have heights relative to the other samples.Thus, for a good peak the intensities should be high. 

|LLCMS40|

* Intensity Barplot heights should be higher for all the samples than Blank samples, as shown above. We use intensities of Blank samples to set our group baseline. Blank intensities are used to calibrate intensity values across zero concentration.

* A good peak should have standards with varying intensity in a particular fashion (increasing or decreasing).

* Quality Control (QC) samples give us information about the quality of the data, i.e., it assesses reproducibility and software performance. Samples whose intensities and concentrations are already known are used as QCs to determine if the instrument are working as expected. Values (and scales) can be calibrated using QCs.

* If peak groups of a particular metabolite are separated apart (not aligned well) then we should use stringent alignment parameters to overcome this problem.

* For a particular metabolite, let’s say if it has n number of groups, then the group which is much closer to the above guidelines should be selected as good peak. Multiple groups can also be selected in case of ambiguity (if RT information is not provided).

**A good peak would look similar to the following peaks:**

|LLCMS41|

|LLCMS42|

|LLCMS43|

   * Gaussian shape
   * Perfect grouping, narrow RT
   * Good sample intensities
   * Low blank intensities
   * QCs look good
   * An observable trend in intensity bars of standards, as well as samples.

**Some examples of bad peaks are given below:**

   * The peaks have a good Gaussian shape. But the blank intensity bars are high. All the sample intensity bars are shorter or roughly equal to the blank intensities, implying the peaks are most likely noise.

|LLCMS44|

   * The intensity levels are low relatively. The peaks are spread over a long range of RTs. They have poor shape, poor grouping and lie close to noise. If the signal to noise ratio was improved, this peak would probably not be detected. 

|LLCMS45|

   * In the following image, many sample intensities are missing from the intensities bar plots. Peaks do not have a Gaussian shape, nor good grouping. These peaks are probably noise which have been wrongly annotated. 

|LLCMS46|

   * This is a noisy group. There are no discrete peaks visible in the image. The X-axis is crowded with noise. The peak shape is sharp, triangular, or line-like; not Gaussian. The intensity levels are high, but so are noise levels. 

|LLCMS47|

 More examples of noisy peaks:

|LLCMS48|

|LLCMS49|

   * The peaks don’t have a Gaussian shape, and are also noisy. The intensity values are very low. 

   * For low intensity groups like this, the peak characteristics can be determined by zooming in. 

|LLCMS50|

 The mouse can be used to select the area of the peak as shown below 

|LLCMS51|

 On zooming, it will be easy to make a decision on peak quality 

|LLCMS52|

NOTE: The user can mark any ambiguous peaks as good, and can review all such peaks later in the process.

Export
------

There are multiple export options available for storing marked peak data. Users can either generate a PDF report to save the EIC for every metabolite, export data for a particular group in .csv format, or export the EICs to a Json file as shown below. 

|LLCMS53|

Users can select *All, Good, Bad or Selected* peaks to export. 

|LLCMS54|

The *Export Groups to CSV* option |export to csv| lets the user save the 'good'/'bad' labels along with the peak table. Users also have the option to filter out rows that have a certain label while exporting the table.

*Generate PDF Report* option |generate pdf| saves all EICs with their corresponding bar plots in a PDF file.

*Export EICs to Json* option |export to json| exports all EICs to a Json file.

Another option is to export the peak data in .mzroll format that can be directly loaded into El-MAVEN by clicking on the Load *Samples|Projects|Peaks* option in the File menu. For this, go to the File option in the menu bar, and click on '*Save Project*'.

|LLCMS55|
