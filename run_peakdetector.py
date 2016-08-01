
import os
from helper import helper
from config import config

def run(list_of_builds):

    for build_maven in list_of_builds:
        run_build(build_maven)

def run_build(build_maven):

    paths = config.paths(build_maven)
    constants = config.constants(build_maven)
    command_line_script = get_command_line_script(paths, constants)
    helper.run_command(command_line_script)

def get_command_line_script(paths, constants):

    command_line_script = ''

    command_line_script += paths.path_peakdetector
    list_of_mzxml_files = helper.get_list_of_files_in_directory(paths.inputdir, '.mzxml' )

    dict_of_paths = helper.get_dict_of_attributes_in_class(paths)

    dict_of_paths.pop('path_peakdetector')
    dict_of_paths.pop('inputdir')
    command_line_paths = helper.convert_dict_to_command_line_arguments(dict_of_paths)
    command_line_script += command_line_paths


    dict_of_constants = helper.get_dict_of_attributes_in_class(constants)
    command_line_constants = helper.convert_dict_to_command_line_arguments(dict_of_constants)
    command_line_script += command_line_constants


    for mzxml_file in list_of_mzxml_files:
        command_line_script += " " + mzxml_file
    return command_line_script
