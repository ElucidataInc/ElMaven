
import xml.etree.cElementTree as ET
import compare_output
from config import config

class xml_generator:

    def __init__ (self):
        pass
    
    def get_xml(self):

        test_func_list = config.variables.test_func_list

        TestCase = self.preprocessing()

        for testFunc in test_func_list:
            self.test_func(TestCase, testFunc)

        self.write_xml(TestCase)


    def preprocessing(self):

        TestCase = ET.Element("TestCase", name = "featureTesting")
        Environment = ET.SubElement(TestCase, "Environment")

        ET.SubElement(Environment, "QtVersion").text = "4.8.6"
        ET.SubElement(Environment, "QTestVersion").text = "4.8.6"

        return TestCase

    def test_func(self, TestCase, testFunc):

        if compare_output.compare_output.validity_builds[testFunc]:

            TestFunction = ET.SubElement(TestCase, "TestFunction", name = testFunc)
            ET.SubElement(TestFunction, "Incident", type="pass", file="", line="0")

        else:

            TestFunction = ET.SubElement(TestCase, "TestFunction", name = testFunc)
            ET.SubElement(TestFunction, "Incident", type="fail", file="", line="0")


    def write_xml(self, TestCase):

        tree = ET.ElementTree(TestCase)
        tree.write("featureTesting.xml")

xml_generator = xml_generator()