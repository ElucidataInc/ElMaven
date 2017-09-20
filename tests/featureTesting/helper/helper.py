
import os
import pandas as pd
from os.path import isfile, join

def get_list_of_files_in_directory_with_full_path(path_to_directory, extension):

    list_of_files = []
    for file in os.listdir(path_to_directory):
        if file.endswith(extension):
            list_of_files.append(join(path_to_directory, file))

    return list_of_files

def get_dict_of_attributes_in_class(class_object):

    dict_of_attributes = vars(class_object)

    return dict_of_attributes

def convert_dict_to_CL_arguments(input_dict):

    keys_in_dict = input_dict.keys()
    CL_script = ""

    for argument in keys_in_dict:
        CL_script += " --" + argument + " " + str(input_dict[argument])

    return CL_script

def run_command(CL_script):

    os.system(CL_script)

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

def export_file(input_string, path_output_file):

    f = open(path_output_file, "w")
    f.write(input_string)
    f.close()

def get_list_of_files_in_directory_without_extension(path_to_directory):

    list_files = os.listdir(path_to_directory)

    list_file_names = []

    for file in list_files:

        list_file_names.append(file.split('.')[0])
    
    return list_file_names
