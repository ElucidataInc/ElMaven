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

"""

import os
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
