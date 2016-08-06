
import os
from helper import helper
from config import config
class run_peakdetector():

    def __init__(self):
        pass

    def run(self, list_of_builds):

        for build_maven in list_of_builds:
            self.run_build(build_maven)

    def run_build(self, build_maven):

        paths = config.paths(build_maven)
        constants = config.constants(build_maven)
        command_line_script = self.get_command_line_script(paths, constants)
        helper.run_command(command_line_script)

    def get_command_line_script(self, paths, constants):

        command_line_script = ''

        command_line_script += paths.path_peakdetector
        list_of_mzxml_files = helper.get_list_of_files_in_directory(paths.inputdir, '.mzXML' )

        dict_of_paths = helper.get_dict_of_attributes_in_class(paths)

        dict_of_paths.pop('path_peakdetector')
        dict_of_paths.pop('inputdir')
        command_line_paths = helper.convert_dict_to_command_line_arguments(dict_of_paths)
        command_line_script += command_line_paths


        dict_of_constants = helper.get_dict_of_attributes_in_class(constants)        
        dict_of_constants = self.set_run_function(dict_of_constants)

        command_line_constants = helper.convert_dict_to_command_line_arguments(dict_of_constants)
        command_line_script += command_line_constants


        for mzxml_file in list_of_mzxml_files:
            command_line_script += " " + mzxml_file
        return command_line_script

    def set_run_function(self, dict_of_constants):

        run_function = config.variables.run_function
        if run_function in (1,3):
            if 'db' in dict_of_constants:
                pass
            else:
                sys.exit("No Database found")

            if run_function in 1:
                dict_of_constants['pullIsotopes'] = 0
            else:
                dict_of_constants['pullIsotopes'] = 15
        else if run_function in (2,4):

            dict_of_constants.pop("db", None)
            dict_of_constants['processAllSlices'] = 1
            if run_function in 2:
                dict_of_constants['pullIsotopes'] = 0
            else:
                dict_of_constants['pullIsotopes'] = 15



run_peakdetector = run_peakdetector()