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

&if $OS in [ UNIX ]
if ( $RELVER eq "R62" or $RELVER eq "R71" ) 
{
	do "$TOOL/bin/linkdll62$TOOLEXT";
	if ( $@ ) { die "Error while executing linkdll62:\n", "$@"; }
}
else
{
&endif

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
&if $OS in [ WIN32 ]
# with MS Compiler > 12 or lcapps pdb files are default
if (($ENV{_MSC_VER} gt "12")  || $ENV{APOMAKE}  || $ENV{FORCE_PDB_FILES} ) 
{
   $USE_PDB_FILES = 1;
}
&endif



do "$TOOL/bin/link_opts$TOOLEXT";
if ( $@ ) { print "$@"; exit }
@IMPLIST = ();
undef $REALNAME;


while ($_ = $ARGV[0], /^-/) {
    shift;
    last if /^--$/;

    if ( /^-linkoption=(.+)$/ ) { push @OFLAGS, $1; next } 

    if ( /^-tooloption=(.+)$/ ) { next } 

&if $OSSPEC in [ WIN32 ]
    if(/^-dll$/)     { next }     # ignored 

    if(/^-nodll$/)   { $NODLL = $_; next }

    if(/^-def=(.*)/) { $DEF = "$1.def"; next }

	if(/^-deflib=(.*)/) {
        @DEFLIB   = ("-defaultlib:$1.lib");
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
        elsif ( -f "$TOOL_LIB/$IMPMOD" ) { push @IMPLIST, "-bI:$TOOL_LIB/$IMPMOD";}
        else { 	push @IMPLIST, $F; 	}
	}
&endif 
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

&if $OSSPEC in [ WIN32 ]
if (!$DEBUG)
    { $STRIPDEBUG = "y" }
&endif

#some systems forbid some extensions for executables
if ($DLLFILE =~ /(.*)\.[fqs]/) { $DLL = $1} else { $DLL = $DLLFILE }
&if $OSSPEC in [ WIN32 ]
($DLLBASE) = $DLLFILE =~ /(\.*[^\.]*)/ ;  # alle Endungen werden abgeschnitten 
# with MS Compiler > 12 or lcapps pdb files are default
if (($ENV{_MSC_VER} gt "12")  || $ENV{APOMAKE}  || $ENV{FORCE_PDB_FILES} ) 
{
   $USE_PDB_FILES = 1;
}

&endif WIN32

if ( $NOSTATICLIB )
{
	$ARCFILE = $DLLFILE;
	$ARCFILE =~ s/\.$DLL_TYPE/$ARCH_TYPE/;
}
else
{
	$ARCFILE = shift;
}

&if $OSSPEC in [ WIN32 ]
if ($ARCFILE =~ /(.*)\.[fqs]/) { $ARC = $1 } else { $ARC = $ARCFILE }
($ARCBASE) = $ARCFILE =~ /(\.*[^\.]*)/ ;  # alle Endungen werden abgeschnitten 
&endif WIN32

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
&endif WIN32

&if $OSSPEC in [ WIN32 ]
@LDCMD = ($LDDLL_C);  # no difference between c, c++, c++->c
push @LDCMD, ( $NOTHREAD ? () : @THREAD_LDFLAGS ), @DLDFLAGS, @OFLAGS;

&elif $OSSPEC not in [ AIX ]
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
&endif WIN32


push @BuildModules, ToolLib::CreateBuildNrModule(@KEEPFLG,$DEBUG?("-g"):(),$NOTHREAD?("-nothread"):());


&if $OSSPEC in [ WIN32 ]

if ( $CURR_VERSION !~ /^fast$/i || $LIBDEBUG ) { $DEBUG = "y" }

if ( $DEBUG && $PROF ) {
        push @LDCMD, "-debug", "-debugtype:$LD_PROFDEBGTYPE", "/PROFILE";
}	
else {

   if ( $PROF ) {
      push @LDCMD, "-debug", "-debugtype:$LD_PROFDEBGTYPE", "/PROFILE";
   }
   elsif ( ! $NOLINKDEBUG ) {
      push @LDCMD, "-debug", "/OPT:REF", "/IGNORE:4089";
      push @LDCMD, "-debugtype:cv" unless ($USE_PDB_FILES);
   }
}

# PTS: 1107332
push @LDCMD, "-pdb:none" unless($USE_PDB_FILES);

if ( $NODLL eq "" ) {
   $TMPFILE = "$ARCBASE.tmp";
   
	$DEF = "$DLLBASE.def" if $DEF eq "";
	if ( ! -f $DEF ) {
		warn "INFORMATION: Obsolete usage of -def=! Adapt description.\n";
		ICopy::icp("$DESC/fast/$DEF", "$WRK/$CURR_VERSION/tmp", 0, 0, 1);
	}
	if ( -f $DEF ) {
	    # PTS: 1107332
		system("$TOOL/pgm/mfconcom", @CONCOMOPTS, $DEF, $TMPFILE, $MACH, $VMAKE_VERSION, 'n', $LD, $OS, $OSSPEC) == 0
			or die "Error while executing \"mfconcom \@CONCOMOPTS $DEF $TMPFILE $MACH $VMAKE_VERSION n $LD $OS $OSSPEC\"\n".
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

unless ($NOSTATICLIB)
{
#
#  Create the static library
#
@ARFLAGS;
@CONCOMOPTS;
&if $OSSPEC == WIN32
&if $MACH == AMD64
	ToolTrace("$AR called with: @ARFLAGS -out:$ARC $DLLFLG @ELIB @OBJECTS @BuildModules\n");
	print ("$AR @ARFLAGS -out:$ARC @DEFLIB $DLLFLG @ELIB @OBJECTS @BuildModules\n") if ($ENV{NOQUIET});
	system("$AR @ARFLAGS -out:$ARC @DEFLIB $DLLFLG @ELIB @OBJECTS @BuildModules") == 0
	   or die "Error while executing $AR @ARFLAGS -out:$ARC -debugtype:cv -nodefaultlib $DLLFLG @ELIB ...\n".
	   "message: $!\n";
	# if $DLLFLG is set, $AR creates $ARC and $ARCBASE.exp
&else
	ToolTrace("$AR called with: @ARFLAGS -out:$ARC -debugtype:cv $DLLFLG @ELIB @OBJECTS @BuildModules\n");
	print ("$AR @ARFLAGS -out:$ARC -debugtype:cv @DEFLIB $DLLFLG @ELIB @OBJECTS @BuildModules\n") if ($ENV{NOQUIET});
	system("$AR @ARFLAGS -out:$ARC -debugtype:cv @DEFLIB $DLLFLG @ELIB @OBJECTS @BuildModules") == 0
	   or die "Error while executing $AR @ARFLAGS -out:$ARC -debugtype:cv -nodefaultlib $DLLFLG @ELIB ...\n".
	   "message: $!\n";
	# if $DLLFLG is set, $AR creates $ARC and $ARCBASE.exp
&endif 
&else
	ToolTrace("$AR called with: @ARFLAGS $ARCFILE @OBJECTS\n");
	print ("$AR @ARFLAGS $ARCFILE @OBJECTS\n") if ($ENV{NOQUIET});
	system($AR, @ARFLAGS, $ARCFILE, @OBJECTS, @BuildModules) == 0
	   or die "Error while executing $AR @ARFLAGS $ARCFILE @OBJECTS @BuildModules\n".
	   "message: $!\n"; 
&endif
}

#
#  Link the dynamic library
#

if ( $NODLL eq "" ) {
&if $OS in [ UNIX ]
	$LIBPATH = "-L$TOOL_LIB";
&endif

&if $OSSPEC in [ AIX ]
	# shared library <libname>.o requires list of extern procs
	# to be exported and list(s) of all still unresolved symbols
   	($BASE) = $DLL =~ /(.*)\./;
	# PTS 1002032 ->
	@LDCMD=( $NOTHREAD ? $LDDLL_CXX_NOTHREAD : $LDDLL_CXX  ,"-p", "-100");
	if ($ENV{BIT64} eq "1")
	{ push @LDCMD, "-X64"; }
	else
	{ push @LDCMD, "-X32"; }
    push @LDCMD, "-o", $DLL, @OBJECTS; 
    # push @LDCMD;  @LIBMATH, @IMPLIST; 
    # push @LDCMD,  "-G"; (PTS 1108186) @LIBPC, (CTS 1107619) "-lC" (PTS 1104276)  
	push @LDCMD, @STLIBS, @BuildModules, @SLIBS;
	push @LDCMD, ( $NOTHREAD ? () : @THREAD_LDFLAGS ), @DLDFLAGS, @OFLAGS;
    push @LDCMD, "-bnoquiet" if ($ENV{NOQUIET});
	# <- PTS 1002032
	$ENV{LDR_CNTRL}="MAXDATA=0x40000000" if ($ENV{APOMAKE} && ($ENV{VMAKE_CURRENT_VARIANT} eq "Debug"));
	
&elif $OSSPEC in [ WIN32 ]
   # $ARCBASE.exp has been created by $AR
   push @LDCMD, "-out:$DLL";
   push @LDCMD, "$ARCBASE.exp" if ( -f "$ARCBASE.exp" );
   push @LDCMD, @DEFLIB;
   

	
if ( $USE_PDB_FILES ) 
{
	$SYMBOL_DIR =~ tr[/][\\];
	if ( ! -d "$SYMBOL_DIR")
	{
		mkpath ("$SYMBOL_DIR", 0777) if ( ! -d "$SYMBOL_DIR") || 
			die ("can't create $SYMBOL_DIR for symbol files\n");
	}
	push @LDCMD, ( "-PDB:$SYMBOL_DIR\\$DLLBASE.pdb" ) if ( $USE_PDB_FILES ) ;
}
   
   push @LDCMD, "-dll", $ENT, $MAP, $RBJ, @OBJECTS, @BuildModules, @LIBPC, @Lib, @ELIB, @LIBMPROC, @LIBPOGO, @LIBBUFCHECK, $PROF;
&elif $OSSPEC in [ HPUX, SOLARIS, SVR4, LINUX, OSF1 ]

&if $MACH = SUN || $OSSPEC == SOLARIS

	if ($ENV{APOMAKE})
	{
		@LOCAL_OBJECTS=();
		
		@ARCHIVES_OBJECTS = ();
		@ALLARCHIVES_MEMBERS = ();

		# if object files are built in object directory
		# -> use objects from object directory and copy static libs to . 
		if ($ENV{VMAKE_OPTION} =~ /u/)
		{
			@ALL_LOCAL_ARCHIVE_OBJECTS = ();
			foreach $archive (@STLIBS)
			{
				if ( $archive =~ /^(.*)\/([^\/]*)$/ )
				{
					if ( -f "../lstfiles/$2.lst" )
					{
						open (LSTFILE, "<../lstfiles/$2.lst");
						while (<LSTFILE>)
						{
							my $localobject= $_;
							chop ($localobject);
							push @ALL_LOCAL_ARCHIVE_OBJECTS, $localobject;
						}
						close (LSTFILE);
					}
					else
					{
						push @ALLARCHIVES_MEMBERS, $2;
						unlink $2;
						copy "$archive", "$2";
					}
				}
				else
				{
					push @ARCHIVES_OBJECTS, $archive;
				}
			}
			
			push @LDCMD, "-o", $DLL, $LIBPATH, @OBJECTS, 
						@ARCHIVES_OBJECTS, @ALL_LOCAL_ARCHIVE_OBJECTS, 
						@ALLARCHIVES_MEMBERS, @BuildModules, @SLIBS;   
		}
		else
		{
			foreach $archive (@STLIBS) 
			{
				if ( $archive =~ /*.$ARCH_TYPE$/ )
				{
					$objsString = `$AR t $archive`;
					@all_arMembers = split " ", $objsString;
					@arMembers = grep /\.o$/, @all_arMembers;
					system($AR, 'x', $archive);
					push @ARCHIVES_OBJECTS, @arMembers;
					push @ALLARCHIVES_MEMBERS, @all_arMembers; 
				}
				else
				{
					push @ARCHIVES_OBJECTS, $archive;
				}	
			}
        
			foreach $object (@OBJECTS) 
			{
				if ($object =~ /.*\/([^\/]*\.o)$/)
				{
					unlink "$1";
					copy "$object", "$1";
					push @LOCAL_OBJECTS, $1;
				}
				else
				{
					push @LOCAL_OBJECTS, $object;
				}
			}
   			push @LDCMD, "-o", $DLL, $LIBPATH, @LOCAL_OBJECTS, @ARCHIVES_OBJECTS, @BuildModules, @SLIBS;
		}
	}
	else
	{
		push @LDCMD, "-o", $DLL, $LIBPATH, @OBJECTS, @STLIBS, @BuildModules, @SLIBS;
	}
&elif $OSSPEC in [ HPUX, OSF1 ]
	if ($ENV{APOMAKE})
	{
		@LOCAL_OBJECTS=();
		
		@ARCHIVES_OBJECTS = ();
		foreach $archive (@STLIBS)
		{
			if ( $archive =~ /^(.*)\/([^\/]*)$/ )
			{
				if ( -f "../lstfiles/$2.lst" )
				{
					open (LSTFILE, "<../lstfiles/$2.lst");
					while (<LSTFILE>)
					{
						my $localobject= $_;
						chop ($localobject);
						push @ARCHIVES_OBJECTS, $localobject;
					}
					close (LSTFILE);
					next;
				}
			}
			push @ARCHIVES_OBJECTS, $archive;
		}
&if $OSSPEC in [ OSF1 ]		
		$ARCHIVES_OBJECTS_ARCHIVE = "archive_$DLL.a";
		print ("archive objects: @ARCHIVES_OBJECTS\n");
		unlink "archive_$DLL.a";
		system ($AR , @ARFLAGS, "$ARCHIVES_OBJECTS_ARCHIVE", @ARCHIVES_OBJECTS)== 0 
			or die "Error while executing $AR @ARFLAGS $ARCHIVES_OBJECTS_ARCHIVE @ARCHIVES_OBJECTS\n";
		push @LDCMD, "-o", $REALNAME ? $REALNAME : $DLL, $LIBPATH, @OBJECTS, 
		           $ARCHIVES_OBJECTS_ARCHIVE, @BuildModules, "-ptr $WRK/fast/template", @SLIBS;
&else
		push @LDCMD, "-o", $DLL, $LIBPATH, @OBJECTS, @ARCHIVES_OBJECTS, @BuildModules, @SLIBS;   
&endif
	}
	else
	{
&if $OSSPEC in [ OSF1 ]	
		push @LDCMD, "-o", $REALNAME ? $REALNAME : $DLL, $LIBPATH, @OBJECTS, @STLIBS, @BuildModules, @SLIBS;
&else
		push @LDCMD, "-o", $DLL, $LIBPATH, @OBJECTS, @STLIBS, @BuildModules, @SLIBS;
&endif
	}
&else
   push @LDCMD, "-o", $DLL, $LIBPATH, @OBJECTS, @STLIBS, @BuildModules, @SLIBS;
&endif
&if $OSSPEC in [ OSF1 ]
	if ($ENV{APOMAKE})
	{	push @LDCMD, "/usr/lib/libdb.a", @LIBMATH; }
	else
	{   push @LDCMD, @LIBMATH, "-lc"; }
&endif OSF1
    # CTS 1107045
	if ($LDDLL_TYPE eq "LDDLL_CXX_C") { 
	   push @LDCMD, @DLD_CXX_C_FLAGS_END;
	}
&else AIX
 	# don't create shared library
	# vmake needs a dummy
	die "ERROR: Sorry but I don't know how to make a shared library\n";
&endif AIX

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
&if $OSSPEC in [ WIN32 ]	
	if ($ENV{APOMAKE})
	{
		system ("$LINKCMD") == 0 or die "Error while executing $LINKCMD \nmessage: $!\n";
	}
	else
	{
&endif
	
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
	}
&endif
	
&if $OSSPEC in [ WIN32 ]
	unless ( $? == 0 )
&else
	unless ( $? <= 4 )
&endif
	{
		die "Error while executing $LINKCMD \nmessage: $!\n";
	}
&if $MACH = SUN || $OSSPEC == SOLARIS
	if ($ENV{APOMAKE})
	{
		unlink @ALLARCHIVES_MEMBERS;
		unlink @LOCAL_OBJECTS;
	}
&endif
&if $OSSPEC in [ OSF1 ]
	if ($REALNAME)
	{
		unlink $DLL;
		rename "$REALNAME", "$DLL";
	}
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
$DLL_DEP_COMMAND = "dumpbin /dependents";
if (scalar(@DENIES))
{
	my @wrong_dependencies = ();
	my %dependencies;
	my $found_start = 0;
	open(DEP_CHECK, "$DLL_DEP_COMMAND $DLL 2>&1 |") or  die "Can't check dependencies of $DLL";
	while (<DEP_CHECK>)
	{
		if ($found_start)
		{
			if (/ Summary/)
			{
				last;
			}
			elsif (/\s*(\S*\.dll)/)
			{
				my $dep = $1;
				$dep =~ tr/a-z/A-Z/;
				$dependencies{$dep} = 1;
			}
		}
		else
		{
			if (/Image has the following dependencies/)
			{	$found_start = 1;	}
		}
	}
		
	foreach $deny (@DENIES)
	{
		$deny =~ tr/a-z/A-Z/;
		if (defined $dependencies{$deny} || defined $dependencies{"$deny.DLL"})
		{
			push  @wrong_dependencies, $deny;
		}
	}
	if (scalar(@wrong_dependencies))
	{
		die ("ERROR: Found denied dependencies ". (join ",", @wrong_dependencies )."\n");
	}
}

if ( -f $DLL && $NODLL eq "" ) {

	($VMAKE_EXT) = $CURR_VERSION =~ /^(.)/;
	$VMAKE_EXT = lc($VMAKE_EXT);
    if ( $NO_SYMBOL_FILES )
	{ 
		unlink "$DLLBASE.dbg", "$DLLBASE.pdb";
		unlink "$DLLBASE.dbg.$VMAKE_EXT", "$DLLBASE.pdb.$VMAKE_EXT";
	}
	else
	{
	  if ( $USE_PDB_FILES ) 
	  {
			unlink "$SYMBOL_DIR/$DLLBASE.pdb.$VMAKE_EXT";
			copy "$SYMBOL_DIR/$DLLBASE.pdb","$SYMBOL_DIR/$DLLBASE.pdb.$VMAKE_EXT" 
	       || warn "Can't copy $SYMBOL_DIR/$DLLBASE.pdb to $SYMBOL_DIR/$DLLBASE.pdb.$VMAKE_EXT: $!\n";
	  }
	  else 
	  {
			if ( $STRIPDEBUG ne "" && $PROF eq "" ) 
			{
	            ToolTrace("separate debug info from $DLL\n");
				  system("$TOOL/pgm/symedit X -qo $DLLBASE.DBG $DLL") == 0
					 or die "Error while executing \"$TOOL/pgm/symedit X -qo $DLLBASE.DBG $DLL\"\n".
					 "message: $!\n"; 
	
		   		  if ( ! -d "$SYMBOL_DIR")
			      {
				  	mkpath ("$SYMBOL_DIR", 0777) if ( ! -d "$SYMBOL_DIR") || 
				  		die ("can't create $SYMBOL_DIR for symbol files\n");
				  }
				  copy "$DLLBASE.DBG", "$SYMBOL_DIR/$DLLBASE.dbg.$VMAKE_EXT";
				  unlink "$SYMBOL_DIR/$DLLBASE.dbg";
				  rename "$DLLBASE.DBG", "$SYMBOL_DIR/$DLLBASE.dbg";
			}
			else {
				  unlink "$SYMBOL_DIR/$DLLBASE.dbg.$VMAKE_EXT";
				  unlink "$SYMBOL_DIR/$DLLBASE.dbg";
			}
	  }
	}
}

if ( @KEEPFLG == 0 ) 
{
	unlink "$DLLBASE.res";
	unlink "$DLLBASE.rbj";
	if ( $NO_SYMBOL_FILES )
	{ 
		unlink "$DLLBASE.map", "$ARCBASE.map";
		unlink "$DLLBASE.map.$VMAKE_EXT", "$ARCBASE.map.$VMAKE_EXT";
	}
	else
	{
		# unlink "$DEF";
		if ( -f $DLL && -f "$DLLBASE.map" ) {
		  if ( ! -d "$MAP_DIR") 
		  {
		  	mkpath ("$MAP_DIR", 0777) || die ("can't create $MAP_DIR for map files\n");
		  }
		  copy "$DLLBASE.map", "$MAP_DIR/$DLLBASE.map.$VMAKE_EXT" 
		  || warn "Can't copy $DLLBASE.map to $MAP_DIR/$DLLBASE.map.$VMAKE_EXT: $!\n";
		  unlink "$MAP_DIR/$DLLBASE.map";
		  rename "$DLLBASE.map", "$MAP_DIR/$DLLBASE.map";
		}
		
		if ( $NODLL eq "" && -f $DLL && -f "$ARCBASE.map" ) {
		  unlink "$ARCBASE.tmp", "$ARCBASE.exp";
		  if ( ! -d "$MAP_DIR") 
		  {
		  	mkpath ("$MAP_DIR", 0777) || die ("can't create $MAP_DIR for map files\n");
		  }
		  copy "$ARCBASE.map", "$MAP_DIR/$ARCBASE.map.$VMAKE_EXT" 
		  || warn "Can't copy $ARCBASE.map to $MAP_DIR/$ARCBASE.map.$VMAKE_EXT: $!\n";
		  unlink "$MAP_DIR/$ARCBASE.map";
		  rename "$ARCBASE.map", "$MAP_DIR/$ARCBASE.map";
		}
		else {
		unlink "$ARCBASE.tmp", "$ARCBASE.exp", "$ARCBASE.map";
		}
	}
}
&endif WIN32


if ( $DLLFILE ne $DLL && -f $DLL ) { unlink $DLLFILE; rename $DLL, $DLLFILE }
&if $OSSPEC in [ WIN32 ]
if ( $ARCFILE ne $ARC && -f $ARC ) { unlink $ARCFILE; rename $ARC, $ARCFILE }
&endif WIN32

&if $OS in [ UNIX ]
}
&endif
