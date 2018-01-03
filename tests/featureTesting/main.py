"""
This runs the feature testing
"""
import os
import run_cli
import compare_csv
from helper import helper_functions as hf
import constants_feature_testing as cs


def main():
    """
    Main function to run feature testing
    """

    config_files = hf.list_files(cs.CLI_CONFIGS_PATH, cs.CONFIG_EXTENSION)

    for config_file in config_files:
        compare_output(config_file)


def compare_output(config_path):
    """
    Compare output of one configration file
    Args:
        config_path (str): Path of configuration file
    """

    output_dir = cs.OUTPUT_PATH

    hf.delete_dir(output_dir)
    hf.make_dir(output_dir)
    cli = run_cli.RunCli()
    cli.run_cli(config_path)

    file_list = []
    file_list.append(os.path.join(output_dir, cs.OUTPUT_FILE_NAME))
    file_list.append(os.path.splitext(cs.CLI_CONFIGS_PATH)[0] + ".csv")
    comp = compare_csv.CompareCsvs(file_list, os.path.basename(cs.CLI_CONFIGS_PATH))
    comp.compare()
    hf.delete_dir(output_dir)


main()
