
import pandas as pd
from helper import helper

data_df_1 = helper.readCsvPandas('/home/sahil/elucidata/compounds.csv')
data_df_2 = helper.readCsvPandas('/home/sahil/code/qe/maven_data/Experiments/769peakDetectorCLITest-1469697786.02/output/769peakDetectorCLITest-1469697786.02.csv')

listOfCompounds_1 = data_df_1['compound'].unique()
listOfCompounds_2 = data_df_2['compound'].unique()

uniqueListOfCompounds = list(set(listOfCompounds_1) & set(listOfCompounds_2))

topThree_1 = data_df_1.groupby('compound').head(3)
topThree_2 = data_df_2.groupby('compound').head(3)

topThree_1 = topThree_1.loc[topThree_1['compound'].isin(uniqueListOfCompounds)]
topThree_2 = topThree_2.loc[topThree_2['compound'].isin(uniqueListOfCompounds)]

for compound in uniqueListOfCompounds:
    compound_topThree_1 = topThree_1[topThree_1['compound'] == compound]
    compound_topThree_2 = topThree_2[topThree_2['compound'] == compound]
    for element in xrange(len(compound_topThree_1)):
        print compound_topThree_1.iloc[[element]]

