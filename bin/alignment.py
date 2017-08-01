import pandas as pd
import numpy as np
from scipy.interpolate import interp1d
from sys import stdin
import sys
import json
import statsmodels.api as sm
import warnings

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

corr_rts = pd.DataFrame()
for k in samples:
	group_samp = lim_groups[lim_groups.sample_name == k]
	lowess = sm.nonparametric.lowess(group_samp.rt_dev, group_samp.rt, frac=span)
	lowess_x = list(zip(*lowess))[0]
	lowess_y = list(zip(*lowess))[1]
	f = interp1d(lowess_x, lowess_y, bounds_error=False)
	sub_rts = groups[groups.sample_name == k]
	rt_dev_new = f(sub_rts.rt)
	abs_rt_dev_new = abs(rt_dev_new)
	rt_fit_dev = pd.DataFrame({'group_name':sub_rts.group_name,'rt': sub_rts.rt,'rt_dev_new':rt_dev_new.tolist(), 'abs_rt_dev_new':abs_rt_dev_new.tolist()})
	cutoff = abs(rt_fit_dev.rt_dev_new).quantile(0.9) * 2
	rt_fit_dev[rt_fit_dev > cutoff].rt_dev_new= np.nan
	no_na_rts = rt_fit_dev[pd.notnull(rt_fit_dev["rt_dev_new"])]
	f = interp1d(no_na_rts.rt, no_na_rts.rt_dev_new, bounds_error=False)
	rt_fit_dev[pd.isnull(rt_fit_dev["rt_dev_new"])].rt_dev_new = f(rt_fit_dev[pd.isnull(rt_fit_dev["rt_dev_new"])].rt)
	sub_rts.rt = sub_rts.rt - rt_fit_dev.rt_dev_new
	corr_rts = corr_rts.append(sub_rts, ignore_index = True)

corr_rts.to_csv("corrected_rts.csv", sep = ",")

input_json = ''

