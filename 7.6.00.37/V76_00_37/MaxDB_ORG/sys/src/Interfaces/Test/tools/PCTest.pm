#!/usr/bin/perl
# PCTest.pm 
#
# @(#)PCTest.pm  7.4.4.1  2003-02-28 
#
# runs Precompiler Testfiles and differs results
# 
# Burkhard Diesing
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2001-2005 SAP AG
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

package PCTest;

#Settings for HTML Support
$HTMLProt=1; # Set to one if the enable html prot. This will generate htmltags to the differences 
$HTMLHeader="\<html\>\<head\>\<meta http-equiv=\"Content-Type\" content=\"text/html\"\> \<meta http-equiv=\"expires\" content=\"28800\"\> \</head\>\<body \>\<PRE\>";
$HTMLErrorTagBegin="\<b\>\<font color=\"#FF0000\"";
$HTMLErrorTagEnd="\</font\>\</b\>";
$HTML_EXT=".html";

&if $OSSPEC in [ WIN32 ]
$EXEC_EXT=".exe";
&else
$EXEC_EXT="";
&endif
$Timeout=300; #Timeout for program execution in seconds 
$IsSema=0;
$PROTFILE="pctest";
$PROTFILE_EXT=".prot";
&if $OSSPEC in [ WIN32 ]
$CC="cl";
$CPP="cl";
&elif $OSSPEC in [ OSF1 ]
$CC="cc";
$CPP="cxx";
&else
$CC="cc";
$CPP="cc";
&endif

use Env;
$Trace=1 if ($ENV{PCTOOL_TRACE});
$Trace++ if ($Trace);
use Purify;
use FileHandle;
use File::Copy;
use Carp;

sub IsInitProg;
sub PCTest_init; #Constructor for PCTest.pm.tt
sub cleanup;
sub cleandir;
sub date;
sub _system;
sub _unlink;
sub _moved2u;
sub WriteProt;
sub WriteProtAscii;
sub WriteErrorProtHTML;
sub WriteProtHTML;
sub WriteTestDiffers;
sub WriteTestOutDiffers;
sub WriteIgnoreDiffbyOption;
sub WriteTestOk;
sub WriteTestKeep;
sub WriteTestNewProt;
sub WriteTestG;
sub WriteTimeOutReached;
sub _mkdir;
sub _diffcheck;
sub _ausgabe;
sub _getprotext;
sub _getolddir;
sub _getwrkdir;
sub _doperl;
sub getTestDir;
sub MakePCProt;
sub MakeSTDOUTProt;
sub MakeTRACEProt;
sub GetOWN;
sub GetOldDir;
sub GetWrkDir;
sub GetPrefix;
sub GetDSNFromInclude;
sub RegisterDriver;
sub _GetOldProt;
sub MakeCompactProt;
sub GetPCProtFile;
sub GetPCHTMLProtFile;
sub errorUSAGE;
sub printProtDir;
sub _getprotdir;
sub HTMLOff;
$_HTMLProt = 0;
sub HTMLOn;
sub ListOnStdOut;
sub ListOnProt;
sub exec_with_timeout; #executes a external programm and stop it after given timeout is reached
sub SetSema;
sub ResetSema;
sub WriteCantResetSema;
sub WriteCantSetSema;
sub WriteWaitSema;
$\="\n";  # Output seperator fuer print */
$USAGE ="USAGE: PCTest [precomoptions] [options] <dirname>/<filename>[-<variantspec>$FNAME[$k]]\n";
$USAGE.="\n";
$USAGE.=" <dirname> specifies the testlayer and <filename> the name of the testfile.\n";
$USAGE.=" No Extension is needed for <filename>. PCTest will try to build one by itselve.\n";
$USAGE.=" Use the <variantspec> to run the same test against different serverdbs or with\n";
$USAGE.=" different options. Note if you use <variantspec> the reference protocol must\n";
$USAGE.=" have the same name as the test including <variantspec>.\n";
$USAGE.="\n";
$USAGE.="   \noptions  :\n";
$USAGE.="     -cleanup        : Clean the output from previous runs.\n";
$USAGE.="     -keep           : Keep all intermediate file.\n";
$USAGE.="     -dynamic        : Link with shared libraries. (UNIX)\n";
$USAGE.="     -nocore         : Switch to debugger in case of a core.\n";
$USAGE.="     -debug          : Include debug information and sets\n";
$USAGE.="                       the -nocore and -keep option.\n";
$USAGE.="     -nobind         : Do not build an executable\n";
$USAGE.="     -purify         : Start the test under Purify control\n";
$USAGE.="     -norun          : Do not start the executable.\n";
$USAGE.="     -nohtml         : Do no generate Protfile in HTML.\n";
$USAGE.="     -cprot          : Write compiler messages to prot.\n";
$USAGE.="     -only <modul>   : Runs only if modul equal filename.\n";
$USAGE.="     -config <file>  : Reads database data from file instead of from\n";
$USAGE.="                       environment.\n";
if ( $ENV{RELVER} gt "R72" ) { 
$USAGE.="     -profile        : Prepare <filename> for profiling.\n";
$USAGE.="                       Use pcprof to run the prepared file.\n";
}
$USAGE.="     -l<linkflags>   : Flags passing to the linker.\n";
$USAGE.="     -v<variantspec> : same as <variantspec> but overwrites.\n";
$USAGE.="                       the <variantspec> argument.\n";
$USAGE.="     -ignore <number>: Ignore warning from differences.\n";
$USAGE.="                       1 = stdout, 2 = trace, 4 = pclist.\n";
$USAGE.="                       Combinations are possible (Bitwise or 6 ignores all).\n";
$USAGE.="     -U              : register Unicode ODBC-driver (sqlod32w.dll)\n";

if ($ENV{PCNOEXE} eq "y") {
    $NOEXE = 1;
    $RUNPROG="No";
} else {
    $NOEXE = 0;
}

if ($NOEXE == 1) {
    if ($ENV{TMP}) {
        $HOME_BAK = $ENV{HOME};
        $ENV{HOME} = $ENV{TMP};
    }
}

$TMPFILE=".temp";
$ERRFILE=".error$$";
$SEMAFILE="$HOME/.sapdbsema";
$OPT="";
$DRIVERDLL="";
$OLDOPT="";
$LFILES="";
$PREA_ODBC="Lo";
$j=1;
$ERROR_OK=0;
$ERROR_NOTOK=1;
$ERROR_OCCURED=$ERROR_OK;
$ERROR_USAGE=-1;
$ERROR_LINKING=-2;
$ERROR_CORE=-3;
$ERROR_COMPILING=-4;
$ERROR_PRECOMPILING=-5;
$ERROR_RUNNING=-6;
$ERROR_TIMEOUT=-7;
$ERROR_OUTDIFF=-8;
$ERROR_INTERNAL=-99;
# this errorcodes can occured as the same time (use bitwise OR)
$ERROR_OUTFILE_DIFFER  =0x00000001; 
$ERROR_TRACEFILE_DIFFER=0x00000002;
$ERROR_LISTFILE_DIFFER =0x00000004;

