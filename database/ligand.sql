--create ligand database

create table pathways( 
	pathway_id varchar(255), 
	pathway_name varchar(255),
	reaction_id varchar(255),
	database varchar(255)
);

create table reactions( 
		reaction_id varchar(255), 
		species_id  varchar(255), 
		reactant_type   varchar(1), 
		stoichiometry int, 
		reversable int,
		database varchar(255)
);

create table species( 
		species_id varchar(255) primary key,
		species_name varchar(255), 
		formula    varchar(255), 
		charge  int,
		database varchar(255)
);

create table sets( 
		keyword varchar(255), 
		species_id varchar(255)
);

create table pathways_layout( 
	pathway_id varchar(255), 
	species_id varchar(255),
	x float,
	y float
);

create table knowns_times (
	compound_id varchar(255),
	method	    varchar(255),
	rt		 	float
);

create index pathway_idx1 on pathways(reaction_id);
create index pathway_idx2 on pathways_layout(pathway_id);
create index reaction_idx1 on reactions(reaction_id);
-- create index keyword_idx1 on sets(keyword);

