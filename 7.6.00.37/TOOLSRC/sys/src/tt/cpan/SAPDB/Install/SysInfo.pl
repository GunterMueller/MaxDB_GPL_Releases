#!/usr/bin/perl

package SAPDB::Install::SysInfo;

sub get_aix_version {
	unless (defined &SAPDB::Install::PipeExec::new) {
		bootstrap SAPDB::Install::PipeExec;
	}

	my $cmd = (-x "/usr/bin/oslevel") ? "/usr/bin/oslevel" : "oslevel";

	my $handle = SAPDB::Install::PipeExec::new ();
	$handle->Open ($cmd);
	my $line = $handle->Readline ();
	$handle->Close ();

	unless (defined $line) {
		my $errtxt = $handle->GetError ();
		if (defined $errtext) {
			return undef;
		}
	}

	chomp ($line);
	return ($line);
}

sub get_solaris_cppruntime {
	unless (defined &SAPDB::Install::PipeExec::new) {
		bootstrap SAPDB::Install::PipeExec;
	}

	my @ret = ();
	my ($os_version,$arch_name) = @_;
	
	unless (defined $os_version) {
		return @ret;
	}

	my $patchnum =
		($os_version eq '2.6') ? '105591' :
		($os_version eq '7')   ? '106327' :
		($os_version eq '8')   ? '108434' :
		($os_version eq '9' && $arch_name eq 'X86-64')  ? '111713' :
		($os_version eq '9')	? '111711' :
		undef;

	unless (defined $patchnum) {
		return @ret;
	}

	my $patchnum64 =
		($os_version eq '7')   ? '106300' :
		($os_version eq '8')   ? '108435' :
		($os_version eq '9')   ? '111712' :
		($os_version eq '10' && $arch_name eq 'X86-64')	? '119964' :	
		undef;
		
	my $cmd =
	(-x "/usr/sbin/showrev") ? "/usr/sbin/showrev -p" :
	(-x "/usr/bin/showrev") ?  "/usr/bin/showrev -p" :
	"showrev -p";

	my $handle = SAPDB::Install::PipeExec::new ();
	unless (defined $handle) {
		return @ret;
	}

	$handle->Open ($cmd);

	my $our = undef;
	my $our64 = undef;

	while (my $line = $handle->Readline ()) {
		chomp ($line);
		(my $their) = ($line =~ /^Patch:\s+$patchnum\-(\d+)/);
		if (defined $their) {
			$our = (!defined $our || $their > $our) ? $their : $our;
			next;
		}

		unless (defined $patchnum64) {
			next;
		}

		(my $their) = ($line =~ /^Patch:\s+$patchnum64\-(\d+)/);
		if (defined $their) {
			$our64 = (!defined $our64 || $their > $our64) ? $their : $our64;
			next;
		}
	}

	$handle->Close ();

	if (defined $patchnum) {
		push @ret,
		(defined $our) ? $patchnum.'-'.$our :'no patch installed';
	}

	if (defined $patchnum64) {
		push @ret,
		(defined $our64) ? $patchnum64.'-'.$our64 :'no patch installed';
	}

	return @ret;
}

