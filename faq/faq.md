---
<!-- # layout: layoutfaq -->
title: FAQ
permalink: /faq/
---


<div>
<head>
<style>
mark {
	background-color: white;
	color: black;
}

 <!-- nav {
  width: 100%; 
  height: 65px; 
  background-color: #8884D9;
  margin:0px;
}


  nav a {
    display: block;
    float: right;

    font-family: Heebo,sans-serif;
	font-weight: 500;
    font-size: 16px;
    color: white;

     background-color: #17b0cf; 
     border: 0px solid #000; 
   
  
   
    
    padding-bottom: 10px;
    padding-left: 38px;
	 padding-top: 25px;
	 padding-right: 20px;

    text-decoration: none;
}

#logo {

  float: left;
 
  font-family: Heebo,sans-serif;
  font-size: 2.0rem;

 

  padding-left: 37px;
  padding-right: 40px;
  margin-right: 10px;
  margin-top: -10px;
  }  -->


</style>

	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1">

	<link rel="stylesheet" href="css/reset.css"> <!-- CSS reset -->
	<link rel="stylesheet" href="css/style.css"> <!-- Resource style -->
	<script src="js/modernizr.js"></script> <!-- Modernizr -->
	<title>El-Maven | FAQs</title>
	
</head>
 


</div>


<div>
<body>


<!-- <nav>
  
    <ul >
	  <li ><a id="logo" href="#">ElMaven</a></li>
	  <li ><a href="#">   </a></li>
      <li ><a href="#">FAQ</a></li>
      <li><a href="/ElMaven/Downloads/">Downloads</a></li>
      <li><a href="/ElMaven/documentation/">Documentation</a></li>
      <li><a href="/ElMaven/Features/">Features</a></li>
	  <li><a href="/ElMaven/Why-ElMaven/">Why ElMaven</a></li>
	  <li><a href="/ElMaven/">Home</a></li>
	
    </ul>

</nav> -->


<section class="cd-faq">
	<ul class="cd-faq-categories">
		<li><a class="selected" href="#general">General</a></li>
		<li><a href="#options_icon">Options Dialog</a></li>
		<li><a href="#compound_database">Compound Database</a></li>
		<li><a href="#blank_samples">Blank Samples	</a></li>
		<li><a href="#peak_grouping">Peak Grouping</a></li>
		<li><a href="#curating_peaks">Curating Peaks</a></li>
		<li><a href="#isotope_detection">Isotope Detection</a></li>
		<li><a href="#untargeted">Untargeted Data Analysis</a></li>
		<li><a href="#baseline_calculation">Baseline Calculation</a></li>
		
		
		
	</ul> <!-- cd-faq-categories -->

	<div class="cd-faq-items">
		<ul id="general" class="cd-faq-group">
			<li class="cd-faq-title"><h2>General</h2></li>
			<li>
				<a class="cd-faq-trigger" href="#0">Which file formats are compatible with El-MAVEN?</a>
				<div class="cd-faq-content">


                            <p>● El-MAVEN supports .mzXML, .mzML, .mzroll, .mzPeaks and netCDF formats as inputs. To convert different machine formats to the above mentioned format use <a href="http://proteowizard.sourceforge.net/tools.shtml" target="_blank">MSConvert</a>.</p><br />

                    <p>● You can store El-MAVEN output as .csv, .pdf, .json, .png and .mzroll or push the data to Polly for analyzing, sharing or storing.</p>

                        </div> <!-- cd-faq-content -->
			</li>

			<li>
				<a class="cd-faq-trigger" href="#0">What settings should I use to convert my raw files?</a>
				<div class="cd-faq-content">
					<p>El-MAVEN supports .mzXML, .mzML formats as input. To convert different machine formats to the above-mentioned format use <a href="http://proteowizard.sourceforge.net/tools.shtml" target="_blank">MSConvert</a>.</p><br />
					<p>The <mark>settings used in MSConvert</mark> should be as follows:</p>
					<p>● Output Format: <mark>mzXML/mzML</mark></p>
					<p>● Binary encoding precision:<mark> according to system</mark></p>
					<p>● Write index: <mark>Checked</mark></p>
					<p>● TPP compatibility: <mark>Checked</mark></p>
					<p>● Use zlib compression: <mark>Unchecked</mark></p>
					<p>● Package in gzip: <mark>Unchecked</mark></p>
					<br />
					<img src="https://user-images.githubusercontent.com/29090291/42496536-00eed7fe-8444-11e8-8ce6-7aa57703fb1b.png" alt="Convert Raw Data" height="280" width="380" style="margin-left: 16%">
				</div> <!-- cd-faq-content -->
			</li>

			<li>
				<a class="cd-faq-trigger" href="#0">How can I reduce the data size input in El-MAVEN?</a>
				<div class="cd-faq-content">
					<p><mark>Centroiding can be used while conversion of raw data to reduce data size.</mark></p>
					<p>Another way to reduce the size of data input in El-MAVEN is as follows:</p>
					<p>● Add "Threshold Peak Filter" as shown in the image</p>
					<p>● Add "Value" as 10000 and convert your raw files</p>
					<br />
					<img src="https://user-images.githubusercontent.com/29090291/42497067-a4468086-8445-11e8-99a4-0c32a4c07361.png" alt="Reduce Data Size" height="480" width="580" style="margin-left: 2%">
					<br />
					<p>These settings reduce the converted file size significantly.</p>
					<p>NOTE: "Value" settings may vary for different types of data</p>
				</div> <!-- cd-faq-content -->
			</li>

            <li>
				<a class="cd-faq-trigger" href="#0">Can El-MAVEN store my data?</a>
				<div class="cd-faq-content">

                            <p>   Yes. You can save your work as a .mzroll project. Loading it back will restore the session. We also offer an integration to Polly, a cloud-based platform where you can store all your data as well as the parameters and peaks you curated.</p>


				</div> <!-- cd-faq-content -->
			</li>

        <li>
				<a class="cd-faq-trigger" href="#0">Should I save the project file (.mzroll) every time on reloading?</a>
				<div class="cd-faq-content">

                            <p>  The project file is automatically updated on making changes in El-MAVEN. It does not have to be saved again.</p>


				</div> <!-- cd-faq-content -->
			</li>

			<li>
				<a class="cd-faq-trigger" href="#0">Which alignment method should I use in El-MAVEN? How are the three methods different?</a>
				<div class="cd-faq-content">

                            <p>    OBI-warp is the latest addition to El-MAVEN's list of alignment methods. It uses <a href="http://proteowizard.sourceforge.net/tools.shtml" target="_blank">Dynamic Time warping</a>, which is a global fit on the retention time drifts.<mark> It has empirically performed better than the other two algorithms and is the recommended method for alignment.</mark></p><br />


