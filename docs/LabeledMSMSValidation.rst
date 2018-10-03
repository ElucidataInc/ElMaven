Labeled MS/MS Validation
========================

.. All images used are referecned here

.. |Validation01| image:: /image/Validation_1.png
.. |Validation02| image:: /image/Validation_2.png
.. |Validation03| image:: /image/Validation_3.png
.. |Validation04| image:: /image/Validation_4.png
.. |Validation05| image:: /image/Validation_5.png
.. |Validation06| image:: /image/Validation_6.png
.. |Validation07| image:: /image/Validation_7.png
.. |Validation08| image:: /image/Validation_8.png
.. |Validation09| image:: /image/Validation_9.png
.. |Validation10| image:: /image/Validation_10.png
.. |Validation11| image:: /image/Validation_11.png
.. |Validation12| image:: /image/Validation_12.png
.. |Validation13| image:: /image/Validation_13.png
.. |Validation14| image:: /image/Validation_14.png
.. |Validation15| image:: /image/Validation_15.png
.. |Validation16| image:: /image/Validation_16.png
.. |Validation17| image:: /image/Validation_17.png
.. |Validation18| image:: /image/Validation_18.png
.. |Validation19| image:: /image/Validation_19.png
.. |Validation20| image:: /image/Validation_20.png
.. |Validation21| image:: /image/Validation_21.png
.. |Validation22| image:: /image/Validation_22.png
.. |Validation23| image:: /image/Validation_23.png
.. |Validation24| image:: /image/Validation_24.png
.. |Validation25| image:: /image/Validation_25.png
.. |Validation26| image:: /image/Validation_26.png
.. |Validation27| image:: /image/Validation_27.png
.. |Validation28| image:: /image/Validation_28.png
.. |Validation29| image:: /image/Validation_29.png
.. |Validation30| image:: /image/Validation_30.png
.. |Validation31| image:: /image/Validation_31.png
.. |Validation32| image:: /image/Validation_32.png
.. |Validation33| image:: /image/Validation_33.png
.. |Validation34| image:: /image/Validation_34.png
.. |Validation35| image:: /image/Validation_35.png
.. |Validation36| image:: /image/Validation_36.png
.. |Validation37| image:: /image/Validation_37.png
.. |Validation38| image:: /image/Validation_38.png
.. |Validation39| image:: /image/Validation_39.png
.. |Validation40| image:: /image/Validation_40.png

MS/MS Support in El-MAVEN
-------------------------

Tandem mass spectrometry or MS/MS is an important technique in analytical Mass spectrometry. Processing MS/MS data for a large batch of samples can be a time-consuming task. We have added some support for processing such data in El-MAVEN.

Data acquired using the following methods are currently supported in El-MAVEN: Multiple Reaction Monitoring, Parallel Reaction Monitoring (or Full-Scan MS2) and Data Dependant MS2.

Feature Additions
-----------------

   * Text search option using precursor and product m/z
   * Targeted Peak Picking for MS/MS
   * Manual annotation of SRM transition

Validation
----------

In order to ensure correct reading and processing of MS2 sample files in El-MAVEN, MS spectra and chromatograms from El-MAVEN were validated using `Skyline <https://skyline.ms/wiki/home/software/Skyline/page.view?name=default>`_ by ProteoWizard.

Mass Spectra Validation
-----------------------

Dataset: Full-scan MS2 sample file

+--------------+----------------------+----------------------+
| Fragment     | Skyline              | El-MAVEN             |
|	       |                      |		             |
+==============+======================+======================+
| #1           | |Validation01|       | |Validation02|       |
+--------------+----------------------+----------------------+
| #2           | |Validation03|       | |Validation04|       |
+--------------+----------------------+----------------------+
| #3           | |Validation05|       | |Validation06|       |
+--------------+----------------------+----------------------+
| #4           | |Validation07|       | |Validation08|       |
+--------------+----------------------+----------------------+
| #5           | |Validation09|       | |Validation10|       |
+--------------+----------------------+----------------------+

Chromatogram Validation
-----------------------

Dataset: Multiple Reaction Monitoring

+--------------+--------------------+--------------------+
| Fragment     | Skyline            | El-MAVEN           |
|	       |                    |		         |
+==============+====================+====================+
| #1           | |Validation11|     | |Validation12|     |
+--------------+--------------------+--------------------+
| #2           | |Validation13|     | |Validation14|     |
+--------------+--------------------+--------------------+
| #3           | |Validation15|     | |Validation16|     |
+--------------+--------------------+--------------------+
| #4           | |Validation17|     | |Validation18|     |
+--------------+--------------------+--------------------+

Multiple Transition cases: The same precursor/product pairs can be tracked in multiple runs, for example, in cases where two metabolites have common fragments like Pyruvate 89/89 and Lactate 89/89. El-MAVEN lists these runs separately in the SRM List widget. Following are some examples:

