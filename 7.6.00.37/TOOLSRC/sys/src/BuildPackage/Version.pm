#! /usr/bin/perl
#
# $Header: //prodprojects/vmake/dev/src/_tools/maketools/sys/src/BuildPackage/Version.pm#2 $
# $DateTime: 2005/05/25 10:04:58 $
# $Change: 560360 $
#
# Desc: helper functions for BuildPackage
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


package BuildPackage::Version;

use Exporter;


@ISA =('Exporter');
@EXPORT = ('getVersionString');


sub getLoadPath{
	foreach my $path (@INC){
		if(-f $path.'/BuildPackage/Distrib.pm'){
			return $path;
		}
	}
	return undef;
}


sub getChangeList{
	my $load_path = getLoadPath();
	#print ">>>> LOAD_PATH = $load_path\n";
	opendir(DH,$load_path.'/BuildPackage') or return undef;
	my @modules = grep { /\.pm$/ } readdir(DH);
	closedir(DH);
	my $change;
	foreach my $module (@modules){
		open(FD,$load_path.'/BuildPackage/'.$module) or next;
		while(<FD>){
			chomp;
			if(/\$Change:\s*\d+/){
				my ($lchange) = (/\$Change:\s*(\d+)/);
				$change = $lchange if ($lchange > $change);
				#print ">>>> MODULE is $module with change $lchange\n";
				last;
			}
		}
		close(FD);
	}
	return $change;
}


sub VersionString{
	my $pd_header = '$Header: //prodprojects/vmake/dev/src/_tools/maketools/sys/src/BuildPackage/Version.pm#2 $';
	my ($tools_state) = ($pd_header =~ /vmake\/(.+)\/src\/_/);
	my $change = getChangeList();	
	return (defined $tools_state ? $tools_state : 'unknown' )." TOOLS, changelist ".(defined $change ? $change : "unkown");
}

$version_string = VersionString();

sub getVersionString{
	return $version_string;
}

1;






