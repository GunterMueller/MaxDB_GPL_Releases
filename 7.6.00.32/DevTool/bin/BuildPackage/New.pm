#! /usr/bin/perl
#
# $Header: //prodprojects/vmake/dev/src/_tools/maketools/sys/src/BuildPackage/New.pm#56 $
# $DateTime: 2006/05/16 15:05:36 $
# $Change: 592950 $
#
# Desc: tool to build several sap db packages
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

package BuildPackage::LCPackDefs;
use BuildPackage::Vars;

if($APOMAKE){
	if(-e $LC_OWN.'/'.$SAPDB_COMPONENTS_DIR.'/BuildPackage/PackDefs.pm'){
		local @INC = ($LC_OWN.'/'.$SAPDB_COMPONENTS_DIR,@INC);
		require BuildPackage::PackDefs;
		import BuildPackage::PackDefs;
	}
	else{
		print STDERR "WRN: PackDefs of liveCache not found\n";
	}
}

1;

package BuildPackage::New;

use Getopt::Long;
use BuildPackage::GetFilesFromCDImages; # needed for XML stuff -> getting PCR 32/64 bit
use BuildPackage::Tools;
use BuildPackage::Vars;
use BuildPackage::LCPool;

$tar = 'gtar';
$tar = 'tar' if $^O =~ /mswin|linux/i;

#
#	IMPORTANT CHANGE:
#	use PackDefs and LCPool::Items from release tree instead of DevTool tree 
#
	
@INC = ("$WRK/fast/$INSTALL_SUB_PATH",@INC);

require BuildPackage::PackDefs;
import BuildPackage::PackDefs;

require BuildPackage::LCPool::Items;
import BuildPackage::LCPool::Items;	

BuildPackage::Tools::setErrMsgHandler(\&err);
BuildPackage::Tools::setMsgHandler(\&msg);


my @all_files;

sub packdbg{
	my ($package) = @_;
	-d $BUILDROOT and (deltree($BUILDROOT) or err("cannot remove buildroot\n"));
	makedir($BUILDROOT,0775) or err("cannot create buildroot\n");
	my $size;
	my $packmd5 = getMD5Sum("$PACKAGEPATH/".$package->{'archive'});
	my $tarname = "$PACKAGEPATH/".$package->{'DBG_PACKAGE'}->{'name'};
	open(FD,'>'."$BUILDROOT/PACKAGEDATA") or err("cannot create file $BUILDROOT/PACKAGEDATA\n");
	open(LST,'>'."$BUILDROOT/FILELIST") or err("cannot create file $BUILDROOT/FILELIST\n");
	print FD "PACKAGE_NAME = \"".$package->{'name'}."\"\n";
	print FD "PACKAGE_MD5SUM = \"".$packmd5."\"\n";
	if ($package->{'build_string'} =~ /\S/){
		print FD "BUILD_STRING = \"".$package->{'build_string'}."\"\n";
	}
	if ($package->{'make_id'} =~ /\S/){
		print FD "MAKE_ID = \"".$package->{'make_id'}."\"\n";
	}
	my %dirs;
	foreach my $dbgfile (@{$package->{'dbgfiles'}}){
			my ($src,$filemode) = @$dbgfile;
			my $dest = $src;
			if(exists $package->{'FileMappings'}->{$src}){
				$src = $package->{'FileMappings'}->{$src};
			}
			
			if(${%$package}{'name'} eq 'APO COM' or ${%$package}{'name'} eq 'APO LC APPS'){
				$src = "sap/$src";
			}
				
			$src = "$USR/$src";
			
			my $dst="$BUILDROOT/$dest";
			my $dir = $dst;
			$dir=~s/\/[^\/]*$//;
			my $rel_dir = $dest;
			$rel_dir =~ s/\/[^\/]*$//;
			
			-d $dir or makedir($dir,0775) or err("cannot create directory $dir\n");
			
			unless(-f $src){
				push @missing_files,$src;
				next;
			}
			
			copy($src,$dst,{'binmode' => 1,'nochown' => 1});
			
			-f $dst or err("cannot copy $src -> $dst\n");
			
			$size += (stat($src))[7];

			unless($^O =~ /mswin/i){
				my $mdir;
				my $sep = '/';
				foreach my $ldir (split('/',$rel_dir)){
					$mdir .= $sep.$ldir;
					$dirs{$mdir} = 1 unless exists $dirs{$mdir}; 
				}
				
				my @statbuf = stat($dst);
								
				my $pmask = 0222; # default: remove write permissions for all

				$pmask = ${%$package}{'pmask'} if defined ${%$package}{'pmask'};
				
				my $newmode;
				
				if(defined $filemode){
					$newmode = oct($filemode);
				}
				else{
					$newmode = $statbuf[2] & ~$pmask; 
				}

				if($newmode != $statbuf[2]){
					chmod($newmode,$dst) or err("cannot change mode of $dst\n");
					if(($newmode & 06777) != ((stat($dst))[2] & 06777)){
						err("cannot set mode of $dst to	".sprintf("0%o\n",$newmode));
					}
				}
			}
						
			my $md5sum=getMD5Sum($dst) or err("cannot get md5sum of $dst\n");
			my $line = "\"$dest\" $md5sum\n";
			push @filelist_buf,$line;
			print LST  $line or err("cannot write into $BUILDROOT/".${%$package}{'filelist'}."\n");
		
	}
	close(LST);
	chdir($BUILDROOT);
	print FD "SIZE = \"$size\"\n";
	close(FD);
	my $packlist = 'PACKAGEDATA FILELIST';
	opendir(DH,'.') || err("cannot open directory: $!\n");
	my @content = readdir(DH);
	closedir(DH);
	foreach (@content){
		(/^PACKAGEDATA$/ or /^FILELIST$/ or /^\.{1,2}$/) and next;
		$packlist="$packlist $_"; 
	}
	
	
	unless($MAJ == 7 && $MIN < 4){	
		foreach my $dir (sort { length($b) <=> length($a)} keys(%dirs)){
			chmod(0555,"$BUILDROOT$dir") || err("cannot chmod of directory $dir to 555\n");			
		}
	}
	
	msg ("pack archive $tarname\n"); 
	
	if($^O =~ /mswin/i){
		do_cmd("$tar -cvpf - $packlist | gzip -c -9 >$tarname");
	}
	else{
		do_cmd("$tar -cvpzf $tarname $packlist");
	}
	$? == 0 or err("tar error while packing \"".${%$package}{'name'}."\"\n");
	-f $tarname or err("no archive \"$tarname\" produced\n");
	chdir('..'); # leave buildroot 
}