+--------------+--------------------+--------------------+--------------------+
| Fragment     | Skyline            | El-MAVEN           | Comments           |
|	       |                    |		         |                    |
+==============+====================+====================+====================+
| Lactate      | |Validation19|     | Compound widget:   | Skyline EIC matches|
| Fragment #1  |                    |                    | Transition 2 from  | 
|              |                    | |Validation20|     | the SRM widget in  |
|              |                    |                    | El-MAVEN           |
|              |                    | Both runs for this |                    |
|              |                    | fragment have been |                    |
|              |                    | annotated as       |                    |
|              |                    | Lactate in the SRM |                    |
|              |                    | widget             |                    |
|              |                    |                    |                    |
|              |                    | Transition 1:      |                    |
|              |                    |                    |                    |
|              |                    | |Validation21|     |                    |
|              |                    |                    |                    |
|              |                    | Transition 2:      |                    |
|              |                    |                    |                    |
|              |                    | |Validation22|     |                    |
|              |                    |                    |                    |
|              |                    |                    |                    |
+--------------+--------------------+--------------------+--------------------+
| Pyruvate     | EIC is same as     | Compound widget:   | Unable to find     |
| Fragment #1  | that of Lactate    |                    | record of another  | 
|              | for this fragment. | |Validation24|     | transition in      |
|              | Different peak is  |                    | Skyline. The choppy|
|              | selected according | Zoomed in:         | peaks in El-MAVEN  |
|              | to the expected    |                    | are due to the     |
|              | retention time     | |Validation25|     | merging of two     |
|              |                    |                    | transitions        |
|              | |Validation23|     | SRM widget:        | annotated as       |
|              |                    | No transitions     | Lactate            |
|              |                    | mapped to          |                    |
|              |                    | Pyruvate           |                    |
|              |                    |                    |                    |
+--------------+--------------------+--------------------+--------------------+

Text search/Compound widget mismatch: Using the text search feature for MRM data can result in choppy peaks at times as compared to those from the SRM/compound widget. Following are some examples:

+--------------+--------------------+--------------------+--------------------+
| Fragment     | Skyline            | El-MAVEN Text      | El-MAVEN Compound  |
|	       |                    | Search	         | Widget             |
+==============+====================+====================+====================+
| #1           | |Validation26|     | |Validation27|     | |Validation28|     |
|              |                    |                    |                    |
|              |                    |                    |                    |
+--------------+--------------------+--------------------+--------------------+
| #2           | |Validation29|     | |Validation30|     | |Validation31|     |
|              |                    |                    |                    |
|              |                    |                    |                    |
+--------------+--------------------+--------------------+--------------------+
| #3           | |Validation32|     | |Validation33|     | |Validation34|     |
|              |                    |                    |                    |
|              |                    |                    |                    |
+--------------+--------------------+--------------------+--------------------+

Dataset: Full-scan MS2

+--------------+--------------------+--------------------+--------------------+--------------------+
| Fragment     | Skyline            | El-MAVEN Text      | El-MAVEN Compund   | Comments           |
|	       |                    | Search		 | Widget             |                    |
+==============+====================+====================+====================+====================+
| #1           | |Validation32|     | |Validation33|     | |Validation34|     | All three EIC's    |
|              |                    |                    |                    | match exactly      |
|              |                    |                    |                    |                    |
+--------------+--------------------+--------------------+--------------------+--------------------+
| #2           | |Validation35|     | |Validation36|     | |Validation37|     | All three EIC's    |
|              |                    |                    |                    | match exactly      |
|              |                    |                    |                    |                    |
+--------------+--------------------+--------------------+--------------------+--------------------+
| #3           | |Validation38|     | |Validation39|     | |Validation40|     | EIC from the       |
|              |                    |                    |                    | compound widget is |
|              |                    |                    |                    | different as one of|
|              |                    |                    |                    | the runs has been  |
|              |                    |                    |                    | annotated as this  |
|              |                    |                    |                    | fragment which     |
|              |                    |                    |                    | pulls up the EIC   |
|              |                    |                    |                    | for the whole run  | 
+--------------+--------------------+--------------------+--------------------+--------------------+

Issues Observed
---------------

   * Text search and compound/SRM widget EIC mismatch for MRM data: EIC obtained from the compound widget and Skyline are the same. The data points on the plot are same for all three. The error in text search is due to different processing. This has been filed as issue number `#487 <https://github.com/ElucidataInc/ElMaven/issues/487>`_; Status: Unresolved
   * Text search and compound widget EIC mismatch for PRM data: EIC from Text search and Skyline are the same. The error in compound widget happens due to linking of SRM ID to the compound widget. This pulls up the whole run instead of the particular fragment. This is a known issue `#442 <https://github.com/ElucidataInc/ElMaven/issues/442>`_; Status: Unresolved
   * Merged peaks in case of multiple transitions: This is being handled as part of issue number `#405 <https://github.com/ElucidataInc/ElMaven/issues/405>`_; Status: In progress
   * Mass spectra displays wrong product M/z value in some cases: The prodMz field in the title of spectra widget displays the base peak m/z value which may or may not be the same as the fragment m/z. This will be resolved as part of `#396 <https://github.com/ElucidataInc/ElMaven/issues/396>`_; Status: Unresolved
   * Peak table displays NA in Expected m/z column for MRM data: This has been filed as issue number `#493 <https://github.com/ElucidataInc/ElMaven/issues/493>`_

Conclusion
----------

Mass spectra validation against Skyline passed. This indicates correct parsing of MS/MS data in El-MAVEN. EIC validation against Skyline highlighted some known and unknown issues. We recommend using Text search for PRM data and Compound/SRM widget for MRM data processing until the reported issues have been fixed.
