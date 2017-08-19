"""
Helper functions for Feature Testing

List of all the helper functions:

1. list_files: List all the files in a given directory with
specified extenstion

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

    for dir_file in os.listdir(dir_path):
        if dir_file.endswith(file_extension):
            files.append(os.path.join(dir_path, dir_file))

    return files
