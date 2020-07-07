#! /usr/bin/perl
#
# $Header: //prodprojects/vmake/dev/src/_tools/maketools/sys/src/BuildPackage/Distrib.pm#64 $
# $DateTime: 2006/05/16 15:05:36 $
# $Change: 592950 $
#
# Desc:	- generate distribution packages with software components from make machines
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

		
package BuildPackage::Distrib;


use BuildPackage::Vars;
use BuildPackage::Tools;
use BuildPackage::Hosts;
use BuildPackage::SysName;
use BuildPackage::Version;

use Getopt::Long;
use WinLink;
use Cwd;



sub resolveLink{
	my ($ftp,$root,$linkdst) = @_;
	my $resolved;

	if($linkdst =~ /\//){
		if($linkdst =~ /^\//){
			$resolved = $linkdst;
		}
		else{
			# relative path
			$resolved = $root.'/'.$linkdst;
		}
		if($resolved =~ /\/\.{1,2}\//){
			$resolved =~ s/\/\.\//\//g;
			$resolved =~ s/\/[^\/]+\/\.\.\//\//g;	
		}
		($root,$linkdst) = ($resolved =~ /(.+)\/([^\/]+)/);		
	}

	my %entries = remoteLs($ftp,$root);
	unless(exists $entries{$linkdst}){
		return undef;
	}
	if($entries{$linkdst}->{'type'} eq 'l'){
		return resolveLink($ftp,$root,$entries{$linkdst}->{'dest'}); 
	}
	else{
		return "$root/$linkdst";
	}
}


sub err{
	my ($errtext) = @_;
	die($errtext);
}


BuildPackage::Tools::setMsgHandler(sub {print $_[0] if($opt_verbose);});
BuildPackage::Tools::setErrMsgHandler(sub {die $_[0];});


my %sapdb_config = ('state' => undef,
					'version' => undef,
					'changelist' => undef,
					'pool_id' => undef,
					'make_id' => undef);

my %lca_config = (	'state' => undef,
					'variant' => undef,
					'changelist' => undef,
					'pool_id' => undef,
					'make_id' => undef);

my $machine_config = (	'os' => undef,
						'mach' => undef,
						'mode' => undef);



my %replacements;

my $cwd = cwd();
my $dest_dir = $cwd;

my $ftp;

