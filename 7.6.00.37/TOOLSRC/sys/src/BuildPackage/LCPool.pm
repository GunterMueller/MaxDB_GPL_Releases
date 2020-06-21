#! /usr/bin/perl
#
# $Header: //prodprojects/vmake/dev/src/_tools/maketools/sys/src/BuildPackage/LCPool.pm#3 $
# $DateTime: 2005/05/25 08:19:10 $
# $Change: 560346 $
#
# Desc:	- subs to handle archive tree LC_POOL 
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

		
package BuildPackage::LCPool;

use BuildPackage::Vars;
use BuildPackage::Tools;
use Getopt::Long;
use WinLink;
use Cwd;
use Exporter;

@ISA = ('Exporter');

@EXPORT = ('findFreePoolSlot','linkSlot','copyItems','findPackage');


#
# initPackage() set global package vars from outside - especially needed for err msg handler 
#

sub initPackage{
	my (%args) = @_;
	foreach my $sym (keys(%args)){
		*{$sym} = $args{$sym};
	}
	return 1;
}


#
# findFreePoolSlot():
#	- searches in a pool directory $pool_root free (without $testfile) subdirs  
# 	- next free is one above latest/highest valid (slot with $testfile)		
#     - $digit_num is number of digits for slot directories (3 means from '000' up to '999')
#	- return full path of next free slot as scalar
#

sub findFreePoolSlot{
	my ($pool_root,$testfile,$digit_num,$search_pattern,$overflow_parms) = @_;
	defined $pool_root and defined $testfile or return undef;
	defined $digit_num and $digit_num =~ /^\d+$/ and $digit_num > 0 or $digit_num = 3; # default length
	my $new_dir;
	my $num_str;
	if(-d $pool_root){
		opendir(DH,$pool_root) or err("FreePoolSlot(): cannot open dir $pool_root\n");
		my @cont =  readdir(DH);
		closedir(DH);
		my @pool = sort grep {/^\d{$digit_num}$/} @cont;
		if($#pool == -1){
			if($digit_num == 4){
				
				#
				#	new slots with 4 digits in DEV area
				#	workaround to support old slots with 3 digits
				#	
				#	code can be replaced when all old slots has disappeared 
				#
				
				my @oldpool = sort grep {/^\d{3}$/} @cont;
				if ($#oldpool > -1){
					my %overflow_parm;
					$overflow_parm{'overflow'} = 0;
					my $old_slot = findFreePoolSlot($pool_root,$testfile,3,$search_pattern,\%overflow_parm);
					if($overflow_parm{'overflow'}){
						msg("convert slots to 4 digits now\n");						
						return "$pool_root/1000";
					}
					my ($old_num) = ($old_slot =~ /(\d{3})$/); 
					unless($old_num == 0){
						return $old_slot;
					}
				}
			}
			$num_str = '0' x $digit_num;
		}
		else{
			while(1){
				my $top = pop @pool;
				
				#
				# $LC_PACK_NAME_OLD and $APO_PACK_NAME_OLD are for compatibility with older version
				# should be cleaned up in next time
 				#
							
				my $apo_package_test = 0;
				foreach my $package_name (@$APO_PACK_NAME_OLD){
					-f "$pool_root/$top/$package_name" and $apo_package_test = 1 and last;
				}
				
				
				my $pattern_match = 0;
				
				if(defined $search_pattern){
					opendir(DH,"$pool_root/$top") or err("FreePoolSlot(): cannot open dir $pool_root/top\n");
					my @files = grep {-f "$pool_root/$top/$_" && /$search_pattern/ } readdir(DH);
					closedir(DH);
					if($#files > -1){
						$pattern_match = 1;
					}									
				}
				
				if(-f "$pool_root/$top/$testfile" || -f "$pool_root/$top/$LC_PACK_NAME_OLD" ||
					$apo_package_test || $pattern_match){
					if($top == (9 x $digit_num)){
						unless(defined $overflow_parms){
							err("LC_POOL slot \"$top\" already reached - cannot proceed!!!!\n");
						}
						local *oparms = $overflow_parms; 
						$oparms{'overflow'} = 1;
						$num_str = $top;
						last; 
					}
					my $num = $top + 1;
					if($num == (9 x $digit_num)){
						msg("###########################################################\n".
							"WRN: pool slot limit \"$num\" reached - no more slots available\n".
							"WRN: next packaging without changes before will fail!!!!!\n".
							"###########################################################\n");
					}
					$num_str = 0 x ($digit_num - length($num)) . $num;
					last;
				}
				
				
				if($#pool == -1){
					$num_str = '0' x $digit_num;
					last;
				}		
			}
		}
	}
	else{
		$num_str = '0' x $digit_num;
	}
	return "$pool_root/$num_str";
}


