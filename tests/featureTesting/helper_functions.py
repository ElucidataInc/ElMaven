import os
from heapq import nsmallest
import itertools
import json
import math
import shutil
import subprocess
import types
from math import log
from urlparse import urlparse

import numpy as np
import pandas as pd
import plotly.graph_objs as go
import xmltodict
from plotly.offline import plot
from posixpath import basename, dirname
from scipy.stats.stats import pearsonr
from scipy.stats import wilcoxon

import constants as cs


def xml_to_dict(path):
    """
        This function reads a xml file for any dataset and converts that to a dict.

        :param path: path of the xml data file

        :return: returns a dict having the data read from the xml file

        """
    with open(path) as fd:
        doc = xmltodict.parse(fd.read().encode(cs.ASCII))
    return doc


def diff_between_two_dicts(dict1, dict2):
    """
    This function returns the difference between two dictionaries.
    :param dict1: this dict is has setting parameters for first dataset.
    :param dict2: this dict has setting parameters for second dataset.
    :return: It is a dict which has difference between two dicts. This dict only has keys which has
    Non-zero
    difference between the two input dicts.
    """
    keys_list = dict1.keys()
    temp_dict = {}
    final_dict = {}
    for item in keys_list:
        try:
            temp_dict[item] = float(dict1[item]) - float(dict2[item])
        except:
            pass
        else:
            new_key = item.split(cs.SPLIT_CHARACTER)[1].encode(cs.ASCII)
            temp_dict[new_key] = temp_dict.pop(item)
            if temp_dict[new_key] != 0:
                final_dict[new_key] = [dict1[item].encode(cs.ASCII), dict2[item].encode(cs.ASCII)]
    return final_dict


def dict_diff(path1, path2):
    """
        This function returns the difference between the parameters file of both the datasets.

        :param path1: file path of dataset one
        for example : "/Users/deepaksharma/Downloads/dataset_one.json"

        :param path2:file path of dataset two
        for example : "/Users/deepaksharma/Downloads/dataset_two.json"

        :return: a dict of which keys are parameter names and values are lists which has different
        values in both the datasets.
        for example : {'compoundRTWindow': ['0.5', '0.3'], 'baseline_smoothingWindow': ['6', '4'],
         'minSignalBlankRatio': ['0', '3'], 'maxRt': ['20', '2'], 'ppmMerge': ['20', '2']}

        """
    dict1 = xml_to_dict(path1)[cs.SETTINGS][cs.PEAK_DETECTION_SETTINGS]
    dict2 = xml_to_dict(path2)[cs.SETTINGS][cs.PEAK_DETECTION_SETTINGS]
    final_dict = diff_between_two_dicts(dict1, dict2)
    if len(final_dict) == 0:
        return cs.DICT_DIFF_RETURN_MESSAGE

    else:
        return final_dict


def json_parser(path):
    """
        This function reads a json file and returns a object.

        :param path: file path of the data file
        for example : "/Users/deepaksharma/Downloads/data.json"

        :return: object of the data

        """
    with open(path) as data_file:
        data = json.load(data_file)
        return data


def get_samples_data(group, group_key):
    """
    This function gives the details of all the samples present in a group.
    :param group: It is a dict which has key as group_key and it's value for that key has details
    about that group.
    :param group_key: it is the key present in the group dictionary.
    :return: returns a dict which has details of all the samples present in that dict as key value
    pairs.
    """
    samples_dict = {}
    for i in range(len(group[group_key])):
        sample_details = group[group_key][i]
        sample_dict = {}
        sample_name = sample_details[cs.SAMPLE_NAME]
        if isinstance(sample_details, dict):
            for sample_key in sample_details.keys():
                if not isinstance(sample_details[sample_key], dict):
                    if sample_key != cs.SAMPLE_NAME:
                        sample_dict[sample_key] = sample_details[sample_key]

        samples_dict[sample_name] = sample_dict
    return samples_dict


def json_comp_data(path):
    """
        This function reads a json file for a dataset and returns a dict having keys as groupId and
        values as dict( this dict has all information (except information about eic) as key value
        pairs for that particular group).
        :param path: path of the data file
        :return: A dict which has all the details (except information about eic data) for all the
        groups in that json file.
        """
    data = json_parser(path)
    key = data.keys()[0]
    data_list = data[key]
    comp_dict = {}
    for group in data_list:
        group_dict = {}
        for group_key in group.keys():
            if isinstance(group[group_key], dict):
                for compound_key in group[group_key].keys():
                    group_dict[compound_key] = group[group_key][compound_key]

            elif isinstance(group[group_key], list):
                samples_dict = get_samples_data(group, group_key)
                group_dict[group_key] = samples_dict
            else:
                group_dict[group_key] = group[group_key]
        comp_dict[group[cs.GROUP_ID]] = group_dict
    return comp_dict


def scatterplot(list_man, list_auto, hover_text):
    """
        This function plots a scatter plot between intensities for all the samples from both the
        data sets.
         for example: manual and automated curated datasets

        :param list_man: List of intensity values for dataset one (for example: manually curated
        dataset)

        :param list_auto: List of intensity values for dataset two (for example: automatically
        curated dataset)

        :param hover_text: It is a list of text (in our case it is respective sample names for the
        intensity values )
        for each data point in scatter plot between intensities from both the datasets.

        :return: returns a plot object without layout
        """
    random_x = list_auto
    random_y = list_man
    hover_text_intensity = []
    for i in range(len(hover_text)):
        details = cs.SAMPLE_TEXT + str(hover_text[i]) + cs.AUTOMATED_INTENSITY_TEXT + \
                  str(random_x[i]) + cs.MANUAL_INTENSITY_TEXT + str(random_y[i])
        hover_text_intensity.append(details)
    trace = go.Scatter(x=random_x, y=random_y, mode=cs.MARKERS, text=hover_text_intensity,
                       hoverinfo=cs.TEXT)
    data = [trace]
    return data


