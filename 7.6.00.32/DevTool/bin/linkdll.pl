#!/usr/bin/perl
# linkdll       *** internal script (called by vmake !)
#
# @(#)linkdll		1997-11-18
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


package linkdll;
use Env;
use File::Copy;
use File::Path;
use ICopy;
use ToolLib;

if ( $RELVER eq "R62" or $RELVER eq "R71" )
{
	do "$TOOL/bin/linkdll62$TOOLEXT";
	if ( $@ ) { die "Error while executing linkdll62:\n", "$@"; }
}
else
{

# Get LDFLAGS, AR
@LIBMATH = ();
do "$TOOLVARS";
if ( $@ ) { print "$@"; exit }
ToolTrace("linkdll called with @ARGV\n");

$USAGE = "usage: linkdll [ld-flags] dll archive objects\n";

$MAP    = "";
$DEF    = "";
$NODLL  = "";
$DLLFLG = "";
$ENT    = "";
@Lib    = ();
@ELIB   = ();
if ($ENV{APOMAKE})
{ @DEFLIB = (); }
else
{ @DEFLIB = ("-nodefaultlib");}
$DEBUG  = "";
$PROF   = "";
$SCRIPT = "";
$RCFILE = "";
$NOTHREAD = 0; # CTS 1110865
$NOSTATICLIB = 0;
@LIBPC;
local @LDCMD=();
@SLIBS=();
@STLIBS=();
@ALLARCHIVES_MEMBERS = ();
@LOCAL_OBJECTS=();
@BuildModules=();
$NOWHATSTRING = 0;
$QUANTIFY=0;
@APPENDFLAGS=();



do "$TOOL/bin/link_opts$TOOLEXT";
if ( $@ ) { print "$@"; exit }
@IMPLIST = ();
undef $REALNAME;


while ($_ = $ARGV[0], /^-/) {
    shift;
    last if /^--$/;

    if ( /^-linkoption=(.+)$/ ) { push @OFLAGS, $1; next }

    if ( /^-tooloption=(.+)$/ ) { next }

	if ( /^-realname=(.+)$/ )
	{
		$REALNAME="$1";
		next;
	}

	if ( /^-nostaticlib$/ )
	{
		$NOSTATICLIB=1;
		next;
	}

    if(/^-.*/)     { push @OFLAGS, $_; next }

} #while

if ( @ARGV < 3 ) { print $USAGE; exit 1 }

$DLLFILE = shift;


#some systems forbid some extensions for executables
if ($DLLFILE =~ /(.*)\.[fqs]/) { $DLL = $1} else { $DLL = $DLLFILE }

if ( $NOSTATICLIB )
{
	$ARCFILE = $DLLFILE;
	$ARCFILE =~ s/\.$DLL_TYPE/$ARCH_TYPE/;
}
else
{
	$ARCFILE = shift;
}


    @OBJECTS = ();
    foreach $F (@ARGV) {
		if ( $F =~ /-def:(.*)/ ) { $DEF = $1 ; next; }
        if ( $F =~ /.*\.o/ ) { push @OBJECTS, $F }
		else {
			if ($F =~ /(.*)\.[fqs]/)
			{
				if ($F =~ /(.*)\.[fqs]/) { $F = $1; }
				if ($F =~ /(.*)\.$DLL_TYPE$/)
				{
					local $dynlibpath = $1  ;
					if ( $dynlibpath =~ /^(.*)\/lib([^\/]*)$/)
                    {
                         push @SLIBS, "-L$1", "-l$2";
                    }
                    else
                    {
                        die "ERROR: try to link dynamic lib with irregular name \n".
							"$dynlibpath.$DLL_TYPE\n";
                    }
				}
				else
				{
					push(@STLIBS, $F) if $F !~ /.*\.def/;
				}
			}
			else
			{
				push(@STLIBS, $F) if $F !~ /.*\.def/;
			}
		}
    }

if ($LDDLL_TYPE eq "LDDLL_CXX") {
	@LDCMD=($LDDLL_CXX);
	push @LDCMD, ( $NOTHREAD ? () : @THREAD_LDFLAGS ) , @DLDXXFLAGS, @OFLAGS;
}
# CTS 1107045
elsif ($LDDLL_TYPE eq "LDDLL_CXX_C") {
	@LDCMD=($LDDLL_CXX_C);
	push @LDCMD, ( $NOTHREAD ? () : @THREAD_LDFLAGS ) , @DLD_CXX_C_FLAGS_START;
}
else {
    @LDCMD=($LDDLL_C);
	push @LDCMD, ( $NOTHREAD ? () : @THREAD_LDFLAGS ), @DLDFLAGS, @OFLAGS;
}


push @BuildModules, ToolLib::CreateBuildNrModule(@KEEPFLG,$DEBUG?("-g"):(),$NOTHREAD?("-nothread"):());



unless ($NOSTATICLIB)
{
#
#  Create the static library
#
@ARFLAGS;
@CONCOMOPTS;
	ToolTrace("$AR called with: @ARFLAGS $ARCFILE @OBJECTS\n");
	print ("$AR @ARFLAGS $ARCFILE @OBJECTS\n") if ($ENV{NOQUIET});
	system($AR, @ARFLAGS, $ARCFILE, @OBJECTS, @BuildModules) == 0
	   or die "Error while executing $AR @ARFLAGS $ARCFILE @OBJECTS @BuildModules\n".
	   "message: $!\n";
}

#
#  Link the dynamic library
#

if ( $NODLL eq "" ) {
	$LIBPATH = "-L$TOOL_LIB";


   push @LDCMD, "-o", $DLL, $LIBPATH, @OBJECTS, @STLIBS, @BuildModules, @SLIBS;
    # CTS 1107045
	if ($LDDLL_TYPE eq "LDDLL_CXX_C") {
	   push @LDCMD, @DLD_CXX_C_FLAGS_END;
	}

	push @LDCMD, @APPENDFLAGS;

	if ($QUANTIFY)
	{ unshift @LDCMD, ($ENV{'QUANTIFYCALL'} ? $ENV{'QUANTIFYCALL'} : "quantify"); }
	$LINKCMD=join " ", @LDCMD;
	$LINKCMD=~s/\s(\S*)\*(\S*)\s/ "$1\*$2" /g;
	$LINKCMD=~s/\s(\S*)\?(\S*)\s/ "$1\?$2" /g;
	$LINKCMD.=" ".ToolLib::getWhatModule($DLL, $LINKCMD, @KEEPFLG,$DEBUG?("-g"):(),$NOTHREAD?("-nothread"):())
		unless ($ENV{APOMAKE} || $NOWHATSTRING );
	print ("$LINKCMD\n") if ($ENV{NOQUIET});
	ToolTrace("linkdll call: $LINKCMD\n");

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

	unless ( $? <= 4 )
	{
		die "Error while executing $LINKCMD \nmessage: $!\n";
	}
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


if ( $DLLFILE ne $DLL && -f $DLL ) { unlink $DLLFILE; rename $DLL, $DLLFILE }

}
