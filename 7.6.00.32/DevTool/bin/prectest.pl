#!/usr/bin/perl
#
# @(#)prectest   1997-12-02
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
use Carp;
use File::Copy;
use Getopt::Std;

sub RestoreParamFile;
sub SaveParamFile;
sub PutParam;

if (!getopts ('hn') || $opt_h ) {
  print <DATA>;
  exit;
}

if (!$opt_n) {
    if ($RELVER ne "R71"){
	$NEEDED="Needed components: slowci and load !\n";
	print "$NEEDED";
	$NEEDED="Needed environment variables: \n";
	print "$NEEDED";
	$NEEDED="          SERVERDB, SERVERNODE,SUPERUID, SUPERPWD !\n";
	print "$NEEDED";

	chdir($TEST) || die "Can't change to directory $TEST: $!";
	chdir(pc) || die "Can't change to directory $TEST\pc: $!";
	chdir(wrk) || die "Can't change to directory $TEST\pc\wrk: $!";

	$DBPARAM = "$DBROOT/pgm/getparam $SERVERDB";
	$RUNDIR  = `$DBPARAM RUNDIRECTORY`; chomp $RUNDIR;

	system("x_stop $SERVERDB");

	system("x_start $SERVERDB") == 0 or die "x_start doesn't work";

	SaveParamFile;

	PutParam "_MAXSERVERDB 1";

#	system("$DBROOT/pgm/slowci$PROG_EXT -b $TEST/pc/bin/DBINIT $SERVERDB $SUPERUID $SUPERPWD") == 0
#	    or die "slowci$PROG_EXT DBINIT doesn't work";
	system("$DBROOT/pgm/slowci$PROG_EXT -b $TEST/pc/bin/DBINIT $SUPERUID $SUPERPWD") == 0
	    or die "slowci$PROG_EXT DBINIT doesn't work";

	RestoreParamFile;

	system("xload -u $SUPERUID,$SUPERPWD -b $DBROOT/env/SYSDBA.ins") == 0 or die "xload SYSDBA doesn't work";

	PutParam "_MAXSERVERDB 2";
    }
    else {
	do "pcstartdb$TOOLEXT";
    }
}
# ***********************************************************
# Run the tests

$TESTDIR = $ENV{OWN}."/test/pc";
$TESTPROG="$TESTDIR/bin/prectest$TOOLEXT";
do $TESTPROG or die "$TESTPROG doesn't work.";

# ***********************************************************

if (!$opt_n) {
    system("x_stop $SERVERDB");
}

sub SaveParamFile
{
    copy("$RUNDIR/xinitdb.sav", "$RUNDIR/xinitdb.sav2");
}

sub RestoreParamFile
{
    rename("$RUNDIR/xinitdb.sav2", "$RUNDIR/xinitdb.sav");
}

sub PutParam
{
    local($PARAM);
    $PARAM = $_[0];shift;
    system("$DBROOT/pgm/putparam $SERVERDB $PARAM") == 0
	or croak "Error while executing \"$DBROOT/pgm/putparam $DBNAME $PARAM\"\n"."message: $!\n";
}



__DATA__

 USAGE: prectest -hn

           Runs all tests of the Interfacegroup.

 OPTIONS:    -h    prints this help
             -n    do not start the serverdb.
