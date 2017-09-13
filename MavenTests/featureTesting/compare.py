"""
This module compares different output of cli

List of functions:

1. compare: Compare all the outputs generated from multiple
    cli instances

2. load_files: Loads every file present in the input list into a
    pandas dataframe

3. remove_outliers: Remove rows whose mzs and rts are not equal

"""

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
        merged_df = helper.merge_dfs(df_list, col_list)
        merged_df = self.remove_outliers(merged_df)

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

    def remove_outliers(self, pandas_df):
        """
        Remove rows whose mzs and rts are not equal
        Args:
            pandas_df (df): Merged pandas dataframe
        Returns:
            pandas_df (df): Pandas dataframe after outliers
                are removed
        """

        for index, row in pandas_df.iterrows():

            mz_1 = row["medMz_x"]
            mz_2 = row["medMz_y"]
            rt_1 = row["medRt_x"]
            rt_2 = row["medRt_y"]

            mz_diff = abs(mz_2-mz_1)
            rt_diff = abs(rt_2-rt_1)

            if mz_diff < 0.5 and rt_diff < 0.3:
                pass
            else:
                pandas_df.drop(index, inplace=True)

        return pandas_df
