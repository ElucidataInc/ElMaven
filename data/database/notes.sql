drop table notes;
create table notes(
	noteid	INTEGER PRIMARY KEY AUTOINCREMENT, 
	mzmin  float,
 	mzmax  float,
	rt  float,
	intensity  float,
	note   text,
	experiment text,
	samplename	varchar(255),
	userid	INTEGER,
	creationTime timestamp
);

drop table validations;
create table validations(
	mz float,
	rt float,
	class	varchar(3),
	peakAreaFractional float,
	noNoiseFraction float,
	symmetry float,
	groupOverlap float,
	gaussFitR2 float,
	SN float,
	peakRank float,
	zeroBaseLine float,
	tinyPeak float,
	symmetry5 float,
	SN3 float
);
