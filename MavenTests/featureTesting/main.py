"""
This runs the feature testing
"""
import run_cli
import compare
from config import config
from helper import helper

def main():
    """
    Main function to run feature testing
    """
    conf = config.Config()

    xml_files = helper.list_files(conf.cli_configs_path, ".xml")
