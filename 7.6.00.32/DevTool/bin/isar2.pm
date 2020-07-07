#!/usr/bin/perl
#
# isar.pm
#
# cba 2000-09-22
#

package isar2;

use Env;
use Getopt::Long;
use File::Basename;
use File::Copy;
use Symbol;
use Exporter;

@ISA = qw (Exporter);
@EXPORT = qw (isar isar2);

$program_version = "isar.pm 2.00";

# 
# isar ()
#
# this is for compatibility reasons,
# used by the historical isar.pl
#
sub isar {
	my ($rc, $err) = isar2 (\@_, undef);
	
	die $err."\n"
		unless ($rc == 0);

	return 0;
}

#
# isar2()
#
# $_[0]  : refrence to argument list
# $_[1]  : reference to protocol output list, 
#          can be undef if you do not want to use it
# $ret[0]: 0 on success
# $ret[1]: errormsg if ($ret[0] != 0)
#
sub isar2 {
	my $ref_args = shift;
	my $ref_prot = shift;
	my @cmd;
	my $CreateFlag;
	my $ExtractFlag;
	my $TestFlag;
	my $VerboseFlag;
	my $ListFilename;
	my $CarFilename;
	my $err;
	my $rc;

	# GetOptions needs a global @ARGV
	local @ARGV = @$ref_args;

	# do we need a protocol
	push @$ref_prot, "<cmd>", "isar2 ".join (" ", @ARGV)
		if (defined $ref_prot);

	# allow use of '-xvf' as a bundled option
	Getopt::Long::config ("bundling")
		if ($] >= 5.004);

	GetOptions (
		'f=s', \$CarFilename, 
		'c', \$CreateFlag, 
		'x', \$ExtractFlag,
		't', \$TestFlag, 
		'v', \$VerboseFlag,
		'A=s',\$ListFilename, 
		'version', \$VersionWanted,
		'help', \$HelpWanted, 
		'h', \$HelpWanted
	);

	# check command line options
	if (defined $VersionWanted) {
		print_version ();
		return 0;
	} elsif (defined $HelpWanted) {
		print_help ();
		return 0;
	}
 
	($rc, $err) = check_opts ($CarFilename, $CreateFlag, 
	                       $ExtractFlag, $TestFlag);
	return (-1, $err)
		unless ($rc == 0); 

	# build the sapcar cmd
	if (is_windows ()) {
		@cmd = ($ENV{'TOOL'}."\\bin\\SAPCAR.exe") 
	} else {
		@cmd = ("SAPCAR");
	}

	push @cmd, "-v" if (defined $VerboseFlag);

	if (defined $TestFlag) {
		exec_car_test (\@cmd, $ref_prot, $CarFilename);
	} elsif (defined $ExtractFlag) {
		exec_car_extract (\@cmd, $ref_prot, $CarFilename);
	} elsif (defined $CreateFlag) {
		my @args;
		if (defined $ListFilename) {
			@args = ("-A", $ListFilename);
		} elsif (defined $ARGV[0]) {
			@args = @ARGV;
		} 
		exec_car_availability (\@cmd, $ref_prot, \@args);
		exec_car_create (\@cmd, $ref_prot, $CarFilename, \@args);
	}

	push @$ref_prot, "</cmd>"
		if (defined $ref_prot);

	return 0;
}

#
# get_listfile ()
#
# $_[0]  : reference to own command line arguments
# 
# $ret[0]: listfilename
#
sub get_listfile {
	my $ref_args = shift;
	my @args = @$ref_args;
	

	# did we got a listfile from command line using -A option
	if ($#args == 2 && $args[0] eq "-A") {
		return $args[1];
	}

	# make our own listfile
	my $line;
	my $list_filename = "list.tmp";
	my $fh_list = gensym ();
	open ($fh_list, "> ".$list_filename);
	if ($#args >= 0) {
		# get filelist from parameter list
		$line = undef;
		foreach (@args) {
			print $fh_list $line;
			$line = $_."\n";
		}
		chomp $line;
		print $fh_list $line;
	} else {
		# get filelist from <STDIN>
		$line = undef;
		while (<STDIN>) {
			print $fh_list $line;
			chomp;
			$line = $_."\n";
		}
		chomp $line;
		print $fh_list $line;
	}
	close $fh_list;
	return $list_filename;
}