<p>In case OBI-warp does not perform well for a particular data set, you can try:</p><br />
<p>
● Poly Fit: the oldest alignment algorithm in El-MAVEN. It was built to correct for retention time drift by fitting a polynomial function to correct for the drifts.</p><br />

<p>● Loess Fit: Uses a part by part fitting thereby having a closer fit to the data and hence works better than Poly Fit in most cases.  </p>
				</div> <!-- cd-faq-content -->
			</li>

			<li>
				<a class="cd-faq-trigger" href="#0">How can I observe the total ion current for my data?</a>
				<div class="cd-faq-content">

                            <p>   Click on the <img src="https://user-images.githubusercontent.com/29090291/42497655-ab9f901e-8447-11e8-87a8-0ea632610b7e.png" alt="Blank Sample Image" height="30" width="40"> widget in the EIC widget bar to display the total ion current for your data.</p>


				</div> <!-- cd-faq-content -->
			</li>

			<li>
				<a class="cd-faq-trigger" href="#0">Can I view the isotope distribution for my samples in El-MAVEN?</a>
				<div class="cd-faq-content">

                            <p>  Yes. You can view the percentage of each isotope present in your sample by clicking on the isotope plot widget  <img src="https://user-images.githubusercontent.com/29090291/42497913-769f150a-8448-11e8-9bb0-960565ded088.png" alt="Blank Sample Image" height="25" width="40"> on the EIC widget bar. </p><br />
							<p>On clicking this icon, the isotope plot appears, where you can hover over each sample to view the isotopic distribution after you have uploaded your samples in El-MAVEN.</p>


				</div> <!-- cd-faq-content -->
			</li>

			<li>
				<a class="cd-faq-trigger" href="#0">What type of LC columns has MAVEN algorithms been tested on?</a>
				<div class="cd-faq-content">

														<p>El-MAVEN has been tested on HILIC columns but a more important thing to note is that El-MAVEN is neutral to the kind of column used and hence using any column will allow the users to process data through El-MAVEN.</p>
				</div> <!-- cd-faq-content -->
			</li>

		</ul> <!-- cd-faq-group -->
		<ul id="options_icon" class="cd-faq-group">
			<li class="cd-faq-title"><h2>Options Dialog</h2></li>
			<li>
				<a class="cd-faq-trigger" href="#0">Do settings in "Options" dialog impact automated peak detection?</a>
				<div class="cd-faq-content">
					<p>Yes. All the parameters within “Options” are global parameters and affect every workflow. You can read more about the parameters in our detailed <a href="https://github.com/ElucidataInc/ElMaven/wiki/Introduction-to-ElMaven-UI#2-global-settings" target="_blank"> UI documentation.</a></p>
				</div> <!-- cd-faq-content -->
			</li>

			<li>
				<a class="cd-faq-trigger" href="#0">What is the default Ionization Mode in El-MAVEN? Can I change it?</a>
				<div class="cd-faq-content">
					<p>El-MAVEN auto detects the Ionization Mode of your data by default. However, you can manually select this by,</p>
					<p>● Going to "Options"<img src="https://user-images.githubusercontent.com/29090291/42500500-1b27c36e-844f-11e8-8659-6e8cdbe18a38.png" alt="Options Icon" height="25" width="40"> in the top menu</p>
					<p>● Selecting “Instrumentation” tab</p>
					<p>● Selecting the correct “Ionization Mode” from the drop-down</p><br />
					 <img src="https://user-images.githubusercontent.com/29090291/42500487-157725b8-844f-11e8-9be6-2c5f19cea0ce.png" alt="Ionization Mode" height="250" width="600">

				</div> <!-- cd-faq-content -->
			</li>
          
		  <li>
				<a class="cd-faq-trigger" href="#0">What is the default Ionization Type in El-MAVEN? Can I change it?</a>
				<div class="cd-faq-content">
					<p>The default Ionization Type in El-MAVEN is ESI (Electron-Spray Ionization). This can be changed as follows:</p>
					<p>● Going to "Options"<img src="https://user-images.githubusercontent.com/29090291/42500500-1b27c36e-844f-11e8-8659-6e8cdbe18a38.png" alt="Options Icon" height="25" width="40"> in the top menu</p>
					<p>● Selecting “Instrumentation” tab</p>
					<p>● Select the correct “Ionization Type” from the drop-down</p><br />
					 <img src="https://user-images.githubusercontent.com/29090291/42501024-69383e3e-8450-11e8-8494-bd688b9b0780.png" alt="Ionization Type" height="250" width="600">

				</div> <!-- cd-faq-content -->
			</li>

			<li>
				<a class="cd-faq-trigger" href="#0">Can I select a particular MS level in El-MAVEN?</a>
				<div class="cd-faq-content">
					<p>Yes. To Select MS Level filters,</p>
					<p>● Going to "Options"<img src="https://user-images.githubusercontent.com/29090291/42500500-1b27c36e-844f-11e8-8659-6e8cdbe18a38.png" alt="Options Icon" height="25" width="40"> in the top menu</p>
					<p>● Go to "File Import" tab</p>
					<p>● Select "Scan Filter MS Level" from the drop-down</p><br />
					 <img src="https://user-images.githubusercontent.com/29090291/42501527-8760baca-8451-11e8-8e28-fface60bd4b6.png" alt="Ionization Type" height="250" width="600">

				</div> <!-- cd-faq-content -->
			</li>

			<li>
				<a class="cd-faq-trigger" href="#0">How to process a Polarity Switching data in El-MAVEN?</a>
				<div class="cd-faq-content">
					<p>El-MAVEN allows a user to extract only positive or negative scans from mzXML files. This can be done before uploading the data by the following steps:</p>
					<p>● Going to "Options"<img src="https://user-images.githubusercontent.com/29090291/42500500-1b27c36e-844f-11e8-8659-6e8cdbe18a38.png" alt="Options Icon" height="25" width="40"> in the top menu</p>
					<p>● Click on "File Import"</p>
					<p>● Click on "Scan Filter Polarity" and select the polarity you want to process</p><br />
					 <img src="https://user-images.githubusercontent.com/29090291/42501535-89e46bd4-8451-11e8-88c3-db579a708537.png" alt="Paolarity Switching" height="250" width="600">

				</div> <!-- cd-faq-content -->
			</li>

			<li>
				<a class="cd-faq-trigger" href="#0">Can I remove low abundance peaks in El-MAVEN?</a>
				<div class="cd-faq-content">
					<p>The abundance of data can be reduced in El-MAVEN in two ways::</p>
					<p>1. Selecting Minimum Intensity Filter in El-MAVEN</p>
					<p>The Minimum Intensity Filter in El-MAVEN reflects the peaks above the selected minimum intensity. Selecting a high minimum intensity filter will remove all the peaks below the intensity and hence remove abundance. This can be done as follows,</p>
					<p>● Go to "Options"<img src="https://user-images.githubusercontent.com/29090291/42500500-1b27c36e-844f-11e8-8659-6e8cdbe18a38.png" alt="Options Icon" height="25" width="40"> in the top menu</p>
					<p>● Select the "File Import" tab</p>
					<p>● Set the "Scan Filter: Minimum Intensity" field</p><br />
					 <img src="https://user-images.githubusercontent.com/29090291/42502198-0695de0a-8453-11e8-84e8-ac765d211161.png" alt="Low Abundance" height="250" width="600">
					 <br />
					 <p>2. Centroiding in El-MAVEN</p>
					 <p>Selecting "Centroid Scan" in the File Import tab within the "Options" icon also reduces the data abundance.</p>
					 <p>● Go to "Options"<img src="https://user-images.githubusercontent.com/29090291/42500500-1b27c36e-844f-11e8-8659-6e8cdbe18a38.png" alt="Options Icon" height="25" width="40"></p>
					<p>● Click on "File Import"</p>
					<p>● Click on "Centroid Scan"</p><br />
					 <img src="https://user-images.githubusercontent.com/29090291/42502188-034e9dc2-8453-11e8-9bfd-b30bc81c1871.png" alt="Centroid Scan" height="150" width="450" style="margin-left: 13.5%">

				</div> <!-- cd-faq-content -->
			</li>



			<li>
				<a class="cd-faq-trigger" href="#0">What does the "EIC Smoothing Algorithm" signify?</a>
				<div class="cd-faq-content">
					<p>The "EIC Smoothing Algorithm" parameter smoothes the data points wrt the selected algorithm which helps in increasing the signal/noise ratio.</p>
					<p>In El-MAVEN there are three Smoothing Algorithms you can select from. Namely,</p>
					<p>1. Savitzky-Golay: It preserves the original shape and features of the signal better than most other filters</p>
					<p>2. Gaussian: It reduces noise by averaging over the neighborhood with the central pixel having higher weight but successfully preserves sharp edges</p>
					<p>3. Moving Average: It takes the simple average of all points over time. Signal behavior is not natural. Least preferred method for smoothing</p><br />
					<p>El-MAVEN has Savitzky-Golay as its default EIC Smoothing Algorithm. You can select any other algorithm by the following steps:</p>
					<p>● Go to "Options"<img src="https://user-images.githubusercontent.com/29090291/42500500-1b27c36e-844f-11e8-8659-6e8cdbe18a38.png" alt="Options Icon" height="25" width="40"> in the top menu</p>
					<p>● Click on "Peak detection" tab</p>
					<p>● Select the algorithm from the drop-down</p><br />
					 <img src="https://user-images.githubusercontent.com/29090291/42503046-3b4c2c4c-8455-11e8-9dbd-68df6d2e239b.png" alt="Eic smoothing " height="250" width="600">
					

				</div> <!-- cd-faq-content -->
			</li>


			<li>
				<a class="cd-faq-trigger" href="#0">What does the "EIC Smoothing Window" signify?</a>
				<div class="cd-faq-content">
					<p>The "EIC Smoothing Window" fits the smoothing algorithm in the selected number of scans.</p>
					<p>This parameter can be changed as follows:</p>
					<p>● Go to "Options"<img src="https://user-images.githubusercontent.com/29090291/42500500-1b27c36e-844f-11e8-8659-6e8cdbe18a38.png" alt="Options Icon" height="25" width="40"> in the top menu</p>
					<p>● Select the "Peak Detection" Tab</p>
					<p>● Select the value for "EIC Smoothing Window"</p><br />
					 <img src="https://user-images.githubusercontent.com/29090291/42503371-1a94db56-8456-11e8-8af2-f8e2fd68db19.png" alt="Eic smoothing window " height="250" width="600">
					

				</div> <!-- cd-faq-content -->
			</li>

			<li>
				<a class="cd-faq-trigger" href="#0">How does "Max Retention Time Difference Between Peaks" affect my data?</a>
				<div class="cd-faq-content">
					<p>This sets a limit to RT difference between peaks in a group. Increasing this value when alignment fails will center the peaks satisfactorily.</p>
					<p>This value can be changed by the following steps:</p>
					<p>● Go to "Options"<img src="https://user-images.githubusercontent.com/29090291/42500500-1b27c36e-844f-11e8-8659-6e8cdbe18a38.png" alt="Options Icon" height="25" width="40"> in the top menu</p>
					<p>● Select the "Peak Detection" Tab</p>
					<p>● Select the value for "Max Retention Time Difference Between Peaks"</p><br />
					 <img src="https://user-images.githubusercontent.com/29090291/42503574-a910cc46-8456-11e8-95a1-d5acc5cc26a5.png" alt="Maximum Retention Time Difference" height="250" width="600">
					

				</div> <!-- cd-faq-content -->
			</li>

			<li>
				<a class="cd-faq-trigger" href="#0">What does the "Minimum Signal Baseline Difference" parameter signify?</a>
				<div class="cd-faq-content">
					<p>The "Minimum Signal Baseline Difference" parameter sets the difference between Intensity and Baseline to detect any signal as a valid signal. To change this value,</p>
					<p>● Go to "Options"<img src="https://user-images.githubusercontent.com/29090291/42500500-1b27c36e-844f-11e8-8659-6e8cdbe18a38.png" alt="Options Icon" height="25" width="40"> in the top menu</p>
					<p>● Select the "Peak Filtering" tab</p>
					<p>● Select the "Minimum Signal Baseline Difference" value</p><br />
					 <img src="https://user-images.githubusercontent.com/29090291/42503771-31467d40-8457-11e8-93fb-956c403aa4cf.png" alt="Minimum Signal Baseline Difference" height="250" width="600">
					

				</div> <!-- cd-faq-content -->
			</li>

			<li>
				<a class="cd-faq-trigger" href="#0">What do the parameters in the Isotope Detection tab affect data?</a>
				<div class="cd-faq-content">
				<p><mark>Samples are Labeled?</mark></p>
				<p>● The labels that are present in the samples or that has to be detected should be checked.</p><br />
				<p><mark>The parameters in Filter Isotopic Peaks section are as follows:</mark></p>
					<p>● Minimum Isotope-Parent Correlation- Sets the minimum threshold for isotope-parent peak correlation. This correlation is a measure of how often they appear together.</p>
					<p>● Isotope is within [X] scans of parent- Sets the maximum scan difference between isotopic and parent peaks. This is a measure of how closely they appear together on the RT scale.</p>
					<p>● Maximum % Error to Natural Abundance- Sets the maximum natural abundance error expected. Natural abundance of an isotope is the expected ratio of the amount of isotope over the amount of parent molecule in nature. The error is the difference between observed and natural abundance as a fraction of natural abundance.</p>
					<p>● Correct for Natural C13 Isotope Abundance- Check the box to correct for natural C13 abundance.</p><br />
					<p>To change these parameters,</p>
					<p>● Go to "Options"<img src="https://user-images.githubusercontent.com/29090291/42500500-1b27c36e-844f-11e8-8659-6e8cdbe18a38.png" alt="Options Icon" height="25" width="40"> in the top menu</p>
					<p>● Select "Isotope Detection" tab</p>
					<p>● Change the parameters</p>
					

				</div> <!-- cd-faq-content -->
			</li>


			<li>
				<a class="cd-faq-trigger" href="#0">What do the parameters in Peak Grouping tab signify?</a>
				<div class="cd-faq-content">
				<p>The Peak Grouping tab has the parameters that calculate the <mark>Peak Grouping Score</mark> which determines if the peaks should be grouped together or not.</p><br />
				<p>The score depends on the following 3 parameters and their weights:</p>
                <p>a) RT difference or DistX- Difference in RT between the peaks under comparison. Closer peaks are assigned a higher score.</p>      
                <p>b) Intensity difference or DistY- Difference in intensity between peaks under comparison. Smaller difference accounts for a higher score.</p>
				<p>c) Overlap- Fraction of RT overlap between the peaks under comparison. Greater overlap accounts for a higher score.</p><br />
				<p>● Consider Overlap- Uncheck this box to calculate grouping score without overlap.</p>
				<p>● Sliders are provided to adjust the weights attached to each of the three parameters.</p><br />
				<p>To change the "Peak Grouping" parameters,</p>
					<p>● Go to "Options"<img src="https://user-images.githubusercontent.com/29090291/42500500-1b27c36e-844f-11e8-8659-6e8cdbe18a38.png" alt="Options Icon" height="25" width="40"> in the top menu</p>
					<p>● Click on "Peak Grouping" tab</p>
					<p>● Change the parameters</p>
					

				</div> <!-- cd-faq-content -->
			</li>


			<li>
				<a class="cd-faq-trigger" href="#0">What do the parameters in Group Ranking tab signify?</a>
				<div class="cd-faq-content">
				<p>Group rank is one of the parameters for group filtering. Peaks are ranked according to their quality.</p><br />
				<p>The score depends on the following 3 parameters and their respective weights A, B and C:</p>
                <p>i) Q or Group Quality- Maximum peak quality of a group. Peaks are assigned a quality score by a machine learning algorithm in El Maven. Better quality leads to a higher rank.</p>      
                <p>ii) I or Group Intensity- Maximum intensity of a group. Better intensity leads to a higher rank.</p>
				<p>iii) dRT or RT difference- Difference between expected RT and group mean RT.</p><br />
				<p>● Consider Retention Time- Check the box to use retention time while group rank calculation.</p>
				<p>● Quality Weight- Adjust the slider to set weight for group quality in group rank calculation.</p>
				<p>● Intensity Weight- Adjust the slider to set weight for group intensity in group rank calculation.</p>
				<p>● dRT Weight- Adjust the slider to set weight for RT difference in group rank calculation. The slider is disabled if Consider Retention Time is unchecked.</p><br />
				<p>To change the group rank parameters,,</p>
					<p>● Go to "Options"<img src="https://user-images.githubusercontent.com/29090291/42500500-1b27c36e-844f-11e8-8659-6e8cdbe18a38.png" alt="Options Icon" height="25" width="40"> in the top menu</p>
					<p>● Click on "Group Rank" tab</p>
					<p>● Change the parameters</p>
					<img src="https://user-images.githubusercontent.com/29090291/42505017-1e477a6a-845b-11e8-8e49-ec75600512e0.png" alt="Group Ranking Tab" height="300" width="610">
					

				</div> <!-- cd-faq-content -->
			</li>

		</ul> <!-- cd-faq-group -->

		<ul id="compound_database" class="cd-faq-group">
			<li class="cd-faq-title"><h2>Compound Database</h2></li>
			<li>
				<a class="cd-faq-trigger" href="#0">What type of compound database does El-MAVEN use?</a>
				<div class="cd-faq-content">
					<p>The compound database is made differently for different kinds of data. Some of the essential columns are the compound name, compound formula (necessary for labeled analysis) or m/z. To do targeted analysis one can also add RT column. You can also refer to these documents to understand more about the compounds DBs for MS and MS-MS data</p>
				</div> <!-- cd-faq-content -->
			</li>

			<li>
				<a class="cd-faq-trigger" href="#0">I do not see my database file in El-MAVEN. Why?</a>
				<div class="cd-faq-content">
					<p>The compound database needs to be specifically added to El-MAVEN. Given below are the steps to add a database to El-MAVEN:</p>
					<img src="https://user-images.githubusercontent.com/29090291/42505510-897daf06-845c-11e8-8ea3-a0d5537b1421.png
