use strict;
use lib "./libs";
use Data::Dumper;
use File::stat;


my $projectFolder="/mnt/mzRockServer/OrbiDataSets";

#my $filelist;
#&covert2csv($projectFolder,$filelist);

my $X = {};
&getProjetsList($projectFolder,$X);


foreach my $x ( keys %$X ) { 
	print "Starting $x\n";
	unlink "mass_slices.csv";
	unlink "fragments.csv";
	
#	system("./eicpull $x/*.mzCSV"); 
	system("./fragpull $x/*.mzXML"); 
	print "Importing..\n";
	system("mysql maven < eicpullDatabase.sql");
	print "Done $x\n";
}

exit;

sub getProjetsList() {
    my $dir = shift;
    my $fileList = shift;

    opendir(DIR, $dir) or warn "getFileList: Can't open directory $dir\n";
    my @contents = readdir(DIR);

    foreach my $f (@contents) {
        next if $f eq '.' or $f eq '..';
        if (-d "$dir/$f" ) {
            &getProjetsList( "$dir/$f", $fileList );
        } elsif ( -f "$dir/$f" and $f =~ /mzXML$/i ) {
            $fileList->{"$dir"}++;
        }
    }
}

