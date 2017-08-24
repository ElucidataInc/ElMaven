"""
This module handles config file(s)

List of all the class functions:

1. load_json: Loads json from the specified path

2. get_json_keys: Return json keys after loading json

"""
import os
import json


class Config(object):
    """
    Loads and stores config file(s) data
    """

    def __init__(self):
        current_file_path = os.path.dirname(__file__)
        config_path = os.path.join(current_file_path, "config.json")
        self.config_keys = self.get_json_keys(config_path)
        self.config_data = self.load_json(config_path)
        self.peakdetector_path = self.config_data["peakdetector_path"]
        self.cli_configs_path = self.config_data["cli_configs_path"]

    def get_json_keys(self, json_path):
        """
        Return json keys after loading json

        Args:
            json_path (str): Path of json file

        Return:
            json_keys (list): Keys of json file
        """

        json_data = self.load_json(json_path)
        json_keys = json_data.keys()
        return json_keys

    def load_json(self, json_path):
        """
        Loads json from the speicified path

        Args:
            json_path (str): Path of the json file

        Return:
            data (json object): Json encoded object

        """

        with open(json_path) as json_data_file:
            data = json.load(json_data_file)

        return data
