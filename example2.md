---
layout: page
title: Features
permalink: /css/
---

<h1 class="page-title cdn-h1" style="color: #8985db !important;">{{ page.title | escape }}</h1>

<div class="section">

<br>
<br>
<h4 class="cdn-h2" style="color: #8985db;" ><b>Supported File Formats
</b></h4>
<br>
<h5><b>Raw Data Files:</b></h5>
<p>
ElMaven can read import raw data and export processed data in the following formats:</p>
<br>
<p>Input-</p>
  <ul class="dotted-list">
    <li>.mzxml</li>
    <li>.mzml</li>
  </ul>
  <br>
<h6>Output-</h6>
<ul class="dotted-list">
    <li>.csv</li>
    <li>.mzml</li>
  </ul>
  <br>
  
<h5><b>Raw Data Methods:</b></h5>
<br>
<b>Instrumentation:</b> <a href="https://user-images.githubusercontent.com/28925846/29772499-b16d721a-8c16-11e7-89d6-0f0d0e2f111e.png
">https://user-images.githubusercontent.com/28925846/29772499-b16d721a-8c16-11e7-89d6-0f0d0e2f111e.png
</a>
<p>Improve m/z calculations by adjusting these modules.</p>
<ul class="dotted-list">
  <li>Polarity/Ionization Modes</li>
  <li>Ionization type</li>
  <li>Q1 accuracy</li>
  <li>Q3 accuracy</li>
  <li>Filterline</li>
</ul>
<br>
<b>File Import:</b> <a href="https://user-images.githubusercontent.com/28925846/29772500-b16e6850-8c16-11e7-952e-6b7a3551c18f.png">https://user-images.githubusercontent.com/28925846/29772500-b16e6850-8c16-11e7-952e-6b7a3551c18f.png</a>

<br>
<p>Determine how to store the imported scan information based on ionization method, polarity of ions, MS level, minimum threshold, et cetra.</p>
<ul class="dotted-list">
  <li>Centroid Scans</li>
  <li>Scan Filter Polarity</li>
  <li>Scan Filter MS Level</li>
  <li>Scan Filter Minimum Intensity</li>
  <li>Scan Filter Intensity Minimum Quantile Cutoff</li>
  <li>Enable Multiprocessing</li>
</ul>

<br>

<p><b>Enable Multiprocessing</b> Multiple runs in chromatography column can lead to a drift in retention time across samples. Alignment of the samples corrects for these drifts and brings the peaks closer to median RT of the group.
</p>

<p>Pre-alignment EIC:</p>
<a href="
 https://user-images.githubusercontent.com/31105033/29817378-4ab73876-8ca7-11e7-9ccc-51cddf7675d9.JPG">
 https://user-images.githubusercontent.com/31105033/29817378-4ab73876-8ca7-11e7-9ccc-51cddf7675d9.JPG</a>
 <br>
<p>Post-alignment EIC:</p>
<a href="
 https://user-images.githubusercontent.com/31105033/29817379-4ab90a70-8ca7-11e7-987c-53990b5e265d.JPG
"> https://user-images.githubusercontent.com/31105033/29817379-4ab90a70-8ca7-11e7-987c-53990b5e265d.JPG
</a>
 https://user-images.githubusercontent.com/31105033/29817379-4ab90a70-8ca7-11e7-987c-53990b5e265d.JPG

<p>ElMaven provides three visualizations for alignment analysis.</p>

<ul class="dotted-list">
  <li>
   <b>Alignment Visualization:</b> Click on this widget and select any group to look at its delta Rt vs Rt graph.
    <br>
    <img class="img-res" src="https://user-images.githubusercontent.com/28925846/29772958-bb02c3fa-8c18-11e7-8efb-2763fc64535a.png" >
  </li>
  <li>
   <b>Alignment Visualization (For All Groups):
   </b> This widget shows a graph for delta Rt vs Rt for all the groups.
    <br>
    <img class="img-res" src="https://user-images.githubusercontent.com/28925846/29772953-bad88ef0-8c18-11e7-83bb-818adbf7d86c.png" >
  </li>

  <li>
   <b>Alignment Polynomial Fit:
   </b>  Click on this widget bar for Poly fit alignment.
    <br>
    <img class="img-res" src="https://user-images.githubusercontent.com/28925846/29772953-bad88ef0-8c18-11e7-83bb-818adbf7d86c.png" >
  </li>

<ul>
<br>

<p><b>Automatic Peak Picking:</b> ElMaven can automatically select high intensity and high quality groups. This workflow is called automatic peak curation. The feature has been tested for accuracy against the gold standard of manually curated peaks by scientists at other tools. This feature reduces the time spent from days to a few hours in curating peaks in Untargted Global Profiling datasets.</p>
<br>
<b>Peak Detection:</b>
<a href=" https://user-images.githubusercontent.com/28925846/29772498-b16c2c16-8c16-11e7-8ada-783f974307c0.png"> https://user-images.githubusercontent.com/28925846/29772498-b16c2c16-8c16-11e7-8ada-783f974307c0.png</a>
<br>
<p>Reduce noise in the Extracted Ion Chromatograms (EIC) with settings for smoothing, baseline and peak filtering.
</p>
<br>

