#!/usr/bin/python
import sys
sys.path.append('/usr/local/lib/python2.6/site-packages/');
import libsbml, re, sqlite3, sys

#import networkx as NX

#filename = 'data/metabolic-reactions.sbml'

if not sys.argv[1]:
	print "give sbml file as input"
	
filename = sys.argv[1]
dbfile =   sys.argv[2]

print filename
print dbfile

conn = sqlite3.connect(dbfile)	#output database

reader = libsbml.SBMLReader()
input_sbml = reader.readSBML(filename)
input_model = input_sbml.getModel()

try:
	conn.execute('''create table reactions( sbmlModel varchar(255), reaction_id varchar(255), species_id  varchar(255), reactant_type varchar(1), stoichiometry int, reversable int)''') 
except:
	print  sys.exc_info()[0]	

try:
	conn.execute('''create table species( sbmlModel varchar(255), species_id varchar(255), species_name varchar(255), formula varchar(255), charge  varchar(1))''')
except:
	print  sys.exc_info()[0]	


conn.execute('delete from reactions where sbmlModel = "' + filename + '"');
conn.execute('delete from species where sbmlModel = "' + filename + '"');


p = re.compile('FULL')
seen = {}

for species in input_model.getListOfSpecies():
#		species_names.append(species.getId())
#		print species.getId() + ",", species.getName()
		m = re.search('(.*)_(\S+)', species.getName())
		formula = m.group(2)
		name  = m.group(1)
		charge = species.getCharge()

		mch = re.search('(H(\d+))', formula)

		if mch:

				hstring = mch.group(1)
				hcount = int(mch.group(2))

				if charge < 0:
					hcount = hcount - charge
				elif charge > 0:
					hcount = hcount - charge
						
				#print hstring, " ", hcount , "--" , charge

				#print ">" + formula
				rpl = re.compile(hstring)
				formula = rpl.sub("H"+str(hcount), formula,count=1)
				#print "<" + formula

		conn.execute('insert into species values (?,?,?,?,?)', (filename, species.getId(), name, formula, charge));

		if not seen.has_key(name) and not p.search(formula):
	#		print formula + " " + name
			seen[name] = 1

	
reaction_names = []
for reaction in input_model.getListOfReactions():
		reaction_names.append(reaction.getId())
		#print reaction.getId();
		rev = reaction.getReversible()

		for x in reaction.getListOfReactants():
			#print  reaction.getId() + "," + x.getSpecies() + ",R," +  str(x.getStoichiometry()) + "," + str(rev)
			conn.execute('insert into reactions values (?,?,?,?,?,?)', (filename,reaction.getId(), x.getSpecies(), 'R', x.getStoichiometry(), rev));

		for x in reaction.getListOfProducts():
			#print  reaction.getId() + "," + x.getSpecies() + ",P," +  str(x.getStoichiometry()) + "," + str(rev)
			conn.execute('insert into reactions values (?,?,?,?,?,?)', (filename,reaction.getId(), x.getSpecies(), 'P', x.getStoichiometry(), rev));

conn.commit()
