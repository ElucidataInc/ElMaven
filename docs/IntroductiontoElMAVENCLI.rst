El-MAVEN Command Line Interface
===============================

The peakdetector executable in El-MAVEN can load sample files, perform peak detection and save the resulting peaks in a CSV or mzroll file. It reads a configuration file with user-set parameters and paths to input and output files.

This executable is not part of installer versions of El-MAVEN. Please clone the repository and build `El-MAVEN <https://github.com/ElucidataInc/ElMaven/blob/develop/README.md#compilation>`_ to use this feature.


**Create a default configuration file**

Users need an XML file to specify their configuration and specify directory paths. The XML template can be generated using a simple command and modified according to user requirements.

   * Open the terminal

   * Move to your local El-MAVEN folder ``cd user@pc:~/ElMaven/ElMaven``

   * Execute the following command

      * For Windows and Linux: 
         
         * ``./bin/peakdetector.exe --defaultxml``
      
      * For MacOS: 

         * ``./bin/peakdetector.app/Contents/MacOS/peakdetector --xml config.xml``

   * A default XML will be created in your current folder

**Peak detector parameters**

CLI provides a limited number of parameters for user-modification. Default values are used for all other parameters used in El-MAVEN GUI.

The template XML contains three sets of parameters:

1. **OptionalDialogArguments - Global parameters**

   * ``ionizationMode type="int" value="-1"`` User needs to know the ionization mode and accepted values are 0 (neutral), -1 (negative) and +1 (positive). 

   * ``charge type="int" value="1"`` Ionization charge. This is the number of positive or negative charge added to every ion. Accepted values are positive integers.

   * ``compoundPPMWindow type="float" value="5"`` Mass accuracy. Set the +/- ppm range for m/z values.

2. **PeaksDialogArguments - Peak detection parameters**

   * ``processAllSlices type="int" value="0"`` Peak Detection Algorithm. Accepted values: "0" for Compound Database Search, "1" (or higher) for Automated Feature Detection.

   Following are the arguments used in Automated Peak Detection and their GUI equivalents. These can be found in the 'Feature Detection Selection' tab of Peaks Dialog:

   *  ``minScanMz type="float" value="0"`` 'Limit m/z Range' minimum

   *  ``maxScanMz type="float" value="1e+009"`` 'Limit m/z Range' maximum

   *  ``minScanRt type="float" value="0"`` 'Limit Time Range' minimum

   *  ``maxScanRt type="float" value="1e+009"`` 'Limit Time Range' maximum

   *  ``minScanIntensity type="float" value="50000"`` 'Limit Intensity Range' minimum

   *  ``maxScanIntensity type="float" value="1e+010"`` 'Limit Intensity Range' maximum

   *  ``ppmMerge type="float" value="20"`` 'Mass Domain Resolution'

   Following are the arguments used in Compound database search and their GUI equivalents. These can be found in the 'Feature Detection Selection' tab of Peaks Dialog:

   *  ``Db type="string" value="0"`` 'Select Database'. Enter full path to the database file

   *  ``matchRtFlag type="int" value="0"`` 'Match Retention Time (+/-)'. Value of '0' will set the flag as false. Positive integer value will set the allowed deviation in minutes

   *  ``eicMaxGroups type="int" value="0"`` 'Limit Number of Reported Groups per Compound'

   Isotope detection. Following are the settings used to pull isotopes in case of database search and their GUI equivalents. These can be found in the 'Isotope Detection Options' in the Options Dialog:

   * ``pullIsotopes type="int" value="0"`` Choose labels present in your data. Enter a four digit value of '1's and '0's each representing a label: D2, N15, S34 and C13. For eg. 0000 signifies no labels, 0001 signifies your data has C13 label and 1111 signifies your data has all four labels

   EIC processing. Following are the parameters used for EIC processing and filtering and their GUI equivalents. These can be found under the 'Peak Detection' tab of Options Dialog:

   *  ``grouping _maxRtWindow type="float" value="0.5"`` 'Maximum Retention Time Difference between Peaks'

   *  ``eicSmoothingWindow type="int" value="10"`` 'EIC smoothing window'

   Group Filtering. Following are the parameters used for filtering detected groups and their GUI equivalents. These can be found under the 'Group Filtering' tab of Peak Dialog:

   *  ``minGroupIntensity type="float" value="5000"`` 'Minimum Peak Intensity'

   *  ``quantitationType type="int" value="0"`` Quantitation type of the intensity set in 'Minimum Peak Intensity'. Accepted values are- "0":AreaTop, "1":Area, "2":Height, "3":AreaNotCorrected

   *  ``quantileIntensity type="float" value="0"`` 'At least x% peaks above minimum intensity' slider. Enter value in percentage. 0% will default to 1 peak

   *  ``minQuality type="float" value="0.5"`` 'Minimum Quality'

   *  ``quantileQuality type="float" value="0"`` 'At least x% peaks above minimum quality' slider

   *  ``minPeakWidth type="int" value="1"`` 'Minimum peak width'. Enter number of scans

   *  ``minSignalBaseLineRatio type="float" value="2"`` 'Minimum Signal/Baseline Ratio'

   *  ``minGoodGroupCount type="int" value="1"`` 'Min. Good Peak/Group'. Value should be less or equal to the number of samples

   *  ``model type="string" value="0"`` 'Peak Classifier Model File'. The default model file can be found in the bin folder of El-MAVEN installation. Enter full path to the 'default.model' file

   *  ``rtStepSize type="float" value="20"`` Not used by CLI

3. **GeneralArguments - General parameters**

   *  ``alignSamples type="int" value="0"`` Alignment. Positive integer value will run an alignment algorithm

   *  ``saveEicJson type="int" value="0"`` Export EIC in JSON. Positive integer value will save EICs JSON in user-specifed output folder

   *  ``outputdir type="string" value="0"`` Output directory. Enter full path to desired output folder

   *  ``savemzroll type="int" value="0"`` Save Project. Positive integer value will save the project as a .mzroll file in user-specified output folder. This file can be loaded in El-MAVEN GUI for further processing or visualization

   *  ``samples type="string" value="0"`` Sample Path. Enter full path to a sample file in each row


**Run**

Once the parameters and directory paths have been set in the configuration file, run peak detection from the terminal using the following command from the El-MAVEN root directory:

   * Windows and Linux: 
   
      * ``./bin/peakdetector.exe --xml config.xml``
   
   * MacOS: 

      * ``./bin/peakdetector.app/Contents/MacOS/peakdetector --xml config.xml``

The resulting CSV file (and other files depending on the configuration) can be found in the specified output directory.

**Help**

``Peakdetector.exe -h`` prints the help commands.