def plot_layout(plot_object, title, x_title, y_title, plot_file_name):
    """
            This function adds layout to the plot object returned by scatterplot() function.

            :param plot_object: plot object for scatter plot returned by scatterplot() function.

            :param title: Title of the plot

            :param x_title: X-axis title of the plot

            :param y_title: Y-axis title of the plot

            :return: returns a plot after adding layout to it

    """
    layout = go.Layout(
        title=title,
        xaxis=dict(title=x_title),
        yaxis=dict(title=y_title),
        showlegend=False

    )
    figure = dict(data=plot_object, layout=layout)
    output_plot = plot(figure, filename="".join([plot_file_name, ".html"]), auto_open=False)
    return output_plot


def unique_identifier_data(unique_identifier_list, data_dict):
    """
        This function finds a unique group from the dict which has parameter values as listed in the
         argument unique_identifier_list.

        :param unique_identifier_list: It is a list of two lists. First list has names of parameters
         which will be used to identify an unique group from input json file and second list has
         their respective values.
        for example : [["compoundId","groupId","rtmin"],["citrate",175,10.79271603]]

        :param data_dict: a dict having keys as groupId and values as dict( this dict has all
        information as key value pairs for that particular group)

        :return: returns a dict having key as compound name for that unique group and value as a
        dict ( this dict has all information as key value pairs for that particular group).

        """
    unique_dict = {}
    parameters_name = unique_identifier_list[0]
    parameters_value = unique_identifier_list[1]
    if len(parameters_name) == len(parameters_value):
        for i in range(len(data_dict)):
            k = 0
            for j in range(len(parameters_name)):
                if (parameters_name[j], parameters_value[j]) in data_dict[i + 1].items():
                    k = k + 1
            if k == len(parameters_name):
                unique_dict[data_dict[i + 1][cs.COMPOUND_NAME]] = data_dict[i + 1]
    return unique_dict


def get_samples_data_with_eic(group, group_key):
    """
        This function gives the details of all the samples present in a group including information
        about EICs.
        :param group: It is a dict which has key as group_key and it's value for that key has
        details about that group.
        :param group_key: it is the key present in the group dictionary.
        :return: returns a dict which has details of all the samples present in that dict as key
        value pairs.
        """

    samples_dict = {}
    for i in range(len(group[group_key])):
        sample_details = group[group_key][i]
        sample_dict = {}
        sample_name = sample_details[cs.SAMPLE_NAME]
        if isinstance(sample_details, dict):
            for sample_key in sample_details.keys():
                if sample_key != cs.SAMPLE_NAME:
                    sample_dict[sample_key] = sample_details[sample_key]

        samples_dict[sample_name] = sample_dict
    return samples_dict


def json_eic_parser(path):
    """
        This function reads a json file for a dataset and returns a dict having keys as groupId and
        values as dict ( this dict has all information (incuding information about eic data) as
        key value pairs for that particular group).

        :param path: path of the data file
        for example : "/Users/deepaksharma/Downloads/data.json"

        :return: A dict which has all the details (including information about eic data) for all the
         groups in that json file.
        It's keys are group ids and values are dict having all information about that group as key
        value pairs.

        """
    data = json_parser(path)
    key = data.keys()[0]
    data_list = data[key]
    comp_dict = {}
    for group in data_list:
        group_dict = {}
        for group_key in group.keys():
            if isinstance(group[group_key], dict):
                for compound_key in group[group_key].keys():
                    group_dict[compound_key] = group[group_key][compound_key]

            elif isinstance(group[group_key], list):
                samples_dict = get_samples_data_with_eic(group, group_key)
                group_dict[group_key] = samples_dict
            else:
                group_dict[group_key] = group[group_key]

        comp_dict[group[cs.GROUP_ID]] = group_dict
    return comp_dict


def lineplot(intensity_data_list, rt_value_list, hover_text, group_name, plot_file_name):
    """
        This function plots a line plot between intensities and respective rt values for all the
        sample of a unique group.

        :param intensity_data_list: It is a list of lists.
        Each list in this list of lists is w.r.t a sample which has intensity values for across all
        rt values in that sample.

        :param rt_value_list: It is a list of lists.
        Each list in this list of lists is w.r.t a sample which has rt values for all intensity
        values in that sample.

        :param hover_text: It is a list of sample names. Sample names from this list will be used as
         names for their respective line plot.

        :param group_name: Name of the compound

        :return: returns a line plot

        """
    data = []
    if len(intensity_data_list) == len(rt_value_list):
        for i in range(len(intensity_data_list)):
            random_x = rt_value_list[i]
            random_y = intensity_data_list[i]
            trace = go.Scatter(x=random_x, y=random_y, mode=cs.LINES, name=hover_text[i])
            data.append(trace)
    line_plot = plot_layout(data, group_name, cs.SAMPLE_TIME, cs.CAPS_INTENSITY, plot_file_name)
    return line_plot


def unique_label_dict(unique_identifier_list, data_dict):
    unique_label_dict = {}
    for key in data_dict.keys():
        unique_label_list = []
        for identifier in unique_identifier_list:
            unique_label_list.append(str(data_dict[key][identifier]))
        unique_label = "_".join(unique_label_list)
        unique_label_dict[unique_label] = data_dict[key]
    return unique_label_dict


