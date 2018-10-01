El-MAVEN User Interface
=======================

|

Following is the general workflow involved in El-MAVEN:

.. image:: /image/Workflow.png

*El-MAVEN Workflow. Peak detection, alignment, grouping and scoring are done multiple times for best results. Data from different cohorts can be compared using visualisation tools and easily exported to other formats.*

.. All widget icons are referenced here

.. |options| image:: /image/Widget_1.png
.. |load samples| image:: /image/Widget_2.png
.. |show samples| image:: /image/Widget_3.png
.. |load project| image:: /image/Widget_4.png
.. |load meta| image:: /image/Widget_5.png
.. |save project as| image:: /image/Widget_6.png
.. |change sample color| image:: /image/Widget_7.png
.. |remove samples| image:: /image/Widget_8.png
.. |show hide selected samples| image:: /image/Widget_9.png
.. |mark sample as blank| image:: /image/Widget_10.png
.. |zoom out| image:: /image/Widget_11.png
.. |copy group info to clipboard| image:: /image/Widget_12.png
.. |bookmark as good group| image:: /image/Widget_13.png
.. |bookmark as bad group| image:: /image/Widget_14.png
.. |history back| image:: /image/Widget_15.png
.. |history forward| image:: /image/Widget_16.png
.. |save eic image to pdf| image:: /image/Widget_17.png
.. |copy eic image to clipboard| image:: /image/Widget_18.png
.. |print eic| image:: /image/Widget_19.png
.. |auto zoom| image:: /image/Widget_20.png
.. |show tic's| image:: /image/Widget_21.png
.. |show bar plot| image:: /image/Widget_22.png
.. |show isotope plot| image:: /image/Widget_23.png
.. |show box plot| image:: /image/Widget_24.png
.. |align| image:: /image/Widget_25.png
.. |show alignment visualisation| image:: /image/Widget_26.png
.. |show alignment visualisation for all groups| image:: /image/Widget_27.png
.. |show alignment polynomial fit| image:: /image/Widget_28.png
.. |peaks| image:: /image/Widget_29.png
.. |switch between group and peak views| image:: /image/Widget_30.png
.. |train neural net| image:: /image/Widget_31.png
.. |scatter plot| image:: /image/Widget_32.png
.. |compare samples| image:: /image/Widget_33.png
.. |volcano plot| image:: /image/Widget_34.png
.. |generate pdf| image:: /image/Widget_35.png
.. |export to csv| image:: /image/Widget_36.png
.. |export to json| image:: /image/Widget_37.png

.. |UI01| image:: /image/UI_1.png
.. |UI02| image:: /image/UI_2.png
.. |UI03| image:: /image/UI_3.png
.. |UI04| image:: /image/UI_4.png
.. |UI05| image:: /image/UI_5.png
.. |UI06| image:: /image/UI_6.png
.. |UI07| image:: /image/UI_7.png
.. |UI08| image:: /image/UI_8.png
.. |UI09| image:: /image/UI_9.png
.. |UI10| image:: /image/UI_10.png
.. |UI11| image:: /image/UI_11.png
.. |UI12| image:: /image/UI_12.png
.. |UI13| image:: /image/UI_13.png
.. |UI14| image:: /image/UI_14.png
.. |UI15| image:: /image/UI_15.png
.. |UI16| image:: /image/UI_16.png
.. |UI17| image:: /image/UI_17.png
.. |UI18| image:: /image/UI_18.png
.. |UI19| image:: /image/UI_19.png
.. |UI20| image:: /image/UI_20.png
.. |UI21| image:: /image/UI_21.png
.. |UI22| image:: /image/UI_22.png
.. |UI23| image:: /image/UI_23.png
.. |UI24| image:: /image/UI_24.png
.. |UI25| image:: /image/UI_25.png
.. |UI26| image:: /image/UI_26.png
.. |UI27| image:: /image/UI_27.png
.. |UI28| image:: /image/UI_28.png
.. |UI29| image:: /image/UI_29.png
.. |UI30| image:: /image/UI_30.png
.. |UI31| image:: /image/UI_31.png
.. |UI32| image:: /image/UI_32.png
.. |UI33| image:: /image/UI_33.png
.. |UI34| image:: /image/UI_34.png
.. |UI35| image:: /image/UI_35.png
.. |UI36| image:: /image/UI_36.png
.. |UI37| image:: /image/UI_37.png
.. |UI38| image:: /image/UI_38.png


