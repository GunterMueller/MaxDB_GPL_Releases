#! /usr/bin/perl
#
# $Header: //prodprojects/vmake/dev/src/_tools/maketools/sys/src/BuildPackage/Vars.pm#29 $
# $DateTime: 2006/05/16 15:05:36 $
# $Change: 592950 $
#
# Desc:	- define vars for plattform specific vars
#		
#
#
#    ========== licence begin LGPL
#    Copyright (C) 2002 SAP AG
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#    ========== licence end
#
 
		
package BuildPackage::Vars;

use Exporter;

@ISA = ('Exporter');

@EXPORT = ('$WRK','$OWN','$USR','$INSTROOT',
		'$MAJ','$MIN','$COR','$BUILD',
		'$BIT', '$VERSION_PATH', '$BUILDROOT',
		'$SOEXT','$EXEEXT','$OS32_PATH','$OS64_PATH',
		'$ARCH','$OS','$PACKAGEPATH','$image_server',
		'$XMLFile','$CDImagePath','$CDIMAGEOS','$SAPDB_TEST_COMPONENTS_DIR',
		'$LIBPATH','$RUNTIME_PATH','$RUNTIME_VERSION',
		'$USE_FTP','SetVars' ,'$LC_RELEASE','$LC_ROOT','$LC_POOL_ROOT',
		'$LC_STATE','$LC_PACK_NAME','$LC_PACK_NAME_OLD','$LC_OWN','$LC_POOL_DIR',
		'$SAPDB_POOL_DIR',,'$SAPDB_POOL_ROOT','$LC_BUILD_SELECT','$APO_REL','$APO_SHORT_VERS','$APO_BUILD',
		'$APOMAKE','$APO_STATE','$APO_POOL_DIR','$EM_MAKE','$APO_VARIANT','$LCAPPS_PACKAGE_PATTERN',
		'$APO_PACK_NAME_OLD','$LC_INNER_SUBDIR','$SAPDB_COMPONENTS_DIR','$LCAPPS_COMPONENTS_DIR',
		'$LCAPPS_PROFILE_PATTERN','$LIVECACHE_PROFILE_PATTERN','$LIVECACHE_PROFILE',
		'$INSTALL_SUB_PATH','$LCAPPS_PROFILE','$RELVER'
		);



$USE_FTP = 1;

$WRK=$ENV{'WRK'};
$WRK=~s/\\/\//g;


$LIBPATH="lib";

if(exists $ENV{BIT64} or $^O=~/.*osf.*/i or ($^O=~/mswin/i and $ENV{PROCESSOR_ARCHITECTURE} eq 'AMD64')){
        $BIT='64';
	  $^O=~/osf|linux/i or $LIBPATH="$LIBPATH/lib64";
}
else{
         $BIT='32';
}


if(${(caller)[0].'::opt_installdir'} =~ /\S/){
	$INSTALL_SUB_PATH = ${(caller)[0].'::opt_installdir'};
}
elsif(exists $ENV{'APOMAKE'}){
	$INSTALL_SUB_PATH = 'install';
}
else{
	if(-d "$WRK/fast/install56"){
		$INSTALL_SUB_PATH = 'install56';
	}
	else{
		$INSTALL_SUB_PATH = 'install';
	}
}

$OWN=$ENV{'OWN'};
$OWN=~s/\\/\//g;
$INSTROOT=$ENV{INSTROOT};
$INSTROOT=~s/\\/\//g;
$PACKAGEPATH="$WRK/fast/$INSTALL_SUB_PATH";
$PACKAGEPATH =~ s/\\/\//g;
$LC_STATE = $ENV{'LC_STATE'};
($MAJ,$MIN) = ($ENV{'RELVER'} =~ /R(\d)(\d+)/ );
$RELVER = $MAJ.$MIN;
$COR = $ENV{'CORRECTION_LEVEL'}; 
$COR = "0$COR" if length($COR) == 1;
$BUILD = $ENV{'BUILDPRAEFIX'};
$BUILD = "0$BUILD" if length($BUILD) == 1;
$BUILDROOT="$WRK/fast/$INSTALL_SUB_PATH/buildroot";
$USR="$OWN/usr";
$cor = $COR;
$cor =~ s/^0//;
$build = $BUILD;
$build =~ s/^0//;

