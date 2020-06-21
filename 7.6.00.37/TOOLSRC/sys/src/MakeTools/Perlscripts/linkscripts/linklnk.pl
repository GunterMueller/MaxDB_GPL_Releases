#!/usr/bin/perl
# linklnk       *** internal script (called by vmake !)
#
# @(#)linklnk		1997-11-18
#
# link pascal object modules to create executable image
#
# TOOL          directory for tools
# TOOL_LIB      specific directory for tool libraries (32bit/64bit specific)
# INSTLIB       specific directory for installed libraries (32bit/64bit specific)
# LDFLAGS       optional loader flags
# KEEPFLG       optional keep intermediate files flag
#
# 1996-10-16 G. Großmann: -lib=ptocnt[s|r] in Descriptions setzt @LIBPC explizit neu 
# 			  mit $TOOL/lib/ptocnt[s|r].lib
#
# 1997-02-04 GG: Map-Files werden nach $SYS/map/$VMAKE_VERSION verschoben
# 1998-09-09 &gar: MAP-Files moved to $DBROOT/Support/Mapfiles -> 2 files: 
#                  normal (xxx.map) and one with second extension <VMAKE_EXT>
#
# 2000-10-12 JoergMensing $TOOL/lib replaced by $TOOL_LIB, $INSTROOT/lib by $INSTLIB
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

package linklnk;
use Env;
use File::Copy;
use File::Path;
use ToolLib;

# Get LDFLAGS, LD, LIB*
do "$TOOLVARS";
if ( $@ ) { print "$@"; exit }
ToolTrace("linklnk called with @ARGV\n");

$USAGE = "usage: linklnk [-p] [ld-flags] objects\n";

$DEBUG  = "";
$PROF   = "";
$SCRIPT = "";
$RCFILE = "";
$NOTHREAD = 0; # CTS 1110865
local @LDCMD=();
@BuildModules=();
@SLIBS= ();
$NOWHATSTRING = 0;
$KEEP_SYMBOLS = 0;
@APPENDFLAGS=();
undef $REALNAME;
$USE_PDB_FILES=0;
$QUANTIFY=0;

&if $OS in [ WIN32 ]
$MAP     = "-map";
# with MS Compiler > 12 or lcapps pdb files are default
if (($ENV{_MSC_VER} gt "12")  || $ENV{APOMAKE}  || $ENV{FORCE_PDB_FILES} ) 
{
   $USE_PDB_FILES = 1;
}
&endif


$OUTFILE = "a.out";
$PROG = $OUTFILE;   # some systems forbid some extensions for executables

do "$TOOL/bin/link_opts$TOOLEXT";
if ( $@ ) { print "$@"; exit }


while ($_ = $ARGV[0], /^-/) {
    shift;
    last if /^--$/;

    if ( /^-linkoption=(.+)$/ ) { push @OFLAGS, $1; next } 

    if ( /^-tooloption=(.+)$/ ) { next } 
    
    if ( /^-realname=(.+)$/ )
	{
		$REALNAME="$1";
		$PROG=$REALNAME;
&if $OSSPEC in [ WIN32 ]
		$PROG.=".exe" unless ($PROG =~ /\.exe$/); 
&endif		
		next;
	}

    if (/^-.*/) { push @OFLAGS, $_; next }
 } #while

if ( @ARGV < 1 ) { print $USAGE;  exit 1; }

&if $OSSPEC in [ WIN32 ]
if (!$DEBUG)
    { $STRIPDEBUG = "y" }
&endif

if  ( ! defined($ARCH_TYPE))
{
    print "Warning: (linklnk) ARCH_TYPE not set !\n";
&if $OSSPEC in [ WIN32 ]
    $ARCH_TYPE = ".lib";
&else
    $ARCH_TYPE = ".a";
&endif
}


# PTS 1103511
$ARCH_SEARCH_STRING=$ARCH_TYPE;
$ARCH_SEARCH_STRING =~ s/\.//;
@OBJECTS = ();
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


push @LDCMD, $LD;
if ($LD eq $CXX) {
	push @LDCMD, @LDXXFLAGS;
} else {
	push @LDCMD, @LDFLAGS;
}

push @LDCMD, @THREAD_LDFLAGS  unless ($NOTHREAD);  # CTS 1110865
push @LDCMD, @LDEXEFLAGS, @OFLAGS;
  

&if $OSSPEC in [ WIN32 ]
$PROG =~ /(.*)\.exe/, $BASE = $1 ne "" ? $1 : $PROG;

if ( $SCRIPT ne "" ) {
   print "SCRIPT = $SCRIPT $BASE\n";
   system("$TOOL/bin/$SCRIPT $BASE") == 0
		     or die "Error while executing \"$TOOL/bin/$SCRIPT $BASE\"\n".
			 "message: $!\n"; 
}

if ( $RCFILE ne "" ) { 
	@ARGV = ( "$BASE", "$RCFILE", @RCINCL );
	do "$TOOL/bin/comprc$TOOLEXT";
	if ( $@ ) { warn "Error while executing comprc:\n", "$@"; exit }
}

if ( $CURR_VERSION !~ /^f/i ) { $DEBUG = "y" }

if ( $DEBUG && $PROF ) {
        push @LDCMD, "-debug", "-debugtype:$LD_PROFDEBGTYPE", "/PROFILE"; }
else {
   if ( $PROF ) {
      push @LDCMD, "-debug", "-debugtype:$LD_PROFDEBGTYPE", "/PROFILE"; 
   }
   elsif ( ! $NOLINKDEBUG ) {
      push @LDCMD, "-debug", "/OPT:REF", "/IGNORE:4089";
      push @LDCMD, "-debugtype:cv" unless ($USE_PDB_FILES);
   };
}

