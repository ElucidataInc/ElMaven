#!/usr/bin/perl
use strict;
use Data::Dumper;
use XML::Simple;
use DBI;

our $DB="../bin/ligand.db";
our $KEGGLIGANDS =    "kegg/compound";
our $KEGGREACTIONS = "kegg/reaction";
our $XMLFILES="kegg/pathways/*.xml";
our %SEEN;

my $dbh= DBI->connect( "dbi:SQLite:dbname=$DB", "", "" );
my $xs = XML::Simple->new( 'KeyAttr' => ['key']  );

$dbh->do("begin");
$dbh->do('delete from reactions where database = "KEGG"');
$dbh->do('delete from species where database = "KEGG"');
$dbh->do('delete from pathways where  database  = "KEGG"');
$dbh->do("delete from pathways_layout where database = 'KEGG'");

&processReactions($KEGGREACTIONS);
&processLigands($KEGGLIGANDS);
foreach my $fname (glob($XMLFILES)) { if ($fname =~ /xml/ ) { &processPathway($fname); } }
$dbh->do("commit");


sub processPathway() { 
		my $fname = shift;
		my $xml= $xs->XMLin($fname);

		my $sth = $dbh->prepare("insert into reactions values(?,?,?,?,?,?)");
		#my $sth2 = $dbh->prepare("insert into keywords values(?,?,?)");
		my $sth3 = $dbh->prepare("insert into pathways values(?,?,?,?)");
		my $sth4 = $dbh->prepare("insert into pathways_layout values(?,?,?,?)");

		my $title = $xml->{'title'};
		my $id =    $xml->{'name'}; $id =~ s/^path://;

		print "$fname  $title\n";

		foreach my $e ( @{$xml->{'entry'}}) { 
				if ($e->{type} eq 'compound' ) {
						if ($e->{'graphics'} ) {
							print "$e->{'graphics'}{'name'} $e->{'graphics'}{'x'}  $e->{'graphics'}{'y'} \n";
						$sth4->execute($id, $e->{'graphics'}{'name'}, $e->{'graphics'}{'x'},  $e->{'graphics'}{'y'} );
					}
			}
		}

		return if (ref $xml->{'reaction'} ne 'ARRAY');

		foreach my $r ( @{$xml->{'reaction'}}) { 
				$r->{name} =~ s/^rn://;
				$sth3->execute($id, $title, $r->{name}, "KEGG");
=head
				print "$r->{name} $r->{type}\n";
				my $reactants = $r->{substrate};
				my $products = $r->{product};
				my $reversable = 1;  $reversable = 0 if ($r->{type} eq 'irreversible'); 
				if ( ref($reactants) eq 'HASH' ) { $reactants = [ $r->{substrate} ]; }
				if ( ref($products) eq 'HASH' ) { $products = [ $r->{product} ]; }

				foreach my $x (@$reactants ) {
						$x->{name} =~ s/cpd://;
						if ( $x->{name}) { 
								$sth->execute($r->{name}, $x->{name}, "R", 1, $reversable, "KEGG"); 
								#$sth2->execute($title, $x->{name}, "KEGG");
						
						}
				}

				foreach my $x (@$products ) {
					$x->{name} =~ s/cpd://;
					if ($x->{name}) { 
							$sth->execute($r->{name}, $x->{name}, "P", 1, $reversable,"KEGG"); 
						#$sth2->execute($title, $x->{name}, "KEGG");
					}
				}
=cut
		}
}

sub processLigands() {
		my $filename = shift || $KEGGREACTIONS;
		my @LIGANDS;
		my $this_ligand = {};
		my $key;
		my $value;

		open(KEGG, "$filename") or die "Can't read $filename";
		while (<KEGG>) {
				if (/\/\/\//) {
						push @LIGANDS, $this_ligand;
						$this_ligand = {};
				}

				if (/^(\S+)\s+(.*)/) {  $key = $1; $value =$2 }

				if (/^\s+(.*)/) { $value = $1; }

				$value =~ s/\;//;

				if ( $key eq 'FORMULA' ) {
						$this_ligand->{$key} = $value;
						#$this_ligand->{MASS} = &MzRock::computeFormulaMass($value);
				} elsif ( $key eq 'ENZYME' ) {
						push @{$this_ligand->{$key}}, split /\s+/, $value;
				} elsif ( $key eq 'NAME') {
						push @{$this_ligand->{$key}}, $value;
				} elsif ( $key eq 'ENTRY') {
						$value =~ s/(\S+).*/$1/;
						$this_ligand->{$key} = $value;
				}
		}

		my $sth = $dbh->prepare("insert into species values(?,?,?,?,?)");
		foreach my $l ( @LIGANDS ) {
			$sth->execute($l->{ENTRY},$l->{NAME}[0],$l->{FORMULA},0,"KEGG");
		}
		return \@LIGANDS;
}

sub processReactions() {
		my $filename = shift || $KEGGLIGANDS;
		my @ALL;
		my $this_ligand = {};
		my $key;
		my $value;

		open(KEGG, "$filename") or die "Can't read $filename";
		while (<KEGG>) {
				if (/\/\/\//) {
						push @ALL, $this_ligand;
						$this_ligand = {};
						$this_ligand->{"REVERSABLE"}=0;
				}

				if (/^(\S+)\s+(.*)/) {  $key = $1; $value =$2 }

				if (/^\s+(.*)/) { $value = $1; }

				$value =~ s/\;//;

				if ( $key eq 'EQUATION' ) {
						$this_ligand->{$key} = $value;
						if ($value=~ /\<=\>/){ $this_ligand->{"REVERSABLE"}=1;}
						my @compounds = split /\s+/, $value;

						my $PRODUCTS=0;
						foreach my $c ( @compounds ) {
								if ($c =~ /\=/ ) { $PRODUCTS=1; }
								if ($c =~ /^[CG]\d+/ and $PRODUCTS)  { push @{$this_ligand->{"PRODUCTS"}}, $c; }
								elsif ($c =~ /^[CG]\d+/)  { push @{$this_ligand->{"REACTANTS"}}, $c; }
						}
								
				} elsif ( $key eq 'NAME' || $key eq 'PATHWAY') {
						push @{$this_ligand->{$key}}, $value;
				} elsif ( $key eq 'ENTRY') {
						$value =~ s/(\S+).*/$1/;
						$this_ligand->{$key} = $value;
				}
		}

		my $sth = $dbh->prepare("insert into reactions values(?,?,?,?,?,?)");
		foreach my $l ( @ALL) {
			next if ( ref $l->{'REACTANTS'} ne 'ARRAY' || ref $l->{'PRODUCTS'} ne 'ARRAY' );
			print &Dumper($l);
			my @reactants = @{$l->{'REACTANTS'}};
			my @products  = @{$l->{'PRODUCTS'}};
			my $reversable = $l->{"REVERSABLE"};
			my $rname = $l->{"ENTRY"};
			$SEEN{$rname}=1;

			foreach my $x (@reactants ) {
					$sth->execute($rname, $x, "R", 1, $reversable, "KEGG"); 
			}

			foreach my $x (@products ) {
					$sth->execute($rname, $x, "P", 1, $reversable,"KEGG"); 
			}
		}
		return \@ALL;
}

