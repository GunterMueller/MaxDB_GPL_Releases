#!/usr/bin/perl
#
#    ========== licence begin LGPL
#    Copyright (C) 2002 SAP AG
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#    ========== licence end
#


package main;

sub main {
	local @ARGV = @_;

	import SMB;
	import SMB::File::Copy;
	import Symbol;
	import Getopt::Std;

	getopts ('c:d:x:hp:r:u:', \%opts);

	unless (defined $opts{'p'} && defined $opts{'r'}) {
		usage ();
		return 0;
	}

	if (defined $opts{'h'}) {
		usage ();
		return 0;
	}

	my %layout = get_platforms ($opts{'p'}); 
	unless (countkeys (%layout) >= 0) {
		print "no platform given\n";
		return 0;
	}

	my $release = $opts{'r'};
	unless (defined $release) {
		print "no release given\n";
		return 0;
	}

	my $extract_pattern = $opts{'x'};
	my $copy_pattern = $opts{'c'};

	my $destdir = $opts{'d'};
	if (defined $destdir) {
		$destdir =~ s/\\/\//g;
	} else {
		$destdir = ".";
	}
	
	my $userpwd = defined $opts{'u'} ? 
		$opts{'u'} : 
		(getpwuid ($<))[0];

	my $sapcar = get_sapcar ();

	#
	# show them the parameters
	#
	print "platforms        : ".join (" ", keys (%layout))."\n";
	print "release          : ".$release."\n";
	print "destination dir  : ".$destdir."\n";
	print "sapcar           : ".$sapcar."\n";

	my $srvdir = '\\\\p26325.bea.sap-ag.de\\CD-Images\\DB_REL';

	#
	# make sure that we got a password
	#
	my ($user, $pwd) = split (/,/, $userpwd);
	unless (defined $pwd) {
		print "\n";
		$pwd = SMB::getsmbpass ($user."\'s password: ");
		print "\n";
	}

	for (;;) {
		SMB::logon ($user, $pwd);

		#
		# test release directory
		#
		my $dir = $srvdir.'/'.$release;
		$dir =~ s/\//\\/g;
		my ($rc, $err) = test_smbdir ($dir);
		last if ($rc == 0);

		if ($user =~ /^\\\\SAP_ALL\\.+/) {
			print $err."\n";
			return 0;
		}
		print "connect for user ".$user." failed";

		$user = "\\\\SAP_ALL\\".$user;
		print " try user ".$user."\n";
	}

	#
	# is there a hints file in release directory?
	#
	my $hintsfilename = $srvdir.'/'.$release.'/hints';
	$hintsfilename =~ s/\//\\/g;
	my @stat = SMB::stat ($hintsfilename);
	if (($stat[2]) && ! ($stat[2] & 040000)) {
		# regular file exists
		my $fh = gensym ();
		SMB::sysopen ($fh, $hintsfilename);
		my $hints = '';
		while ($_ = SMB::getline ($fh)) {
			chomp;
			$hints .= $_."\n";
		}
		SMB::close ($fh);
		
		print "\nhints file begin\n";
		eval ($hints);
		print "hints file done\n\n";
	}
	
	#
	# set defaults for $extract_pattern and $copy_pattern
	#
	unless (defined $extract_pattern) {
		$extract_pattern = 'sapdb-server.+\.sar$';
	}
	unless (defined $copy_pattern) {
		$copy_pattern = '^SETUPS\\\\.+\\\\.+';
	}

	#
	# show them the parameters
	#
	print "extract matching : ".$extract_pattern."\n";
	print "copy matching    : ".$copy_pattern."\n";
	print "\n";

	#
	# test any platform directory
	#
	foreach $_ (keys %layout) {
		my $dir = $srvdir.'/'.$release.'/'.$_;
		$dir =~ s/\//\\/g;
		my ($rc, $err) = test_smbdir ($dir);
		if ($rc != 0) {
			print $err."\n";
			return 0;
		}
	}

	my @platformdirs = keys %layout;

	$rc = copy_with_extract (
		'srvdir' => $srvdir, 
		'release' => $release,
		'destdir' => $destdir,
		'pattern' => $extract_pattern,
		'sapcar' => $sapcar,
		'layout' => \%layout
	); 

	if ($rc != 0) {
		print "copy with extract failed\n";
		return 0;
	}

	$rc = copy_without_extract (
		'srvdir' => $srvdir, 
		'release' => $release,
		'destdir' => $destdir,
		'pattern' => $copy_pattern,
		'platformdirs' => \@platformdirs
	); 

	if ($rc != 0) {
		print "copy without extract failed\n";
		return 0;
	}

}