El-MAVEN User Interface
-----------------------

|UI01|

Global Settings
---------------

Global Settings can be changed from the Options dialog |options|.

**Instrumentation**

|UI02|

   * *Polarity/Ionization mode*: Polarity information is required for m/z calculation. User can set the polarity of the metabolites in their experiment from the drop-down list or set it to Auto-detect.

   * *Ionization type*: Ionization methods can affect m/z calculation. Drop-down provides a list of the most popular ionization types.

   * *Q1 accuracy*: This is the mass resolution in amu of the first quadrapole.

   * *Q3 accuracy*: This is the mass resolution in amu of the third quadrapole.

   * *Filterline*: The drop-down lists different mass ranges and allows the user to process the data in these ranges separately with different parameters. Primarily used for polarity-switching experiments.

**File Import**

|UI03|

   * *Centroid Scans*: Centroid acquisition is an acquisition method where only centroid m/z and intensity are stored. Centroid m/z is calculated based on the average m/z value weighted by the intensity and m/z values are assigned based on a calibration file. User may leave the box unchecked if they have centroid data. Check the box if centroiding has to be done in El-MAVEN.

   * *Scan Filter Polarity*: User may choose to import scans based on the polarity of ions in the scan. Especially helpful in polarity-switching experiments.

   * *Scan Filter MS Level*: User may choose to import only MS1 or MS2 scans. This feature can be used with MS/MS data.

   * *Scan Filter Minimum Intensity*: Set a minimum threshold for reading in intensity values.

   * *Scan Filter Intensity Minimum Quantile Cutoff*: Scans with x% of their intensity values below the threshold will be filtered out during import.

   * *Enable Multiprocessing*: In order to reduce sample load time, El-MAVEN uses multiprocessing. This behavior can be changed by the user.

**Peak Detection** 

|UI04|

**Peak Grouping and Grouping Settings**

   * *EIC Smoothing Algorithm*: Smoothing of data points helps in increasing the signal/noise ratio. There are three algorithms provided for EIC smoothing: 

      (a) *Savitzky-Golay*: It preserves the original shape and features of the signal better than most other filters `(Learn more) <https://www.researchgate.net/publication/270819321_Smoothing_and_Differentiation_of_Data_by_Simplified_Least_Squares_Procedures>`_. 

      (b) *Gaussian*: It reduces noise by averaging over the neighborhood with the central pixel having higher weight but successfully preserves sharp edges. `(Learn more) <https://people.csail.mit.edu/asolar/papers/pldi276-chaudhuri.pdf>`_. 

      (c) *Moving Average*: It takes the simple average of all points over time. Signal behavior is not natural. Least preferred method for smoothing `(Learn more) <https://www.wavemetrics.com/products/igorpro/dataanalysis/signalprocessing/smoothing>`_.

   * *EIC Smoothing Window*: Number of scans used for fitting in the smoothing algorithm can be adjusted here.

   * *Maximum Retention Time Difference Between Peaks*: Set a limit to RT difference between peaks in a group. Increase the value if alignment fails to center peaks satisfactorily.

**Baseline Calculation**

   * *Drop top x% intensities from chromatogram*: Set the baseline for every peak. Baseline is obtained once x% of the highest intensities in a peak are removed from consideration. Baseline should be set high when there is more noise in the data.

   * *Baseline Smoothing*: Number of scans used for fitting in the smoothing algorithm can be adjusted here.

**Peak Filtering**

|UI05|

   * *Minimum Signal Baseline Difference*: Set the minimum difference between intensity and baseline to detect any signal as a valid peak.

Isotope Detection
-----------------

.. **Isotope Detection**

|UI06|

