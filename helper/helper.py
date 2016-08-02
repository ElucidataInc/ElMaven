
import os
import pandas as pd
from os.path import isfile, join

def get_list_of_files_in_directory(path_to_directory, extension):

    list_of_files = []
    for file in os.listdir(path_to_directory):
        if file.endswith(extension):
            list_of_files.append(join(path_to_directory, file))
    print list_of_files
    return list_of_files

def get_dict_of_attributes_in_class(class_object):

    dict_of_attributes = vars(class_object)

    return dict_of_attributes

def convert_dict_to_command_line_arguments(input_dict):

    keys_in_dict = input_dict.keys()
    command_line_script = ""

    for argument in keys_in_dict:
        command_line_script += " --" + argument + " " + str(input_dict[argument])

    return command_line_script

def run_command(command_line_script):

    os.system(command_line_script)

def read_csv_pandas(path_to_csv):
    df = pd.read_csv(path_to_csv)
    return df

def get_intersection_of_list(list_of_lists):

    a = list_of_lists[0]
    b= list_of_lists[1]
    intersected_list = list(set(a) & set(b))

    return intersected_list

def create_list_of_empty_dfs(size_of_list):

    list_of_empty_dfs = []
    for i in xrange(size_of_list):
        list_of_empty_dfs.append(pd.DataFrame())

    return list_of_empty_dfs