$SAPDB_COMPONENTS_DIR = 'SAPDB_COMPONENTS';
$SAPDB_TEST_COMPONENTS_DIR = 'TEST_COMPONENTS';
$LCAPPS_COMPONENTS_DIR = 'LCAPPS_COMPONENTS';

$LCAPPS_PACKAGE_PATTERN = '^APOCOM.TGZ$|^APOLCA.TGZ$';
$LCAPPS_PROFILE = 'APO LiveCache';
$LCAPPS_PROFILE_PATTERN = '^'.$LCAPPS_PROFILE.'$';
$LCAPPS_PROFILE_PATTERN =~ s/\s/\\s/g;
$LIVECACHE_PROFILE = 'Server';
$LIVECACHE_PROFILE_PATTERN = '^'.$LIVECACHE_PROFILE.'$';


$LC_POOL_ROOT = ((exists $ENV{'LC_POOL_ROOT'} and $ENV{'LC_POOL_ROOT'} =~ /\S/) ? $ENV{'LC_POOL_ROOT'} : '/LC_POOL'); 
$LC_POOL_DIR = "$LC_POOL_ROOT/LC_$MAJ$MIN$COR".(($LC_STATE =~ /DEV|COR/i) ? "" : $BUILD)."_${BIT}_${LC_STATE}";

$SAPDB_POOL_ROOT = ((exists $ENV{'SAPDB_POOL_ROOT'} and $ENV{'SAPDB_POOL_ROOT'} =~ /\S/) ? $ENV{'SAPDB_POOL_ROOT'} : '/SAPDB_COMPONENTS_POOL');
$SAPDB_POOL_DIR = "$SAPDB_POOL_ROOT/SDBC_$MAJ$MIN$COR".(($LC_STATE =~ /DEV|COR/i) ? "" : $BUILD)."_${BIT}_${LC_STATE}";



