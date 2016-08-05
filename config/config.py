
class paths():

	def __init__(self, build_maven):

		#For build 769
		if build_maven == 769:
			self.path_peakdetector = '/home/sahil/elucidata/maven/mavenOpenSource/bin/peakdetector'
			self.outputdir = '/home/sahil/Desktop/769'

		#For build 776
		if build_maven == 776:
			self.path_peakdetector = '/home/sahil/code/qe/maven_stable/bin/peakdetector'
			self.outputdir = '/home/sahil/Desktop/776'

		#For all builds
		self.inputdir = '/home/sahil/Desktop/Dataset'
		self.db = '/home/sahil/code/qe/maven_data/CDatabases/qe3_v11_2016_04_13.tab.txt'
		self.model = '/home/sahil/code/qe/maven_data/Models/default.model'


class constants():

	def __init__(self, build_maven):

		#For build 769
		if build_maven == 769:
			pass
		
		#For build 776
		if build_maven == 776:
			
			self.minPrecursorCharge = 0
			self.pullIsotopes = 0
		
		#For all builds
		self.minGoodGroupCount = 1
		self.eicMaxGroups = 5	
		self.savemzroll = 1
		self.minQuality = 0.5
		self.ppmMerge = 10
		self.minSignalBaseLineRatio = 2
		self.grouping_maxRtWindow = 0.5
		self.alignSamples = 0
		self.minGroupIntensity = 10000
		self.matchRtFlag = 0
		self.minPeakWidth = 5
		self.minGoodGroupCount = 3
		self.eicSmoothingWindow = 12
		self.rtStepSize = 10

class runFunction():

	def __init__(self):

		


class variables():

	def __init__(self):

		#	By default process Compounds (Compound Database Search)
		# 	1 for processCompounds 	(Compound Database Search)
		#	2 for processMassSlices	(Automatic Peak Detection)
		#	3 for processCompounds with pullIsotopes (includes Reporting isotopic Peaks)
		#	4 for processMassSlices with pullIsotopes (includes Reporting isotopic Peaks)
		self.runFunction = 1

		self.path_compare_compounds_csv = '/home/sahil/Desktop/comparison/compare_compounds.csv'
		self.path_r2_values_csv = '/home/sahil/Desktop/comparison/r2_values.csv'
		self.path_csv_with_diff_rt_mz = '/home/sahil/Desktop/comparison/diff_rt_mz_values.csv'
		self.path_summary_of_results = '/home/sahil/Desktop/comparison/summary_of_results.docx'
variables = variables()