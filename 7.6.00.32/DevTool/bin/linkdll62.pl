#!/usr/bin/perl
# linkdll62       *** internal script (called by vmake !)
#
#
# @(#)linkdll62		1999-06-02
#
# link object modules to create a shared library
#
# TOOL          directory for tools
# LDFLAGS       optional loader flags
# KEEPFLG       optional keep intermediate files flag
#
# 1994-06-02 J.Bruehe  modify for AIX import lists
#
# 1995-08-01	R. Appelt	Einbau des "-deflib"-Parameters
# 1997-02-04 GG: Map-Files werden nach $SYS/map/$VMAKE_VERSION verschoben
# 1998-09-09 &gar: MAP-Files moved to $DBROOT/Support/Mapfiles -> 2 files:
#                  normal (xxx.map) and one with second extension <VMAKE_EXT>
# 1999-06-02 &gar: rename old linkdll.pl to link62.dll (only for UNIX)
#                  and call it in linkdll.pl if RELVER<72
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


package linkdll;
use Env;
use File::Copy;
use ICopy;
use ToolLib;

# Get LDFLAGS, AR
@LIBMATH = ();
do "$TOOLVARS";
if ( $@ ) { print "$@"; exit }
ToolTrace("linkdll62 called with @ARGV\n");

$USAGE = "usage: linkdll62 [ld-flags] dll archive objects\n";

$MAP    = "";
$DEF    = "";
$NODLL  = "";
$DLLFLG = "";
$ENT    = "";
@Lib    = ();
@ELIB   = ();
$DEFLIB = "-nodefaultlib";
$DEBUG  = "";
$PROF   = "";
$SCRIPT = "";
$RCFILE = "";
@LIBPC;
local @LDCMD=();
@SLIBS=();
@OFLAGS=();

do "$TOOL/bin/link_opts$TOOLEXT";
if ( $@ ) { print "$@"; exit }
@IMPLIST = ();
push @LDFLAGS,@OFLAGS;

while ($_ = $ARGV[0], /^-/) {
    shift;
    last if /^--$/;

    if ( /^-linkoption=(.+)$/ ) { push @LDFLAGS, $1; next }

    if ( /^-tooloption=(.+)$/ ) { next }


    if(/^-.*/)     { push @LDFLAGS, $_; next }

} #while

if ( @ARGV < 3 ) { print $USAGE; exit 1 }

$DLLFILE = shift;


#some systems forbid some extensions for executables
if ($DLLFILE =~ /(.*)\.[fqs]/) { $DLL = $1} else { $DLL = $DLLFILE }

$ARCFILE = shift;

    @OBJECTS = ();
    @SLIBS = ();
    foreach $F (@ARGV) {
		if ( $F =~ /-def:(.*)/ ) { $DEF = $1 ; next; }
        if ( $F =~ /.*\.o/ ) { push @OBJECTS, $F }
		else {
			if ($F =~ /(.*)\.[fqs]/) { $F = $1; }
			push(@SLIBS, $F) if $F !~ /.*\.def/;
		}
    }

push @OBJECTS, ToolLib::CreateBuildNrModule(@KEEPFLG,$DEBUG?("-g"):());



#
#  Create the static library
#
@ARFLAGS;
ToolTrace("$AR called with: @ARFLAGS $ARCFILE @OBJECTS\n");
print ("$AR @ARFLAGS $ARCFILE @OBJECTS\n") if ($ENV{NOQUIET});
system($AR, @ARFLAGS, $ARCFILE, @OBJECTS) == 0
   or die "Error while executing $AR @ARFLAGS $ARCFILE @OBJECTS\n".
   "message: $!\n";

#
#  Link the dynamic library
#

if ( $NODLL eq "" ) {
	$LIBPATH = "-L$TOOL/lib";

	# shared library <libname>.so
    @LDCMD=("ld", "-shared", "-o", $DLL, $LIBPATH, @OBJECTS);
	system(@LDCMD) == 0
	   or die "Error while executing \"@LDCMD\"\n".
	   "message: $!\n";

}
else {
   # don't create shared library
   # vmake needs a dummy
	if ( -f $DLL ) { my($now)=time; utime($now, $now, $DLL)}
	else {
		open(FILE_OUT, ">>$DLL") or die "Can't open $DLL (touch): $!\n";
		close FILE_OUT;
	}
}


if ( @KEEPFLG == 0 ) {

   unlink "$DLLBASE.res";
   unlink "$DLLBASE.rbj";
   # unlink "$DEF";
    if ( -f $PROG && -f "$BASE.map" ) {
      mkdir ("$OWN/usr/Support", 0775) if ( ! -d "$OWN/usr/Support") ;
	  mkdir ("$OWN/usr/Support/Mapfiles", 0775) if ( ! -d "$OWN/usr/Support/Mapfiles") ;
	  copy "$BASE.map", "$OWN/usr/Support/Mapfiles/$BASE.map.$VMAKE_EXT"
	  || warn "Can't copy $BASE.map to $OWN/usr/Support/Mapfiles/$BASE.map.$VMAKE_EXT: $!\n";
	  unlink "$OWN/usr/Support/Mapfiles/$BASE.map";
      rename "$BASE.map", "$OWN/usr/Support/Mapfiles/$BASE.map";
   }

   if ( $NODLL eq "" && -f $DLL && -f "$ARCBASE.map" ) {

      unlink "$ARCBASE.tmp", "$ARCBASE.exp";
      mkdir ("$OWN/usr/Support", 0775) if ( ! -d "$OWN/usr/Support") ;
      mkdir ("$OWN/usr/Support/Mapfiles", 0775) if ( ! -d "$OWN/usr/Support/Mapfiles") ;
      copy "$ARCBASE.map", "$OWN/usr/Support/Mapfiles/$ARCBASE.map.$VMAKE_EXT"
	  || warn "Can't copy $ARCBASE.map to $OWN/usr/Support/Mapfiles/$ARCBASE.map.$VMAKE_EXT: $!\n";
	  unlink "$OWN/usr/Support/Mapfiles/$ARCBASE.map";
	  rename "$ARCBASE.map", "$OWN/usr/Support/Mapfiles/$ARCBASE.map";
   }
   else {
	unlink "$ARCBASE.tmp", "$ARCBASE.exp", "$ARCBASE.map";
   }

}

if ( $DLLFILE ne $DLL && -f $DLL ) { unlink $DLLFILE; rename $DLL, $DLLFILE }
