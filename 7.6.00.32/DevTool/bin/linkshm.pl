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

push @LDFLAGS, @LDEXEFLAGS;
push @LDXXFLAGS, @LDEXEFLAGS;

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


$PROG = $OUTFILE;   # some systems forbid some extensions for executables

do "$TOOL/bin/link_opts$TOOLEXT";
if ( $@ ) { print "$@"; exit }

$PROG_ORG = $PROG;

while ($_ = $ARGV[0], /^[-\+]/) {
    shift;
    last if /^--$/;


	if ( /^-realname=(.+)$/ )
	{
		$REALNAME="$1";
		$PROG=$REALNAME;
		next;
	}

    if(/^[-\+].*/) { push @OFLAGS, $_; next }

} #while

if ( @ARGV < 1 ) { print $USAGE; exit 1 }


if  ( ! defined($ARCH_TYPE))
{
    print "Warning: (linkshm) ARCH_TYPE not set !\n";
    $ARCH_TYPE = ".a";
}

# PTS 1103511
$ARCH_SEARCH_STRING=$ARCH_TYPE;
$ARCH_SEARCH_STRING =~ s/\.//;
@OBJECTS = (); # gsize.o not used
foreach $F (@ARGV) {
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



if ( $SCRIPT ne "" ) {
    print "SCRIPT=$SCRIPT $BASE\n";
    system("$TOOL/bin/$SCRIPT $BASE") == 0
	   or die "Error while executing \"$TOOL/bin/$SCRIPT $BASE\"\n".
	   "message: $!\n";
}
@OBJECTS = ( "gsize.o", @OBJECTS );
map { s/-lg/-g/ } @OBJECTS;


# Make gsize.c
@CFLAGS;
do "$TOOL/bin/ptc_gsiz$TOOLEXT";
if ( $@ ) { die "Error while executing $TOOL/bin/ptc_gsiz$TOOLEXT: $@" }

system($CC, @CCFLAGS, @CFLAGS, "-c", "gsize.c") == 0
	 or die "Error while executing \"$CC $CCFLAGS @CFLAGS -c gsize.c\"\n".
	 "message: $!\n";

push @BuildModules, ToolLib::CreateBuildNrModule(@KEEPFLG,$DEBUG?("-g"):(),$NOTHREAD?("-nothread"):());

@LDCMD= ($LD);
if ($LD eq $CXX) {
	push @LDCMD, @LDXXFLAGS;
} else {
	push @LDCMD, @LDFLAGS;
}

push @LDCMD, @THREAD_LDFLAGS unless ($NOTHREAD);   # CTS 1110865

push @LDCMD, @LDEXEFLAGS, @OFLAGS;
push @LDCMD, @LDSHMFLAGS;  # PTS 1106517

push @LDCMD, "-o", $PROG;
#&if $OSSPEC in [ HP9 ]
#push @LDCMD,"-Wl", "-a" , "archive_shared";
#&endif
push @LDCMD, "-L$TOOL_LIB";
push @LDCMD, @KEEP_SYMBOLS_FIRST_OPTION if ($KEEP_SYMBOLS);
push @LDCMD, @OBJECTS, @BuildModules, @SLIBS, @LIBPC, @LIBSCR, @LIBMATH, @DYNAMIC_LIB, @LIBNET;
push @LDCMD, @LIBLD;     # CTS 1106257
push @LDCMD, @LIBAIO, @LIB_PTHREAD;
push @LDCMD, @Lib, @LIBMPROC;

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