def add_comparater_to_group(comparater_list, data_dict):
    """
    It adds a key value pair(for example:- comparatername : Citrate Label-1 2.8745) to each value
    in the data_dict dictionary.
    :param comparater_list: It is a list of parameters using which we make the comparater name
    for each key in data_dict.
    :param data_dict: dict of dicts which has all the data extracted from the json file.
    :return: It is a dict which now has newly added comparatername and and it's value for that key.
    """
    for key in data_dict.keys():
        key_comaprater_list = []
        for comparater in comparater_list:
            key_comaprater_list.append(str(data_dict[key][comparater]))
        comparater_name = "_".join(key_comaprater_list)
        data_dict[key][cs.COMPARATER_NAME] = comparater_name
    return data_dict


def get_comparater_list(data_dict):
    """
    This function gives appends all the comparater name for each key in the input dict and gives a
    list having unique comparater names.
    :param data_dict: It is a dict which has comparater name for each key. This comparater name is
    present in the value for each key in the dict.

    :return: It is a list which has unique comparater names from all the keys present in the
    data_dict.
    """
    comparater_list = []
    for key in data_dict.keys():
        comparater_list.append(data_dict[key][cs.COMPARATER_NAME])
    unique_comparater_list = list(set(comparater_list))
    return unique_comparater_list


def get_comparater_name_dict(comparater_name, data_dict):
    """
    This function renturns the only key value pairs in the data_dict which has the comparater_name.
    :param comparater_name: It is combination of some parameter for a element in the dict. let's
    say if we want all the key value pairs which has the comparater_name equal to
    "Citrate label-1 2.893".
    :param data_dict: dict which has all the groups as key value pairs for a Dataset.
    :return: It is a dict which has keys which have the input comparater_name in their value.
    """
    comparater_name_dict = {}
    for key in data_dict.keys():
        if data_dict[key][cs.COMPARATER_NAME] == comparater_name:
            comparater_name_dict[key] = data_dict[key]
    return comparater_name_dict


def get_corr_coff_and_pval(numeric_intensity_list_man, numeric_intensity_list_auto):
    """
     This function calculates correlation and p-val between two lists.
    :param numeric_intensity_list_man: sample intensities list for a group from file one
    :param numeric_intensity_list_auto: sample intensities list for a group from file two
    :return: returns a list of correlation, p-val, logfc, mean intensity
    """
    if not any(numeric_intensity_list_man) and not any(numeric_intensity_list_auto):
        cor_coff = 1
        p_val = cs.UNDEFINED
        mean_intensity_man = np.mean(numeric_intensity_list_man)
        mean_intensity_auto = np.mean(numeric_intensity_list_auto)
        fc = cs.UNDEFINED
        logfc = cs.UNDEFINED
        return cor_coff, p_val, logfc, mean_intensity_man, mean_intensity_auto

    elif not any(numeric_intensity_list_man) or not any(numeric_intensity_list_auto):
        cor_coff = cs.UNDEFINED
        p_val = \
            wilcoxon(numeric_intensity_list_man, numeric_intensity_list_auto, zero_method='wilcox')[1]
        mean_intensity_man = np.mean(numeric_intensity_list_man)
        mean_intensity_auto = np.mean(numeric_intensity_list_auto)
        fc = cs.UNDEFINED
        logfc = cs.UNDEFINED
        return cor_coff, p_val, logfc, mean_intensity_man, mean_intensity_auto

    elif numeric_intensity_list_man == numeric_intensity_list_auto:
        cor_coff = 1
        p_val = cs.UNDEFINED
        mean_intensity_man = np.mean(numeric_intensity_list_man)
        mean_intensity_auto = np.mean(numeric_intensity_list_auto)
        fc = mean_intensity_man / mean_intensity_auto
        logfc = math.log(fc, 2)
        return cor_coff, p_val, logfc, mean_intensity_man, mean_intensity_auto

    else:
        cor_coff = pearsonr(numeric_intensity_list_man, numeric_intensity_list_auto)[0]
        p_val = \
            wilcoxon(numeric_intensity_list_man, numeric_intensity_list_auto, zero_method='wilcox')[
                1]
        mean_intensity_man = np.mean(numeric_intensity_list_man)
        mean_intensity_auto = np.mean(numeric_intensity_list_auto)
        fc = mean_intensity_man / mean_intensity_auto
        logfc = math.log(fc, 2)
        return cor_coff, p_val, logfc, mean_intensity_man, mean_intensity_auto


