#!/usr/bin/perl
# convert_idl      *** internal script
#
# @(#)ptc_glob  6.1.2  1995-09-20
#
# only UNIX: convert idl-Files to headerfiles
# --- GAR ---
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
ToolTrace("convert_idl called with @ARGV\n");

$IFILE=shift;
$OFILE=shift;
open(FILE_IN, "<$IFILE") || die "Can't open $IFILE: $!";
open(FILE_OUT,">$OFILE") || die "Can't open $OFILE: $!";

while(<FILE_IN>)
{
    s/^(.*|)\[(\s*|)(out|in)((,|\s)(.*))?]/$1\/\*$2$3$4\*\//;
    s/^\s*\[\s*$/\/\*\n/;
    s/^\s*\]\s*$/\*\/\n/;
    print FILE_OUT $_;
}
close(FILE_IN);
close(FILE_OUT);
