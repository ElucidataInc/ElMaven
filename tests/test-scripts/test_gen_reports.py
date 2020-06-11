#!/usr/bin/env python3

# coding: utf-8

import shutil
import subprocess
import sys
import tempfile
import os.path as path
from urllib.request import Request, urlopen
from urllib.error import URLError

import pandas as pd


script_url = (
    'https://gist.githubusercontent.com/saifulbkhan/'
    '045001391c2aee5b0dd5fbf059f65b3b/raw/'
    'c14cbf8ccce693c08f8ec7980367bd5c073653a8/gen_reports.py'
)

bin_path = path.abspath(path.join(path.dirname(__file__), '..', '..', 'bin'))
peakdetector_path = path.join(bin_path, "peakdetector")
samples_path = path.join(bin_path, "methods", "091215_120*.mzXML")


def download_script():
    req = Request(script_url)
    try:
        response = urlopen(req)
    except URLError as e:
        if hasattr(e, 'reason'):
            print('Error: failed to reach a server.')
            print('Reason:', e.reason)
        elif hasattr(e, 'code'):
            print('Error: server could not fulfill the request.')
            print('Error code:', e.code)
    else:
        with tempfile.NamedTemporaryFile(delete=False) as tmp_file:
            shutil.copyfileobj(response, tmp_file)
        return tmp_file.name
    return None


def dummy_db():
    db_contents = "{}\n{}\n".format("name,formula,id,rt",
                                    "UMP,C9H13N2O9P,C00105,10")
    tmp_file = tempfile.NamedTemporaryFile(mode='w+',
                                           delete=False,
                                           suffix=".csv")
    tmp_file.write(db_contents)
    return tmp_file.name


def generate_reports(output_dir, project_path, peakdetector_args, has_db=True):
    script_file_path = download_script()
    if not script_file_path:
        print("Error: unable to download report-generation script.")
        sys.exit(1)

    # run command-line utility "peakdetector", and save the analysis
    subprocess.run(" ".join(peakdetector_args), shell=True, check=True)

    # make sure that the emDB project was generated
    if not path.isfile(project_path):
        print("Error: no project file was created.")
        sys.exit(1)

    # run report-generation on the emDB
    subprocess.run(" ".join(["python3",
                             script_file_path,
                             "-o", output_dir,
                             "-p", project_path]),
                   shell=True,
                   check=True)

    # make sure that the four expected files were created
    samples_report = project_path.rstrip(".emDB") + ".dat.samples.tab"
    compounds_report = project_path.rstrip(".emDB") + ".dat.cpds.tab"
    peakgroups_report = project_path.rstrip(".emDB") + ".dat.peakgroups.tab"
    peaks_report = project_path.rstrip(".emDB") + ".dat.peaks.tab"
    if not path.isfile(samples_report):
        print("Error: samples report was not found where expected.")
        sys.exit(1)
    if has_db and not path.isfile(compounds_report):
        print("Error: compounds report was not found where expected.")
        sys.exit(1)
    if not path.isfile(peakgroups_report):
        print("Error: peakgroups report was not found where expected.")
        sys.exit(1)
    if not path.isfile(peaks_report):
        print("Error: peaks report were was found where expected.")
        sys.exit(1)
    return (samples_report, compounds_report, peakgroups_report, peaks_report)


def match_contents(expected_file_name, generated_report_path, ignore_cols=[]):
    expected_report_path = path.join(path.dirname(__file__), expected_file_name)
    exp_df = pd.read_csv(expected_report_path, sep='\t', header=None)
    gen_df = pd.read_csv(generated_report_path, sep='\t', header=None)
    if len(ignore_cols) > 0:
        exp_df.drop(exp_df.columns[ignore_cols], axis=1, inplace=True)
        gen_df.drop(gen_df.columns[ignore_cols], axis=1, inplace=True)
    pd.testing.assert_frame_equal(exp_df,
                                  gen_df, 
                                  check_exact=False, 
                                  check_less_precise=3)


def test_regular_lcms_reports(output_dir):
    print("=== Testing reports for regular LC-MS analysis ===")

    project_name = "regular.raw.emDB"
    project_path = path.join(output_dir, project_name)
    reports = generate_reports(output_dir,
                               project_path,
                               [peakdetector_path,
                                "-c", "1",
                                "-d", dummy_db(),
                                "-f", "0000",
                                "-s", project_path,
                                samples_path])
    samples_report, compounds_report, peakgroups_report, peaks_report = reports

    match_contents('expected.dat.samples.tab', samples_report)
    match_contents('expected-regular.raw.dat.cpds.tab', compounds_report)
    match_contents('expected-regular.raw.dat.peakgroups.tab',
                   peakgroups_report,
                   ignore_cols=[17]) # ignore comparison of group rank
    match_contents('expected-regular.raw.dat.peaks.tab', peaks_report)

    print("Tests passed!\n")


def test_labeled_lcms_reports(output_dir):
    print("=== Testing reports for labeled LC-MS analysis ===")

    project_name = "labeled.emDB"
    project_path = path.join(output_dir, project_name)
    reports = generate_reports(output_dir,
                               project_path,
                               [peakdetector_path,
                                "-c", "1",
                                "-d", dummy_db(),
                                "-f", "0001",
                                "-s", project_path,
                                samples_path])
    samples_report, compounds_report, peakgroups_report, peaks_report = reports

    match_contents('expected.dat.samples.tab', samples_report)
    match_contents('expected-labeled.dat.cpds.tab', compounds_report)
    match_contents('expected-labeled.dat.peakgroups.tab',
                   peakgroups_report,
                   ignore_cols=[17]) # ignore comparison of group rank
    match_contents('expected-labeled.dat.peaks.tab', peaks_report)

    print("Tests passed!\n")


def test_feature_detection_reports(output_dir):
    print("=== Testing reports for feature-detection analysis ===")

    project_name = "features.emDB"
    project_path = path.join(output_dir, project_name)
    reports = generate_reports(output_dir,
                               project_path,
                               [peakdetector_path,
                                "-e", "1",
                                "-i", "1500000",
                                "-I", "1.0",
                                "-s", project_path,
                                samples_path],
                                has_db=False)
    samples_report, compounds_report, peakgroups_report, peaks_report = reports

    match_contents('expected.dat.samples.tab', samples_report)
    match_contents('expected-features.dat.peakgroups.tab',
                   peakgroups_report,
                   ignore_cols=[17]) # ignore comparison of group rank
    match_contents('expected-features.dat.peaks.tab', peaks_report)

    print("Tests passed!\n")


def main():
    output_dir = "reports"

    test_regular_lcms_reports(output_dir)
    test_labeled_lcms_reports(output_dir)
    test_feature_detection_reports(output_dir)

    # remove all contents from output directory
    shutil.rmtree(output_dir)


if __name__ == "__main__":
    main()
