#!/usr/bin/perl
# mfc           *** internal script (called by vmake !)
#
# @(#)mfc		1997-11-18
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

package mfc;
use Env;
use File::Basename;
use File::Copy;
use Carp;
use ToolLib;

do "$TOOLVARS";
if ( $@ ) { print "$@"; exit }
ToolTrace("mfc called with @ARGV\n");

$USAGE  = "usage: $0 [-f|-q|-s] [-trace=<procedure>] [-check=no]";
$USAGE .= " [-<symbol>=<value>] [-k] source [includes]\n";

do "$TOOL/bin/mf_opts$TOOLEXT";
if ( $@ ) { print "$@"; exit }

if ( @ARGV < 1 ) { print $USAGE; exit 1 }

$SOURCE = shift;
($BASE, $SOURCEPATH, $EXT) = fileparse($SOURCE, '\..*');
if ( $EXT eq "" ) {
    if ($LANG eq "c" ) { $EXT = ".c" }
    else { $EXT = ".cpp" }
}

my(@INCLUDES) = @ARGV;

if ( $unpack ne "no" ) {
	# needed for debugging &gar with original source
	ToolLib::SrcCopy("$SOURCE", "$BASE.dbg"); # CTS 1108093
	# unlink("$BASE.dbg"); copy("$SOURCE", "$BASE.dbg");
	if ( $CURR_VERSION =~ /^s/i || $CURR_VERSION =~ /^q/i ) {
		system("$TOOL/pgm/mfcline $WRK/$CURR_VERSION/debug/$BASE$EXT < $SOURCE > $BASE.wln 2>$BASE.err") == 0
		   or croak "Error while executing \"$TOOL/pgm/mfcline $WRK/$VMAKE_VERSION/debug/$BASE$EXT < $SOURCE > $BASE.wln\"\n".
		   "message: $!\n";
	}
	else {
		system("$TOOL/pgm/mfcline $BASE$EXT < $SOURCE > $BASE.wln 2>$BASE.err") == 0
		   or croak "Error while executing \"$TOOL/pgm/mfcline $BASE$EXT < $SOURCE > $BASE.wln\"\n".
		   "message: $!\n";
	}

	system("$TOOL/pgm/mfspl $BASE.wln $BASE.def $NULL $BASE.cod") == 0
	   or croak "Error while executing \"$TOOL/pgm/mfspl $BASE.wln $BASE.def $NULL $BASE.cod\"\n".
	   "message: $!\n";

	if ( $COMP eq "XTP" ) {
		# SINIX Pascal-XT compiler expects 2 files:
		#   $BASE.spec (created here, removed in compc)
		#   $BASE.body (compc will create it by copying $BASE$EXT)

		open(FILE_OUT, "+>$BASE.spec") or croak "Can't open $BASE.spec (output): $!\n";
		print FILE_OUT "PACKAGE $BASE;\n";
		print FILE_OUT "@INCLUDES", "\n";
		open(FILE_IN, "<$BASE.def") or croak "Can't open $BASE.def (input): $!\n";
		while(<FILE_IN>) {
		   s/PROCEDURE/ENTRY PROCEDURE/g;
		   print FILE_OUT ;
		}
		close FILE_IN;
		print FILE_OUT "END.\n";
		close FILE_OUT;
		open(FILE_OUT, "+>$BASE.cuc") or croak "Can't open $BASE.cuc (output): $!\n";
		open(FILE_IN, "<$BASE.cod") or croak "Can't open $BASE.cod (input): $!\n";
		print FILE_OUT "PACKAGE BODY $BASE;\n";
		print FILE_OUT <FILE_IN>;
		close FILE_IN;
		print FILE_OUT "BEGIN\n";
		print FILE_OUT "END.\n";
		close FILE_OUT;
	}
	else {
		open(FILE_OUT, "+>$BASE.cuc") or croak "Can't open $BASE.cuc (output): $!\n";
		if ( $KIND eq "SLOW" ) {
		   print FILE_OUT "#define DEBUG 1\n";
		}
		else {
			print FILE_OUT "\n";
		}
		foreach $I (@INCLUDES) {
			if ( -f $I ) {
				$I =~ s!\\!/!g;
				print FILE_OUT "#line 1 \"$I\"\n";
				open(FILE_IN, "<$I") or croak "Can't open $I (input): $!\n";
				print FILE_OUT <FILE_IN>;
				close FILE_IN;

			}
		}
		open(FILE_IN, "<$BASE.cod") or croak "Can't open $BASE.cod (input): $!\n";
		print FILE_OUT <FILE_IN>;
		close FILE_IN;
		close FILE_OUT;
	}
} # unpack
else
{
	if ($DEBUG == 1) # &gar PTS 1002135
	{
		if ( $concom ne "no" )
		{
			system("$TOOL/pgm/mfconcom", "-n", @CONCOMOPTS, "$SOURCE", "$BASE.dbg", $MACH, $KIND, $TRACE, $COMP, $OS, $OSSPEC) == 0
			   or croak "Error while executing \"$TOOL/pgm/mfconcom -n @CONCOMOPTS $BASE.cuc $BASE$EXT $MACH $KIND $TRACE $COMP $OS $OSSPEC\"\n".
			   "message: $!\n";
		}
		else
		{
			ToolLib::SrcCopy($SOURCE, "$BASE.dbg") || die "Can't copy $SOURCE to $BASE.dbg: $!\n";
		}
    }
	ToolLib::SrcCopy($SOURCE, "$BASE.cuc") || die "Can't copy $SOURCE to $BASE.cuc: $!\n"; # CTS 1108093
}

