create table if not exists eics(
		mz float,
		mzmin float,
		mzmax float,
		projectname varchar(255),
		samplename varchar(255),
		polarity varchar(1),
		eic text
);

create table if not exists fragments(
		mz float,
		rt float,
		projectname varchar(255),
		samplename varchar(255),
		polarity varchar(1),
		fragments text
);

load data local infile 'fragments.csv' into table fragments FIELDS TERMINATED by ",";
--load data local infile 'mass_slices.csv' into table eics FIELDS TERMINATED by ",";
