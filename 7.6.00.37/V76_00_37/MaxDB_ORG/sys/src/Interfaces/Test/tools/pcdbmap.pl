#!/usr/bin/perl
#
# @(#)pcdbmap   1997-11-28
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2001-2005 SAP AG
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

use Env;
$USAGE ="USAGE: pcdbmap filename\n";
die $USAGE if (@ARGV < 1);

local ($XX) = "";
$XX="$XX SERVERDB" if (length $SERVERDB == 0);
$XX="$XX SUPERUID" if (length $SUPERUID == 0);
$XX="$XX SUPERPWD" if (length $SUPERPWD == 0);
die "Env $XX not set.\n" if ( length $XX );

$mapfile=$ARGV[0];

open(FILE_IN, "$mapfile") || die "Error: '$mapfile' not found.";
open(FILE_OUT, ">$TESTDIR/wrk/$$.h") || die "Error can't open tempfile.";
while(<FILE_IN>) {
# #define SERVERDB "$SERVERDB"
    s/^(#\s*define\s+SERVERDB\s+)"(.*)"/$1"$SERVERDB"/;
    s/^(#\s*define\s+SERVERNODE\s+)"(.*)"/$1"$SERVERNODE"/;
    s/^(#\s*define\s+SUPERUID\s+)"(.*)"/$1"$SUPERUID"/;
    s/^(#\s*define\s+SUPERPWD\s+)"(.*)"/$1"$SUPERPWD"/;
# EXEC SQL n SET SERVERDB '$SERVERDB' ON '$SERVERNODE';
    s/(^.*\/\*PCDBMAP\*\/.*SET\s+SERVERDB\s+').*('\s+ON.*$)/$`$1$SERVERDB$2/;
    s/(^.*\/\*PCDBMAP\*\/.*SET\s+SERVERDB\s+.*\s+ON\s+').*('.*$)/$`$1$SERVERNODE$2/;
    print FILE_OUT ;
}
close(FILE_IN);
close(FILE_OUT);
unlink "$mapfile";
system("cp $TESTDIR/wrk/$$.h $mapfile");
unlink "$TESTDIR/wrk/$$.h";