$image_server="production.wdf.sap.corp";
$XMLFile = "$ENV{'WRK'}/fast/$INSTALL_SUB_PATH/pcr_releases.xml";
$CDImagePath = 'CD-Images/PCR_REL';
$CDImagePath =~ s/\//\\/g if $^O =~ /mswin/i; 
if($^O=~/.*win32.*/i){
	$OS32_PATH = "NT\\I386";
	if($ENV{'PROCESSOR_ARCHITECTURE'} eq 'x86'){
		if(exists $ENV{'VMAKE_PATH'} and
			$ENV{'XCOMP'}){
 			$ARCH='ia64';
			$OS64_PATH = "NT\\IA64";
		}
		else{
			$ARCH="i386";
		}
	}
	elsif($ENV{'PROCESSOR_ARCHITECTURE'} =~ /ia64/i){
		$ARCH='ia64' if exists $ENV{'BIT64'};
		$OS64_PATH = "NT\\IA64";
	}
	elsif($ENV{'PROCESSOR_ARCHITECTURE'} =~ /amd64/i){
		$ARCH = 'x86_64';
		$OS64_PATH = "NT\\AMD64";		
	}
	else{
		$ARCH = 'unknown';
	}
	$OS="win";
	$CDIMAGEOS='NT';
	$LIBPATH="pgm";
	$SOEXT="dll";
	$EXEEXT = '.exe';
	
	
}
elsif($^O=~/linux/i){
	$ARCH="i386";
	$ARCH='ia64' if $ENV{'MACH'} eq 'IA64';
	$OS32_PATH="UNIX/LINUX_32";
	$OS64_PATH="UNIX/LINUX_64";
	if($ENV{'MACH'} eq 'X86_64'){
		$OS64_PATH="UNIX/LINUXX86_64";
		$ARCH='x86_64';
	}
	if($ENV{'MACH'} eq 'S390X'){
		$OS64_PATH="UNIX/LINUXS390X";
		$ARCH='s390x';
	}
	if($ENV{'MACH'} eq 'SDBonPPC64'){
		$OS64_PATH="UNIX/LINUXPPC64";
		$ARCH='ppc';	
	}
	$OS="linux";
	$SOEXT="so";
}
elsif($^O=~/.*aix.*/i){
	$ARCH="ppc";
	$SOEXT="so";
	$OS32_PATH="UNIX/AIX_32";
	my $outtext = `oslevel`;
	if($outtext =~ /^5/){
		$OS = 'aix5';
		$OS64_PATH = 'UNIX/AIX5_64';
	}
	else{
		$OS="aix";
		$OS64_PATH="UNIX/AIX_64";
	}	
}
elsif($^O=~/.*solaris.*/i){
	my $out = `uname -m`;
	if ($out =~ /i86pc/){
		if (`isainfo -k` =~ /amd64/ and exists $ENV{'BIT64'}){
			$ARCH = 'x86_64';
		}
		else{
			$ARCH = 'i386';
		}
		$OS32_PATH="UNIX/SUNOSX86_32";
		$OS64_PATH="UNIX/SUNOSX86_64";
	}
	else{
		$ARCH="sparc";
		$OS32_PATH="UNIX/SUNOS_32";
		$OS64_PATH="UNIX/SUNOS_64";
	}
	$OS="sun";
	$SOEXT="so";
}
elsif($^O=~/.*osf.*/i){
	$ARCH="alpha";
	$OS="tru64";
	$SOEXT="so";
	$OS64_PATH="UNIX/DEC_64";
}
elsif($^O=~/.*hp.*/i){
	$OS="hpux";
	my $mach = `uname -m`;
	if($mach =~ /ia64/i){
		$LIBPATH="lib";
		$ARCH="ia64";
		$SOEXT="so";
		$OS32_PATH="UNIX/HP11_32";
		$OS64_PATH="UNIX/HPIA64_64";
	}
	else{
		$ARCH="hppa";
		$SOEXT="sl";
		$OS32_PATH="UNIX/HP11_32";
		$OS64_PATH="UNIX/HP11_64";
	}
}
elsif($^O=~/svr4/i){
	$ARCH="mips";
	$OS="reliant";
	$SOEXT="so";
	$OS32_PATH="UNIX/RLX_32";
	$OS64_PATH="UNIX/RLX_64";
}

$LC_PACK_NAME_OLD = "sapdb-server-$OS-${BIT}bit-$ARCH-${MAJ}_${MIN}_${cor}_${build}.sar";


$RUNTIME_VERSION = "$MAJ$MIN$COR";
if($MAJ == 7 && $MIN == 2){
	my $cor = $COR;
	$cor =~ s/^0*//; # remove nulls at begin of line
	$RUNTIME_VERSION = $MAJ.$MIN.$cor.'0'; 
}

$RUNTIME_PATH= 'runtime/'.$RUNTIME_VERSION;


#
# find out if it is a final assembly make
#

if ($^O !~ /mswin/i){
	if(-d $LC_POOL_ROOT && ($> == getpwnam('makefact') || $> == getpwnam('falcmake') || $> == getpwnam('vmake'))){	
		$EM_MAKE = 1;
	}
}

if(exists $ENV{'EM_MAKE'} || $ENV{'RELSTAT'} eq 'PROD'){
		$EM_MAKE = 1;
	}



#
# special vars for APO COM packages
#

