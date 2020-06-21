#!/usr/bin/perl
# @(#) sed_stack   6.2.8   1998-13-02   kern   admin
#
# sed fuer die Behandlung der Stackmessungen
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
use File::Copy;

$lFILE = $ARGV[0];
$lTEMP = "$TMP/stack.pp";

copy($lFILE, $lTEMP) || die "Can't copy $lFILE to $lTEMP: $!";

open(FILE_IN, "$lTEMP") || die "Can't open $lTEMP: $!";
open(FILE_OUT, ">$lFILE") || die "Can't open $lFILE: $!";
while(<FILE_IN>) {
	if ( /DIAGNOSE VTRACE DEFAULT OFF/ ) {
		$lines = 3; next;
	}
	if ( /SWITCH/ ) {
		$lines = 4; next;
	}
	if ( $lines ) {
		$lines--; next;
	}
	print FILE_OUT $_;
}

close(FILE_OUT); close(FILE_IN);
unlink $lTEMP;
