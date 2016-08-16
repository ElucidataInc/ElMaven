import time
import datetime

class input_paths():

	def __init__(self, build_maven):

		#For build 769
		if build_maven == 769:
			self.path_peakdetector = '/home/sahil/elucidata/maven/mavenOpenSource/bin/peakdetector'

		#For build 776
		if build_maven == 776:
			self.path_peakdetector = '/home/sahil/code/qe/maven_stable/bin/peakdetector'

		#For all builds
		self.inputdir = '/home/sahil/Desktop/Dataset'


class arguments():

	def __init__(self, build_maven):

		#For build 769
		if build_maven == 769:
			self.outputdir = '/home/sahil/Desktop/769'

		#For build 776
		if build_maven == 776:
			self.outputdir = '/home/sahil/Desktop/776'
			self.minPrecursorCharge = 0
			self.pullIsotopes = 0

		#For all builds
		self.db = '/home/sahil/code/qe/maven_data/CDatabases/qe3_v11_2016_04_13.tab.txt'
		self.model = '/home/sahil/code/qe/maven_data/Models/default.model'
		self.eicMaxGroups = 5	
		self.savemzroll = 1
		self.minQuality = 0.5
		self.ppmMerge = 10
		self.minSignalBaseLineRatio = 2
		self.grouping_maxRtWindow = 0.5
		self.alignSamples = 0
		self.minGroupIntensity = 100000
		self.matchRtFlag = 0
		self.minPeakWidth = 5
		self.minGoodGroupCount = 3
		self.eicSmoothingWindow = 12
		self.rtStepSize = 10
		self.processAllSlices = 0

class variables():

	def __init__(self):

		#	By default process Compounds (Compound Database Search)
		# 	1 for processCompounds 	(Compound Database Search)
		#	2 for processMassSlices	(Automatic Peak Detection)
		#	3 for processCompounds with pullIsotopes (includes Reporting isotopic Peaks)
		#	4 for processMassSlices with pullIsotopes (includes Reporting isotopic Peaks)
		self.run_function = 1

		ts = time.time()
		self.st = \
		datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d_%H:%M:%S')

		self.csv_compare_output = '/home/sahil/Desktop/comparison/'
		self.csv_compare_compounds = 'compare_compounds.csv'
		self.csv_r2_values = 'r2_values.csv'
		self.csv_with_diff_rt_mz = 'diff_rt_mz_values.csv'
		self.csv_summary_of_results = 'summary_of_results.docx'
		self.plots_comparison = '/home/sahil/Desktop/plots'

		self.test_func_list = ['compoundDatabaseSearch']

variables = variables()
