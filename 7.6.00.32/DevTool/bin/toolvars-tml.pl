#!/usr/bin/perl
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

#
#    @(#)toolvars-tml.pl
#    included by several scripts, defines system specific values
#


use Env;
use Config;
use CCVers;
use POSIX;

# CTS 1110865
@THREAD_CCFLAGS=();
@THREAD_LDFLAGS=();
@LDFLAGS=();
@LDXXFLAGS=();

if ($RELVER eq "")
{
	print "\n!!! WARNING:  RELVER not set!!!\n";
	exit 1;
}

if (! $ENV{INSTROOT})
{
	print "\n!!! WARNING:  INSTROOT not set!!!\n";
	exit 1;
}

# force set of compiler version
CCVers::setup_compiler_version();


$ADABAS_COPYRIGHT="Copyright (c) SAP AG 1997-2003";
if ($RELVER eq "R61") {
	$ADABAS_VERSION="6.1";
}
elsif ($RELVER eq "R62") {
	$ADABAS_VERSION="6.2";
}
elsif ($RELVER eq "R70") {
	$ADABAS_VERSION="7.0";
}
elsif ($RELVER eq "R71") {
	$ADABAS_VERSION="7.1";
}
elsif ($RELVER eq "R72") {
	$ADABAS_VERSION="7.2";
}
elsif ($RELVER eq "R73") {
	$ADABAS_VERSION="7.3";
}
elsif ($RELVER eq "R74") {
	$ADABAS_VERSION="7.4";
}
elsif ($RELVER eq "R75") {
	$ADABAS_VERSION="7.5";
}
elsif ($RELVER eq "R80") {
	$ADABAS_VERSION="8.0";
}


$SQLVERSION="REL30";$ENV{SQLVERSION} = $SQLVERSION; # export $SQLVERSION

$OSSPEC=LINUX;
$ENV{OSSPEC} = $OSSPEC;		# export $OSSPEC
$MACH = "I386";
$ENV{MACH} = $MACH;		# export $MACH
$OS = "UNIX";
$ENV{OS} = $OS;			# export $OS

# CTS 1106697
unless ( $ENV{OS_VER} && $ENV{OS_REL} )
{
    local
    ($os_sysname, $os_nodename, $os_release, $os_version, $os_machine) =
    POSIX::uname ();
	  if ($os_release =~ /^(\d+)\.(\d+)\.(\d+).*/)
	  {
	      $os_version = "$1";
	      $os_release = "$2$3";
	  }
    $ENV{OS_VER} = $os_version;
    $ENV{OS_REL} = $os_release;
}
$OS_VER = $ENV{OS_VER};
$OS_REL = $ENV{OS_REL};

# PTS 1135222: differentiation SuSE SLES-8/SLES-9
$ENV{LNX_VER} = "undef";
if ( $OSSPEC eq "LINUX" && $MACH eq "IA64" ){
   $LIN_REL=substr($OS_REL,0,1);
   if ( $OS_VER == 2 && $LIN_REL == 6 ){
      $ENV{LNX_VER} = "SLES_9";
   }
   if ( $OS_VER == 2 && $LIN_REL == 4 ){
      $ENV{LNX_VER} = "SLES_8";
   }
}


$TOOL_LIB = ($ENV{BIT64} eq "1") ? "$TOOL/lib/lib64" : "$TOOL/lib";
$ENV{TOOL_LIB} = $TOOL_LIB;

