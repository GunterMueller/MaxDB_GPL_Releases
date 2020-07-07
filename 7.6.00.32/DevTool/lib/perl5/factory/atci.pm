#!/usr/bin/perl
#
# source:
# :tt/factory/atci.pm
#
# dest:
# $TOOL/lib/perl5/factory/atci.pm
#
# cba 2000-09-01
#
# atci - all to cd images
#
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


package factory::atci;
use Exporter;
use Term::ReadKey;
use Symbol;
use Config;
use isar;

@ISA = qw (Exporter);

sub get_build {
	return $ENV{"BUILDPRAEFIX"};
}

sub ask_safty_question {
	my $cdimagepath = shift;

	print "your archive files will be written to\n";
	print "\"".$cdimagepath."\"\n";
	print "is this correct[y/n]? ";
	
	my $tty = gensym ();
	die "cannot open /dev/tty\n"
		unless (open ($tty, "< /dev/tty"));

	ReadMode ("raw", $tty);
	my $key;
	$key = ReadKey (0, $tty);	
	ReadMode ("restore", $tty);
	print "\n";
	
	close ($tty);	
	return ($key);
}

sub create_archive {
	my $cdimagepath = shift;
	my $filename = shift;
	my $packetname = shift;
	my $build = shift;
	my @cmd;

	if (defined $build) {
		my $localfile;
		my @file = split (/\./, $packetname);
		my $ext = pop @file;
		if ($ext =~ /car/i || $ext =~ /sar/i) {
			$localfile = sprintf 
			"%s.%02d.%s", join (".", @file), $build, $ext;
		} else {
			$localfile = sprintf 
			"%s.%s.%02d", join (".", @file),$ext, $build;
		}

		@cmd = ("-cvf", 
		$cdimagepath."/".$packetname.",".$localfile, "-A", $filename);
	} else {
		@cmd = 
		("-cvf", $cdimagepath."/".$packetname, "-A", $filename);
	}
	print ("isar ".join (" ", @cmd)."\n");
	return (isar::isar (@cmd));
}

sub get_relname {
	my $relname;

	my $relver = $ENV{"RELVER"};
	my $correction = $ENV{"CORRECTION_LEVEL"};

	$relver =~ /R(\d+)/;
	$relname = 10000 * $1 + 100 * $correction + get_build ();

	return $relname;
}

sub get_archpath {
	my $archpath;
	my $osname = $Config{"osname"};
	my $archname = $Config{"archname"};

	my $use_64bit = 0;
	$use_bit64 = 1
		if (defined $ENV{"BIT64"});

	if ($osname =~ /hpux/i && $use_bit64 == 0) {
		$archpath = "UNIX/HP11_32";
		$archpath = "UNIX/HP10_32"
			if (get_osrel () =~ /10\.20/);
	} elsif ($osname =~ /hpux/i && $use_bit64 == 1) {
		$archpath = "UNIX/HP11_64";
	} elsif ($osname =~ /aix/i && $use_bit64 == 0) {
		$archpath = "UNIX/AIX_32";
	} elsif ($osname =~ /aix/i && $use_bit64 == 1) {
		$archpath = "UNIX/AIX_64";
	} elsif ($osname =~ /solaris/i && $use_bit64 == 0) {
		$archpath = "UNIX/SUNOS_32" if ($archname == 'sparc' || $archname == 'sun' );
		$archpath = "UNIX/SOLARIS_I386" if ($archname == 'i386');
		$archpath = "UNIX/SUN7_32" if (get_osrel () =~ /5\.7/);
	} elsif ($osname =~ /solaris/i && $use_bit64 == 1) {
		$archpath = "UNIX/SUNOS_64" if ($archname == 'sparc' || $archname == 'sun');
		$archpath = "UNIX/SOLARIS_X86_64" if ($archname == 'x86_64');
	} elsif ($osname =~ /linux/ && $use_bit64 == 0) {
		$archpath = "UNIX/LINUX_32";
	} elsif ($osname =~ /dec_osf/i) {
		$archpath = "UNIX/DEC_64";
	} elsif ($osname =~ /svr4/i && 
		 $archname =~ /RM400/i && $use_bit64 == 0) {
		$archpath = "UNIX/RLX_32";
	} elsif ($osname =~ /svr4/i && 
		 $archname =~ /RM400/i && $use_bit64 == 1) {
		$archpath = "UNIX/RLX_64";
	} else {
		die "unknown OSPSEC/MACH combination\n";
	}

	return $archpath;
}

sub get_osrel {
	my $fh = gensym ();
	my $cmd = "uname -r";
	open ($fh, $cmd." |");

	my $osrel = <$fh>;
	chomp $osrel;
	close $fh;
	return $osrel;
}

1;
