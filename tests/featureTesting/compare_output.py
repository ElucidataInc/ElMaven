import os
import sys
import numpy as np
import pandas as pd
from os.path import join
from helper import helper
from config import config
from scipy import stats
import run_peakdetector

class compare_output():

    def __init__(self):
        self.st = config.variables.st
        self.append_path = ''
        self.dict_of_changes = {}
        self.dict_of_dfs = {}
        self.validity_builds = {}

    def compare(self, list_builds):

        self.check_input_conditions(list_builds)

        dict_of_scripts = run_peakdetector.run_peakdetector.dict_of_scripts

        for build_maven in list_builds:
            for key, value in dict_of_scripts[build_maven].iteritems():
                if key in self.dict_of_changes:
                    self.dict_of_changes[key].append(value)
                else:
                    self.dict_of_changes[key] = [value]

        for key, value in self.dict_of_changes.iteritems():
                list_of_dfs = []
                self.append_path = key
                for v in value:
                    df = helper.read_csv_pandas(join(v['outputdir'], 'compounds.csv'))
                    list_of_dfs.append(df)

                lists_of_compounds = self.get_lists_of_compounds(list_of_dfs)
                unique_list_of_compounds = helper.get_intersection_of_list(lists_of_compounds)

                self.compare_lists(lists_of_compounds)

                list_of_intersected_dfs = self.get_list_of_intersected_dfs(list_of_dfs, unique_list_of_compounds)
                list_of_dfs_with_same_mz_rt = self.get_list_of_dfs_with_same_mz_rt(list_of_intersected_dfs, unique_list_of_compounds)

                self.dict_of_dfs[key] = list_of_dfs_with_same_mz_rt

                list_mzxml_file_names = helper.get_list_of_files_in_directory_without_extension(config.input_paths(0).inputdir)

                len_of_df = len(list_of_dfs_with_same_mz_rt[0])

                list_of_r2_values = []

                df1 = list_of_dfs_with_same_mz_rt[0]
                df2 = list_of_dfs_with_same_mz_rt[1]
                number_r2 = 0
                for i in xrange(len_of_df):
                    df_1 = list_of_dfs_with_same_mz_rt[0].iloc[[i]][list_mzxml_file_names].values.tolist()[0]
                    df_2 = list_of_dfs_with_same_mz_rt[1].iloc[[i]][list_mzxml_file_names].values.tolist()[0]
                    df_1 = [float(i) for i in df_1]
                    df_2 = [float(i) for i in df_2]
                    slope, intercept, r_value, p_value, std_err = stats.linregress(df_1,df_2)
                    r2_value = round(r_value ** 2,3)
                    if r2_value > 0.9:
                        number_r2 += 1
                    list_of_r2_values.append(r2_value)


                df1 = df1[['compound', 'medMz', 'medRt']]
                df2 = df2[['compound', 'medMz', 'medRt']]
                df1.columns = ['compound_1', 'medMz_1', 'medRt_1']
                df2.columns = ['compound_2', 'medMz_2', 'medRt_2']

                index_df1 = df1.index.values
                index_df2 = df2.index.values

                original_df1 = list_of_dfs[0]
                original_df2 = list_of_dfs[1]

                original_df1 = original_df1.drop(original_df1.index[index_df1])
                original_df2 = original_df2.drop(original_df2.index[index_df2])

                build_df1_list = [list_builds[0]] * len(original_df1.index)
                build_df2_list = [list_builds[1]] * len(original_df2.index)

                build_df_list = (build_df1_list + build_df2_list)

                ser = pd.Series(build_df_list)
                df_with_diff_rt_mz = pd.concat([original_df1, original_df2], ignore_index = True)[['compound', 'medMz', 'medRt']]
                df_with_diff_rt_mz['build'] = ser.values
                if os.path.isdir(join(config.variables.csv_compare_output, self.st, self.append_path)):
                    pass
                else:
                    os.makedirs(join(config.variables.csv_compare_output, self.st, self.append_path))
                df_with_diff_rt_mz.to_csv(join(config.variables.csv_compare_output, self.st, self.append_path, config.variables.csv_with_diff_rt_mz))
                df1 = df1.reset_index()
                df2 = df2.reset_index()

                r2_df = pd.concat([df1, df2], axis=1)

                se = pd.Series(list_of_r2_values)
                r2_df['r2'] = se.values
                if os.path.isdir(join(config.variables.csv_compare_output, self.st, self.append_path)):
                    pass
                else:
                    os.makedirs(join(config.variables.csv_compare_output, self.st, self.append_path))
                r2_df.to_csv(join(config.variables.csv_compare_output, self.st, self.append_path, config.variables.csv_r2_values))


                summary = self.generate_summary(list_of_dfs, lists_of_compounds, list_of_r2_values, number_r2)

                self.check_equality_builds(list_of_dfs, lists_of_compounds, list_of_r2_values, number_r2)
                
                if os.path.isdir(join(config.variables.csv_compare_output, self.st, self.append_path)):
                    pass
                else:
                    os.makedirs(join(config.variables.csv_compare_output, self.st, self.append_path))
                helper.export_file(summary, str(join(config.variables.csv_compare_output, self.st, self.append_path, config.variables.csv_summary_of_results)))


    def get_list_of_dfs(self, list_builds):

        list_of_dfs = []
        for build_maven in list_builds:
            paths = config.input_paths(build_maven)
            df = helper.read_csv_pandas(join(paths.outputdir, 'compounds.csv'))
            list_of_dfs.append(df)

        return list_of_dfs

    def get_lists_of_compounds(self, list_of_dfs):

        lists_of_compounds = []

        for df in list_of_dfs:
            list_of_compounds = self.get_list_of_compounds(df)
            lists_of_compounds.append(list_of_compounds)
        
        return lists_of_compounds

    def get_list_of_intersected_dfs(self, list_of_dfs, unique_list_of_compounds):

        list_of_intersected_dfs = []

        for df in list_of_dfs:
            list_of_intersected_dfs.append(df.loc[df['compound'].isin(unique_list_of_compounds)])
        
        return list_of_intersected_dfs

    def get_list_of_dfs_with_same_mz_rt(self, list_of_dfs, unique_list_of_compounds):

        list_of_dfs_with_same_mz_rt = helper.create_list_of_empty_dfs(len(list_of_dfs))

        for compound in unique_list_of_compounds:
            list_compound_df = []
            for df in list_of_dfs:
                compound_df = df[df['compound'] == compound]
                list_compound_df.append(compound_df)
            
            list_of_dfs_with_same_mz_rt_compound = self.compare_mz_rt(list_compound_df)

            for i in xrange(len(list_of_dfs_with_same_mz_rt_compound)):
                list_of_dfs_with_same_mz_rt[i] = list_of_dfs_with_same_mz_rt[i].append(list_of_dfs_with_same_mz_rt_compound[i])
        
        return list_of_dfs_with_same_mz_rt

    def compare_mz_rt(self, list_of_dfs):

        df1 = list_of_dfs[0]
        df2 = list_of_dfs[1]
        df1_len = len(df1.index)
        df2_len = len(df2.index)

        mz_difference = 10*10e-6
        rt_difference = 0.3

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


    def get_list_of_compounds(self, df):
        list_of_compounds = df['compound'].unique()
        return list_of_compounds

    def compare_lists(self, list_of_lists):

        a = list_of_lists[0]

        b = list_of_lists[1]

        c = list(set(a) | set(b)) #union of a & b

        SEP = ','
        csv_output = "Compounds, Compounds (769), compounds (776)\n"


        for i in c:

            csv_output += str(i)
            csv_output += SEP

            if not(i in a):
                csv_output += "Not Present"
            csv_output += SEP

            if not(i in b):
                csv_output += "Not Present"
            csv_output += '\n'
        if os.path.isdir(join(config.variables.csv_compare_output, self.st, self.append_path)):
            pass
        else:
            os.makedirs(join(config.variables.csv_compare_output, self.st, self.append_path))
        helper.export_file(csv_output, join(config.variables.csv_compare_output, self.st, self.append_path, config.variables.csv_compare_compounds))

    def check_input_conditions(self, list_builds):

        if len(list_builds) < 2:
            sys.exit('You need 2 or more outputs to compare')

    def generate_summary(self, list_of_dfs, lists_of_compounds, list_of_r2_values, number_r2):

        summary = ''
        summary += '1. Total numer of peaks found by maven v769 is %d and by maven v776 is %d\n' %(len(list_of_dfs[0].index), len(list_of_dfs[1].index))

        a = lists_of_compounds[0]

        b = lists_of_compounds[1]

        c = list(set(a) | set(b)) #union of a & b

        summary += '2. Out of the total %d compounds found by both the versions, v769 found %d compounds and v776 found %d compounds\n' %(len(c), len(a), len(b))
        summary += '3. The total numer of peaks with same mz & rt are %d\n' %(len(list_of_r2_values))
        summary += '4. Out of this %d peaks, %d number of peaks has r-square greater than 0.9\n' %(len(list_of_r2_values), number_r2)

        return summary

    def check_equality_builds(self, list_of_dfs, lists_of_compounds, list_of_r2_values, number_r2):

        test_function = self.get_test_func_from_run_function()

        self.validity_builds[test_function] = True

        a = lists_of_compounds[0]
        b = lists_of_compounds[1]

        intersection_list = set(a) & set(b)

        if not float(len(intersection_list))/len(a) > 0.95:
            self.validity_builds[test_function] = False

        if not float(number_r2)/(len(list_of_r2_values)) > 0.9:
            self.validity_builds[test_function] = False


    def get_test_func_from_run_function(self):

        if config.variables.run_function == 1:
            return "compoundDatabaseSearch"



compare_output = compare_output()