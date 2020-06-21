#!/usr/bin/perl
# linkshm       *** internal script (called by vmake !)
#
# @(#)linkshm		1997-11-18
#
# link a program, which uses shared memory
#
# TOOL          directory for tools
# LDFLAGS       optional loader flags
# KEEPFLG       optional keep intermediate files flag
#
# 1996-10-16 G. Großmann: -lib=ptocnt[s|r] in Descriptions setzt $LIBPC explizit neu 
# 			  mit $TOOL/lib/ptocnt[s|r].lib
#
# 1997-02-04 GG: Map-Files werden nach $SYS/map/$VMAKE_VERSION verschoben
# 1998-09-09 &gar: MAP-Files moved to $DBROOT/Support/Mapfiles -> 2 files: 
#                  normal (xxx.map) and one with second extension <VMAKE_EXT>
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


package linkshm;
use Env;
use File::Basename;
use File::Copy;
use File::Path;
use ToolLib;

# Get LDFLAGS, CC, LIB*
do "$TOOLVARS";
if ( $@ ) { print "$@"; exit }
ToolTrace("linkshm called with @ARGV\n");

$USAGE = "usage: linkshm [-k] [-p] [ld-flags] objects\n";

@argsForDetectGlobals = @ARGV;

&if $OSSPEC in [ HPUX ]
@LDFLAGS = (@KLDFLAGS, @LDEXEFLAGS);
@LDXXFLAGS = (@KLDFLAGS, @LDEXEFLAGS);
&else
push @LDFLAGS, @LDEXEFLAGS;
push @LDXXFLAGS, @LDEXEFLAGS;
&endif

$SHRGLOB = "";
$OUTFILE = "a.out";
@LIBDYN  = ();
$DEBUG   = "";
$PROF    = "";
$SCRIPT  = "";
$RCFILE  = "";
$NOTHREAD = 0; # CTS 1110865
@RCINCL  = ();
@BuildModules=();
$NOWHATSTRING = 0;
undef $REALNAME;
$KEEP_SYMBOLS = 0;
@SLIBS=();
@APPENDFLAGS=();
$USE_PDB_FILES=0;
$QUANTIFY=0;

local @LDCMD   = ();
&if $OS in [ WIN32 ]
$MAP     = "-map";
# with MS Compiler > 12 or lcapps pdb files are default
if (($ENV{_MSC_VER} gt "12")  || $ENV{APOMAKE}  || $ENV{FORCE_PDB_FILES} ) 
{
   $USE_PDB_FILES = 1;
}
&endif


$PROG = $OUTFILE;   # some systems forbid some extensions for executables

do "$TOOL/bin/link_opts$TOOLEXT";
if ( $@ ) { print "$@"; exit }

$PROG_ORG = $PROG;

while ($_ = $ARGV[0], /^[-\+]/) {
    shift;
    last if /^--$/;

&if $OSSPEC in [ WIN32 ]
    if(/^-t(.*)/) { push @OFLAGS, $1; next }
&endif

	if ( /^-realname=(.+)$/ )
	{
		$REALNAME="$1";
		$PROG=$REALNAME;
&if $OSSPEC in [ WIN32 ]
		$PROG.=".exe" unless ($PROG =~ /\.exe$/); 
&endif		
		next;
	}

    if(/^[-\+].*/) { push @OFLAGS, $_; next }

} #while

if ( @ARGV < 1 ) { print $USAGE; exit 1 }

&if $OSSPEC in [ WIN32 ]
if (!$DEBUG)
    { $STRIPDEBUG = "y" }
&endif

if  ( ! defined($ARCH_TYPE))
{
    print "Warning: (linkshm) ARCH_TYPE not set !\n";
&if $OSSPEC in [ WIN32 ]
    $ARCH_TYPE = ".lib";
&else
    $ARCH_TYPE = ".a";
&endif
}

