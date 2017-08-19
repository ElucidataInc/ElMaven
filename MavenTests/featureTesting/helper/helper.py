"""
Helper functions for Feature Testing

List of all the helper functions:

1. list_files: List all the files in a given directory with
specified extenstion

2. is_exe: Check if the specified file is exe or not

3. check_exe_exists: Check if the specified exe exists in path

"""

import os


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

    for fname in os.listdir(dir_path):
        if fname.endswith(file_extension):
            files.append(os.path.join(dir_path, fname))

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
