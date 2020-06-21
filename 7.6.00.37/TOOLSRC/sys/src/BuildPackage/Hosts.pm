#! /usr/bin/perl
#
# $Header: //prodprojects/vmake/dev/src/_tools/maketools/sys/src/BuildPackage/Hosts.pm#38 $
# $DateTime: 2006/05/04 11:04:16 $
# $Change: 591703 $
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
 
		
package BuildPackage::Hosts;

use Exporter;
use BuildPackage::Vars;

@ISA = ('Exporter');

@EXPORT = qw($CUR_OS %HOSTS %OSS $OS_ALIAS);

$CUR_OS;

if($OS eq 'win'){
	$CUR_OS = ($ARCH eq 'ia64' ? 'WIN64' : $ARCH eq 'x86_64' ?  'WINAMD64' : 'WIN32');
}
elsif($OS eq 'linux'){
	$CUR_OS = ($ARCH eq 'ia64' ? 'LINUX64' : 'LINUX32');
}
elsif($OS eq 'tru64'){
	$CUR_OS = 'DEC64';
}
elsif($OS eq 'hpux'){
	if($ARCH eq 'ia64'){
		$CUR_OS = 'HPIA64';
	}
	else{
		$CUR_OS = (exists $ENV{'BIT64'} ? 'HP64' : 'HP32');
	}
}
elsif($OS eq 'sun'){
	$CUR_OS = (exists $ENV{'BIT64'} ? 'SUN64' : 'SUN32');
}
elsif($OS eq 'aix'){
	$CUR_OS = (exists $ENV{'BIT64'} ? 'AIX64' : 'AIX32');
}
elsif($OS eq 'aix5'){
	$CUR_OS = 'AIX564';
}
elsif($OS eq 'reliant'){
	$CUR_OS = (exists $ENV{'BIT64'} ? 'RELIANT64' : 'RELIANT32');
}
else{
	print STDERR "unknown os\n";
	exit -1;
}

$OS_ALIAS = {
	'LINUX64' => {
				'7403' => 'LINUX24IA64',
				'7404' => 'LINUX24IA64',
				'7500' => 'LINUX24IA64',
				'7600' => 'LINUX26IA64',
				'7700' => 'LINUX26IA64',
				'7701' => 'LINUX26IA64'
				}
};


%OSS = (
	'WIN32' => {'name' => 'win','arch' => 'i386','bit' => '32'},
	'WIN64' => {'name' => 'win','arch' => 'ia64','bit' => '64'},
	'WINAMD64' => {'name' => 'win','arch' => 'x86_64','bit' => '64'},				
	'AIX32' => {'name' => 'aix','arch' => 'ppc','bit' => '32'},
	'AIX64' => {'name' => 'aix','arch' => 'ppc','bit' => '64'},
	'AIX532' => {'name' => 'aix5','arch' => 'ppc','bit' => '32'},
	'AIX564' => {'name' => 'aix5','arch' => 'ppc','bit' => '64'},
	'DEC64' => {'name' => 'tru64','arch' => 'alpha','bit' => '64'},
	'HP32' => {'name' => 'hpux','arch' => 'hppa','bit' => '32'},
	'HP64' => {'name' => 'hpux','arch' => 'hppa','bit' => '64'},
	'HPIA64' => {'name' => 'hpux','arch' => 'ia64','bit' => '64'},
	'SUN32' => {'name' => 'sun','arch' => 'sparc','bit' => '32'},
	'SUN64' => {'name' => 'sun','arch' => 'sparc','bit' => '64'},
	'SUNX86' => {'name' => 'sun','arch' => 'i386','bit' => '32'},
	'SUNX8664' => {'name' => 'sun','arch' => 'x86_64','bit' => '64'},
	'LINUX32' => {'name' => 'linux','arch' => 'i386','bit' => '32'},
	'LINUX24IA64' => {'name' => 'linux','arch' => 'ia64','bit' => '64'},
	'LINUX26IA64' => {'name' => 'linux','arch' => 'ia64','bit' => '64'},
	'LINUXX8664' => {'name' => 'linux','arch' => 'x86_64','bit' => '64'},
	'LINUXPPC64' => {'name' => 'linux','arch' => 'ppc','bit' => '64'},
	'LINUXS390X' => {'name' => 'linux','arch' => 's390x','bit' => '64'}		
);


