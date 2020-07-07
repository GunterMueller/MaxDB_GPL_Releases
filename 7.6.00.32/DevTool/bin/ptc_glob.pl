#!/usr/bin/perl
# ptc_glob      *** internal script
#
# @(#)ptc_glob  6.1.2  1995-09-20
#
# make a main program from module with the sizeof-statements made by
# ptoc (option -Y) and execute it
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

use Env;
use File::Basename;

do "$TOOLVARS";
if ( $@ ) { print "$@"; exit }

$ARGV[0] =~ tr[\\][/];
$Param = $ARGV[0];
push @CCFLAGS, "-I$TOOL/incl";
$FFILE = "s_$Param";
$BASE = basename($Param);
($BASE) = $BASE =~ /(\.*[^\.]*)/;
$PROG = "$BASE.exe";
$SFILE = "$BASE.gsz";

$LC = `echo $SFILE | cut -c6`; chomp $LC;

CASE: {
	if ( $LC =~ /[0-9]/ ) {
		$LC1 =`echo $SFILE | cut -c2,4,5,6`; chomp $LC1;
		$LC2 =`echo $SFILE | cut -c2,3,4,5,6`; chomp $LC2;
		last CASE;
	}
	$LC1 = `echo $SFILE | cut -c2,4,5`; chomp $LC1;
	$LC2 = `echo $SFILE | cut -c2,3,4,5`; chomp $LC2;
}

open(FILE_OUT, ">$FFILE") || die "Can't open $FFILE: $!";
open(PIPE_IN, "$TOOL/pgm/ptc_delbody $Param |");
while(<PIPE_IN>) {
	print FILE_OUT $_ unless /extern/;
}
close(PIPE_IN);
print FILE_OUT "#include <stdio.h>\n";
print FILE_OUT "main()\n{";
open(FILE_IN, "<$Param") || die "Can't open $Param: $!";
while(<FILE_IN>) {
	next unless m!/\*size!;
	s!/\*size(.*)\*/!$1!;
	print FILE_OUT $_;
}
close(FILE_IN);
print FILE_OUT "return 0;";
print FILE_OUT "}";
print FILE_OUT "\n";
close(FILE_OUT);

if ( !system($CC, @CCFLAGS, "-o", $PROG, $FFILE) ) {

    system("./$PROG | egrep \"$LC1|$LC2|td\"  > $SFILE");
    system("echo                            >>$SFILE");
} else {
	# return 2!
    2;
}

