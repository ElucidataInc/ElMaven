El-MAVEN Command Line Interface
===============================

|

The peakdetector executable in El-MAVEN can load sample files, perform peak detection and save the resulting peaks in a CSV or mzroll file. It reads a configuration file with user-set parameters and paths to input and output files.

This executable is not part of installer versions of El-MAVEN. Please clone the repository and build `El-MAVEN <https://github.com/ElucidataInc/ElMaven/blob/develop/README.md#compilation>`_ to use this feature.


Create a default configuration file
-----------------------------------

Users need an XML file to specify their configuration and specify directory paths. The XML template can be generated using a simple command and modified according to user requirements.

   * Open the terminal

   * Move to your local El-MAVEN folder: 
   
   ::
   
   $ cd user@pc:~/ElMaven/ElMaven

   * Execute the following command

      * For Windows and Ubuntu: 

      ::

      $ ./bin/peakdetector.exe --defaultxml
      
      * For MacOS: 

      ::

      $ ./bin/peakdetector.app/Contents/MacOS/peakdetector --xml config.xml

   * A default XML will be created in your current folder

Peak detector parameters
------------------------

CLI provides a limited number of parameters for user-modification. Default values are used for all other parameters used in El-MAVEN GUI.

The template XML contains three sets of parameters:

**1. OptionalDialogArguments: Global parameters**

   * Users need to know the ionization mode and accepted values are 0 (neutral), -1 (negative) and +1 (positive): ``ionizationMode type="int" value="-1"``  

   * Ionization charge. This is the number of positive or negative charge added to every ion. Accepted values are positive integers: ``charge type="int" value="1"`` 

   * Mass accuracy. Set the +/- ppm range for m/z values: ``compoundPPMWindow type="float" value="5"`` 

**2. PeaksDialogArguments: Peak detection parameters**

   * Peak Detection Algorithm. Accepted values: "0" for Compound Database Search, "1" (or higher) for Automated Feature Detection: ``processAllSlices type="int" value="0"``

   Following are the arguments used in Automated Peak Detection and their GUI equivalents. These can be found in the 'Feature Detection Selection' tab of Peaks Dialog:

   * 'Limit m/z Range' minimum: ``minScanMz type="float" value="0"`` 

   * 'Limit m/z Range' maximum: ``maxScanMz type="float" value="1e+009"`` 

   * 'Limit Time Range' minimum: ``minScanRt type="float" value="0"`` 

   * 'Limit Time Range' maximum: ``maxScanRt type="float" value="1e+009"`` 

   * 'Limit Intensity Range' minimum: ``minScanIntensity type="float" value="50000"`` 

   * 'Limit Intensity Range' maximum: ``maxScanIntensity type="float" value="1e+010"`` 

   * 'Mass Domain Resolution': ``ppmMerge type="float" value="20"`` 

   Following are the arguments used in Compound database search and their GUI equivalents. These can be found in the 'Feature Detection Selection' tab of Peaks Dialog:

   * 'Select Database'. Enter full path to the database file: ``Db type="string" value="0"`` 

   * 'Match Retention Time (+/-)'. Value of '0' will set the flag as false. Positive integer value will set the allowed deviation in minutes: ``matchRtFlag type="int" value="0"`` 

   * 'Limit Number of Reported Groups per Compound':  ``eicMaxGroups type="int" value="0"`` 

   Isotope detection. Following are the settings used to pull isotopes in case of database search and their GUI equivalents. These can be found in the 'Isotope Detection Options' in the Options Dialog:

   * Choose labels present in your data. Enter a four digit value of '1's and '0's each representing a label: D2, N15, S34 and C13. For eg. 0000 signifies no labels, 0001 signifies your data has C13 label and 1111 signifies your data has all four labels: ``pullIsotopes type="int" value="0"`` 

   EIC processing. Following are the parameters used for EIC processing and filtering and their GUI equivalents. These can be found under the 'Peak Detection' tab of Options Dialog:

   * 'Maximum Retention Time Difference between Peaks':  ``grouping _maxRtWindow type="float" value="0.5"`` 

   * 'EIC smoothing window': ``eicSmoothingWindow type="int" value="10"`` 

   Group Filtering. Following are the parameters used for filtering detected groups and their GUI equivalents. These can be found under the 'Group Filtering' tab of Peak Dialog:

   * 'Minimum Peak Intensity': ``minGroupIntensity type="float" value="5000"``

   * Quantitation type of the intensity set in 'Minimum Peak Intensity'. Accepted values are- "0":AreaTop, "1":Area, "2":Height, "3":AreaNotCorrected: ``quantitationType type="int" value="0"`` 

   * 'At least x% peaks above minimum intensity' slider. Enter value in percentage. 0% will default to 1 peak: ``quantileIntensity type="float" value="0"`` 

   * 'Minimum Quality': ``minQuality type="float" value="0.5"`` 

   * 'At least x% peaks above minimum quality' slider: ``quantileQuality type="float" value="0"`` 

   * 'Minimum peak width'. Enter number of scans: ``minPeakWidth type="int" value="1"`` 

   * 'Minimum Signal/Baseline Ratio': ``minSignalBaseLineRatio type="float" value="2"`` 

   * 'Minimum Good Peak/Group'. Value should be less or equal to the number of samples: ``minGoodGroupCount type="int" value="1"``

   * 'Peak Classifier Model File'. The default model file can be found in the bin folder of El-MAVEN installation. Enter full path to the 'default.model' file: ``model type="string" value="0"`` 

   * Not used by CLI: ``rtStepSize type="float" value="20"``

**3. GeneralArguments: General parameters**

   * Alignment. Positive integer value will run an alignment algorithm: ``alignSamples type="int" value="0"`` 

   * Export EIC in JSON. Positive integer value will save EICs JSON in user-specifed output folder: ``saveEicJson type="int" value="0"``

   * Output directory. Enter full path to desired output folder: ``outputdir type="string" value="0"``

   * Save Project. Positive integer value will save the project as a .mzroll file in user-specified output folder. This file can be loaded in El-MAVEN GUI for further processing or visualization: ``savemzroll type="int" value="0"``

   * Sample Path. Enter full path to a sample file in each row: ``samples type="string" value="0"`` 


Run
---

Once the parameters and directory paths have been set in the configuration file, run peak detection from the terminal using the following command from the El-MAVEN root directory:

   * Windows and Ubuntu: 

   ::

   $ ./bin/peakdetector.exe --xml config.xml
   
   * MacOS: 

   ::

   $ ./bin/peakdetector.app/Contents/MacOS/peakdetector --xml config.xml

The resulting CSV file (and other files depending on the configuration) can be found in the specified output directory.

Help
----

To print the help commands, execute the following:

::

$ Peakdetector.exe -h