" alt="Group Ranking Tab" height="220" width="310" style="margin-left: 17%">
   <p>● Step 1: Select Compounds tab on the left panel</p>
   <p>●Step 2: Select Open and browse to your compound database file</p>
					
				</div> <!-- cd-faq-content -->
			</li>

		</ul> <!-- cd-faq-group -->

		<ul id="blank_samples" class="cd-faq-group">
			<li class="cd-faq-title"><h2>Blank Samples</h2></li>
			<li>
				<a class="cd-faq-trigger" href="#0">Some of my samples are black in colour. Why is that?</a>
				<div class="cd-faq-content">
					<p>If the sample file name has the word “blank" in it, El-MAVEN automatically marks them as a blank and the background colour is set to black. If these samples are not blanks, you can select them in the samples widget and click on the <img src="https://user-images.githubusercontent.com/29090291/42093200-21c09996-7bc9-11e8-9a61-0491f9912cd6.png" alt="Blank Sample Image" height="20" width="40"> "Blank" icon to unmark them. (Screenshot required)</p>
				</div> <!-- cd-faq-content -->
			</li>

			<li>
				<a class="cd-faq-trigger" href="#0">How to mark samples as blanks in ElMaven?</a>
				<div class="cd-faq-content">
					<p>Samples can be marked as blanks by selecting the sample and clicking  <img src="Blank_Icon.png" alt="Blank Sample Image" height="20" width="40">  "Blank" icon. The sample gets greyed out once it is marked as a blank.</p>
				</div> <!-- cd-faq-content -->
			</li>

		</ul> <!-- cd-faq-group -->

		<ul id="peak_grouping" class="cd-faq-group">
			<li class="cd-faq-title"><h2>Peak Grouping</h2></li>
			<li>
				<a class="cd-faq-trigger" href="#0">My data is not getting grouped properly. What should I do?</a>
				<div class="cd-faq-content">
					<p>Samples do not get grouped correctly because of a couple of reasons.</p><br />
