LIGANDDB := ../bin/ligand.db

all: clean create_tables kegg kegg_rpair

create_tables: ligand.sql 
	sqlite3 $(LIGANDDB) < ligand.sql

ecoli:
	 ./sbml2db.py ./sbmlmodels/EcoliMetabolicNetwork.xml $(LIGANDDB)

human:
	./sbml2db.py  ./sbmlmodels/HumanMetabolicNetwork.xml $(LIGANDDB)

yeast:
	./sbml2db.py  ./sbmlmodels/YeastMetabolicNetwork.xml $(LIGANDDB)

kegg:
	perl ./parseKeggXml.pl

kegg_rpair:
	perl ./parseKeggRpair.pl

metacyc:
	./parseMetaCyc.pl

clean:
	rm -f $(LIGANDDB)


