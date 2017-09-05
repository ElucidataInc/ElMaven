"""
This module compares different output of cli

List of functions:

1. compare: Compare all the outputs generated from multiple
    cli instances
2. load_files: Loads every file present in the input list into a pandas
    dataframe
3. dfs_groupby: Converts list of pandas dataframes into groupby
    objects based on the specified columns

"""
import pandas as pd
from helper import helper


class CompareOutput(object):
    """
    This class compares different outputs of Cli generaed using
    different configuration files
    """

    def __init__(self):
        pass

    def compare(self):
        """
        Compare all the outputs generated from multiple
        cli instances
        """
        file_list = ['test1.tab', 'test2.tab']
        df_list = self.load_files(file_list)
        col_list = ["compoundId", "compound", "formula", "goodPeakCount"]
        groupby_list = self.dfs_groupby(df_list, col_list)


    def load_files(self, file_list):
        """
        Loads every file present in the input list into a pandas
        dataframe
        Args:
            file_list (list): List of paths of files
        Returns:
            df_list (list): List of pandas dataframes
        """
        df_list = []

        for fpath in file_list:
            pdataframe = helper.load_df(fpath)
            df_list.append(pdataframe)

        return df_list

    def dfs_groupby(self, df_list, col_list):
        """
        Converts list of pandas dataframes into groupby
        objects based on the specified columns
        Args:
            df_list (list): List of pandas dataframes
            col_list (list): List of columns needed for groupby
        Returns:
            groupby_list (list): List of groupby objects
        """
        groupby_list = []

        for pdataframe in df_list:
            groupby_obj = pdataframe.groupby(col_list)
            groupby_list.append(groupby_obj)

        return groupby_list
