#! /usr/bin/perl
#
# $Header: //prodprojects/vmake/dev/src/_tools/maketools/sys/src/BuildPackage/SysName.pm#7 $
# $DateTime: 2006/01/03 15:46:49 $
# $Change: 581457 $
#
# Desc:
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

package BuildPackage::SysName;

use Exporter;
use Config;

@ISA = qw(Exporter);

@EXPORT = qw(getsysinfo);

sub getsysinfo{
	my $mach;
	my $machine_config;
	if($^O =~ /mswin/i ){
		$machine_config->{'os'} = 'Microsoft Windows';
		if($ENV{'PROCESSOR_ARCHITECTURE'} eq 'x86'){
			$machine_config->{'mach'} = 'i386';
			$machine_config->{'bp_name'} = 'WIN32';
			if(	exists $ENV{'VMAKE_PATH'} and
			    exists $ENV{'BIT64'}){
				
				$machine_config->{'os'} = 'Microsoft Windows [cross compile machine (native i386)]';
				$machine_config->{'mach'} = 'ia64';
				$machine_config->{'bp_name'} = 'WIN64';
 			}
		}
		elsif($ENV{'PROCESSOR_ARCHITECTURE'} =~ /ia64/i){
			$machine_config->{'mach'} = 'ia64';
			$machine_config->{'bp_name'} = 'WIN64';
		}
		elsif($ENV{'PROCESSOR_ARCHITECTURE'} =~ /amd64/i){
			$machine_config->{'mach'} = 'x64-64';
			$machine_config->{'bp_name'} = 'WINAMD64';
		}
		else{
			$machine_config->{'mach'} = 'unknown';
		}
		return $machine_config;
	}
	else{
		if ($^O =~ /linux/i and $Config{'archname'} =~ /i\d86/ and -d '/emul/ia32-linux'){
			print STDERR "WARNING: You are using a 32 bit perl!!!!!\n";
			open (PROC,'/proc/cpuinfo');
            while (<PROC>){
				if (/^arch/i){
					my ($arch) = (/:\s+(\S+)/);
                    if ($arch eq 'IA-64'){
						$mach = 'ia64';
                    }
                }
            }
            close(PROC);
        }
        else{
			$mach = `uname -m`;
        }
		$machine_config->{'mach'} = $mach;
	}

	if($^O =~ /linux/i){
		$machine_config->{'os'} = 'Linux';
		if($mach =~ /i\d86/i){
			$machine_config->{'mach'} = 'i386';
			$machine_config->{'bp_name'}= 'LINUX32';
		}
		elsif($mach =~ /ia64/i){
			$machine_config->{'mach'} = 'ia64';	
			$machine_config->{'bp_name'}=  'LINUX64';
			my $os_release = `uname -r`;
			if($os_release =~ /^2\.6/){
				$machine_config->{'os'} = 'Linux 2.6 (SLES9)';
				$machine_config->{'bp_name'}=  'LINUX26IA64';
			}
			elsif($os_release =~ /^2\.4/){
				$machine_config->{'os'} = 'Linux 2.4';
				$machine_config->{'bp_name'}=  'LINUX24IA64';
			}
			
		}
		elsif($mach =~ /x86_64/i){
			$machine_config->{'mach'} = 'x86-64';	
			$machine_config->{'bp_name'}=  'LINUXX8664';
		}
		elsif($mach =~ /s390/i){
			$machine_config->{'mach'} = 's390x';	
			$machine_config->{'bp_name'}=  'LINUXS390X';
		}
		elsif($mach =~ /ppc/i){
			$machine_config->{'mach'} = 'ppc64';	
			$machine_config->{'bp_name'}=  'LINUXPPC64';
		}
		else{
			$machine_config->{'mach'} = 'unkown';	
		}

	}
	elsif($^O =~ /aix/i){
		my $oslevel = `oslevel`;
			$machine_config->{'os'} = 'AIX '.$oslevel;
		if($oslevel =~ /^5/){
			$machine_config->{'bp_name'}=  'AIX564'; 
		}
		else{
			$machine_config->{'bp_name'}=  'AIX64';
		}
	}
	elsif($^O =~ /solaris/i){
		$machine_config->{'os'} = 'Solaris';
		if($mach =~ /i86pc/){
			if (`isainfo -k` =~ /amd64/){
				$machine_config->{'bp_name'}=  'SUNX8664';
				$machine_config->{'mach'} = 'x86_64';
			}
			else{
				$machine_config->{'bp_name'}=  'SUNX86';
				$machine_config->{'mach'} = 'i386';
			}
		}
		elsif($mach =~ /sun4u/){
			$machine_config->{'bp_name'}=  'SUN64';
			$machine_config->{'mach'} = 'sparc';
		}
		else{
			$machine_config->{'bp_name'}=  'SUN64';
		}
	}
	elsif($^O =~ /hp/i){
		$machine_config->{'os'} = 'HP-UX';
		if($mach =~ /ia64/i){
			$machine_config->{'bp_name'}=  'HPIA64';
		}
		else{
			$machine_config->{'bp_name'}=  'HP64';
		}
	}
	elsif($^O =~ /osf/i){
		$machine_config->{'os'} = 'Compaq Tru64';
		$machine_config->{'bp_name'}=  'DEC64';
	}
	else{
		print STDERR "unkown system\n";
		return undef;
	}
	return $machine_config;
}


1;

