
import os
import config
import helper


commandLineScript = ''

commandLineScript += config.paths.pathPeakDetector
listOfMzxmlFiles = helper.getListOfFilesInDirectory(config.paths.inputdir, '.mzxml')

dictOfPaths = helper.getDictOfAttributesInClass(config.paths)

dictOfPaths.pop('pathPeakDetector')
dictOfPaths.pop('inputdir')
commandLinePaths = helper.convertDictToCommandLineArguments(dictOfPaths)
commandLineScript += commandLinePaths


dictOfConstants = helper.getDictOfAttributesInClass(config.constants)
commandLineConstants = helper.convertDictToCommandLineArguments(dictOfConstants)
commandLineScript += commandLineConstants


for mzxmlFile in listOfMzxmlFiles:
    commandLineScript += " " + mzxmlFile

os.system(commandLineScript)
