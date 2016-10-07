Maven
=====

## Installation
Run `./run.sh`.

## Uninstall MAVEN
Run  `./uninstall.sh`.

## Documentation
1. Use `cd docs/ && doxygen Doxyfile` to generate documentation of the mzroll source code.
2. Open the `docs/html/index.html` file in browser to view the documentation.
3. Use `rm -rf docs/html maven_opensource/docs/latex` to clean the documentation files. Complete step 1 to create documentation again.
  NOTE: uninstall.sh also cleans stale documentation files.

## PeakDetector CLI

1. Run `cd peakdetector && make && cd ..`

/path/to/peakdetector --savemzroll 1 --minQuality 0.5 --ppmMerge 10 //
--minSignalBaseLineRatio 2 --ionizationMode 1 --grouping_maxRtWindow 0.5 //
--minPrecursorCharge 0 --db /path/to/database/Kegg_PopularMetabolites.tab.txt //
--alignSamples 0 --minGroupIntensity 10000 --matchRtFlag 0 --pullIsotopes 0 //
--model /path/to/model/default.model --minPeakWidth 5 --minGoodGroupCount 3 //
--eicMaxGroups 10 --eicSmoothingWindow 12 --rtStepSize 10 --outputdir /path/to/output/directory //
/path/to/mzxml/file1/"a2811-pos/"bk_#sucyxpe_1_10.mzxml /path/to/mzxml/file2/"a2811-pos/"bk_#sucyxpe_1_11.mzxml //
/path/to/mzxml/file3/"a2811-pos/"bk_#sucyxpe_1_12.mzxml ..