**Are Samples Labeled?**

   * *Bookmarks, peak detection, file export*: Select the labeled atoms that should be used in bookmarking, peak detection and export. D2: Deuterium, C13: Labeled carbon, N15: Labeled nitrogen, S34: Labeled sulphur.

   * *Isotopic barplot*: Select the labeled atoms that should be displayed in the isotopic barplot. D2: Deuterium, C13: Labeled carbon, N15: Labeled nitrogen, S34: Labeled sulphur.

   * *Isotopic widget*: Select the labeled atoms that should be displayed in the isotopic widget. D2: Deuterium, C13: Labeled carbon, N15: Labeled nitrogen, S34: Labeled sulphur.

   * *Number of M+n isotopes*: Set the maximum number of labeled atoms per ion in the experiment.

   * *Abundance Threshold*: Set the minimum threshold for isotopic abundance. Isotopic abundance is the ratio of intensity of isotopic peak over the parent peak.

**Filter Isotopic Peaks based on these criteria**

   * *Minimum Isotope-Parent Correlation*: Set the minimum threshold for isotope-parent peak correlation. This correlation is a measure of how often they appear together.

   * *Isotope is within [X] scans of parent*: Set the maximum scan difference between isotopic and parent peaks. This is a measure of how closely they appear together on the RT scale.

   * *Maximum % Error to Natural Abundance*: Set the maximum natural abundance error expected. Natural abundance of an isotope is the expected ratio of amount of isotope over the amount of parent molecule in nature. Error is the difference between observed and natural abundance as a fraction of natural abundance.

   * *Correct for Natural C13 Isotope Abundance*: Check the box to correct for natural C13 abundance.

**EIC (XIC) [BETA]**

|UI07|

   * *EIC Type*: Select a method to merge EICs over m/z. There are two options: 

      (a) *MAX*: Merged EIC is created by taking the maximum intensity across the m/z window at a particular scan.

      (b) *SUM*: Merged EIC is created by taking the sum average of intensities across the m/z window at a particular scan.

**Peak Grouping**

|UI08|

   * *Peak Grouping Score*: Peaks are assigned a grouping score to determine whether they should be grouped together. There are two formulas for grouping score calculation: 

      (a) score = 1.0/((distX * A) + 0.01)/((distY * B) + 0.01) * (C * overlap) 

      (b) score = 1.0/((distX * A) + 0.01)/((distY * B) + 0.01)

   The score depends on the following 3 parameters and their weights:

      * *RT difference or DistX*: Difference in RT between the peaks under comparison. Closer peaks are assigned a higher score.

      * *Intensity difference or DistY*: Difference in intensity between peaks under comparison. Smaller difference accounts for a higher score.

      * *Overlap*: Fraction of RT overlap between the peaks under comparison. Greater overlap accounts for a higher score.

         * Uncheck *Consider Overlap* to calculate grouping score without overlap.

         * Sliders are provided to adjust the weights attached to each of the three parameters.

**Group Rank**

|UI09|

   * *Group Rank Formula*: Group rank is one of the parameters for group filtering. There are two formulas below for group rank calculation:

      (a) Group Rank = ((1.1 - Q) ^ A) * (1/(log(I + 1)) ^ B)

      (b) Group Rank = ((1.1 - Q) ^ A) * (1/(log(I + 1)) ^ B) * (dRT) ^ (2 * C)

   The score depends on the following 3 parameters and their respective weights A, B and C:

      * *Q or Group Quality*: Maximum peak quality of a group. Peaks are assigned a quality score by a machine learning algorithm in El-MAVEN. Better quality leads to a higher rank.

      * *I or Group Intensity*: Maximum intensity of a group. Better intensity leads to a higher rank.

      * *dRT or RT difference*: Difference between expected RT and group mean RT.

         * *Consider Retention Time*: Check the box to use formula (b) for group rank calculation. Formula (a) is used by default.

         * *Quality Weight*: Adjust slider to set weight for group quality in group rank calculation.

         * *Intensity Weight*: Adjust slider to set weight for group intensity in group rank calculation.

         * *dRT Weight*: Adjust slider to set weight for RT difference in group rank calculation. The slider is disabled if Consider Retention Time is unchecked.

Sample Upload
-------------

**Load Sample Files**

Load |load samples| sample files into El-MAVEN and click on *Show Samples Widget* |show samples| on the widget bar to show/hide the project space. Blanks will not show up in the sample list if the file names start with 'blan' or 'blank'. 

