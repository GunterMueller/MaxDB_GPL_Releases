#!/usr/bin/perl

package SAPDB::Install::Jar;

sub SetValueInManifest {
	my ($filename, %args) = @_;
	my $filename_wrk = $filename.".~wrk~";

	return undef unless (-w $filename);

	my $in = SAPDB::Install::Jar->new ();
	my $out = SAPDB::Install::Jar->new ();

	$in->OpenArchive ($filename);
	$out->OpenArchive ($filename_wrk, 'w');

	for (my $i = 0; $i < $in->{'num_of_files'}; $i++) {
		my $fi = $in->Next ();

		if ($fi->{'filename'} ne 'META-INF/MANIFEST.MF') {
			$in->CopyTo ($out);
			next;
		}

		$in->Open ();
		$out->Open ($fi);

LINE:	for (;;) {
			my $line = $in->ReadLine ();
			last unless (defined $line);

			my ($their_key) = ($line =~ /^(.+):\s".+"$/);

			unless (defined $their_key) {
				($their_key) = ($line =~ /^(.+):\s.+$/);
			}
			
			unless (defined $their_key) {
				$out->Print ($line."\n");
				next;
			}

			foreach my $my_key (keys (%args)) {
				next LINE if ($my_key eq $their_key);
			}

			$out->Print ($line."\n");
		}

		while (my ($my_key, $my_val) = each (%args)) {
			$out->Print ($my_key.": \"".$my_val."\"\n");
		}

		$out->Close ();
		$in->Close ();
	}

	$in->CloseArchive ();
	$out->CloseArchive ();

	RestoreZip ($filename, $filename_wrk);
	unlink ($filename_wrk);

	return 0;
}

sub JarBuildString {
	my ($filename) = @_;

	return undef unless (-r $filename);

	my $in = SAPDB::Install::Jar->new ();

	$in->OpenArchive ($filename);
	$in->Open ('META-INF/MANIFEST.MF');

	my $buildstr;
	for (;;) {
		my $line = $in->ReadLine ();
		last unless defined ($line);

		($buildstr) = ($line =~ /^Implementation-Version:\s"(.+)"$/);
		last if (defined $buildstr);

		($buildstr) = ($line =~ /^Implementation-Version:\s(.+)$/);
		last if (defined $buildstr);
	}

	$in->Close ();
	$in->CloseArchive ();

	return undef unless (defined $buildstr);

	my ($rel, $build);

	($rel, $build) =
	($buildstr =~ /(\d\.\d\.\d+)\s+Build:\s+(\d[\d|-]*)/);

	unless (defined $rel && defined $build) {
		($rel, $build) =
		($buildstr =~ /(\d\.\d\.\d+)\s+Build\s+(\d[\d|-]*)/);
	}

	unless (defined $rel && defined $build) {
		return undef;
	}

	$build =~ s/-$//;

	return ("JAR  ".$rel." Build: ".$build);
}

sub JarBuildInfo {
	my ($filename) = @_; 
	return undef unless (-r $filename);

	my $buildstr = JarBuildString ($filename);
	return undef unless (defined $buildstr);

	my (@info) =
	($buildstr =~ /^JAR\s+(\d+)\.(\d+)\.(\d+)\s+Build:\s+(\d+)/);
	return undef unless (defined $info[0] && defined $info[1] &&
	                     defined $info[2] && defined $info[3]);
	
	#
	# convert to integer
	#
	$info[3] += 0;
	return ("Build" => \@info);
}


sub CreateSelfExtractingArchive{
	my ($self,$name,$extractor) = @_;
	if($^O =~ /mswin/i){
		$name =~ s/\//\\/g;
		$extractor  =~ s/\//\\/g;
	}
	unless($self->OpenArchive($name,'w') == 0){
		print STDERR "cannot create pk zip archive $name\n";
		return undef;
	}
	unless($self->SetExtractor($extractor) == 0){
		$self->CloseArchive();
		print STDERR "cannot set extractor $extractor\n";
		return undef;
	}
	unless($self->Add($extractor) == 0){
		$self->CloseArchive();
		print STDERR "cannot add extractor $extractor\n";
		return undef;
	}
	return 1;
}

