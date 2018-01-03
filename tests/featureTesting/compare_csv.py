import os

import plotly.graph_objs as go
from plotly.offline import plot
import pandas
import numpy
import constants as cs
from helper import helper_functions as hf

class CompareCsvs(object):
    """
        This class compares different outputs of Cli generated using
        different configuration files
        """
    def __init__(self, file_list, config_name, sample_list=cs.SAMPLE_LIST):
        self.file_list = file_list
        self.col_list=[cs.COMPOUND,cs.compoundId]
        self.config_name = config_name
        self.sample_list = sample_list

        hf.make_dir(cs.RESULT_DIR)

    def compare(self):
        """
        Compare all the outputs generated from multiple
        cli instances
        """

        df_list = self.load_files()
        merged_df = hf.merge_dfs(df_list, self.col_list)
        correlation_and_wilcox_df = self.get_correlation_and_wilcox_df(merged_df)
        merged_df = self.remove_outliers(merged_df)
        plot_fig = self.plot(merged_df)

        cor_plot = hf.get_correlation_plot(correlation_and_wilcox_df)
        return plot_fig


    def load_files(self):
        """
        Loads every file present in the input list into a pandas
        dataframe
        Args:
            file_list (list): List of paths of files
        Returns:
            df_list (list): List of pandas dataframes
        """
        df_list = []

        for fpath in self.file_list:
            pdataframe = hf.load_df(fpath)
            df_list.append(pdataframe)

        return df_list

    def remove_outliers_using_diff(self, pandas_df):
        """
        Remove outliers in pandas dataframe on the basis
        of mzs and rts difference
        Args:
            pandas_df (df): Pandas dataframe
        Returns:
            pandas_df (df): Pandas dataframe
        """

        score_list = []
        for index, row in pandas_df.iterrows():

            mz_1 = row["medMz_x"]
            mz_2 = row["medMz_y"]
            rt_1 = row["medRt_x"]
            rt_2 = row["medRt_y"]

            mz_diff = abs(mz_2 - mz_1)
            rt_diff = abs(rt_2 - rt_1)

            if mz_diff < 0.3 and rt_diff < 0.2:
                score_list.append((mz_diff+rt_diff))
                pass
            else:
                pandas_df.drop(index, inplace=True)
        pandas_df["score"] = score_list
        return pandas_df

    def remove_outliers_using_score(self, pandas_df):
        """
        Remove outliers in pandas dataframe on the basis
        of score and duplicates
        Args:
            pandas_df (df): Pandas dataframe
        Returns:
            pandas_df (df): Pandas dataframe
        """

        mzrt1 = []
        mzrt2 = []

        for index, row in pandas_df.iterrows():

            mz_1 = row["medMz_x"]
            mz_2 = row["medMz_y"]
            rt_1 = row["medRt_x"]
            rt_2 = row["medRt_y"]

            key_1 = (mz_1, rt_1)
            key_2 = (mz_2, rt_2)

            if key_1 in mzrt1 and key_2 in mzrt2:
                pandas_df.drop(index, inplace=True)
            else:
                mzrt1.append(key_1)
                mzrt2.append(key_2)

        return pandas_df

    def remove_outliers(self, pandas_df):
        """
        Remove rows whose mzs and rts are not equal
        Args:
            pandas_df (df): Merged pandas dataframe
        Returns:
            pandas_df (df): Pandas dataframe after outliers
                are removed
        """

        pandas_df = self.remove_outliers_using_diff(pandas_df)
        pandas_df = pandas_df.sort_values("score")
        pandas_df = self.remove_outliers_using_score(pandas_df)

        return pandas_df

    def plot(self, pandas_df):
        """
        Plot scatter plot for comparison single sample from two pandas
        dataframes
        Args:
            pandas_df (df): Merged pandas dataframe
        """

        data = []

        for sample_name in self.sample_list:
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

        layout = self.get_layout(
            cs.CSV_SCATTER_PLOT_TITLE, cs.CSV_SCATTER_PLOT_X_TITLE , cs.CSV_SCATTER_PLOT_Y_TITLE)

        fig = go.Figure(data=data, layout=layout)

        plot_fig = plot(fig, filename= os.path.join(
            cs.RESULT_DIR, self.config_name + cs.PLOT_RESULT))
        return plot_fig


    def get_layout(self, plot_title, x_title, y_title):
        """
        Returns layout for plotly
        Args:
            plot_title (str): Title of plot
            x_title (str): Title of x axis
            y_title (str): Title of y axis
        Returns:
            layout (plotly obj): Layout for plotly
        """

        layout = go.Layout(
            title=plot_title,
            xaxis=dict(
                title=x_title,
                type='log'
            ),
            yaxis=dict(
                title=y_title,
                type='log'
            )
        )

        return layout

    def get_correlation_and_wilcox_df(self, pandas_df):

        unique_identifier_name_x_list = []
        unique_identifier_name_y_list = []
        average_intensity_list_x = []
        average_intensity_list_y = []
        rows_list = []
        for index, row in pandas_df.iterrows():
            x = []
            y = []
            unique_identifier_x_list = []
            unique_identifier_y_list = []
            for sample_name in self.sample_list:
                sample_name_x = sample_name + "_x"
                sample_name_y = sample_name + "_y"
                x.append(row[sample_name_x])
                y.append(row[sample_name_y])

            unique_identifier_columns_x = ["compound", "label_x", "medRt_x"]
            unique_identifier_columns_y = ["compound", "label_y", "medRt_y"]
            for (column_name_x, column_name_y) in zip(unique_identifier_columns_x,
                                                      unique_identifier_columns_y):
                unique_identifier_x_list.append(row[column_name_x])
                unique_identifier_y_list.append(row[column_name_y])
            unique_identifier_x = " ".join(str(v) for v in unique_identifier_x_list)
            unique_identifier_y = " ".join(str(v) for v in unique_identifier_y_list)
            unique_identifier_name_x_list.append(unique_identifier_x)
            unique_identifier_name_y_list.append(unique_identifier_y)
            average_x = numpy.mean(x)
            average_y = numpy.mean(y)
            average_intensity_list_x.append(average_x)
            average_intensity_list_y.append(average_y)

            corr_pval_tuple = hf.get_corr_coff_and_pval(average_intensity_list_x,
                                                        average_intensity_list_y)
            row_list = [unique_identifier_x, unique_identifier_y, corr_pval_tuple[0],
                        corr_pval_tuple[1], corr_pval_tuple[2],
                        corr_pval_tuple[3], corr_pval_tuple[4]]
            rows_list.append(row_list)

        corr_wilcox_df = pandas.DataFrame(rows_list,
                                          columns=[cs.unique_identifier_man,
                                                   cs.unique_identifier_auto,
                                                   cs.corr_coff, cs.p_val, cs.logfc_auto_to_man,
                                             cs.avg_intensity_man, cs.avg_intensity_auto])
        return corr_wilcox_df
