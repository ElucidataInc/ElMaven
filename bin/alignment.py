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

lim_groups = pd.DataFrame()
for i in group_num:
	sub_groups = groups[groups.group_number == i]
	sub_grp_samp = groups.sample_name.unique()
	nsamp = len(sub_grp_samp)
	if nsamp >= minSample:
		flag = False
		for j in sub_grp_samp:
			sample_groups = sub_groups[sub_groups.sample_name == j]
			row,col = sample_groups.shape
			if row > extraPeaks + 1 :
				flag = True
		if(flag == False):
			sub_groups = sub_groups.assign(rt_dev = (sub_groups.rt - np.median(sub_groups.rt)))
			lim_groups = lim_groups.append(sub_groups, ignore_index = True)