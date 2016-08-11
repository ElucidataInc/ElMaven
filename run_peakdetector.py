"""This module is used to create script(s) from the arguments in
configuration file and run the executable on the terminal using 
that script"""

import os
import sys
import time
import datetime
from os.path import join
from helper import helper
from config import config

class run_peakdetector():

    def __init__(self):
        ts = time.time()
        self.st = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d_%H:%M:%S')
        self.list_of_dict_of_argu = []
        
    def run(self, list_of_builds):

        for build_maven in list_of_builds:
            self.run_build(build_maven)

    def run_build(self, build_maven):

        input_paths = config.input_paths(build_maven)
        arguments = config.arguments(build_maven)

        list_of_CL_scripts = self.get_list_of_CL_scripts(input_paths, arguments)
        
        for CL_script in list_of_CL_scripts:
            helper.run_command(CL_script)

    def get_list_of_CL_scripts(self, input_paths, arguments):

        script_exec = self.add_path_exec_script(input_paths)
        script_inps = self.add_input_files_script(input_paths)

        self.list_of_dict_of_argu = self.get_list_of_dict_of_argu(arguments)

        list_of_CL_scripts = []
        for dict_of_arguments in self.list_of_dict_of_argu:

            script_argu = self.add_arguments_script(dict_of_arguments)
            CL_script = script_exec + script_argu + script_inps
            list_of_CL_scripts.append(CL_script)
        return list_of_CL_scripts

    def get_list_of_dict_of_argu(self, arguments):

        dict_of_arguments = helper.get_dict_of_attributes_in_class(arguments)

        list_of_tuples = self.get_tuple_if_value_list(dict_of_arguments)
        dict_of_arguments = self.del_key_if_in_tuple(dict_of_arguments, list_of_tuples)

        path = dict_of_arguments['outputdir']
        for key_value_tuple in list_of_tuples:

            del dict_of_arguments[key_value_tuple[0]]
            del dict_of_arguments['outputdir']
            os.makedirs(str(join(path, self.st, (key_value_tuple[0] + '_' + str(key_value_tuple[1])))))
            dict_of_arguments['outputdir'] = str(join(path, self.st, (key_value_tuple[0] + '_' + str(key_value_tuple[1]))))

            dict_of_arguments[key_value_tuple[0]] = key_value_tuple[1]
            self.list_of_dict_of_argu.append(dict_of_arguments.copy())

        return self.list_of_dict_of_argu
    
    def get_tuple_if_value_list(self, dict_of_arguments):

        list_of_tuples = []
        for key, value in dict_of_arguments.iteritems():
            dict_of_argu = {}
            if isinstance(value, list):
                for val in value:
                    list_of_tuples.append((key, val))
        return list_of_tuples

    def del_key_if_in_tuple(self, dict_of_arguments, list_of_tuples):

        for key_value_tuple in list_of_tuples:
            if key_value_tuple[0] in dict_of_arguments:
                dict_of_arguments[key_value_tuple[0]] = key_value_tuple[1]
        return dict_of_arguments

    def add_path_exec_script(self, input_paths):

        script_exec = input_paths.path_peakdetector
        return script_exec

    def add_arguments_script(self, dict_of_arguments):

        dict_of_arguments = self.set_run_function(dict_of_arguments)
        script_argu = helper.convert_dict_to_CL_arguments(dict_of_arguments)
        return script_argu

    def add_input_files_script(self, input_paths):

        list_of_mzxmls = \
        helper.get_list_of_files_in_directory_with_full_path(input_paths.inputdir, '.mzxml')

        script_inps = ''

        for mzxml_file in list_of_mzxmls:
            script_inps += " " + mzxml_file

        return script_inps

    def set_run_function(self, dict_of_arguments):

        run_function = config.variables.run_function

        dict_of_arguments = \
        self.run_function_pullIsotopes(run_function, dict_of_arguments)
        dict_of_arguments = \
        self.run_function_processAllSlices(run_function, dict_of_arguments)

        return dict_of_arguments

    def run_function_pullIsotopes(self, run_function, dict_of_arguments):

        if run_function in [1,2]:
            dict_of_arguments['pullIsotopes'] = 0
        elif run_function in [3,4]:
            dict_of_arguments['pullIsotopes'] = 15
        else:
            self.run_function_exit_message()

        return dict_of_arguments

    def run_function_processAllSlices(self, run_function, dict_of_arguments):

        if run_function in [1,3]:
            if 'db' in dict_of_arguments:
                pass
            else:
                sys.exit("No Database found")
        elif run_function in [2,4]:
            dict_of_arguments.pop("db", None)
            dict_of_arguments['processAllSlices'] = 1
        else:
            self.run_function_exit_message()

        return dict_of_arguments

    def run_function_exit_message(self):

        sys.exit("Wrong configuration selected\n" \
                    "run_function can be following:\n" \
                    "run_function = 1 (processCompounds)\n" \
                    "run_function = 2 (processAllSlices)\n" \
                    "run_function = 3 (processCompounds & pullIsotopes)\n"\
                    "run_function = 4 (processAllSlices & pullIsotopes)\n")

run_peakdetector = run_peakdetector()