|UI10|

Load sample files into El-MAVEN and click on Show Samples Widget on the widget bar to show/hide the project space. Blanks will not show up in the sample list if the file names start with 'blan' or 'blank'.

There are three columns in the project space:

   * *Sample*: This column has the sample name and the random color assigned to the sample. Double-click the sample name to change the color.

   * *Set*: The set column holds the cohort name for every sample. Example: subjects and controls.

   * *Scaling*: This column holds the normalization constant for every sample. For example, all intensities in a sample will be halved if the constant is two. This is done to normalize data if sample volumes are different.

**Sample Space Menu**

   * |load project| *Load Project*: Sample files can be loaded here.

   * |load meta| *Load Meta*: User may upload a meta file with sample and set names in a comma separated file (.csv) or double-click to enter text. Meta file template is shown below:

|UI11|

   * |save project as| *Save Project as*: Current state of El-MAVEN can be saved in a .mzroll file for future use. All the settings, EICs and peak tables are stored in the file and may be reloaded at any point in the future.

   * |change sample color| *Change Sample Color*: Sample colors can be changed by either clicking on this menu button or double-clicking the sample name. User can pick a color of their choice to represent their samples.

   * |remove samples| *Remove Samples*: Apart from deselecting samples, user also has the option to remove samples from the project space. The sample files will not be deleted, only removed from El-MAVEN's project space.

   * |show hide selected samples| *Show/Hide Selected Samples*: Samples can be selected/deselected in batches. This is especially helpful when dealing with large datasets as the EIC window gets increasingly noisy with more samples.

   * |mark sample as blank| *Mark Sample as Blank*: User can select sample files and set them as blanks as depicted below. Clicking the button again will reverse the move.

|UI12|

Compound Database
-----------------

**Load Reference File**

.. |UI13|

**Shubhra**

Reference file contains a list of metabolites and their properties that are used for peak detection. This is a comma separated (.csv) or tab separated (.tab) file with compound name, id, formula, mass, expected retention time and category. It is preferable but not necessary to have retention time information in the reference file but either mass or formula is required. In case both mass and formula are provided, formula will be used to calculate the m/z. Click on the *Show Compounds Widget* on the widget toolbar to view the compounds panel. User may upload a new reference file or use any of the default files loaded on start-up.

|UI14|

EIC
---

.. **EIC Window**

|UI15|

An Extracted Ion Chromatogram is a graph of Intensity vs. RT for a certain m/z range. EIC window displays the EIC for every group/compound selected or m/z range provided. The group name and/or the m/z range is displayed at the top. Following are the different menu options on top of the EIC window:

   * |zoom out| *Zoom Out*: The EIC is initially zoomed-in to display the region near the expected RT of a group. This button will zoom out and display the whole RT range for the selected m/z range. User may zoom into a region by right dragging the mouse over it. Left-dragging will zoom-out.

   * |copy group info to clipboard| *Copy Group Information to Clipboard*: On clicking this button, group information is copied to the clipboard with every row representing a different sample.

   * |bookmark as good group| *Bookmark as Good Group*: User can manually curate a group as 'good' and store it in the bookmark table using this button. (Manual curation of groups has been covered `here <https://github.com/ElucidataInc/ElMaven/wiki/Introduction-to-ElMaven-UI>`_) 

|UI16|

   * |bookmark as bad group| *Bookmark as Bad Group*: User can manually curate a group as 'bad' and store it in the bookmark table using this button. (Manual curation of groups has been covered `here <https://github.com/ElucidataInc/ElMaven/wiki/Introduction-to-ElMaven-UI>`_) 

|UI17|

   * |history back| *History Back*: EIC window display history is recorded. Clicking this button will display the previous state of the window.

   * |history forward| *History Forward*: EIC window display history is recorded. Clicking this button will display the next state of the window if available. 

   * |save eic image to pdf| *Save EIC Image to PDF File*: Saves the current EIC window display in a PDF file.

   * |copy eic image to clipboard| *Copy EIC Image to Clipboard*: Current EIC window display is copied to clipboard.

   * |print eic| *Print EIC*: Current EIC window display can be directly printed out.

   * |auto zoom| *Auto Zoom*: Auto Zoom is selected by default. It zooms-in and centers the EIC to the expected retention time. The expected retention time is depicted as a dashed red line. 

