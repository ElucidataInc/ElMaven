"""
This module compares different output of cli

List of functions:

1. compare: Compare all the outputs generated from multiple
    cli instances

2. load_files: Loads every file present in the input list into a
    pandas dataframe

3. remove_outliers: Remove rows whose mzs and rts are not equal

"""
import os
import plotly.graph_objs as go
from plotly.offline import plot
from helper import helper
from config import config

class CompareOutput(object):
    """
    This class compares different outputs of Cli generated using
    different configuration files
    """

    def __init__(self, file_list, config_name):
        self.file_list = file_list
        self.config_name = config_name
        self.config = config.Config()

    def compare(self):
        """
        Compare all the outputs generated from multiple
        cli instances
        """

        df_list = self.load_files(self.file_list)
        merged_df = helper.merge_dfs(df_list, self.config.unique_identifiers)
        merged_df = self.remove_outliers(merged_df)
        self.plot(merged_df, self.config.sample_list)


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

            if mz_diff < 0.3 and rt_diff < 0.2:
                pass
            else:
                pandas_df.drop(index, inplace=True)

        return pandas_df

    def plot(self, pandas_df, sample_list):
        """
        Plot scatter plot for comparison single sample from two pandas
        dataframes

        Args:
            pandas_df (df): Merged pandas dataframe
        """

        data = []

        for sample_name in sample_list:
            sample_name_x = sample_name + '_x'
            sample_name_y = sample_name + '_y'
            x = []
            y = []

            for index, row in pandas_df.iterrows():
                x.append(row[sample_name_x])
                y.append(row[sample_name_y])

            trace = go.Scatter(
                x=x,
                y=y,
                name=sample_name,
                mode='markers'
            )

            data.append(trace)

        layout = go.Layout(
            title=self.config_name,
            xaxis=dict(
                title='test1.tab',
                type='log'
            ),
            yaxis=dict(
                title='test2.tab',
                type='log'
            )
        )


        fig = go.Figure(data=data, layout=layout)

        os.system('mkdir -p results')
        plot(fig, filename=os.path.join('results', self.config_name + '.html'))
