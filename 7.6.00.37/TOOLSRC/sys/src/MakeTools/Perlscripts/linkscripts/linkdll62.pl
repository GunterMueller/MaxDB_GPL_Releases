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

&if $OSSPEC in [ WIN32 ]
    if(/^-dll$/)     { next }     # ignored 

    if(/^-nodll$/)   { $NODLL = $_; next }

    if(/^-def=(.*)/) { $DEF = "$1.def"; next }

    if(/^-deflib=(.*)/) {
        $DEFLIB   = "-defaultlib:$1.lib";
        @LIBMPROC =	();
		next; }

    if(/^-elib=(.*)/) { push @ELIB, "$1.lib"; next }

    if(/^-usedef$/)  { next }    # unused

    if(/^-e=(.*)/)   { $ENT = "-entry:$1"; next }

    if(/^-exp=.*/) {
        #additional export, not implemented.
        print "linkdll: warning: Parameter not implemented: $_\n";
		next; }
&elif $OSSPEC in [ AIX ]
    # We have to separate the arguments into objects and import lists
    # because 1) only objects are given to "nm" for the export list
    # and     2) import lists must be searched in a hierarchy until found.
    if ( /^-bI:(.*)/ ) 
	{
		$IMPMOD = $1;
        if  ( -f "$WRK/fast/obj/$IMPMOD" ) {push @IMPLIST , "-bI:$WRK/fast/obj/$IMPMOD"; }
        elsif ( -f "$WRK/quick/obj/$IMPMOD" ) {push @IMPLIST, "-bI:$WRK/quick/obj/$IMPMOD";}
        elsif ( -f "$WRK/slow/obj/$IMPMOD" ) { push @IMPLIST, "-bI:$WRK/slow/obj/$IMPMOD";}
        elsif ( -f "$TOOL/lib/$IMPMOD" ) { push @IMPLIST, "-bI:$TOOL/lib/$IMPMOD";}
        else { 	push @IMPLIST, $F; 	}
	}
&endif 

    if(/^-.*/)     { push @LDFLAGS, $_; next }

} #while

if ( @ARGV < 3 ) { print $USAGE; exit 1 }

$DLLFILE = shift;

&if $OSSPEC in [ WIN32 ]
if (!$DEBUG)
    { $STRIPDEBUG = "y" }
&endif

#some systems forbid some extensions for executables
if ($DLLFILE =~ /(.*)\.[fqs]/) { $DLL = $1} else { $DLL = $DLLFILE }
&if $OSSPEC in [ WIN32 ]
($DLLBASE) = $DLLFILE =~ /(\.*[^\.]*)/ ;  # alle Endungen werden abgeschnitten 
&endif WIN32

$ARCFILE = shift;
&if $OSSPEC in [ WIN32 ]
if ($ARCFILE =~ /(.*)\.[fqs]/) { $ARC = $1 } else { $ARC = $ARCFILE }
($ARCBASE) = $ARCFILE =~ /(\.*[^\.]*)/ ;  # alle Endungen werden abgeschnitten 
&endif WIN32

&if $OSSPEC in [ AIX ]
    # We have to separate the arguments into objects and import lists
    # because 1) only objects are given to "nm" for the export list
    # and     2) import lists must be searched in a hierarchy until found.
    @OBJECTS = ();
    foreach $F (@ARGV) {
		if ( $F =~ /-bI:(.*)/ ) {
			$IMPMOD = $1;
            if  ( -f "$WRK/fast/obj/$IMPMOD" ) {
				push @IMPLIST , "-bI:$WRK/fast/obj/$IMPMOD";
			}
            elsif ( -f "$WRK/quick/obj/$IMPMOD" ) {
				push @IMPLIST, "-bI:$WRK/quick/obj/$IMPMOD";
			}
            elsif ( -f "$WRK/slow/obj/$IMPMOD" ) {
				push @IMPLIST, "-bI:$WRK/slow/obj/$IMPMOD";
			}
            elsif ( -f "$TOOL/lib/$IMPMOD" ) {
				push @IMPLIST, "-bI:$TOOL/lib/$IMPMOD";
			}
            else { 
				push @IMPLIST, $F; 
			}
		}
		else {    
			if ( $F =~ /.*\.o/ ) { 
				push @OBJECTS, $F 
			} 
			else {
				if ($F =~ /(.*)\.[fqs]/) { $F = $1; } 
				push(@SLIBS, $F) if $F !~ /.*\.def/;
			}
		} 
    }
