"""
This module compares different output of cli

List of functions:

1. compare: Compare all the outputs generated from multiple
    cli instances

"""
import pandas as pd


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

        df1 = pd.read_csv('test1.tab', sep='\t',)
        df2 = pd.read_csv('test2.tab', sep='\t',)

        df3 = pd.merge(df1, df2, on=["compoundId", "compound", "formula"])

        df3.to_csv("test3.csv", index=False, encoding='utf-8')
    