<p>
First, you should check for alignment of your samples in the EIC. Misalignment is a common observation due to drift in retention time. If the samples seem to be misaligned, you should perform alignment through the following steps:</p><br />

<p>● Step 1: Click on “Align” <img src="https://user-images.githubusercontent.com/29090291/42505937-fc4db6e2-845d-11e8-8f90-9521803f4664.png" alt="Align Icon Icon" height="25" width="40"> in the top menu</p>
<p>● Step 2: Select an alignment algorithm (ElMaven provides Loess, Poly Fit, and OBI Warp alignment methods)</p>
<p>● Step 3: Click on “Align” in the popup</p>
<img src="https://user-images.githubusercontent.com/29090291/42505935-fad37d10-845d-11e8-9026-2319cb38f2e2.png" alt="Peak Grouping Alignment" height="250" width="600"><br />

<p>
If grouping issue persists, you can increase the peak grouping parameters in the “Options” menu.</p><br />


<p>In ElMaven, a peak grouping score is determined to decide whether a peak should be grouped together or not. The “Peak Grouping” tab in the “Options” menu gives the equation and the parameters for calculation of the score. You can tweak these parameters to improve grouping of your data.</p>
<br />

<p>If the issue still persists, you can increase the EIC Smoothing Window under “Options” menu in “Peak Detection” tab. Smoothing of data points helps in increasing the signal/noise ratio. There are three algorithms in ElMaven on the basis of which EIC Smoothing is done, namely: Savitzky-Golay, Gaussian and Moving Average.
</p><br />
<p>Check <a href="https://github.com/ElucidataInc/ElMaven/wiki/Introduction-to-ElMaven-UI" target="_blank">Documentation</a> for details.</p>
				</div> <!-- cd-faq-content -->
			</li>

			<li>
				<a class="cd-faq-trigger" href="#0">Why am I not getting enough groups in my data?</a>
				<div class="cd-faq-content">
					<p>The Compound Database is very small in size because of which we do not detect all groups in our samples.</p><br />
					<p>

