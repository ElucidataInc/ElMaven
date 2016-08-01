
class paths():

	def __init__(self, buildMaven):

		#For build 769
		if buildMaven == 769:
			self.pathPeakDetector = '/home/sahil/elucidata/maven/mavenOpenSource/bin/peakdetector'
			self.outputdir = '/home/sahil/Desktop/769'

		#For build 776
		if buildMaven == 776:
			self.pathPeakDetector = '/home/sahil/code/qe/maven_stable/bin/peakdetector'
			self.outputdir = '/home/sahil/Desktop/776'

		#For all builds
		self.inputdir = '//home/sahil/Desktop/deleteIT'
		self.db = '/home/sahil/code/qe/maven_data/CDatabases/qe3_v11_2016_04_13.tab.txt'
		self.model = '/home/sahil/code/qe/maven_data/Models/default.model'


class constants():

	def __init__(self, buildMaven):

		#For build 769
		if buildMaven == 769:
			pass
		
		#For build 776
		if buildMaven == 776:
			self.savemzroll = 1
			self.minPrecursorCharge = 0
			self.pullIsotopes = 0
			self.eicMaxGroups = 20
			self.minGoodGroupCount = 3
		
		#For all builds
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
