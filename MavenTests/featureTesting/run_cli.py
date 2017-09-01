"""
This module runs peakdetector cli and generates the output

List of all the functions in this module:

1. run_peakdetector
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

    def run_peakdetector(self, xml_path):
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