&else AIX
&if $OSSPEC in [ WIN32 ]
$ARC_TYPE = "lib" unless defined($ARC_TYPE);
@OBJECTS = ();
foreach $F (@ARGV) {
    if ($F =~ /(.*)\.dll(.*)/) { 
       $SUFF = $2;
       push @OBJECTS, "$1.$ARC_TYPE$SUFF" ; 
       next;
    }
	if ( $F =~ /-def:(.*)/ ) { $DEF = $1 ; next; }
    push(@OBJECTS, $F) if $F !~ /.*\.def/;
} #foraech
&else WIN32
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
&endif WIN32
&endif AIX

push @OBJECTS, ToolLib::CreateBuildNrModule(@KEEPFLG,$DEBUG?("-g"):());

&if $OSSPEC in [ WIN32 ]
if ( $CURR_VERSION !~ /^fast$/i || $LIBDEBUG ) { $DEBUG = "y" }

if ( $DEBUG && $PROF ) {
        push @LDFLAGS, "-debug", "-debugtype:both", "-pdb:none", "/PROFILE";
}
else {

   if ( $PROF ) {
      push @LDFLAGS, "-debug", "-debugtype:both", "-pdb:none", "/PROFILE";
   }
   elsif ( ! $NOLINKDEBUG ) {
      push @LDFLAGS, "-debug", "-debugtype:cv", "-pdb:none", "/OPT:REF";
   }

}

if ( $NODLL eq "" ) {
   $TMPFILE = "$ARCBASE.tmp";
   
	$DEF = "$DLLBASE.def" if $DEF eq "";
	if ( ! -f $DEF ) {
		warn "INFORMATION: Obsolete usage of -def=! Adapt description.\n";
		ICopy::icp("$DESC/fast/$DEF", "$WRK/$CURR_VERSION/tmp", 0, 0, 1);
	}
	if ( -f $DEF ) {
		system("$TOOL/pgm/mfconcom $DEF $TMPFILE $MACH $VMAKE_VERSION n $LD $OS $OSSPEC") == 0
			or die "Error while executing \"mfconcom $DEF $TMPFILE $MACH $VMAKE_VERSION n $LD $OS $OSSPEC\"\n".
			"message: $!\n"; 

		unlink $DEF;        # delete before write because it may be read-only

		open(FILE_IN, "<$TMPFILE") or die "Can't open $TMPFILE (input): $!\n";
		open(FILE_OUT, ">$DEF") or die "Can't open $DEF (output): $!\n";
		while(<FILE_IN>) { 
			s/INITINSTANCE TERMINSTANCE/$ARC/;
			print FILE_OUT ;
		}
		close FILE_IN; close FILE_OUT;

		$DLLFLG = "-def:$DEF";
	} else { warn "No definition file for $DLLFILE" }
}

if ( $MAP eq "" ) { $MAP = "-map:$ARCBASE.map" }

if ( $SCRIPT ne "" ) {
        print "SCRIPT = $SCRIPT $DLLBASE\n";
        system("$TOOL/bin/$SCRIPT $DLLBASE") == 0
           or die "Error while executing \"$TOOL/bin/$SCRIPT $DLLBASE\"\n".
		   "message: $!\n"; 
    }
    if ( $RCFILE ne "" ) {

		@ARGV = ( "$DLLBASE", "$RCFILE", @RCINCL );
		do "$TOOL/bin/comprc$TOOLEXT";
		if ( $@ ) { warn "Error while executing comprc:\n", "$@"; exit }
    }
&endif WIN32