# PTS 1103511
$ARCH_SEARCH_STRING=$ARCH_TYPE;
$ARCH_SEARCH_STRING =~ s/\.//;
@OBJECTS = (); # gsize.o not used
foreach $F (@ARGV) {
&if $OSSPEC in [ WIN32 ]
    if ($F =~ /(.*)\.dll(\..*)/) { 
       $SUFF = $2;
       push @OBJECTS, "$1.$ARCH_TYPE$SUFF" ; 
       next;
    }
&else
	if ($F =~ /(.*)\.$DLL_TYPE$\.[fqs]?$/) 
	{
		local $dynlibpath = $1  ;
		if ( $dynlibpath =~ /^(.*)\/lib([^\/]*)$/)
		{
			 push @SLIBS, "-L$1", "-l$2";
		}
		else
		{
			die "ERROR: try to include dynamic lib with irregular name \n".
				"$dynlibpath.$DLL_TYPE\n";
		}
		next;
	}
&endif
    if ($F =~ /(.*)\.$ARCH_SEARCH_STRING(\.[^\.]*)?$/) 
    { 
    	$F="$1$ARCH_TYPE";
    	push @OBJECTS, @KEEP_SYMBOLS_OPTION_BEFORE_LIB if ( $KEEP_SYMBOLS && @KEEP_SYMBOLS_OPTION_BEFORE_LIB );
    	push @OBJECTS, $F;
    	push @OBJECTS, @KEEP_SYMBOLS_OPTION_AFTER_LIB if ( $KEEP_SYMBOLS && @KEEP_SYMBOLS_OPTION_AFTER_LIB );
    }
    else
    {
    	push @OBJECTS, $F;
    }
} #foraech


&if  $OSSPEC in [ WIN32 ]
$BASE = basename($PROG, "\.exe");
if ( $RCFILE ne "" ) {

	@ARGV = ( "$BASE", "$RCFILE", @RCINCL );
	do "$TOOL/bin/comprc$TOOLEXT";
	if ( $@ ) { warn "Error while executing comprc:\n", "$@"; exit }
}
&endif

if ( $SCRIPT ne "" ) {
    print "SCRIPT=$SCRIPT $BASE\n";
    system("$TOOL/bin/$SCRIPT $BASE") == 0
	   or die "Error while executing \"$TOOL/bin/$SCRIPT $BASE\"\n".
	   "message: $!\n"; 
}
&if  $OS = UNIX
& if $OSSPEC in [ AIX, MI_ULT, OSF1, SINIX, SUNOS ]
@OBJECTS = ( "gsize.o", @OBJECTS );
& else
@OBJECTS = ( "gsize.o", @OBJECTS );
map { s/-lg/-g/ } @OBJECTS;
& endif
&endif UNIX


&if $OSSPEC in [ WIN32 ]
# gsize not used
&else WIN32
# Make gsize.c
@CFLAGS;
do "$TOOL/bin/ptc_gsiz$TOOLEXT";
if ( $@ ) { die "Error while executing $TOOL/bin/ptc_gsiz$TOOLEXT: $@" }

& if $OSSPEC in [ MI_ULT, OSF1 ] 
# With any other level of optimization, cc hangs.
system($CC, @CCFLAGS, @CFLAGS, $NO_OPTIMIZE, "-c", "gsize.c") == 0
	 or die "Error while executing \"$CC $CCFLAGS @CFLAGS $NO_OPTIMIZE -c gsize.c\"\n".
	 "message: $!\n"; 
& elif $MACH in [ NMP ]
system($CC, @CCFLAGS, @CFLAGS, "-c", "-g", "gsize.c") == 0
	 or die "Error while executing \"$CC $CCFLAGS @CFLAGS -c -g gsize.c\"\n".
	 "message: $!\n"; 
& else
system($CC, @CCFLAGS, @CFLAGS, "-c", "gsize.c") == 0
	 or die "Error while executing \"$CC $CCFLAGS @CFLAGS -c gsize.c\"\n".
	 "message: $!\n"; 
& endif
&endif WIN32

push @BuildModules, ToolLib::CreateBuildNrModule(@KEEPFLG,$DEBUG?("-g"):(),$NOTHREAD?("-nothread"):());

@LDCMD= ($LD);
&if $OSSPEC in [ HPUX ]
push @LDCMD, @KLDFLAGS;
&else
if ($LD eq $CXX) {
	push @LDCMD, @LDXXFLAGS;
} else {
	push @LDCMD, @LDFLAGS;
}
&endif

push @LDCMD, @THREAD_LDFLAGS unless ($NOTHREAD);   # CTS 1110865

