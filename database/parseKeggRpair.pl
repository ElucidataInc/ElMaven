#!/usr/bin/perl
use strict;
use Data::Dumper;
use DBI;

#our $DB="rpairs.db";
our $DB="../bin/ligand.db";
our $KEGGRPAIRS = "kegg/rpair";

my $dbh= DBI->connect( "dbi:SQLite:dbname=$DB", "", "" );
$dbh->do("begin");
$dbh->do("drop table atoms");
$dbh->do("drop table rpairs");
$dbh->do("drop table reactions_rpairs");
$dbh->do("create table atoms  ( species_id varchar(20), atomn int, atom varchar(5), x float, y float); ");
$dbh->do("create table rpairs ( rpair_id  varchar(50), atom1n int, atom1 varchar(5), atom2n int, atom2 varchar(5)); ");
$dbh->do("create table reactions_rpairs ( reaction_id varchar(50), rpair_id varchar(50), species1 varchar(20), species2 varchar(20)); ");
&processRPairs($KEGGRPAIRS);
$dbh->do("commit");

sub processRPairs() {
		my $filename = shift;
		my @LIGANDS;
		my $this_ligand = {};
		my $key;
		my $value;

		open(KEGG, "$filename") or die "Can't read $filename";
		while (<KEGG>) {
				if (/\/\/\//) {
#				print &Dumper($this_ligand);
						push @LIGANDS, $this_ligand;
						$this_ligand = {};
				}

				if (/^(\S+)\s+(.*)/) {  $key = $1; $value =$2 }

				if (/^\s+(.*)/) { $value = $1; }

				$value =~ s/\;//;

				if ( $key eq 'RCLASS' ) {
						$this_ligand->{$key} = $value;
						#$this_ligand->{MASS} = &MzRock::computeFormulaMass($value);
				} elsif ( $key eq 'REACTION' ) {
					push @{$this_ligand->{$key}}, split /\s+/, $value;
				} elsif ( $key eq 'NAME') {
						push @{$this_ligand->{$key}}, $value;
				} elsif ( $key eq 'COMPOUND') {
						push @{$this_ligand->{$key}}, $value;
				} elsif ( $key eq 'ALIGN') {
						my @pairs = split /\s+/, $value; 
						if ($value =~ /^\d/ and scalar @pairs > 2 ) {
							my $atom1 = $pairs[1]; 
							my $atom2 = $pairs[2]; 
							my $atom1n = $pairs[1];
							my $atom2n = $pairs[2];
							$atom1 =~ s/^\d+://;
							$atom2 =~ s/^\d+://;
							$atom1n =~ s/:\S+//;
							$atom2n =~ s/:\S+//;
							push @{$this_ligand->{$key}}, [ $atom1n, $atom1, $atom2n, $atom2 ];
							print "$atom1 $atom2\n";
						}
				} elsif ( $key eq 'ENTRY1' || $key eq 'ENTRY2') {
						if ($value =~ /BOND/) { $key="IGNORE"; }
						if ($value =~ /COMPOUND\s+(\S+)/) { $this_ligand->{"$key\_COMPOUND"}= $1; }

						if ($value =~/^\s*(\d+)/) {
							my @atoms = split /\s+/, $value;
							push @{$this_ligand->{$key}}, [ $atoms[0], $atoms[1], $atoms[3], $atoms[4] ];
						}
				} elsif ( $key eq 'ENTRY') {
						print $value, "\n";
						$value =~ s/(\S+).*/$1/;
						$this_ligand->{$key} = $value;
				}
		}
		
		my $sth = $dbh->prepare("insert into atoms values(?,?,?,?,?)");
		my %DONE;
		foreach my $l ( @LIGANDS ) {
			my $entry = $l->{"ENTRY1_COMPOUND"};
			next if $DONE{$entry};
			print $entry, "\n";
			foreach my $atom ( @{$l->{"ENTRY1"}} ) {
				$sth->execute($entry, $atom->[0], $atom->[1], $atom->[2], $atom->[3]);
			}
			$DONE{$entry}=1;
		}

		foreach my $l ( @LIGANDS ) {
			my $entry = $l->{"ENTRY2_COMPOUND"};
			next if $DONE{$entry};
			print $entry, "\n";
			foreach my $atom ( @{$l->{"ENTRY2"}} ) {
				$sth->execute($entry, $atom->[0], $atom->[1], $atom->[2], $atom->[3]);
			}
			$DONE{$entry}=1;
		}

		my $sth = $dbh->prepare("insert into rpairs values(?,?,?,?,?)");
		foreach my $l ( @LIGANDS ) {
			my $entry = $l->{"ENTRY"};
			print $entry;
			next if $DONE{$entry};
			print $entry, "\n";
			foreach my $pair ( @{$l->{"ALIGN"}} ) {
				$sth->execute($entry, $pair->[0], $pair->[1], $pair->[2], $pair->[3]);
			}
			$DONE{$entry}=1;
		}

		my $sth = $dbh->prepare("insert into reactions_rpairs values(?,?,?,?)");
		foreach my $l ( @LIGANDS ) {
			foreach my $rid ( @{$l->{"REACTION"}} ) {
				$sth->execute($rid, $l->{"ENTRY"}, $l->{"ENTRY1_COMPOUND"}, $l->{"ENTRY2_COMPOUND"});
			}
		}
		return \@LIGANDS;
}