sub copy_with_extract {
	my %args = @_;
	my $srvdir = $args{'srvdir'};
	my $release = $args{'release'};
	my $destdir = $args{'destdir'};
	my $pattern = $args{'pattern'};
	my $sapcar = $args{'sapcar'};
	my %layout = %{$args{'layout'}};

	#
	# get matching packet names
	#
	foreach $_ (keys %layout) {
		my $dir = $srvdir.'/'.$release.'/'.$_;
		$dir =~ s/\//\\/g;
		my @dirlist = get_dirlist ($dir);
		@dirlist = get_matching ($pattern, @dirlist);
		$layout{$_} = \@dirlist;
	}

	#
	# make target directories
	#
	foreach $_ (keys %layout) {
		unless (make_target_dir ($destdir, $_)) {
			print "cannot create target dir ".$destdir."/".$_."\n";
			return -1;
		}
	}	

	#
	# copy from src to target directory
	# extract and remove package
	#
	my $rc = 0;
	while (($key, $val) = each (%layout)) {
		foreach my $file (@$val) {
			my $src = $srvdir.'/'.$release.'/'.$key.'/'.$file;
			$src =~ s/\//\\/g;

			my $dest = 
				$destdir ne "" ?
				$destdir.'/'.$key.'/'.$file :
				$key.'/'.$file;
			$dest =~ s/\\/\//g;

			print "copy from ".$src." to ".$dest."\n";
			$rc = !(SMB::File::Copy::copy ($src, $dest, 'binary'));
			unless ($rc == 0) {
				print "aborting cause of ".
				      "SMB::File::Copy::copy problem\n";
				last;
			}

			my @cmd = 
			($sapcar, '-R', $destdir.'/'.$key, '-xvf', $dest);

			$rc = exec_sapcar (@cmd);
			print "removing ".$dest."\n";
			unlink ($dest);

			unless ($rc == 0) {
				print "aborting cause of ".$sapcar." error\n";
				last; 
			}
		
			if ($key =~ /NT\/I386/i) {
				$rc = rename_iso9660 ($destdir.'/'.$key);
				unless ($rc == 0) {
					print 
					"aborting cause of iso9660 problem\n";
					last; 
				}
			}

		}
		last unless ($rc == 0);
	}
	return ($rc);
}

sub copy_without_extract {
	my %args = @_;
	my $srvdir = $args{'srvdir'};
	my $release = $args{'release'};
	my $destdir = $args{'destdir'};
	my $pattern = $args{'pattern'};
	my @platformdirs = @{$args{'platformdirs'}};

	my $rc;
	foreach $lay (@platformdirs) {
		my $dir = $srvdir.'/'.$release.'/'.$lay;
		my @tree = get_matching ($pattern, get_tree ($dir));
	
		foreach $filename (@tree) {
			my ($subdir) = ($filename =~ /(\S+)\\[\w\.]+$/);
			make_target_dir ($destdir.'/'.$lay, $subdir);

			my $src = $srvdir.'/'.$release.'/'.$lay.'/'.$filename;
			$src =~ s/\//\\/g;

			my $dest = $destdir.'/'.$lay.'/'.$filename;
			$dest =~ s/\\/\//g;

			print "copy from ".$src." to ".$dest."\n";
			$rc = !(SMB::File::Copy::copy ($src, $dest, 'binary'));
			unless ($rc == 0) {
				print "aborting cause of ".
				      "SMB::File::Copy::copy problem\n";
				last;
			}
		}
	}
	return ($rc);
}

sub usage {
	print 
	" copy archived sapcar packages\n".
	" from p26325 to destination directory\n".
	" and extract archive to cd image\n".
	"\n".
	"usage:\n".
	" -d destination directory     ".
	"(optional, default='.')\n".
	" -c pattern matching for files to copy\n".
	"                              ".
	"(optional, default='^SETUPS\\\\.+\\\\.+')\n".
	" -x pattern matching for files to extract\n".
	"                              ".
	"(optional, default='sapdb-server.+\.sar$')\n".
	" -h help                      ".
	"(optional)\n".
	" -p platform                  ".
	"(mandatory)\n".
	" -r release                   ".
	"(mandatory)\n".
	" -u user,pwd                  ".
	"(optional, default=getuid(),prompt for password)\n".
	"\n";

	return;
}

sub rename_iso9660 {
	my ($dirname) = @_;
	my $rc;

	$rc = opendir (DIR, $dirname);	
	return (-1) unless $rc;
	@files = readdir (DIR);
	closedir (DIR);

	foreach $filename (@files) {
		next if ($filename eq '.');
		next if ($filename eq '..');

		next unless (-f $dirname.'/'.$filename);
	
		my $isoname = $filename;
		$isoname =~ tr/a-z/A-Z/;

		if ($isoname ne $filename) {
			unlink ($dirname.'/'.$isoname);
			rename ($dirname.'/'.$filename, $dirname.'/'.$isoname);
			print ("renaming ".$dirname."/".$filename.
			       " to ".$dirname."/".$isoname."\n");
		}
	}
	return 0;
}