#
# copyItems() copy files and fs trees with original owner/group, timestamp and access rights 
# items = files/directories 
#

sub copyItems{
	my ($src_tree,$dest_tree,$item_list) = @_;
	-d $src_tree or return undef;
	ref($item_list) eq 'ARRAY' and $#{@$item_list} > -1 or return undef;
	my @not_found = ();
	foreach (@$item_list){
		defined $_ or next;
		-e "$src_tree/$_" or push @not_found,$_;
	}
	unless ($#not_found == -1){
		err("items:\n\t".
		    join("\n\t",@not_found)."\n".
		    "in $src_tree not found\n");
	}	

	-d $dest_tree or (makedir($dest_tree,0775) or err("cannot create directory \"$dest_tree\"\n"));
	my $tar = ($^O =~ /mswin|linux/i) ? 'tar' : 'gtar';	
	

	#
	# is not needed any more because of updated tar works correct now	
	# workaround for tar bug: cannot handle drive letters in path names
	# remove drive letter before calling tar
	#
	
	#if($^O =~ /mswin/i){
	#	$src_tree = s/^([a-zA-Z]:)\//\// and $drive_src = $1;
	#	$dest_tree = s/^([a-zA-Z]:)\//\// and $drive_dest = $1;
	#	unless($drive_src =~ /^$drive_dest$/i){
	#		err("copyItems: source and destination have different hd drives: not supported yet\n");
	#	}
	#	
	#}
	
	if($^O =~ /mswin/i){
		$src_tree =~ s/\//\\/g;
		$dest_tree =~ s/\//\\/g;	
		$src_tree =~ s/\\/\\\\/g;
		$dest_tree =~ s/\\/\\\\/g;
	}

	if($^O =~/mswin/i){
		do_cmd("$tar -C $src_tree -cpf - ".join(' ',@$item_list)." | $tar -xpf - -C $dest_tree");
	}
	else{
		do_cmd("$tar -C $src_tree -cpO ".join(' ',@$item_list)." | $tar -xpf- -C $dest_tree");
	}
	return 1;
}


#
# linkSlot(): create a symblolic link $link_name in $pool_root to given slot
#

sub linkSlot{
	my ($slot_path,$link_name) = @_;
	-d $slot_path or err("linkSlot(): directory $slot_path not found\n");
	my ($pool_root,$slot) = ($slot_path =~ /^(.+)\/(\d+)\/{0,1}$/);
	my $cwd = getcwd();
	chdir($pool_root) || err("linkSlot(): cannot change into directory $pool_root: $!\n");
	if($^O =~ /mswin/i){
		-e $link_name and (unlink($link_name) or err("cannot remove existing $link_name: $!\n"));  
		createLink($slot,$link_name) or err("linkSlot(): cannot create link \"$link_name\" to slot $slot: $!\n");
	}
	else{
		if(-e $link_name){
			unless( -l $link_name){
				err("linkSlot(): $pool_root/$link_name exists and is no link\n");		
			}
			unlink($link_name) or err("linkSlot(): cannot remove symlink $pool_root/$link_name: $!\n"); 
		}
		symlink($slot,$link_name) or err("linkSlot(): cannot create link \"$link_name\" to slot $slot\n"); 
	}
	chdir($cwd) or err("linkSlot(): cannot change into directory $cwd: $!\n");
	return 1;
}