Another possible could be high group filtering parameters which filter out even the good groups. To change these parameters, we can perform the following steps:
</p><br />
<p>● Go to “Peaks”<img src="https://user-images.githubusercontent.com/29090291/42505924-f523d2ca-845d-11e8-8c81-cc6beeea1567.png" alt="Align Icon Icon" height="25" width="40"> in the top menu</p>
<p>● Select “Group Filtering” tab</p>
<p>● Lower “Minimum Peak Intensity” values</p>
<img src="https://user-images.githubusercontent.com/29090291/42505931-f798d776-845d-11e8-8d93-059109d0ca3e.png" alt="Not Enough Groups" height="550" width="600"><br />
				</div> <!-- cd-faq-content -->
			</li>


            <li>
				<a class="cd-faq-trigger" href="#0">What does the parameter "minimum peak width" signify?</a>
				<div class="cd-faq-content">
					<br />
					<p>Peak width is equal to the number of scans that a peak is spread over. Groups with no peak widths above this threshold are filtered out. Spurious signals can be filtered out using this option.</p>
				</div> <!-- cd-faq-content -->
			</li>


			<li>
				<a class="cd-faq-trigger" href="#0">What does the parameter "minimum good peak/group" signify?</a>
				<div class="cd-faq-content">
					<br />
					<p>Minimum Good Peak/Group in a sample signifies the number of good peaks that should be present for a group to be accepted as a good peak.</p>
				</div> <!-- cd-faq-content -->
			</li>



			<li>
				<a class="cd-faq-trigger" href="#0">How does ElMaven detect the best group in any m/z?</a>
				<div class="cd-faq-content">
					<br />
					<p>ElMaven does this on the basis of group rank. Group rank is calculated using intensity and quality score. Quality score, in turn, is calculated using 9 different metrics of a peak.You can adjust the score calculation from the "Group Rank" tab in "Options" dialog.</p>
				</div> <!-- cd-faq-content -->
			</li>
		</ul> <!-- cd-faq-group -->

		<ul id="curating_peaks" class="cd-faq-group">
			<li class="cd-faq-title"><h2>Curating Peaks</h2></li>
			<li>
				<a class="cd-faq-trigger" href="#0">How to automatically detect peaks in El-MAVEN?</a>
				<div class="cd-faq-content">
					<br />
					<p>● Select “Peaks”<img src="https://user-images.githubusercontent.com/29090291/42505924-f523d2ca-845d-11e8-8c81-cc6beeea1567.png" alt="Align Icon Icon" height="25" width="40"> on the top menu</p>
					<p>● Step2: You can select "Automatic Features Detection" or "Compound Database Search" </p>
					<p>● Step3: Select "Find Peaks"</p><br />
					<img src="https://user-images.githubusercontent.com/29090291/42507794-07cc8966-8464-11e8-8bfc-02c6dff37bea.png" alt="Automatic Detect Peaks" height="450" width="600" ><br />
					<p>The peak table shows the list of groups detected. Automatic Curation of Peaks selects high-quality groups with high intensities.</p>



				</div> <!-- cd-faq-content -->
			</li>

			<li>
				<a class="cd-faq-trigger" href="#0">How can we manually curate peaks in ElMaven?</a>
				<div class="cd-faq-content">
					<p>● To use manual curation using the compound DB widget, the user has to iterate over all the compounds in the compound DB.</p>

