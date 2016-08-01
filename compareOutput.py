import os
import sys
import pandas as pd
from os.path import join
from helper import helper
from config import config
from scipy import stats

def compare(listBuilds):

    if len(listBuilds) < 2:
        sys.exit('You need 2 or more outputs to compare')

    listOfDfs = getListOfDfs(listBuilds)
    uniqueListOfCompounds = getIntersectedListOfCompounds(listOfDfs)
    listOfIntersectedDfs = getListOfIntersectedDfs(listOfDfs, uniqueListOfCompounds)
    listOfDfsWithSameMzRt = getListOfDfsWithSameMzRt(listOfIntersectedDfs, uniqueListOfCompounds)

    listMzXmlFiles = os.listdir(config.paths(0).inputdir)

    listMzXmlFilesNames = []

    for mzXmlFile in listMzXmlFiles:

        listMzXmlFilesNames.append(mzXmlFile.split('.')[0])

    lenOfDf = len(listOfDfsWithSameMzRt[0])

    listOfR2Values = []

    for i in xrange(lenOfDf):
        df_1 = listOfDfsWithSameMzRt[0].iloc[[i]][listMzXmlFilesNames].values.tolist()[0]
        df_2 = listOfDfsWithSameMzRt[1].iloc[[i]][listMzXmlFilesNames].values.tolist()[0]
        df_1 = [float(i) for i in df_1]
        df_2 = [float(i) for i in df_2]
        slope, intercept, r_value, p_value, std_err = stats.linregress(df_1,df_2)
        r2_value = round(r_value ** 2,3)
        listOfR2Values.append(r2_value)
    print listOfR2Values

def getListOfDfs(listBuilds):

    listOfDfs = []
    for buildMaven in listBuilds:
        paths = config.paths(buildMaven)
        df = helper.readCsvPandas(join(paths.outputdir, 'compounds.csv'))
        listOfDfs.append(df)
    
    return listOfDfs

def getIntersectedListOfCompounds(listOfDfs):

    listOfLists = []

    for df in listOfDfs:
        listOfCompounds = getListOfCompounds(df)
        listOfLists.append(listOfCompounds)
    
    uniqueListOfCompounds = helper.getIntersectionOfList(listOfLists)
    return uniqueListOfCompounds

def getListOfIntersectedDfs(listOfDfs, uniqueListOfCompounds):

    listOfIntersectedDfs = []

    for df in listOfDfs:
        listOfIntersectedDfs.append(df.loc[df['compound'].isin(uniqueListOfCompounds)])
    
    return listOfIntersectedDfs

def getListOfDfsWithSameMzRt(listOfDfs, uniqueListOfCompounds):

    listOfDfsWithSameMzRt = helper.createListOfEmptyDfs(len(listOfDfs))

    for compound in uniqueListOfCompounds:
        list_compound_df = []
        for df in listOfDfs:
            compound_df = df[df['compound'] == compound]
            list_compound_df.append(compound_df)
        
        listOfDfsWithSameMzRtCompound = compareMzRt(list_compound_df)

        for i in xrange(len(listOfDfsWithSameMzRtCompound)):
            listOfDfsWithSameMzRt[i] = listOfDfsWithSameMzRt[i].append(listOfDfsWithSameMzRtCompound[i])
    
    return listOfDfsWithSameMzRt

def compareMzRt(listOfDfs):

    df1 = listOfDfs[0]
    df2 = listOfDfs[1]
    df1_len = len(df1.index)
    df2_len = len(df2.index)

    mzDifference = 5*10e-6
    rtDifference = 0.01

    modifiedDf_1 = pd.DataFrame()
    modifiedDf_2 = pd.DataFrame()


    for i in xrange(df1_len):
        for j in xrange(df2_len):

            df1_currentRow = df1.iloc[[i]]
            df2_currentRow = df2.iloc[[j]]

            if abs(float(df2_currentRow['medMz']) - float(df1_currentRow['medMz'])) < mzDifference:
                if abs(float(df2_currentRow['medRt']) - float(df1_currentRow['medRt'])) < rtDifference:
                    modifiedDf_1 = modifiedDf_1.append(df1_currentRow)
                    modifiedDf_2 = modifiedDf_2.append(df2_currentRow)

    return [modifiedDf_1, modifiedDf_2]


def getListOfCompounds(df):
    listOfCompounds = df['compound'].unique()
    return listOfCompounds