$COMP="PTOC";
if ($RELVER eq "R61") {
	#@CONCOMOPTS = ( "-REL=R61" );
	@CONCOMOPTS = ( "-$SQLVERSION=1", "-REL=R61" );
	#@CONCOMOPTS = ( "-SAG=1", "-$SQLVERSION=1", "-REL=R61" );
}
else
{
	#@CONCOMOPTS = ( );
	@CONCOMOPTS = ( "-$SQLVERSION=1" );
	#@CONCOMOPTS = ( "-SAG=1", "-$SQLVERSION=1" );
}
push @CONCOMOPTS, "-RELVER=$RELVER";
push @CONCOMOPTS, "-DIAGNOSE=1" if defined( $ENV{DIAGNOSE} );
push @CONCOMOPTS, "-LC_STATE=".(defined $ENV{'LC_STATE'} ? "$ENV{'LC_STATE'}" : "DEV");
push @CONCOMOPTS, "-COMPILER=".(defined $ENV{'COMPILER'} ? "$ENV{'COMPILER'}" : "UNDEF");
push @CONCOMOPTS, "-MACHINE=".(defined $ENV{'MACHINE'} ? "$ENV{'MACHINE'}" : "UNDEF");

#
# Used for all systems using 64 bit compilation!
#
if ( $ENV{BIT64} eq "1" )
{
	push @CONCOMOPTS, "-BIT64=1";
	if ( $ENV{XCOMP} eq "1" )
	{
		push @CONCOMOPTS, "-XCOMP=1";
	}
}
# used for 32 KB pages  (PTS 1111111)
if ( $ENV{page32KB} )
{
	push @CONCOMOPTS, "-page32KB=1";
}

# jrg 2.9.1999

if ( not $CC )
{
	$CC = ($ENV{'COMPILER'} =~ /^ICC$/i) ? "icc" : "cc";

}
#else take CC from the environment
if ( not $CXX )
{
	$CXX = ($ENV{'COMPILER'} =~ /^ICC$/i) ? "icpc" : "c++";
}




# CTS 1107045
# $LDDLL_C         - command of link c objects to shared lib
# $LDDLL_CXX       - command of link c++ shared lib
# $LDDLL_CXX_C     - command of link c++ objects to c shared lib
# to initialize $LDDLL
$LDDLL_C = $CC;
$LDDLL_CXX = $CXX;
$LDDLL_CXX_C = $CXX;
#else take CXX from the environment PTS 1001861

if (($RELVER ge "R72") && ($RELVER ne "RTOOL"))  # &gar PTS 1002110
{
	$LD = $CXX if ( not $LD );
	$LDDLL_TYPE = "LDDLL_CXX";
}
else
{
	$LD	= $CC if ( not $LD );
	$LDDLL_TYPE = "LDDLL_C";
}


# jrg 2.9.1999
# For relocatable objects (collection of object files) a different linker is used
# These definitions replace former system dependend setting in linkrel.pl!
#
# WIN32 uses $AR and $ARFLAGS...
$LDREL = ($ENV{'COMPILER'} =~ /^ICC$/i) ? "icc" : "ld";

$AS	= ($ENV{'COMPILER'} =~ /^ICC$/i) ? "gcc" : $CC;
$AR = ($ENV{'COMPILER'} =~ /^ICC$/i) ? "xiar" : "ar";
$NULL = "/dev/null";

# set C and CXX as environment variable
$ENV{CC} = $CC;
$ENV{CXX} = $CXX;

#
# Additional compiler flags may be set temporary in the environment
# before calling vmake.
#

@ASFLAGS = ();

# 98/06/19 &gar: for differences between C and C++-Compiling in UNIX !
# @CCFLAGS   -> FLAGS for both ( C, C++ )
# @CFLAGS    -> FLAGS for C only
# @CXXFLAGS  -> FLAGS for C++ only
  @CXXFLAGS = split /\s+/, $CXXFLAGS;
  @CFLAGS = split /\s+/, $CFLAGS;