push @LDCMD, @LDEXEFLAGS, @OFLAGS;
push @LDCMD, @LDSHMFLAGS;  # PTS 1106517

&if $OSSPEC in [ WIN32 ]
if ( $VMAKE_VERSION !~ /^fast$/i ) { $DEBUG = "y" }
if ( $DEBUG && $PROF ) {
    # Debug und Profiling
    push @LDCMD, "-debug", "-debugtype:$LD_PROFDEBGTYPE", "/PROFILE"; 
}
else {
    # Profiling ODER Debug
    if ( $PROF ) {
        push @LDCMD, "-debug", "-debugtype:$LD_PROFDEBGTYPE", "/PROFILE";
    }
    elsif ( ! $NOLINKDEBUG ) {
      push @LDCMD, "-debug", "/OPT:REF", "/IGNORE:4089";
      push @LDCMD, "-debugtype:cv" unless ($USE_PDB_FILES);
    }
}

# PTS: 1107332


push @LDCMD,"-out:$PROG",$MAP;
if ( $USE_PDB_FILES ) 
{
	$SYMBOL_DIR =~ tr[/][\\];
	if ( ! -d "$SYMBOL_DIR")
	{
		mkpath ("$SYMBOL_DIR", 0777) if ( ! -d "$SYMBOL_DIR") || 
			die ("can't create $SYMBOL_DIR for symbol files\n");
	}
	push @LDCMD, ( "-PDB:$SYMBOL_DIR\\$BASE.pdb" ) if ( $USE_PDB_FILES ) ;
}
else
{
	push @LDCMD, "-pdb:none";
}

push @LDCMD, @LIBPOGO;
push @LDCMD, @LIBBUFCHECK;
&else
push @LDCMD, "-o", $PROG;
#&if $OSSPEC in [ HP9 ]
#push @LDCMD,"-Wl", "-a" , "archive_shared";
#&endif
push @LDCMD, "-L$TOOL_LIB";
&endif WIN32
push @LDCMD, @KEEP_SYMBOLS_FIRST_OPTION if ($KEEP_SYMBOLS);
push @LDCMD, @OBJECTS, @BuildModules, @SLIBS, @LIBPC, @LIBSCR, @LIBMATH, @DYNAMIC_LIB, @LIBNET;
push @LDCMD, @LIBLD;     # CTS 1106257
&if $OS in [ UNIX ]
push @LDCMD, @LIBAIO, @LIB_PTHREAD;
&else
push @LDCMD, $PROF;
&endif
push @LDCMD, @Lib, @LIBMPROC;
&if ($MACH = SUN AND $OSSPEC = SVR4) || $OSSPEC == SOLARIS
push @LDCMD, @LIBDYN;
push @LDCMD, @LDSHMLIBS;  # PTS 1100802
&endif

if ($QUANTIFY)
{ unshift @LDCMD, ($ENV{'QUANTIFYCALL'} ? $ENV{'QUANTIFYCALL'} : "quantify"); }
push @LDCMD, @APPENDFLAGS;
$LINKCMD=join " ", @LDCMD;
$LINKCMD=~s/\s(\S*)\*(\S*)\s/ "$1\*$2" /g;
$LINKCMD=~s/\s(\S*)\?(\S*)\s/ "$1\?$2" /g;
$LINKCMD.=" ".ToolLib::getWhatModule($PROG, $LINKCMD, @KEEPFLG,$DEBUG?("-g"):(),$NOTHREAD?("-nothread"):())
	unless ($ENV{APOMAKE} || $NOWHATSTRING );
print ("$LINKCMD\n") if ($ENV{NOQUIET});
ToolTrace("linkshm call: $LINKCMD\n");
open(LINK,"$LINKCMD 2>&1 |") or  die "Can't create pipe with command '$LINKCMD'";
while(defined($line=<LINK>))
{
		# ignore lines with ignored values
		next if ( ($LNK_IGNORE ne "") && ($line=~/($LNK_IGNORE)/i));
		print $line; 
}
close(LINK);
unless ( $? == 0 )
{
		 die "Error while executing $LINKCMD \nmessage: $!\n";
}


# Because of set -e, output file is left unrenamed if ld fails
# and shell returns exit code of ld.

