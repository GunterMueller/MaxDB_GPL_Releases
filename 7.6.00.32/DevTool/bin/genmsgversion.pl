#!/usr/bin/perl
use BuildPackage::XMLParser;
use IO::File;
sub getVersion{
	my ($file) = @_;
	my $version;
	unless(-f $file){
		print STDERR "cannot get version of $file: $!\n";
		return undef;
	} 
	$fh = IO::File::new();
	$rc = open ($fh, "< ".$file);
	unless ($rc) {
		print STDERR "cannot open ".$file."\n";
		return undef;
	}
	my $parser = BuildPackage::XMLParser::new ();
	my $tree = $parser->ParseStream ($fh);
	if($tree == 0){
		print STDERR "parsing $file failed\n";	
		return undef;
	}
	if(defined $tree && $tree->GetType eq 'MessageDefinitions'){
		$tree = $tree->VisitChild;
		if($tree != 0){
			while(1){
				if($tree->GetType eq 'Metadata'){
					$version = $tree->GetAttribute('versionnr');	
					last;
                        	}
				$tree = $tree->VisitNeighbor;
				if($tree == 0){
					last;
				}
			}	
		}	
	}
	unless(defined $version){
		print STDERR "$file is no valid message file\n";		
	}	
	$fh->close ();
	return ($version);
}

sub main{
	my @ARGV = @_;
	
	my $version = getVersion($ARGV[0]);
	return -1 unless defined $version;
	
	my $version_file = $ENV{'INSTROOT'}.'/msg/version.txt';
	-d ($ENV{'INSTROOT'}.'/msg') or mkdir ($ENV{'INSTROOT'}.'/msg', 0755);
	my $fh = IO::File->new;		
	my $cur_version;
	if(-f $version_file){
		$fh->open($version_file);
		while(<$fh>){
			chomp;
			next unless /\S/;
			$cur_version = $_;
		}
		$fh->close();
	}
	if((not defined $cur_version) || (defined $cur_version && $cur_version < $version)){	
		unless($fh->open('>'.$version_file)){
			die ("cannot create file $version_file\n");
		}
		print $fh $version."\n";
		$fh->close();
	}
	return 0;	
}

main(@ARGV);

