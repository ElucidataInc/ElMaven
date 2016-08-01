import os
import sys
import pandas as pd
from os.path import join
from helper import helper
from config import config
from scipy import stats

def compare(list_builds):

    if len(list_builds) < 2:
        sys.exit('You need 2 or more outputs to compare')

    list_of_dfs = get_list_of_dfs(list_builds)
    lists_of_compounds = get_lists_of_compounds(list_of_dfs)
    unique_list_of_compounds = helper.get_intersection_of_list(lists_of_compounds)

    list_of_intersected_dfs = get_list_of_intersected_dfs(list_of_dfs, unique_list_of_compounds)
    list_of_dfs_with_same_mz_rt = get_list_of_dfs_with_same_mz_rt(list_of_intersected_dfs, unique_list_of_compounds)

    list_mzxml_files = os.listdir(config.paths(0).inputdir)

    list_mzxml_file_names = []

    for mzxml_file in list_mzxml_files:

        list_mzxml_file_names.append(mzxml_file.split('.')[0])

    len_of_df = len(list_of_dfs_with_same_mz_rt[0])

    list_of_r2_values = []

    for i in xrange(len_of_df):
        df_1 = list_of_dfs_with_same_mz_rt[0].iloc[[i]][list_mzxml_file_names].values.tolist()[0]
        df_2 = list_of_dfs_with_same_mz_rt[1].iloc[[i]][list_mzxml_file_names].values.tolist()[0]
        df_1 = [float(i) for i in df_1]
        df_2 = [float(i) for i in df_2]
        slope, intercept, r_value, p_value, std_err = stats.linregress(df_1,df_2)
        r2_value = round(r_value ** 2,3)
        list_of_r2_values.append(r2_value)
    print list_of_r2_values

def get_list_of_dfs(list_builds):

    list_of_dfs = []
    for build_maven in list_builds:
        paths = config.paths(build_maven)
        df = helper.read_csv_pandas(join(paths.outputdir, 'compounds.csv'))
        list_of_dfs.append(df)
    
    return list_of_dfs

def get_lists_of_compounds(list_of_dfs):

    lists_of_compounds = []

    for df in list_of_dfs:
        list_of_compounds = get_list_of_compounds(df)
        lists_of_compounds.append(list_of_compounds)\
    
    return lists_of_compounds

def get_list_of_intersected_dfs(list_of_dfs, unique_list_of_compounds):

    list_of_intersected_dfs = []

    for df in list_of_dfs:
        list_of_intersected_dfs.append(df.loc[df['compound'].isin(unique_list_of_compounds)])
    
    return list_of_intersected_dfs

def get_list_of_dfs_with_same_mz_rt(list_of_dfs, unique_list_of_compounds):

    list_of_dfs_with_same_mz_rt = helper.create_list_of_empty_dfs(len(list_of_dfs))

    for compound in unique_list_of_compounds:
        list_compound_df = []
        for df in list_of_dfs:
            compound_df = df[df['compound'] == compound]
            list_compound_df.append(compound_df)
        
        list_of_dfs_with_same_mz_rt_compound = compare_mz_rt(list_compound_df)

        for i in xrange(len(list_of_dfs_with_same_mz_rt_compound)):
            list_of_dfs_with_same_mz_rt[i] = list_of_dfs_with_same_mz_rt[i].append(list_of_dfs_with_same_mz_rt_compound[i])
    
    return list_of_dfs_with_same_mz_rt

def compare_mz_rt(list_of_dfs):

    df1 = list_of_dfs[0]
    df2 = list_of_dfs[1]
    df1_len = len(df1.index)
    df2_len = len(df2.index)

    mz_difference = 5*10e-6
    rt_difference = 0.01

    modified_df_1 = pd.DataFrame()
    modified_df_2 = pd.DataFrame()


    for i in xrange(df1_len):
        for j in xrange(df2_len):

            df1_current_row = df1.iloc[[i]]
            df2_current_row = df2.iloc[[j]]

            if abs(float(df2_current_row['medMz']) - float(df1_current_row['medMz'])) < mz_difference:
                if abs(float(df2_current_row['medRt']) - float(df1_current_row['medRt'])) < rt_difference:
                    modified_df_1 = modified_df_1.append(df1_current_row)
                    modified_df_2 = modified_df_2.append(df2_current_row)

    return [modified_df_1, modified_df_2]


def get_list_of_compounds(df):
    list_of_compounds = df['compound'].unique()
    return list_of_compounds