<p>● Once on a compound, ElMaven shows the highest ranked group for that M/Z. The user can now choose a group or reject it. There are two ways to do this.</p>

<p>● In the first workflow, the user needs to double click on the peak group of his choice. This will get the Rt line to the median of the group and also add the metabolite to the bookmarks table.</p><p>● In the second workflow, the user can "Shift"+Drag on the peak they want to add to their bookmark table.</p>
				</div> <!-- cd-faq-content -->
			</li>

			<li>
				<a class="cd-faq-trigger" href="#0">Why can’t I see the peaks in my data?</a>
				<div class="cd-faq-content">
					<p>Peaks might not be displayed for any sample because of multiple reasons.</p><br />
					<p><mark>● Conversion issue:</mark> The raw files from the experiment were not converted properly to the required .mzXML or .mzML format. Sometimes conversion using 32bit MSConvert gives an issue. Check <a href="https://github.com/ElucidataInc/ElMaven/wiki/Introduction-to-ElMaven-UI" target="_blank">Documentation</a> for correct parameters to be used while conversion.</p>
					<p><mark>● Incorrect ppm values:</mark> The ppm values can be very high or low as compared to what was used in the experiment. This leads to the peaks not getting detected. Select proper ppm value in the Top Left menu or while Peak Detection through Compound Database Search.</p>
					<p><mark>● Incorrect polarity settings:</mark> Sometimes peaks are not detected because of incorrect polarity settings. This can be changed by selecting “Options” in the top menu and changing “Polarity/Ionization Mode” under the “Instrumentation” tab</p>
					<p><mark>● Unit mismatch:</mark> ElMaven works with the monoisotopic mass unit, whereas mass spectrometry machines give an output in the atomic mass unit. Due to this unit mismatch, we have to raise the ppm value to negate the mismatch and detect peaks.</p>
				</div> <!-- cd-faq-content -->
			</li>

			<li>
				<a class="cd-faq-trigger" href="#0">What parameters can I change to get good peaks for LCMS/MS data?</a>
				<div class="cd-faq-content">
					<p>LC-MS/MS data generally has peak intensities lower than that of LC-MS or GC-MS data so we should typically use intensity cutoffs which are in the range of 1000 to 10000. Along with this, we should use the model_QQQ instead of the generic model for better quality scores.</p>
				</div> <!-- cd-faq-content -->
			</li>



			<li>
				<a class="cd-faq-trigger" href="#0">What parameters can I change to get good peaks for GCMS data?</a>
				<div class="cd-faq-content">
					<p>GCMS data often requires high PPM range as compared to LC-MS data. Thus we would suggest using high ppm ranges (order of magnitude 100) to detect the correct peaks in GCMS.</p>
				</div> <!-- cd-faq-content -->
			</li>



			<li>
				<a class="cd-faq-trigger" href="#0">What are good peaks?</a>
				<div class="cd-faq-content">
					<p>A good peak can be defined to have the following properties:</p><br />
					<p>● Gaussian Shape</p>
					<p>● Perfect grouping</p>
					<p>● Narrow Retention Time</p>
					<p>● Good Sample Intensities</p>
					<p>● Low Blank Intensities</p>
					<p>● A similar trend between observable standards and other samples</p>
				</div> <!-- cd-faq-content -->
			</li>


			<li>
				<a class="cd-faq-trigger" href="#0">What are bad peaks?</a>
				<div class="cd-faq-content">
					<p>Bad Peaks can be defined to show the following properties:</p><br />
					<p>● The Peaks do not have Gaussian Shape</p>
					<p>● Peaks are not grouping well</p>
					<p>● Standard samples have intensities very high as compared to other samples</p>
					<p>● The Samples show intensities lower or roughly equal to the blank samples indicating noisy peaks</p>
					<p>● Peaks show very low intensity</p>
				</div> <!-- cd-faq-content -->
			</li>



			<li>
				<a class="cd-faq-trigger" href="#0">What does the background color in the bookmark/peak table behind the good/bad marked peaks signify?</a>
				<div class="cd-faq-content">
					<p>This feature takes advantage of the group quality score to decide whether the group has been correctly marked ‘good’ or ‘bad’. Darker the shade of red implies worse is the curation and should be considered as a bad peak. </p><br />
					<p>Here the peak curated for UTP is a bad group and should be marked as bad ideally.</p>
					<img src="https://user-images.githubusercontent.com/29090291/42093201-21f149f6-7bc9-11e8-9584-153f178ccb2f.png" alt="Bookmark Peak Table" height="140" width="280" style="margin-top:2%"><br />
				</div> <!-- cd-faq-content -->
			</li>
		</ul> <!-- cd-faq-group -->


         
   


