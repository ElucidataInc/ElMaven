"""
Helper functions for Feature Testing

List of all the helper functions:

1. list_files: List all the files in a given directory with
specified extenstion

2. is_exe: Check if the specified file is exe or not

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