|UI18|

   * |show tic's| *Show TICs*: Displays the Total Ion Current. TIC is the sum of all intensities in a scan.

   * |show bar plot| *Show Bar Plot*: Displays the peak intensity for a group in every sample. Intensity can be calculated by various methods known as quantitation types in El-MAVEN. User can change the quantitation type from the drop-down list on the top right or choose to display other parameters like retention time and peak quality. 

|UI19|

   * |show isotope plot| *Show Isotope Plot*: Displays the isotope plot for a group. Each bar in the plot represents the relative percentage of different isotopic species for the selected group in a sample.

|UI20|

   * |show box plot| *Show Box Plot*: Displays the boxplot for a group. The box plot shows the spread of intensities in the group and where each peak lies in relation to the median. Median of the intensities is the vertical line between the boxes.

Apart from the top menu, there are other features in the EIC window. Right-click anywhere in the window and go to Options. 

|UI21|

Some of the important options are:

   * *Show Peaks*: Peaks are marked by the colored circles that represent the quality score of the peak. Bigger the circle, better the peak quality. This option allows the user to show/hide the peak quality score.

   * *Group Peaks Automatically*: Peak grouping happens automatically when grouping parameters are changed. To prevent automatic grouping, user can uncheck this option.

   * *Show Baseline*: Hide/Show the baseline for every peak. (Read more about baseline `here <https://github.com/ElucidataInc/ElMaven/wiki/Introduction-to-ElMaven-UI>`_).

   * *Show Merged EIC*: Merged EIC is the sum average of EICs across samples. It smoothens the data and helps in grouping peaks.

   * *Show EIC as Lines*: In case of large number of samples, it can get difficult to look at short individual peaks as they are obscured by larger peaks. Showing EIC as lines cleans up the display window and allows the user to look at small peaks.

Mass Spectra
------------

.. **Mass Spectra**

Mass Spectra Widget displays each peak, its mass, and intensity for a scan. As the widget shows all detected masses in a scan, the ppm window for the EIC and consequently grouping can be adjusted accordingly. This feature is especially useful for MS/MS data and isotopic detection. 

|UI22|

Alignment
---------

.. **Alignment** 

Prolonged use of the LC column can lead to a drift in retention time across samples. Alignment shifts the peak RTs in every sample to correct for this drift and brings the peaks closer to median RT of the group.

Click on the *Align* button |align| and adjust the settings.

|UI23|

The first panel in Alignment options is for Group Selection criteria. 'Group' here refers to a set of peaks across samples that is annotated as a particular ion.

   * *Group must contain at least [X] good peaks*: The value of x is set to filter out groups that do not have at least x good peaks from the alignment process. As there is only one peak per sample for a group, this value should not exceed the number of samples in your project. This option allows the user to discard groups with very few good peaks under the assumption that those could be stray peaks.

   * *Limit total number of groups in alignment to*: User can change the number of groups being used for alignment in case there are too many groups detected after the peak detection process.
   
   * *Peak Grouping Window*: This value controls the number of scans required to get the most accurate peaks. Enter a high number if the reproducibility is low to ensure best results.

The next panel is for *Peak Selection* settings:

   * *Minimum Peak Intensity*: The intensity value can be adjusted to only look at high or low intensity peaks in case you have prior information about the concentration of metabolite you are looking for.

   * *Minimum peak S/N ratio*: This is the minimum signal to noise ratio of your experiment. Increase the value if you see too much noise in the data.
    
   * *Minimum Peak Width*: This is the least number of scans to be considered to evaluate the width of any peak.

   * *Peak Detection Algorithm*: Select the *Compound Database Search* algorithm and then choose an appropriate database from the next drop-down menu.

The *Alignment Algorithm* panel provides the following options:

   * *Alignment Algorithm*: There are two alignment algorithms available in El-MAVEN: Poly fit and Loess fit. Loess fit has been released as a beta feature for now.
    
   * *Maximum number of Iterations*: This parameter is only required for Poly fit algorithm. Enter the number of times El-MAVEN should fit a model to the data in order to align it.
    
   * *Polynomial Degree*: This is the degree of the non-linear model we are trying to fit. Recommended settings are entered by default.

