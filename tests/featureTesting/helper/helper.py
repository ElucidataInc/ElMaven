"""
Helper functions for Feature Testing

List of all the helper functions:

1. list_files: List all the files in a given directory
    with specified extenstion

2. is_exe: Check if the specified file is exe or not

3. check_exe_exists: Check if the specified exe exists in
    path

4. run_exe: Run the executable with arguments

5. parse_arguments: Parse arguments. Supported types :
    string, list & dict

6. intersection_lists: Returns intersection of lists of any data
    type

7. load_df: Load specified file path into a pandas dataframe.
    Supported files types are .csv or .tab

8. get_combinations: Get all the combinations of values in different
    lists

9. get_column_values: Get all the values with index of specified
    column from a pandas dataframe

10. merge_dfs: Merge multiple dataframes based on specified columns

11. delete_dir: Delete the directory and all its contents

12. make_dir: Create directory recursively

"""

import os
import shutil
import itertools
import subprocess
import pandas as pd


def list_files(dir_path, file_extension):
    """
    List all the files in a given directory with specified
    extenstion.

    Args:
        dir_path (str): Directory path where files are present

        file_extenstion (str): Extenstion of files needs to
            returned

    Returns:
        files (list): List of files with specified extenstion
            and directory
    """
    files = []

    for root, dirnames, filenames in os.walk(dir_path):
        for fname in filenames:
            if fname.endswith(file_extension):
                files.append(os.path.join(root, fname))

    return files


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
        raise ValueError("Type not supported for arguments")

    return list_args

def intersection_lists(lists):
    """
    Returns intersection of lists of any data type

    Args:
        lists(list): List of lists

    Returns:
        intersected_list(list): List after intersection
    """

    return list(set.intersection(*map(set, lists)))

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


    if fextension == ".tab":
        pdataframe = pd.read_csv(fpath, sep='\t')
        return pdataframe
    elif fextension == ".csv":
        pdataframe = pd.read_csv(fpath, sep=',')
        return pdataframe
    else:
        raise ValueError(fextension + " file format not supported")

def get_combinations(list_of_lists):
    """
    Get all the combinations of values in different lists
    Args:
        list_of_lists(list): List of lists where each list consist some
            int values
    Returns:
        combinations(list): List of combinations
    """

    combinations = list(itertools.product(*list_of_lists))

    return combinations

def get_column_values(pandas_df, column_name):
    """
    Get all the values with index of specified column
    from a pandas dataframe
    Args:
        pandas_df (df): Pandas dataframe
        column_name (str): Name of column
    Returns:
        values_wth_index (dict): Column values with index
    """

    values_wth_index = {}

    for index, row in pandas_df.iterrows():
        values_wth_index[index] = row[column_name]

    return values_wth_index

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
        print "Not able to delete " + dir_path

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
        print "Not able to create " + dir_path
