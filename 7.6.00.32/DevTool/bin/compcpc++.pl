#!/usr/bin/perl
#
# @(#)compcpc++ 
# cba 1999-12-29
#
# this software is based on properitary software originaly developed by 
# @(#)ntcpc++ 1999-01-14
# R. Roedling, SAP AG
#
#
#    ========== licence begin  GPL
#    Copyright (C) 2001 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end
#


package compcpcxx;
use File::Basename;
use File::Copy;

sub main {
	local @ARGV = @_;

	my $COMPILE_GOSCRIPT = $ENV{COMPILE_GOSCRIPT} 
		if (defined $ENV{COMPILE_GOSCRIPT});
	my $COMPILE_DBROOT = $ENV{COMPILE_DBROOT}
		if (defined $ENV{COMPILE_DBROOT});
	my $TOOLVARS = $ENV{TOOLVARS}
		if (defined $ENV{TOOLVARS});

	do "$TOOLVARS";
	if ($@) { 
		warn "Error while executing toolvars:\n", "$@";
		exit;
	}

	ToolTrace ("compcpc++ called with @ARGV\n");

	my $cpc;
	my @cmd;

	if ($OS eq "UNIX") {
		$cpc = $DBROOT."/bin/cpc";
	} else {
		$cpc = $DBROOT."\\bin\\cpc.exe";
	}

	my $infile = $ARGV[0];
	my $source = basename ($infile, "\.cpp");

	unless ( -f $cpc ) { 
		warn $cpc." doesn't exist!\n";
		exit 2;
	}

	copy $infile, $source.".cpc" ;
	unlink $source.".cpp";

	if ((defined $COMPILE_DBROOT) && ($COMPILE_DBROOT ne "")) {
		$DBROOT = $COMPILE_DBROOT;
		$ENV{DBROOT} = $DBROOT;
	}

	if ((defined $COMPILE_GOSCRIPT) && ($COMPILE_GOSCRIPT ne "")) {
		@cmd = ($TOOLSHELL, $TOOL."/bin/".$COMPILE_GOSCRIPT);
		unless (system (@cmd) == 0) {
			warn "Error while executing ".join (" ", @cmd)."\nmessage: ".$!."\n";
			exit;
		}
	}

	@cmd = ("cpc", "-H", "nocheck", "-s", "-S", "adabas", "-E", "cplus", "-c");
	push @cmd, $source;
	my $rc = system (@cmd);

	if ($OS eq "UNIX") {
		rename $source.".C", $source.".cpp";
	}

	if ($rc == 0) {
		unlink $source.".cpc" 
	} else {
		open (FILE_IN, $source.".pcl");
		print <FILE_IN>;
		close (FILE_IN);
	}
}

main (@ARGV);
