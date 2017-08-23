"""
This module handles config file(s)

List of all the class functions:

1. load_json: Loads json from the specified path

2. get_json_keys: Return json keys after loading json

"""
import json


class Config(object):
    """
    Loads and stores config file(s) data
    """

    def __init__(self):
        pass

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