%HOSTS = (
	'7300' => {
					'WIN32' => {'name' => 'production.wdf.sap.corp',
								'LC_POOL_ROOT' => '/LC_POOL/WIN32'},
					'AIX32' => {'name' => 'production.wdf.sap.corp',
								'LC_POOL_ROOT' => '/LC_POOL/AIX32'},
					'AIX64' => {'name' => 'production.wdf.sap.corp',
								'LC_POOL_ROOT' => '/LC_POOL/AIX64'},
					'AIX532' => {'name' => 'production.wdf.sap.corp',
								'LC_POOL_ROOT' => '/LC_POOL/AIX532'},
					'AIX564' => {'name' => 'production.wdf.sap.corp',
								'LC_POOL_ROOT' => '/LC_POOL/AIX564'},
					'DEC64' => {'name' => 'production.wdf.sap.corp',
								'LC_POOL_ROOT' => '/LC_POOL/DEC64'},
					'HP32' => {'name' => 'production.wdf.sap.corp',
								'LC_POOL_ROOT' => '/LC_POOL/HP32'},
					'HP64' => {'name' => 'production.wdf.sap.corp',
								'LC_POOL_ROOT' => '/LC_POOL/HP64'},
					'SUN32' => {'name' => 'production.wdf.sap.corp',
								'LC_POOL_ROOT' => '/LC_POOL/SUN32'},
					'SUN64' => {'name' => 'production.wdf.sap.corp',
								'LC_POOL_ROOT' => '/LC_POOL/SUN64'},
					'LINUX32' => {'name' => 'production.wdf.sap.corp',
								'LC_POOL_ROOT' => '/LC_POOL/LINUX32'}
						
	},
	'7402' => {
					'WIN32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/NTintel'},
					'WIN64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/NTia64'},				
					'AIX32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/rs6000_64'},
					'AIX64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/rs6000_64'},				
					'AIX564' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/rs6000_51_64'},
					'DEC64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/alphaosf'},
					'HP32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/hp_64'},
					'HP64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/hp_64'},
					'SUN32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/sun_64'},
					'SUN64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/sun_64'},
					'LINUX32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/linux2.4ia32'}						
	},
	'7403' => {
					'WIN32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/NTintel'},
					'WIN64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/NTia64'},				
					'AIX32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/rs6000_51_64'},
					'AIX564' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/rs6000_51_64'},
					'DEC64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/alphaosf'},
					'HP32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/hp_64'},
					'HP64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/hp_64'},
					'HPIA64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/hpia64'},
					'SUN32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/sun_64'},
					'SUN64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/sun_64'},
					'LINUX32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/linux2.4ia32'},
					'LINUX24IA64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/linux2.4ia64'}									
	},
	'7404' => {
					'WIN32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/NTintel'},
					'WIN64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/NTia64'},				
					'AIX32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/rs6000_51_64'},
					'AIX564' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/rs6000_51_64'},
					'DEC64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/alphaosf'},
					'HP32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/hp_64'},
					'HP64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/hp_64'},
					'HPIA64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/hpia64'},
					'SUN32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/sun_64'},
					'SUN64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/sun_64'},
					'LINUX32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/linux2.4ia32'},
					'LINUX24IA64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/linux2.4ia64'}	
	},
	'7500' => {
					'WIN32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/NTintel'},
					'WIN64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/NTia64'},				
					'WINAMD64' => {'name' => 'production.wdf.sap.corp',
								'LC_POOL_ROOT' => '/LC_POOL/ntamd64'},			
					'AIX32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/rs6000_51_64'},
					'AIX564' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/rs6000_51_64'},
					'DEC64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/alphaosf'},
					'HP32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/hp_64'},
					'HP64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/hp_64'},
					'HPIA64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/hpia64'},
					'SUN32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/sun_64'},
					'SUN64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/sun_64'},
					'LINUX32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/linux2.4ia32'},
					'LINUX24IA64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/linux2.4ia64'},				
					'LINUX26IA64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/linux2.6ia64'},
					'LINUXX8664' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/linuxx86_64'},
					'LINUXPPC64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/linuxppc64'},
					'LINUXS390X' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/linuxs390x'}	
	},
	'7501' => {
					'WIN32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/NTintel'},
					'WIN64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/NTia64'},			
					'AIX32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/rs6000_51_64'},
					'AIX564' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/rs6000_51_64'},
					'DEC64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/alphaosf'},
					'HP32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/hp_64'},
					'HP64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/hp_64'},
					'HPIA64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/hpia64'},
					'SUN32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/sun_64'},
					'SUN64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/sun_64'},
					'LINUX32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/linux2.4ia32'},
					'LINUX24IA64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/linux2.4ia64'}
	},
	'7600' => {
					'WIN32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/NTintel'},
					'WIN64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/NTia64'},				
					'WINAMD64' => {'name' => 'production.wdf.sap.corp',
								'LC_POOL_ROOT' => '/LC_POOL/ntamd64'},			
					'AIX32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/rs6000_52_64'},
					'AIX564' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/rs6000_52_64'},
					'DEC64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/alphaosf'},
					'HP32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/hp_64'},
					'HP64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/hp_64'},
					'HPIA64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/hpia64'},
					'SUN32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/sun_64'},
					'SUN64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/sun_64'},
					'SUNX8664' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/sunx86_64'},				
					'LINUX32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/linux2.6ia32'},
					'LINUX26IA64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/linux2.6ia64'},
					'LINUXX8664' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/linuxx86_64'},
					'LINUXPPC64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/linuxppc64'},
					'LINUXS390X' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/linuxs390x'}										
	},
	'7700' => {'WIN32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/NTintel'},
					'WIN64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/NTia64'},				
					'WINAMD64' => {'name' => 'production.wdf.sap.corp',
								'LC_POOL_ROOT' => '/LC_POOL/ntamd64'},			
					'AIX32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/rs6000_52_64'},
					'AIX564' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/rs6000_52_64'},
					'HP32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/hp_64'},
					'HP64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/hp_64'},
					'HPIA64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/hpia64'},
					'SUN32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/sun_64'},
					'SUN64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/sun_64'},
					'SUNX8664' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/sunx86_64'},				
					'LINUX32' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/linux2.6ia32'},
					'LINUX26IA64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/linux2.6ia64'},
					'LINUXX8664' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/linuxx86_64'},
					'LINUXPPC64' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/linuxppc64'},
					'LINUXS390X' => {'name' => 'production.wdf.sap.corp',
									'LC_POOL_ROOT' => '/LC_POOL/linuxs390x'}
	}

);

$HOSTS{'7701'} = $HOSTS{'7700'};


1;