# --- begin perl script ---
sub pctest
{
print @_ if ( $Trace );

$CProt=0; # Set to one to pipe the compiler output to the prot or set the 
# -cprot option
 
local ( $DSN );
local ( @ARGV ) = @_;
local ( @FNAME, @FDIR, @LFILES, $OLDOPT, @OPT, $PREF );

errorUSAGE if (@ARGV < 1);
$j=1;
$COMMENT=0;

do $TOOLVARS;

print "SAPDBSDK=$ENV{SAPDBSDK}" if ($Trace);
local ( $TESTDIR ) = getTestDir;
print "TESTDIR=".$TESTDIR if ($Trace);
local ( $configfile ) = "$TESTDIR/config/default.cfg" if ( -f "$TESTDIR/config/default.cfg" );

if (defined $ENV{TOOLSHELL}) {
    $PERLCMD="$ENV{TOOLSHELL} -S"
}
else {
    WriteProt (date()." ===\> ERROR: Environment variable \$TOOLSHELL not set.");
    return $ERROR_USAGE;
}
if ( !defined $OWN ) {
    print "getOWN=".GetOWN."\n" if ($Trace);
    $OWN = GetOWN;
}
if (!defined $ENV{ISWDFNACHT}) {
    $ENV{SAPDBINSTKEY}=$INSTROOT;
    print "SAPDBINSTKEY=$ENV{SAPDBINSTKEY}" if ($Trace);
}
$CPCLNK="cpclnk";
&if $OSSPEC in [ WIN32 ]
&else
if ( $ENV{RELVER} gt "R75" ) { 
   $CPCLNK.=" -P";
}
&endif

$CPC64="";
if ( $ENV{RELVER} gt "R73" ) { 
    if ( $ENV{RELVER} eq "R74" && $ENV{CORRECTION_LEVEL} eq 1 ) {
        $CPC64="";
    }
    else {
        $CPC64="-BIT64" if (defined $ENV{BIT64});
    }
}
if (-x "$TESTDIR/bin/upddsn$EXEC_EXT") {
    $UPDDSN = "$TESTDIR/bin/upddsn$EXEC_EXT";
}
else {
    $UPDDSN = "upddsn$EXEC_EXT";
}
print "UPDDSN=$UPDDSN" if ($Trace);

foreach $i (@ARGV) {
    if ($i =~ m!//! || $COMMENT eq 1) {
#       all arguments after a // are ignored
        $COMMENT=1;
        print "ignore ARG=$i" if ($Trace);
        next;        
    }
    if ($i =~ /.*-nohtml*/ ) {
        $HTMLProt=0;
    }
    if ($i =~ /.*-config(.*)/ ) {
        if ( $1 ne "" ) {
            $configfile="$1";
            print "configfile2=$configfile\n" if ($Trace);
        }
        next;
    }    
# wenn -only gesetzt ist, nachfolgendes Argument merken
    elsif ($i =~ /.*-only*/ ) {
	    $ONLY="INIT";
	    $\="";
	    print "ONLY=$ONLY" if ($Trace);

        if ($i =~ /.*-only\s*\w+.*/)
        {
            $ONLY="YES";
	        print "ONLY=$ONLY" if ($Trace);
	        ($ONLYFILE)= $i =~ m!.*/(.*)!;  
            ($ONLYDIR) = $i =~ m!.*\s+(\w+.*)/.*!;
        }
    }

    elsif (defined $ONLY && $ONLY eq "INIT")
    {
        $ONLY="YES";
	    print "ONLY=$ONLY" if ($Trace);
	    ($ONLYFILE)= $i =~ m!.*/(.*)!;  
        ($ONLYDIR) = $i =~ m!(.*)/.*!;
    }
    else
    {	
    
        if ( ($i =~ m/-.*/) || !($i =~ m/.*\/.*/) ) {
	        $OPT[$j] .= " ".$i;
        }
        else {
	        ($FNAME[$j]) = $i =~ m!.*/(.*)!;
	        ($FDIR[$j]) = $i =~ m!(.*)/.*!;
	        $LFILES[$j] = $FNAME[$j];
	        print $FNAME[$j].";".$FDIR[$j].";".$OPT[$j] if ($Trace);
	        $j++;
	        $OPT[$j]="";
        }
        $OLDOPT=$i;
        }
    }
        if ( length $FNAME[1] > 33 ) {
        print "Filename for the executable program has more than 32 letters";
        exit 4700;
}

_mkdir("$TESTDIR"); 
_mkdir("$TESTDIR/wrk");
_mkdir("$TESTDIR/dif");
_mkdir("$TESTDIR/old");

chdir $TESTDIR."/wrk";

local ( $KEEP, $CLEANUP, $COREPROT, $PRECOMP );
local ( $NOBIND );
local ( $CCFLAGS, $LFLAGS );

$PRECOMP="No";
$CLEANUP="No";
$KEEP="No";

$CCFLAGS.="-D$OSSPEC "; 
if ( ($ENV{RELVER} eq "R73" && $ENV{CORRECTION_LEVEL} gt 0)
     || ($ENV{RELVER} ge "R74" && $ENV{CORRECTION_LEVEL} ge 3)) { 
$CCFLAGS.="-I$OWN/usr/incl "; 
}
else {
$CCFLAGS.="-I$OWN/usr/incl -I$OWN/sys/wrk/incl -I$OWN/sys/wrk/incl/SAPDB "; 
}

# Wdf-tests on customer like installation
if (defined $ENV{INDEPPATH}) {
    $CCFLAGS.="-I".$ENV{INSTROOT}."/incl ";
    $CCFLAGS.="-I".$ENV{INDEPPATH}."/incl ";
}
else {
    $ENV{INDEPPATH}=$ENV{INSTROOT};
}

if ( -f "$TESTDIR/lib/cpct.lib" ) {
    rename "$TESTDIR/lib/cpct.lib", "$TESTDIR/lib/cpct$ARCH_TYPE";
}

&if $OSSPEC in [ LINUX ]
$ADDLIBS="-lm -ldl";
$LFLAGS.="../lib/cpct$ARCH_TYPE ";
$CCFLAGS.="-D$MACH ";
&if $MACH == SDBonPPC64
$CCFLAGS.="-m64 ";
&endif
&endif
&if $OSSPEC in [ OSF1 ]
$CCFLAGS.="-pthread ";
$ADDLIBS="-lm ";
$LFLAGS.="../lib/cpct$ARCH_TYPE ";
&endif
&if $OSSPEC in [ WIN32 ]
$LFLAGS.="../lib/cpct$ARCH_TYPE ";
&endif
&if $OSSPEC = HPUX
&if $MACH == HP_IA64
$LFLAGS.="../lib/cpct$ARCH_TYPE ";
&else
$LFLAGS.="-lm ../lib/cpct$ARCH_TYPE ";
&endif
$CCFLAGS.="-Ae "; 
if (defined $ENV{BIT64}) {
&if $MACH == HP_IA64
    $CCFLAGS.="+DD64 ";
&else
    $CCFLAGS.="+DA2.0W +DS2.0 ";
&endif
}
&elif $MACH == SUN || $OSSPEC == SOLARIS
$LFLAGS.="-lm ../lib/cpct$ARCH_TYPE "; 
if (defined $ENV{BIT64}) {
&  if $MACH in [ SPARC, SUN ]
	$CCFLAGS.="-xarch=v9 ";
&  elif $MACH == X86_64
	$CCFLAGS.="-xarch=amd64 "; 
&  endif
} else {
	$CCFLAGS.=""; 
}
&elif ($OSSPEC = SVR4 && $MACH = NMP)
if (defined $ENV{BIT64}) {
	$CCFLAGS.="-Klp64 "; 
}
$LFLAGS.="-lm ";
&elif $OSSPEC = AIX
$LFLAGS.="-lm ../lib/cpct$ARCH_TYPE";
&endif
$VARSPEC="";
$RUNPROG="Yes";
$COREPROT="Yes";

# Komandozeilen-Optionen auswerten
local($actarg) = -1;
foreach $_ (@ARGV) {
    $actarg += 1;
    print "$_=".$_ if ($Trace);
    if ( /.*-help */ || /.*-\?.*/ ) { 
	print errorUSAGE;
	exit 1;
    }
    if ( /.*-c */ ) { 
	$PRECOMP="Yes";
	print "PRECOMP=$PRECOMP" if ($Trace);
    }
    if ( /.*-nobind*/ ) { 
	$NOBIND="Yes";
	print "NOBIND=$NOBIND" if ($Trace);
    }
    if ( /.*-cleanup*/ ) { 
	$CLEANUP="Yes";
	print "CLEANUP=$CLEANUP" if ($Trace);
    }
    if ( /.*-keep*/ ) { 
	$KEEP="Yes";
	print "KEEP=".$KEEP if ($Trace);
    }
    if ( /.*-nocore*/ ) { 
	$COREPROT="No";
	print "COREPROT=$COREPROT" if ($Trace);
    }
    if ( /.*-debug*/ || /.*-profile*/ ) { 
&if $OSSPEC in [ WIN32 ]
	$CCFLAGS.="-Zi -Od";
	$LFLAGS.="/DEBUG /Zi ";
&else
	$CCFLAGS.="-g ";
&endif
        $COREPROT="No";
        $KEEP="Yes";
        if (/.*-profile*/ ) { 
	    print "$ENV{RELVER}" if ($Trace);
	    if ( $ENV{RELVER} eq "R72" ) {
		$LFLAGS.="/PROFILE ";
		$PROFILE="Yes";	    
	    }
	    else {
		die "Option -profile only alowed in R72.";
	    }
        }
	print "CCFLAGS=$CCFLAGS" if ($Trace);
    }
    if ( /.*-dynamic*/ ) { 
	$DYNAMIC="Yes";
    }
    if ( /.*-purify*/ ) {
	if ($RUNPROG !~ /No/) {
            $RUNPROG="purify";
            print "RUNPROG=$RUNPROG" if ($Trace);
        }
    }
    if ( /.*-norun*/ ) { 
	$RUNPROG="No";
	print "RUNPROG=$RUNPROG" if ($Trace);
    }
    if ( /-cprot/ ) {
        $CProt = 1;
    }
    if ( /(-I.*)/ ) {
        local($OPT);
# switch '\' to '/' in include option
        $OPT = $1;
        $OPT =~ tr!\\!/!;
	$CCFLAGS.="$OPT ";
    }
    if ( /-l(-l.*)/ ) {
        local($OPT);
# switch '\' to '/' in linkoption
        $OPT = $1;
        $OPT =~ tr!\\!/!;
	$ADDLIBS.="$OPT ";
        print "\$ADDLIBS=$ADDLIBS\n" if ($Trace);
    }
    elsif ( /-l(.*)/ ) {    
        local($OPT);
# switch '\' to '/' in linkoption
        $OPT = $1;
        $OPT =~ tr!\\!/!;
	$LFLAGS="$OPT "."$LFLAGS ";
        print "\$LFLAGS=$LFLAGS\n" if ($Trace);
    }
    if ( /-v(.*)/ ) { 
	$VARSPEC.="$1";
    }
    if ( /-ignore(.*)/ ) { 
	$IGNORERC.="$1";
        if ($IGNORERC eq "") {
            $IGNORERC = $ARGV[$actarg+1];
        }
        print "\$IGNORERC=$IGNORERC\n"  if ($Trace);
        errorUSAGE "Invalid option argument -ignore ($IGNORERC)." unless ($IGNORERC =~ m/[0-9]/ );
    }
    next;    
} #while

if ( length $FNAME[1] == 0 ) {
    errorUSAGE "No filename was specified";
}

# wenn only gesetzt ist, und der Paramter von -only != Filename =>Programmabbruch

print "ONLY=$ONLY" if ($Trace);; 
print "$FNAME[1]=$ONLYFILE" if ($Trace);;
print "$FDIR[1]=$ONLYDIR" if ($Trace);;

if (defined $ONLY 
    && $ONLY eq "YES"
    && ($FNAME[1] ne $ONLYFILE
    ||  $FDIR[1]  ne $ONLYDIR) ) {
    
    return $ERROR_OK;
   
}
else {
  $\="\n";
}
$RETCODE=0;
$k=1;
#############################
#call contructor PCTest_init#
#############################
PCTest_init;
WriteProtAscii(date()." Start local pctest @_");
if (IsInitProg == 1) {
    WriteProt("RELVER = $ENV{RELVER}");
    WriteProt("CORRECTION_LEVEL = $ENV{CORRECTION_LEVEL}");
    WriteProt("OSSPEC = $OSSPEC");    
    WriteProt("MACH = $MACH");    
    WriteProt("ISWDFNACHT = $ENV{ISWDFNACHT}");    
    WriteProt("SERVERDB = $ENV{SERVERDB}");
    WriteProt("SERVERNODE = $ENV{SERVERNODE}");
    WriteProt("SUPERUID = $ENV{SUPERUID}");
    WriteProt("SUPERPWD = $ENV{SUPERPWD}");
    WriteProt("SAPDBINSTKEY = $ENV{SAPDBINSTKEY}");
    WriteProt("SAPDBSDK = $ENV{SAPDBSDK}");
    WriteProt("HOME = $HOME");
    WriteProt("OWN = $OWN");
    WriteProt("TEST_ROOT = $ENV{TEST_ROOT}");
    WriteProt("JTEST_ROOT = $ENV{JTEST_ROOT}");
    WriteProt("TESTDIR = $TESTDIR");
    WriteProt("INDEPPATH = $ENV{INDEPPATH}");
    WriteProt("INSTROOT = $ENV{INSTROOT}");
    WriteProt("PATH = $ENV{PATH}");
    WriteProt("LD_LIBRARY_PATH = $ENV{LD_LIBRARY_PATH}");
    WriteProt("PERL5LIB = $ENV{PERL5LIB}");
    WriteProt("CC = $CC");
    WriteProt("CPP = $CPP");
    WriteProt("CCFLAGS = $CCFLAGS");
    WriteProt("LFLAGS = $LFLAGS");
    WriteProt("BIT64 = $ENV{BIT64}");
}

while ( $k < $j ) {
    $OPT=$OPT[$k];    
    $LAY=$FDIR[$k];
    $PREF=GetPrefix ($LAY);

    $TESTNAME = $FNAME[$k];
    if ($TESTNAME =~ m/(.*)(-.*)/) {    
      $TESTNAME = $1 if ($1);
      if ($VARSPEC eq "") {
        $VARSPEC = $2 if ($2);
      }
      else {
        # -v was set ignore -<variantspec>
        $FNAME[$k]=$TESTNAME;
      }
    }
    print ("\$TESTNAME=$TESTNAME, \$VARSPEC=$VARSPEC\n") if ($Trace);
    print ("Reference prot: $TESTDIR/old/$PREF.$FNAME[$k].cpo\n") if ($Trace);
    if ( ! -f "$TESTDIR/old/$PREF.$FNAME[$k].cpo" ) {
        # $FNAME[$k] can contain a variantspec. If there is no oldfile for this variantspec 
        # run the test without the variantspec
        print ("no old variant of oldfile exsits use the general version\n") if ($Trace);
        $FNAME[$k] = $TESTNAME;
        $LFILES[$k] = $FNAME[$k];

    }

    print ("\$FNAME[$k]=$FNAME[$k]\n") if ($Trace);

    # cleanup old files from last run
    cleanup("$PREF", "$FNAME[$k]", "tol");
    if ( $CLEANUP =~ /Yes/ ) { 
        # do not run any test if option cleanup is set
        print "cleanup $FNAME[$k]";
        return $ERROR_OK;
    }

    $_=$LAY;

  SWITCH:{
      if (/^win$/ || /^odbc$/ || /^wac$/ ) {
	  $MMM="c"; last SWITCH;
      }
      if (/^c\+\+$/ || /^c\+\+sql$/ ) {
	  $MMM="C";last SWITCH;
      }
      if (/^CPC$/ ) {
	  $MMM="cpc";last SWITCH;
      }
      if (/^wa$/ ) {
          $MMM="cpp";last SWITCH;
      }
      $MMM=$LAY;;
  }
    copy("$TESTDIR/$LAY/$TESTNAME.$MMM", "$TESTDIR/wrk/$FNAME[$k].$MMM") || warn "Can't copy $TESTDIR/$LAY/$TESTNAME.$MMM to $TESTDIR/wrk/$FNAME[$k].$MMM";
    print ("copy $TESTDIR/$LAY/$TESTNAME.$MMM to $TESTDIR/wrk/$FNAME[$k].$MMM") if ($Trace);

    $_=$LAY;
#   sets a semaphore with timeout          
    SetSema $Timeout;
    WriteProtAscii(date()." Compile $FNAME[$k].$MMM");
  SWITCH:{
      if (/^c$/ || /^win$/ ) {
	  print "c";
	  callpc $FNAME[$k].$MMM;
	  last SWITCH;
      }
      if (/^c\+\+$/ || /^c\+\+sql$/ ) {
	  print "c++";
	  callpc $FNAME[$k].$MMM;
	  last SWITCH;
      }
      if (/^cpc$/ || /^CPC$/) {
	  if (/^CPC$/) {
	      $CCFLAGS .= "/TP ";
	      print "CPC";
	  }
	  else {
	      print "cpc";
	  }	     
          if (defined $configfile) {
              _doperl ("$TOOL/bin/pcmap$TOOLEXT -c$configfile -f$TESTDIR/config/cpc.flt $TESTDIR/incl/cpchead.h"); 
              _doperl ("$TOOL/bin/pcmap$TOOLEXT -c$configfile -f$TESTDIR/config/cpc.flt $TESTDIR/wrk/$FNAME[$k].$MMM"); 
          }
          else {
            if (!/^odbc$/ ) {
#         map strings in file to set the correct serverdb username combination
              _doperl ("$TESTDIR/bin/pcxuser$TOOLEXT cpc cpc");
              _doperl ("$TESTDIR/bin/pcdbmap$TOOLEXT $TESTDIR/incl/cpchead.h");
              _doperl ("$TESTDIR/bin/pcdbmap$TOOLEXT $TESTDIR/wrk/$FNAME[$k].$MMM");
            }
          }
	  
&if $OS in [ WIN32 ]
	  $CCFLAGS.="-DSQLEXCEPTION" if ( $COREPROT =~ /Yes/ );
&endif
          _unlink $ERRFILE;
          _unlink "$FNAME[$k].pcl";
          _unlink "$FNAME[$k].c" if (-f "$FNAME[$k].c");
          _unlink "$FNAME[$k].cpp" if (-f "$FNAME[$k].cpp");

          $cmd = "cpc $OPT $CPC64 $FNAME[$k]";
&if $OS in [ WIN32 ]
              $cmd .= " 2> $ERRFILE";
&else
              $cmd .= " > $ERRFILE 2>&1";
&endif
          if ($CProt) { WriteProt("$cmd"); } else { print("$cmd"); }
	  _system ("$cmd");
          ListOnStdErr("$ERRFILE");
          if ($CProt) {
              ListOnProt("$FNAME[$k].pcl") if (-f "$FNAME[$k].pcl");
          }
	  print "cpc returncode $?" if ( $Trace );
	  if ($? == 0 && $PRECOMP =~ /Yes/ ) {
	      $CEXT=".c";
	      if ( (! -f "$FNAME[$k].c") && -f "$FNAME[$k].cpp" ) {
		  $CEXT=".cpp";
                  $CC=$CPP;
	      }
              _unlink "$ERRFILE";
              if ( $ENV{RELVER} gt "R73" ) { 
                  $cmd = "$CC -c -I$ENV{SAPDBSDK}/incl -I$TESTDIR/incl -I$TESTDIR/incl/SAPDB $CCFLAGS $FNAME[$k]$CEXT";
              }
              else {
                  $cmd = "$CC -c -I$ENV{INDEPPATH}/incl -I$TESTDIR/incl $CCFLAGS $FNAME[$k]$CEXT";
              }
&if $OS in [ WIN32 ]
              $cmd .= " > $ERRFILE";
&else
              $cmd .= " > $ERRFILE 2>&1";
&endif
              if ($CProt) { WriteProt("$cmd"); } else { print("$cmd"); }
	      _system ("$cmd");
              ListOnStdErr("$ERRFILE");
              if ($CProt) {
                  ListOnProt("$ERRFILE") if (-f "$ERRFILE");
              }
	      if ($? == 0) {
		  _unlink "$FNAME[$k]$CEXT" unless ( $KEEP =~ /Yes/);
	      }		  
	  }
	  last SWITCH;
      }
      if (/^odbc$/ ) {
	  print "odbc";
	  if ( $DYNAMIC =~ /Yes/ ) { $LINKOPT="-b"; }
          _doperl("$TESTDIR/bin/pcdbmap$TOOLEXT $TESTDIR/incl/winhead.h");
          $DSN = GetDSNFromInclude ( "$TESTDIR/incl/winhead.h" );
&if $OSSPEC in [HPUX]
      $CCFLAGS.="-Ae";
&elif $MACH in [ NMP ]
	  $CCFLAGS.= "-Kschar";
&elif $OSSPEC in [ WIN32 ]
	  $CCFLAGS.="-DSQLEXCEPTION" if ( $COREPROT =~ /Yes/ );
&endif
          _unlink $ERRFILE;
          $cmd = "$CC -c -DSQLODBC -I$TESTDIR/incl $CCFLAGS $FNAME[$k].$MMM";
&if $OS in [ WIN32 ]
              $cmd .= " 2> $ERRFILE";
&else
              $cmd .= " > $ERRFILE 2>&1";
&endif

          if ($CProt) { WriteProt("$cmd"); } else { print("$cmd"); }
	  _system ($cmd);
          ListOnStdErr("$ERRFILE");
          if ($CProt) {
              ListOnProt("$ERRFILE") if (-f "$ERRFILE");
          }
	  last SWITCH;
      }
      if (/^wa$/ || /^wac$/ ) {
&if $OS in [ WIN32 ]
	$CCFLAGS.=" -DWIN32 ";
&endif
      if (/^wa$/ ) {
&if $MACH == SUN || $OSSPEC == SOLARIS
		print "using C++ compiler...";
		$CC=CC;
&endif
	    print "wa";
		$CCFLAGS.="-DSQLEXCEPTION" if ( $COREPROT =~ /Yes/ );
	  }
	  else {
		print "wac";
	  }
	  if ( $DYNAMIC =~ /Yes/ ) { $LINKOPT="-b"; }
          _unlink $ERRFILE;
          $cmd = "$CC -c -I$TESTDIR/incl $CCFLAGS $FNAME[$k].$MMM";
&if $OS in [ WIN32 ]
              $cmd .= " 2> $ERRFILE";
&else
              $cmd .= " > $ERRFILE 2>&1";
&endif
          if ($CProt) { WriteProt("$cmd"); } else { print("$cmd"); }
	  _system ($cmd);
          ListOnStdErr("$ERRFILE");
          if ($CProt) {
              ListOnProt("$ERRFILE") if (-f "$ERRFILE");
          }
	  last SWITCH;
      }      
  }
    
    $RETCODE=$?;
    if ( $RETCODE != 0 ) {
        
       if ($HTMLProt) {
        WriteErrorProtHTML(date()."\> ===\> ERROR AT (PRE)/COMPILATION OF '$FNAME[$k].$MMM");
       }
       HTMLOff;
       ListOnProt("$ERRFILE") if (-f "$ERRFILE");
       WriteProt(date()." ===\> ERROR AT (PRE)/COMPILATION OF '$FNAME[$k].$MMM");
       HTMLOn;
       WriteProt ("Testargs: @ARGV");
       ResetSema;
       return $ERROR_COMPILING;
    }
    else {
	 _unlink "$FNAME[$k].$MMM" unless ( $KEEP =~ /Yes/);
    }

    $k++;
} # while

$k=1;
$KKK=$FNAME[$k];
$LAY=$FDIR[$k];
$LINKSTR="";
while ( $k < $j ) {
   $STR=$FNAME[$k];
   $LINKSTR .= " ".$STR;
   $k++;
} 
if ($NOBIND =~ /Yes/ ) {
    ResetSema;
    return $ERROR_OK;
}
else {
$_=$LAY;
    WriteProtAscii(date()." Build $KKK");
 SWITCH:{
     if (/^c$/ || /^win$/ || /^cpc$/ ) {
	 if ($PROFILE eq "Yes") {
              $cmd = "$CPCLNK $CPC64 -SMT $ADDLIBS $LINKSTR $LFLAGS";
&if $OS in [ WIN32 ]
              $cmd .= " > $ERRFILE";
&else
              $cmd .= " > $ERRFILE 2>&1";
&endif
             if ($CProt) { WriteProt("$cmd"); } else { print("$cmd"); }
	     _system ($cmd);
	 }
	 else {
              $cmd = "$CPCLNK $CPC64 $ADDLIBS $LINKSTR $LFLAGS";
&if $OS in [ WIN32 ]
              $cmd .= " > $ERRFILE";
&else
              $cmd .= " > $ERRFILE 2>&1";
&endif
             if ($CProt) { WriteProt("$cmd"); } else { print("$cmd"); }
	     _system ($cmd);
	 }
         $RETCODE=$?;
         ListOnStdErr("$ERRFILE");
         if ($CProt) {
             ListOnProt("$ERRFILE") if (-f "$ERRFILE");
         }
	 last SWITCH;
     }
     if (/^CPC$/ || /^c\+\+$/ || /^c\+\+sql$/ ) {
         $cmd = "$CPCLNK $CPC64  $ADDLIBS $LINKSTR $LFLAGS";
&if $OS in [ WIN32 ]
         $cmd .= " > $ERRFILE";
&else
         $cmd .= " > $ERRFILE 2>&1";
&endif
         if ($CProt) { WriteProt("$cmd"); } else { print("$cmd"); }
         _system ($cmd);
         ListOnStdErr("$ERRFILE");
         if ($CProt) {
             ListOnProt("$ERRFILE") if (-f "$ERRFILE");
         }
	 last SWITCH;
     }
     if (/^odbc$/ ) {
&if $OS in [ WIN32 ]
&else
# transfer option -U to odbclnk
         $UNICODE = "";
         if ( $OPT =~ /-U(.*)/ ) {
           $UNICODE = " Unicode";
         }
&endif
         $cmd = "$TESTDIR/bin/odbclnk$TOOLEXT $LINKOPT $LINKSTR $LFLAGS $ADDLIBS" . $UNICODE;
&if $OS in [ WIN32 ]
         $cmd .= " > $ERRFILE";
&else
         $cmd .= " > $ERRFILE 2>&1";
&endif
         if ($CProt) { WriteProt("$cmd"); } else { print("$cmd"); }
	 _doperl ($cmd);
         $RETCODE=$?;
         ListOnStdErr("$ERRFILE");
         if ($CProt) {
             ListOnProt("$ERRFILE") if (-f "$ERRFILE");
         }
&if $OSSPEC in [ WIN32 ]
&else
         if ( -f "$HOME/.odbc.ini" ) {
             _unlink "$HOME/.odbc.ini";
         }
&endif
         if ( $OPT =~ /-U(.*)/ ) {
           $DRIVERDLL = "sqlod32w.dll";
         } else {
           $DRIVERDLL = "sqlod32.dll";
         }
         RegisterDriver("$DSN");
         if ( $OPT =~ /-X(.*)/ ) {
	     _system ("$UPDDSN \"$DSN\" TraceFileName $TESTDIR/wrk/sqltrace.pct");
	 }
	 else {
	     _system ("$UPDDSN \"$DSN\" TraceFileName");
	 }
	 last SWITCH;
     }
     if (/^wa$/ || /^wac$/ ) {
&if $OSSPEC in [ WIN32 ]
         $cmd = "$CC  $LINKSTR -link $LFLAGS $ADDLIBS watestlib.lib odbc32.lib wapi.lib dbfsapi.lib";
&else
&if ($OSSPEC in [SVR4] && $MACH == SUN) || $OSSPEC == SOLARIS
         $cmd = "$CC -o $LINKSTR $ADDLIBS $LFLAGS $LINKSTR.o $OWN/usr/lib/watestlib.a -L$OWN/usr/lib -lsqlod -lwapi -ldbfsapi -lsocket";
&else
         $cmd = "$CC -o $LINKSTR $ADDLIBS $LFLAGS $LINKSTR.o $OWN/usr/lib/watestlib.a -L$OWN/usr/lib -lsqlod -lwapi -ldbfsapi";
&endif
&endif    
&if $OS in [ WIN32 ]
         $cmd .= " > $ERRFILE";
&else
         $cmd .= " > $ERRFILE 2>&1";
&endif
         if ($CProt) { WriteProt("$cmd"); } else { print("$cmd"); }
         _system ($cmd);
         $RETCODE=$?;
         ListOnStdErr("$ERRFILE");
         if ($CProt) {
             ListOnProt("$ERRFILE") if (-f "$ERRFILE");
         }
	 last SWITCH;
     }     
 }

} # nobind

if ( $RETCODE || !-x "$TESTDIR/wrk/$KKK$EXEC_EXT" ) {
    if ($HTMLProt) {
      WriteErrorProtHTML(date()."\> ===\> ERROR AT LINKING THE FILE(S) WITH $KKK");
    }
    HTMLOff;
    ListOnProt("$ERRFILE") if (-f "$ERRFILE");
    WriteProt (date()." ===\> ERROR AT LINKING THE FILE(S) WITH $KKK");
    HTMLOn;
    WriteProt ("Testargs: @ARGV");
    ResetSema;
    return $ERROR_LINKING;
} else {
  if (!/^odbc$/ ) {
    _doperl ("$TESTDIR/bin/pcxuser$TOOLEXT cpc cpc");
  }
}

#remove old cores before starting the program
@CORE = <$TESTDIR/wrk/core*>;
foreach $CORE (@CORE) {
    _unlink "$CORE" unless ( $KEEP =~ /Yes/);
}

#start the testprogram
if ( $RUNPROG =~ /purify/ )
{
    if ($PROFILE eq "Yes") {
	print "Option profile is set. No execution of $KKK";
        ResetSema;
    	return 0;
    }
    else {
	print "Execute $LAY/$KKK";
        WriteProtAscii(date()." Execute $KKK$EXEC_EXT");
# for valgrind run, TTK
&if $OSSPEC in [ LINUX ]  and  $MACH in [ I386 ]
        _system ("~/ttk/opt/bin/valgrind  $TESTDIR/wrk/$KKK$EXEC_EXT 2&> $KKK.out");
        _system ("grep -v \"^==\" $KKK.out > $KKK.dou");
        _system ("grep \"^==\" $KKK.out > $KKK.val");
        _unlink "$KKK.out";
        Purify::EvaluateSingleValgrindResults ("$KKK");
&endif

&if $OS in [ WIN32 ]  and  $MACH in [ I386 ]
	_system ("purify /SaveTextData /ExcludeSystemDir=yes  $TESTDIR/wrk/$KKK$EXEC_EXT > $KKK.dou");
        Purify::EvaluateSingleResults ("$KKK");        
&endif
    }
    ListOnStdOut("$KKK.dou");
}
elsif ( $RUNPROG =~ /Yes/ ) 
{ 
    if ($PROFILE eq "Yes") {
	print "Option profile is set. No execution of $KKK";
        ResetSema;
    	return 0;
    }
    else {
	print "Execute $LAY/$KKK";
        WriteProtAscii(date()." Execute $KKK$EXEC_EXT");
#	_system ("$TESTDIR/wrk/$KKK$EXEC_EXT > $KKK.dou");
	$timeout_occured = exec_with_timeout ($TESTDIR.'/wrk/'.$KKK.$EXEC_EXT, $Timeout, $KKK.'.dou');
      
        if ($timeout_occured == $ERROR_TIMEOUT) {
          WriteTimeOutReached();
	}   
        if ( $ENV{RELVER} eq "R73" && $ENV{CORRECTION_LEVEL} eq 1 ) {
            if ($timeout_occured == $ERROR_RUNNING ) {
                if ($HTMLProt) {
                    WriteErrorProtHTML(date()."\> ===\> ERROR DURING RUN OF $KKK");
                }
                HTMLOff;
                WriteProt(date()." ===\> ERROR DURING RUN OF $KKK");
                $ERROR_OCCURED = $timeout_occured;
                HTMLOn;            
            }
        }
    }
    ListOnStdOut("$KKK.dou");
}
else {
   WriteProt(date()." Option -norun is set. Keep executable $KKK.");
   ResetSema;
   return 0; 
}
if ( -s  "$KKK.out") {
    _unlink "$KKK.dou" unless ( $KEEP =~ /Yes/);
}
else {
    if ( -f "$KKK.dou" ) {
	_moved2u ("$KKK.dou", "$KKK.out");
    }
    if ( -f "$KKK.pco" ) {
	_moved2u ("$KKK.pco", "$KKK.out");
    }
}

ResetSema;

if ( $COREPROT =~ /^Yes$/ ) {
    
    @CORE = <$TESTDIR/wrk/core*>;
    foreach $CORE (@CORE) {
        if ($HTMLProt) {
         WriteErrorProtHTML(date()."\> ===\> ERROR WITH A CORE-FILE FROM $KKK");
        }
        HTMLOff;
        WriteProt(date()." ===\> ERROR WITH A CORE-FILE FROM $KKK");
	HTMLOn;


        _unlink "$CORE" unless ( $KEEP =~ /Yes/);
        WriteProt ("Testargs: @ARGV");
        ResetSema;
    	return $ERROR_CORE;
    }
}

@FILES = ( "CALLTRAC.pct", "sqltrace.pct");
foreach $FILE (@FILES) 
    {
    if ( -f "$FILE" && ! -f "$KKK.pct" ) 
    {
	_moved2u ("$FILE", "$KKK.pct");
    }
}

    _unlink "$KKK$EXEC_EXT" unless ( $KEEP =~ /Yes/ );

$KK=$KKK;
$LL=$LAY;
$k=1; 
while ( $k < $j ) {
  $_=$LL;
  SWITCH:{
      $MM=$LL;
      if (/^win$/ || /^odbc$/ ) {
	  $MM="c";
          last SWITCH;
      }
      if (/^c\+\+$/ || /^c\+\+sql$/ ) {
	  $MM="C";
          last SWITCH;
      }
      $MM=$LL;
  }
&if $OSSPEC in [WIN32]
    _unlink "$KK.obj" unless ( $KEEP =~ /Yes/ );
&else
    _unlink "$KK.o" unless ( $KEEP =~ /Yes/ );
&endif
    _unlink "$KK.$MM" unless ( $KEEP =~ /Yes/ );
    $k++; 
    $KK=$FNAME[$k];
    $LL=$FDIR[$k];
}

# diffcheck

_GetOldProt ( "l", "o", "t" );

# pruefen des precompiler output

$INFILE="$KKK.pcl";
$OUTFILE="$KKK.lll";
if ( -f "$INFILE" ) {
    $temp=$\;$\="";
    open(FILE_IN, "<$INFILE") || die "Can't open $INFILE (input): $!";
    open(FILE_OUT, ">$OUTFILE") || die "Can't open $OUTFILE (output): $!";
    while(<FILE_IN>) {
        s/MySQL MaxDB MaxDB C-PreComp (.*)/SAP AG SAP DB C-PreComp $1/g;
 	s/(.*Version.*)[0-9]{1,}\.[0-9]{1,}\.[0-9]{1,}(.*Build )[0-9]{3}-[0-9]{3}-[0-9]{3}-[0-9]{3}(.*)$/$1X.X.X$2XXX-XXX-XXX-XXX$3/;
	s/PROGNAME//g;
	s/OPTIONUSER//g;
	s/SERVERDB//g;
	s/DEFAULTUSER//g;
	s/(: \()($ENV{SUPERUID})(\),.*)/$1XXX$3/g;
	print FILE_OUT $_;
    }
    close(FILE_OUT); close(FILE_IN);
    $\=$temp;
    _moved2u ("$OUTFILE", "$INFILE");
}

if (_diffcheck("$PREF.$KKK.l", "$KKK.pcl")) {
    if ($IGNORERC & $ERROR_LISTFILE_DIFFER) {
        WriteIgnoreDiffbyOption;
    }
    else {
        $ERROR_OCCURED |= $ERROR_LISTFILE_DIFFER;
    }
}

$INFILE="$KKK.out";
$OUTFILE="$KKK.ooo";
if ( -f "$INFILE" ) {
    $temp=$\;$\="";
    open(FILE_IN, "<$INFILE") || die "Can't open $INFILE (input): $!";
    open(FILE_OUT, ">$OUTFILE") || die "Can't open $OUTFILE (output): $!";
    while(<FILE_IN>) {
	s/^(.*GREPWEG\()(.*)(\).*$)/$1\*\*\*\*$3/g;
        if ($LAY=~/^odbc$/ ) {
            s/\[[\w\s]*\]\[[\w\s]*\]\[[\w\s]*\]/\[<vendor>\]\[<driver>\]\[<name>\]/g;
            s/\[[\w\s]*\]\[[\w\s]*\]/\[<vendor>\]\[<driver>\]/g;
        }
&if $OSSPEC in [ WIN32 ]
        s/(.*)(\(-709\):CONNECT: \(could not connect to socket \[.*\]\))/$1\(-709\):CONNECT: \(unknown host name\)/g; 
&else
        s/(.*)(\(-709\):CONNECT: \(cannot connect to server socket\))/$1\(-709\):CONNECT: \(unknown host name\)/g; 
        s/(.*)(\(-709\):CONNECT: \(database .*\))/$1\(-709\):CONNECT: \(database not running\)/g; 
        s/(.*)(\(-709\):CONNECT: \(unknown host \(see \/etc\/hosts\)\))/$1\(-709\):CONNECT: \(unknown host name\)/g; 
&endif
	print FILE_OUT $_;
    }
    close(FILE_OUT); close(FILE_IN);
    $\=$temp;
    _moved2u ("$OUTFILE", "$INFILE");
}

# pruefen des stdout

if (_diffcheck("$PREF.$KKK.o", "$KKK.out")) {
    if ($IGNORERC & $ERROR_OUTFILE_DIFFER) {
        WriteIgnoreDiffbyOption;
    }
    else {
        $ERROR_OCCURED |= $ERROR_OUTFILE_DIFFER;
    }
}

# pruefen der tracefiles

$j=0;
@SERVERS=();
foreach $MFILE (@LFILES) {
    $INFILE="$MFILE.pct";
    $OUTFILE="$KKK.ttt";
    if ( -f "$INFILE" ) {
        my $file_out = new FileHandle;
        my $file_in = new FileHandle;
        $IGNORELINE=0;
        local($VER[1]="0",$VER[2]="0",$VER[3]="0");
        local($BUILD[1]="000",$BUILD[2]="000",$BUILD[3]="000",$BUILD[4]="000");
	$temp=$\;$\="";
	open($file_in, "<$INFILE") || die "Can't open $INFILE (input): $!";
	open($file_out, ">$OUTFILE") || die "Can't open $OUTFILE (output): $!";
        LINE:
	while(<$file_in>) {
            if ($IGNORELINE) {
                $IGNORELINE=0;
                next LINE;
            }
	    s/(START)(.*):(.*)DATE(.*):(.*)TIME(.*):(.*)/$1:/g; 
	    s/(END)(.*):(.*)DATE(.*):(.*)TIME(.*):(.*)/$1:/g;
	    s/(PARSEID.*:)(.*)$/$1/g;
	    s/(PARSE AGAIN.*:)(.*)$/$1/g;
	    s/(PARAMETER DESCRIPTION:)(.*)$/$1/g;
            if (s/SQLERRD\(INDEX_5\).*$//g) {
                next LINE;
            }
	    s/PREP ID//g;
	    s/(Statement Name: :)(.*)/$1\*\*\*\*/g;
            s/(^\<html\>.*$)//g;
	    if (s/(SERVER\w+\s*: +)(\w+).*$/$1/g) {
              $SERVERS[$j]=$2;
              $j++;
            }
	    if (s/(VERSION *: )([0-9]{1,})\.([0-9]{1,})\.([0-9]{1,})(.*)//g) {
                $VER[1]=$2;$VER[2]=$3;$VER[3]=$4;
                print $file_out "$1X.X.X$5";                
            }
            if (s/(BUILD *: )([0-9]{3})-([0-9]{3})-([0-9]{3})-([0-9]{3})(.*)//g) {
                $BUILD[1]=$2;$BUILD[2]=$3;$BUILD[3]=$4;$BUILD[4]=$5;
                print $file_out "$1XXX-XXX-XXX$6";
            } 
            s/(PRODUCT *: )MaxDB C-PreComp Runtime/$1SAP DB C-PreComp Runtime/g;
	    s/(DRIVER *: ).*(libpcr)(.*)/$1\(...\)\/$2$3/g; 
            s/(OUTPUT :  PCR  :  C-PreComp )($VER[1]\.$VER[2]\.$VER[3])(    Build )($BUILD[1]-$BUILD[2]-$BUILD[3]-$BUILD[4])(.*)/$1X.X.X$3XXX-XXX-XXX/g;
            if (s/(OUTPUT :  LZU  :  )(.*)([0-9]{1,}\.[0-9]{1,}\.[0-9]{1,})(    Build )([0-9]{3}-[0-9]{3}-[0-9]{3}-[0-9]{3})(.*)//g) {
                print $file_out "$1XXX X.X.X$4XXX-XXX-XXX$6";
            }
            if (s/(OUTPUT :.*: Kernel    )([0-9]{1,}\.[0-9]{1,}\.[0-9]{1,})(    Build )([0-9]{3}-[0-9]{1,})(.*)//g) {
                
                print $file_out "$1X.X.X$3XXX-X$5";
                $IGNORELINE=1;
                print $file_out " ...   : XX-XXX-XXX\n";
            }
&if $OS in [ WIN32 ]
	    s/(SQLCODE:  -709       CONNECT: )\(could not connect to socket \[.*\]\)/$1\(unknown host name\)/g; 
&else
	    s/(SQLCODE:  -709       CONNECT: )\(database not accessible\)/$1\(database not running\)/g; 
	    s/(SQLCODE:  -709       CONNECT: )\(database not running: no request pipe\)/$1\(database not running\)/g; 
	    s/(SQLCODE:  -709       CONNECT: )\(cannot connect to server socket\)/$1\(unknown host name\)/g; 
	    s/(SQLCODE:  -709       CONNECT: )\(unknown host \(see \/etc\/hosts\)\)/$1\(unknown host name\)/g; 
	    s/(SQLCODE:  -715       Missing username or password for CONNECT \()database not acce/$1database not runn/g; 
	    s/(SQLCODE:  -715       Missing username or password for CONNECT \()database not acce/$1database not runn/g; 
            s/(INFO    : sqlarg3 failed. )\(-- illegal precompiler options\)/$1\(the USERKEY is unknown\)/g;
&endif
            if (defined $ENV{BIT64})
            {
# removes 64 Bit length from p04trorada
#              Nr.     T      L     TYPE   LEN         FRAC 
            s/([0-9] .*401 .*)24( .*[86] .*[0-9]{1,} .*0 .*)/${1}16${2}/g;
            s/([0-9] .*401 .*)24( .*34 .*[0-9]{1,} .*0 .*)/${1}16${2}/g;
            s/([0-9] .*1 .*)8( .*[0] .*[0-9]{1,} .*0 .*)/${1}4${2}/g;
            s/([0-9] .*51 .*)24( .*[86] .*[0-9]{1,} .*0 .*)/${1}16${2}/g;
            s/([0-9] .*51 .*)24( .*34 .*[0-9]{1,} .*0 .*)/${1}16${2}/g;
            }
        
	  SWITCH:{
	      if ($LAY=~/^odbc$/ ) {
		  s/(DATASOURCE:).*/$1/g;
		  last SWITCH;
	      }
	  }	    
               if ($_ ne "") {
                   print $file_out $_;
               }
	  NEXT:
	}
	close($file_out); close($file_in);
	$\=$temp;
	_moved2u ("$KKK.ttt", "$KKK.pct");
	if ( "$MFILE" ne "$KKK" ) {
	    _unlink "$MFILE.pct" unless ( $KEEP =~ /Yes/ );
	}
    }
}

if (_diffcheck( "$PREF.$KKK.t", "$KKK.pct")) {
    if ($IGNORERC & $ERROR_TRACEFILE_DIFFER) {
        WriteIgnoreDiffbyOption;
    }
    else {
        $ERROR_OCCURED |= $ERROR_TRACEFILE_DIFFER;
    }
}

_ausgabe ( @SERVERS );
WriteProt ("with $AUSGABE");
if ($ERROR_OCCURED ne $ERROR_OK)
{
    WriteProt ("Testargs: @ARGV");
}
    WriteProtAscii(date()." End $LAY/$KKK rc = $ERROR_OCCURED");
    print "\$ERROR_OCCURED=$ERROR_OCCURED\n" if ($Trace);
    if ($ERROR_OCCURED & $ERROR_OUTFILE_DIFFER) {
        return $ERROR_OUTDIFF;
    } else {
        return $ERROR_OCCURED;
    }
}

###################################################

# --- internal subroutines ---
sub IsInitProg
{
  my ($param)=@_ ;
  if(  $FNAME[$k] ne "initcpc"
     &&$FNAME[$k] ne "initodbc"
     &&$param     ne "init"){
    return 0;
  }     
  return 1;
}

# PCTest_init - Constructor #
sub PCTest_init {
  # garbage collection for protokoll files
  # the Constructor "PCTest_init" will only be executed 
  # if the initcpc/odbc programm was called 
  my ($param)=@_ ;
  if( IsInitProg == 0 ) {
    return;
  }     
  $modtime = (stat(GetPCProtFile()))[9];   
  @mtime = localtime( $modtime );
  @today = localtime();
  $spentdaytime = ($today[2]*3600 + $today[1]*60 +$today[0])/86400; # currently spent time of day in days
  @Days  = ("Su","Mo","Tu","We","Th","Fr","Sa");
  #delete protocols older than 7 days
  foreach $day (@Days) {
    if (-e GetPCProtFile($day) && int(-M GetPCProtFile($day))>=7) {    
      print "remove ",GetPCProtFile($day)."(.html)","\n" if ($Trace);
      _unlink GetPCProtFile($day);
      _unlink GetPCHTMLProtFile($day);
    } 
  } 
  #save current protocol if its not older than 7 days
  $HowOldIsProt = (-M GetPCProtFile());
  print "last modification before :",$HowOldIsProt," day(s)\n" if ($Trace);
  if ($HowOldIsProt < $spentdaytime) {
    #last modification of file occurs today    
    return;
  } 
  elsif ($HowOldIsProt < 7) {
    #copy to corresponding day protocol
    File::Copy::copy(GetPCProtFile(), GetPCProtFile($Days[$mtime[6]]));
    File::Copy::copy(GetPCHTMLProtFile(), GetPCHTMLProtFile($Days[$mtime[6]]));
    utime $modtime, $modtime, (GetPCProtFile($Days[$mtime[6]]));
    utime $modtime, $modtime, (GetPCHTMLProtFile($Days[$mtime[6]]));
  }
  print "remove ",GetPCProtFile()."(.html)","\n" if ($Trace);
  _unlink GetPCProtFile();  #delete old protokoll
  _unlink GetPCHTMLProtFile();  #delete old protokoll
}

sub cleandir
{
    _unlink "../$_[0]/$_[1].$_[2].[$_[3]]";
    return 0;
}

sub cleanup
{
    cleandir ("wrk", $_[0], $_[1], $_[2]);
    cleandir ("old", $_[0], $_[1], $_[2]);
    cleandir ("dif", $_[0], $_[1], $_[2]);
    return 0;
}

sub _moved2u
{
    print ( "d2u $_[0] $_[1]" ) if ($Trace);
    rename $_[0], $_[1];
    _doperl( "d2u$TOOLEXT $_[1]" );
}
sub _system
{
    print "$_[0]" if ($Trace);
    return system($_[0]);
}
sub _doperl
{
    local @tmp = @ARGV;
    local $lsep = $\;$\="";

    local @args = split (' ', $_[0]);
    local $perlsrc = @args[0];
    local $n = @args-1;
    @ARGV = @args[1..$n];
    print "@ARGV" if ($Trace);
    do $perlsrc;
    @ARGV = @tmp;
    $\=$lsep;
}

sub _unlink
{
    print "unlink $_[0]" if ($Trace);
    return unlink($_[0]);
}

sub WriteTestG
{
    $Prefix = $_[0];
    $Dir = $_[1];
    $FileName = $_[2];

    if ($HTMLProt) {
        WriteProtHTML(date()."$Prefix \<A href=\"$Dir/$FileName\"\>$FileName\</A\>");
    }
    HTMLOff;
    WriteProt(date()."$Prefix $FileName");
    HTMLOn;
}

sub HTMLOff
{
    if ($HTMLProt) {
        $_HTMLProt = $HTMLProt; # save old state
        $HTMLProt = 0;          # diable HTMLProt
    }
}

sub HTMLOn
{
    if ($_HTMLProt) {          # if HTMLProt was on
        $HTMLProt = $_HTMLProt; # restore the old values
        $_HTMLProt = 0;         
    }
}

sub WriteTestNewProt
{
    WriteTestG(" A new listing is produced of ", "../wrk", $_[0]);
}

sub WriteTestKeep
{
    WriteTestG(" Keep listing of ", "../wrk", $_[0]);
}

sub WriteTestOk
{
    WriteTestG("           Listing is O.K. of ", "../old", $_[0]);
}

sub WriteTestDiffers
{
    $FileName = $_[0];
    $Dir = "../dif";
    if ($HTMLProt) {
        $Prefix = "<span style=\"background-color: #FFFF00\"><font color=\"#008000\"> =\> DIFF \<= in the listing  of </font></span>";
        WriteProtHTML(date()."$Prefix \<A href=\"../dif/$FileName\"\>$FileName\</A\> <A href=\"../wrk/$FileName\"\>new\</A\> <A href=\"../old/$FileName\"\>old\</A\> ");
    }
    HTMLOff;
    $Prefix = " =\> DIFF \<= in the listing  of ";
    WriteProt(date()."$Prefix $FileName");
    HTMLOn;
}

sub WriteTestOutDiffers
{
    $FileName = $_[0];
    $Dir = "../dif";
    if ($HTMLProt) {
        $Prefix = "<b><font color=\"#FF0000\"> =\> ERROR DIFF \<= in the listing  of </font></b>";
        WriteProtHTML(date()."$Prefix \<A href=\"../dif/$FileName\"\>$FileName\</A\> <A href=\"../wrk/$FileName\"\>new\</A\> <A href=\"../old/$FileName\"\>old\</A\> ");
    }
    HTMLOff;
    $Prefix = " =\> ERROR DIFF \<= in the listing  of ";
    WriteProt(date()."$Prefix $FileName");
    HTMLOn;
}

sub WriteIgnoreDiffbyOption
{
    WriteTestG(" Difference is ignored by option argument.");
}

sub WriteErrorProtHTML
{
    WriteProtHTML("$HTMLErrorTagBegin$_[0]$HTMLErrorTagEnd");
}

sub WriteProtHTML
{
    if ($HTMLProt) {
        my $fileout = new FileHandle;
        if ( -s GetPCHTMLProtFile) {
            open($fileout, ">> ".GetPCHTMLProtFile);
        }
        else {
            open($fileout, "> ".GetPCHTMLProtFile);
            if ($HTMLProt) {
                print $fileout $HTMLHeader;
            }
        }        
        print $fileout $_[0];
        close($fileout);
    }
}

sub WriteProtAscii
{
    my $fileout = new FileHandle;
    open($fileout, ">> ".GetPCProtFile);
    print $fileout $_[0];
    close($fileout);
}

sub WriteTimeOutReached
{
   if ($HTMLProt) {
     WriteErrorProtHTML(date()."\> ===\> STOPPING EXECUTION OF $KKK BECAUSE TIMEOUT REACHED");
   }
   HTMLOff;
     WriteProt(date()." ===\> STOPPING EXECUTION OF $KKK BECAUSE TIMEOUT REACHED");
   HTMLOn;
}

sub _WriteProt
{
    WriteProtAscii($_[0]);
    WriteProtHTML($_[0]);
}

sub WriteProt
{
    print $_[0];
    _WriteProt($_[0]);
}

sub date
{
    $Year_index = \5;	#constant
    $Month_index = \4;	#constant
    $Day_index = \3;	#constant
    $Hour_index = \2;	#constant
    $Min_index = \1;	#constant
    $Sec_index = \0;	#constant
    @localtime = localtime(); 
    $localtime[$$Month_index]++; 
    $localtime[$$Year_index]+=1900;
    return sprintf("%02d:%02d:%02d %04d.%02d.%02d", $localtime[$$Hour_index], $localtime[$$Min_index], $localtime[$$Sec_index], $localtime[$$Year_index], $localtime[$$Month_index], $localtime[$$Day_index]);
}

sub _mkdir
{
    if (! -d $_[0] ) {
	print "_mkdir $_[0]" if ( $Trace );
&if $OSSPEC in [ WIN32 ]	
	mkdir ($_[0], 0);
&else
	mkdir ($_[0], 0775);
&endif
    }
}

sub _diffcheck
{
    $OLDFILE=$_[0];
    $NEWFILE=$_[1];
    local ($RC) = $ERROR_OK;
    print "_diffcheck $NEWFILE, $OLDFILE" if ( $Trace );

    if ( -f "../dif/$OLDFILE" ) {
	_unlink "../dif/$OLDFILE";
    }
    if ( -f "../old/$OLDFILE" && -f "$NEWFILE" ) {
&if $OSSPEC = HPUX
# on HPUX option -h does not work with -w     
	_system ("diff -bw ../old/$OLDFILE $NEWFILE > ../dif/$OLDFILE");
&else
	_system ("diff -bwh ../old/$OLDFILE $NEWFILE > ../dif/$OLDFILE");
&endif
    }
    else {
	if ( -f "../old/$OLDFILE" ) {
	    _system ("echo It exists an oldfile! The program did not work correctly! > ../dif/$OLDFILE"); 
	}
    }

    if ( -s "$NEWFILE" ) {
	rename $NEWFILE, $OLDFILE;
    } 
    else {
	_unlink "$NEWFILE" unless ( $KEEP =~ /Yes/ );
    }

    if ( -s "../dif/$OLDFILE" ) {
        if ($OLDFILE =~ /.*\.o$/) {
            WriteTestOutDiffers($OLDFILE);
        } else {
            WriteTestDiffers($OLDFILE);
        }
        $RC = $ERROR_NOTOK;
    }
    else {
	if ( -f "../dif/$OLDFILE" ) {
	    _unlink "$OLDFILE" unless ( $KEEP =~ /Yes/ );
	    _unlink "../dif/$OLDFILE" unless ( $KEEP =~ /Yes/ );
            WriteTestOk($OLDFILE);
	}
	if ( -f "$OLDFILE" ) {
	    if ( $KEEP =~ /Yes/ ) {
		WriteTestKeep($OLDFILE);
	    }
	    else {
		WriteTestNewProt($OLDFILE);
	    }
	}
    }
    return $RC;
}

sub _diffprot
{
    $OLDFILE=$_[0];
    $NEWFILE=$_[1];
    if ( -f "../old/$OLDFILE" && -f "$NEWFILE" ) {
	_system ("diff ../old/$OLDFILE $NEWFILE > ../dif/$OLDFILE");
    }
    else {
	if ( -f "../old/$OLDFILE" ) {
	    _system ("echo It exists an oldfile! The program did not work correctly! > ../dif/$OLDFILE"); 
	    $ERROR_OCCURED++;
	}
    }

    if ( -s "$NEWFILE" ) {
	rename $NEWFILE, $OLDFILE;
    } 
    else {
	_unlink "$NEWFILE" unless ( $KEEP =~ /Yes/ );
    }
}

sub _ausgabe
{
    $AUSGABE="";
    $I=0;
    local($oldNode) = "";
    local($oldServ) = "";
    foreach $i (@_) {
	print "$i" if ( $Trace );
	if ( $I == 0 ) {
            if ($oldNode ne $i) {
                $AUSGABE.=" db: $i";
            }
	    $I=1;
            $oldNode = $i;
	}
	else {
            if ($oldServ ne $i) {
                $AUSGABE.=" on node: $i";
            }
	    $I=0;
            $oldServ = $i;
	}
    }
}

sub GetPrefix
{
    $_=$_[0];
    local($PREF);
  SWITCH:{
      if (/^c$/ ) {
	  $PREF="Lc";
	  last SWITCH;
      }
      if (/^c\+\+$/ ) {
	  $PREF="LC";
	  last SWITCH;
      }
      if (/^c\+\+sql$/ ) {
	  $PREF="L+";
	  last SWITCH;
      }
      if (/^cpc$/ ) {
	  $PREF="c";
	  last SWITCH;
      }
      if (/^CPC$/ ) {
	  $PREF="C";
	  last SWITCH;
      }
      if (/^win$/ ) {
	  $PREF="Lw";
	  last SWITCH;
      }
      if (/^odbc$/ ) {
	  $PREF="Lo";
	  last SWITCH;
      }
      if (/^wa$/ || /^wac$/ ) {
	  $PREF="Wo";
	  last SWITCH;
      }
  }
  return ($PREF);
}

sub RegisterDriver
{
    local ($DSN) = $_[0];
    local ($PROT) = $_[1];
    $SERVERDB=$ENV{SERVERDB};
    $SERVERNODE=$ENV{SERVERNODE};
    local ($XX) = "";
    $XX="$XX SERVERDB" if (length $SERVERDB == 0);
    die "Env $XX not set.\n" if ( length $XX );
    
&if $OSSPEC in [ WIN32 ]
    local ($TMP)="\"$DSN\" Driver \"$ENV{INDEPPATH}\\pgm\\".$DRIVERDLL;
    WriteProt(date()." Register ODBC Driver $TMP") if ( $PROT ne "");
    _system("$UPDDSN $TMP");

    local ($TMP)="\"$DSN\" Setup \"$ENV{INDEPPATH}\\pgm\\sqlsp32.dll\"";
    _system("$UPDDSN $TMP");
&endif
    local ($TMP)="\"$DSN\" ServerDB \"$SERVERDB\"";
    _system("$UPDDSN $TMP");

    local ($TMP)="\"$DSN\" ServerNode \"$SERVERNODE\"";
    _system("$UPDDSN $TMP");
    $ENV{DBROOT} = $ENV{INDEPPATH};
}

sub GetDSNFromInclude
{
    local($DSN)="";
    $incfile=$_[0];
    open(FILE_IN, "$incfile") || die "Error: '$incfile' not found.";
    LINE: while(<FILE_IN>) {
	if ( /^#\s*define\s+DATASOURCENAME\s+"(.*)"/ ) {
	    $DSN=$1;
	    last LINE;
	}
    }
    close(FILE_IN);
    return $DSN;
}

sub MakeCompactProt
{
    print "===\> MakeCompactProt" if ( $Trace );
    local ($PREF, $FILE, @EXTENSION);
    $PREF = $_[0];shift;
    $FILE = $_[0];shift;
    @EXTENSIONS = @_;
    print "PREF=$PREF;FILE=$FILE;EXTENSION=@EXTENSIONS" if ( $Trace );

    local ($OLDDIR) = _getolddir;
    local ($PROTEXT) =_getprotext;
    local ($COMPACTFILE) = "$PREF.$FILE.$PROTEXT";
    local ($COMPACTFILEPATH) = "$OLDDIR/$COMPACTFILE";
    my($file_in) = new FileHandle;
    my($file_out) = new FileHandle;
    local ( $open ) = 0;
    foreach $EXTENSION (@EXTENSIONS) {
	$REFFILE="$PREF.$FILE.$EXTENSION";
	$f1="$OLDDIR/$REFFILE";
	print "\$f1=$f1" if ( $Trace );
	if ( -f "$f1" ) {
	    print "found $f1" if ( $Trace );
	    if (!$open) {
		print "create new compact protocol $COMPACTFILE" if ( $Trace );
		open($file_out, ">$COMPACTFILEPATH") || die "Can't open $COMPACTFILEPATH (output): $!";
		WriteProt(date()." A new compact protocol $COMPACTFILE will be generate");
		$open = 1;
	    }
	    open($file_in, "<$f1") || die "Can't open $f1 (output): $!";
	    WriteProt(date()." Adding $REFFILE to compact protocol $COMPACTFILE");
	    $temp=$\;$\="";
	    print $file_out 	"====== BEGIN COMPACT PROT $REFFILE;\n";
	    while (<$file_in>) {
		print $file_out $_;
	    }
	    print $file_out 	"====== END COMPACT PROT;\n";
	    close $file_in;	    
	    $\=$temp;
	}
    }
    if ($open) {
	close $file_out;
    }    
    print "\<=== MakeCompactProt" if ( $Trace );
}

sub _getprotext
{
    return ("cpo");
}

sub _getolddir
{
    return(getTestDir."/old");
}

sub _getwrkdir
{
    return(getTestDir."/wrk");
}

sub GetOldDir
{
    return(_getolddir);
}

sub GetWrkDir
{
    return(_getwrkdir);
}

sub GetOWN
{
    if ( defined $ENV{OWN} ) {
        return ("$ENV{OWN}");
    }
    else {
        if ( defined $ENV{TEST_ROOT} ) {
            return ("$ENV{TEST_ROOT}/..");
        }
        else {
            if ( defined $ENV{JTEST_ROOT} ) {
                return ("$ENV{JTEST_ROOT}/../..");
            }
        }
    }
    return (".");
}

sub printProtDir
{
    print _getprotdir;
}

sub _getprotdir
{
    return("$ENV{PROT_DIR}") if ($ENV{PROT_DIR} ne "");
    return(getTestDir."/wrk");
}

sub getTestDir
{
    local ($TESTDIR) = GetOWN."/test/pc";
    $TESTDIR =~ tr!\\!/!;
    return("$TESTDIR");
}

sub _GetOldProt
{
    local ( $PROTEXT ) =_getprotext;
    local ( $OLDDIR ) = _getolddir;
    local ( @EXTENSIONS ) = @_;
    print "_GetOldProt @EXTENSIONS" if ( $Trace );
    my $file_out = new FileHandle;
    my $file_in = new FileHandle;
    local ( $OLDPROT ) = "$OLDDIR/$PREF.$KKK.$PROTEXT";
#    File::Copy::copy( "$OLDPROT", "$OLDDIR" );
    if ( -f "$OLDPROT" ) {
	print "found $OLDPROT" if ( $Trace );
	$temp=$\;$\="";
	open($file_in, "<$OLDPROT") || die "Can't open $OLDPROT (input): $!";
	while(<$file_in>) {
	    if ( /^====== BEGIN COMPACT PROT (.*);$/ ) {
		$OUTFILE = "$1";
		open($file_out, ">$OLDDIR/$OUTFILE") || die "Can't open $OUTFILE (output): $!";
	      WRITEFILE: while(<$file_in>) {
		  if ( /^====== END COMPACT PROT;$/ ) {
		      close $file_out;
		      last WRITEFILE;
		  }
		  else {
		      print $file_out $_;
		  }
	      }
	    }
	}
	close $file_in;
	$\=$temp;
    }
    else {
	MakeCompactProt "$PREF", "$KKK", @EXTENSIONS ;
    }
}

sub GetPCHTMLProtFile
{
    return(_getprotdir."/$PROTFILE".$_[0].$PROTFILE_EXT.$HTML_EXT);
}

sub GetPCProtFile
{
    return(_getprotdir."/$PROTFILE".$_[0].$PROTFILE_EXT);
}

sub MakeTRACEProt
{
    local ($INFILE, $OUTFILE);
    $INFILE = $_[0];shift;
    $OUTFILE = $_[0];shift;
    my $file_out = new FileHandle;
    my $file_in = new FileHandle;
    if ( -s $INFILE ) {
	$temp=$\;$\="";
	open($file_in, "<$INFILE") || die "Can't open $INFILE (input): $!";
	open($file_out, ">$TMPFILE") || die "Can't open $TMPFILE (output): $!";
	while(<$file_in>) {
	    s/(START)(.*):(.*)DATE(.*):(.*)TIME(.*):(.*)/$1:/g; 
	    s/(END)(.*):(.*)DATE(.*):(.*)TIME(.*):(.*)/$1:/g;
	    s/(PARSEID.*:)(.*)$/$1/g;
	    s/(PARAMETER DESCRIPTION:)(.*)$/$1/g;
	    s/SQLERRD\(INDEX_5\).*$//g;
	    s/PREP ID//g;
	    s/(Statement Name: :)(.*)/$1\*\*\*\*/g;
	    if (s/(SERVER\w+\s*: +)(\w+).*$/$1/g) {
		$SERVERS[$j]=$2;
		$j++;
	    }
	    s/(BUILD *: )([0-9]{3}-[0-9]{3}-[0-9]{3})(.*)/$1XXX-XXX-XXX$3/g; 
	  SWITCH:{
	      if ($LAY=~/^odbc$/ ) {
		  s/(DATASOURCE:).*/$1/g;
		  last SWITCH;
	      }
	  }	    
	    print $file_out $_;
	}
	close($file_out); close($file_in);
	$\=$temp;
	_moved2u ("$TMPFILE", "$OUTFILE");
	if ( -f "$INFILE" ) {
	    _unlink "$INFILE" unless ( $KEEP =~ /Yes/ );
	}
    }
}

sub MakeSTDOUTProt
{
    local ($INFILE, $OUTFILE);
    $INFILE = $_[0];shift;
    $OUTFILE = $_[0];shift;
    $temp=$\;$\="";
    if ( -s $INFILE ) {
	open(FILE_IN, "<$INFILE") || die "Can't open $INFILE (input): $!";
	open(FILE_OUT, ">$TMPFILE") || die "Can't open $TMPFILE (output): $!";
	while(<FILE_IN>) {
	    s/(GREPWEG :.*)$//g;
	    print FILE_OUT $_;
	}
	close(FILE_OUT); close(FILE_IN);
	_moved2u ("$TMPFILE", "$OUTFILE");
	if ( -f "$INFILE" ) {
	    _unlink "$INFILE" unless ( $KEEP =~ /Yes/ );
	}
    }
    $\=$temp;
}


sub MakePCProt
{
    local ($INFILE, $OUTFILE);
    $INFILE = $_[0];shift;
    $OUTFILE = $_[0];shift;
    $temp=$\;$\="";
    if ( -s $INFILE ) {
	open(FILE_IN, "<$INFILE") || die "Can't open $INFILE (input): $!";
	open(FILE_OUT, ">$TMPFILE") || die "Can't open $TMPFILE (output): $!";
	while(<FILE_IN>) {
	    s/(.*Build )([0-9]{3}-[0-9]{3}-[0-9]{3}-[0-9]{3})(.*)$/$1XXX-XXX-XXX-XXX$3/g;
	    s/PROGNAME//g;
	    s/OPTIONUSER//g;
	    s/SERVERDB//g;
	    s/DEFAULTUSER//g;
	    print FILE_OUT $_;
	}
	close(FILE_OUT); close(FILE_IN);
	_moved2u ("$TMPFILE", "$OUTFILE");
	if ( -f "$INFILE" ) {
	    _unlink "$INFILE" unless ( $KEEP =~ /Yes/ );
	}
    }
    $\=$temp;
}

sub errorUSAGE
{
    print "ERROR: $_[0]" if ($_[0] ne "");
    print $USAGE;    
    exit $ERROR_USAGE;    
}

sub ListOnStdOut
{    
    my $fname = shift;    
    return unless ( -f "$fname" );
    my $file_in = new FileHandle;
    my $temp=$\;$\="";
    if (!open($file_in, "<$fname")) {
        WriteProt(date()." ===\> ERROR: ListOnStdOut: Can't open $fname (output): $!\n");
    }
    else {
        while(<$file_in>) {
            print $_;
        }
        close $file_in;
    }
    $\=$temp;
}

sub ListOnStdErr
{    
    my $fname = shift;
    return unless ( -f "$fname" );
    my $file_in = new FileHandle;
    my $temp=$\;$\="";
    if (!open($file_in, "<$fname")) {
        WriteProt(date()." ===\> ERROR: ListOnStdErr: Can't open $fname (output): $!\n");
    }
    else {
        while(<$file_in>) {
            print STDERR $_;
        }
        close $file_in;
    }
    $\=$temp;
}

sub ListOnProt
{    
    my $fname = shift;
    return unless ( -f "$fname" );
    my $file_in = new FileHandle;
    my $temp=$\;$\="";
    if (!open($file_in, "<$fname")) {
        WriteProt(date()." ===\> ERROR: ListOnProt: Can't open $fname (output): $!\n");
    }
    else {
        while(<$file_in>) {
            _WriteProt($_);
        }
        close $file_in;
    }
    $\=$temp;
}

#################################################################
# starts an external process with given commandline and timeout #
#################################################################
sub exec_with_timeout {
  my ($cmd, $some_seconds, $redirect) = @_;
  print "CMD: $cmd\n TIMEOUT: $some_seconds sec.\n REDIRECT OUTPUT: $redirect\n" if($Trace);
  my $ret = $ERROR_OK;
  if ($redirect ne "" ) {
    print "redirect active: $redirect\n" if($Trace);
    open SAVEOUT, ">&STDOUT";
    open STDOUT, ">".$redirect or die "redirection of STDOUT failed";
    select STDOUT; $| = 1; #buffering deactivated
  }     
  
  # Win32 do not support fork
  if ( defined $ENV{OS} && ($ENV{OS} eq "WIN32" || $ENV{OS} eq "Windows_NT") ) { 
    require Win32::Process;
    $cmd =~ m/(\S+)\s?(.*)/;
    my $call = $1;
    my $options = $1.' '.$2;
    print "COMMAND: $call\n OPTIONS: $options\n" if ($Trace);
    Win32::Process::Create($Proc, $call, $options,'1', 'NORMAL_PRIORITY_CLASS', '.') || die Win32::FormatMessage(Win32::GetLastError());
    $Proc->Wait($some_seconds*1000);
    $Proc->GetExitCode($exit_code );
    print"EXITCODE: $exit_code\n" if($Trace);
    if ($exit_code == 259) {  # if process timed out  
      print"Timeout reached - Killing Worker\n" if($Trace);
      $Proc->Kill(0);
      $ret = $ERROR_TIMEOUT;
    } 
    elsif ($exit_code != 0) {
        $ret = $ERROR_RUNNING;
    }
  }
  else {
    my ($pidwrk, $pidwdg);
    my $wrk_finished=0;
    my $wdg_finished=0;
    my $exit_code = -1;
    $pidwrk = fork();
    unless (defined $pidwrk){
        WriteProt (date()." ===\> ERROR: Internal (fork worker failed).");
        exit $ERROR_INTERNAL;
    }
    elsif ($pidwrk == 0) {
      # worker child
      print "Starting Worker\n" if($Trace);
      exec split (' ', $cmd);
      WriteProt (date()." ===\> ERROR: Internal (exec \'$cmd\' failed: $!).");
      print "Leaving Worker\n" if($Trace);
      exit $ERROR_INTERNAL;
    }

    $pidwdg = fork();
    unless (defined $pidwdg){
      WriteProt (date()." ===\> ERROR: Internal (fork watcher failed: $!).");
      exit $ERROR_INTERNAL;
    }
    elsif ($pidwdg == 0) {
      # watcher child
      print "Starting WatchDog\n" if($Trace);
      sleep $some_seconds;
      print "Leaving WatchDog\n" if($Trace);
      exit 0;
    }

    for (;;) {
      my $child = wait ();
      $exit_code = $?;
      if ($child == $pidwdg && ! $wrk_finished) {
        print "Killing Worker\n" if($Trace);
        kill 9, $pidwrk;
        $wdg_finished=1;
        $ret = $ERROR_TIMEOUT;
      }
      if ($child == $pidwrk && ! $wdg_finished){
        print"EXITCODE: $exit_code\n" if($Trace);
        print "Killing WatchDog\n" if($Trace);
        kill 9, $pidwdg;
        $wrk_finished = 1;
        if ($exit_code != 0) {
            $ret = $ERROR_RUNNING;
        }
      }
      last if ($child == -1);
    }
  }  
  if ($redirect ne "" ) {
    close STDOUT;
    open STDOUT,">&SAVEOUT";
    print "undo redirect\n" if($Trace);
  }  
  return $ret;
}

####################################################################
# wait until the passed sema is free or a timeout has been reached #
####################################################################
use File::stat;
sub SetSema
{
    my ($timeout) = @_;
    my ($sema) = $SEMAFILE;
    local($_mtime)=0;
    if (($st = stat($sema))) {
        $timegone=time()-$st->mtime;
      SLEEP: while($timegone <= $timeout) {
          if (($st = stat($sema))) {
              $timegone=time()-$st->mtime;
          }
          else {
              last SLEEP;                
          }
          if ($_mtime != $st->mtime) {
              WriteWaitSema($timeout-$timegone);                  
              $_mtime = $st->mtime
          }
          sleep 1;
      }
    }
    my $file_out = new FileHandle;
    if (!open($file_out, ">$sema")) {
        WriteCantSetSema();
    }
    else {
        $IsSema = 1;
        close($file_out);
    }
}

sub ResetSema
{
    my ($sema) = $SEMAFILE;
    _unlink "$sema" || WriteCantResetSema();
    $IsSema = 0;
}

sub WriteWaitSema
{
    if ($HTMLProt) {
        my $Prefix = "<span style=\"background-color: #C0C0C0\"> Another test ist running wait max. $_[0] seconds </font></span>";
        WriteProtHTML(date()."$Prefix");
    }
    HTMLOff;
    WriteProt(date()." Another test ist running wait max. $_[0] seconds");
    HTMLOn;
}

sub WriteCantResetSema
{
   if ($HTMLProt) {
     WriteErrorProtHTML(date()."\> ===\> CAN'T RESET SEMAFORE FILE");
   }
   HTMLOff;
     WriteProt(date()." ===\> CAN'T RESET SEMAFORE FILE");
   HTMLOn;
}

sub WriteCantSetSema
{
   if ($HTMLProt) {
     WriteErrorProtHTML(date()."\> ===\> CAN'T SET SEMAFORE FILE");
   }
   HTMLOff;
     WriteProt(date()." ===\> CAN'T SET SEMAFORE FILE");
   HTMLOn;
}
__END__