Click on *Align* at the bottom.

**Alignment Visualizations**

El-MAVEN provides three visualizations for alignment analysis.

   * *Show Alignment Visualization*: Click on |show alignment visualisation| in the widget bar to open this visualization. Click on any grouped peak to look at its delta Rt vs Rt graph as shown.

|UI24|

   * *Show Alignment Visualization (For All Groups)*: Click on |show alignment visualisation for all groups| in the widget bar for this visualization.

|UI25|

   * *Show Alignment Polynomial Fit*: Click on |show alignment polynomial fit| in the widget bar for Poly fit alignment. 

|UI26|

The above graphs give a clear indication of how aligned/misaligned the peaks are. User may run alignment again with different parameters if required (or with a different algorithm).

Peak Detection
--------------

.. **Peak Detection**

Peak detection algorithm pulls the EICs, detects peaks and performs grouping and filtering based on parameters controlled by the user. The algorithm groups identical peaks across samples and calculates the quality score by a machine learning algorithm. Click on the *Peaks* icon |peaks| on the top to open the settings dialog.

There are 3 tabs for setting Peak Detection parameters:

**1. Feature Detection Selection**

|UI27|

The Feature Detection Selection panel has the following parameters:

   * *Automated Feature Detection*: This is one of the two strategies for finding peaks. Automated search creates thousands of mass slices across the whole m/z and retention time space to find all peaks present in the sample. This strategy is used when looking for new/unknown metabolites in the samples.

      * *Mass Domain Resolution*: This value defines the m/z range of every mass slice in parts per million

      * *Time Domain Resolution*: This value defines the scan range (or retention time range) of every mass slice

      * *Limit Mass Range*: User can limit the automated search to a range of m/z according to their requirements

      * *Limit Time Range*: User can limit the automated search to a retention time range according to their requirements

   * *Compound Database Search*: Database search is used to search for compounds listed in the reference file using their m/z information. For better accuracy, retention time information can also be used for the search.

      * *Select Database*: Select a desired reference file for the search from the drop-down list
   
      * *EIC Extraction Window*: Provide a ppm buffer range to all compound masses. A larger window is useful for processing low resolution data. The window should be smaller for high resolution data to reduce noise.

      * *Match Retention Time*: Enable/disable use of retention time information along with m/z to perform database search. Compounds can have different retention times in every experiment, therefore this option should only be checked if the reference file is specific to the experiment and the sampled used. Enter the time buffer in the accompanying box.

      * *Limit Number of Reported Groups Per Compound*: Multiple groups can be annotated as the same compound, especially when retention time is not taken into consideration for the search. User can set the value to only report X best groups according to their rank. The group rank formula will be discussed later in the tutorial.

   * *Match Fragmentation*: This panel is activated for MS/MS data

   * *Report Isotopic Peaks*: Check this box to find and report isotopic peaks for labeled data.

   To perform peak detection with reference, check the box next to *Compound Database Search* and choose the appropriate database. The *EIC Extraction Window* should be set according to the instrument's resolving power. Select the *Match Retention Time* option if you wish to search for compounds using both the m/z ratio and rt value given in the database. In case of a generic database, searching by retention time is not recommended.

**2. Group Filtering**

|UI28|

