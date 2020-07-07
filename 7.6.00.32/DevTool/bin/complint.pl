#!/usr/bin/perl
# complint      *** internal script (called by vmake !)
#
# @(#)complint  	1998-03-02
#
# lint
#
# CCFLAGS       optional c compiler flags
# KEEPFLG       optional keep intermediate files flag
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

package complint;
use Env;
do "$TOOLVARS";
if ( $@ ) { print "$@"; exit }
ToolTrace("complint called with @ARGV\n");

$USAGE="usage: complint [lint-flags] source\n";

@CCFLAGS = @CCMODENORM;

while ($_ = $ARGV[0], /^[+-]/) {
    shift;
    last if /^--$/;

    if (/^-c$/)  {
        push @CCFLAGS, "-oo";
        push @CCFLAGS, "-u";
        next; }  # ignore

    if (/^-g$/) { next; }   # ignore

    push @CCFLAGS, $_; next;

} #while

if ( @ARGV != 1 ) { print $USAGE;  exit 1; }

$SOURCE=$ARGV[0];

$lintpgm = "$TOOL/pgm/lint-nt -b -i$TOOL/lib/lint/ -w2 std.lnt";
$cmd = "$lintpgm @CCFLAGS $SOURCE";
print "$cmd\n";
system ($cmd);

