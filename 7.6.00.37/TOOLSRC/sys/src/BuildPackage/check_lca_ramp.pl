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


# $Header: //prodprojects/vmake/dev/src/_tools/maketools/sys/src/BuildPackage/check_lca_ramp.pl#2 $
#
# $DateTime: 2005/05/25 08:19:10 $
#
# $Change: 560346 $


use BuildPackage::Vars;


if($^O =~ /mswin/i){
	require WinLink;
	import WinLink;
}

unless($LC_STATE eq 'RAMP'){
	print STDERR "lc state isn\'t RAMP\n";
	exit -1;
}
-d $LC_POOL_DIR or exit 0; #there is no liveCache package yet

my $slot;

#resolve links
if($^O =~ /mswin/i){
	-f "$LC_POOL_DIR/LastBuild" or exit 0; #there is no lastbuild link  
	my $slotnum = readLink("$LC_POOL_DIR/LastBuild");
	-d "$LC_POOL_DIR/$slotnum"  or exit 0; #there is no such slot directory  
	$slot = "$LC_POOL_DIR/$slotnum";
}
else{
	-l "$LC_POOL_DIR/LastBuild" or exit 0; #there is no lastbuild link 		
	my $slot = readlink("$LC_POOL_DIR/LastBuild");
	-d "$LC_POOL_DIR/$slotnum"  or exit 0; #there is no such slot directory  
	$slot = "$LC_POOL_DIR/$slotnum";	
}

print "last liveCache pool slot: $slot\n";

opendir(DH,$slot) or print STDERR "cannot open slot directory: $!\n" and exit -2;
my @ramp_dirs = grep {-d "$slot/$_" && /RAMP|HOT/} readdir(DH);
closedir(DH);
$ramp_found = 0;
foreach my $rampdir (@ramp_dirs){
	-e "$rampdir/LastBuild" and $ramp_found = 1 and print STDERR "LCA RAMP in $rampdir found\n"; 
}
exit -3 if $ramp_found;

exit 0;
1;