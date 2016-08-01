
import os
from helper import helper
from config import config

def run(listBuilds):

    for buildMaven in listBuilds:
        runBuild(buildMaven)

def runBuild(buildMaven):

    paths = config.paths(buildMaven)
    constants = config.constants(buildMaven)
    commandLineScript = getCommandLineScript(paths, constants)
    helper.runCommand(commandLineScript)

def getCommandLineScript(paths, constants):

    commandLineScript = ''

    commandLineScript += paths.pathPeakDetector
    listOfMzxmlFiles = helper.getListOfFilesInDirectory(paths.inputdir, '.mzxml')

    dictOfPaths = helper.getDictOfAttributesInClass(paths)

    dictOfPaths.pop('pathPeakDetector')
    dictOfPaths.pop('inputdir')
    commandLinePaths = helper.convertDictToCommandLineArguments(dictOfPaths)
    commandLineScript += commandLinePaths


    dictOfConstants = helper.getDictOfAttributesInClass(constants)
    commandLineConstants = helper.convertDictToCommandLineArguments(dictOfConstants)
    commandLineScript += commandLineConstants


    for mzxmlFile in listOfMzxmlFiles:
        commandLineScript += " " + mzxmlFile
    
    return commandLineScript