@CCFLAGS = split /\s+/, $CCFLAGS;
push @CCFLAGS, "-D$SQLVERSION",  "-D$OSSPEC", "-D$MACH", "-DSAG";
if ($RELVER =~ /^R(\d)(\d)/)
{
push @CCFLAGS, "-DSDB_RELEASE_0${1}0${2}" . ((length ("$CORRECTION_LEVEL") == 2) ? "$CORRECTION_LEVEL" : "0$CORRECTION_LEVEL");
}
if ( ! defined $ENV{APOMAKE} && ($ENV{'LC_STATE'} =~ /^DEV$/) )
{ push @CCFLAGS,  "-DSDB_LCSTATE_" . (defined $ENV{LC_STATE} ? $ENV{LC_STATE} : "DEV"); }

push @CCFLAGS, "-DDIAGNOSE" if defined( $ENV{DIAGNOSE} );
push @CCFLAGS, "-DSAPDBMem_USE_SYSTEM_ALLOC" if defined( $ENV{SAPDBMem_USE_SYSTEM_ALLOC} );
@CCMODEANSI = ();
@CCMODENORM = ();
@CCMODELINT = ();

if ($ENV{APOMAKE})
{
	push @CCFLAGS,  "-DAPO_COM_SHORT=$ENV{APO_COM_SHORT}";

	push @CCFLAGS,  "-DAPO_PATCH_LEVEL=$ENV{APO_PATCH_LEVEL}" if ($ENV{APO_PATCH_LEVEL});
	#  "-DAPO_COM_RELEASE=$APO_COM_RELEASE",
	#  "-DAPO_COM_BUILD=$APO_COM_BUILD",


	push @CCFLAGS,  "-DBRANCH_INFO=$ENV{BRANCH_INFO}",
	                "-DLATEST_CHANGE_LIST=$ENV{LATEST_CHANGE_LIST}";
#	                "-DBRANCH_INFO_STR=\"$BRANCH_INFO\"",
#					"-DLATEST_CHANGE_LIST_STR=\"$LATEST_CHANGE_LIST\"";

	# BRANCH_INFO, BRANCH_INFO
	push @CCFLAGS, "-DSAPonUNIX", "-DSAPwithTHREADS";
	push @CCFLAGS, "-DSAP_JS_OS_VERSION=$OS_VER", "-DSAP_JS_OS_RELEASE=$OS_REL";
	if ($ENV{'BIT64'} eq "1")
	{
		push @CCFLAGS, "-DSAPonLIN64";
	}
	else
	{
		push @CCFLAGS, "-DSAPonLIN";
	}
	if ($MACH =~ /SDBonPPC64/)
	{
		push @CCFLAGS, "-DSAPonPPC64";
	}
}



# 1.8.2001 E.Z. PTS 1111111
if ( $ENV{page32KB} )
{
   push @CCFLAGS, "-Dpage32KB";
}


if ( $RELVER ge "R80" ) {
#
# gg 22.12.2001 put stlport directory in include search path
#
    push @CCFLAGS, "-I$WRK/incl/SAPDB/STLport/stlport";
#
# Some day this can be removed again if we can use src/SAPDB as new src...
#
    push @CCFLAGS, "-I$WRK/incl/SAPDB";
}
@EXCEPT_FLAG = ();
@RTTI_FLAG = ();    # on by default

# check for LC_INC for APO
die "LC_INC is not defined in environment!\n" if ( $ENV{APOMAKE} && ! $ENV{LC_INC} );
@LC_INC_FLAGS = ("-I$ENV{LC_INC}") if ( $ENV{APOMAKE} && $ENV{LC_INC} );



  if ( ($RELVER ge "R72") && ($RELVER ne "RTOOL")){

push @THREAD_CCFLAGS, "-D_REENTRANT";		# CTS 1110865


       push @CCFLAGS, "-I$INSTROOT/incl", "-I$WRK/incl";

  } else {
#
# older versions ( < V7.2 ) do not support multithreading in any way...
#
    push @CCFLAGS, "-I$INSTROOT/incl", "-I$WRK/incl";

  }