After grouping is done, groups that do not fulfill the criteria shown above are filtered out.

   * *Minimum Peak Intensity*: Groups with no peak intensities above this threshold are filtered out. The drop-down list beside the input box defines how intensity is calculated. Different methods of intensity calculation are known as quantitation types. The slider below can be adjusted to change the minimum percentage of peaks per group that must pass the threshold (minimum number of peaks is 1).

   * *Minimum Quality*: Quality of peaks is calculated using a machine learning algorithm. Groups with no peak qualities above this threshold are filtered out. The slider below can be adjusted to change the minimum percentage of peaks per group that must pass the threshold (minimum number of peaks is 1).

   * *Minimum Signal/Blank Ratio*: Signal/Blank ratio is the ratio of peak intensity over maximum intensity observed in blanks. Groups with no peaks above this threshold are filtered out. The slider can be adjusted to change the minimum percentage of peaks per group that must pass the threshold (minimum number of peaks is 1). This helps in filtering out peaks that are also present in blanks.

   * *Minimum Signal/Baseline Ratio*: Signal/Baseline ratio is the ratio of peak intensity over baseline value for that peak. Baseline calculation is used to filter out noise in the signal and will be discussed later in the tutorial. The slider can be adjusted to change the minimum percentage of peaks per group that must pass the threshold (minimum number of peaks is 1).

   * *Minimum Peak Width*: Peak width is equal to the number of scans that a peak is spread over. Groups with no peak widths above this threshold are filtered out. Spurious signals can be filtered out using this option.

   * *Peak Classifier Model File*: This is the default model that is used by the machine learning algorithm for classifying peaks according to their quality.

Change the settings according to the data and click on *Find Peaks* to run peak detection. For beginners, performing peak detection with default values at first is recommended. User may then adjust the settings depending on their results.

**3. Method Summary**

|UI29|

Peak Table
----------

.. **Peak Table**

|UI30|

Groups information obtained after Peak Detection is stored and displayed in the form of a Peak Table with a row representing a group and its corresponding features in columns. User can show/hide the peak table by clicking on |peaks| the widget bar.

**Peak Table Features**

Following are the different features/columns in a peak table:

   * *#*: is the serial number for a group
    
   * *ID*: Group ID is assigned according to the search mode used during peak detection. In case of Automated search, groups are named by their m/z and RT values separated by '@' sign. For example, ID for a group with 230.2 m/z and 1.89 RT will be given as '230.2@1.89'. In case of Database search, groups are annotated as a compound from the reference file. For example, 'malate'.

   * *Observed m/z*: is the median m/z of the group.

   * *Expected m/z*: is the m/z value provided in the reference file for the compound represented by the group. This field is populated only in case of Database search.

   * *rt*: is the median RT of the group.

   * *rt delta*: is the difference between expected RT from the reference file and the observed RT. This field is set to -1 in case of Automated Search.

   * *#peaks*: is the number of peaks in the group.

   * *#good*: is the number of good peaks in a group. A good peak is defined as one with its quality score above the defined threshold in Peak Detection dialog.

   * *Max Width*: is the maximum peak width in a group. Peak width is defined as the number of scans over which a peak is spread.

   * *Max AreaTop*: is the maximum peak AreaTop intensity in a group. AreaTop is one of the quantitation types used to represent peak intensity in El-MAVEN. Read more about the different quantitation types `here <https://github.com/ElucidataInc/ElMaven/wiki/Introduction-to-ElMaven-UI>`_.

   * *Max S/N*: is the maximum peak signal/noise ratio in a group.

   * *Max Quality*: is the maximum peak quality score in a group.

   * *Rank*: is the group rank. The formula and parameters involved have been explained `here <https://github.com/ElucidataInc/ElMaven/wiki/Introduction-to-ElMaven-UI>`_.

**Peak Table Menu Bar**

|UI31|

Multiple groups can be annotated as the same compound especially when retention time information is not used during Database search. The peak table provides options for filtering, comparing or exporting data from the table. Following are the different menu options available in the peak table:

   * |switch between group and peak views| *Switch between group and peak views*: Switching to Peak view displays only Peak information. This includes group number, group ID, Expected m/z, Observed m/z, rt and intensity of all peaks in the group with sample names as the respective column headers. Peak intensity cells are colored based on their relative values in a group. Highest intensity value has the lightest color and vice-versa. 

|UI32|

   * |bookmark as good group| *Mark Group as Good*: Used to manually curate selected peaks as 'good'. User can also press 'G' on their keyboard for the same. Manual curation has been described `here <https://github.com/ElucidataInc/ElMaven/wiki/Introduction-to-ElMaven-UI>`_. 

|UI33|

   * |bookmark as bad group| *Mark Group as Bad*: Used to manually reject peaks by marking them as 'bad'. User can also press 'B' on their keyboard for the same. Manual curation has been described `here <https://github.com/ElucidataInc/ElMaven/wiki/Introduction-to-ElMaven-UI>`_. 

