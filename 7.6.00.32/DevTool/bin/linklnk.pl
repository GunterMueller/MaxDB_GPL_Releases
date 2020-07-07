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
		next;
	}

    if (/^-.*/) { push @OFLAGS, $_; next }
 } #while

if ( @ARGV < 1 ) { print $USAGE;  exit 1; }


if  ( ! defined($ARCH_TYPE))
{
    print "Warning: (linklnk) ARCH_TYPE not set !\n";
    $ARCH_TYPE = ".a";
}


# PTS 1103511
$ARCH_SEARCH_STRING=$ARCH_TYPE;
$ARCH_SEARCH_STRING =~ s/\.//;
@OBJECTS = ();
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


push @LDCMD, $LD;
if ($LD eq $CXX) {
	push @LDCMD, @LDXXFLAGS;
} else {
	push @LDCMD, @LDFLAGS;
}

push @LDCMD, @THREAD_LDFLAGS  unless ($NOTHREAD);  # CTS 1110865
push @LDCMD, @LDEXEFLAGS, @OFLAGS;



push @BuildModules, ToolLib::CreateBuildNrModule(@KEEPFLG,$DEBUG?("-g"):(),$NOTHREAD?("-nothread"):());

push @LDCMD, ( "-o", "$PROG" );
push @LDCMD, ( "-L$TOOL_LIB" );
push @LDCMD, ( "-L$INSTLIB" );
push @LDCMD, @KEEP_SYMBOLS_FIRST_OPTION if ($KEEP_SYMBOLS);
push @LDCMD, @OBJECTS;
push @LDCMD, @Lib unless ($ENV{APOMAKE});
push @LDCMD, @BuildModules;
push @LDCMD, @SLIBS;
push @LDCMD, @LIBPC;
push @LDCMD, @LIBSCR;
push @LDCMD, @LIBMATH;
push @LDCMD, @LIBLD;    # CTS 1106257
push @LDCMD, "-lcrypt";

push @LDCMD, @DYNAMIC_LIB;
push @LDCMD, @LIBNET;
push @LDCMD, @LIBMPROC;
push @LDCMD, @LIBAIO, @LIB_PTHREAD  unless ($NOTHREAD);

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


if ( @KEEPFLG == 0 ) {
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