def get_delta_rt_and_mz_df(comparater_dict_man, comparater_dict_auto, delta_rt, delta_mz):
    """
    This function compares groups between two dictionaries within a delta rt and mz range.
    :param comparater_dict_man: First json file converted to a dictionary with unique identifier
    name for each group.
    :param comparater_dict_auto: Second json file converted to a dictionary with unique identifier
    name for each group.
    :param delta_rt: user defined delta rt range
    :param delta_mz: user defined delta mz range
    :return: returns a df satisfying the delta rt and delta mz ranges for a comparison
    """
    rows_list = []
    for key_man in comparater_dict_man.keys():
        for key_auto in comparater_dict_auto.keys():
            if abs(comparater_dict_man[key_man][cs.MEAN_RT] - comparater_dict_auto[key_auto][
                cs.MEAN_RT]) <= delta_rt and abs(comparater_dict_man[key_man][cs.MEAN_MZ] -
                                                        comparater_dict_auto[key_auto][
                                                            cs.MEAN_MZ]) <= delta_mz:
                intensity_list_man = []
                intensity_list_auto = []
                sample_names = comparater_dict_man[key_man][cs.PEAKS].keys()
                for sample in sample_names:
                    intensity_list_auto.append(
                        comparater_dict_auto[key_auto][cs.PEAKS][sample][cs.PEAK_INTENSITY])
                    intensity_list_man.append(
                        comparater_dict_man[key_man][cs.PEAKS][sample][cs.PEAK_INTENSITY])
                numeric_intensity_list_man = [i if i != cs.NA else 0 for i in intensity_list_man]
                numeric_intensity_list_auto = [i if i != cs.NA else 0 for i in intensity_list_auto]

                corr_pval_tuple = get_corr_coff_and_pval(numeric_intensity_list_man,
                                                         numeric_intensity_list_auto)
                row_list = [key_man, key_auto, corr_pval_tuple[0], corr_pval_tuple[1],
                            corr_pval_tuple[2], corr_pval_tuple[3],
                            corr_pval_tuple[4]]
                rows_list.append(row_list)
    corr_wilcox_df = pd.DataFrame(rows_list,
                                  columns=[cs.UNIQUE_IDENTIFIER_MAN, cs.UNIQUE_IDENTIFIER_AUTO,
                                           cs.CORR_COFF, cs.P_VAL, cs.LOGFC_AUTO_TO_MAN,
                                           cs.AVERAGE_INTENSITY_MAN, cs.AVERAGE_INTENSITY_AUTO])
    return corr_wilcox_df


def find_correlation_and_wilcox_test_df(unique_identifier_list, man_dict, auto_dict,
                                        comparater_list, analysis_type, delta_rt, delta_mz):
    """
    This function will return a dataframe with only groups which satisfies delta rt and delta mz
    range.
    :param unique_identifier_list: unique identifier names for which the group would be given a
    unique name
    :param man_dict: First JSON file converted to a dict
    :param auto_dict: Second JSON file converted to a dict
    :param comparater_list: list of parameters on which groups should be compared
    :param analysis_type: type of analysis
    :param delta_rt: delta rt range
    :param delta_mz: delta mz range
    :return: a data frame
    """
    if len(unique_identifier_list) != 0:
        unique_label_dict_man = unique_label_dict(unique_identifier_list, man_dict)
        unique_label_dict_auto = unique_label_dict(unique_identifier_list, auto_dict)
    else:
        unique_label_dict_man = unique_label_dict([cs.COMPOUND_ID, cs.TAG_STRING, cs.MEAN_RT],
                                                  man_dict)
        unique_label_dict_auto = unique_label_dict([cs.COMPOUND_ID, cs.TAG_STRING, cs.MEAN_RT],
                                                   auto_dict)
    if len(comparater_list) != 0:
        unique_dict_man = add_comparater_to_group(comparater_list, unique_label_dict_man)
        unique_dict_auto = add_comparater_to_group(comparater_list, unique_label_dict_auto)
    else:
        unique_dict_man = add_comparater_to_group([cs.COMPOUND_ID, cs.TAG_STRING],
                                                  unique_label_dict_man)
        unique_dict_auto = add_comparater_to_group([cs.COMPOUND_ID, cs.TAG_STRING],
                                                   unique_label_dict_auto)

    comparater_list_man = get_comparater_list(unique_dict_man)
    comparater_list_auto = get_comparater_list(unique_dict_auto)
    comparater_list_combined = list(set(comparater_list_man + comparater_list_auto))
    corr_wilcox_test_df = pd.DataFrame()

    for comparater_name in comparater_list_combined:
        comparater_dict_man = get_comparater_name_dict(comparater_name, unique_dict_man)
        comparater_dict_auto = get_comparater_name_dict(comparater_name, unique_dict_auto)
        if (len(comparater_dict_man) >= 1 and len(comparater_dict_auto) >= 1):
            comparison_corr_wilcox_df = get_delta_rt_and_mz_df(comparater_dict_man,
                                                               comparater_dict_auto, delta_rt,
                                                               delta_mz)
            corr_wilcox_test_df = pd.concat([corr_wilcox_test_df, comparison_corr_wilcox_df],
                                            ignore_index=True)

    return corr_wilcox_test_df


def get_closest_rt_and_mz_df(key_auto, unique_dict_auto, man_closest_rt_and_mz_dict):
    """
    :param key_auto: unique identifier for a group from one dataset
    (for example:  a combined string of compoundID, tagString and mean rt value
    "HMDB00292_Label-1_1.86")
    :param unique_dict_auto: A dictionary with unique identifier names for each group from second
    dataset
    :param man_closest_rt_and_mz_dict: a dictionary with unique identifier names for all the groups
    for first dataset
    :return: a data frame
    """
    rows_list = []

    for key_man in man_closest_rt_and_mz_dict.keys():
        if man_closest_rt_and_mz_dict[key_man][cs.COMPOUND_NAME] == unique_dict_auto[key_auto][
            cs.COMPOUND_NAME] and man_closest_rt_and_mz_dict[key_man][cs.TAG_STRING] == \
                unique_dict_auto[key_auto][cs.TAG_STRING]:
            intensity_list_man = []
            intensity_list_auto = []
            sample_names = unique_dict_auto[key_auto][cs.PEAKS]
            for sample in sample_names:
                intensity_list_auto.append(
                    unique_dict_auto[key_auto][cs.PEAKS][sample][cs.PEAK_INTENSITY])
                intensity_list_man.append(
                    man_closest_rt_and_mz_dict[key_man][cs.PEAKS][sample][cs.PEAK_INTENSITY])
            numeric_intensity_list_man = [i if i != cs.NA else 0 for i in intensity_list_man]
            numeric_intensity_list_auto = [i if i != cs.NA else 0 for i in intensity_list_auto]
            corr_pval_tuple = get_corr_coff_and_pval(numeric_intensity_list_man,
                                                     numeric_intensity_list_auto)
            row_list = [key_man, key_auto, corr_pval_tuple[0], corr_pval_tuple[1],
                        corr_pval_tuple[2], corr_pval_tuple[3],
                        corr_pval_tuple[4]]
            rows_list.append(row_list)

    corr_wilcox_df = pd.DataFrame(rows_list,
                                  columns=[cs.UNIQUE_IDENTIFIER_MAN, cs.UNIQUE_IDENTIFIER_AUTO,
                                           cs.CORR_COFF, cs.P_VAL,
                                           cs.LOGFC_AUTO_TO_MAN, cs.AVERAGE_INTENSITY_MAN,
                                           cs.AVERAGE_INTENSITY_AUTO])

    return corr_wilcox_df