|UI34|

   * |train neural net| *Train Neural Net*: Used to retrain the neural net algorithm to recognize good/bad peaks. User manually curates 100 peaks to train the algorithm.

   * |remove samples| *Delete Group*: Deletes the selected group(s) from the peak table.

   * |scatter plot| *Show Scatter Plot*: Opens the Scatter plot widget used to compare different cohorts via Scatter plot and Volcano plot.

The remaining are export options and will be detailed in the `Export <https://github.com/ElucidataInc/ElMaven/wiki/Introduction-to-ElMaven-UI#export-options>`_ section.

Statistics
----------

.. **Statistics Module**

El-MAVEN comes equipped with a statistics module for comparing data across different cohorts. User can set the sample cohorts either by editing the Set column in the Sample space, or upload a meta file with sample and cohort names as detailed above under the "Sample Space Menu" section.

The statistics module can be accessed through the Peak Table menu.

|UI35| 

   * *Set1/Set2*: Select two cohorts to be compared

   * *Min Log2 Fold Difference*: Fold difference is a measure of how much the intensity of a group changes from one cohort to another. User can set the minimum threshold for this value in log(base 2) format.

   * *Min Intensity*: Groups with all peak intensities less than this value will be filtered out from the comparison process.

   * *p value*: A t-test is performed to find if the intensity distributions of the two selected cohorts are significantly different from each other. This test returns a p-value indicating how significantly different a group behaves between the two cohorts. A lower p-value shows higher significance.

   * *Set Missing Values*: User can set the default intensity value to be used in case the group is missing from a sample.

   * *Min. Good Sample*: Groups should have a minimum number of good peaks (based on peak quality score) to be considered for comparison.

   * *FDR Correction*: False discovery rate is the expected proportion of false positives in a test. There are a number of ways to correct for false positives. (`Read More <http://nebc.nerc.ac.uk/courses/GeneSpring/GS_Mar2006/Multiple%20testing%20corrections.pdf>`_)

   * *Compare Sets*: Click to get comparison results.

*Compare Sets* opens the scatter plot by default

|UI36|

   * |zoom out| *Zoom Out*: Zooms out of the plot.

   * |compare samples| *Compare Samples*: Opens the compare samples dialog again to adjust settings.

   * |scatter plot| *Scatter Plot*: The axes represent the average peak intensity (Peak Height) for sample 1 and 2 respectively. Each bubble is a group. The bubble size represents fold change between the samples. The significance (or p-value) of the fold change is represented by the bubble color. Red and blue signify higher intensity in sample 1 and 2 respectively. Opaqueness of the bubble represents the significance (or inverse of p-value) of the fold change between cohorts.

   * |volcano plot| *Volcano Plot*: The axes represent Fold change and significance of fold change respectively. Red and blue bubbles represent positive and negative fold change.

   * |remove samples| *Delete*: Delete a data point from the graph.

   * |peaks| *Scatter Plot Table*: A separate Peak Table is created with all filtered groups being used for statistical analysis. User can also export these in a CSV or JSON.

Export
------

.. **Export options**

User can either save the state of the project or export only relevant data from the peak table. These are the different export options available in El-MAVEN:

|UI37|

   * *Save Project as*: This option is available in the File menu. It saves all peak tables and current settings in a .mzroll file. On loading the .mzroll file, all sample files are uploaded and the peak tables and EIC are available. If the user wishes to save only certain Peak Tables, they can click on |save project as| at the top of the Peak Table(s). This will only store that specific peak table instead of all.

   * *Generate PDF Report*: This option is available on |generate pdf| at the top of the Peak Table. It saves all EICs with their corresponding bar plots in a PDF file.

   * *Export Groups to SpreadSheet (.csv)*: This option is available on top of the Peak Table |export to csv|. You can choose to export the whole table or a subset of the data. There are 4 possible selections: export only selected groups, export all groups, export only good groups or export only bad groups. The data is stored in a comma separated file. 

|UI38|

   * *Export EICs to Json*: This option is available on top of the Peak Table |export to json|. It exports all EICs to a Json file.
