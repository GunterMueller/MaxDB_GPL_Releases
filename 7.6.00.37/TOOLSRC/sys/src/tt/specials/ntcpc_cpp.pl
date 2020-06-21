#!/usr/bin/perl
#
# @(#)ntcpc++ 1999-01-14
#
# R. Rödling, SAP AG
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

package ntcpc;
use Env;
use Carp;
use File::Basename;
use File::Copy;

do "$TOOLVARS";
if ( $@ ) { warn "Error while executing toolvars:\n", "$@"; exit }
ToolTrace("ntcpc++ called with @ARGV\n");

$SOURCE = basename($ARGV[0], "\.cpp");

if ( ! -f "$DBROOT/bin/cpc.exe" ) { 
	warn "$DBROOT/bin/cpc.exe doesn't exist!\n";
	exit 2;
}

copy $ARGV[0], "$SOURCE.cpc" ;
if ( $COMPILE_DBROOT ne "" ) { $DBROOT = $COMPILE_DBROOT; $ENV{DBROOT} = $DBROOT }

if ( $COMPILE_GOSCRIPT ) {
	system("$TOOLSHELL $TOOL/bin/$COMPILE_GOSCRIPT") == 0
		or croak "Error while executing \"$SHELL $TOOL/bin/$COMPILE_GOSCRIPT\"\n".
		"message: $!\n";
}


$RET = system("cpc", "-H nocheck", "-s", "-S adabas", "-E cplus", "-c", $SOURCE);

if ( $RET == 0 ) { 
	unlink "$SOURCE.cpc" 
}
else {
	open(FILE_IN, "$SOURCE.pcl");
	print <FILE_IN>;
	close(FILE_IN);
}

__END__