def find_correlation_and_wilcox_test_df_closest_rt_and_mz(unique_identifier_list, man_dict,
                                                          auto_dict, comparater_list, n_rt, n_mz,
                                                          delta_rt, delta_mz, analysis_type):
    """
    Compare groups in two dictionaries with n closest rt and mz
    :param unique_identifier_list:unique identifier names for which the group would be given a
    unique name
    :param man_dict:A dictionary with unique identifier names for each group from first
    dataset
    :param auto_dict: A dictionary with unique identifier names for each group from second
    dataset
    :param comparater_list: list of parameters on which groups should be compared
    :param n_rt: number of closeset rt groups
    :param n_mz: number of closest mz groups
    :param delta_rt: delta rt range
    :param delta_mz: delta mz range
    :param analysis_type: type of analysis
    :return: a data frame
    """
    corr_wilcox_test_df = pd.DataFrame()
    if len(unique_identifier_list) != 0:
        unique_label_dict_man = unique_label_dict(unique_identifier_list, man_dict)
        unique_label_dict_auto = unique_label_dict(unique_identifier_list, auto_dict)
    else:
        unique_label_dict_man = unique_label_dict([cs.COMPOUND_ID, cs.TAG_STRING, cs.MEAN_RT],
                                                  man_dict)
        unique_label_dict_auto = unique_label_dict([cs.COMPOUND_ID, cs.TAG_STRING, cs.MEAN_RT],
                                                   auto_dict)
    if len(comparater_list) != 0:
        unique_dict_man = add_comparater_to_group(comparater_list, unique_label_dict_man)
        unique_dict_auto = add_comparater_to_group(comparater_list, unique_label_dict_auto)
    else:
        unique_dict_man = add_comparater_to_group([cs.COMPOUND_ID, cs.TAG_STRING],
                                                  unique_label_dict_man)
        unique_dict_auto = add_comparater_to_group([cs.COMPOUND_ID, cs.TAG_STRING],
                                                   unique_label_dict_auto)

    comparater_list_man = get_comparater_list(unique_dict_man)
    comparater_list_auto = get_comparater_list(unique_dict_auto)
    comparater_list_combined = list(set(comparater_list_man + comparater_list_auto))
    for comparater_name in comparater_list_combined:
        comparater_dict_man = get_comparater_name_dict(comparater_name, unique_dict_man)
        comparater_dict_auto = get_comparater_name_dict(comparater_name, unique_dict_auto)
        if (len(comparater_dict_man) >= 1 and len(comparater_dict_auto) >= 1):
            for key_auto in comparater_dict_auto:
                man_closest_rt_keys = [
                    comparater_dict_man.keys()[comparater_dict_man.values().index(value)] for value
                    in
                    nsmallest(n_rt, comparater_dict_man.values(), key=lambda x: abs(
                        x[cs.MEAN_RT] - comparater_dict_auto[key_auto][cs.MEAN_RT]))]
                man_closest_rt_dict = {key: comparater_dict_man[key] for key in man_closest_rt_keys}
                man_closest_rt_and_mz_keys = [
                    man_closest_rt_dict.keys()[man_closest_rt_dict.values().index(value)] for
                    value in nsmallest(n_mz, man_closest_rt_dict.values(), key=lambda x: abs(
                        x[cs.MEAN_MZ] - comparater_dict_auto[key_auto][cs.MEAN_MZ]))]
                man_closest_rt_and_mz_dict = {key: man_closest_rt_dict[key] for key in
                                              man_closest_rt_and_mz_keys}
                if analysis_type == cs.BY_N_CLOSEST_RT_AND_MZ:
                    comparison_corr_wilcox_df = get_closest_rt_and_mz_df(key_auto,
                                                                         comparater_dict_auto,
                                                                         man_closest_rt_and_mz_dict)
                    corr_wilcox_test_df = pd.concat(
                        [corr_wilcox_test_df, comparison_corr_wilcox_df], ignore_index=True)
                elif analysis_type == cs.BY_CLOSEST_AND_DELTA_RT_AND_MZ:
                    man_closest_and_delta_rt_mz_dict = {}
                    for key_man in man_closest_rt_and_mz_dict.keys():
                        if abs(man_closest_rt_and_mz_dict[key_man][cs.MEAN_RT] -
                                       comparater_dict_auto[key_auto][
                                           cs.MEAN_RT]) <= delta_rt and abs(
                                    man_closest_rt_and_mz_dict[key_man][cs.MEAN_MZ] -
                                    comparater_dict_auto[key_auto][
                                        cs.MEAN_MZ]) <= delta_mz:
                            man_closest_and_delta_rt_mz_dict[key_man] = man_closest_rt_and_mz_dict[
                                key_man]

                    comparison_corr_wilcox_df = get_closest_rt_and_mz_df(key_auto,
                                                                         comparater_dict_auto,
                                                                         man_closest_and_delta_rt_mz_dict)
                    corr_wilcox_test_df = pd.concat(
                        [corr_wilcox_test_df, comparison_corr_wilcox_df], ignore_index=True)

    return corr_wilcox_test_df