@CONCOMOPTS;
if ( $concom ne "no" )
{
system("$TOOL/pgm/mfconcom", "-n", @CONCOMOPTS, "$BASE.cuc", "$BASE$EXT", $MACH, $KIND, $TRACE, $COMP, $OS, $OSSPEC) == 0
   or croak "Error while executing \"$TOOL/pgm/mfconcom -n @CONCOMOPTS $BASE.cuc $BASE$EXT $MACH $KIND $TRACE $COMP $OS $OSSPEC\"\n".
   "message: $!\n";
}
else
{
	unlink "$BASE$EXT"; rename "$BASE.cuc", "$BASE$EXT";
}

# 'MOD_DAT' is exported by 'vmake' !!
open(FILE_OUT, "+>>$BASE$EXT") or croak "Can't open $BASE$EXT (append output): $!\n";
print FILE_OUT "\n$SR_ID \"@(#)$MOD_DAT\"$ID_END\n" unless ($NOWHATSTRING);


$tmpBASE = $BASE; $tmpBASE =~ s/-/_/g; $tmpBASE =~ s/\+/_/g;
print FILE_OUT "extern char *${tmpBASE}_force_data () { return( ident_sr ); }\n" unless ($NOWHATSTRING);
# This function forces a reference into the data section, so the AIX, HPUX
# compiler cannot ignore that section, so we ensure that the 'what' strings
# become part of the object.
# CAUTION: This works only for the "static char" type of 'what' string!

splice(@CONCOMOPTS, 0, 0, $KIND, $MACH, $OS, $OSSPEC, $COMP);

unless ($NOWHATSTRING)
{
	print FILE_OUT "$MF_ID \"@(#)$BASE @CONCOMOPTS\"$ID_END\n";
}

close FILE_OUT;

if ( $unpack eq "no" and $LANG =~ /c|x/ and $checkinc ne "no" ) {
    ToolLib::CheckIncludes( \@INCLUDES, "$BASE$EXT", $LAYER );
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

#if ($DEBUG == 1)
#{
#	ToolLib::SrcCopy("$BASE$EXT", "$BASE.dbg") || die "Can't copy $BASE$EXT to $BASE.dbg: $!\n";
#}

if ( @KEEPFLG == 0 ) {
    unlink "$BASE.cod", "$BASE.cuc", "$BASE.wln", "$BASE.err";
    unlink "$BASE.def";
}