push @LDCMD, "-pdb:none" unless($USE_PDB_FILES);
&endif WIN32

push @BuildModules, ToolLib::CreateBuildNrModule(@KEEPFLG,$DEBUG?("-g"):(),$NOTHREAD?("-nothread"):());

&if $OSSPEC in [ WIN32 ]
push @LDCMD, ( "-out:$PROG" , $MAP );
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

&else WIN32
push @LDCMD, ( "-o", "$PROG" );
push @LDCMD, ( "-L$TOOL_LIB" );
push @LDCMD, ( "-L$INSTLIB" );
&endif WIN32
push @LDCMD, @KEEP_SYMBOLS_FIRST_OPTION if ($KEEP_SYMBOLS);
push @LDCMD, @OBJECTS;
push @LDCMD, @Lib unless ($ENV{APOMAKE});
push @LDCMD, @BuildModules;
push @LDCMD, @SLIBS;
push @LDCMD, @LIBPC;
push @LDCMD, @LIBSCR;
push @LDCMD, @LIBMATH;
push @LDCMD, @LIBLD;    # CTS 1106257
&if $OSSPEC in [ LINUX ]
push @LDCMD, "-lcrypt";
&endif
&if $OSSPEC in [ AIX ]  # PTS 1001950
push @LDCMD, "-bnoipath";
push @LDCMD, "-brtl";
push @LDCMD, "-bnoquiet" if ($ENV{NOQUIET});
&endif

&if $OSSPEC in [ SOLARIS, SVR4 ]
&if $MACH = SUN || $OSSPEC == SOLARIS
if ($LD ne $CXX) {
	push @LDCMD, @DYNAMIC_LIB;
}
&else
push @LDCMD, @DYNAMIC_LIB;
&endif
&else
push @LDCMD, @DYNAMIC_LIB;
&endif
push @LDCMD, @LIBNET;
push @LDCMD, @LIBMPROC;
&if $OSSPEC in [ WIN32 ]
push @LDCMD, ( $PROF );
push @LDCMD, @LIBPOGO;
push @LDCMD, @LIBBUFCHECK;
&else
push @LDCMD, @LIBAIO, @LIB_PTHREAD  unless ($NOTHREAD);
&endif

if ($QUANTIFY)
{ unshift @LDCMD, ($ENV{'QUANTIFYCALL'} ? $ENV{'QUANTIFYCALL'} : "quantify"); }
push @LDCMD, @APPENDFLAGS;
$LINKCMD=join " ", @LDCMD;
$LINKCMD=~s/\s(\S*)\*(\S*)\s/ "$1\*$2" /g;
$LINKCMD=~s/\s(\S*)\?(\S*)\s/ "$1\?$2" /g;
$LINKCMD.=" ".ToolLib::getWhatModule($PROG, $LINKCMD, @KEEPFLG,$DEBUG?("-g"):(),$NOTHREAD?("-nothread"):())
		unless ($ENV{APOMAKE} || $NOWHATSTRING || ($ENV{RELVER} eq "RTOOL") );
print ("$LINKCMD\n") if ($ENV{NOQUIET});
ToolTrace("linklnk call: $LINKCMD\n");
open(LINK,"$LINKCMD 2>&1 |");
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

&if $OSSPEC in [ WIN32 ]
if ( -f $PROG ) {

	($VMAKE_EXT) = $CURR_VERSION =~ /^(.)/;
	$VMAKE_EXT = lc($VMAKE_EXT);

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
	  } else {
			if ( $STRIPDEBUG && !$PROF ) {
	         ToolTrace("separate debug info from $PROG\n");
	         ToolTrace("linklnk called:\symedit X -qro $BASE.DBG $PROG\n");
			   system("$TOOL/pgm/symedit X -qro $BASE.DBG $PROG") == 0
					 or die "Error while executing \"$TOOL/pgm/symedit X -qro $BASE.DBG $PROG\"\n".
					 "message: $!\n"; 
		     if ( ! -d "$SYMBOL_DIR")
		      {
			  	mkpath ("$SYMBOL_DIR", 0777) if ( ! -d "$SYMBOL_DIR") || 
			  		die ("can't create $SYMBOL_DIR for symbol files\n");
			  }

			   copy "$BASE.DBG","$SYMBOL_DIR/$BASE.dbg.$VMAKE_EXT" 
	          || warn "Can't copy $BASE.dbg to $SYMBOL_DIR/$BASE.dbg.$VMAKE_EXT: $!\n";
			   unlink "$SYMBOL_DIR/$BASE.dbg";
			   rename "$BASE.DBG", "$SYMBOL_DIR/$BASE.dbg"
	          || warn "Can't rename $BASE.dbg to $SYMBOL_DIR/$BASE.dbg: $!\n";
			}
			else {
			   unlink "$SYMBOL_DIR/$BASE.dbg.$VMAKE_EXT";
			   unlink "$SYMBOL_DIR/$BASE.dbg";
			}
	  }
	 }
}
&endif

if ( @KEEPFLG == 0 ) {
&if $OSSPEC in [ WIN32 ]
   unlink "$BASE.res"; # resource pre-object
   unlink "$BASE.rbj"; # resource object
   unlink <*.pch>; # precompiled header
&endif
	if ( -f $PROG && -f "$BASE.map" ) 
	{
		if ( $NO_SYMBOL_FILES )
		{ 
			unlink "$BASE.map", "$BASE.map.$VMAKE_EXT";
		}
		else
		{
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

if ( $OUTFILE ne $PROG ) { unlink $OUTFILE; rename($PROG, $OUTFILE) }


