import pandas as pd
import numpy as np
from scipy.interpolate import interp1d
from sys import stdin
import sys
import json
import statsmodels.api as sm
import warnings


input_json = ''

for line in iter(sys.stdin.readline, ''):

    if "start processing" in line:
            continue

    if "end processing" in line:

        json_obj = json.loads(input_json)
        processedData = processData(json_obj)

        print processedData
        sys.stdout.flush()

        print "stop"
        sys.stdout.flush()

    input_json += line