def correlation_scatter_plot(list_man, list_auto, hover_text, cor_list, config_name):
    """
    This function plots a scatter plot between averaged intensities of groups form two datasets
    :param list_man: list of average intensities for all groups from dataset one
    :param list_auto: list of average intensities for all groups from dataset two
    :param hover_text: list of texts to be shown while hovering on a data point
    :param cor_list: list of correlation values for all the group comparisons
    :param config_name: configuration file name
    :return: a plotly plot
    """
    trace = go.Scatter(x=list_auto, y=list_man, mode=cs.MARKERS, marker=dict(
        size=cs.COR_PLOT_DATAPOINT_SIZE,
        color=cor_list,
        colorscale=cs.COR_PLOT_COLOR_SCALE,
        showscale=True
    ), text=hover_text, hoverinfo=cs.TEXT)
    data = [trace]
    layout = go.Layout(
        title=cs.COR_PLOT_MAIN_TITLE,
        xaxis=dict(title=cs.COR_PLOT_X_TITLE),
        yaxis=dict(title=cs.COR_PLOT_Y_TITLE), showlegend=False, hovermode=cs.COR_PLOT_HOVER_MODE)

    figure = dict(data=data, layout=layout)
    make_dir(cs.OUTPUT_DIR)
    cor_plot = plot(figure, filename=os.path.join(
        cs.RESULT_DIR, config_name + cs.CORRELATION_PLOT_NAME), auto_open=False)
    return cor_plot


def get_correlation_plot(df_to_plot, config_name):
    """
    This function is wrapper function to make a correlation plot. Which takes a data frame which
    columns are to be plotted.
    :param df_to_plot: a data frame
    :param config_name: configuration file name, just to give a unique name to the output plot file
    :return: a correlation scatter plot
    """
    list_man = df_to_plot[cs.AVERAGE_INTENSITY_MAN].tolist()
    list_auto = df_to_plot[cs.AVERAGE_INTENSITY_AUTO].tolist()
    log_intensity_man = [log(value, 10) for value in list_man]
    log_intensity_auto = [log(value, 10) for value in list_auto]
    cor_list = df_to_plot[cs.CORR_COFF].tolist()
    hover_text = list(df_to_plot[cs.UNIQUE_IDENTIFIER_MAN].map(str) + '<->' + df_to_plot[
        cs.UNIQUE_IDENTIFIER_AUTO].map(str) + cs.BREAK_LINE_TAG + df_to_plot[cs.CORR_COFF].map(str))
    cor_plot = correlation_scatter_plot(log_intensity_man, log_intensity_auto, hover_text, cor_list,
                                        config_name)
    return cor_plot


def wilcox_scatter_plot(neg_log10_pval_list, log2_fc_list, hover_text, plot_name):
    """
    Plots a scatter plot between negative log10 p values and their log fold changes.
    :param neg_log10_pval_list: list of negative log10 p values
    :param log2_fc_list: list of log2 fold chnage values
    :param hover_text: list of hover text for all the data points
    :return: a scatter plot (wilcox plot)
    """
    trace = go.Scatter(x=log2_fc_list, y=neg_log10_pval_list, mode=cs.MARKERS, marker=dict(
        size=cs.CORRELATION_PLOT_NAME,
        color=cs.ORANGE_COLOR,
        colorscale=False,
        showscale=False
    ), text=hover_text, hoverinfo=cs.TEXT)
    data = [trace]
    layout = go.Layout(
        title=cs.WILCOX_PLOT_TITLE,
        xaxis=dict(title=cs.LOG2_FC),
        yaxis=dict(title=cs.NEG_LOG10_PVAL), showlegend=False, hovermode=cs.COR_PLOT_HOVER_MODE)

    figure = dict(data=data, layout=layout)
    wilcox_plot = plot(figure, filename=plot_name, auto_open=False)
    return wilcox_plot


def get_wilcox_plot(df_to_plot, plot_name='wilcox_plot'):

    """
    This function is a wrapper function for wilcox plot.
    :param df_to_plot: data frame to be plotted
    :return: a plotly plot (wilcox plot)
    """
    hover_text = list(df_to_plot[cs.UNIQUE_IDENTIFIER_MAN].map(str) + '<->' + df_to_plot[
        cs.UNIQUE_IDENTIFIER_AUTO].map(str))
    p_val_list = df_to_plot[cs.P_VAL].tolist()
    log2_fc_list = df_to_plot[cs.LOGFC_AUTO_TO_MAN].tolist()
    neg_log10_pval = [-log(value, 10) for value in p_val_list]
    wilcox_plot = wilcox_scatter_plot(neg_log10_pval, log2_fc_list, hover_text, plot_name)
    return wilcox_plot


