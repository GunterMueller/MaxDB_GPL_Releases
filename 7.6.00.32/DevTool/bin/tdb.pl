#!/usr/bin/perl
# tdb           *** internal script
#
# @(#)tdb    6.2.7  1996-08-27
#
# Extract and copy an installable release
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

package tdb;
use Env;
use Carp;
use File::Copy;
use File::Find;
require "hostname.pl";

do "$TOOLVARS";
if ( $@ ) { print "$@"; exit }

$USAGE = "Usage: tdb [hostname] f|q|s [-d directory]\n";

while ($_ = $ARGV[0]) {
    shift;
    last if /^--$/;

    if(/^(f|q|s)$/i)	{ $TYPE = $_; next }

    if(m!^(-d|/d)$!i)	{ $BASE = shift; $BASE =~ tr!\\!/!; next }

    if(m!^(-.*|/.*)$!)	{ print $USAGE; exit 1 }

    if(/(.*)/)			{ $HOST = $_; next }

} #while

if ( $TYPE eq "" ) { print $USAGE; exit 1 }

chdir($DBROOT);

find(\&GetNameWithVers, $DBROOT);
foreach $File (@Files) {
	($Name) = $File =~ /(.*)\.f/i ;
	if ( -f $Name) { unlink $Name}
	copy $File, $Name;
}

find(\&GetNameLib, $DBROOT);
foreach $File (@Files) {
	unlink "$NAME.lib";
	rename "$NAME.a", "$NAME.lib";
	unlink "$NAME.lib.f";
	unlink "$NAME.lib.q";
}


if ( $HOST ne "" or $BASE ne "" ) {

	if ( $HOST ne "" ) { $HOST =~ tr/[a-z]/[A-Z]/ }
	else { $HOST = &hostname }

	if ( $HOST eq &hostname ) {
		if ( $BASE eq "" or $BASE eq "." or $BASE eq $DBROOT )  { exit 0 }
	}
	if ( $BASE eq "" ) { $BASE = `$REMCL sh $HOST echo %DBROOT%` }
	if ( $BASE eq "" or $BASE eq $DBROOT ) { $BASE = "."; }

	local($DBROOT) =~ tr!\\!/!;
	system("vdistrib -c -b -i -r $DBROOT -s nt_intel total 2>tdb.log") == 0
       or croak "Error while executing vdistrib -c -b -i -r $DBROOT -s nt_intel total\n".
	   "message: $!\n";

	my $DLIST = "./etc/distlist";

	system("cat ./etc/filelist | $REMCL -sv mput $HOST . $BASE 2>&1") == 0
       or croak "Error while executing type ./etc/filelist | $REMCL -sv mput $HOST . $BASE 2>&1\n".
	   "message: $!\n";

	system("$REMCL -sv mput $HOST $DLIST $BASE");

	$_ = $TYPE;
	s/^f/fast/i || s/^q/quick/i || s/^s/slow/i;

	my $MAPDIR = "$WRK/$_/map";
	opendir(MAP, "$MAPDIR");
	local(@mapfiles) = readdir(MAP);
	closedir(MAP);

	$MAPDIR =~ tr!\\!/!;
	foreach $f (@mapfiles) {
		system("$REMCL -sv mput $HOST $MAPDDIR/$f $BASE/map/.") == 0
			or croak "Error while executing $REMCL -sv mput $HOST $MAPDDIR/$f $BASE/map/.\n".
			"message: $!\n";
	}
}

sub GetNameWithVers {
	push @Files, $File::Find::name if $File::Find::name =~ /.*\.s$/;
}

sub GetNameLib {
	push @Files, $File::Find::name if $File::Find::name =~ /.*\.a$/;
}

__END__