sub get_tree {
	my ($src, $dir) = @_;
	$src =~ s/\//\\/g;

	# first read list of current directory 
	my @dirlist = ();
	my $dirhandle = \*{'DIR'};
	if (defined $dir && $dir ne '') {
		SMB::diropen ($dirhandle, $src.'\\'.$dir);
	} else {
		SMB::diropen ($dirhandle, $src);
	}
	while (my $item = SMB::readdir ($dirhandle)) {
		# skip self directory
		next if ($item eq '.');

		# skip parent directory
		next if ($item eq '..');

		# translate then to upper case and keep them
		$item =~ tr/a-z/A-Z/;
		if (defined $dir && $dir ne '') {
			push (@dirlist, $dir.'\\'.$item);
		} else {
			push (@dirlist, $item);
		}
	} 
	SMB::dirclose ($dirhandle);

	# do recursive get_tree on sub directories
	my @tree = ();
	foreach $item (@dirlist) {
		my @stat = SMB::stat ($src.'\\'.$item);
		if ($stat[2] & 040000) {
			# directory
			push @tree, get_tree ($src, $item);
		} else {
			# regular file
			push @tree, $item;
		}
	}	
	return @tree;
}

sub exec_sapcar {
	my @cmd = @_;
	my $fh = \*{'sapcar'};

	my $rc = 0;
	my $line;

	print join (" ", @cmd)."\n";
	open ($fh, join (" ", @cmd)." |");
	while (<$fh>) {
		chomp;
		$line = $_;		# save last message

		print $_."\n";
		#
		# see if error occured
		#
		if ($_ =~ /^$cmd[0]:.*\(error \d+\)/) {
			$rc = -1;
			last;
		}
	}
	close ($fh);
	
	#
	# look for success message
	#
	unless ($line =~ /^$cmd[0]: \d+ file\(s\) extracted$/) {
		$rc = -1;
	}
	return ($rc);
}

sub countkeys {
	my %hash = @_;
	my @keys = keys %hash;
	return $#keys;
}

sub make_target_dir {
	my ($destdir, $subdir) = @_;

	my $dir = $destdir ne "" ?  $destdir.'/'.$subdir : $subdir;
	$dir =~ s/\\/\//g;
	return (recmkdir ($dir, (0777 &~ umask)));
}

sub recmkdir {
	my ($dir, $mode) = @_;

	# try to make it
	my $rc = mkdir ($dir, $mode);

	# return good case
	return ($rc) if ($rc);
	return (!$rc) if ($! =~ /file exists/i);

	# realy bad case
	unless ($! =~ /no such file or directory/i) {
		print "mkdir ".$dir." : ".$!."\n";
		return ($rc);
	}

	# end of recusion
	my ($topdir) = ($dir =~ /^(.+)\/\w+/);
	return ($rc) unless defined ($topdir);

	# create top level directory
	$rc = recmkdir ($topdir, $mode);
	
	# return if creating toplevel dir failed
	return ($rc) unless ($rc);

	$rc = mkdir ($dir, $mode);
	return ($rc);
}

sub get_matching {
	my ($pattern, @in) = @_;
	my @out = ();

	foreach $_ (@in) {
		push @out, $_ if ($_ =~ /$pattern/i);
	}
	return @out;	
}

sub get_dirlist {
	my ($dir) = @_;
	my @dirlist = ();
	
	my $dirhandle = \*{'DIR'};
	SMB::diropen ($dirhandle, $dir);
	while (my $item = SMB::readdir ($dirhandle)) {
		# skip self directory
		next if ($item eq '.');

		# skip parent directory
		next if ($item eq '..');
		
		# skip any other directory
		my @stat = SMB::stat ($dir.'\\'.$item);
		next if ($stat[2] & 040000);
		
		push (@dirlist, $item);
	} 
	SMB::dirclose ($dirhandle);
	return @dirlist;
}

sub test_smbdir {
	my ($dir) = @_;
	my @stat = SMB::stat ($dir);
	unless ($#stat >= 0) {
		return (-1, "no such file or directory ".$dir);
	}
	
	unless ($stat[2] & 040000) {
		return (-1, "not a directory ".$dir);
	}
	return (0, undef);
}

sub get_platforms {
	my ($opt) = @_;
 
	return () unless (defined $opt); 

	my %platforms;
	foreach $_ (split (/,/, $opt)) {
		if ($_ eq 'NT') {
			$platforms{'NT/I386'} = ();
			next;
		}
		$platforms{'UNIX/'.$_} = ();
	}
	return %platforms;
}

