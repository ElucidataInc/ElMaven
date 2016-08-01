
import sys
import pandas as pd
from os.path import join
from helper import helper

def compare(listBuilds):

    if len(listBuilds) < 2:
        sys.exit('You need 2 or more outputs to compare')

    listOfDataFrames = getListOfDataFrames(listBuilds)
    uniqueListOfCompounds = getIntersectedListOfCompounds(listOfDataFrames)
    listOfIntersectedDataFrames = getListOfIntersectedDataFrames(listOfDataFrames, uniqueListOfCompounds)

def getListOfDataFrames(listBuilds):

    listOfDataFrames = []
    for buildMaven in listBuilds:
        paths = config.paths(buildMaven)
        df = helper.readCsvPandas(join(paths.outputdir, 'compounds.csv'))
        listOfDataFrames.append(df)
    
    return listOfDataFrames

def getIntersectedListOfCompounds(listOfDataFrames):

    listOfLists = []

    for df in listOfDataFrames:
        listOfCompounds = getListOfCompounds(df)
        listOfLists.append(listOfCompounds)
    
    uniqueListOfCompounds = helper.getIntersectionOfList(listOfLists)
    return uniqueListOfCompounds

def getListOfCompounds(df):
    listOfCompounds = df['compound'].unique()
    return listOfCompounds

def getListOfIntersectedDataFrames(listOfDataFrames, uniqueListOfCompounds)

    listOfIntersectedDataFrames = []

    for df in listOfDataFrames:

        listOfIntersectedDataFrames.append(df.loc[df['compound'].isin(uniqueListOfCompounds)])
    
    return listOfIntersectedDataFrames

for compound in uniqueListOfCompounds:
    compound_topThree_1 = topThree_1[topThree_1['compound'] == compound]
    compound_topThree_2 = topThree_2[topThree_2['compound'] == compound]
    for element in xrange(len(compound_topThree_1)):
        # print compound_topThree_1.iloc[[element]]
    
    break


