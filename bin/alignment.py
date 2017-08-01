import pandas as pd
import numpy as np
from scipy.interpolate import interp1d
from sys import stdin
import sys
import json
import statsmodels.api as sm
import warnings


input_json = ''


def processData(json_obj):

    minFraction = 0.9
    extraPeaks = 1
    span = 0.2

    groups_data = json_obj["groups"]
    samples_data = json_obj["rts"]

    groups_rt = pd.DataFrame()

    for g in groups_data.keys():
            for s in range(len(groups_data[g])):
                groups_rt = groups_rt.append(pd.DataFrame({'group': g, 'sample': groups_data[g][s].keys(), 'rt': groups_data[g][s][groups_data[g][s].keys()[0]]}))

    samples_rt = pd.DataFrame()

    for g in samples_data.keys():
            for s in range(len(samples_data[g])):
                    samples_rt = samples_rt.append(pd.DataFrame({'sample': g, 'rt': samples_data[g][s]}, index=[0]))

    samples = samples_rt['sample'].unique()
    nSamples = len(samples)
    group_num = groups_rt.group.unique()
    minSample = nSamples * minFraction

    lim_groups = pd.DataFrame()
    for i in group_num:
            sub_groups = groups_rt[groups_rt.group == i]
            sub_grp_samp = sub_groups['sample'].unique()
            nsamp = len(sub_grp_samp)
            if nsamp >= minSample:
                    flag = False
                    for j in sub_grp_samp:
                            sample_groups = sub_groups[sub_groups['sample'] == j]
                            row, col = sample_groups.shape
                            if row > extraPeaks + 1:
                                    flag = True
                    if(flag is False):
                            sub_groups = sub_groups.assign(rt_dev=(sub_groups.rt - np.median(sub_groups.rt)))
                            lim_groups = lim_groups.append(sub_groups, ignore_index=True)

    corr_group_rts = dict()
    corr_sample_rts = dict()

    for k in samples:

        group_samp = lim_groups[lim_groups['sample'] == k]
        lowess = sm.nonparametric.lowess(group_samp.rt_dev, group_samp.rt, frac=span)
        lowess_x = list(zip(*lowess))[0]
        lowess_y = list(zip(*lowess))[1]
        f = interp1d(lowess_x, lowess_y, bounds_error=False, fill_value='extrapolate')
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
