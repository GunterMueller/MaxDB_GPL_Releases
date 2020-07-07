#!/usr/bin/perl
# mf00ccc       *** internal script (called by vmake !)
#
# @(#)mf00ccc	1997-11-18
#
# create a v..00ccc module from a v..00 pascal include module
#
# TOOL          directory for tool scripts
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

package mf00ccc;
use Env;
use File::Copy;
use File::Basename;
use Carp;
do "$TOOLVARS";
if ( $@ ) { print "$@"; exit }
ToolTrace("mf00ccc called with @ARGV\n");

$USAGE = "usage: mf00ccc [-<symbol>=<value>] [-k] vxx00.con vxx00.typ [vyy00.con ...]\n";

do "$TOOL/bin/mf_opts$TOOLEXT";
if ( $@ ) { print "$@"; exit }

if ( @ARGV < 2 ) { print $USAGE; exit 1 }

$CON = shift;
$TYP = shift;
$BASE = basename $CON;
($BASE) = $BASE =~ /(\.*[^\.]*)/ ;  # alle Endungen werden abgeschnitten

$INCS = "" if !defined($INCS);
foreach $ARG (@ARGV) {

    if ( $INCS ne "" ) { $INCS .= ",$ARG" }
    else  { $INCS = "-I $ARG" }

}

if ($RELVER ge "R71" and $RELVER ne "TOOL" ) {
	if ($ENV{SAPDB_INTERNAL})
	{	$ptoc00="$PYTHON/python $TOOL/bin/ptoc00.py"; }
	else
	{	$ptoc00="python $TOOL/bin/ptoc00.py"; }

	$ptoc00.=" -n" if ( $VMAKE_OPTION !~ /I/ ); # CTS 1106486
	unlink "$BASE.cpp"; # CTS 1106486
    if (system("$ptoc00 -i -o $BASE.h $INCS $CON $TYP") == 0) {
        if ( $concom ne "no" ) {
         unlink "$BASE.tmp";
         rename("$BASE.h", "$BASE.tmp") || die "Can't rename $BASE.h to $BASE.tmp: $!\n";
         system("$TOOL/pgm/mfconcom", "-n", @CONCOMOPTS, "$BASE.tmp", "$BASE.h", $MACH, $KIND, $TRACE, $COMP, $OS, $OSSPEC) == 0
           or croak "Error while executing \"$TOOL/pgm/mfconcom -n @CONCOMOPTS $BASE.cod $BASE.h $MACH $KIND $TRACE $COMP $OS $OSSPEC\"\n".
           "message: $!\n";
        }
        # compile
        if (-r "$BASE.cpp") {
            @ARGV = (@KEEPFLG, "$BASE.cpp");
            do "$TOOL/bin/compc$TOOLEXT";
            if ( !$@ ) {
                #&if $OS in [ WIN32 ]
                $OUT = "$BASE.exe";
                #        &else
                #        $OUT = "$BASE.out";
                #        &endif
				@ARGV =();
				push @ARGV, "-tooloption=CXX";  # PTS 1102351
                push @ARGV ,"-BuildNrFunction=", "-no_symbol_files", "-nowhatstring", @KEEPFLG, "-o", $OUT, "$BASE.o";
                do "$TOOL/bin/linklnk$TOOLEXT";
                if ( $@ ) { warn "Error while executing linklnk:\n", "$@"; exit }
                #
                # execute to generate .siz file
                #
                $result = "$BASE.siz";
                $cmd = "$OUT $MACH > $result";
                (system ($cmd) == 0)
                    or croak "Error while executing \"$cmd\"\n" .
                        "message: $!\n";
            }
            else {
                warn "Compile errors for sizeof-check ignored!\n";
            }
            # remove temporary files
            if ( @KEEPFLG == 0 ) {
                unlink ("$BASE.cpp", "$BASE.o", "$BASE.tmp", $OUT);
            }
        }
    }
    else {
        die "mf00ccc: p-to-c-00 error!\n error during execute: $ptoc00 -i -o $BASE.h $INCS $CON $TYP\n";
    }

}
else {
	unlink("$BASE.h");
	system_touch ("$BASE.h");
}

sub system_touch {
	my @CMD = ("$TOOL/Posix/touch", $_[0]);
	return 0 if (system (@CMD) == 0);
	die ("error while system ".join (" ", @CMD).":\n$!\n ");
}

__END__

Hier können lange Kommentare stehen
