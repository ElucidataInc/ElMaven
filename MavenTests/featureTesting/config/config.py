"""
This module handles config file(s)

List of all the class functions:

1. load_json: Loads json from the specified path

"""
import json


class Config(object):
    """
    Loads and stores config file(s) data
    """

    def __init__(self):
        pass

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
