#!/usr/bin/perl
# mfainc        *** internal script (called by vmake !)
#
# @(#)mfainc		1997-11-18
#
# module frame to assembler include file
#
# TOOL          directory for tools
# KEEPFLG       optional keep intermediate files flag
#
#
# includes currently unused but syntactically allowed
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

package mfainc;
use Env;
use File::Basename;
use Carp;
do "$TOOLVARS";
if ( $@ ) { print "$@"; exit }
ToolTrace("mfainc called with @ARGV\n");

$USAGE = "usage: mfainc [-<symbol>=<value>] [-k] source [includes]\n";

do "$TOOL/bin/mf_opts$TOOLEXT";
if ( $@ ) { print "$@"; exit }

if ( @ARGV < 1 ) { print $USAGE; exit 1 }

$SOURCE = shift;
$BASE = basename($SOURCE);

@CONCOMOPTS;

system("$TOOL/pgm/mfspl $SOURCE $NULL $NULL $BASE.cod") == 0
	or croak "Error while executing \"$TOOL/pgm/mfspl $SOURCE $NULL $NULL $BASE.cod\"\n".
	"message: $!\n";

if ( $concom eq "yes" ) {
    system("$TOOL/pgm/mfconcom", @CONCOMOPTS, "$BASE.cod", "$BASE.h", $MACH, $KIND, $TRACE, $COMP, $OS, $OSSPEC) == 0
	or croak "Error while executing \"$TOOL/pgm/mfconcom @CONCOMOPTS $BASE.cod $BASE.h $MACH $KIND $TRACE $COMP $OS $OSSPEC\"\n".
	"message: $!\n";
}
else { unlink "$BASE.h"; rename "$BASE.cod", "$BASE.h" }

if ($SETBUILDINFO)
{
	require Buildinfo;
	import Buildinfo;
	Buildinfo::setbuildinfo("$BASE.h");
}

if ($SCRIPT ne "")
{
	require SDB::Make::ExecScript;
	import SDB::Make::ExecScript;

	SDB::Make::ExecScript::ExecScript("$SCRIPT", "$BASE.h");
}

if ( @KEEPFLG == 0 ) { unlink $BASE.cod }

__END__

Hier können lange Kommentare stehen
