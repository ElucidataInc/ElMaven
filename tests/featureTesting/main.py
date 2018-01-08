"""
This runs the feature testing
"""
import os
import run_cli
import compare_csv
from helper import helper as hf
import constants_feature_testing as cs
import compare_json



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
    file_list_csv = []
    file_list_csv.append(os.path.join(output_dir,cs.OUTPUT_FILE_NAME_CSV))
    file_list_csv.append(os.path.splitext(config_path)[0] + ".csv")
    comp_csv = compare_csv.CompareCsvs(file_list_csv, os.path.basename(config_path))
    comp_csv.compare()
    file_list_json = []
    file_list_json.append(os.path.join(output_dir,cs.OUTPUT_FILE_NAME_JSON))
    file_list_json.append(os.path.splitext(config_path)[0] + ".json")
    comp_json = compare_json.CompareJsons(file_list_json[0],file_list_json[1])
    comp_json.summary_report()
    hf.delete_dir(output_dir)

main()
