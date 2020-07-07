#!/usr/bin/perl
# mfa           *** internal script (called by vmake !)
#
# @(#)mfa		1997-11-18
#
# module frame to assembler source
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

package mfa;
use Env;
use File::Basename;
use File::Copy;
use ToolLib; # CTS 1108093
use Carp;
do "$TOOLVARS";
if ( $@ ) { print "$@"; exit }
ToolTrace("mfa called with @ARGV\n");

$USAGE  = "usage: mfa [-f|-q|-s] [-trace=<procedure>] [-check=no]";
$USAGE .= " [-<symbol>=<value>] [-k] source [includes]\n";

do "$TOOL/bin/mf_opts$TOOLEXT";
if ( $@ ) { print "$@"; exit }

if ( @ARGV < 1 ) { print $USAGE; exit 1 }

$SOURCE = shift;
$BASE = basename($SOURCE,(".a",".s")); #PTS 1106799

my(@INCLUDES) = @ARGV;

# PTS 1106799
if ($unpack ne "no")
{
system("$TOOL/pgm/mfspl $SOURCE $NULL $NULL $BASE.cod") == 0
	or croak "Error while executing \"$TOOL/pgm/mfspl $SOURCE $NULL $NULL $BASE.cod\"\n".
	"message: $!\n";
}
else
{
	ToolLib::SrcCopy($SOURCE, "$BASE.cod") || die "Can't copy $SOURCE to $BASE.cod: $!\n" # CTS 1108093
}

open(FILE_OUT, "+>$BASE.suc") or croak "Can't open $BASE.suc (output): $!\n";
push @INCLUDES, "$BASE.cod";
foreach $I (@INCLUDES) {
	if ( -f $I ) {
		open(FILE_IN, "<$I") or croak "Can't open $I (input): $!\n";
		while (<FILE_IN>) { print FILE_OUT }
		close FILE_IN;
	}
}
close FILE_OUT;

@CONCOMOPTS;

system("$TOOL/pgm/mfconcom", @CONCOMOPTS, "$BASE.suc", "$BASE.s", $MACH, $KIND, $TRACE, $COMP, $OS, $OSSPEC) == 0
	or croak "Error while executing \"$TOOL/pgm/mfconcom @CONCOMOPTS $BASE.suc $BASE.s $MACH $KIND $TRACE $COMP $OS $OSSPEC\"\n".
	"message: $!\n";

if ($SETBUILDINFO)
{
	require Buildinfo;
	import Buildinfo;
	Buildinfo::setbuildinfo("$BASE.s");
}

if ($SCRIPT ne "")
{
	require SDB::Make::ExecScript;
	import SDB::Make::ExecScript;

	SDB::Make::ExecScript::ExecScript("$SCRIPT", "$BASE.s");
}

if ( @KEEPFLG == 0 ) { unlink "$BASE.cod", "$BASE.suc" }


__END__

Hier können lange Kommentare stehen