#
# Some day this can be removed again if we can use src/SAPDB as new src...
#
  if (( $RELVER ge "R73" ) && ( ! $ENV{APOMAKE} )) {
  		push @CCFLAGS, "-I$WRK/incl/SAPDB";
    }



	#
	# DanielD we want to disable warnings about how the compiler
	# goes from tsp00_Array to const char *
	#
	push @CCFLAGS, "-DSYSV", "-D_SVID";
	push @CCFLAGS, "-I/usr/include/ncurses";
	push @CCFLAGS, "-w", "-Wall" if ($RELVER ge "R74");
	if (($MACH =~ /^(IA64|SDBonPPC64)$/) && ($ENV{'COMPILER'} !~ /ICC/i))
	{	push @CXXFLAGS, '-fno-rtti', '-Wno-deprecated';  }
	if ($ENV{'COMPILER'} =~ /ICC/i)
	{	push @CCFLAGS, "-Qoption,cpp,--new_conditional", '-fno-rtti'; }

	push @CXXFLAGS, '-export-dynamic'  if ($MACH eq 'SDBonPPC64');

	push @CCFLAGS, '-fsigned-char' if ($MACH eq 'S390X');

	push @CCFLAGS, "-D_FILE_OFFSET_BITS=64";
    @LINTFLAGS = @CCFLAGS;





if ($ENV{APOMAKE})
{
	if ($MACH eq "I386") {
		@OPTIMIZE = ($ENV{LNX_VER} eq "SLES_9") ? ("-O3", "-march=pentiumpro") :
		                                          ("-O3", "-march=pentium", "-mcpu=pentiumpro");
	} elsif (($MACH eq "IA64") && ($ENV{'COMPILER'} =~ /ICC$/i)) {
		@OPTIMIZE = ();
	} elsif ($MACH eq "IA64" || $MACH eq "SDBonPPC64" || $MACH eq "S390X" || $MACH eq "X86_64") {
		@OPTIMIZE = ("-O2");
	} else {
		@OPTIMIZE = ("-O");
	}
}
# "OPTIMIZE" now depends on "VMAKE_VERSION" (needed on AIX)
elsif ($CURR_VERSION =~ /^f/i) {
    # the old value, it was used for everything
	if ($MACH eq "I386") {
		@OPTIMIZE = ("-O3", "-march=pentium", "-mcpu=pentiumpro");
	}
	elsif (($MACH =~ /^IA64|X86_64$/) && ($ENV{'COMPILER'} =~ /ICC$/i)) {
    @OPTIMIZE = ("-O1");
  }
	elsif ($MACH eq "IA64" || $MACH eq "SDBonPPC64" || $MACH eq "S390X" || $MACH eq "X86_64") {
		@OPTIMIZE = ("-O2");
	} else {
		@OPTIMIZE = ("-O");
	}
}
else {
	# VMAKE_VERSION is quick or slow
	# one level less than fast - not yet really tested
	# the idea is to combine range checks with less risky code
	if ($MACH eq "I386") {
		@OPTIMIZE = ("-O2");
	} elsif ($MACH eq "IA64" || $MACH eq "SDBonPPC64" || $MACH eq "S390X" || $MACH eq "X86_64") {
		@OPTIMIZE = ("-O2");
	} else {
		@OPTIMIZE = ();
	}
}

# CTS 1103440
@DEBUG_FLAGS = ("-g");

push @DEBUG_FLAGS, "-DSAPDB_DEBUG";



# CTS 1108431
@NO_FLAGS = ();



