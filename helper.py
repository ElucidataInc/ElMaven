
import os
from os.path import isfile, join

def getListOfFilesInDirectory(pathToDirectory, extension):

    listOfFiles = []
    for file in os.listdir(pathToDirectory):
        if file.endswith(extension):
            listOfFiles.append(join(pathToDirectory, file))

    return listOfFiles

def getDictOfAttributesInClass(classObject):

    dictOfAttributes = vars(classObject)

    return dictOfAttributes

def convertDictToCommandLineArguments(ipDict):

    keysInDict = ipDict.keys()
    commandLineScript = ""

    for argument in keysInDict:
        commandLineScript += " --" + argument + " " + str(ipDict[argument])

    return commandLineScript

def runCommand(commandLineScript):

    os.system(commandLineScript)