sub findPackage{
	local @ARGV = @_;
	
	my %optCtrl = (
		'type=s' => \$opt_type,
		'lcversion=n' => \$opt_lcversion,
		'comversion=s' => \$opt_comversion,
		'lcstate=s' => \$opt_lcstate,
		'comstate=s' => \$opt_comstate,
		'comvariant=s' => \$opt_comvariant,
		'limit=n' => \$opt_limit,
		'bit=n' => \$opt_bit,
		'sort=s' => \$opt_sort,
		'l' => \$opt_list,
		'h' => \$opt_help, 
		'help' => \$opt_help
	);


	my $usage = "[-l] [-h|--help] [-type SAPDB|LCA]\n".
				"\t[-lcversion <lc version>] [-comversion <lca version>]\n".
				"\t[-lcstate DEV|RAMP] [-comstate COR|DEV|RAMP]\n".
				"\t[-limit <max number of listed packages>] \n".
				"\t[-sort name|date|size] [-bit 32|64]\n".
				"\t[-comvariant Release|Debug]\n".
				"\t\t\n".
				"\t\t\n".
				"\t\t\n";

	sub do_opt_check{
		my $rc = 1;
		if(defined $opt_type){
			unless($opt_type =~ /^SAPDB$|^LCA$/){
				print "\n-type: wrong usage\n";
				$rc = 0;
			}
		}
		if(defined $opt_lcstate){
			unless($opt_lcstate =~ /^DEV$|^RAMP$/){
				print "\n-lcstate: wrong usage\n";
				$rc = 0;
			}
		}
		
		if(defined $opt_sort){
			unless($opt_sort =~ /^date$|^size$|^name$/){
				print "\n-sort: wrong usage\n";
				$rc=0;
			}
		}
		
		if(defined $opt_limit){
			unless($opt_limit =~ /^\d+$/ && $opt_limit > 0){
				print "\n-limit: wrong usage\n";
				$rc=0;
			}
		}

		if(defined $opt_comstate){
			unless($opt_comstate =~ /^COR$|^DEV$|^RAMP$/){
				print "\n-comstate: wrong usage\n";
				$rc = 0;
			}
		}

		if(defined $opt_lcversion){
			unless($opt_lcversion =~ /^[78][0-4][0-2][0-9][0-2][0-9]$/ || $opt_lcversion =~ /^[78][0-4][0-2][0-9]$/){
				print "\n-lcversion: wrong usage\n";
				$rc = 0;
			}
		}

		if(defined $opt_comversion){
			unless($opt_comversion =~ /^[3][01][0A][0-9]{2}$/){
				print "\n-comversion: wrong usage\n";
				$rc = 0;
			}
		}

		if(defined $opt_bit){
			unless($opt_bit =~ /^32$|^64$/){
				print "\n-bit: wrong usage\n";
				$rc = 0;
			}
		}
		
		if(defined $opt_comvariant){
			unless($opt_comvariant =~ /^Debug$|^Release$/){
				print "\n-comvariant: wrong usage\n";
				$rc = 0;
			}
		}

		return $rc;
	}
	
	
	
	if(! GetOptions(%optCtrl) || $opt_help || ! do_opt_check){
		my $program = $0;
		$program =~ s/\\/\//g;
		$program =~ s/^.*\/([^\/]+$)/$1/;
		$program =~ s/\.pl$//i;
		print "\nusage: $program  $usage\n";
		if($opt_help){
			exit 0;
		}
		exit -1;
	}

		
	# set search pattern
	
	my $lc_pool_root = '/LC_POOL';
	my $lc_pat = '^LC_';
	my $lca_pat = '^LCAPPS_';
	my %packages;

	if(defined $opt_lcversion){
		$lc_pat .= $opt_lcversion.'_';
	}
	else{						# 30.08.2002
		$lc_pat .= '\d{4,6}_';  # accept 4 digits too, caused by new directory 
	}							# structure of DEV packages without build prefix

	if(defined $opt_bit){
		$lc_pat .= $opt_bit.'_';
	}
	else{
		$lc_pat .= '\d{2}_';
	}
	
	if(defined $opt_lcstate){
		$lc_pat .= $opt_lcstate.'$';
	}
	else{
		$lc_pat .= '[A-Z]{3,4}$';
	}

	#if( !$opt_type  || $opt_type eq 'LCA'){
	
	if(defined $opt_comversion){
		$lca_pat .= $opt_comversion.'_';
	}
	else{
		$lca_pat .= '[0-9A]{5,6}_';
	}		
	if(defined $opt_comvariant){
		$lca_pat .= $opt_comvariant.'_';
	}
	else{
		$lca_pat .= '\w{5,7}_';
	}
	if(defined $opt_comstate){
		$lca_pat .= $opt_comstate.'$';
	}
	else{
		$lca_pat .= '\w{3,4}$';
	}

	#LCAPPS_${APO_SHORT_VERS}_${APO_VARIANT}_${APO_STATE}	
	


		
	opendir(DH,$lc_pool_root) or die("cannot open directory $lc_pool_root:$!\n");
	my @possible_lc_dirs = grep {-d "$lc_pool_root/$_" && /$lc_pat/} readdir (DH);
	closedir(DH);
	#$#possible_lc_dirs == -1 and print "no package found\n" and exit 0;
	foreach my $dir (@possible_lc_dirs){
		my $dir_path = $lc_pool_root.'/'.$dir; 
		opendir (DH,$dir_path) or die("cannot open directory $dir_path:$!\n");
		my @slots = grep {-d "$dir_path/$_" && /^\d{3}$/} readdir(DH);
		closedir(DH);
		foreach my $slot (@slots){
			my $slot_path = $dir_path.'/'.$slot;
			opendir (DH,$slot_path) or die("cannot open directory $slot_path:$!\n");
			my @all_cont = readdir(DH);
			closedir(DH);
			
			unless($opt_type eq 'LCA'){
				my @lc_packs = grep {-f "$slot_path/$_" && (/^sapdb-server.*\.sar$/ || /^SAPDB\d.*\.SAR$/ )} @all_cont;
				foreach my $lc_pack (@lc_packs){
					my $lc_pack_path = $slot_path.'/'.$lc_pack;
					my @statbuf = stat($lc_pack_path) or die("cannot stat $lc_pack_path: $!\n"); 
					$packages{$lc_pack_path} = {'date' => $statbuf[9],
						'size' => $statbuf[7]};
				}

			}
			unless($opt_type eq 'SAPDB'){
				my @lcapps_dirs	= grep { -d "$slot_path/$_" && /$lca_pat/} @all_cont;	
				foreach my $lca_root (@lcapps_dirs){
					my $lca_root_path = $slot_path.'/'.$lca_root;
					opendir(DH,$lca_root_path) or die("cannot open dir $lc_root_path: $!\n");
					my @lca_slots = grep { -d "$lca_root_path/$_" && /^\d{3}$/ } readdir(DH);
					closedir(DH);
					foreach my $lca_slot (@lca_slots){
						my $lca_slot_path = $lca_root_path.'/'.$lca_slot;
						opendir(DH,$lca_slot_path) or die("cannot open dir $lc_slot_path: $!\n");
						my @lca_packages = grep { -f "$lca_slot_path/$_" && /^LC.*\.SAR$|^sapdb-apo.*\.sar$/} readdir(DH);
						closedir(DH);	
						foreach my $lca_pack (@lca_packages){
							my $lca_pack_path = $lca_slot_path.'/'.$lca_pack;
							my @statbuf = stat($lca_pack_path) or die("cannot stat $lca_pack_path: $!\n"); 
							$packages{$lca_pack_path} = {'date' => $statbuf[9],
								'size' => $statbuf[7]};
							}
					}
				}
			}
		}
	}
	
	
	
	# sort

	my @show;

	if($opt_sort eq 'name'){
		@show = sort keys %packages;
 
	}
	elsif($opt_sort eq 'date'){
		@show = sort { $packages{$b}->{'date'} <=> $packages{$a}->{'date'} } keys %packages;
	}
	elsif($opt_sort eq 'size'){
		@show = sort { $packages{$b}->{'size'} <=> $packages{$a}->{'size'} } keys %packages;
	}
	else{
		@show = keys %packages;
	}





	# output
	
	my $count = 0;
	
	foreach my $file (@show){
		defined $opt_limit && $count >= $opt_limit && last;
		if ($opt_list){
			my $date = localtime($packages{$file}->{'date'}); 
			print $packages{$file}->{'size'}.' bytes - '.$date.' - '.$file."\n"; 	
		}
		else{
			print $file."\n"; 	
		}
		$count++;
	}	
	
	

}


1;