def get_basename_url(url):
    """
    This function gives basename of a URL
    :param url: URL to get basename for
    :return: basename of the url
    """
    parse_object = urlparse(url)
    url_basename = basename(parse_object.path)
    return url_basename


def get_summary_report(correlation_and_wilcox_df, correlation_plot_url, wilcox_plot_url,
                       outlier_plots):
    """
    This function gives a summary report of comparison between two json files. This report consists
    of Correlation plot, wilcox plot, comparison data table and EIC and Scatter plots for groups
    below user defined cut offs for Correlation and p-value.
    :param correlation_and_wilcox_df: This data frame has columns for unique identifier names,
    correlation and p-value for a comparison, logfc, average intensities.
    :param correlation_plot_url: Url of Correlation plot
    :param wilcox_plot_url: url of wilcox plot
    :param outlier_plots: list of three lists
    :return: an html report
    """
    correlation_plot_basename = get_basename_url(correlation_plot_url)
    wilcox_plot_basename = get_basename_url(wilcox_plot_url)
    html_string = '''
    <html>
        <head>
            <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.1/css/bootstrap.min.css">
            <style>body{ margin:0 100; background:whitesmoke; }</style>
            <script src="https://cdn.plot.ly/plotly-latest.min.js"></script>
        </head>
        <body>
            <h1>Summary Report of your Analysis</h1>
            <h2>Section 1: Correlation Plot</h2>
            <iframe id="correlation_plot" src=''' + correlation_plot_basename + ''' height="600" width="800"></iframe>
            <h2>Section 2: Wilcox plot </h2>
            <iframe id="wilcox_plot" src=''' + wilcox_plot_basename + ''' height="600" width="800"></iframe>
            <h3>Correlation and P-val dataframe</h3>
            ''' + correlation_and_wilcox_df + ''' '''

    for i in range(0, len(outlier_plots[0])):
        html_string = html_string + '''<h4>''' + \
                      get_basename_url(outlier_plots[0][i]).split(".html")[0] \
                      + " eic plot" + '''</h4>''' + '''<br>'''
        html_string = html_string + '''<br>''' + '''<iframe id=''' + '''eic_plot_man''' + \
                      str(i) + ''' ''' + '''src=''' + get_basename_url(outlier_plots[0][i]) \
                      + ''' ''' + ''' height="600" width="800"></iframe>'''

        html_string = html_string + '''<h4>''' + \
                      get_basename_url(outlier_plots[1][i]).split(".html")[0] \
                      + " eic plot" + '''</h4>''' + '''<br>'''

        html_string = html_string + '''<br>''' + '''<iframe id=''' + '''eic_plot_auto''' + \
                      str(i) + ''' ''' + '''src=''' + get_basename_url(outlier_plots[1][i]) \
                      + ''' ''' + ''' height="600" width="800"></iframe>'''

        html_string = html_string + '''<h4>''' + \
                      get_basename_url(outlier_plots[2][i]).split(".html")[0] \
                      + " scatter plot" + '''</h4>''' + '''<br>'''

        html_string = html_string + '''<br>''' + '''<iframe id=''' + '''scatter_plot_man''' + \
                      str(i) + ''' ''' + '''src=''' + get_basename_url(outlier_plots[2][i]) \
                      + ''' ''' + ''' height="600" width="800"></iframe>'''

    html_string = html_string + '''</body>''' + '''<br>''' + '''</html>'''
    f = open('report.html', 'w')
    f.write(html_string)
    f.close()


def load_df(fpath):
    """
    Load specified file path into a pandas dataframe. Supported
    files types are .csv or .tab
    Args:
        fpath(str): Path of file
    Returns:
        pdataframe(pandas dataframe): Specified file loaded to pandas df
    """
    fname, fextension = os.path.splitext(fpath)

    if fextension == cs.TAB_EXTENSION:
        pdataframe = pd.read_csv(fpath, sep='\t')
        return pdataframe
    elif fextension == cs.CSV_EXTENSION:
        pdataframe = pd.read_csv(fpath, sep=',')
        return pdataframe
    else:
        raise ValueError(fextension + cs.FILE_FORMAT_NOT_SUPPORTED)


def merge_dfs(df_list, col_list):
    """
    Merge multiple dataframes based on specified columns
    Args:
        df_list (list): List of pandas dataframes
        col_list (list): List of column names which will be used for
            merging dataframes
    Returns:
        merged_df (pandas df): Pandas dataframe after merging multiple
            pandas dataframes
    """

    merged_df = reduce(lambda left, right: pd.merge(left, right, on=col_list), df_list)

    return merged_df


def get_corr_and_pval_cutoff_df(corr_cutoff, p_val_cutoff, input_df):
    """
    This function subsets the input dataframe based on correlation and p-value cut offs.
    :param corr_cutoff: Correlation cutoff for a outlier
    :param p_val_cutoff: p-val cut off for a outlier
    :param input_df: Input dataframe having comaprison details
    :return: data frame with rows having correlation and p-value below user defined cut offs
    """
    output_df = input_df[
        (input_df[cs.CORR_COFF] <= corr_cutoff) & (input_df[cs.P_VAL] <= p_val_cutoff)]
    return output_df


