#!/usr/bin/perl
# pcprof
#
# @(#)pcvt  7.1  1998-12-15
#
# Profiles a executable.
#
# Burkhard Diesing
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
use Getopt::Std;

sub System;

$PFLAGS="/NOLOGO";
$PREP1OPT="/OM /FT";

   die "ERROR: Invalid Operatingsystem.\n";

if (!getopts ('hno:i:v') || $opt_h ) {
   print <DATA>;
   exit;
}

if (@ARGV < 1) {
   print <DATA>;
   exit;
}

if ( $opt_i ) {
    $INFILE=$opt_i;
    if ( -f "$INFILE" ) {
	open(FILE_IN, "<$INFILE") || die "Can't open $INFILE (input): $!";
	while(<FILE_IN>) {
	    if ( /^[#;].*/i ) {
		    next;
            }
	    if (/\[INCLUDE\].*\n/i) {
		$PARAM="/INC";
		next;
	    }
	    else {
		if (/\[EXCLUDE\].*\n/i) {
		    $PARAM="/EXC";
		    next;
		}
		else {
		    if ($PARAM eq "") {
			print "ERROR: Keyword '$_' not allowed in $INFILE.";
			exit -1;
		    }
		}
	    }
	    if ( s/^(.+)\n/$1/g ) {
		$PREP1OPT .= " $PARAM $1";
	    }
	}
    }
    else {
	print "ERROR: $INFILE not found.";
	exit -1;
    }
}

$PROG=$ARGV[0];shift;
if ($PROG =~ /(.*)\.(.*)/) {
    $PROG = $1;
    $PROG_EXT = $2;
}

if ($opt_o) {
    $PROT=$opt_o;
}
else {
    $PROT="$PROG.prf";
}


my($CMD)="PREP $PFLAGS $PREP1OPT $PROG";
System($CMD);
$CMD = "PROFILE $PFLAGS /I $PROG /O $PROG $PROG @ARGV";
System($CMD);
$CMD = "PREP $PFLAGS /M $PROG";
System($CMD);
$CMD = "PLIST $PFLAGS $PROG > $PROT";
System($CMD);

sub System
{
  my($CMD) = $_[0];
  print "$CMD\n" if ($opt_v);
  if ($opt_n) {
  }
  else {
      my($RC)=system($CMD);
      print "RC=$RC\n" if ($opt_v);
  }
}

__DATA__

 USAGE: pcprof [-ho:i:vn] file args...

           Profiles the given program an writes a profiling report to
           file.prf.

	   writes the output of profiling to program.prf

 OPTIONS:    -h         Prints this help.
             -o file    Overwrites the default output <file>.prf.
             -i file    Inputfile with [include]/[exclude] sections.
                        Each line in a section specifies a module/function/
                        lib to include or exclude from profiling.
                        Comments start with a '#' or ';'.'.

             -n         No execution
             -v         Verbose

 ARGUMENTS:
             file       Executable to profile.
             args       Arguments passed to file.



