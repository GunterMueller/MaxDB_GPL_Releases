#!/usr/bin/perl
#
# @(#)sut   1997-11-18
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
use Cwd;
use Getopt::Std;
require "$TOOL/bin/sutvars$TOOLEXT";

my $usage = "usage: sut [-n <node> -R <remote root>] fast|quick|slow teststream [dbname]\n";

if ( !getopts('n:R:') ) {
  print $usage;
  exit
}

if ( @ARGV <= 1) {
  print $usage;
  exit
}


$SPEED = shift;
$stream = shift;
$stream =~ tr/[a-z]/[A-Z]/;
if ( $stream =~ /[*?]/) {
	print "usage: sut teststream \n";
	warn "Zeichen *, ? sind in 'teststream' nicht erlaubt!\n";
	die "'teststream' wird automatisch zu 'teststream*' erweitert!\n"
}
$serverdb = shift;
$SERVERDB = $serverdb if ( $serverdb );

$PROG = "slowci$PROG_EXT";

chdir($SUT) || die "Can't change to directory $SUT: $!";

opendir(DIR_IN, $SUT);
if ( $stream !~ /.*\.$/ ) {
	@Streams = grep /^$stream.*\.vdnts/i, readdir(DIR_IN);
} else {
	chop $stream;
	@Streams = grep /^$stream\.vdnts$/i, readdir(DIR_IN);
}
closedir DIR_IN;

if ( @Streams == 0 ) { die "Die Teststroeme existieren nicht!\n";}

@ARGV = ($SPEED, $PROG, $SERVERDB, @Streams);
@ARGV = ("-R $opt_R", @ARGV) if ( $opt_R );
@ARGV = ("-n $opt_n", @ARGV) if ( $opt_n );
do "$TOOL/bin/sutsingle$TOOLEXT";
if ( $@ ) { die "Error while executing sutsingle:\n", "$@"; exit }
