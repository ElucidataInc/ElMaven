import pandas as pd
import numpy as np
from scipy.interpolate import interp1d
import statsmodels.api as sm

groups = pd.read_csv('groups.csv')
rts = pd.read_csv('rts.csv')
minFraction = 0.9
extraPeaks = 1
span = 0.2
samples = rts.sample_name.unique()
nSamples = len(samples)
group_num = groups.group_number.unique()
minSample = nSamples * minFraction