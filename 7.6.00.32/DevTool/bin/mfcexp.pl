#!/usr/bin/perl
# mfcexp        *** internal script (called by vmake !)
#
# @(#)mfcexp		1997-11-18
#
# module frame export section to c include file
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

package mfcexp;
use Env;
use File::Copy;
use File::Basename;
use Carp;
use ToolLib;

do "$TOOLVARS";
if ( $@ ) { print "$@"; exit }
ToolTrace("mfcexp called with @ARGV\n");

$USAGE = "usage: mfcexp [-<symbol>=<value>] [-k] source [includes]\n";

do "$TOOL/bin/mf_opts$TOOLEXT";
if ( $@ ) { print "$@"; exit }

if ( @ARGV < 1 ) { print $USAGE; exit 1 }

$SOURCE = shift;
my(@INCLUDES) = @ARGV;

if (( $LANG eq "r" ) || ($NOEXTENSION == 1) )  # CTS 1112700
{
	undef $EXT;
	$BASE = basename($SOURCE);
}
else {
    #  PTS 1102250
	($BASE, $DIR, $EXT) = fileparse ($SOURCE, ('\.h','\.hpp', '\.c', '\.cpp'));
	$EXT=".h" if ($EXT !~ /^(\.hpp|\.h|\.cpp|\.c)$/);
}



@CONCOMOPTS;

if ( $unpack ne "no" ) {
	system("$TOOL/pgm/mfspl $SOURCE $BASE.def $NULL $NULL") == 0
		or croak "Error while executing \"$TOOL/pgm/mfspl $SOURCE $BASE.def $NULL $NULL\"\n".
		"message: $!\n";
}
else { ToolLib::SrcCopy($SOURCE, "$BASE.def") || die "Can't copy $SOURCE to $BASE.def: $!\n" } # CTS 1108093

# &gar (includes + \n at the end of file, if necessary ->
# else compiler this not accepted) -> 28.08.98
open (FILE_OUT, "+>>$BASE.def");
seek (FILE_OUT, 0, 2);
seek (FILE_OUT,-1,1);
$last_c=getc(FILE_OUT);
if ($last_c ne "\n")
{
   seek (FILE_OUT, 0, 2);
   print FILE_OUT "\n" if($last_c ne "\n");
   ToolTrace("mfcexp: $BASE$EXT has a incomplete last line\n");
   ToolTrace("mfcexp: \"\\n\" append at $BASE$EXT\n");
}
close FILE_OUT;
# 28.08.98 <- &gar

if ( $concom eq "yes" ) {
    system("$TOOL/pgm/mfconcom", "-n", @CONCOMOPTS, "$BASE.def", "$BASE$EXT", $MACH, $KIND, $TRACE, $COMP, $OS, $OSSPEC) == 0
	or croak "Error while executing \"$TOOL/pgm/mfconcom -n @CONCOMOPTS $BASE.def $BASE$EXT $MACH $KIND $TRACE $COMP $OS $OSSPEC\"\n".
	"message: $!\n";
}
else { unlink "$BASE$EXT"; rename "$BASE.def", "$BASE$EXT" }

if ( $unpack eq "no" && $checkinc ne "no" )
{
    ToolLib::CheckIncludes(\@INCLUDES, "$BASE$EXT", $LAYER);
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

if ($INC_COMPILECHECK)
{
	print "Start compile check of a header file ...\n";
	my $check_filename = "${BASE}${EXT}";
	$check_filename =~ tr/\./_/;
	$check_filename .= "_compile_check.$INC_COMPILETYPE";
	open ( COMPFILE, ">$check_filename") or die "Can't create check file $check_filename \n";
	{
		print COMPFILE "#include \"${BASE}${EXT}\"\n";
	}
	close (COMPFILE);
    push @DEFINES,  "-DSDB_INCLUDE_COMPILE_CHECK";
	@ARGV = (@KEEPFLG, @DEFINES, @ADDITIONAL_COMPILE_FLAGS, "-nowhatstring", "$check_filename");
    do "$TOOL/bin/compc$TOOLEXT";
    if ( $@ )
    {  	die "Compile errors for dependency check:\n $@\n";   }
    elsif ( @KEEPFLG == 0 )
    { unlink "$check_filename" }
}


if ( @KEEPFLG == 0 ) { unlink "$BASE.def" }

