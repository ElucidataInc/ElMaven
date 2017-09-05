"""
This module compares different output of cli

List of functions:

1. compare: Compare all the outputs generated from multiple
    cli instances
2. load_files: Loads every file present in the input list into a
    pandas dataframe
3. dfs_groupby: Converts list of pandas dataframes into groupby
    objects based on the specified columns
4. get_keys_groupby: Get list of keys of all groupby objects
5. get_groups: Get groups from list of groupby objects basedon key
6. get_indexes: Get indexes of each pandas df in the list

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
        groupby_list = self.dfs_groupby(df_list, col_list)
        keys = self.get_keys_groupby(groupby_list)
        common_keys = helper.intersection_lists(keys)

        for key in common_keys:
            groups_list = self.get_groups(groupby_list, key)
            group_indexes = self.get_indexes(groups_list)

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

    def get_keys_groupby(self, groupby_list):
        """
        Get list of keys of all groupby objects
        Args:
            groupby_list (list): List of groupby objects
        Returns:
            keys (list): List of keys of groupby objects
        """
        keys = []
        for groupby_obj in groupby_list:
            key = groupby_obj.groups.keys()
            keys.append(key)

        return keys

    def get_groups(self, groupby_list, key):
        """
        Get groups from list of groupby objects based on key
        Args:
            groupby_list (list): List of groupby objects
            key (tuple): Key for accessing the group in groupby
                object
        Returns:
            groups_list (list): List of pandas dataframes for the
            specified key
        """
        groups_list = []

        for groupby_obj in groupby_list:
            group = groupby_obj.get_group(key)
            groups_list.append(group)

        return groups_list

    def get_indexes(self, groups_list):
        """
        Get indexes of each pandas df in the list
        Args:
            groups_list (list): List of pandas dataframes
        Returns:
            group_indexes (list): List of indexes
        """
        group_indexes = []

        for group in groups_list:
            index = group.index.values
            group_indexes.append(index)

        return group_indexes
