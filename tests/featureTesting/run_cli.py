"""
This module runs peakdetector cli and generates the output

List of all the functions in this module:

1. run: This function runs multiple instances of cli using different config
    files which results in different output
2. run_cli: Runs one instance of peakdetector with specified arguments
3. run_multiple_cli: Runs multiple instances of peakdetector to generate
    multiple outputs
4. get_config_paths: Get paths of configuration files

"""

from helper import helper as hf
import constants_feature_testing as cs

class RunCli(object):
    """
        This class runs multiple instances of peakdetector cli and generates
        the output
        """

    def __init__(self):
        self.peakdetector_path = cs.PEAK_DETECTOR_PATH


    def run(self):
        """
        This function runs multiple instances of cli using different config
        files which results in different output
        """
        config_files = self.get_config_paths()
        self.run_multiple_cli(config_files)


    def run_cli(self, xml_path):
        """
        Runs one instance of peakdetector with specified arguments
        Args:
            xml_path: Path of configuration xml file
        """
        exe_path = self.peakdetector_path
        if hf.check_exe_exists(exe_path):
            args = ["--xml"]
            args.append(xml_path)
            hf.run_exe(exe_path, args)
        else:
            raise ValueError("Peakdetector CLI exe not found")


    def run_multiple_cli(self, xml_paths):
        """
        Runs multiple instances of peakdetector to generate multiple
        outputs
        """

        for path in xml_paths:
            self.run_cli(path)

    def get_config_paths(self):
        """
        Get paths of configuration files
        Returns:
            config_files: List of paths of config files
        """

        config_files = hf.list_files(cs.CLI_CONFIGS_PATH, ".xml")
        return config_files
