"""
This module runs peakdetector cli and generates the output

List of all the functions in this module:

1. run_cli: Runs one instance of peakdetector with specified arguments
2. run_multiple_cli: Runs multiple instances of peakdetector to generate
    multiple outputs
3. get_config_paths: Get paths of configuration files

"""

from helper import helper
from config import config


class RunCli(object):
    """
    This class runs multiple instances of peakdetector cli and generates
    the output
    """

    def __init__(self):
        self.config = config.Config()

    def run_cli(self, xml_path):
        """
        Runs one instance of peakdetector with specified arguments

        Args:
            xml_path: Path of configuration xml file

        """
        exe_path = self.config.peakdetector_path
        if helper.check_exe_exists(exe_path):
            args = ["--xml"]
            args.append(xml_path)
            helper.run_exe(exe_path, args)
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
        config_path = self.config.cli_configs_path

        config_files = helper.list_files(config_path, ".xml")
        return config_files
