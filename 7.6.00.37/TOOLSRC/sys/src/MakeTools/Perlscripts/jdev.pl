#!/usr/bin/perl
#
#       @(#)eclipse     Version:6.2.8           22-04-2005
#
#       Martin Reinecke, SAP AG
#
# usage: eclipse [-w workspace] 
#	-w workspace	: name of the warkspace
#
# open eclipse ide with the given workspace
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

package Eclipse;
use Env;
use Getopt::Std;

$Usage = "usage: jdev [-h] [-w workspace]\n";

getopts('hw:');

if ( $opt_h ) {
	print <DATA>;
	exit 0;
}

if ( !defined($opt_w) ) {
	$workspace = $ECLIPSE_WORKSPACE;
} else { $workspace = $opt_w }

print ("Start new Eclispe IDE ...\n");
print ("... $workspace\n");
system("start \"Eclipse\" \"$ECLIPSE_HOME\\eclipse.exe\" -data $workspace");

exit;

__DATA__

usage: jdev [-h] [-w workspace]

options:

 -w workspace  : use the workspace 