#
# clean_listfile ()
#
# $_[0]  : reference to own command line arguments
# 
sub clean_listfile {
	my $ref_args = shift;
	my @args = @$ref_args;

	# did we got a listfile from command line using -A option
	return
		if ($#args == 2 && $args[0] eq "-A");
	
	my $list_filename = "list.tmp";
	unlink $list_filename;
	return;
}

#
# exec_car_create ()
# 
# $_[0]  : reference to command line arguments  
# $_[1]  : reference to protocol array
# $_[2]  : archive name
# $_[3]  : reference to my own unparsed command line parameters
#
# $ret[0]: 0 on success
# $ret[1]: errtext or undef on success
#
sub exec_car_create {
	my $ref_cmd = shift;
	my @cmd = @$ref_cmd;
	my $ref_prot = shift;
	my $filename = shift;
	my $ref_args = shift;
	
	my @filename_list;
	my $filename_local;
	my $listfilename = get_listfile ($ref_args);

	@filename_list = split (/,/, $filename);
	$filename_local = "~".basename ($filename_list[0]);
	push @cmd, "-f", $filename_local, "-c", "-A", $listfilename;

	exec_cmd (\@cmd, $ref_prot);
	my ($rc, $err) = write_out_files (\@filename_list, $filename_local);
	unlink $filename_local;
	clean_listfile (\@args);

	unless ($rc == 0) {
		push @$ref_prot, "<err>", $err, "</err>", "</cmd>"
			if (defined $ref_prot);
		return (-1, $err)
	}
	return (0, undef);
}

sub exec_car_availability {
	my $ref_cmd = shift;
	my @cmd = @$ref_cmd;
	my $ref_prot = shift;
	my $ref_args = shift;

	my @missing_file_list = ();
	my $listfilename = get_listfile ($ref_args);
	push @cmd, "-l", $listfilename;

	my $err = undef;
	my $fh_their_out = gensym ();
	open ($fh_their_out, join (" ", @cmd)." |");
	while (<$fh_their_out>) {
		chomp;
		unless (defined $err) {
			if ($_ =~ /some files are missing/) {
				$err = "missing files";
				next;
			} else {
				$err = "unexpected error";
				last;
			}
		}
		print $_."\n";
	}
	close $fh_their_out;

	my $filename_log = "files.log";
	if ($err eq "missing files") {
		my $fh_log = gensym ();
		open ($fh_log, $filename_log);
		while (<$fh_log>) {
			# skip comments
			next if ($_ =~ /^# /);
			chomp;
			push @missing_file_list, $_;
		}

		close $fh_log;
	}

	unlink $filename_log;
	clean_listfile (\@args);
	return @missing_file_list;
}

#
# exec_car_test ()
# 
# $_[0]  : reference to command line arguments  
# $_[1]  : reference to protocol array
# $_[2]  : archive name
#
# $ret[0]: 0 on success
# $ret[1]: errtext or undef on success
#
sub exec_car_test {
	my $ref_cmd = shift;
	my @cmd = @$ref_cmd;
	my $ref_prot = shift;
	my $filename = shift;
	my $local_filename;
	my $rc;
	my $err;

	($rc, $err, $local_filename) = get_input_file ($filename);
	push @cmd, "-t", "-f", $local_filename;
	exec_cmd (\@cmd, $ref_prot);
	clean_input_file ($filename);
}

#
# exec_car_extract ()
# 
# $_[0]  : reference to command line arguments  
# $_[1]  : reference to protocol array
# $_[2]  : archive name
#
# $ret[0]: 0 on success
# $ret[1]: errtext or undef on success
#
sub exec_car_extract {
	my $ref_cmd = shift;
	my @cmd = @$ref_cmd;
	my $ref_prot = shift;
	my $filename = shift;
	my $local_filename;
	my $rc;
	my $err;

	($rc, $err, $local_filename) = get_input_file ($filename);
	push @cmd, "-x", "-f", $local_filename;
	exec_cmd (\@cmd, $ref_prot);
	clean_input_file ($filename);
}

sub check_opts {
	my ($CarFilename, $CreateFlag, $ExtractFlag, $TestFlag) = @_;
	my $rc;
	my $err;

	($rc, $err) = (-1, "isar.pm: please use either -x, -t or -c flag");

	return ($rc, $err)
		if ((defined $CreateFlag) && (defined $ExtractFlag)); 

	return ($rc, $err)
		if ((defined $CreateFlag) && (defined $TestFlag));
	
	return ($rc, $err)
		if ((defined $ExtractFlag) && (defined $TestFlag));

	return ($rc, $err)
		unless (defined $CreateFlag || 
		        defined $ExtractFlag || 
		        defined $TestFlag);

	($rc, $err) = (-1, "isar.pm:filename required");

	return ($rc, $err)
		unless  (defined $CarFilename);

	return (0, undef);
}

#
# exec_cmd ()
#
# $_[0]  : reference to command line arguments 
# $_[1]  : reference to protocol array
#
# $ret[0]: 0 on success
#
sub exec_cmd {
	my $ref_cmd = shift;
	my $ref_prot = shift;
	my @cmd = @$ref_cmd;
	
	push @$ref_prot, "<system>", join (" ", @cmd) 
		if (defined $ref_prot);

	my $fh_their_out = gensym ();

	open ($fh_their_out, join (" ", @cmd)." |");
	push @$ref_prot, "<info>" 
		if (defined $ref_prot);

	while (<$fh_their_out>) {
		chomp;
		print $_."\n";
		push @$ref_prot, $_
			if (defined $ref_prot);
	}
	push @$ref_prot, "</info>" 
		if (defined $ref_prot);

	close ($fh_their_out);
	my $rc = $?;
	
	my $err = "isar: execution of \"".(join " ", @cmd)."\" failed";

	if ($rc != 0) {
		push @$ref_prot, "<err>", $err, "</err>", "</cmd>"
			if (defined $ref_prot);
		return (-1, $err);
	}

	push @$ref_prot, "</system>"
		if (defined $ref_prot);
	
	return 0;
}

#
# write_out_files ()
#
# $_[0]   : reference to array of output filenames
# $_[1]   : name of local input file
#
# $ret[0] : 0 on success
# $ret[1] : errtext or undef on success
#
sub write_out_files {
	my $ref_out_filename_list = shift;
	my @out_filename_list = @$ref_out_filename_list;
	my $local_filename = shift;
	my $out_filename;
	my $rc;
	my $err;
	my $fh_in;
	my $fh_out;

	($rc, $err, $fh_in) = open_in_file ($local_filename);

	my @fh_out_list = ();
	foreach $out_filename (@out_filename_list) {
		($rc, $err, $fh_out) = open_out_file ($out_filename);
		return ($rc, $err) 
			unless ($rc == 0);
		push @fh_out_list, $fh_out;
	}

	for (;;) {
		my $got;
		my $buffer;
		my $offset = 0;
	
		($got, $offset, $err) = 
		read_into_buffer ($fh_in, \$buffer, 32768, $offset);

		if ($got == 0) {
			# successful end of loop
			$err = undef;
			$rc = 0;
			last;
		} elsif (! ($got > 0)) {
			# fatal end of loop
			$rc = -1;
			last;
		}			

		foreach $fh_out (@fh_out_list) {
			($rc, $err) = write_out ($fh_out, $buffer, $got);
			return ($rc, $err)
				unless ($rc == 0);
		}
	}

	close ($fh_in);
	foreach $fh_out (@fh_out_list) {
		close ($fh_out);
	}

	return ($rc, $err);
}

#
# read_into_buffer ()
#
# $_[0]   : filehandle
# $_[1]   : buffer
# $_[2]   : bytes wanted
# $_[3]   : offset
#
# $ret[0] : got
# $ret[1] : offset
# $ret[3] : errtext or undef if ($got >= 0)
#
sub read_into_buffer {
	my $got;
	my $err;
	my $fh = shift;
	my $ref_buffer = shift;
	my $want = shift;
	my $offset = shift;

	$got = sysread ($fh, $$ref_buffer, $want, $offset);
	unless (defined $got) {
		$err = "isar: error in read_into_buffer";
		return ($got, $offset, $err);
	}
	$offset += $got if ($got >= 0);
	return ($got, $offset, undef);
}

#
# open_out_file ()
#
# $_[0]   : filename, if it conatains a ':', the file will be opened 
#           on the remote machine 
# $ret[0] : return code, 0 on success
# $ret[1] : errortext or undef on success
# $ret[2] : filehandle on sucess or undef
#
sub open_out_file {
        my $filename = shift;
	my $fh;
	my $err;

	# use sysopen if it is a local file
        unless ($filename =~ /:/) {
                unless (defined &O_RDWR) {
                        require Fcntl;
                        import Fcntl qw (O_RDWR O_CREAT O_EXCL 
			                 O_RDONLY O_TRUNC);
                }
		$fh = gensym ();
		unless (
		sysopen ($fh, $filename, &O_RDWR | &O_CREAT | &O_TRUNC)) {
                	$err = "isar: cannot open ".$filename." read write";
			return (-1, $err, undef);
		}
        	binmode ($fh);
	        return (0, undef, $fh);
	}

	# use rem pipe if it is a local file
       	my @rem_cmd;
	my $ptr;

	if (is_windows ()) {
		@rem_cmd = ("|", "_rem", "write");
	} else {
		@rem_cmd = ("|", "rem", "write");
	}

	$filename =~ /^([\w\.-]+):(.+)$/;
	push @rem_cmd, $1, $2;

	$fh = gensym ();
	open ($fh, join " ", @rem_cmd);
	unless ($? == 0) {
		close $fh;
		$err = "isar: cannot open ".(join " ", @rem_cmd);
		return  (-1, $err, undef);
	}
       	binmode ($fh);
       	return (0, undef, $fh);
}

#
# open_in_file ()
#
# $_[0]   : filename, if it conatains a ':', the file will be opened 
#           on the remote machine 
# $ret[0] : return code, 0 on success
# $ret[1] : errortext or undef on success
# $ret[2] : filehandle on sucess or undef
#
sub open_in_file {
        my $filename = shift;
	my $fh;
	my $err;

        unless ($filename =~ /:/) {
                unless (defined &O_RDONLY) {
                        require Fcntl;
                        import Fcntl qw (O_RDWR O_CREAT O_EXCL 
			                 O_RDONLY O_TRUNC);
                }
                $fh = gensym ();
                unless (sysopen ($fh, $filename, &O_RDONLY)) {
                        $err = "isar: cannot open ".$filename." read only";
			return (-1, $err, undef),
		}
        	binmode ($fh);
		return (0, undef, $fh);
	}

       	my @rem_cmd;
	if (is_windows ()) {
		@rem_cmd = ("_rem", "read");
	} else {
		@rem_cmd = ("rem", "read");
	}

	$filename =~ /^([\w\.-]+):(.+)$/;
	push @rem_cmd, $1, $2, "|";
	
	$fh = gensym ();
	open ($fh, join " ", @rem_cmd);
	unless ($? == 0) {
		close $fh;
		$err = "isar: cannot open ".(join " ", @rem_cmd);
		return  (-1, $err, undef);
	}
        binmode ($fh);
        return (0, undef, $fh);
}

#
# write_out ()
#
# $_[0]   : filehandle;
# $_[1]   : buffer;
# $_[2]   : len;
#
# $ret[0] : 0 on success
# $ret[1] : errtext on undef on success
# $ret[2] : number of written bytes on success or undef
#
sub write_out {
	my ($fh, $buffer, $want) = @_; 

	my $put = syswrite ($fh, $buffer, $want);
        return (-1, "isar: could not write", undef)
		unless ($want == $put);

        return (0, undef, $put);
}

#
# get_input_file ()
# 
# $_[0] :   filename of input file
#           if it conatains a ':', a local copy of the file will be created
#
# $ret[0] : 0 on success 
# $ret[1] : errortext or undef on success
# $ret[2] : name of the local useable file
#
sub get_input_file {
	my $in_filename = shift;

	# use local file if you can
	return ($in_filename) 
		unless ($in_filename =~ /:/);

	# make a local copy of the file
	my $rc;
	my $err;
	my $filename;
	my $fh_in;
	my $fh_out;

	$filename = "~". basename ($in_filename);
	($rc, $err, $fh_out) = open_out_file ($filename);
	if ($rc != 0) {
		return ($rc, $err, undef);
	}

	($rc, $err, $fh_in) = open_in_file ($in_filename);
	if ($rc != 0) {
		close $fh_out;
		return ($rc, $err, undef);
	}

	my $want = 32768; 
	for (;;) {
		my $got;
		my $buffer;

		$got = sysread ($fh_in, $buffer, $want);
		last if ($got == 0);
		syswrite ($fh_out, $buffer, $got);
	}

	close ($fh_in);
	close ($fh_out);
	return (0, undef, $filename);
}

#
# clean_input_file ()
# 
# $_[0] :   filename of input file
#           if it contains a ':', the local copy will be removed
# 
sub clean_input_file {
	my $in_filename = shift;

	unlink "~".basename ($in_filename)
        	if ($in_filename =~ /:/);
}

#
# is_windows ()
#
# $ret[0] : true if we are running windows
#
sub is_windows {
	return ($^O =~ /^MsWin/i);
}

#
# print_version ()
#
sub print_version {
	print $program_version."\n";
}

#
# print_help ()
#
sub print_help {
	print "isar --help, isar -h\n";
	print "isar --version\n";
	print "isar -x[v]f [host:]archive\n";
	print "isar -t[v]f [host:]archive\n";
	print "isar -c[v]f [host:]archive[,[host:]archive][,...]\n".
	      "\t[-A listfile]\n";
	print "isar -c[v]f [host:]archive[,[host:]archive][,...]\n".
	      "\tfile_0 file_1 file_2 ...\n";
	print "isar -c[v]f [host:]archive[,[host:]archive][,...]\n".
	      "\tuse <STDIN> to pipe in filelist\n";
}

1;

