#!/usr/bin/perl
# pcclear
#
# @(#)pcclear  6.2  1998-01-09
#
# Removes most of temporary used files.
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

package pcclear;
use Env;
use Getopt::Std;
use ICopy;
if ( chdir "$TMP" ) {
    print "removing $TMP *.*\n";
    unlink <*>;
}

@dirs = ("$DBROOT/pgm", "$DBROOT/bin", "$DBROOT/lib", "$DBROOT/symbols", "$DBROOT/Support/Mapfiles", "$DBROOT/WebDB/cgi-bin");
foreach $dir (@dirs) {
    if ( chdir "$dir" ) {
	print "removing $dir *.*.[qfs]\n";
	unlink <*.*.f>;
	unlink <*.*.q>;
	unlink <*.*.s>;
	print "removing $dir _*.exe\n";
	unlink <_*.exe>;
	print "removing $dir _*.dll\n";
	unlink <_*.dll>;
    }
}

@dirs = ("$WRK/fast", "$WRK/quick", "$WRK/slow");
foreach $dir (@dirs) {
    @edir = ( "tmp", "csrc" );
    foreach $edir (@edir) {
	if ( chdir "$dir/$edir" ) {
	    print "removing $dir/$edir *\n";
	    unlink <*>;
	}
    }
    if ( chdir "$dir/debug" ) {
	print "removing $dir/debug *.p\n";
	unlink <*.p>;
    }
}
