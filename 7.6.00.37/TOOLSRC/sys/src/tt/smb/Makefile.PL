#!/usr/bin/perl
#
# 2000-10-06 cba: build in support for arch ix86-linux
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


# we need perl xs header files from perl 5.005 or higher
require 5.005;

use ExtUtils::MakeMaker;
use Config;
use Cwd;

sub main {
	my ($InstBin, $InstLib) = get_inst_dirs (get_tool_dir());
	my $ExtLib = get_ext_lib ();
	my @CFiles = ("smbglue.c", "strcasecmp.c");
	my $Define = get_defines ();

	my @MakeOpts = (
		NAME => "SMB",
		INSTALLDIRS => "site",
		INSTALLSITELIB => $InstLib,
		INSTALLSITEARCH => $InstLib,
		INSTALLBIN => $InstBin,
		INSTALLSCRIPT => $InstBin,
		INSTALLARCHLIB => $InstLib,
		C => ["SMB.c", @CFiles],
		OBJECT => '$(O_FILES)',
		DEFINE => $Define,
		MYEXTLIB => $ExtLib,
		PM => { "SMB.pm" => '$(INSTALLSITELIB)/SMB.pm',
			"IO/SMB.pm" => '$(INSTALLSITELIB)/IO/SMB.pm',
			"SMB/File/Copy.pm" => '$(INSTALLSITELIB)/SMB/File/Copy.pm' }
	);

	my @ExtOpts = get_ext_opts ();
	push @MakeOpts, @ExtOpts if ($#ExtOpts >= 0);

	WriteMakefile (@MakeOpts);
}

sub get_inst_dirs {
	my $InstDir = shift;
	my $InstBin = $InstDir."/bin";
	my $InstLib = $InstDir;
	if ($^O =~ /win32/) {
		$InstLib .= "/lib/Perl";
	} else {
		$InstLib .= "/lib/perl5";
	}

	return ($InstBin, $InstLib);
}

sub get_tool_dir {
	my $TOOL = $ENV{"TOOL"};
	unless (defined $TOOL && $TOOL ne "") {
		die "\$TOOL undefined, please set \$TOOL\n";
	}
	return $TOOL;
}

sub get_ext_lib {
	my $ext_lib;
	my $cwd = getcwd ();
	my $ArchName = $Config{"archname"};
	my $OsName = $Config{"osname"};
	my $OsVers = $Config{"osvers"};

	if ($ArchName eq "aix") {
		# it seems that some peopole use a aix 4.3.1.0 maked perl
		$OsVers = "4.3.2.0"	if ($OsVers =~ /^4\.3\.\d\.\d/);
		$ext_lib = $cwd."/arch/".$ArchName."-".$OsVers."/libsmb.ro";
	} elsif ($OsName eq "hpux") {
		# HPUX 11.00 lib has some problems with pathinfo2 use 10.20
		# $ext_lib = $cwd."/arch/".$OsName."-".$OsVers."/libsmb.ro";
		$OsVers = "10.20";
		$ext_lib = $cwd."/arch/".$OsName."-".$OsVers."/libsmb.ro";
	} elsif ($ArchName =~ /i\d86-linux/i) {
		$ext_lib = $cwd."/arch/i386-linux/libsmb.ro";
	} else {
		$ext_lib = $cwd."/arch/".$ArchName."/libsmb.ro";
	}
	return $ext_lib;
}

sub get_ext_opts {
	my @ExtOpts = ();
	my $OsName = $Config{"osname"};

	if ($OsName eq "hpux") {
		# long long only available in extended ansi c,
		# most versions perl are made in ansi c mode
		my @MyCCFlags = ();
		my @TheirCCFlags = split (' ', $Config{"ccflags"});
		foreach (@TheirCCFlags) {
			if ($_ eq "-Aa") {
				push @MyCCFlags, "-Ae";
			} else {
				push @MyCCFlags, $_;
			}
		}
		push @ExtOpts, "CCFLAGS", join (" ", @MyCCFlags);
	}
	
	return @ExtOpts;
}

sub get_defines {
	my $Defines;
	my $ArchName = $Config{"archname"};

	if ($ArchName eq "aix") {
		$Defines = "-DAIX";
	} elsif ($ArchName =~ /i\d86-linux/i) {
		$Defines = "-DI386 -DLINUX";
	} elsif ($ArchName eq "RM400") {
		$Defines = "-DRELINAT";
	} else {
		$Defines = "";
	}
	return $Defines;
}

main (@ARGV);