sub do_opt_check{
	my $rc = 1;
	
	if (defined $opt_isolated_db_studio){
		$opt_profile = 'Database Studio';
		$opt_format = 'exe';
		$opt_os_style = 1;
	}

	
	unless((defined $opt_profile && $opt_profile =~ /\S/) || $opt_get_archives ||
			$opt_list_profiles || $opt_component_group){
		print "\nwrong usage - no profile\n\n";
		$rc = 0;
	}
	if(defined $opt_component_group){
		unless($opt_component_group =~ /^Client$|^Server\+Client$/){
			print "\nwrong usage -component_group $opt_component_group: only Server+Client and Client possible\n";
			$rc = 0;
		}
		if(defined $opt_profile){
			print STDERR ("WRN: -component_group overloads -profile\n");
		}
	}
	
	if(defined $opt_format){
		unless($opt_format =~ /^TGZ$|^ZIP$|^SAR$|^RPM$|^EXE$/i){
			print "\n-format: wrong usage\n\n";
			$rc = 0;
		}
		if($opt_format =~ /^RPM$/i){
			if(defined $opt_os){
				unless ($opt_os =~ /^LINUX/){
					print "\n-format: wrong usage - RPM is for linux only\n\n";
					$rc = 0;
				}

			}
			elsif($^O !~ /linux/i){
				print "\n-format: wrong usage - RPM is for linux only\n\n";
				$rc = 0;
			}
			if($opt_os_style){
				print "\n-os_style: wrong usage - RPM has no special open source style\n\n";
				$rc = 0;
			}
		}
		if($opt_format =~ /zip|exe/i){
			eval{
				require BuildPackage::PackZip;
			};
			if($@){
				print "\n-format $opt_format: error: cannot create zip or self extracting archive:\n".
					 "\tPKZIP extension not available. For MaxDB development environment\n".
					  "\ttry \"imf ::tt/cpan/SAPDB/Install/xs.mac\" in Devtools\n".
					  "\tto create this extension.\n\n";
				$rc = 0;
			}
		}
	}
		
	if(defined $opt_state){
		unless($opt_state =~ /^DEV$|^COR$|^HOT$|^ANA$/){
			print "\n-state: wrong usage\n\n";
			$rc = 0;
		}
	}
	
	if(defined $opt_distrib_type){
		my $failed = 0;
		foreach my $type (split(',',$opt_distrib_type)){
			if ($type ne 'OS' and $type ne 'BETA'){
					$failed = 1;
					last;	
			}
		}
		if ($failed){
			print "\n-distrib_type: wrong usage\n\n";
			$rc = 0;	
		}
		else{
			if ($opt_distrib_type =~ /OS/){
				unless ($opt_no_debug_package){
					$opt_no_debug_package = 1;
				}
				unless ($opt_os_style){
					$opt_os_style = 1;
				}
				unless ($opt_format){
					if ((defined $opt_os and $opt_os =~ /WIN/) or
						(not defined $opt_os and $^O =~ /mswin/i)){
						$opt_format = 'exe';
						eval{
							require BuildPackage::PackZip;
						};
						if($@){
							print "\n-distrib_type OS: cannot self extracting archive:\n".
							"\tPKZIP extension not available. For MaxDB development environment\n".
							"\ttry \"imf ::tt/cpan/SAPDB/Install/xs.mac\" in Devtools\n".
							"\tto create this extension.\n\n";
							$rc = 0;
						}
					}
					else{
						$opt_format = 'tgz';	
					}	
				}
			}
		}
	}
	
	if(defined $opt_version){
		if($opt_version =~ /\./){
			
			#
			#	normalizing version string
			#
			
			my ($ma,$mi,$co,$bu) = ($opt_version =~ /^(\d)\.(\d)\.(\d{1,2})\.(\d{1,2})$/);
			
			unless($mi =~ /\d/){
				($ma,$mi,$co) = ($opt_version =~ /^(\d)\.(\d)\.(\d{1,2})$/);
			}
			
			if($ma =~ /\d/ and $mi =~ /\d/ and $co =~ /\d/){
				$co = '0'.$co if(length($co) == 1);
				$opt_version = $ma.$mi.$co;
				if($bu =~ /\d/){
					$bu = '0'.$bu if(length($bu) == 1);
					$opt_version .= $bu
				}
			}
			else{
				$rc = 0;
			}	
		}
		if($opt_version !~ /^7[234567]0[0-5][0-6][0-9]$|^7[234567]0[0-3]$/){
			print "\n-version: wrong usage\n\n";
			$rc = 0;
		}
		else{
			if(length($opt_version) == 4){
				unless($opt_state =~ /^DEV$|^COR$/){
					print "\n-version: wrong usage:  need -state COR|DEV\n\n";
					$rc = 0;
				}
			}
			else{
				if($opt_state =~ /\S/ and $opt_state !~ /^HOT$|^ANA$/){
					print "\n-state: wrong usage: only HOT or ANA possible,\n\twithout this switch means RAMP\n\n";
					$rc = 0;
				}
			}
		}
	
	}

	if(defined $opt_state){
		unless($opt_version =~ /^7[234567]0[0-5][0-6][0-9]$|^7[234567]0[0-5]$/){
			print "\n-lcstate: wrong usage\n\n";
			$rc = 0;
		}
	}

	

	if(defined $opt_dir){
		if(-d $opt_dir){
			$dest_dir = $opt_dir;
			if(($^O =~ /mswin/i && $dest_dir !~ /^[a-z]:/i) || ($^O !~ /mswin/i && $dest_dir !~ /^\//)){
				$dest_dir = $cwd.'/'.$opt_dir;
			}
			$dest_dir =~ s/\\/\//g;			
		}
		else{
			print "\n-dir $opt_dir: wrong usage - no such directory\n\n";
			$rc = 0;
		}

	}

	unless($opt_os){
		$sys = getsysinfo();
		if($sys->{'bp_name'} =~ /\S/){
			$opt_os = $sys->{'bp_name'};
		}
		if($sys->{'os'} =~ /\S/){
			$machine_config->{'os'} = $sys->{'os'};
		}
		if($sys->{'mach'} =~ /\S/){
			$machine_config->{'mach'} = $sys->{'mach'};
		}
	}
	
	
	if($opt_os){
		my $found;
		unless(exists $OSS{$opt_os}){
			my ($version) = ($opt_version =~ /^(\d{4})/);
			if(exists $OS_ALIAS->{$opt_os}){
				if (exists $OS_ALIAS->{$opt_os}->{$version}){
					$opt_os = $OS_ALIAS->{$opt_os}->{$version};
				}
				else{
					print "\n-os: wrong usage - invalid operating system\n";
					print "operating system alias $opt_os is not valid for release $opt_version\n";
					print "it\'s valid for releases: ".join (' ',sort(keys(%{$OS_ALIAS->{$opt_os}})))."\n\n";
					$rc=0
				}
			}
			else{
				print "\n-os: wrong usage - invalid operating system\n";
				print "valid systems: ".join(' ',sort (keys(%OSS),keys(%$OS_ALIAS)))."\n\n";
				$rc=0
			}
		}

	}
	else{
		print "\nmissing option -os <operating system>\n\n";
		$rc = 0;
	}
	
	if($opt_gui_setups && $opt_os !~ /win/i ){
		print "\n-gui_setups: invalid operating system\n\n";
		$rc = 0;
	}

	
	return $rc;
}



$tmp_dir = '/tmp';
exists $ENV{'TMP'} and -d $ENV{'TMP'} and $tmp_dir = $ENV{'TMP'}; 
$tmp_dir = (exists $ENV{TEMP} ? $ENV{TEMP} : $ENV{TMP}) if $^O =~ /mswin/i;


sub loadModule{
	my ($module,$path,$remote_host,$replacements) = @_;
	
	if ($remote_host){
		my ($modpath,$modfile) = ($module =~ /(.*)::([^:]+)$/);
		$modfile .= '.pm';
		$modpath =~ s/::$//;
		$modpath =~ s/::/\//g;

		#
		# create local tmp_path
		#
		makedir("$tmp_dir/$modpath",0775) or die("cannot create \"$tmp_dir/modpath\"\n");
		
		
		#
		# copy module into local tmp_path
		#
		
		remoteGet($ftp,$path."/$modpath/$modfile","$tmp_dir/$modpath/$modfile");
		
		if(defined $replacements && ref($replacements) eq 'HASH'){
			open(FD,"$tmp_dir/$modpath/$modfile") or die ("cannot open file $tmp_dir/$modpath/$modfile: $!\n");
			my @buf;
			while(<FD>){ chomp; push @buf,$_;}
			close(FD);
			open(FD,">$tmp_dir/$modpath/$modfile") or die ("cannot create file $tmp_dir/$modpath/$modfile: $!\n");
			foreach my $line (@buf){
				foreach my $repl (keys(%$replacements)){
					my $with = $replacements->{$repl};
					$line =~ s/$repl/$with/g;
				}
				print FD $line."\n";
			}
			close(FD);	
		}
		
		loadModule($module,$tmp_dir);	
	}
	else{
		-d $path or die("loadPackDefs(): directory \"$path\" not found\n");
		my $module_file = $module.'.pm';
		$module_file =~ s/::/\//g;
		-f "$path/$module_file" or die("loadPackDefs(): module \"$module\" not found\n");
		$path =~ s/\//\\/g if ($CUR_OS =~ /WIN/); 
		$path = 'd:'.$path if ($CUR_OS =~ /WIN/ && ! $path =~ /^[a-z]:/i); 
		local @INC = ($path);
		eval "require $module";
		die($@) if $@;
		import $module;
		foreach my $key (keys(%profiles)){
			$opt_verbose && print ">>>>>>$key\n";
		} 
	}
	return 1;
}




sub getPoolPath{
	my $pool_path = '';
	my $buildselect = ($opt_buildselect ? $opt_buildselect : 'LastBuild');
	if(!$opt_os){
		#
		# local
		#
		my $mode = ($CUR_OS =~ /64|390X/ ? '64' : '32');	
		my $state = $LC_STATE;
		$state = 'DEV' unless($state);
		$sapdb_config{'state'} = $state;
		$opt_version =~ s/\d\d$// if($state =~ /^DEV$|^COR$/ and length($opt_version) == 6);
		if($opt_version){
			if($opt_old){
				$pool_path = $SAPDB_POOL_ROOT; 
				$pool_path .= "/SDBC_${opt_version}_${mode}_${state}";
			}
			else{
				$pool_path = $LC_POOL_ROOT; 
				$pool_path .= "/LC_${opt_version}_${mode}_${state}";
			}
		}
		else{
			# version of dev environment
			if($opt_old){
				$pool_path = $SAPDB_POOL_DIR;
			}
			else{
				$pool_path = $LC_POOL_DIR;
			}

		}
		
		-e "$pool_path/$buildselect" or die("cannot find pool slot \"$pool_path/$buildselect\"\n");		
		$sapdb_config{'pool_id'} = $buildselect;
		my %config = getMakeIDs("$pool_path/$buildselect");
		$sapdb_config{'make_id'} = $config{'make.id'};
		$sapdb_config{'changelist'} = $config{'changelist.log'};
		$sapdb_config{'version'} = $config{'VERSIONS'}->{'kernel'}->{'version'};
		-d "$pool_path/$buildselect" and return "$pool_path/$buildselect";
		return "$pool_path/".(readLink ("$pool_path/$buildselect")) if($CUR_OS =~ /WIN/ && -f "$pool_path/$buildselect");
	
	}
	elsif($opt_version){
		#
		# remote 
		#
		my $mode = ($opt_os =~ /64|390X/ ? '64' : '32');	
		my $state = ((length($opt_version) == 6 and $opt_state !~ /^HOT$|^ANA$/) ? 'RAMP' : $opt_state);
		my ($version) = ($opt_version =~ /^(\d{4})/);
		exists $HOSTS{$version} or die("no host defined for version $version\n");
		exists ${%{$HOSTS{$version}}}{$opt_os}  or die("no host defined for os $opt_os\n");
		my $host = ${%{$HOSTS{$version}}}{$opt_os}->{'name'};
		if($opt_host =~ /\S/){
			$host = $opt_host;
		}
		#$opt_verbose && print "host = $host\n";	
		if(exists $HOSTS{$version}->{$opt_os}->{'LC_POOL_ROOT'}){
			$LC_POOL_ROOT = $HOSTS{$version}->{$opt_os}->{'LC_POOL_ROOT'};
		}
		
		if($opt_lc_pool_root =~ /\S/){
			$LC_POOL_ROOT = $opt_lc_pool_root; 
		}
		
		$sapdb_config{'state'} = $state;
		if($opt_old){
			$pool_path = $SAPDB_POOL_ROOT."/SDBC_${opt_version}_${mode}_${state}";
		}
		else{
			$pool_path = $LC_POOL_ROOT."/LC_${opt_version}_${mode}_${state}";
		}
		$opt_verbose && print ">>>>>path = $pool_path\n";
		$ftp = ftp_connect($host,$opt_os eq 'LINUX64' ? ('remuser','Remuser') : ());  
		my %entries = remoteLs($ftp,$pool_path);
		exists $entries{$buildselect} or err("buildselect \"$buildselect\" not found\n"); 
		if($entries{$buildselect}->{'type'} eq 'd'){
			$opt_verbose && print ">>>>> final pool path = $pool_path/$buildselect\n";
			$sapdb_config{'pool_id'} = $buildselect;
			my %config = getMakeIDs($pool_path."/$buildselect");
			$sapdb_config{'make_id'} = $config{'make.id'};
			$sapdb_config{'changelist'} = $config{'changelist.log'};
			$sapdb_config{'version'} = $config{'VERSIONS'}->{'kernel'}->{'version'};
			return ($pool_path."/$buildselect",$host);
		}
		elsif($entries{$buildselect}->{'type'} eq 'l'){
			if($entries{$buildselect}->{'dest'} =~ /\//){
				my $resolved = resolveLink($host,$pool_path,$entries{$buildselect}->{'dest'});
				unless(defined $resolved){
					err("cannot resolve link"); 
				}
				$sapdb_config{'pool_id'} = $buildselect;
				my %config = getMakeIDs($resolved);
				$sapdb_config{'make_id'} = $config{'make.id'};
				$sapdb_config{'changelist'} = $config{'changelist.log'};
				$sapdb_config{'version'} = $config{'VERSIONS'}->{'kernel'}->{'version'};
				return ($resolved,$host);
			}
			else{
				exists $entries{$entries{$buildselect}->{'dest'}} or 
				err("buildselect \"$buildselect\"not found: cannot resolve link\n");
			}
			if($entries{$entries{$buildselect}->{'dest'}}->{'type'} eq 'd'){
				$opt_verbose && print ">>>>> final pool path = $pool_path/".$entries{$buildselect}->{'dest'}."\n";
				$sapdb_config{'pool_id'} = $entries{$buildselect}->{'dest'};
				my %config = getMakeIDs($pool_path.'/'.$entries{$buildselect}->{'dest'});
				$sapdb_config{'make_id'} = $config{'make.id'};
				$sapdb_config{'changelist'} = $config{'changelist.log'};
				$sapdb_config{'version'} = $config{'VERSIONS'}->{'kernel'}->{'version'};
				return ($pool_path.'/'.$entries{$buildselect}->{'dest'},$host);
			}
			elsif($entries{$entries{$buildselect}->{'dest'}}->{'type'} eq 'l'){
					my $resolved = resolveLink($host,$pool_path,$entries{$entries{$buildselect}->{'dest'}}->{'dest'});
					unless(defined $resolved){
						err("cannot resolve link"); 
					}
					$sapdb_config{'pool_id'} = $entries{$buildselect}->{'dest'};
					my %config = getMakeIDs($resolved);
					$sapdb_config{'make_id'} = $config{'make.id'};
					$sapdb_config{'changelist'} = $config{'changelist.log'};
					$sapdb_config{'version'} = $config{'VERSIONS'}->{'kernel'}->{'version'};
					return ($resolved,$host);
			}
			else{
				err("resolved link is no directory\n");
			} 
		}
		else{
			# windows
			remoteGet($ftp,$pool_path.'/'.$buildselect,$tmp_dir.'/'.$buildselect);
			my $resolved = readLink($tmp_dir.'/'.$buildselect);
			$resolved =~ s/\r//g;
			exists $entries{$resolved} or err(" resolved buildselect \"$resolved\" not found\n"); 
			if($entries{$resolved}->{'type'} eq 'd'){
				$opt_verbose && print ">>>>> final pool path = $pool_path/$resolved\n";
				$sapdb_config{'pool_id'} = $resolved;
				my %config = getMakeIDs($pool_path.'/'.$resolved);
				$sapdb_config{'make_id'} = $config{'make.id'};
				$sapdb_config{'changelist'} = $config{'changelist.log'};
				$sapdb_config{'version'} = $config{'VERSIONS'}->{'kernel'}->{'version'};
				return ($pool_path.'/'.$resolved,$host);
			}
			else{
				err("resolved link \"$resolved\" is no directory\n");
			}
		}
		
		
		#remoteLs(p26325,'/CD-Images/db_rel');
		
		die("remote pool: not implemented yet\n");
		
	}
	else{
		die("cannot get pool path: version not known\n");
	}
}


sub getLCAPoolPath{
	my ($pool_path) = @_;
	my $buildselect = ($opt_lca_buildselect ? $opt_lca_buildselect : 'LastBuild');
	my $mode = ($CUR_OS =~ /64|390X/ ? '64' : '32');	
	# remote 
	#
	my $lca_pool_path = $pool_path;
	
	my $mode = ($opt_os =~ /64|390X/ ? '64' : '32');
	my $lca_state = $opt_lca_state =~ /^DEV$|^COR$|^CONS$|^RAMP$|^HOT$/ ? $opt_lca_state : 'RAMP';
	my $lca_variant = '';
	if ($opt_lca_variant =~ /opt|dbg/){
		$lca_variant = $opt_lca_variant;
	}
	else{
		$lca_variant = 'opt'
	}
	
	$lca_config{'variant'} = $lca_variant;
	$lca_config{'state'} = $lca_state;


	$lca_pool_path .= '/'.'LCAPPS_'.$opt_lca_version.'_'.$lca_variant.'_'.$lca_state;


	unless(defined $ftp){	
		my ($version) = ($opt_version =~ /^(\d{4})/);
		exists $HOSTS{$version} or die("no host defined for version $version\n");
		exists ${%{$HOSTS{$version}}}{$opt_os}  or die("no host defined for os $opt_os\n");
		my $host = ${%{$HOSTS{$version}}}{$opt_os}->{'name'};
			$pool_path .= "/LCAPPS_${opt_lca_version}_${mode}_${lca_variant}${lca_state}";
		$opt_verbose && print ">>>>>path = $pool_path\n";
		$ftp = ftp_connect($host);  
	}
	
		
	my %entries = remoteLs($ftp,$lca_pool_path);
	exists $entries{$buildselect} or err("buildselect \"$buildselect\" not found\n"); 
	
		
	
	if($entries{$buildselect}->{'type'} eq 'd'){
		$opt_verbose && print ">>>>> final pool path = $lca_pool_path/$buildselect\n";
		$lca_config{'pool_id'} = $buildselect; 
		my %config = getMakeIDs($lca_pool_path."/$buildselect");
		$lca_config{'make_id'} = $config{'make.id'};
		$lca_config{'changelist'} = $config{'changelist.log'};
		return $lca_pool_path."/$buildselect";
	}
	elsif($entries{$buildselect}->{'type'} eq 'l'){
		exists $entries{$entries{$buildselect}->{'dest'}} or 
			err("buildselect \"$buildselect\"not found: cannot resolve link\n");
		if($entries{$entries{$buildselect}->{'dest'}}->{'type'} eq 'd'){
			$opt_verbose && print ">>>>> final lca pool path = $pool_path/".$entries{$buildselect}->{'dest'}."\n";
			$lca_config{'pool_id'} = $entries{$buildselect}->{'dest'}; 
			my %config = getMakeIDs($lca_pool_path.'/'.$entries{$buildselect}->{'dest'});
			$lca_config{'make_id'} = $config{'make.id'};
			$lca_config{'changelist'} = $config{'changelist.log'};
			return $lca_pool_path.'/'.$entries{$buildselect}->{'dest'};
		}
		else{
			err("resolved link is no directory\n");
		}
		 
	}
	else{
		# windows
		remoteGet($ftp,$lca_pool_path.'/'.$buildselect,$tmp_dir.'/'.$buildselect);
		my $resolved = readLink($tmp_dir.'/'.$buildselect);
		$resolved =~ s/\r//g;
		exists $entries{$resolved} or err(" resolved buildselect \"$resolved\" not found\n"); 
		if($entries{$resolved}->{'type'} eq 'd'){
			$opt_verbose && print ">>>>> final lca pool path = $lca_pool_path/$resolved\n";
			$lca_config{'pool_id'} = $resolved; 
			my %config = getMakeIDs($lca_pool_path.'/'.$resolved);
			$lca_config{'make_id'} = $config{'make.id'};
			$lca_config{'changelist'} = $config{'changelist.log'};
			return $lca_pool_path.'/'.$resolved;
		}
		else{
			err("resolved link \"$resolved\" is no directory\n");
		}
	}
		
	
	#remoteLs(p26325,'/CD-Images/db_rel');
	
	die("remote pool: not implemented yet\n");

}




sub getArchiveBuildNum{
	my ($archive) = @_;
	my $local_dir = 'getBuildNum'; 
	my $build;
	my $mycwd = cwd();
	chdir($tmp_dir);
	makedir($local_dir,0775);
	chdir($local_dir);
	my $cmd = ($^O =~ /mswin|linux/i ? 'tar' : 'gtar' )." -xvpzf \"$archive\" PACKAGEDATA";
	if($^O =~ /mswin/i){
		$cmd =~ s/[a-z]://i;
		$cmd =~ s/\//\\/g;
		#$cmd =~ s/\\/\\\\/g;
	}
	do_cmd($cmd);	
	my $version;
	open(FD,'PACKAGEDATA') or die("cannot open PACKAGEDATA\n");
	while(<FD>){
		/^SOFTWARE_VERSION\s*=\s*"(.+)"/ and $version = $1 and last;
	}
	close(FD);
	if(defined $version){
		($build) = ($version =~ /(\d\d)$/); 
	}
	else{
		die("cannot get build\n");
	}
	chdir($tmp_dir);
	deltree($local_dir);
	chdir($mycwd);
	return $build;
}


sub getMakeIDs{
	my ($pool_path) = @_;
	my $local_dir = 'getMakeIDs'; 
	my $build;
	my $mycwd = cwd();
	chdir($tmp_dir);
	makedir($local_dir,0775);
	my %entries = remoteLs($ftp,$pool_path);
	my %return_value;
	foreach my $file ('make.id','changelist.log'){
		if(exists $entries{$file} and $entries{$file}->{'type'} eq 'f'){
			remoteGet($ftp,$pool_path.'/'.$file,$local_dir.'/'.$file);
			open(FH,"$local_dir/$file") or err("cannot open $local_dir/$file\n");
			while(<FH>){
				chomp;
				$return_value{$file} = $_;
			}	
			close(FH); 
		}
	}
	
	unless($pool_path =~ /\/LCAPPS/){
		my $file = 'VERSIONS';
		if(exists $entries{$file} and $entries{$file}->{'type'} eq 'f'){
			remoteGet($ftp,$pool_path.'/'.$file,$local_dir.'/'.$file);
			my %data = readini($local_dir.'/'.$file);
			$return_value{$file} = \%data; 
		}
		
	}

	
	deltree($local_dir);
	chdir($mycwd);
	return %return_value;
}



sub getRPMs{
	my ($src_dir,$files,$remote_host) = @_;
	
	my $buildroot = "$tmp_dir/buildroot";
	-d $buildroot and deltree($buildroot);
	makedir("$buildroot/".$inner_subdir,0775) or die("cannot create directory \"$inner_sub_dir\": $!\n");
		
	if($remote_host){
		my %entries = remoteLs($ftp,$src_dir);
		print "get files...\n";
		my @rpm_files = ();
		foreach my $file (@$files){
			my $pattern = $file;
			$pattern =~ s/\//\\\//g;
			foreach my $repl (keys(%replacements)){
				$pattern =~ s/$repl/$replacements{$repl}/g;
			}
			$pattern =~ s/%build/\\d+/g;

			$pattern =~ s/\.i386\./\.i\\d86\./g if $opt_os eq 'LINUX32';
			$pattern =~ s/\.i386\./\.ia64\./g if $opt_os =~ /LINUX.*IA64|LINUX64/;
			$pattern =~ s/\.i386\./\.ppc64\./g if $opt_os eq 'LINUXPPC64';
			$pattern =~ s/\.i386\./\.x86_64\./g if $opt_os eq 'LINUXX8664';
			$pattern =~ s/\.i386\./\.s390x\./g if $opt_os eq 'LINUXS390X';

			my $found_rpm = 0;
			foreach my $found_file (keys(%entries)){
				if($found_file =~ /^$pattern$/){
					$found_rpm = 1;
					$entries{$found_file}->{'type'} eq 'f' or err("RPM $found_file isn\'t a file\n");
					push @rpm_files,$found_file;
					last;
				}
			}
			unless($found_rpm){
				err("no matching RPM found: \"$file\"\n");
			}
		}	

		foreach my $rpm (@rpm_files){
			print "+ $rpm\n";
			remoteGet($ftp,"$src_dir/$rpm","$buildroot/$rpm",1);
			unless($^O =~ /mswin/i){
				if($entries{$rpm}->{'mode'}){
					chmod($entries{$rpm}->{'mode'},"$buildroot/$rpm");
				}
			}
			utime($entries{$rpm}->{'time'},$entries{$rpm}->{'time'},"$buildroot/$rpm");			
			copy("$buildroot/$rpm",$dest_dir,{'binmode' => 1});
		}
		ftp_release($ftp);
		deltree($buildroot);
	}
	else{
		err("RPMs local not yet implemeted\n");
	}
	return 1;
}


sub getArchives{
	my ($pool_path,$archive_opt) = @_;
	
	my $src_dir = "$pool_path/archives";
	my $buildroot = "$tmp_dir/buildroot";
	-d $buildroot and deltree($buildroot);
	makedir($buildroot,0775) or die("cannot create directory \"$buildroot\": $!\n");
		
	my %entries = remoteLs($ftp,$src_dir);
	my $pattern = '^'.$archive_opt.'$';
	$pattern =~ s/\?/\./g;
	$pattern =~ s/\*/\.\*/g;
	
	my @files;
	foreach my $file (keys(%entries)){
		if($entries{$file}->{'type'} eq 'f'){
			if($file =~ /$pattern/){
				push @files,$file;
			}
		}
	}
	if($#files > -1){
		print "getting files...\n";
	}
	else{
		print "no matching archives found!\n";
		return 0;
	}


	foreach my $file (@files){
		print "+ $file\n";
		remoteGet($ftp,"$src_dir/$file","$buildroot/$file",1);
		unless($^O =~ /mswin/i){
			if($entries{$file}->{'mode'}){
				chmod($entries{$file}->{'mode'},"$buildroot/$file");
			}
		}
		utime($entries{$file}->{'time'},$entries{$file}->{'time'},"$buildroot/$file");			
		copy("$buildroot/$file",$dest_dir,{'binmode' => 1});
	}
	deltree($buildroot);
	return 1;
}




sub packArchive{
	#my ($src_dir,$arfiles,$profile,$remote_host) = @_;
	my ($args) = @_;


	my $inner_subdir;
	my $archive_name;
	
	if($args->{'profile'} =~ /^all$/i){
		$inner_subdir = $profile_data{'Server'}->{'inner_subdir_tmpl'};
		$inner_subdir =~ s/server-/all-/;
		if($opt_component_group eq 'Client'){
			$inner_subdir =~ s/all-/client-/;
		}
		$archive_name = $profile_data{'Server'}->{'archive_name_tmpl'};
	}
	elsif($args->{'profile'} =~ /$LCAPPS_PROFILE_PATTERN/){
		
		my ($lca_rel,$lca_build) = ($opt_lca_version =~ /^(\d{2})\d*(\d{2})$/);
		$replacements{'%APO_REL'} = $lca_rel;
		$replacements{'%APO_BUILD'} = $lca_build;
		$replacements{'%APO_SHORT_VERS'} = $opt_lca_version;
		
		if (exists $profile_data{$LCAPPS_PROFILE} &&  
			exists $profile_data{$LCAPPS_PROFILE}->{'inner_subdir_tmpl'}){
			
			$inner_subdir = $profile_data{$LCAPPS_PROFILE}->{'inner_subdir_tmpl'};
		}
		else{
			$inner_subdir = $profile_data{$LIVECACHE_PROFILE}->{'inner_subdir_tmpl'};
			my $profile_in_dir_name = 'apo'.$opt_lca_version.'_livecache';
			$inner_subdir =~ s/server/$profile_in_dir_name/;
		}
		
		if (exists $profile_data{$LCAPPS_PROFILE} &&
			exists $profile_data{$LCAPPS_PROFILE}->{'archive_name_tmpl'}){
			
			$archive_name = $profile_data{$LCAPPS_PROFILE}->{'archive_name_tmpl'};
		}
		else{
			my ($lca_maj,$lca_min,$lca_build) = ($opt_lca_version =~ /^(\d)(\d)\S(\d{2,3})$/);
			$lca_build =~ s/^0// if (length($lca_build) == 3);
			$archive_name = 'LC%MAJ%MIN%COR%BUILDA'.$lca_maj.$lca_min.'_'.$lca_build;
		}
	}
	else{
		$inner_subdir = $profile_data{$args->{'profile'}}->{'inner_subdir_tmpl'};
		$archive_name = $profile_data{$args->{'profile'}}->{'archive_name_tmpl'};
	}

	
	foreach my $pattern (keys(%replacements)){
		$inner_subdir =~ s/$pattern/$replacements{$pattern}/g;
		$archive_name =~ s/$pattern/$replacements{$pattern}/g;
	}	
	
	my $buildroot = "$tmp_dir/buildroot";
	-d $buildroot and deltree($buildroot);
	makedir("$buildroot/".$inner_subdir,0775) or die("cannot create directory \"$inner_sub_dir\": $!\n");
	
	my @kit_files;
	my $local_extractor;	
			
	# get files
	if($args->{'host'}){
		#remote 
		
		my %entries = remoteLs($ftp,$args->{'pool_path'});
	
	
		if($args->{'profile'} =~ /$LCAPPS_PROFILE_PATTERN/ and $args->{'lca_package_name'} !~ /\S/ and
			$args->{'large_lca_package'} =~ /\S/){
			
			#
			#	do not copy files here. 
			#	there is a large livecache SAR archive with liveCache + LC Apps inside
			#	copy only this archive to get a complete old liveCache. 
			#	this change is needed by the upgrade test to get old 7.4.02 liveCaches 
			#
			
			$args->{'files'} = 	[];	
		}
			
		my $PCR_inside = 0;
		my $mainPCR,$mainSQLDBC;
		print "get files...\n";
		
		if(defined $args->{'self_extractor'}){
			if(exists $entries{$args->{'self_extractor'}}){
				print "+ ".$args->{'self_extractor'}." (needed to create self extracting archive)\n";
				$local_extractor = "$buildroot/".$args->{'self_extractor'}; 
				remoteGet($ftp,$args->{'pool_path'}.'/'.$args->{'self_extractor'},
						  $local_extractor,1);
			}
			else{
				die("extractor ".$args->{'self_extractor'}." not found\n");
			}
		}
		
		#$local_extractor = 'C:\SAPDevelop\DEV\develop\sys\wrk\fast\install\extractor.exe';
		#print ">>>> FAKE EXTRACTOR - use $local_extractor\n";
		
		foreach my $file (@{$args->{'files'}}){
			if($file =~ /^PCR\d{4}\.TGZ$/){
				$PCR_inside = 1;
				$mainPCR = $file;
			}
			if($file =~ /^SDBC.TGZ$/){
				$PCR_inside = 1;
			}
			
			if($file =~ /^SDBC\d{4}.TGZ$|^SQLDBC\d{2}.TGZ$/){
				$PCR_inside = 1;
				$mainSQLDBC = $file;
			}
				
			unless($file =~ /\//){
				exists $entries{$file} or die("$file not found in ".$args->{'host'}.':'.$args->{'pool_path'}." \n");
			}
			push @kit_files,$file;
			
			my ($variant,$dest_file) = ( $file =~ /^(.*)\/([^\/]*)/);
			
			$dest_file = $file unless $dest_file =~ /\S/;
			
			print "+ $dest_file ".( $variant =~ /\S/ ? "($variant)" : '')."\n";
			#if($file =~ /Wx.dll/){
			#	print ">>>>> FAKE WX_DLL <<<<<<\n";
			#	copy('C:/SAPDevelop/DEV/develop/sys/wrk/fast/Wx.dll',"$buildroot/$inner_subdir/$dest_file");
			#}
			#elsif($file =~ /wxmsw24.dll/){
			#	print ">>>>> FAKE WX_DLL <<<<<<\n";
			#	copy('C:/SAPDevelop/DEV/develop/sys/wrk/fast/install/wxmsw24.dll',"$buildroot/$inner_subdir/$dest_file");
			#}
			#if($file =~ /SDBINST.TGZ/){
			#	print ">>>>> FAKE SDBINST_TGZ <<<<<<\n";
			#	copy('C:/SAPDevelop/DEV/develop/sys/wrk/fast/install/SDBINST.TGZ',"$buildroot/$inner_subdir/$dest_file");	
			#}
			#elsif($file =~ /sdbrun.dll/){
			#	print ">>>>> FAKE SDBRUN_DLL <<<<<<\n";
			#	copy('C:/SAPDevelop/DEV/develop/sys/wrk/fast/install/sdbrun.dll',"$buildroot/$inner_subdir/$dest_file");	
			#}
			#else{
				remoteGet($ftp,$args->{'pool_path'}."/$file","$buildroot/$inner_subdir/$dest_file",1);
			#}
			unless($^O =~ /mswin/i){
				if($entries{$file}->{'mode'}){
					chmod($entries{$file}->{'mode'},"$buildroot/$inner_subdir/$dest_file");
				}
			}
			utime($entries{$file}->{'time'},$entries{$file}->{'time'},"$buildroot/$inner_subdir/$dest_file");			
			#copy("$args->{'pool_path'}/$file","$buildroot/".$inner_subdir,{'binmode' => 1});					
		}	
		
		my @PCRs;
		if($PCR_inside and $args->{'profile'} !~ /Develop|Precompiler|SDK/){
			@PCRs = grep { /^PCR\d{4}\.TGZ$|^SQLDBC\d{2}\.TGZ$|^SDBC\d{4}\.TGZ$/ and not /^$mainPCR$/ and not /^$mainSQLDBC$/ and $entries{$_}->{'type'} eq 'f'} keys(%entries);
		}
		
		foreach my $pcr (@PCRs){
			if ($opt_distrib_type =~ /OS/){
				print "filter SAP only package \"$pcr\"\n";
				next;
			}		
			print "+ $pcr\n";
			push @kit_files,$pcr;
			remoteGet($ftp,$args->{'pool_path'}."/$pcr","$buildroot/$inner_subdir/$pcr",1);
			unless($^O =~ /mswin/i){
				if($entries{$pcr}->{'mode'}){
					chmod($entries{$pcr}->{'mode'},"$buildroot/$inner_subdir/$pcr");
				}
			}
			utime($entries{$pcr}->{'time'},$entries{$pcr}->{'time'},"$buildroot/$inner_subdir/$pcr");			
		}

		
		if($opt_gui_setups){
			my %real_pool_entries = remoteLs($ftp,$args->{'real_pool_path'});
			if($real_pool_entries{'Setup'} =~ /\S/ and $real_pool_entries{'Setup'}->{'type'} eq 'd'){
				my %setup_entries = remoteLs($ftp,$args->{'real_pool_path'}.'/Setup');
				foreach my $file (keys(%setup_entries)){
					if($setup_entries{$file}->{'type'} eq 'f' and $file =~ /\.exe$/i){
						print "+ $file\n";
						remoteGet($ftp,$args->{'real_pool_path'}.'/Setup/'.$file,"$buildroot/$inner_subdir/$file",1);
						utime($setup_entries{$file}->{'time'},$setup_entries{$file}->{'time'},"$buildroot/$inner_subdir/$file");			
					}
				}
			}
			else{
				print "ERR: no gui setups found\n";
			}
			
		}


		
		
		if($args->{'profile'} =~ /$LCAPPS_PROFILE_PATTERN/){
			my %lca_entries = remoteLs($ftp,$args->{'lca_pool_path'});
			if($args->{'lca_package_name'} !~ /\S/){
				
				#
				#	getting the large old SAR archive
				#
				my $lca_pool = $args->{'lca_pool_path'};
				$lca_pool =~ s/\/[^\/]+$//;
				
				print '+ '.$args->{'large_lca_package'}."\n";
				remoteGet($ftp,$lca_pool.'/'.$args->{'large_lca_package'},
							"$buildroot/".$args->{'large_lca_package'},1);
			}
			else{
				print '+ '.$args->{'lca_package_name'}."\n";
				remoteGet($ftp,$args->{'lca_pool_path'}.'/'.$args->{'lca_package_name'},
							"$buildroot/$inner_subdir/".$args->{'lca_package_name'},1);
			}
			unless($^O =~ /mswin/i){
				if($lca_entries{$args->{'lca_package_name'}}->{'mode'}){
					chmod($lca_entries{$args->{'lca_package_name'}}->{'mode'},"$buildroot/$inner_subdir/".$args->{'lca_package_name'});
				}
			}
			utime($lca_entries{$args->{'lca_package_name'}}->{'time'},$lca_entries{$args->{'lca_package_name'}}->{'time'},"$buildroot/$inner_subdir/".$args->{'lca_package_name'});
		}

		ftp_release($ftp);

		
		if($inner_subdir =~ /%build/i or $archive_name =~ /%build/i){
			my $build;
			my $BUILD;
			if($sapdb_config{'version'} =~ /^\d+\.\d+\.\d+\.\d+$/){
				($build) = ($sapdb_config{'version'} =~ /^\d+\.\d+\.\d+\.(\d+)$/);			
			}
			else{
				$build = getArchiveBuildNum("$buildroot/$inner_subdir/SAPDBBAS.TGZ");
			}
			unless(defined $build){
				err("cannot determine build number\n");
			}		
			$build =~ s/^0*//;
			$build = 0 unless ($build =~ /\S/);  
			if (length($build) == 1){
				$BUILD = '0'.$build;
			}
			else{
				$BUILD = $build;
			}
			my $new_inner_subdir = $inner_subdir;
			$archive_name =~ s/%build/$build/g;
			$archive_name =~ s/%BUILD/$BUILD/g;
			$build =~ s/^0*//;
			$build = 0 unless($build =~ /\S/);
			$new_inner_subdir =~ s/%build/$build/g;
			$new_inner_subdir =~ s/%BUILD/$BUILD/g;
			rename("$buildroot/$inner_subdir","$buildroot/$new_inner_subdir");	
			$inner_subdir = $new_inner_subdir;
		}
	}
	else{
		#local
		my $PCR_inside = 0;
		my $mainPCR;
		print "get files...\n";
		foreach my $file (@{$args->{'files'}}){
			$file =~ /^PCR\d{4}\.TGZ$/ and $PCR_inside = 1 and $mainPCR = $file;
			copy($args->{'pool_path'}."/$file","$buildroot/".$inner_subdir,{'binmode' => 1});					
		}
		my @PCRs;
		if($PCR_inside){
			opendir(DH,$args->{'pool_path'}) or die ("cannot open directory \"".$args->{'pool_path'}."\"\n");
			@PCRs = grep { /^PCR\d{4}\.TGZ$/ and not /$mainPCR/ and -f $args->{'pool_path'}."/$_" } readdir(DH);
			closedir(DH);
		}
		unless (exists $replacements{'%BUILD'}){
			my $build = getArchiveBuildNum("$buildroot/$inner_subdir/SAPDBBAS.TGZ");
			my $new_inner_subdir = $inner_subdir;
			$archive_name =~ s/%BUILD/$build/g;
			$build =~ s/^0*//;
			$build = 0 unless($build =~ /\S/);
			$new_inner_subdir =~ s/%build/$build/g;
			rename("$buildroot/$inner_subdir","$buildroot/$new_inner_subdir");	
			$inner_subdir = $new_inner_subdir;
		}
		foreach my $pcr (@PCRs){
			copy($args->{'pool_path'}."/$pcr","$buildroot/".$inner_subdir,{'binmode' => 1});					
		}
			
	}
	
	my $sapcar = 'SAPCAR';
	my $separator = ':';
	my $exeext = '';
	my $sapcar_found = 0;
	
	if($^O =~ /mswin/i){
		$exeext = '.exe';
		$separator = ';';
	}
	
	foreach my $path (split($separator,$ENV{'PATH'}),'/usr/local/bin'){
		if(-x $path.'/'.$sapcar.$exeext){
			$sapcar = $path.'/'.$sapcar.$exeext;
			$sapcar_found = 1;
			last;
		}
	}
	
	if($args->{'profile'} =~ /$LCAPPS_PROFILE_PATTERN/ and $args->{'lca_package_name'} !~ /\S/){
		unless($sapcar_found){
			err("SAPCAR not found\n");
		}
		chdir($buildroot);
		my $cmd = "$sapcar -xvf $buildroot/".$args->{'large_lca_package'};
		my $out = `$cmd 2>&1`;
		unless($? == 0){
			err("SAPCAR error: $out\n");
		}
		
		my ($ardir) = ($out =~ /x\s+([^\/\\]+)SDBINST/m);  
		unless($inner_subdir eq $ardir){
			rmdir("$buildroot/$inner_subdir");
			rename("$buildroot/$ardir","$buildroot/$inner_subdir");
		}
	}
	
	if($opt_build_cd){
		opendir(DH,"$buildroot/$inner_subdir");
		my @files = grep {-f "$buildroot/$inner_subdir/$_"} readdir(DH);
		closedir(DH);
		foreach (@files){
			my $destination = $dest_dir;
			if(exists $CD_MAP_LIBNAME{$_}){
				print "using $_ as ".$CD_MAP_LIBNAME{$_}."\n";
				$destination = $dest_dir.'/'.$CD_MAP_LIBNAME{$_};
			}
			copy("$buildroot/$inner_subdir/$_",$destination,{'binmode' => 1,'nochown' =>1});
		}
		print "distribution files successfully copied\n";
		chdir($cwd);
		deltree($buildroot);
		return 1;
	}

	
	chdir($buildroot);
	print "pack distribution archive...\n";
	
	my $ar_ext = '.SAR';

	if($opt_os_style){
		$archive_name = $inner_subdir;
		$ar_ext =~ tr/[A-Z]/[a-z]/
	}
	
	unless($sapcar_found){
		err("SAPCAR not found\n");
	}
	if($^O =~ /mswin/i){
		$sapcar =~ s/\//\\/g;
	}
		
	my $cmd = "$sapcar -cvf \"$dest_dir/${archive_name}${ar_ext}\" $inner_subdir";
	if($opt_format =~ /^tgz$/i){
		if($args->{'host'} && $^O =~ /mswin/i && $opt_os !~ /WIN/){
			$ar_ext = '.TAR';
			$ar_ext =~ tr/[A-Z]/[a-z]/ if ($opt_os_style);
			$cmd = 'tar -cvpf '."\"${archive_name}${ar_ext}\" $inner_subdir";
		
		}
		else{
			$ar_ext = '.TGZ';
			$ar_ext =~ tr/[A-Z]/[a-z]/ if ($opt_os_style);
			$cmd = (($^O =~ /mswin|linux/i) ? 'tar' : 'gtar').
				" -cvpzf \"".($^O =~ /mswin/i ? '' : "$dest_dir/")."${archive_name}${ar_ext}\" $inner_subdir";
		}
	}
	elsif($opt_format =~ /^zip$/i){
		$ar_ext = '.ZIP';
		$ar_ext =~ tr/[A-Z]/[a-z]/ if ($opt_os_style);
		$cmd = undef;
		if(($opt_os and $opt_os !~ /WIN/) or (not $opt_os and $^O !~ /mswin/i)){
			print STDERR "WRN: pkzip isn\'t able to restore unix file permissons\n";
			print STDERR "WRN: please use another archive format (sar,tgz) for unix OSs\n";
		}
		BuildPackage::PackZip::packzip("$dest_dir/${archive_name}${ar_ext}",'.',$inner_subdir);			
	}
	elsif($opt_format =~ /^exe$/i){
		$ar_ext = '.EXE';
		$ar_ext =~ tr/[A-Z]/[a-z]/ if ($opt_os_style);
		$cmd = undef;
		my @files;
		foreach my $file (@kit_files){
			push @files,$inner_subdir.'/'.$file;
		}
		unless(BuildPackage::PackZip::pack_selfextracting_archive("$dest_dir/${archive_name}${ar_ext}",
															$local_extractor,'.',@files)){
															
			die ("cannot create archive\n");
		}													
	}
	$cmd =~ s/\//\\\\/g if $^O =~ /mswin/i;
	do_cmd($cmd) if $cmd;
	
	#
	# add execute permissions for unix installer files inside a windows packed sar archive
	#
	
	if($args->{'host'} && $^O =~ /mswin/i && $opt_os !~ /WIN/ && $cmd =~ /SAPCAR/){
		require BuildPackage::PatchSar;
		print "change permissions of installer inside sar archive...\n";
		my $sarpatcher = BuildPackage::PatchSar->new("$dest_dir/${archive_name}${ar_ext}");
		$sarpatcher->patchPermissions({'*/SDBINST' => 0775,'*/SDBRUN' => 0775,'*/SDBUPD' => 0775,
									  '*/libperl.s*' => 0775,'*/libwx*' => 0775,'*/Grid.s*' => 0775,
									  '*/Wx.s*' => 0775,'*/SDBSETUP' => 0775});  
	}
	elsif($args->{'host'} && $^O =~ /mswin/i && $opt_os !~ /WIN/ && $cmd =~ /^tar/){
		require BuildPackage::PatchTar;
		print "change permissions of installer inside tar archive...\n";
		my $tarpatcher = BuildPackage::PatchTar->new("${archive_name}${ar_ext}");
		$tarpatcher->patchPermissions({'*/SDBINST' => 0775,'*/SDBRUN' => 0775,'*/SDBUPD' => 0775,
									  '*/libperl.s*' => 0775,'*/libwx*' => 0775,'*/Grid.s*' => 0775,
									  '*/Wx.s*' => 0775,'*/SDBSETUP' => 0775});  
		my $new_ext = $opt_os_style ? '.tgz' : '.TGZ';
		$cmd = 'gzip -c -9 '."\"${archive_name}${ar_ext}\"".' >'.${archive_name}.$new_ext;
		${ar_ext} = $new_ext;
		do_cmd($cmd);
	}


	if ($^O =~ /mswin/i && $opt_format =~ /^tgz$/i){
		copy("${archive_name}${ar_ext}",$dest_dir,{'binmode' => 1});
	}	
	chdir($cwd);
	print "archive $dest_dir/${archive_name}${ar_ext} successfully packed\n";
	deltree($buildroot);
}

$pid_file;


sub main{
	local @ARGV = @_;
	my %optCtrl = (
		'h' => \$opt_h,
		'help' => \$opt_h,
		'profile=s' => \$opt_profile,
		'pool=s' => \$opt_pool,
		'format=s' => \$opt_format,
		'os=s' => \$opt_os,
		'os_style' => \$opt_os_style,
		'variant=s' => \$opt_variant,
		'old' => \$opt_old,
		'build_cd' => \$opt_build_cd,
		'version=s' => \$opt_version,
		'lca_version=s' => \$opt_lca_version,
		'lca_state=s' => \$opt_lca_state,
		'lca_variant=s' => \$opt_lca_variant,
		'buildselect=s' => \$opt_buildselect,
		'lca_buildselect=s' => \$opt_lca_buildselect,
		'dir=s' => \$opt_dir,
		'state=s' => \$opt_state,
		'gui_setups' => \$opt_gui_setups,
		'get_archives=s' => \$opt_get_archives,
		'host=s' => \$opt_host,
		'lc_pool_root' => \$opt_lc_pool_root,
		'list_profiles' => \$opt_list_profiles,
		'list_packages' => \$opt_list_packages,
		'checkmode' => \$opt_checkmode,
		'component_group=s' => \$opt_component_group,
		'verbose' => \$opt_verbose,
		'distrib_type=s' => \$opt_distrib_type,
		'no_debug_package' => \$opt_no_debug_package,
		'isolated_db_studio' => \$opt_isolated_db_studio 
	);
	
	my $usage = " [-h|--help]\n".
				"\t-profile <profile> | -component_group Client|Server+Client\n".
				"\t-os <operating system>\n".
				"\t-version <version> [-state COR|DEV|HOT|ANA]\n".
				"\t[-format SAR|TGZ|ZIP|EXE|RPM]\n".
				"\t[-buildselect <000..999>|<link name>]\n".
				"\t[-verbose] [-dir <destination directory>]\n".
				"\t[-os_style] [-lca_version <APO version>]\n".
				"\t[-checkmode] [-list_profiles] -[list_packages]\n".
				"\t[-lca_state DEV|COR|CONS|RAMP|HOT] [-lca_variant opt|dbg]\n".
				"\t[-lca_buildselect <000..999>|<link name>]\n".
				"\t[-distrib_type OS|BETA] [-no_debug_packages]\n";
		
	
	if(!GetOptions(%optCtrl) || $opt_h ||  ! do_opt_check()){
		print "usage: packdistrib $usage\n"; 
		exit ($opt_h ? 0 : 1);
	}
	
	if($opt_verbose){
		$BuildPackage::Tools::DEBUG = 1;
	}

	$tmp_dir = $tmp_dir."/packdistrib";
	my $tmp_dir_base = $tmp_dir;

	my $count;
	while(1){
		$tmp_dir = $tmp_dir_base.$count;
		$count = (defined $count ? ($count + 1) : 0);
		if (-d $tmp_dir){
			my $pid_file = $tmp_dir.'/pid'; 
			if(-f $pid_file){
				open(FH,$pid_file) or next;
				my $pid;
				while(<FH>){
					chomp;
					$pid = $_;
				}
				close(FH);
				if($^O =~ /mswin/i || kill (0 => $pid)){
					if((time - (stat($pid_file))[9]) >  86400){
						# more than one day 
						last;
					}
				}	
			}
			else{
				last;
			} 
		}
		else{
			makedir($tmp_dir,0775) or die("cannot create directory $tmp_dir\n");
			last;
		}
	}
	
	$opt_verbose && print ">>>>>tmp dir = $tmp_dir\n";

	if($machine_config->{'os'} =~ /\S/){
			print "SYSTEM:\t\t".$machine_config->{'os'}.' '.$machine_config->{'mach'}."\n";
	}

	print "PACKDISTRIB:\t".getVersionString()."\n";
	
	my ($version) = ($opt_version =~ /^(\d{4})/);
	my $host;
	if (exists $HOSTS{$version} && 
		exists $HOSTS{$version}->{$opt_os}){
		
		$host = ${%{$HOSTS{$version}}}{$opt_os}->{'name'};
	}
	if($opt_host =~ /\S/){
		$host = $opt_host;
	}
	print "FTP HOST:\t".($host =~ /\S/ ? $host : 'unknown')."\n";
	
	open(FH,'>'.$tmp_dir.'/pid') or die ("cannot create pid file in $tmp_dir\n"); 
	print FH "$$\n";
	close(FH);
	$pid_file = $tmp_dir.'/pid';

	my ($pool_path,$host) = getPoolPath();
	my $real_pool_path = $pool_path; 
	
	print "VERSION:\t".$sapdb_config{'version'}."\n" if $sapdb_config{'version'} =~ /\S/; 
	print "STATE:\t\t".$sapdb_config{'state'}."\n";
	print "POOL ID:\t".$sapdb_config{'pool_id'}."\n";
	print "CHANGELIST:\t".$sapdb_config{'changelist'}."\n" if defined $sapdb_config{'changelist'};
	print "MAKE ID:\t".$sapdb_config{'make_id'}."\n" if defined $sapdb_config{'make_id'};
	
	
	if($opt_get_archives){
		if(getArchives($pool_path,$opt_get_archives)){
			ftp_release($ftp);
			return 0;
		}
		else{
			ftp_release($ftp);
			return -1;
		}
	}




	
	my $lca_pool_path;
	my $real_lca_pool_path;
	my $lca_package_name;
	my $large_lca_package;
	
	if($opt_profile =~ /$LCAPPS_PROFILE_PATTERN/){
		$lca_pool_path = getLCAPoolPath($pool_path);
		$real_lca_pool_path = $lca_pool_path;
		$lca_pool_path .= "/$LCAPPS_COMPONENTS_DIR";
		
		# find name of LCAPPS package
		my @lca_names;

		my %entries = remoteLs($ftp,$lca_pool_path);

		
		
		foreach my $name (keys(%entries)){
			if($name =~ /$LCAPPS_PACKAGE_PATTERN/){
				push @lca_names,$name if $entries{$name}->{'type'} eq 'f';
			}
		}
		if($#lca_names == -1){
			my $lca_pool = $lca_pool_path;
			$lca_pool =~ s/\/[^\/]+$//;
			my %entries = remoteLs($ftp,$lca_pool);
			foreach my $name (keys(%entries)){
				if($name =~ /^LC7.*\.SAR$/){
					$large_lca_package = $name;
				} 
			}
			unless(defined $large_lca_package){
				die("no LCAPPS package found in $lca_pool_path\n");
			}			
		}
		die("to many LCAPPS packages found in $lca_pool_path\n") if $#lca_names > 0;
		$lca_package_name = $lca_names[0];	
		$opt_verbose && print ">>>>> lca package name = $lca_package_name\n"; 
		$opt_verbose && print ">>>>> large lca package name = $large_lca_package\n"; 
	
		print "LC APPS STATE:\t\t".$lca_config{'state'}."\n";
		print "LC APPS VARIANT:\t".$lca_config{'variant'}."\n";
		print "LC APPS POOL ID:\t".$lca_config{'pool_id'}."\n";
		print "LC APPS CHANGELIST:\t".$lca_config{'changelist'}."\n" if defined $lca_config{'changelist'};
		print "LC APPS MAKE ID:\t".$lca_config{'make_id'}."\n" if defined $lca_config{'make_id'};

	
	
	}
	
	unless($opt_old or $opt_os =~ /AIX32|HP32|SUN32|AIX532/){
		$pool_path .= "/$SAPDB_COMPONENTS_DIR";
	}
	
		
	print "load profile definition module...\n";
	
	my ($version) = ($opt_version =~ /^(\d{4})/);
	my ($relver) = ($version =~ /^(\d{2})/);
	if($opt_component_group =~ /Server/){
		$opt_profile = 'all';
	}
	elsif($opt_component_group =~ /Client/){
		$opt_profile = 'all';
	}		
	
	loadModule('BuildPackage::PackDefs',$pool_path,$host,($version =~ /^\d{4}$/ ? {'\$RUNTIME_VERSION' => $version,
		'\$RELVER' => $relver,'\$MAJ\$MIN\$COR' => $version, '\$MAJ\s==\s7\s\&\&\s\$MIN\s>\s4' => 1} : undef));	
	unless(($opt_profile =~ /^all$/i or exists $profiles{$opt_profile}) or
			$opt_list_profiles or
			$opt_profile =~ /$LCAPPS_PROFILE_PATTERN/ and exists $profiles{$LIVECACHE_PROFILE}){
		die("no profile \"".($opt_profile =~ /$LCAPPS_PROFILE_PATTERN/ ? $LIVECACHE_PROFILE : $opt_profile)."\" found\n");
	}
	
	if($opt_os){
		exists $OSS{$opt_os} or die("cannot find \"$opt_os\" in \%OSS\n");	
		%replacements = (
			'%OS' => $OSS{$opt_os}->{name},
			'%ARCH' => $OSS{$opt_os}->{arch},
			'%BIT' => $OSS{$opt_os}->{bit}
		);
	}
	else{
		exists $OSS{$CUR_OS} or die("cannot find \"$opt_os\" in \%OSS\n");	
		%replacements = (
			'%OS' => $OSS{$CUR_OS}->{name},
			'%ARCH' => $OSS{$CUR_OS}->{arch},
			'%BIT' => $OSS{$CUR_OS}->{bit}
		);
	}


	if($opt_version){
		my ($maj,$min,$cor,$build) = ($opt_version =~ /^(\d)(\d)(\d\d)(\d{0,2})$/);		
		$replacements{'%MAJ'} = $maj;
		$replacements{'%MIN'} = $min;
		$replacements{'%COR'} = $cor;
		$cor =~ s/^0*//;
		$cor = 0 unless($cor =~ /\S/);
		$replacements{'%cor'} = $cor;
		if($build =~ /\d\d/){
			$replacements{'%BUILD'} = $build;
			$build =~ s/^0*//;
			$build = 0 unless($build =~ /\S/);
			$replacements{'%build'} = $build;
		}
	}
	else{
		$replacements{'%MAJ'} = $MAJ;
		$replacements{'%MIN'} = $MIN;
		$replacements{'%COR'} = $COR;
		my $cor = $COR;
		$cor =~ s/^0*//;
		$cor = 0 unless($cor =~ /\S/);
		if($LC_STATE eq 'RAMP'){ 
			$replacements{'%BUILD'} = $BUILD;
			my $build = $BUILD;
			$build =~ s/^0*//;
			$build = 0 unless($build =~ /\S/);
			$replacements{'%cor'} = $cor;
			$replacements{'%build'} = $build;
		}
	}

	
	my @files = ();
	
	if($opt_list_profiles){
		my @packages;
		foreach (@allpackages){
			push @packages,$_->{'name'};
		}	
		print "PROFILE all:\t".join(', ',@packages)."\n";
		foreach my $profile (sort keys(%profiles)){
			my @packages = @{$profiles{$profile}};
			if($profile =~ /liveCache/i){
				unshift(@packages,@{$profiles{'Server'}});
			}
			print "PROFILE $profile: ".join(',',@packages)."\n";
		}
		if(%RPM_PROFILES){
			foreach my $profile (sort keys(%RPM_PROFILES)){
				print "RPM PROFILE $profile: ".join(', ',@{$RPM_PROFILES{$profile}})."\n";
			}
		}
		return 0;
	}
	
	if(defined $opt_component_group){
		print "checking component group \"$opt_component_group\"...\n";	
	}
	else{
		print "checking profile \"$opt_profile\"...\n";
	}
	if($opt_format =~ /^rpm$/i){
		exists $RPM_PROFILES{$opt_profile}  or $opt_profile =~ /^all$/i or die("rpm profile $opt_profile not found\n");
		if($opt_profile =~ /^all$/i){
			@files = values(%ALL_RPMS);
		}
		else{
			foreach my $rpm (@{$RPM_PROFILES{$opt_profile}}){
				push @files,$ALL_RPMS{$rpm};
			}
		}
		getRPMs($pool_path,\@files,$host);		
		print "rpms successfully packed\n";
		exit (0);
	}
	
	if($opt_component_group eq 'Client'){
		my @tmp_allpackages;
		foreach my $package (@allpackages){
			push @tmp_allpackages,$package 
				unless $package->{'name'} =~ /^Server\sUtilities$|^Database\sKernel$/;
		}
		@allpackages = @tmp_allpackages;	
	}
	
	my @variant_packages;
	
	my @profile_packages = ();
	
	if($opt_profile =~ /^all$/i){
		foreach my $package (@allpackages){
			$package->{'name'} =~ /^LC|^APO/ and next;
			push @profile_packages,$package;
			if ($opt_distrib_type =~ /OS/ and $package->{'distribution'} eq 'SAP ONLY'){
				print "filter SAP only package \"".$package->{'name'}."\"\n";
				next;
			}
			if ($opt_distrib_type !~ /BETA/ and $package->{'distribution'} eq 'BETA'){
				print "filter BETA package \"".$package->{'name'}."\"\n";
				next;
			}
			if($opt_variant =~ /\S/ and exists $package->{'variant'}->{$opt_variant}){
				push @variant_packages, $package->{'name'}; 	
				push @files, "$opt_variant/".$package->{'archive'};
				if (exists $package->{'DBG_PACKAGE'} and not $opt_no_debug_package){
					push @files, $package->{'DBG_PACKAGE'}->{'name'};
				}
			}
			else{
				push @files, $package->{'archive'};
				if (exists $package->{'DBG_PACKAGE'}  and not $opt_no_debug_package){
					push @files, $package->{'DBG_PACKAGE'}->{'name'};
				}
			}
			exists $package->{'additional_installer_files'} and push @files,@{$package->{'additional_installer_files'}}; 
		}
	}
	else{
		foreach my $package_of_profile (@{$profiles{$opt_profile =~ /$LCAPPS_PROFILE_PATTERN/ ? $LIVECACHE_PROFILE : $opt_profile}}){
			next if (defined $opt_isolated_db_studio and $package_of_profile eq 'Base');
			my $found = 0;
			$package_of_profile =~ /\S/ or next;
			foreach my $package (@allpackages){
				if($package->{'name'} eq $package_of_profile){
					$found = 1;
					if ($opt_distrib_type =~ /OS/ and $package->{'distribution'} eq 'SAP ONLY'){
						print "filter SAP only package \"".$package->{'name'}."\"\n";
						last;
					}
					if ($opt_distrib_type !~ /BETA/ and $package->{'distribution'} eq 'BETA'){
						print "filter BETA package \"".$package->{'name'}."\"\n";
						last;
					}
					push @profile_packages,$package;
					if($opt_variant =~ /\S/ and exists $package->{'variant'}->{$opt_variant}){
						push @variant_packages, $package->{'name'}; 	
						push @files, "$opt_variant/".$package->{'archive'};
						if (exists $package->{'DBG_PACKAGE'}  and not $opt_no_debug_package){
							push @files, $package->{'DBG_PACKAGE'}->{'name'};
						}
					}
					else{
						push @files, $package->{'archive'};
						if (exists $package->{'DBG_PACKAGE'}  and not $opt_no_debug_package){
							push @files, $package->{'DBG_PACKAGE'}->{'name'};
						}	
					}	
					exists $package->{'additional_installer_files'} and push @files,@{$package->{'additional_installer_files'}}; 
					last;
				} 
			}
			unless($found){
				die("definition of package \"$package_of_profile\" not found\n");
			}
		}
	}
	
	if ($opt_distrib_type =~ /OS/ and defined $freeware_license_file){
		push @files,$freeware_license_file;
	}
		
	
	if($opt_list_packages){
		if(defined $opt_component_group){
			print "component group $opt_component_group contains:\n";
		}
		else{
			print "profile $opt_profile contains:\n";
		}
		my $max_name_len = 0;
		my @names;
		my $need_old_files = 0;
		foreach my $package (@profile_packages){
			if ($opt_distrib_type =~ /OS/ and $package->{'distribution'} eq 'SAP ONLY'){
				next;
			}
			if ($opt_distrib_type !~ /BETA/ and $package->{'distribution'} eq 'BETA'){
				next;
			}
			my $name = $package->{'name'};
			$name = 'SQLDBC' if $name eq 'Database Connectivity';
			if($name =~ /SQLDBC|PCR/){
				$need_old_files = 1;
			}
			
			my $len = length($name);
			$max_name_len = $len if $len > $max_name_len;
			my $archive = $package->{'archive'};
			if (exists $package->{'DBG_PACKAGE'} and not $opt_no_debug_package){
					$archive .= ','.$package->{'DBG_PACKAGE'}->{'name'};
			}
			push @names,[$name,$archive];
		}
		
		if($opt_distrib_type !~ /OS/ and $need_old_files){
			my %entries = remoteLs($ftp,$pool_path);
			my @PCRs = grep { /^PCR\d{4}\.TGZ$|^SQLDBC\d{2}\.TGZ$|^SDBC\d{4}\.TGZ$/ and $entries{$_}->{'type'} eq 'f'} keys(%entries);
			foreach my $pcr (@PCRs){
				my $found = 0;
				foreach my $name (@names){
					if($name->[1] eq $pcr){
						$found = 1;
					}
				}
				unless($found){
					if($pcr =~ /PCR/){
						my ($version) = ($pcr =~ /^PCR(\d{4})\.TGZ/);
						push @names,['PCR '.$version,$pcr];
					}
					else{
						my ($version) = ($pcr =~ /^SDBC(\d{4})\.TGZ/);
						unless (defined $version){
							($version) = ($pcr =~ /^SQLDBC(\d{2})\.TGZ/);
						}
						push @names,['SQLDBC '.$version,$pcr];
					}
				}	
			}
		}
		
		foreach my $name (sort {$a->[0] cmp $b->[0]} @names){
			my $len = length($name->[0]);
			print $name->[0].' ' x ($max_name_len - $len + 1).'['.$name->[1]."]\n";
		}
		deltree($tmp_dir); 
		return 0;
	}
	
	
	
	
	if($opt_variant and $#variant_packages == -1){
		die("no $opt_variant package found\n");
	}
	
	if (defined $opt_isolated_db_studio){
		$self_extractor = $dbstd_self_extractor;
		foreach my $ifile (@installer_files){
			next if $ifile =~ /^SDBINST\.exe$|^README$/i;
			push @files,$ifile;
		}
	}
	else{
		push @files,@installer_files;
	}
	
	
	if($opt_format =~ /exe/i){
		unless(defined $self_extractor and $self_extractor =~ /\S/){
			print STDERR ("cannot create self extracting archive: no extractor defined\n");
			return -1;
		}
	}
	else{
		$self_extractor = undef;
	}
	
	if($opt_checkmode){
		my $pool;
		if($opt_profile eq 'APO LiveCache'){
			($pool) = ($real_lca_pool_path =~ /\/(LC_7.+)$/);
		}
		else{
			($pool) = ($real_pool_path =~ /\/(LC_7.+)$/);
		}
		print "\nLC_POOL_PATH=$pool\n";
		print "OK\n";
		deltree($tmp_dir);
		return 0;
	}
	
	packArchive({'pool_path' => $pool_path,'lca_pool_path' => $lca_pool_path,'files' => \@files,
				'profile' => $opt_profile,'host' => $host,'lca_package_name' => $lca_package_name,
				'large_lca_package' => $large_lca_package,
				'real_pool_path' => $real_pool_path, 'self_extractor' => $self_extractor});
	deltree($tmp_dir);
	return 0;
}


1;