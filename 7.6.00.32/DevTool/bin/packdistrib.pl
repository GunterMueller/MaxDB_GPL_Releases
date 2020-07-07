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
#    21.06.2001 integrate new setup package for DBanalyzer (DBA)
#
#


# $Header: //prodprojects/vmake/dev/src/_tools/maketools/sys/src/BuildPackage/packdistrib.pl#7 $
#
# $DateTime: 2005/05/25 08:19:10 $
#
# $Change: 560346 $


my $load_path = $0;
$load_path =~ s/[\/\\][^\/\\]+$//;
if($load_path =~ /[\\\/]sys[\\\/]src[\\\/]/){
	@INC = (@INC,$load_path);
	$load_path =~ s/[\/\\][^\/\\]+$//;
}
@INC = ($load_path,@INC);
require BuildPackage::Distrib;
import BuildPackage::Distrib;

sub end_packdistrib{
	my $pid_file = $BuildPackage::Distrib::pid_file; 
	-f $pid_file && unlink($pid_file);	
	return 1;
}


eval{
	BuildPackage::Distrib::main(@ARGV);
	my $pid_file = $BuildPackage::Distrib::tmp_dir.'/pid'; 
	-f $pid_file && unlink($pid_file);
}; (print STDERR "ERR: ".$@."\n" and end_packdistrib and exit -1) if ($@);

exit 0;

1;