# cba 98/07/16 : cause of differeces of C- and C++-Linker
# we may have different linker options on some UNIX-platforms
# we use @LDFLAGS for C-linking and @LDXXFLAGS for C++-Linking
# &gar 99/06/03: for linkdll new variable DLDFLAGS and DLDXXFLAGS
# ( PTS 1103001 )
# initialize
# CTS 1107045
# @DLD_CXX_C_FLAGS_START, DLD_CXX_C_FLAGS_END
# @DLD_CXX_C_FLAGS_START  options before objects
# @DLD_CXX_C_FLAGS_END    options after objects
@LDFLAGS = @LDXXFLAGS = (($MACH eq "IA64" || $MACH eq "SDBonPPC64") && ($ENV{'COMPILER'} !~ /^ICC$/i)) ? ("-Wl,-relax") : ();
@DLDFLAGS = @DLDXXFLAGS = ("-shared");
@DLD_CXX_C_FLAGS_START = ("-shared");
@DLD_CXX_C_FLAGS_END = ();

if ($ENV{'COMPILER'} =~ /ICC/i)
{
	push @LDFLAGS, "-i-static";
	push @LDXXFLAGS, "-i-static";
	push @DLDXXFLAGS, "-i-static";
	push @DLDFLAGS, "-i-static";
	push @LDREL_FLAGS, "-i-static";
}

#############################
# options for Multipasslink #
#############################

@KEEP_SYMBOLS_FIRST_OPTION = ();
@KEEP_SYMBOLS_OPTION_BEFORE_LIB = ();
@KEEP_SYMBOLS_OPTION_AFTER_LIB = ();

@KEEP_SYMBOLS_OPTION_BEFORE_LIB = ('-Wl,-whole-archive');
@KEEP_SYMBOLS_OPTION_AFTER_LIB = ('-Wl,-no-whole-archive');

#
# For relocatable objects (collection of object files) a different linker is used
# These definitions replace system dependend setting in linkrel.pl!
#
# WIN32 uses AR and AS_FLAGS
#
# SOLARIS:
# There is no specific option that tells ld   to  link  64-bit
# objects;  the  class of the first object that gets processed
# by ld  determines whether it is to perform  a  32-bit  or  a
# 64-bit link edit.
# AIX:
# 32bit and 64bit relocatable object may even coexist...
#
	@LDREL_FLAGS = ( "-r", "-L$TOOL_LIB" );
	( $ENV{'COMPILER'} =~ /^ICC$/i ) and
		push @LDREL_FLAGS,  "-mno-relax", "-nostdlib", "-no_cpprt";
	# Since compiler is used for linker instead plain ‘ld’.


# &jrg 14.9.1999
#
# C++ compilation sometimes needs to setup a path to a directory for
# the template cache. In this template cache instances of template
# functions with external linkage are stored.
# Since template instances may differ depending on
# VMAKE_VERSION (fast,slow...) for each version a seperate
# template cache is forced
#
#
# gcc...nothing needed. If REALLY wanted smaller code is producable with
# -fexternal-templates and #pragma implementation / #pragma interface...
# I would not recomment this even whit producing LOTs of code now...
#

# &gar PTS 1001828
@PRF_FLAGS = "-pg";

push @PRF_FLAGS, "-DSDB_WITH_PROFILER";


# &gar OFLAGS for call options
# used in link... and comp...
# always initialize here with ()
@OFLAGS=();

$SR_ID =   "static char ident_sr[] =";
$MF_ID =   "static char ident_mf[] =";
$PC_ID =   "static char ident_pc[] =";
$CC_ID  =  "static char ident_cc[] =";
$ID_END = ";";
$BUILD_ID      = "#ident";
$BUILD_ID_END  = "";

@PCFLAGS= ( "-qn", "-e" );
@ARFLAGS = ( "rcs" );
$RANLIB = "ar ts";

if ($ENV{APOMAKE})
{	@LIBPC=(); }
else
{
	@LIBPC = ( "$TOOL_LIB/libsqlptc.a" );	# static, not dynamic lib
	@LIBPC_PROF = ( "$TOOL_LIB/libsqlptc_p.a" );	# static, not dynamic lib
}

#if ($RELVER ge "R77")
#{
#	@LIBPC = ();
#}


