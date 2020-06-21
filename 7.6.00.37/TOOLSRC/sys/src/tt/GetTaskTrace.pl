#!/usr/bin/perl
# GetTaskTrace
#
# @(#)GetTaskTrace			2002-03-11
#
# extract one task from vtrace
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
use Getopt::Std;

my ($Usage, $TaskID, $InFileName, $OutFileName, $InTask, $LineNr, %PrgOpts);

$Usage = "GetTaskTrace [-n] <taskid> <tracefile>\n\n  -n ::= with line number";

if (!getopts('hn',\%PrgOpts) || $PrgOpts{h}) {
    print $Usage;
    exit 1;
}

if ( @ARGV != 2 ) {
    print $Usage;
    print "\n\ninvalid argument count: " . scalar @ARGV . "\n";
    exit 1
}

$TaskID = shift;
if ( $TaskID !~ /^(T|t)?(\d+)$/ ) {
    print $Usage;
    print "\n\ninvalid taskid: $TaskID\n";
    exit 1
}
$TaskID = $2;

$InFileName = shift;
if ( ! -f $InFileName ) {
    print $Usage;
    print "\n\ntracefile not found: $InFileName\n";
    exit 1
}
$OutFileName = basename ($InFileName) . ".T$TaskID";



open (INFILE, "<$InFileName") or die "can't open $InFileName[0]\n";
open (OUTFILE, ">$OutFileName") or die "can't open $OutFileName\n";

$InTask    = 1;
$LineNr    = 0;
$LineNrTxt = sprintf "%8d : ", $LineNr;
while (<INFILE>) {
    $LineNr++;
    if (/=+\s+T(\d+)\s+=+/i) {
        if ($1 == $TaskID) {
            $InTask = 1;
            $LineNrTxt = sprintf "%8d : ", $LineNr;
        } else {
            $InTask = 0;
        }
        next;
    } elsif (/=+\s+(log|data)_writer.*/i) {
            $InTask = 0;
            next;
    }
    $_ = $LineNrTxt . $_ if ( $PrgOpts{n} );
    print OUTFILE $_ if ( $InTask == 1 ); 
    $LineNrTxt = "           ";
}

close (INFILE);
close (OUTFILE);

print "\n\n$OutFileName generated\n";
