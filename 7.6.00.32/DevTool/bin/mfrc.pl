#!/usr/bin/perl
# mfc           *** internal script (called by vmake !)
#
# @(#)mfrc		1997-11-18
#
# module frame to c source
#
# TOOL          directory for tools
# KEEPFLG       optional keep intermediate files flag
# VMAKE_VERSION optional module version
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

package mfrc;
use Env;
use Carp;
use File::Basename;
use ToolLib; # CTS 1108093
use File::Copy;

do "$TOOLVARS";
if ( $@ ) { print "$@"; exit }
ToolTrace("mfrc called with @ARGV\n");

$USAGE  = "usage: $0 [-f|-q|-s] source\n";

do "$TOOL/bin/mf_opts$TOOLEXT";
if ( $@ ) { print "$@"; exit }

if ( @ARGV < 1 ) { print $USAGE; exit 1 }

$SOURCE = shift;
# well known extensions rc, ico, dlg, def, mc, idl, lex, ycc
($BASE, $path,$EXT) = fileparse($SOURCE, '\..*');

my(@INCLUDES) = @ARGV;

@CONCOMOPTS;

if ( $unpack ne "no" ) {
	system("$TOOL/pgm/mfspl $SOURCE $NULL $NULL $BASE.cod") == 0
	   or croak "Error while executing \"$TOOL/pgm/mfspl $SOURCE $NULL $NULL $BASE.cod\"\n".
	   "message: $!\n";
}
else { ToolLib::SrcCopy($SOURCE, "$BASE.cod") || die "Can't copy $SOURCE to $BASE.cod: $!\n"} # CTS 1108093


if ( $concom ne "no" ) {
system("$TOOL/pgm/mfconcom", "-n", @CONCOMOPTS, "$BASE.cod", "$BASE$EXT", $MACH, $KIND, $TRACE, $COMP, $OS, $OSSPEC) == 0
   or die "Error while executing \"$TOOL/pgm/mfconcom -n @CONCOMOPTS $BASE.cod $BASE$EXT $MACH $KIND $TRACE $COMP $OS $OSSPEC\"\n".
   "message: $!\n";
}
else
{
    unlink "$BASE$EXT";
    rename "$BASE.cod", "$BASE$EXT" ;
}

if ($SETBUILDINFO)
{
	require Buildinfo;
	import Buildinfo;
	Buildinfo::setbuildinfo("$BASE$EXT");
}

if ($SCRIPT ne "")
{
	require SDB::Make::ExecScript;
	import SDB::Make::ExecScript;

	SDB::Make::ExecScript::ExecScript("$SCRIPT", "$BASE$EXT");
}

if ( @KEEPFLG == 0 ) {
    unlink "$BASE.cod";
}

__END__

Hier k�nnen lange Kommentare stehen