sub start{
	local @ARGV=@_;
	my @argbuf = @ARGV;
	
	my %opt_ctrl=(
			'h' => \$opt_help,
			'help' => \$opt_help,
			'profile=s' => \$opt_profile,
			'package=s' => \$opt_package,
			'format=s' => \$opt_format,
			'os_style' => \$opt_os_style,
			'rpm' => \$opt_rpm,
			'components' => \$opt_components,
			'profile_components' => \$opt_profile_components,
			'ignore_errors' => \$opt_ignore_errors,
			'single_pcr' => \$opt_single_pcr,
			'no_compat_files' => \$opt_no_compat_files,
			'no_dbg_files' => \$opt_no_dbg_files,
			'finalize' => \$opt_finalize,
			#'no_archive' => \$opt_no_archive,
			'ftp' => \$opt_ftp,
			'var=s' => \$opt_var

	);
	
	Getopt::Long::Configure ('no_pass_through');
	if(!GetOptions(%opt_ctrl) or $opt_help){
		msg ("usage: buildpackage [-h] [-ftp] [-no_archive] [-components] [-no_compat_files] [-profile <profile>] [-package <package list>] [-format tgz|sar] [-finalize]\n\n");
		exit 0;
	}
	
	
	if($opt_var){
		my %vars;
		foreach my $var_string (split(',',$opt_var)){
			my ($varname,$varvalue) = ($var_string =~ /^([^=]+)=([^=]*)$/);
			$varname =~ /\S/ and $vars{$varname} = $varvalue;
		}
		SetVars(\%vars,\&err);
	}

	# tiny helper functions
	sub timestamp{
		my($sec,$min,$hour,$day,$mon,$year)=localtime(time);
		$mon++;
		$sec="0$sec" if(length($sec)==1);
		$min="0$min" if(length($min)==1);
		$hour="0$hour" if(length($hour)==1);
		$day="0$day" if(length($day)==1);
		$mon="0$mon" if(length($mon)==1);
		$year=$year+1900;
		return "$year$mon$day$hour$min$sec";
	}

	-d "$WRK/fast/$INSTALL_SUB_PATH" or makedir("$WRK/fast/$INSTALL_SUB_PATH",0775);
	my $count = '';
	while(1){
		$logfilename="$WRK/fast/$INSTALL_SUB_PATH/buildpackage_".timestamp.$count.'.log';
		-f $logfilename or last;
		$count++ unless $count eq ''; 
		$count = 0 if $count eq '';
	}
	
	open(LOG, ">$logfilename") || err ("cannot create logfile\n");
	msg("call: ".join(' ',@argbuf)."\n");
	sub err{
		my ($text)=@_;
		if($opt_ignore_errors){
			print STDERR "ERR: $text";
			print LOG "ERR: $text";
		}
		else{
			print LOG "ERR: $text";
			close(LOG);
			print STDERR $text;
			exit -1;
		}
	}

	sub msg{
		my ($text)=@_;
		print LOG $text;	
		print $text;
	}

	if($^O =~ /linux/ and ($opt_format =~ /^rpm$/i or ($opt_components and $opt_rpm ))){
		require BuildPackage::RPM;
		import BuildPackage::RPM;
		BuildPackage::RPM::setErrMsgHandler(\&err);
		BuildPackage::RPM::setMsgHandler(\&msg);
		my @rpms = packRPM();
		push @all_files,@rpms;
		exit 0 unless $opt_components;	
	}




	if($opt_ftp){
		SetVars({'USE_FTP' => 1},\&err);
	}


	BuildPackage::LCPool::initPackage('err' => \&err,'msg' => \&msg);
	

	sub existInList{
		my ($packname,@packages)=@_;
		foreach my $hrPack (@packages){
			$packname eq ${%$hrPack}{'name'} and return 1; 
		}
		return 0;
	}
	
		
	#sub copyPCR{
	#	BuildPackageOld::OldPCR();
	#}	


	my @packages;
	my @testpackages;
	
	#
	#define replacements -> for replacing strings in signature file and package script
	#	
	
	my %replacements = (
		'§RELEASE' => "$MAJ.$MIN.$COR.$BUILD",
		'§APORELEASE' => "$APO_REL.$APO_BUILD",
		'§LC_RELEASE' => "$LC_RELEASE",
		'§PACKVERS' => "$RUNTIME_VERSION", # used for names/pathes of PCR and SDK packages
		'§RELVER' => $RELVER,
		'§BIT' => $BIT,
		'§SOEXT' => '.'.$SOEXT,
		'§EXEXT' => $EXEEXT
	);
	
	
	#foreach my $sym (keys(%BuildPackage::New::)){
	#	local *value = $BuildPackage::New::{$sym};
	#	defined $value and print "$sym = $value\n";
	#} 	

		
	#
	# select packages to build
	# with profile option
	#
	
	if($opt_profile and $opt_profile ne 'all' ){
		my @profiles = split(',',$opt_profile);
		foreach my $profile (@profiles){
			exists $profiles{$profile} or err ("no such profile: \"".$profile."\"\n");
			my @prof_pack;
			foreach my $pack (@{$profiles{$profile}}){
				defined $pack or next;
				my $found = 0;
				foreach my $packref (@allpackages){
					if($pack eq ${%$packref}{'name'}){
						$found=1;
						existInList($pack,@packages) and last;
						push @packages,$packref;
						last;
					}
				}
				if($found){
					push @prof_pack,$pack;
				}
				else{
					err("no such package: \"$pack\"\n");
				}
			}
			msg ("found for \"$profile\":\n");
			foreach my $packnam (@prof_pack){
				msg ("\t$packnam\n");
			}
			msg ("\n");
		}
		if ($APOMAKE and $opt_profile ne 'APO LiveCache'){
			err("LC APPS MAKE ENVIRONMENT: profile \"APO LiveCache\" allowed only\n");
		}
		if(!$APOMAKE and $opt_profile =~ /APO\s*LiveCache/){
			err("NO LC APPS MAKE ENVIRONMENT: profile \"APO LiveCache\" not allowed\n");
		}
			
	}
	
	my @profile_packages = @packages;


	#
	# select packages to build
	# with package option
	#
	
	if($opt_package){
		my @packs = split(',',$opt_package);
		foreach my $pack (@packs){
			my $found = 0;
			foreach my $packref (@allpackages,@alltestpackages){
				if($pack eq ${%$packref}{'name'}){
					push @packages,$packref;
					$found=1;
					last;
				}
				elsif($pack eq 'APO COM'){
					if(${%$packref}{'name'} eq 'APO LC APPS'){
						push @packages,$packref;
						$found = 1;
						last;
					}
				}
			}
			$found || err("no such package: \"$pack\"\n");
		}
		if ($APOMAKE and ($opt_package ne 'APO COM' and $opt_package ne 'APO LC APPS')){
			err("LC APPS MAKE ENVIRONMENT: package \"APO COM\" / \"APO LC APPS\" allowed only\n");
		}
		if(!$APOMAKE and $opt_package =~ /APO\s*COM|APO\s\LC\sAPPS/){
			err("NO LC APPS MAKE ENVIRONMENT: package \"$opt_package\" not allowed\n");
		}
	}
	
	
	sub packageInside{
		my ($packages,$package) = @_;
		foreach my $pack (@$packages){
			return 1 if (${%$pack}{'name'} eq ${%$package}{'name'});	
		}
		return 0;
	}

	
	if($opt_profile eq 'all' || $opt_components){
		unless($APO_MAKE){
			#@packages = (); # init
			foreach my $packref (@allpackages){
				#packageInside
				push @packages,$packref if ((${%$packref}{'name'} ne 'APO COM' and ${%$packref}{'name'} ne 'APO LC APPS') and not packageInside(\@packages,$packref)); 
			}
		}
		@testpackages = @alltestpackages if $opt_components;
	}

	@profile_packages = @packages if $opt_profile eq 'all';


	#
	# select all packages to build if no option set
	#
	if($#packages == -1){
		err("no package selected\n");
	}
	
	
	# print out packages
	msg ("try to build following packages: \n");
	foreach $hrPack (@packages){
		msg ("\t".${%$hrPack}{'name'}."\n");
		if(!$APOMAKE and ${%$hrPack}{'name'} =~ /APO\s*COM|APO\sLC\sAPPS/){
			err("NO LC APPS MAKE ENVIRONMENT: package \"".${%$hrPack}{'name'}."\" not allowed\n");
		}
		if ($APOMAKE and (${%$hrPack}{'name'} ne 'APO COM' and ${%$hrPack}{'name'} ne 'APO LC APPS')){
			err("LC APPS MAKE ENVIRONMENT: package \"APO COM\" / \"APO LC APPS\" allowed only\n");
		}
		if($hrPack->{'name'} eq 'JDBC'){
			require SAPDB::Install::Jar;
			import SAPDB::Install::Jar;
		}
	}
	msg ("\n"); 


	my $packpath = $PACKAGEPATH.'/packages';

	if($APOMAKE){
		$packpath = "$LC_OWN/sys/src/lcsrc/packages" unless(-f $PACKAGEPATH.'/packages/apocom/PACKAGEDATA');
	}



	#
	# test needed files:
	# 
	
	my $include_compat_files = 0;
	foreach my $package (@packages){
		foreach my $file (${%$package}{'filelist'},${%$package}{'script'},'PACKAGEDATA'){
			-f $packpath.'/'.${%$package}{'subdir'}.'/'.$file || err($packpath.'/'.${%$package}{'subdir'}.'/'.$file.' [package "'.${%$package}{'name'}."\"] not found\n");
		}
		$include_compat_files = 1 if $package->{'name'} =~ /^PCR|^Database\sConnectivity|^SQLDBC/;
		if($^O =~ /mswin/i and $package->{'name'} eq 'Base'){
			$include_compat_files = 1;
		}
	}

	my @oldpcr_packages = (); 
	
	if($include_compat_files and !$opt_single_pcr and !$opt_no_compat_files){
		# copy pcr packages
		BuildPackage::GetFilesFromCDImages::setErrMsgHandler(\&err);
		BuildPackage::GetFilesFromCDImages::setMsgHandler(\&msg);
		my @files = grep {/\.TGZ$/} getFiles("$MAJ.$MIN.$COR.$BUILD",$OWN);
		push(@oldpcr_packages,@files) unless $opt_profile =~ /SDK|Develop|Precompiler/;
		push @all_files,@oldpcr_packages;
	}
	
	
	#
	# get niping for APO COM package
	#

	if($APOMAKE && defined $LC_ROOT){
		my $src = "$LC_OWN/sys/src/lcsrc/sap/niping$EXEEXT";
		my $dst = "$INSTROOT/sap/niping$EXEEXT";
		if($^O=~/mswin/i){
			my $winsrc = $src;
			my $windst = $dst;
			$winsrc=~s/\//\\/g;
			$windst=~s/\//\\/g;
			`copy $winsrc $windst`;
		}
		else{
			`cp -p $src $dst`;
		}
		-f $dst or err("cannot copy $src -> $dst\n");
	}


	#
	# build packages
	#

	@variant_files;
	
	foreach my $package (@packages,@testpackages){
		
		#
		# make sure that there is no old stuff 
		#

		-d $BUILDROOT and (deltree($BUILDROOT) or err("cannot remove buildroot\n"));
		makedir($BUILDROOT,0775) or err("cannot create buildroot\n");
		
		#
		#
		#

		my $JDBC_version;
		if ($package->{'name'} eq 'JDBC'){
			my ($rc_key,$rc_value);
			if($^O =~ /aix/i){
				sub JarBuildString {
					my ($filename) = @_;

					return undef unless (-r $filename);

					my $in = SAPDB::Install::Jar->new ();

					$in->OpenArchive ($filename);
					$in->Open ('META-INF/MANIFEST.MF');

					my $buildstr;
					for (;;) {
						my $line = $in->ReadLine ();
						last unless defined ($line);

						($buildstr) = ($line =~ /^Implementation-Version:\s"(.+)"$/);
						last if (defined $buildstr);

						($buildstr) = ($line =~ /^Implementation-Version:\s(.+)$/);
						last if (defined $buildstr);
					}

					$in->Close ();
					$in->CloseArchive ();
		
					return undef unless (defined $buildstr);

					my ($rel, $build);

					($rel, $build) =
					($buildstr =~ /(\d\.\d\.\d+)\s+Build:\s+(\d[\d|-]*)/);

					unless (defined $rel && defined $build) {
						($rel, $build) =
						($buildstr =~ /(\d\.\d\.\d+)\s+Build\s+(\d[\d|-]*)/);
					}

					unless (defined $rel && defined $build) {
						return undef;
					}

					$build =~ s/-$//;

					return ("JAR  ".$rel." Build: ".$build);
				}

				sub JarBuildInfo {
					my ($filename) = @_; 
					return undef unless (-r $filename);
				
					my $buildstr = JarBuildString ($filename);
					return undef unless (defined $buildstr);
				
					my (@info) =
					($buildstr =~ /^JAR\s+(\d+)\.(\d+)\.(\d+)\s+Build:\s+(\d+)/);
					return undef unless (defined $info[0] && defined $info[1] &&
										defined $info[2] && defined $info[3]);
	
					#
					# convert to integer
					#
					$info[3] += 0;
					return ("Build" => \@info);
				}
							
				($rc_key,$rc_value) = JarBuildInfo("$INSTROOT/runtime/jar/sapdbc.jar");
			}
			else{
				($rc_key,$rc_value) = SAPDB::Install::Jar::JarBuildInfo("$INSTROOT/runtime/jar/sapdbc.jar");
			}	
			my ($jmaj,$jmin,$jcor,$jbuild) = @$rc_value;
			$jcor = "0$jcor" if length($jcor) == 1;
			$jbuild = "0$jbuild" if length($jbuild) == 1;
			$JDBC_version = "$jmaj.$jmin.$jcor.$jbuild";
			err("cannot get JDBC version\n") unless $JDBC_version =~ /^\d+\.\d+\.\d+\.\d+$/;
			msg("set JDBC version to $JDBC_version\n");
		} 
		else{
			$JDBC_version = undef;
		}
		
		
		
		#	
		# copy and change script and signature 
		#
		
		foreach my $file (${%$package}{'script'},'PACKAGEDATA'){
			open(IN,$packpath.'/'.${%$package}{'subdir'}.'/'.$file) or err('cannot open '.$packpath.'/'.${%$package}{'subdir'}."/$file\n");
			open (OUT,">$BUILDROOT/$file") or err("cannot create $BUILDROOT/$file\n");
			my $test_file;
			while(<IN>){
				chomp;
				s/§RELEASE/$JDBC_version/g if $JDBC_version =~ /\S/;
				if($package->{'name'} eq 'Database Manager' and /§RELEASE/){
					my $dbmgui_version = getDBMGuiVersion();
					if(defined $dbmgui_version){
						print "set DBM GUI version to $dbmgui_version ...\n";
						s/§RELEASE/$dbmgui_version/g;
					}
				}
				if($package->{'name'} eq 'SQL Studio' and /§RELEASE/){
					my $sqlstudio_version = getSQLStudioVersion();
					if(defined $sqlstudio_version){
						print "set SQL Studio version to $sqlstudio_version ...\n";
						s/§RELEASE/$sqlstudio_version/g;
					}
				}
				foreach my $string (keys(%replacements)){
					s/$string/$replacements{$string}/g;
				}
				if(/^TEST_FILE\s/){
					($test_file) = (/=\s*"(.*)"/);
				}
				if(/^BUILD_STRING\s/){
					($package->{'build_string'}) = (/^BUILD_STRING\s*=\s*"(.+)"/);
				}
				if(/^MAKE_ID\s/){
					($package->{'make_id'}) = (/^MAKE_ID\s*=\s*"(.+)"/);
				}
				print OUT "$_\n" or err("cannot write into $BUILDROOT/$file\n");
			}
			close(IN);
			if($test_file =~ /\S/ && -f "$USR/$test_file"){
				my $build_info = getBuildInfo("$USR/$test_file");
				if(defined $build_info){
					foreach my $key (keys(%$build_info)){
						next if($key eq 'Build');
						my $ukey = $key;
						$ukey =~ tr/[a-z]/[A-Z]/;
						print OUT ($ukey.' = "'.$build_info->{$key}."\"\n"); 
					}
				}
			}
			close(OUT);	
		}
	
		if(exists ${%$package}{'additional_installer_files'}){
			push @all_files ,@{${%$package}{'additional_installer_files'}};
			push @additional_installer_files, @{${%$package}{'additional_installer_files'}};
		}
		
		# call special package function if exists, at the moment only for pcr packages needed
		#exists ${%$package}{'specialFunc'} and &{${%$package}{'specialFunc'}}($package); 

		#
		# copy files, generate md5sums and change filelist (attach checksums)
		#
		
		my $size = 0;
		open(IN,$packpath.'/'.${%$package}{'subdir'}.'/'.${%$package}{'filelist'}) or err('cannot open '.$packpath.'/'.${%$package}{'subdir'}.'/'.${%$package}{'filelist'}."\n");
		open(OUT,">$BUILDROOT/".${%$package}{'filelist'}) or err("cannot create $BUILDROOT/".${%$package}{'filelist'}."\n");
		my @dirs;		
		my @missing_files;
		my @filelist_buf;
		my @dbg_files = ();	
		my $dbg_pattern;
		if(exists $package->{'DBG_PACKAGE'}){
			$dbg_pattern = '^'.join('$|^',@{$package->{'DBG_PACKAGE'}->{'files'}}).'$';
			$dbg_pattern =~ s/\./\\\./g;
			$dbg_pattern =~ s/\*/\.\*/g;
		}		
		
		while(<IN>){
			chomp;
			/^#/ && next;
			/\S/ or next; # ignore empty lines
			s/§PACKVERS/$RUNTIME_VERSION/g;
			s/§RELVER/$RELVER/g;
			my $src = $_;
			my $filemode;
			if(/\s\d{4,5}\s*$/){
				($src, $filemode ) = (/(^.+\S)\s+([0-7]{4,5})\s*$/);
			}
			
			if(exists $package->{'DBG_PACKAGE'}){
				if($src =~ /$dbg_pattern/){
					push @dbg_files,[$src,$filemode];
					next;
				}	
			}
			
			$_ = $src;
			
			
			if(exists ${%{${%$package}{'FileMappings'}}}{$src}){
				$src = ${%{${%$package}{'FileMappings'}}}{$src};
			}
			
			if(${%$package}{'name'} eq 'APO COM' or ${%$package}{'name'} eq 'APO LC APPS'){
				$src = "sap/$src";
			}
				
			$src = "$USR/$src";
			
			my $dst="$BUILDROOT/$_";
			my $dir = $dst;
			$dir=~s/\/[^\/]*$//;
			my $rel_dir = $_;
			$rel_dir=~s/\/[^\/]*$//;
			-d $dir or makedir($dir,0775) or err("cannot create directory $dir\n");
			
			unless(-f $src){
				push @missing_files,$src;
				next;
			}
			
			copy($src,$dst,{'binmode' => 1,'nochown' => 1});
			
			-f $dst or err("cannot copy $src -> $dst\n");
			
			$size += (stat($src))[7];

			unless($^O =~ /mswin/i){
				my $mdir,$sep;
				foreach my $ldir (split('/',$rel_dir)){
					$mdir .= $sep.$ldir;
					$sep = '/';
					my $pattern = $mdir;
					$pattern =~ s/\//\\\//g;
					$pattern = '[^\|]'.$pattern.'[\|$]';
					push @dirs,$mdir unless ((join('|',@dirs)) =~ /$pattern/); 
				}
				
				my @statbuf = stat($dst);
								
				my $pmask = 0222; # default: remove write permissions for all

				$pmask = ${%$package}{'pmask'} if defined ${%$package}{'pmask'};
				
				my $newmode;
				
				if(defined $filemode){
					$newmode = oct($filemode);
				}
				else{
					$newmode = $statbuf[2] & ~$pmask; 
				}

				if($newmode != $statbuf[2]){
					chmod($newmode,$dst) or err("cannot change mode of $dst\n");
					if(($newmode & 06777) != ((stat($dst))[2] & 06777)){
						err("cannot set mode of $dst to	".sprintf("0%o\n",$newmode));
					}
				}
			}
						
			my $md5sum=getMD5Sum($dst) or err("cannot get md5sum of $dst\n");
			my $line = "\"$_\" $md5sum\n";
			push @filelist_buf,$line;
			print OUT  $line or err("cannot write into $BUILDROOT/".${%$package}{'filelist'}."\n");
		}
		close(IN);
		close(OUT);
		open(FD,">>$BUILDROOT/PACKAGEDATA") or err("cannot open PACKAGEDATA to append package size\n");
		print FD "SIZE = \"$size\"\n";
		if(not defined $opt_no_dbg_files and $#dbg_files > -1){
			print FD "DBG_PACKAGE = \"".$package->{'DBG_PACKAGE'}->{'name'}."\"\n";
		}
		close(FD);
		

		if($#missing_files > -1){
			err("can\'t find following files [package ".$package->{'name'}."]: \n".join("\n",@missing_files)."\n");
		}

		
		unless($MAJ == 7 && $MIN < 4){	
			foreach my $dir (@dirs){
				chmod(0555,"$BUILDROOT/$dir") || err("cannot chmod of directory $dir to 555\n");			
			}
		}

		if(exists ${%$package}{'add_files'}){
			foreach my $file (@{${%$package}{'add_files'}}){
				my ($path) = ($file =~ /^(\S+)\/[^\/]+$/);
				my $dest = ($path =~ /\S/ ? "$BUILDROOT/$path" : $BUILDROOT);
				-d $dest or makedir($dest,0775);
				copy("$INSTROOT/$file",$dest,{'binmode' => 1});
			}
		}
		
		
		
		chdir("$BUILDROOT") or err("cannot change into directory $BUILDROOT\n");
		my $tarname="$PACKAGEPATH/".${%$package}{'archive'};
		
		# 
		# generate list of files and folders to pack
		# -> sequencing is important for performance of installer
		#
		
		my $packlist = 'PACKAGEDATA '.${%$package}{'script'}.' ' .${%$package}{'filelist'};
		opendir(DH,'.') || err("cannot open directory: $!\n");
		my @content = readdir(DH);
		closedir(DH);
		
		foreach (@content){
			(/^PACKAGEDATA$/ or /^${%$package}{'script'}$/ or /^${%$package}{'filelist'}$/ or /^\.{1,2}$/) and next;
			$packlist="$packlist $_"; 
		}
		
		#
		# pack archive
		#
		
		msg ("pack archive $tarname\n"); 
		#print "COMMAND: tar -cvpzf $tarname $packlist\n";
		if($^O =~ /mswin/i){
			do_cmd("$tar -cvpf - $packlist | gzip -c -9 >$tarname");
		}
		else{
			do_cmd("$tar -cvpzf $tarname $packlist");
		}
		$? == 0 or err("tar error while packing \"".${%$package}{'name'}."\"\n");
		-f $tarname or err("no archive \"$tarname\" produced\n");
		
		if(exists $package->{'variant'}){
			foreach my $variant (keys(%{$package->{'variant'}})){
				my @variant_dbgfiles = ();
				msg("creating  $variant package variant of ".$package->{'name'}."...\n");
				my $vardir = "$WRK/fast/$INSTALL_SUB_PATH/$variant";
				if(-d $vardir){
					chmod(0775,$vardir);
				}
				else{
					makedir($vardir,0775);
				}
				my %md5sums;
				foreach my $file (keys(%{$package->{'variant'}->{$variant}->{'files'}})){
					if(exists $package->{'DBG_PACKAGE'}){
						if($file =~ /$dbg_pattern/){
							push @variant_dbgfiles,$file;
							next;
						}	
					}
					msg("using ".$package->{'variant'}->{$variant}->{'files'}->{$file}." as $file\n");
					my $dest = $BUILDROOT."/$file";
					my $source = $INSTROOT.'/'. $package->{'variant'}->{$variant}->{'files'}->{$file};
					-f $source or err("file $source of $variant ".$package->{'name'}." not found\n");
					my @statbuf  = stat($dest);
					chmod($statbuf[2] | 0200,$dest); #add write permission for owner
					copy($source,$dest,{'binmode'=> 1,'nochown' => 1});
					chmod($statbuf[2],$dest);
					$md5sums{$file} = getMD5Sum($dest); 	
				}
				$package->{'variant'}->{$variant}->{'dbgfiles'} = \@variant_dbgfiles
					if $#variant_dbgfiles > -1;
				open(OUT,">$BUILDROOT/".${%$package}{'filelist'}) or err("cannot create $BUILDROOT/".${%$package}{'filelist'}."\n");
				foreach my $line (@filelist_buf){
					my ($file) = ($line =~ /\"(.*)\"/);
					if(exists $md5sums{$file}){
						print OUT "\"$file\" $md5sums{$file}\n";
					}
					else{
						print OUT $line;		
					}
				
				}
				close(OUT);
				push @variant_files , "$variant/".$package->{'archive'};
				$tarname = $vardir.'/'.$package->{'archive'};
				msg ("pack archive $tarname\n"); 
				#print "COMMAND: tar -cvpzf $tarname $packlist\n";
				if($^O =~ /mswin/i){
					do_cmd("$tar -cvpf - $packlist | gzip -c -9 >$tarname");
				}
				else{
					do_cmd("$tar -cvpzf $tarname $packlist");
				}
				$? == 0 or err("tar error while packing \"".${%$package}{'name'}."\"\n");
				-f $tarname or err("no archive \"$tarname\" produced\n");
			}
		}
		chdir('..'); # leave buildroot
		if(not defined $opt_no_dbg_files and $#dbg_files > -1){
			$package->{'dbgfiles'} = \@dbg_files;
			my $rc = packdbg($package);
			push @all_files,$package->{'DBG_PACKAGE'}->{'name'};
			
		}
	}

	foreach my $package (@packages){
		push @all_files,$package->{'archive'};
	}

	push @all_files,@installer_files;
	push @installer_files,@additional_installer_files;
	
	if( (not $opt_profile =~ /\S/) || $opt_no_archive){
		if($opt_finalize){
			$EM_MAKE || err("no valid productive environment to finalize\n");	
			
			#
			# find free pool slot
			#

			my $slot = findFreePoolSlot($LC_POOL_DIR,'changelist.log',$LC_STATE eq 'DEV' ?  4 : 3);
			
			my $new_dir = $slot;
			
			#
			# copy package into pool directotry
			#
			
			
			msg("copy $WRK/fast/$INSTALL_SUB_PATH/$package_name -> $new_dir/$package_name\n");
			
			my $vmake_gid;
			unless($^O =~ /mswin/i){
				$vmake_gid = getgrnam('vmake') || err("unknown group \"vmake\"\n");
			}
			
			-d $new_dir or makedir($new_dir,0775,\$undefined,\$vmake_gid) or err("cannot create directory $new_dir: $!\n");
			
			
			foreach my $package (@all_files){
				msg("copy $WRK/fast/$INSTALL_SUB_PATH/$package -> $slot/$package\n");
				copy("$WRK/fast/$INSTALL_SUB_PATH/$package",$slot,{'binmode' => 1}) || err("cannot copy: $!");
			}
			
			copyItems("$WRK/fast/$INSTALL_SUB_PATH",$slot,['BuildPackage/PackDefs.pm']);

			copy("$OWN/changelist.log",$slot);

			#
			# change latest link
			#
				
			linkSlot($slot,'LastBuild');
		}		
		exit 0;
	}


	#
	# build liveCache + APO COM package
	#
	
	if ($APOMAKE && $opt_profile eq 'APO LiveCache'){
	
		my @server_files;
		my $found_pcr = 0;
		my $pcr_archive_name;
		my $package_name;
	
		my $lca_state;
		
		if(exists $ENV{'BRANCH_INFO'} and $ENV{'BRANCH_INFO'} =~ /\S/){
			$lca_state = $ENV{'BRANCH_INFO'};
		}
		else{
			$lca_state = 'RAMP';
		}
		 
	
		if($LC_STATE =~ /RAMP|HOT/ and $lca_state !~ /DEV|COR/i){	
			my $server_profile = 'Server';
			@server_files = @BuildPackage::LCPackDefs::installer_files;
			-d $BUILDROOT and (deltree($BUILDROOT) or err("cannot remove buildroot\n"));
			makedir($BUILDROOT,0775) or err("cannot create buildroot\n");
  			chdir($BUILDROOT) || err("cannot change into buildroot\n");
			makedir($profile_data{$opt_profile}->{'inner_subdir'},0775) or err("cannot create inner subdir\n");
			foreach my $packname (@{$BuildPackage::LCPackDefs::profiles{$server_profile}}){
				foreach my $package (@BuildPackage::LCPackDefs::allpackages){
					if ($packname eq $package->{'name'}){
						if($packname =~ /^PCR/){
							$found_pcr = 1;
							$pcr_archive_name = $package->{'archive'};
						}
						push @server_files, $package->{'archive'},
									ref($package->{'additional_installer_files'}) eq 'ARRAY' ? 
											@{$package->{'additional_installer_files'}} : 
											();							
					}
				}
			}
		} 
		my $lcapps_package;
		foreach my $package (@allpackages){
			if ($package->{'name'} =~ /^APO/){
				$lcapps_package = $package->{'archive'};
				
			}
		}
		
		if($LC_STATE =~ /RAMP|HOT/ and $lca_state !~ /DEV|COR/i){	
			if($found_pcr){
				my $ardir = "$LC_OWN/$SAPDB_COMPONENTS_DIR"; 
				opendir(DH,$ardir);
				push @server_files, grep { -f "$ardir/$_" and /^PCR/ and not /$pcr_archive_name/ } readdir(DH);
				closedir(DH);
			}

			foreach my $file (@server_files){
				$file =~ /\S/ or next;
				copy("$LC_OWN/$SAPDB_COMPONENTS_DIR/$file",$profile_data{$opt_profile}->{'inner_subdir'},{'binmode' => 1,'nochown' => 1});						
			}
			copy("$WRK/fast/$INSTALL_SUB_PATH/$lcapps_package",$profile_data{$opt_profile}->{'inner_subdir'},{'binmode' => 1}) || err ("cannot copy APO LC APPS  package\n");		
			$package_name = $profile_data{$opt_profile}->{'archive_name'}.'.SAR';
			do_cmd("SAPCAR -cvf $WRK/fast/$INSTALL_SUB_PATH/$package_name ".$profile_data{$opt_profile}->{'inner_subdir'});
			-f "$WRK/fast/$INSTALL_SUB_PATH/$package_name" &&
			msg("package $WRK/fast/$INSTALL_SUB_PATH/$package_name successfully packed\n");
		}
		
		if($opt_finalize){
		
			#
			# find free pool slot
			#

			my $slot = findFreePoolSlot($APO_POOL_DIR,'make.id',3);
			
			my $new_dir = $slot;
			
			#
			# copy package into pool directotry
			#
			
			
			my $vmake_gid;
			unless($^O =~ /mswin/i){
				$vmake_gid = getgrnam('vmake') || err("unknown group \"vmake\"\n");
			}
			
			-d $new_dir or makedir($new_dir,0775,\$undefined,\$vmake_gid) or err("cannot create directory $new_dir: $!\n");
			
			
			copy("$OWN/changelist.log",$new_dir) || err("cannot copy changelist.log: $!\n");
			
			if($LC_STATE =~ /RAMP|HOT/ and $lca_state !~ /DEV|COR/i){	
				msg("copy $WRK/fast/$INSTALL_SUB_PATH/$package_name -> $new_dir/$package_name\n");
				copy("$WRK/fast/$INSTALL_SUB_PATH/$package_name",$new_dir,{'binmode' => 1}) || err("cannot copy: $!\n");		
			}
			
			#
			#	copy components
			#
						
			my $components = $new_dir.'/'.$LCAPPS_COMPONENTS_DIR;
			-d $components or makedir($components,0775,\$undefined,\$vmake_gid) or err("cannot create directory $components: $!\n");
			msg("copy $WRK/fast/$INSTALL_SUB_PATH/$lcapps_package -> $components\n");
			copy("$WRK/fast/$INSTALL_SUB_PATH/$lcapps_package",$components,{'binmode' => 1}) || err("cannot copy $WRK/fast/$INSTALL_SUB_PATH/$lcapps_package -> $components: $!\n");		
						
			copy("$OWN/make.id",$new_dir) || err("cannot copy make.id: $!\n");


			#
			# change latest link
			#
				
			linkSlot($slot,'LastBuild');
		}
		close(LOG);
		exit 0;
	
	}	
	
	
	#
	# build packages without APO COM
	# LC or other SAP DB package
	#

	my $PACKNAME = $opt_profile;
	if(exists $profile_in_name{$opt_profile}){
		$PACKNAME = $profile_in_name{$opt_profile};
	}
	
	
	
	$PACKNAME =~ tr/[A-Z]/[a-z]/;
	$PACKNAME =~ s/\s/_/g;
	
	my $name_COR = $COR;
	my $name_BUILD = $BUILD;
	$name_COR =~ s/^0*//;
	$name_COR eq '' and $name_COR = '0';
	$name_BUILD =~ s/^0*//;
	$name_BUILD eq '' and $name_BUILD = '0';
	
	
	$packagename="sapdb-$PACKNAME-$OS-".$BIT."bit-$ARCH-".$MAJ.'_'.$MIN.'_'.$name_COR.'_'.$name_BUILD;
	my $package_name = $packagename.'.sar'; 

	my $packages;
	@profile_files;
	foreach my $package (@profile_packages){
		$packages .= ' '.${%$package}{'archive'};
		push @profile_files,${%$package}{'archive'};
	}

	
	my $ext='tgz';
	my $exec_ext;
	$exec_ext = '.exe' if ($^O =~ /mswin/i);
	my $errtxt;
	my $inner_subdir;
	if($opt_format eq 'sar' || $EM_MAKE){
		$ext='sar';
		#print "\$opt_profile = $opt_profile\n";
		#foreach my $key (keys(%profile_data)){
		#	print "found member $key in \%profile_data\n";
		#}
		
		$ext =~ tr/[a-z]/[A-Z]/;
		$PACKNAME = $profile_data{$opt_profile}->{'archive_name'};
		$packagename = $PACKAGEPATH.'/'.$profile_data{$opt_profile}->{'archive_name'}.'.'.$ext;
		$inner_subdir = $PACKAGEPATH.'/'.($profile_data{$opt_profile}->{'inner_subdir'});
		if(-d $inner_subdir){
			deltree($inner_subdir) or err("cannot delete existing $inner_dir\n:$!\n");
		}
		else{
			makedir($inner_subdir,0775) or err("cannot create directory $inner_subdir\n");
		}
		my @archive_files = (@profile_files,@installer_files,@oldpcr_packages);
		copyItems($PACKAGEPATH,$inner_subdir,\@archive_files); 
		chdir($PACKAGEPATH) or err("cannot change into dir $PACKAGEPATH\n");
		my $cmd = "SAPCAR -cvf $PACKNAME.$ext ".$profile_data{$opt_profile}->{'inner_subdir'};	
		do_cmd($cmd);
		$? == 0 || err("sapcar error: $!\n");
		deltree($inner_subdir)  or err("cannot delete $inner_dir\n:$!\n");;
	}
	else{
		unless($opt_os_style){
			$ext =~ tr/[a-z]/[A-Z]/;
		}
		if($opt_profile eq 'all'){
			$inner_subdir = $packagename;
			if($opt_os_style){
				$PACKNAME = $packagename;
				$packagename = $PACKAGEPATH.'/'.$packagename.'.'.$ext;
			}
			else{
				$PACKNAME = 'SAPDB'.$MAJ.$MIN.$COR.'_'.$BUILD;
				$packagename = $PACKAGEPATH.'/'.$PACKNAME.'.'.$ext;
			}
		}
		else{
			if($opt_os_style){
				$PACKNAME = $profile_data{$opt_profile}->{'inner_subdir'};
				$packagename = $PACKAGEPATH.'/'.$PACKNAME.'.'.$ext;
			}
			else{
				$PACKNAME = $profile_data{$opt_profile}->{'archive_name'};
				$packagename = $PACKAGEPATH.'/'.$profile_data{$opt_profile}->{'archive_name'}.'.'.$ext;
			}
			$inner_subdir = ($profile_data{$opt_profile}->{'inner_subdir'});
		}
		if(-d $PACKAGEPATH.'/'.$inner_subdir){
			deltree($PACKAGEPATH.'/'.$inner_subdir) or err("cannot delete existing $PACKAGEPATH/$inner_subdir:$!\n");
		}
		else{
			makedir($PACKAGEPATH.'/'.$inner_subdir,0775) or err("cannot create directory $inner_subdir\n");
		}
		my @archive_files = (@profile_files,@installer_files,@oldpcr_packages);
		copyItems($PACKAGEPATH,$PACKAGEPATH.'/'.$inner_subdir,\@archive_files); 
		chdir($PACKAGEPATH) or err("cannot change into dir $PACKAGEPATH\n");
		$packagename =~ s/^[a-z]://i if $^O =~ /mswini/;			
		my $cmd = "$tar -cvpzf $packagename $inner_subdir";	
		do_cmd($cmd);
		$? == 0 || err("tar error: $!\n");
		deltree($PACKAGEPATH.'/'.$inner_subdir)  or err("cannot delete $PACKAGEPATH/$inner_subdir :$!\n");
	} 
	if(-f $packagename){
		msg ("packed $packagename successfully\n");
	}
	else{
		err ("cannot pack mainpackage \"$packagename\": $errtxt\n");
	}
	if($opt_finalize){
		
		if($EM_MAKE){
			$package_name = $profile_data{$opt_profile}->{'archive_name'}.'.SAR';	

			#
			# find free pool slot
			#
	
			my $slot = findFreePoolSlot($LC_POOL_DIR,'make.id',($LC_STATE eq 'DEV' ?  4 : 3),'^SAPDB'.$MAJ.$MIN.$COR.'_\d\d.SAR$|^MAXDB'.$MAJ.$MIN.$COR.'_\d\d.SAR$');
			defined $slot or err("cannot find free pool slot\n");
			my $new_dir = $slot;	
			
			my $vmake_gid;
			unless($^O =~ /mswin/i){
				$vmake_gid = getgrnam('vmake') || err("unknown group \"vmake\"\n");
			}

			-d $new_dir or makedir($new_dir,0775,\$undefined,\$vmake_gid) or err("cannot create directory $new_dir: $!\n");
		
		
			foreach my $src_dir (keys(%lcitems)){
				my $dest_dir = $new_dir;
				exists ${%{$lcitems{$src_dir}}}{'subdir'} and $dest_dir .= '/'.$lcitems{$src_dir}->{'subdir'};
				copyItems($src_dir,$dest_dir,$lcitems{$src_dir}->{'items'});				
				
			}
			
			if(defined @BuildPackage::LCPool::Items::simulator_list){
				open(FD,">$new_dir/simul.lst") or err("cannot create simulator file list $new_dir/simul.lst: $!\n");	
				foreach my $file (@BuildPackage::LCPool::Items::simulator_list){
					$file =~ /\S/ or next;
					print FD $file."\n";
				}	
				close(FD);
			} 

			foreach my $file (@add_pool_files){
				msg('copy '.$file->[0].' -> '."$new_dir/".$file->[1]."\n");
				copy($file->[0],"$new_dir/".$file->[1],{'binmode' => 1,'nochown' => 1,'createdir' => 1});
			}
							
			
			#
			# copy SAP DB components
			#
			if($opt_components || $opt_profile_components){
				my $components_dir = $slot."/$SAPDB_COMPONENTS_DIR";
				-d $components_dir or (makedir($components_dir,0775) or err("cannot create SAP DB components directory\n"));
				foreach my $file (@all_files){
					msg("copy $WRK/fast/$INSTALL_SUB_PATH/$file -> $components_dir/$file\n");
					copy("$WRK/fast/$INSTALL_SUB_PATH/$file",$components_dir,{'binmode' => 1}) || err("cannot copy: $!");				
				}
				copyItems("$WRK/fast/$INSTALL_SUB_PATH",$components_dir,['BuildPackage/PackDefs.pm',@variant_files]);
			}
				if($#testpackages > -1){
				my $test_components_dir = $slot."/$SAPDB_TEST_COMPONENTS_DIR";
				-d $test_components_dir or (makedir($test_components_dir,0775) or err("cannot create SAP DB test components directory\n"));
				foreach my $package (@testpackages){
					my $file = $package->{'archive'};
					msg("copy $WRK/fast/$INSTALL_SUB_PATH/$file -> $test_components_dir/$file\n");
					copy("$WRK/fast/$INSTALL_SUB_PATH/$file",$test_components_dir,{'binmode' => 1}) || err("cannot copy: $!");				
				}
			}
			
			
			
			#
			# copy package into pool directotry
			#
	
			#msg("copy $WRK/fast/$INSTALL_SUB_PATH/$package_name -> $slot/$package_name\n");
			#copy("$WRK/fast/$INSTALL_SUB_PATH/$package_name",$slot,{'binmode' => 1}) || err("cannot copy: $!");		
		
			copy("$OWN/make.id",$new_dir) || err("cannot copy make.id: $!\n");
						
			#
			# change latest link
			#
						
			linkSlot($slot,'LastBuild');
				
			close(LOG);
			exit 0;									
		}
		
		if($USE_FTP){
			err("no ftp access rights to put package to CD-Images\n");
		}
		
		msg("copy $packagename.$ext to CD-Images\n");
		my $ospath;
		if(exists $ENV{'BIT64'} or $^O =~ /osf/i){
			$ospath = $OS64_PATH;
		}
		else{
			$ospath = $OS32_PATH;
		}
		my $cmd;
		if($^O =~ /mswin/i){
			$cmd = "copy $packagename.$ext \\\\$image_server\\DB_REL\\$MAJ$MIN$COR$BUILD\\$ospath\\$packagename.$ext";
		}
		else{
			$cmd = "rem cp $packagename.$ext $image_server:d:/DB_REL/$MAJ$MIN$COR$BUILD/$ospath/$packagename.$ext";
		}
		do_cmd($cmd);
		$? == 0 || err("cannot copy archive to CD-Images: $errtext");
	}
	close(LOG);
	exit 0;
}


1;