sub get_sapcar {
	my @tools = ('sapcar', 'SAPCAR');
	my $sapcar;
	
	foreach $_ (@tools) {
		my $fh = \*{'IN'};

		my $rc = open ($fh, $_." |");
		if (defined <$fh>) {
			$sapcar = $_;
		}
		close ($fh);
	}
	return ($sapcar);
}

package Symbol;

sub BEGIN {
        @ISA = ('Exporter');
        @EXPORT = ('gensym');
}

sub gensym () {
	my $name = "GEN" . $genseq++;
	my $ref = \*{$genpkg . $name};
	delete $$genpkg{$name};
	$ref;
}

package SMB::File::Copy;

sub BEGIN {
        @ISA = ('Exporter');
        @EXPORT = ();
}

sub copy {
        return SMB::copy_from_smb (@_);
}

sub import {
        local $Exporter::ExportLevel = 1;
        Exporter::import (@_);
	
	import SMB;
}

package SMB;

sub BEGIN {
        @ISA = ('Exporter');
        @EXPORT = ();
}

sub import {
        local $Exporter::ExportLevel = 1;
        Exporter::import (@_);

        bootstrap SMB;
}

sub AUTOLOAD {
	die unless ($AUTOLOAD =~ "bootstrap");

	require DynaLoader;
	push @ISA, 'DynaLoader';

	local @INC = ('.');
	bootstrap SMB;
}

sub logon {
	my ($user, $pass, $domain) = @_;
	
	if ($user =~ /^\\\\(\w+)\\(\w+)$/g) {
		$domain = $1;
		$user = $2;
	}

	return (SMB::logon_to_smb ($user, $pass, $domain));
}

package Getopt::Std;
sub BEGIN {
	@ISA = ('Exporter');
	@EXPORT = ('getopt', 'getopts');
}

sub getopt {
	my ($argumentative, $hash) = @_;
	local @EXPORT = ();

	while (@ARGV && ($_ = $ARGV[0]) =~ /^-(.)(.*)/) {
		my ($first, $rest) = ($1, $2);
		if (index ($argumentative, $first) >= 0) {
			shift @ARGV;
			$rest = shift @ARGV if ($rest eq '');
			add_options ($hash, $first, $rest);
		} elsif ($rest eq '') {
			shift @ARGV;
			add_options ($hash, $first, 1);
		} else {
			$ARGV[0] = "-$rest";
			add_options ($hash, $first, 1);
		}
	}
	export_options () unless (ref $hash);
}

sub getopts {
	my ($argumentative, $hash) = @_;
	my $errs = 0;
	local @EXPORT = ();

	my @args = split (/ */, $argumentative);
	while (@ARGV && ($_ = $ARGV[0]) =~ /^-(.)(.*)/) {
		my ($first, $rest) = ($1, $2);
		$pos = index ($argumentative, $first);
		if ($pos >= 0) {
			if (defined ($args[$pos + 1]) && 
			($args[$pos + 1] eq ':')) {
				shift @ARGV;
				if ($rest eq '') {
					++$errs unless @ARGV;
					$rest = shift @ARGV;
				}
				add_options ($hash, $first, $rest);
			} elsif ($rest eq '') {
				shift @ARGV;
				add_options ($hash, $first, 1);
			} else {
				$ARGV[0] = "-$rest";
				add_options ($hash, $first, 1);
			}
		} else {
			warn "Unknown option: $first\n";
			$errs++;
			if ($rest eq '') {
				shift @ARGV;
			} else {
				$ARGV[0] = "-$rest";
			}
		}
	}
        
	export_options () unless (ref $hash);
	$errs == 0;
}

sub add_options {
	my ($hash, $key, $value) = @_;
	if (ref $hash) {
		$$hash{$key} = $value;
		return;
	}

	${"opt_$key"} = $value;
	push @EXPORT, "\$opt_$key";
}

sub export_options {
	local $Exporter::ExportLevel = 2;
	import Getopt::Std;
}


package Exporter;
sub BEGIN {
	$ExportLevel = 0;
}

sub import {
	my $pkg = shift;
	my $callpkg = caller ($ExportLevel);

	*exports = *{"${pkg}::EXPORT"};
	foreach (@exports) {
		$_ =~ s/^(\W)//;
		my $type = (defined $1) ? $1 : '&';

		*{"${callpkg}::$_"} =
		$type eq '&' ? \&{"${pkg}::$_"} :
		$type eq '$' ? \${"${pkg}::$_"} :
		$type eq '@' ? \@{"${pkg}::$_"} :
		$type eq '%' ? \%{"${pkg}::$_"} :
		$type eq '*' ? \*{"${pkg}::$_"} :
		undef;
	}
}

package main;

main (@ARGV);

