
import os
import runPeakdetector
import compareOutput
from helper import helper
from config import config

listBuilds = [769, 776]

runPeakdetector.run(listBuilds)

compareOutput.compare(listBuilds)