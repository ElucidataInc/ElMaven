#!/usr/bin/python
import csv
import sys

#columns
MZ=0
RT=1
FILENAME=2

#grouping settings
maxppmdiff =20
maxrtdiff  = 0.25

allrows=[]
groups=[]
filenames=[]

def readset(filename):
	set = csv.reader(open(filename), delimiter=',',quotechar='"')
	counter=0
	header = [];
	mzcol=3
	rtcol=4
	scale=1
	for row in set:
		if ( counter == 0 ):
				header = row
				for i in range(0,len(header)):
					if header[i] == "medMz" or header[i] == "mz": mzcol= i
					if header[i] == "medRt" or header[i] == "rt": rtcol= i
					if header[i] == "rt": scale=1.0/60
		try:
			if len(row)>5: 
			  	data=[0,0,"dummy"]
				data[MZ] = float(str(row[mzcol]).strip())
				data[RT] = float(str(row[rtcol]).strip())*scale
				data[FILENAME]  = filename
				allrows.append(data)
				counter = counter + 1
		except:
			"Skipping line"

	if counter > 0: 
		filenames.append(filename);

	print >> sys.stderr, filename, mzcol, rtcol, counter
	
#print "#readin ", counter, " lines" 

def ppm(mz1, mz2):
		return abs(mz2-mz1)/mz1*1e6

def printGroupsLine():
	#header
	for group in groups:
		if len(group) == 0: continue
		for row in group:
			print row[-1], row[MZ], ",", row[RT]
		print


def printGroups():
	#header
	print "mz,rt,", ','.join(filenames);

	for group in groups:
		if len(group) == 0: continue

		matrix=[ 0.0 for i in filenames ]

		for row in group:
			index = filenames.index(row[-1])
			matrix[index]=row[MZ]

		print row[MZ], ",",  row[RT], ",",
		print ','.join(map(str,matrix))

def splitOnRt(group):

	if ( len(group) == 0 ): return

	if ( len(group) == 1 ):
		groups.append(group)
		return

	group.sort(lambda x, y: cmp(x[RT],y[RT]))

	breaks=[]
	for i in range(1,len(group)):
		a = group[i-1];
		b = group[i]
		rtdiff = abs(a[RT]-b[RT])
		if rtdiff > maxrtdiff:
			breaks.append(i)

	if (len(breaks) == 0 ): 
			groups.append(group);
			return

	s=0
	for e in breaks:
		groups.append(group[s:e])
		s=e

	e = len(group)
	groups.append(group[s:e])


def mergesets():
	allrows.sort(lambda x, y: cmp(x[MZ],y[MZ]))
	breaks=[]
	for i in range(1,len(allrows)):
		a = allrows[i-1];
		b = allrows[i]
		ppmdif = ppm(a[MZ],b[MZ])
#		print a[MZ], b[MZ], ppmdif
		if ppmdif > maxppmdiff:
#			print "split"
			breaks.append(i)

	s=0
	for e in breaks:
		splitOnRt(allrows[ s:e ])
		s=e
	splitOnRt(allrows[s:len(allrows)])

for arg in sys.argv: 
	if arg != "./mzmerge.py":
		readset(arg)

mergesets()
#printGroupsLine()
printGroups()
		

		
