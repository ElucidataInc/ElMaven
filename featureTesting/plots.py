import os
from os.path import join
import compare_output
from config import config
from helper import helper
import pandas as pd
import plotly
import plotly.graph_objs as go

class plots():

    def __init__(self):
        self.st = config.variables.st
        self.dict_of_changes = compare_output.compare_output.dict_of_changes
        self.dict_of_dfs = compare_output.compare_output.dict_of_dfs

    def plot_comparison(self, list_of_builds):

        for key, value in self.dict_of_changes.iteritems():

            os.makedirs(join(config.variables.plots_comparison,self.st, key))

            trace = go.Scatter(
                x = self.dict_of_dfs[key][0]['medRt'].values.tolist(),
                y = self.dict_of_dfs[key][1]['medRt'].values.tolist(),
                mode = 'markers'
            )

            data = [trace]

            plotly.offline.plot(data, filename = join(config.variables.plots_comparison,self.st, key, 'retentionTime'))

            list_mzxml_file_names = helper.get_list_of_files_in_directory_without_extension(config.input_paths(0).inputdir)

            for mzxml in list_mzxml_file_names:

                trace = go.Scatter(
                    x = self.dict_of_dfs[key][0][mzxml].values.tolist(),
                    y = self.dict_of_dfs[key][1][mzxml].values.tolist(),
                    mode = 'markers'
                )    
                data = [trace]

                plotly.offline.plot(data, filename = join(config.variables.plots_comparison,self.st, key, 'intensity_' + mzxml))

plots = plots()