<p>Peak Detection and Grouping Settings</p>
<ul class="dotted-list">
  <li>EIC Smoothing Algorithm</li>
  <li>EIC Smoothing Window</li>
  <li>Max Retention Time Difference Between Peaks
</li>
<ul>
<br>
<p>Baseline Calculation</p>
<ul class="dotted-list">
  <li>Drop top x% intensities from chromatogram</li>
  <li>Baseline Smoothing</li>
<ul>
<br>
<p>Peak Filtering</p>
<ul class="dotted-list">
  <li>Min. Signal Baseline Difference</li>
<ul>
<br>
<p>
<b>Isotope Detection:</b><a href="https://user-images.githubusercontent.com/28925846/29772496-b1680ec4-8c16-11e7-81ea-2308c552a30d.png">https://user-images.githubusercontent.com/28925846/29772496-b1680ec4-8c16-11e7-81ea-2308c552a30d.png</a>
</p>
<p>Detect labeled samples by enabling the following settings according to the tracer used.</p>
<p>Are Sample Labeled?</p>
<ul class="dotted-list">
  <li>Bookmarks, peak detection, file export</li>
  <li>Isotopic barplot</li>
  <li>Isotopic widget</li>
  <li>Number of M+n isotopes</li>
  <li>Abundance Threshold</li>
</ul>

<p>Filter Isotopic Peaks based on these criteria</p>
<ul class="dotted-list">
  <li>Minimum Isotope-Parent Correlation</li>
  <li>Isotope is within [X] scans of parent</li>
  <li>Maximum % Error to Natural Abundance</li>
  <li>Correct for Natural C13 Isotope Abundance</li>
<ul>
<p>
<br>
<b>EIC (XIC):</b> <a href="https://user-images.githubusercontent.com/28925846/29772495-b1675da8-8c16-11e7-9263-ff7d355cfc15.png">https://user-images.githubusercontent.com/28925846/29772495-b1675da8-8c16-11e7-9263-ff7d355cfc15.png</a>
</p>
<br>
<p>Detect labeled samples by enabling the following settings according to the tracer used.</p>
<p>Select a method to merge EICs over m/z from:</p>
<ul class="dotted-list">
  <li>EIC type</li>
</ul>

<p><b>Peak Grouping:</b><a href=" https://user-images.githubusercontent.com/28925846/29772494-b1392604-8c16-11e7-8b9e-94323ca93b27.png">https://user-images.githubusercontent.com/28925846/29772494-b1392604-8c16-11e7-8b9e-94323ca93b27.png</a></p>
<br>
<p>All peaks originating from the same ion are sought to be viewed together. Peaks are assigned a grouping score to determine whether they should be grouped together. The score depends on the following 3 parameters and their weights:</p>
<ol>
  <li>a) RT difference: Closer peaks are assigned a higher score.</li>
  <li>b) Intensity difference: Smaller difference accounts for a higher score.</li>
  <li>c) Overlap: Greater overlap accounts for a higher score.</li>
</ol>
<br>
<p>Adjust these parameters with:</p>
<ul class="dotted-list">
  <li>Consider Overlap</li>
  <li>Sliders provided to adjust the weights attached to each of the three parameters.</li>
</ul>
<p><b>Group Ranks:</b> <a href="https://user-images.githubusercontent.com/28925846/29772493-b10d2dce-8c16-11e7-8712-06558f025c14.png"> https://user-images.githubusercontent.com/28925846/29772493-b10d2dce-8c16-11e7-8712-06558f025c14.png</a></p>
<p>Group rank is one of the parameters for group filtering. The score changes in the following manner</p>
<ol>
  <li>a) Group Quality: Peaks are assigned a quality score by a machine learning algorithm in El Maven. Better quality leads to a higher rank.</li>
  <li>b) Group Intensity: Better intensity leads to a higher rank.</li>
  <li>c) RT difference: Samller RT difference leads to a higher rank. The slider is disabled if Consider Retention Time is unchecked.</li>
