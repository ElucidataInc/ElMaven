
class paths:
	def __init__(self):

		self.pathPeakDetector = '/home/sahil/elucidata/maven/mavenOpenSource/bin/peakdetector'
		self.db = '/home/sahil/code/qe/maven_data/CDatabases/qe3_v11_2016_04_13.tab.txt'
		self.model = '/home/sahil/code/qe/maven_data/Models/default.model'
		self.outputdir = '/home/sahil/code/qe/maven_data/Experiments/maven769test-1469542445.7/output'
		self.inputdir = '/home/sahil/code/qe/maven_data/Datasets/alignmenttest-1469088471.6'

paths = paths()

class constants:
	def __init__(self):

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

constants = constants()
