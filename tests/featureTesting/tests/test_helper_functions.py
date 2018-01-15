from tests.featureTesting import helper_functions as hf


def test_dict_diff():
    path1 = "../data/test_method.method"
    path2 = "../data/test_method_changed.method"
    result = hf.dict_diff(path1, path2)
    print result
    expected_result = {'minSignalBaseLineRatio': ['2', '3'], 'minGroupIntensity': ['1e+6', '1000']}
    assert result == expected_result


def test_get_basename_url():
    url = "../data/test_config1.csv"
    result = hf.get_basename_url(url)
    expected_result = "test_config1.csv"
    assert result == expected_result


def test_list_files():
    dir_path = "../data/"
    file_extenson = ".csv"
    result = hf.list_files(dir_path, file_extenson)
    expected_result = ["../data/test_config1.csv","../data/test_config2.csv"]
    assert result == expected_result