</ol>
<br>
<p>Adjust these parameters with:</p>
<ol>
<li>a) Consider Retention Time</li>
<li>b) Quality Weight slider</li>
<li>c) Intensity Weight slider</li>
<li>d) dRT Weight slider</li>
</ol>
<br>
<h4 class="cdn-h2" style="color: #8985db;"><b>Statistical Analysis</b></h4>
<br>
<b>Scatter Plot:</b><p> It represents the log2 of fold change between the two sets: Sample 1 and Sample 2. The points falling on the diagonal line indicates high similarity between the sets. The two sets can be, for example, samples from a diseased cell and a healthy cell. In such cases the comparison helps study the difference between them.
</p>
<br>
<a href="https://elucidatainc.atlassian.net/wiki/download/attachments/21201203/ScatterPlot.PNG?version=1&modificationDate=1504860441052&cacheVersion=1&api=v2">ScatterPlot.PNG</a>
<br>
<p>
<b>Volcano Plot:</b> Significance vs. Fold change graph to view differential intensities or abundances of two sets: Sample 1 and Sample 2.</p> 
<a href="https://elucidatainc.atlassian.net/wiki/download/attachments/21201203/FlowerPlot.PNG?version=1&modificationDate=1504860460783&cacheVersion=1&api=v2">FlowerPlot.PNG
</a>
<br>
<p><b>Cluster Analysis:</b> The cluster analysis is used to give a visual representation of the correlation between the metabolites in all the samples.</p>
<br>
<br>
<h4 class="cdn-h2" style="color: #8985db;"><b>Visualizations</b></h4>
<br>
<b>Chromatogram Plot:</b><a href="https://user-images.githubusercontent.com/28925846/29777350-5f5f2bcc-8c29-11e7-8f70-d1fd87146fb8.png">https://user-images.githubusercontent.com/28925846/29777350-5f5f2bcc-8c29-11e7-8f70-d1fd87146fb8.png</a> 
<p>An Extracted Ion Chromatogram (EIC) is a graph of Intensity vs. RT for a specific m/z range. EIC window displays the peaks for every compound selected or m/z range provided.
</p>

<b>Total Ion Current:</b> TIC is the sum of all intensities in a scan.
<br>
<a href="https://elucidatainc.atlassian.net/wiki/download/attachments/21201203/TIC.PNG?version=1&modificationDate=1504861203790&cacheVersion=1&api=v2">TIC.PNG</a>
<br>
<b>Peak Intensity Barplot:</b><a href=" https://user-images.githubusercontent.com/28925846/29876673-18d6f49e-8dbb-11e7-92da-3c9132aafeed.png"> https://user-images.githubusercontent.com/28925846/29876673-18d6f49e-8dbb-11e7-92da-3c9132aafeed.png</a>

They display the peak intensity for a group in every sample. Intensity can be calculated by various quantitation types: Area, AreaTop, Height etc.
<br>
<b>Isotope Plot:</b><a href=" https://user-images.githubusercontent.com/28925846/29966908-9b79f35c-8f31-11e7-9516-b2746f444234.png "> https://user-images.githubusercontent.com/28925846/29966908-9b79f35c-8f31-11e7-9516-b2746f444234.png </a>
This visual displays the relative presence of all isotopologues of an ion. Each bar in the plot represents the relative percentage of different isotopic species for the selected group in a sample.
<br>
<b>Box Plot: </b>
<br>
The box plot shows the spread of intensities in the group, and where each peak lies in relation to the median RT.
<br>
<b>Mass Spectra:</b><a href=" https://user-images.githubusercontent.com/28925846/29877841-5dc3e7a8-8dbe-11e7-804c-a897c15fe763.JPG "> https://user-images.githubusercontent.com/28925846/29877841-5dc3e7a8-8dbe-11e7-804c-a897c15fe763.JPG </a>
<br>
Mass Spectra plot displays each peak, its mass, and intensity for a scan. This feature is especially useful for MS/MS data and isotopic detection. Adjustments made in grouping, by changing the PPM value for EIC, is reflected in this spectra.
<br>
<b>Alignment Visualization:</b> Prolonged use of the LC column can lead to a drift in retention time across samples. Alignment shifts the peak RTs in every sample to correct for this drift and brings the peaks closer to median RT of the group. The three alignment graphs indicate how aligned or misaligned the peaks in a group are.
<ul class="dotted-list">
<li >
Visualization for One Group:<a href="https://user-images.githubusercontent.com/28925846/29772958-bb02c3fa-8c18-11e7-8efb-2763fc64535a.png">https://user-images.githubusercontent.com/28925846/29772958-bb02c3fa-8c18-11e7-8efb-2763fc64535a.png</a> </li>
<li>
Visualization for All Groups: <a href="https://user-images.githubusercontent.com/28925846/29772953-bad88ef0-8c18-11e7-83bb-818adbf7d86c.png">https://user-images.githubusercontent.com/28925846/29772953-bad88ef0-8c18-11e7-83bb-818adbf7d86c.png</a> </li>
<li>
Alignment Polynomial fit: <a href="https://user-images.githubusercontent.com/28925846/29772952-ba8b43d4-8c18-11e7-852d-34db2bf20ac6.png">https://user-images.githubusercontent.com/28925846/29772952-ba8b43d4-8c18-11e7-852d-34db2bf20ac6.png</a> </li>
</ul>