#
#  Create the static library
#
@ARFLAGS;
&if $OSSPEC in [ WIN32 ]
ToolTrace("$AR called with: @ARFLAGS -out:$ARC -debugtype:cv $DLLFLG @ELIB @OBJECTS\n");
print ("$AR @ARFLAGS -out:$ARC -debugtype:cv $DLLFLG @ELIB @OBJECTS\n") if ($ENV{NOQUIET});
system("$AR @ARFLAGS -out:$ARC -debugtype:cv $DLLFLG @ELIB @OBJECTS") == 0
   or die "Error while executing $AR @ARFLAGS -out:$ARC -debugtype:cv $DLLFLG @ELIB ...\n".
   "message: $!\n"; 
# if $DLLFLG is set, $AR creates $ARC and $ARCBASE.exp
&else
ToolTrace("$AR called with: @ARFLAGS $ARCFILE @OBJECTS\n");
print ("$AR @ARFLAGS $ARCFILE @OBJECTS\n") if ($ENV{NOQUIET});
system($AR, @ARFLAGS, $ARCFILE, @OBJECTS) == 0
   or die "Error while executing $AR @ARFLAGS $ARCFILE @OBJECTS\n".
   "message: $!\n"; 
&endif

#
#  Link the dynamic library
#

if ( $NODLL eq "" ) {
&if $OS in [ UNIX ]
	$LIBPATH = "-L$TOOL/lib";
&endif

&if $OSSPEC in [ AIX ]
	# shared library <libname>.o requires list of extern procs
	# to be exported and list(s) of all still unresolved symbols
   	($BASE) = $DLL =~ /(.*)\./;
	# PTS 1002032 ->
	@LDCMD=("makeC++SharedLib_r","-p","-100", "-o");
    push @LDCMD, $DLL, @OBJECTS; 
    # push @LDCMD;  @LIBMATH, @IMPLIST; 
    push @LDCMD,@LIBPC, "-lC","-G"; 
	push @LDCMD,@SLIBS;
	push @LDCMD,@LDFLAGS;
    push @LDCMD,"-bnoquiet" if ($ENV{NOQUIET});
	ToolTrace("linkdll62 call: @LDCMD\n");
	print ("@LDCMD\n") if ($ENV{NOQUIET});
    system(@LDCMD)<=4
         or die "Error while executing @LDCMD\n".
           "message: $!\n";
	# local @LDCMD =("ld","-o");
	# push @LDCMD, $DLL, $LIBPATH, @OBJECTS, "-bM:SRE","-brtl", "-bexpall","-bnoentry";
    # push @LDCMD, @IMPLIST, @LIBMATH, "-G", "-lc";
	# push @LDCMD,"-bnoquiet" if ($ENV{NOQUIET});
	# system(@LDCMD)<=4 
	#	or die "Error while executing @LDCMD\n".
	#   "message: $!\n"; ;
	# <- PTS 1002032
&elif $OSSPEC in [ HPUX ]
	# shared library <libname>.sl
	@LDCMD=("ld", "-b", "-o", $DLL, $LIBPATH, @SLIBS, @OBJECTS);
	print "@LDCMD\n" if ($ENV{NOQUIET});
	system(@LDCMD) == 0
	   or die "Error while executing \"@LDCMD\"\n".
	   "message: $!\n"; 
&elif $OSSPEC in [ OSF1 ]
	# Avoid sys5 ld script
	# system("/usr/bin/ld -shared -no_archive -o $DLL $LIBPATH @OBJECTS @LIBMATH -lc") == 0
	@LDCMD=("/usr/bin/ld", "-expect_unresolved", "*");
	push @LDCMD, "-shared", "-no_archive", "-o", $DLL;
	push @LDCMD, $LIBPATH, @OBJECTS, @LIBMATH, "-lc";
	system (@LDCMD) == 0	
	   or die "Error while executing @LDCMD\n".
	   "message: $!\n"; 
&elif $OSSPEC in [ SOLARIS, SVR4 ]
&if $MACH in [ SUN ] || $OSSPEC == SOLARIS
	# shared library <libname>.so
	@LDCMD =("ld", "-G");
	if ($DLDFLAGS ne "") {
		push @LDCMD, $DLDFLAGS;
	}
	push @LDCMD, "-o", $DLL, $LIBPATH, @OBJECTS, @SLIBS; 
	push @LDCMD, @LIBPC, @LIBMATH, @LIBNET, @LIBAIO, "-lc";
	print ("@LDCMD\n") if ($ENV{NOQUIET});
	system (@LDCMD) == 0
	   or die "Error while executing \"@LDCMD\"\n".
	   "message: $!\n";
&else
	@LDCMD=("ld");
	if ($#DLDFLAGS != -1) {
		push @LDCMD, @DLDFLAGS;
	}
	push @LDCMD, "-o", $DLL, $LIBPATH, @OBJECTS, @SLIBS, @LIBPC, @LIBMATH, @LIBNET, @LIBAIO, "-lc";
	print ("@LDCMD\n") if ($ENV{NOQUIET});
	system (@LDCMD) == 0
	   or die "Error while executing \"@LDCMD\"\n".
	   "message: $!\n";
&endif 
&elif $OSSPEC in [ WIN32 ]
   # $ARCBASE.exp has been created by $AR
   push @LDFLAGS, "-out:$DLL", "$ARCBASE.exp", "$DEFLIB";
   ToolTrace("linkdll62: $LD @LDFLAGS -dll $ENT $MAP $RBJ @OBJECTS @LIBPC @Lib @ELIB @LIBMPROC $PROF"); 
   print("$LD @LDFLAGS -dll $ENT $MAP $RBJ @OBJECTS @LIBPC @Lib @ELIB @LIBMPROC $PROF") if ($ENV{NOQUIET}); 
   system("$LD @LDFLAGS -dll $ENT $MAP $RBJ @OBJECTS @LIBPC @Lib @ELIB @LIBMPROC $PROF") == 0
      or die "Error while executing $LD @LDFLAGS -dll $ENT $MAP $RBJ ...\n".
	  "message: $!\n"; 
&elif $OSSPEC in [ LINUX ]
	# shared library <libname>.so
    @LDCMD=("ld", "-shared", "-o", $DLL, $LIBPATH, @OBJECTS);
	system(@LDCMD) == 0
	   or die "Error while executing \"@LDCMD\"\n".
	   "message: $!\n"; 
&else
	# don't create shared library
	# vmake needs a dummy
	system("$TOOL/Posix/touch $DLL");
&endif

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

&if $OSSPEC in [ WIN32 ]
if ( -f $DLL && $NODLL eq "" ) {

	($VMAKE_EXT) = $CURR_VERSION =~ /^(.)/;
	$VMAKE_EXT = lc($VMAKE_EXT);

	if ( $STRIPDEBUG ne "" && $PROF eq "" ) {
          ToolTrace("separate debug info from $DLL\n");
		  system("$TOOL/pgm/symedit X -qo $DLLBASE.DBG $DLL") == 0
			 or die "Error while executing \"$TOOL/pgm/symedit X -qo $DLLBASE.DBG $DLL\"\n".
			 "message: $!\n"; 

		  if ( ! -d "$OWN/usr/symbols" ) { mkdir ("$OWN/usr/symbols", 0) }
		  copy "$DLLBASE.DBG", "$OWN/usr/symbols/$DLLBASE.dbg.$VMAKE_EXT";
		  unlink "$OWN/usr/symbols/$DLLBASE.dbg";
		  rename "$DLLBASE.DBG", "$OWN/usr/symbols/$DLLBASE.dbg";
	}
	else {
		  unlink "$OWN/usr/symbols/$DLLBASE.dbg.$VMAKE_EXT";
		  unlink "$OWN/usr/symbols/$DLLBASE.dbg";
	}
}
&endif WIN32

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
&if $OSSPEC in [ WIN32 ]
if ( $ARCFILE ne $ARC && -f $ARC ) { unlink $ARCFILE; rename $ARC, $ARCFILE }
&endif WIN32