if(exists $ENV{'APOMAKE'}){

	$APOMAKE = 1;

	my ($maj,$min,$cor,$build) = ($ENV{'LCVERSION'} =~ /SAPDB_(\d)(\d)(\d+)_b(\d+)/);
	my $cor_short = $cor;
	$cor_short =~ s/^0*//;						# remove leading zeros
	$cor_short = '0' unless $cor_short =~ /\S/;	#
	my $build_short = $build;
	$build_short =~ s/^0*//;
	$build_short eq '' and $build_short = '0';
	$cor = "0$cor" if length($cor) == 1;
	$build = "0$build" if length($build) == 1;
	
	$MAJ = $maj;
	$MIN = $min;
	$COR = $cor;
	$BUILD= $build;


	$RUNTIME_VERSION = "$maj$min$cor";
	$RELVER = $MAJ.$MIN;
	
	$LC_OWN = $ENV{'LC_OWN'};
	
	if($^O =~ /mswin/i){
		($LC_BUILD_SELECT) = ($LC_OWN =~ /\\(\d{3,4})\\*$/);
	}
	else{
		($LC_BUILD_SELECT) = ($LC_OWN =~ /\/(\d{3,4})\/*$/);
	}
		
	
	defined $LC_BUILD_SELECT and $LC_BUILD_SELECT =~ /\S/ or 
	$LC_BUILD_SELECT = 'LastBuild';
	
	$LC_PACK_NAME_OLD = "sapdb-server-$OS-${BIT}bit-$ARCH-${maj}_${min}_${cor_short}_${build_short}.sar";
	
	$LC_INNER_SUBDIR = "sapdb-server-$OS-${BIT}bit-$ARCH-${maj}_${min}_${cor_short}_${build_short}";


	$LC_PACK_NAME = "SAPDB${maj}${min}${cor}_${build}.SAR";

	$LC_RELEASE = "$maj.$min.$cor.$build";
	
	$LC_ROOT = $ENV{'LCROOT'};

	($APO_STATE) = ($ENV{'BRANCH_INFO'} =~ /(DEV|COR|CONS|HOT)/);
	$APO_STATE = 'RAMP' unless defined $APO_STATE;
 	
	$LC_POOL_DIR = "$LC_POOL_ROOT/LC_$maj$min$cor".(($LC_STATE =~ /DEV|COR/i) ? "" : $build)."_${BIT}_${LC_STATE}";

	$APO_REL = $ENV{'APO_COM_RELEASE'};

	$APO_BUILD = $ENV{'APO_COM_BUILD'};
	
	$APO_SHORT_VERS = $APO_REL;

	$APO_SHORT_VERS =~ s/\.//g;

	$APO_SHORT_VERS .= $APO_BUILD; 

	$APO_VARIANT = $ENV{'VMAKE_DEFAULT_VARIANT'};

	defined $APO_VARIANT and $APO_VARIANT =~ /Debug|Release/ or
		$APO_VARIANT = 'Release';
	
	$APO_VARIANT =~ s/Release/opt/;
	$APO_VARIANT =~ s/Debug/dbg/;

	$APO_PACK_NAME_OLD = ["sapdb-apo${APO_SHORT_VERS}_livecache-$OS-${BIT}bit-$ARCH-${maj}_${min}_${cor_short}_${build_short}.sar",
						 "LCA${maj}${min}${cor}${build}_${APO_BUILD}.SAR"							
						];

	$APO_POOL_DIR = "$LC_POOL_DIR/$LC_BUILD_SELECT/LCAPPS_${APO_SHORT_VERS}_${APO_VARIANT}_${APO_STATE}";		
}


sub SetVars{
	my ($hrArgs,$err) = @_;
	ref($hrArgs) eq 'HASH' or return undef;
	my %vars = %$hrArgs;
	foreach my $var (keys(%vars)){
		if(exists $BuildPackage::Vars::{$var}){
			${$var} = $vars{$var};
		}
		else{
			if(defined $err and ref($err) eq 'CODE'){
				&$err ("cannot set unknown scalar \$BuildPackage::Vars::$var to $vars{$var}\n");
			}
		}
	}
		
	return 1;
}

1;