# PTS 1109770 (core dump when using libncurses.so.5x)
@LIBSCR = "/usr/lib/libncurses.a";
@LIBMATH = "-lm";
@DYNAMIC_LIB = ();
@LIBNET = ();
@LIBCSCI    = ();

#
# PTS 1002837 jrg 26.5.1999
@LIBMPROC = ();

# CTS 1106257
@LIBLD=();
if (( $RELVER ge "R72") && ($RELVER ne "RTOOL"))
{	@LIB_PTHREAD=("-lpthread"); }
else
{	@LIB_PTHREAD=(); }

# Check for new pthread library support
if ($RELVER eq "RTOOL") {
	$ENV{'__HAS_NPTL'} = 0;
} else {
	CCVers::setup_NPTL_support_linux ($CC, @CCFLAGS, @LDFLAGS, @LIB_PTHREAD);
}


@LIBAIO = ();



# &gar PTS 1106517
@LDSHMFLAGS=();

# jrg PTS 1110802
@LDSHMLIBS=();

#
# map portable lib names to platform specific lib names
#   dl: dynamic linking
#
%plibMapping = ();


%plibMapping = (
    dl => 'dl',
);

if ( $SQLVERSION eq "REL30" )  { $TRACETYPE = "tsp_trace" }
	else { $TRACETYPE = "tracetext" }


##########
$PATH_SEPARATOR=":";

###########
# $PREPROCESS_FLAGS
@PP_FLAGS=("-E","-Wp,-P");


################
# $ANT_FLAGS
@ANT_FLAGS=("-Dfile.encoding=ISO-8859-1");
$ANT_CALL = "$ENV{TOOL}/ant/bin/ant";

###########
if ($ENV{APOMAKE})
{
	$SYMBOL_DIR = "$INSTROOT/sap";
	$MAP_DIR    = "$INSTROOT/sap";
}
else
{
	$SYMBOL_DIR = "$INSTROOT/symbols";
	$MAP_DIR    = "$INSTROOT/Support/Mapfiles";
}

$SYMBOL_DIR = $ENV{FORCED_SYMBOL_DIR} if ($ENV{FORCED_SYMBOL_DIR});
$MAP_DIR = $ENV{FORCED_MAP_DIR} if ($ENV{FORCED_MAP_DIR});


#################################################
# vmake environment variables must be set by hand
#################################################

if ( ! $DEVENV ) {
#   $DEVENV = "YES";		# Pascal vsp-modules
    $DEVENV = "NO";		# Assembler or C vsp-modules
}
$ENV{DEVENV} = $DEVENV; 	# export $DEVENV



# ================================================================

$DLL_TYPE = "so";

	push @CCFLAGS, "-fPIC";
$ENV{DLL_TYPE} = $DLL_TYPE;		# can be exported even if not set

$ARCH_TYPE = ".a";
$ENV{ARCH_TYPE} = $ARCH_TYPE;		# can be exported even if not set

#################################################
#  IGNORE following strings in output
#  separate with "|"
#################################################

# string for compile
#
$CC_IGNORE="";

# string for link
$LNK_IGNORE="";

sub setvar {
    my ($varname, $commonDefault, $internalDefault) = @_;
    if (!defined ($internalDefault)) {
        $internalDefault = $commonDefault;
    }
    my ($value);
    if ($ENV{$varname}) {
        $value = $ENV{$varname};
    }
    else {
        if ($ENV{SAPDB_INTERNAL}) {
            $value = $internalDefault;
        }
        else {
            $value = $commonDefault;
        }
        $ENV{$varname} = $value;
    }
    eval '$' . "$varname = '$value'";
}

setvar ('PYTHON', 'NO', "$TOOL/Python");
# setvar ('PYTHON_INCLUDE', $ENV{PYTHON} . '/include');
if ( ($^O =~ /win32/i) && ( ${'BIT64'} =~ /1/ ))
{ setvar ('PYTHON_INCLUDE', $ENV{'TOOL'} . '/incl/extern/python/sdk-py21'); }
else
{ setvar ('PYTHON_INCLUDE', $ENV{'TOOL'} . '/incl/extern/python/sdk-py15'); }

