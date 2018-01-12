from collections import OrderedDict

from tests.featureTesting import helper_functions as hf


def test_dict_diff():
    path1 = "../data/test_method.method"
    path2 = "../data/test_method_changed.method"
    result = hf.dict_diff(path1, path2)
    print result
    expected_result = {'minSignalBaseLineRatio': ['2', '3'], 'minGroupIntensity': ['1e+6', '1000']}
    assert result == expected_result