def eicplot(unique_identifier, file_path, plot_file_name):
    """
       This function is used to plot a line plot (eic plot) between intensites and rt values for all
        the samples to visualise the peak shape for a unique group.
       :param unique_identifier: It is a list of two lists. First list has names of parameters which
        will be used to identify an unique group from input json file and second list has their
        respective values.
       for example : [["compoundId","groupId","rtmin"],["citrate",175,10.79271603]]

       :param file_path: file path of dataset one (let's say for manually curated dataset)


       :return: retuen a plot between intensities and rt values for all the samples in a chossen
       unique group.

       """
    comp_dict = json_eic_parser(file_path)
    unique_data = unique_identifier_data(unique_identifier, comp_dict)
    group_name = unique_data.keys()[0]
    intensity_list = []
    rt_list = []
    rt_range = []
    mz_range = []
    sample_names = unique_data[group_name][cs.PEAKS].keys()
    for key in sample_names:
        intensity_list.append(unique_data[group_name][cs.PEAKS][key][cs.EIC][cs.INTENSITY])
        rt_range.append(unique_data[group_name][cs.PEAKS][key][cs.RT])
        mz_range.append(unique_data[group_name][cs.PEAKS][key][cs.PEAK_MZ])
        rt_list.append(unique_data[group_name][cs.PEAKS][key][cs.EIC][cs.RT])
    eic_plot = lineplot(intensity_list, rt_list, sample_names, group_name, plot_file_name)
    return eic_plot


def intensityscatterplot(unique_identifier_man, unique_identifier_auto, file_path_man,
                         file_path_auto, plot_file_name):
    """

        :param unique_identifier_man: It is a list of two lists. First list has names of parameters
        which will be used to identify an unique group from input json file and second list has
        their respective values.
        [["compoundId","groupId","rtmin"],["citrate",175,10.79271603]]

        :param unique_identifier_auto: It is a list of two lists. First list has names of parameters
         which will be used to identify an unique group from input json file and second list has
         their respective values.
        for example : [["tagString","formula","meanMz"],["C13-label-1","C6H8O7",192.0224457]]

        :param file_path_man: file path of dataset one (let's say for manually curated dataset)


        :param file_path_auto: file path of dataset one (let's say for automatically curated
        dataset)


        :return: scatter plot for all the sample intensities between both the datasets

        """
    comp_dict_man = json_comp_data(file_path_man)
    comp_dict_auto = json_comp_data(file_path_auto)
    unique_data_man = unique_identifier_data(unique_identifier_man, comp_dict_man)
    unique_data_auto = unique_identifier_data(unique_identifier_auto, comp_dict_auto)
    group_name = unique_data_man.keys()[cs.ZERO]
    intensity_list_man = []
    intensity_list_auto = []
    sample_names = unique_data_man[group_name][cs.PEAKS].keys()
    for key in sample_names:
        intensity_list_auto.append(unique_data_auto[group_name][cs.PEAKS][key][cs.PEAK_INTENSITY])
        intensity_list_man.append(unique_data_man[group_name][cs.PEAKS][key][cs.PEAK_INTENSITY])

    plot = scatterplot(intensity_list_man, intensity_list_auto, sample_names)
    scatter_plot = plot_layout(plot, group_name, cs.AUTOMATED_INTENSITY,
                               cs.MANUAL_INTENSITY, plot_file_name)
    return scatter_plot


def is_exe(fpath):
    """
    Check if the specified file/path is exe or not.
    Args:
        fpath (str): Path of file
    Returns:
        file_is_exe (bool): Returns true if file is exe
    """
    file_is_exe = os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    return file_is_exe


def check_exe_exists(exe_path):
    """
    Check if the specified exe exists in path.
    Args:
        exe_path (str): Path of the exe to be checked
    Returns:
        Returns true if exe exist else false
    """

    fpath, fname = os.path.split(exe_path)
    if fpath:
        if is_exe(exe_path):
            return True
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            path = path.strip('"')
            exe_file = os.path.join(path, exe_path)
            if is_exe(exe_file):
                return True

    return False


def parse_arguments(arguments):
    """
    Parse arguments. Supported types : string, list & dict
    Args:
        arguments (str, list or dict): Arguments to be
            parsed
    Return:
        list_args(list): List of arguments
    """

    list_args = []
    if isinstance(arguments, basestring):
        list_args.append(arguments)
    elif isinstance(arguments, list):
        list_args = arguments
    elif isinstance(arguments, dict):
        for arg, value in arguments.iteritems():
            list_args.append(arg)
            if value:
                list_args.append(value)
    else:
        raise ValueError(cs.TYPE_NOT_SUPPORTED_FOR_ARGUMENTS)

    return list_args


def run_exe(exe_path, arguments):
    """
    Run the executable with arguments
    Args:
        exe_path(str) : Path of the executable
        arguments(dict): Arguments with values
    """
    list_args = parse_arguments(arguments)
    list_args.insert(0, exe_path)
    subprocess.call(list_args)


def list_files(dir_path, file_extension):
    """
    List all the files in a given directory with specified
    extenstion.
    Args:
        dir_path (str): Directory path where files are present
        file_extenstion (str): Extenstion of files needs to
            returned
    Returns:
        files (list): List of files with specified extension
            and directory
    """
    files = []

    for root, dirnames, filenames in os.walk(dir_path):
        for fname in filenames:
            if fname.endswith(file_extension):
                files.append(os.path.join(root, fname))

    return files


def delete_dir(dir_path):
    """
    Delete the directory and all its contents
    Args:
        dir_path (str): Path of directory
    """

    try:
        shutil.rmtree(dir_path)
    except OSError:
        pass
    except:
        print cs.NOT_ABLE_TO_DELETE + dir_path


def make_dir(dir_path):
    """
    Create directory recursively
    Args:
        dir_path (str): directory to be created
    """

    try:
        os.makedirs(dir_path)
    except OSError:
        pass
    except:
        print cs.NOT_ABLE_TO_DELETE + dir_path