setvar ('ACTIVEPERL_INCLUDE', 'NO', 'NO');
setvar ('ACTIVEPERL_LIB', 'NO', 'NO');
setvar ('COREPERL_INCLUDE', $Config{'archlib'} . "/CORE");
setvar ('COREPERL_LIB', $Config{'archlib'} . "/CORE");
setvar ('JDK_INCLUDE', 'NO', "$TOOL/java/include");
setvar ('JAVA_HOME', 'NO', "$TOOL/java");

unless (defined $ENV{INSTLIB}) {
	require GetInstDir;
	$ENV{INSTLIB} = GetInstDir::GetInstLib ();
}

unless (defined $ENV{INSTMISC}) {
	require GetInstDir;
	$ENV{INSTMISC} = GetInstDir::GetInstMisc ();
}

unless (defined $ENV{INSTLIBRELOC}) {
	require GetInstDir;
	$ENV{INSTLIBRELOC} = GetInstDir::GetInstLibReloc ();
}

unless (defined $ENV{INSTMISCRELOC}) {
	require GetInstDir;
	$ENV{INSTMISCRELOC} = GetInstDir::GetInstMiscReloc ();
}

unless (defined $ENV{INSTRUNTIME}) {
	require GetInstDir;
	$ENV{INSTRUNTIME} = GetInstDir::GetInstRuntime ();
}

unless (defined $ENV{INSTRUNTIMEBIN}) {
	require GetInstDir;
	$ENV{INSTRUNTIMEBIN} = GetInstDir::GetRuntimeBin ();
}

unless (defined $ENV{SAPDBSDK}) {
	require GetInstDir;
	$ENV{SAPDBSDK} = GetInstDir::GetSDKRoot ();
}

unless (defined $ENV{SAPDBSDKLIB}) {
	require GetInstDir;
	$ENV{SAPDBSDKLIB} = GetInstDir::GetSDKRootLib ();
}

unless (defined $ENV{SAPDBSDKPGM}) {
	require GetInstDir;
	$ENV{SAPDBSDKPGM} = GetInstDir::GetSDKRootPgm ();
}

$TraceFile = "$WRK/$VMAKE_VERSION/prot/trace.prot";
sub ToolTrace {
	# usage ToolTrace($string, \@daten)
	if ($ENV{TOOLTRACE}) {
		local($string, $r_daten) = @_;
		open(TOOLTRACE, ">>$TraceFile") ||
			die "Can't open $TraceFile (append): $!\n";
		print TOOLTRACE sprintf($string, @$r_daten);
		close(TOOLTRACE);
	}
}

#
# use variables from RTE_Configure.pl (DanielD)
#
if (-r "$WRK/config/toolvars_configure.pl") {
    do "$WRK/config/toolvars_configure.pl";
}

#
# Override toolvars variables through env variable
#    example: export TOOLVARS_PATCH="OS=WIN32;OSSPEC=WIN32;MACH=I386"
#    useful to generate vmake dependencies for any platform without
#    remote compilation (DanielD)
#
if ($ENV{TOOLVARS_PATCH}) {
    my @entries = split (';', $ENV{TOOLVARS_PATCH});
    foreach my $entry (@entries) {
        if ($entry =~ /(.*)=(.*)/) {
            eval '$' . $1 . "=" . $2 . ";";
            $ENV{$1} = $2;
        }
    }
}

#
# Search for toolvars overrides (DanielD)
#
foreach my $toolvarsext (("$TOOL/bin", "$OWN")) {
    if (-r "$toolvarsext/toolvarsext.pl") {
        do "$toolvarsext/toolvarsext.pl";
    }
}

# initializing flush of output buffer
$|=1; print "";