`	


    	<ul id="isotope_detection" class="cd-faq-group">
			<li class="cd-faq-title"><h2>Isotope Detection</h2></li>
			<li>
				<a class="cd-faq-trigger" href="#0">Can we perform untargeted data analysis in El-MAVEN? How?</a>
				<div class="cd-faq-content">
				<p>Isotopologues/ Labels are not detected in any data due to the following reasons:</p><br />
				<p> <mark>● Report Isotopic Peaks is disabled:</mark> If Report Isotopic Peaks is disabled, during peak detection, the labels are not detected. Make sure the “Report Isotopic Peaks” field is selected after you have selected "Peaks"  on the top menu</p>
                <img src="https://user-images.githubusercontent.com/29090291/42508200-4a876bd0-8465-11e8-9b74-8fb7dfad6df7.png" alt="Report Isotopic Peaks" height="550" width="600" >
				<p> <mark>● High Peak Filtering Settings:</mark> If minimum signal baseline difference and minimum peak quality are too high, the labels are not detected. This can be changed by going to “Options” in the top menu, selecting the “Peak Filtering” tab and tweaking the parameters.</p>
         <p><mark>● Select Labels:</mark> Labels are not detected if the isotopic labels are not selected. Go to “Options”<img src="https://user-images.githubusercontent.com/29090291/42500500-1b27c36e-844f-11e8-8659-6e8cdbe18a38.png" alt="Options Icon" height="25" width="40">, select “Isotope Detection” and select the labels you want to detect in your samples.</p>
		 
		 <img src="https://user-images.githubusercontent.com/29090291/42511454-90318d50-846f-11e8-925a-428030f9f4ae.png" alt="Select Isotope" height="250" width="600">
         <p><mark>● High Isotope-Parent Peak Correlation value: </mark>Set the minimum threshold for isotope-parent peak correlation. This correlation is a measure of how often they appear together. To change this,</p>
         <p>	        1. Go to “Options”<img src="https://user-images.githubusercontent.com/29090291/42500500-1b27c36e-844f-11e8-8659-6e8cdbe18a38.png" alt="Options Icon" height="25" width="40"></p>
         <p>         2. Select “Isotope Detection” tab</p>
         <p>         3. Change the “Minimum Isotope-Parent Peak Correlation” parameter</p>
                   <br />
         <p><mark>● Narrow scan range :</mark> If the number of scans of the parent within which Isotope has to be detected is set very low, the labels do not get detected. To change this,</p>
          <p>1.Go to “Options”<img src="https://user-images.githubusercontent.com/29090291/42500500-1b27c36e-844f-11e8-8659-6e8cdbe18a38.png" alt="Options Icon" height="25" width="40"></p>
          <p>2.Select “Isotope Detection” tab</p>
          <p>3.Change the “Isotope is within [X] scans of parent” parameter</p>





				
				</div> <!-- cd-faq-content -->
			</li>
	</ul>



    <ul id="untargeted" class="cd-faq-group">
			<li class="cd-faq-title"><h2>Untargeted Data Analysis</h2></li>
			<li>
				<a class="cd-faq-trigger" href="#0">Can we perform untargeted data analysis in El-MAVEN? How?</a>
				<div class="cd-faq-content">
					<p>	Yes. Untargeted data analysis can be done in El-MAVEN through automated peak detection. For Untargeted Analysis perform the following steps:</p>
					<br/>
					<p>● Step 1: Upload samples</p>
					<p>● Step 2: Go to "Peaks"<img src="https://user-images.githubusercontent.com/29090291/42505924-f523d2ca-845d-11e8-8c81-cc6beeea1567.png" alt="Align Icon Icon" height="25" width="40">  on the top menu </p>
					<p>● Step 3: Select the tab "Feature Detection Selection"</p>
					<p>● Step 4: Mark "Automated Feature Detection" as Check. Set the parameters here as per requirement.</p>
					<p>● Step 5: Click on "Find Peaks"</p>
				</div> <!-- cd-faq-content -->
			</li>
	</ul>`


	 <ul id="baseline_calculation" class="cd-faq-group">
			<li class="cd-faq-title"><h2>Baseline Calculation</h2></li>
			<li>
				<a class="cd-faq-trigger" href="#0">How to automatically detect peaks in El-MAVEN?</a>
				<div class="cd-faq-content">
					<p>	If you set “Droptop x% intensities from chromatogram” to 80%:</p>
					<br/>
					<p>● El-MAVEN will sort the intensity vector from lowest to the highest intensity</p>
					<p>● Intensity at the (100-80) = 20% mark will be set as the baseline cut-off value</p>
					<p>● Baseline intensity is the same as signal intensity for all values below the cut-off. The baseline for higher signal intensity is set at the cut-off value.</p>
					<p>● Baseline smoothing is done using Gaussian smoothing algorithm.</p><br />
					<p>Basically, the top x% of the intensity points are dropped. If your data has a high noise level, set this parameter to a lower percentage.</p>
				</div> <!-- cd-faq-content -->
			</li>
	</ul>
 
	</div> <!-- cd-faq-items -->
	<a href="#0" class="cd-close-panel">Close</a>
</section> <!-- cd-faq -->
<script src="js/jquery-2.1.1.js"></script>
<script src="js/jquery.mobile.custom.min.js"></script>
<script src="js/main.js"></script> <!-- Resource jQuery -->