sub get_tru64_version {
	unless (defined &SAPDB::Install::PipeExec::new) {
		bootstrap SAPDB::Install::PipeExec;
	}

	my $cmd = (-x "/usr/sbin/sizer") ? "/usr/sbin/sizer -v" : "sizer -v";

	my $handle = SAPDB::Install::PipeExec::new ();
	$handle->Open ($cmd);
	my $line = $handle->Readline ();
	$handle->Close ();

	unless (defined $line) {
		my $errtxt = $handle->GetError ();
		if (defined $errtext) {
			return undef;
		}
	}

	(my $version) = ($line =~ /(\d+\.\d+\w*)\s+\(Rev/);
	return ($version);
}

sub get_tru64_subversion {
	unless (defined &SAPDB::Install::PipeExec::new) {
		bootstrap SAPDB::Install::PipeExec;
	}
	
	my $filename = "/var/adm/patch/log/event.log";
	unless (-r $filename) {
		return undef;
	}
	
	unless (open (IN, $filename)) {
		return undef;
	}

	my $kitname = undef;
	while (my $line = <IN>) {
		chomp $line;
		
		#
		# may be DIGITAL UNIX
		# looks like 'KITNAME><DUV40DAS0005-19991007>'
		#
		if ($line =~ /^KITNAME\>\<(DU\w+)-\d+\>/) {
			$kitname = $1;
			next;
		}

		#
		# may be TRU64 UNIX
		# looks like 'KITNAME><T64V51AS0003-20010521>'
		#
		if ($line =~ /^KITNAME\>\<(T64\w+)-\d+\>/) {
			$kitname = $1;
			next;
		}
	}

	close (IN);

	unless (defined $kitname) {
		return undef;
	}

	return ($kitname);
}

sub IsRunnable {
	my ($our, $their, $magic) = @_;

	return 0 unless (defined $our && defined $magic);

	return 0 unless (defined $our->{'system'});
	return 0 unless (defined $our->{'architecture'});

	if ($our->{'system'} =~ /AIX|HPUX|Linux|Reliant|Solaris|Tru64/) {
	
		return 0 unless (defined $their);	
	
		#
		# UNIX,
		# usually we can use sysinfo from binary
		#
		return 1 unless (defined $their->{'architecture'});
		return 1 unless (defined $their->{'system'});

		#
		# fixed misspelling in system name
		#
		$their->{'system'} = 'Tru64' if ($their->{'system'} eq 'True64');

		return 0 unless ($our->{'system'} eq $their->{'system'});

		return 0
		unless ($our->{'architecture'} eq $their->{'architecture'});

		return
		$our->{'system'} eq 'AIX' ?
			is_runnable_aix ($our, $their, $magic) :
		$our->{'system'} eq 'HPUX' ?
			is_runnable_hpux ($our, $their, $magic) :
		$our->{'system'} eq 'Linux' ?
			is_runnable_linux ($our, $their, $magic) :
		$our->{'system'} eq 'Reliant' ?
			is_runnable_reliant ($our, $their, $magic) :
		$our->{'system'} eq 'Solaris' ?
			is_runnable_solaris ($our, $their, $magic) :
		$our->{'system'} eq 'Tru64' ?
			is_runnable_tru64 ($our, $their, $magic) :
			1;
	} elsif ($our->{'system'} =~ /Windows/) {
		#
		# Windows,
		# usually we cannot use sysinfo from binary,
		# use magic instead
		#
		return 0 unless ($magic =~ /Windows/);

		if ($our->{'architecture'} eq 'I386') {
			return ($magic =~ /i386/) ? 1 : 0;
		} elsif ($our->{'architecture'} eq 'IA64') {
			return ($magic =~ /IA64/) ? 1 : 0;
		}
		elsif ($our->{'architecture'} eq 'AMD64'){
			return ($magic =~ /IA64/) ? 0 : 1;
		}
		return 1;
	}
	return 1;
}

sub is_runnable_aix {
	my ($our, $their, $magic) = @_;

	return 0 unless ($our->{'system'} eq 'AIX');

	my @our_version = ($our->{'version'} =~ /^(\d+)\.(\d+)\.(\d+)/);	
	my @their_version = ($their->{'version'} =~ /^(\d+)\.(\d+)\.(\d+)/);	

	#
	# return true in any case of doubt,
	# because we cannot be sure that we always get 3 numbers
 	#
	return 1 unless ($#our_version == 2 && $#their_version == 2);

	#
	# aix 4 64-bit cannot be executed on aix 5 and vice versa
	#
	if ($magic =~ /64-bit/) {
		return 0 if ($our_version[0] >= 5 && $their_version[0] <= 4);
		return 0 if ($our_version[0] <= 4 && $their_version[0] >= 5);
	}

	return 1 if ($our_version[0] > $their_version[0]);
	return 0 if ($our_version[0] < $their_version[0]);

	return 1 if ($our_version[1] > $their_version[1]);
	return 0 if ($our_version[1] < $their_version[1]);

	return 1 if ($our_version[2] > $their_version[2]);
	return 0 if ($our_version[2] < $their_version[2]);

	return 1;
}

sub is_runnable_hpux {
	my ($our, $their) = @_;

	return 0 unless ($our->{'system'} eq 'HPUX');

	my @our_version = ($our->{'version'} =~ /(\d+)\.(\d+)/);	
	my @their_version = ($their->{'version'} =~ /(\d+)\.(\d+)/);	

	#
	# return true in any case of doubt,
	# because we cannot be sure that we always get 2 numbers
 	#
	return 1 unless ($#our_version == 1 && $#their_version == 1);

	return 1 if ($our_version[0] > $their_version[0]);
	return 0 if ($our_version[0] < $their_version[0]);

	return 1 if ($our_version[1] > $their_version[1]);
	return 0 if ($our_version[1] < $their_version[1]);

	return 1;
}

sub is_runnable_linux {
	my ($our, $their) = @_;

	return 0 unless ($our->{'system'} eq 'Linux');

	#
	# when 'c_runtime' is known,
	# we have to make sure that our 'c_runtime' is greater than or equal
	# their 'c_runtime
	#
	# c_runtime looks like 'GLIBC 2.1.3'
	#
	return 1 unless (defined $our->{'c_runtime'} && $their->{'c_runtime'});

	my @our_libc =
		($our->{'c_runtime'} =~ /GLIBC\s+(\d+)\.(\d+)\.(\d+)/);

	my @their_libc =
		($their->{'c_runtime'} =~ /GLIBC\s+(\d+)\.(\d+)\.(\d+)/);

	#
	# return true in any case of doubt,
	# because we cannot be sure that we always get 3 numbers
 	#
	return 1 unless ($#our_libc == 2 && $#their_libc == 2);

	return 1 if ($our_libc[0] > $their_libc[0]);
	return 0 if ($our_libc[0] < $their_libc[0]);

	return 1 if ($our_libc[1] > $their_libc[1]);
	return 0 if ($our_libc[1] < $their_libc[1]);

	return 1 if ($our_libc[2] > $their_libc[2]);
	return 0 if ($our_libc[2] < $their_libc[2]);

	return 1;
}

sub is_runnable_reliant {
	my ($our, $their) = @_;

	return 0 unless ($our->{'system'} eq 'Reliant');

	my @our_version = ($our->{'version'} =~ /(\d+)\.(\d+)/);	
	my @their_version = ($their->{'version'} =~ /(\d+)\.(\d+)/);	

	#
	# return true in any case of doubt,
	# because we cannot be sure that we always get 2 numbers
 	#
	return 1 unless ($#our_version == 1 && $#their_version == 1);

	return 1 if ($our_version[0] > $their_version[0]);
	return 0 if ($our_version[0] < $their_version[0]);

	return 1 if ($our_version[1] > $their_version[1]);
	return 0 if ($our_version[1] < $their_version[1]);

	return 1;
}

sub is_runnable_solaris {
	my ($our, $their) = @_;

	return 0 unless ($our->{'system'} eq 'Solaris');

	if ($their->{'version'} eq '2.6') {
		return 0 if ($our->{'version'} =~ /^2\.4/);
		return 0 if ($our->{'version'} =~ /^2\.5/);
		return 1;
	}

	if ($their->{'version'} eq '7') {
		return 0 if ($our->{'version'} =~ /^2\.4/);
		return 0 if ($our->{'version'} =~ /^2\.5/);
		return 0 if ($our->{'version'} eq '2.6');
		return 1;
	}

	if ($their->{'version'} eq '8') {
		return 0 if ($our->{'version'} =~ /^2\.4/);
		return 0 if ($our->{'version'} =~ /^2\.5/);
		return 0 if ($our->{'version'} eq '2.6');
		return 0 if ($our->{'version'} eq '7');
		return 1;
	}

	if ($their->{'version'} eq '9') {
		return 0 if ($our->{'version'} =~ /^2\.4/);
		return 0 if ($our->{'version'} =~ /^2\.5/);
		return 0 if ($our->{'version'} eq '2.6');
		return 0 if ($our->{'version'} eq '7');
		return 0 if ($our->{'version'} eq '8');
		return 1;
	}

	if ($their->{'version'} eq '10') {
		return 0 if ($our->{'version'} =~ /^2\.4/);
		return 0 if ($our->{'version'} =~ /^2\.5/);
		return 0 if ($our->{'version'} eq '2.6');
		return 0 if ($our->{'version'} eq '7');
		return 0 if ($our->{'version'} eq '8');
		return 0 if ($our->{'version'} eq '9');
		return 1;
	}


	#
	# return true in any case of doubt
	#
	return 1;
}

sub is_runnable_tru64 {
	my ($our, $their) = @_;

	return 0 unless ($our->{'system'} eq 'Tru64');

	#
	# Tru64 version looks like '4.0F' or '5.0'
	# these versions are transformed to '4.0.6' or '5.0.0'
	#
	my @our_version = ($our->{'version'} =~ /(\d+)\.(\d+)([A-Z])/);
	if ($#our_version == 2) {
		$our_version[2] = ord ($our_version[2]) - ord ('A') + 1;
	} else {
		@our_version = ($our->{'version'} =~ /(\d+)\.(\d+)/);
		$our_version[2] = 0;
	} 

	my @their_version = ($their->{'version'} =~ /(\d+)\.(\d+)([A-Z])/);
	if ($#their_version == 2) {
		$their_version[2] = ord ($their_version[2]) - ord ('A') + 1;
	} else {
		@their_version = ($their->{'version'} =~ /(\d+)\.(\d+)/);
		$their_version[2] = 0;
	}

	#
	# return true in any case of doubt about getting the version
	#
	return 1 unless ($#our_version == 2 && $#their_version == 2);

	return 1 if ($our_version[0] > $their_version[0]);
	return 0 if ($our_version[0] < $their_version[0]);

	return 1 if ($our_version[1] > $their_version[1]);
	return 0 if ($our_version[1] < $their_version[1]);

	return 1 if ($our_version[2] > $their_version[2]);
	return 0 if ($our_version[2] < $their_version[2]);

	return 1;
}