if ( -f $PROG ) {

   $_ = $VMAKE_VERSION;
   CASE: {
	if (/^fast$/i) { $VMAKE_EXT = "f"; last CASE; }

	if (/^slow$/i) { $VMAKE_EXT = "s"; last CASE; }

	if (/^quick$/i) { $VMAKE_EXT = "q"; last CASE; }
   }

&if $OS in [ WIN32 ]

	if ( $NO_SYMBOL_FILES )
	{ 
		unlink "$BASE.dbg", "$BASE.pdb";
		unlink "$BASE.dbg.$VMAKE_EXT", "$BASE.pdb.$VMAKE_EXT";
	}
	else
	{
		
	 if ( $USE_PDB_FILES ) {
			unlink "$SYMBOL_DIR/$BASE.pdb.$VMAKE_EXT";
			copy "$SYMBOL_DIR/$BASE.pdb","$SYMBOL_DIR/$BASE.pdb.$VMAKE_EXT" 
	       || warn "Can't copy $SYMBOL_DIR/$BASE.pdb to $SYMBOL_DIR/$BASE.pdb.$VMAKE_EXT: $!\n";
	  }
	  else {
	    if ( $STRIPDEBUG ne ""  && $PROF eq "") {
	       print "separate debug info from $PROG\n";
	       system("$TOOL/pgm/symedit X -qo $BASE.DBG $PROG") == 0
		 	   or die "Error while executing \"$TOOL/pgm/symedit X -qo $BASE.DBG $PROG\"\n".
			 "message: $!\n"; 
	
	        if ( ! -d "$SYMBOL_DIR")
		      {
			  	mkpath ("$SYMBOL_DIR", 0777) if ( ! -d "$SYMBOL_DIR") || 
			  		die ("can't create $SYMBOL_DIR for symbol files\n");
			  }
	       copy $BASE.DBG, "$SYMBOL_DIR/$BASE.dbg.$VMAKE_EXT";
	       unlink "$SYMBOL_DIR/$BASE.dbg";
	       rename "$BASE.DBG", "$SYMBOL_DIR/$BASE.dbg";
	     }
	     else {
	       unlink "$SYMBOL_DIR/$BASE.dbg.$VMAKE_EXT";
	       unlink "$SYMBOL_DIR/$BASE.dbg";
	     }
	  }
	}
	# CTS 1108541
	print "run imagecfg ...\n";
	$rc=system("$TOOL/pgm/imagecfg -l $PROG");
	# print "\nRC=$rc\n";
	#     or die "\nError while executing \"$TOOL/pgm/imagecfg -l $PROG\"\n".
	#	 "message: $!\n"; 
	print "OK\n";

    #
    # run special script to report global variables as warnings
    #
    if ($ENV{VMAKE_WARNING_PROTOCOL} && -r "$WRK/bin/detectGlobals.py") {
        if (-x "$TOOL/Python/python.exe") {
            $python = "$TOOL/Python/python.exe";
        }
        else {
            $python = "python";    # expect python in path
        }
        system ("$python", "$WRK/bin/detectGlobals.py", "-for_warning_protocol", "--", @argsForDetectGlobals);
    }

&endif
}

if ( @KEEPFLG == 0 ) {
	if ( $NO_SYMBOL_FILES )
	{ 
		unlink "$BASE.map", "$BASE.map.$VMAKE_EXT";
	}
	else
	{
	    unlink "gsize", "allsiz", "gsize.siz", "gsize.c", "gsize.o";
	    if ( -f "$BASE.res" ) { unlink "$BASE.res" }
	    if ( -f "$BASE.rbj" ) { unlink "$BASE.rbj" }
		if ( -f $PROG && -f "$BASE.map" ) {
	      if ( ! -d "$MAP_DIR") 
		  {
		  	mkpath ("$MAP_DIR", 0777) || die ("can't create $MAP_DIR for map files\n");
		  }
		  copy "$BASE.map", "$MAP_DIR/$BASE.map.$VMAKE_EXT" 
		  || warn "Can't copy $BASE.map to $MAP_DIR/$BASE.map.$VMAKE_EXT: $!\n";
		  unlink "$MAP_DIR/$BASE.map";
	      rename "$BASE.map", "$MAP_DIR/$BASE.map";
	   }
	}
}

if ( $OUTFILE ne $PROG ) { unlink $OUTFILE; rename $PROG, $OUTFILE }
