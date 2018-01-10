import helper_functions as hf


class CompareParameters(object):
    """
    This class compares two parameters files and gives the differences as a dict.
    """
    def __init__(self, file_1_path, file_2_path):
        self.file_1_path = file_1_path
        self.file_2_path = file_2_path

    def parameters_comparison(self):
        """
            This function first reads the two method files (xml files) for both (for example: manual
             and automated curated datasets) the datasets and returns the parameters which
             are different in both the files as a dict. This dict has parameter names which has
             different values in both the datasets as key and their respective
             values in a list for that key.

            :param file1_path: file path of dataset one

            :param file2_path: file path of the other dataset

            :return: A dict of parameters which are different in between both the datsets.
            for example: {'compoundRTWindow': ['0.5', '0.3'],
            'baseline_smoothingWindow': ['6', '4'], 'minSignalBlankRatio': ['0', '3'],
            'maxRt': ['20', '2'], 'ppmMerge': ['20', '2']}

            """
        param_comp = hf.dict_diff(self.file_1_path, self.file_2_path)
        return param_comp
