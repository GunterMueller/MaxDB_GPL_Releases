#! /usr/bin/perl
#
# $Header: //prodprojects/vmake/dev/src/_tools/maketools/sys/src/BuildPackage/APOCOM30A.pm#8 $
# $DateTime: 2006/03/20 18:20:16 $
# $Change: 587163 $
#
# Desc: special module to handle APO COM package 3.0A
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

package BuildPackage::APOCOM30A;

use BuildPackage::Tools;
use BuildPackage::Vars;
use Cwd;
use Exporter;

BuildPackage::Tools::setErrMsgHandler(sub {die(@_)});
BuildPackage::Tools::setMsgHandler(sub {print @_});



if($^O =~ /mswin/i){
	require WinLink;
	import WinLink;
}


#@ISA =('Exporter');
#@EXPORT = ('');


#
#	 initialize 
#

my $startdir = cwd();
my $make_home = $startdir;


opendir(DH,$make_home) or die("cannot search in directory $make_home: $!\n");
my @com_archives = grep {/^SAPCOM_3\.0A_\d+_\d+_\S+_\d\d.SAR$/} readdir(DH);
closedir(DH);

if($#com_archives > 0){
	die("too many com archives found: ".join(',',@com_archives)."\n");	
}
elsif($#com_archives == -1){
	die("no com archive found\n");	
}

my $com_archive = $com_archives[0];

print "selecting archive $make_home/$com_archive\n";


my ($lca_version,$lca_build,$lca_changelist,$lca_os,$lca_mode) = 
	($com_archive =~ /^SAPCOM_(3\.0A)_(\d+)_(\d+)_(\S+)_(\d\d)/);


my $os,$arch,$pack_os;
my $platform;

if($lca_os =~ /win/i){
	$os = 'nti386';
	$pack_os = 'win';
	$arch = 'i386';
	$platform = "NTintel";
	if ($lca_mode == 64){
		$os = 'ntia64';
		$arch = 'ia64';
		$platform = "NTia64";
		$ENV{'BIT64'} = 1; 
	}
}
elsif($lca_os =~ /alpha/i){
	$os = 'alphaosf';
	$arch = 'alpha';
	$pack_os = 'tru64';
}
elsif($lca_os =~ /aix_5/i){
	$os = 'rs6000_51_64';
	$arch = 'ppc';
	$pack_os = 'aix5';
}
elsif($lca_os =~ /aix_4/i){
	$os = 'rs6000_64';
	$arch = 'ppc';
	$pack_os = 'aix';

}
elsif($lca_os  =~ /hpux/i){
	$os = 'hp_64';
	$arch = 'hppa';
	$pack_os = 'hpux';

}
elsif($lca_os  =~ /solaris/i){
	$os = 'sun_64';
	$arch = 'sparc';
	$pack_os = 'sun';
}

my %lc_dirs;
my %hosts;
my %platforms;
my %poolsubs;
 
my $config_file = '/bas/APO_30A_74/src/config/.makedbv_74';

$config_file = '//pwdfm007.wdf.sap.corp/depot/apo/3.0A_74/maketools/iniAPO_COM.cmd' if $^O =~ /mswin/i;

open(FD,$config_file) or die("cannot open $config_file\n");
while(<FD>){
	chomp;
	if($^O =~ /mswin/i){
		if(/set\sLCROOT\d\d=\\\\\S+\\LC_POOL.*\\LC_74\d{2,4}_\d\d_\w+\\000\\sys\\src\\lcsrc/i){
			my ($mode,$host,$pool_sub,$platform,$lc_dir) = (/set\sLCROOT(\d\d)=\\\\([^\\]+)\\(.*\\?)LC_POOL\\(.*\\?)(LC_74\d{2,4}_\d\d_\w+)\\000\\sys\\src\\lcsrc/i);
			if($mode == 32){
				$lc_dirs{'nti386'} = $lc_dir;
				$hosts{'nti386'} = $host;
				$platforms{'nti386'} = $platform;
				$poolsubs{'nti386'} = $pool_sub;
			}
			elsif($mode == 64){
				$hosts{'ntia64'} = $host;
				$platforms{'ntia64'} = $platform;
				$lc_dirs{'ntia64'} = $lc_dir; 
				$poolsubs{'ntia46'} = $pool_sub;
			}
		}
		last if (exists $lc_dirs{'nti386'} and  exists $lc_dirs{'ntia64'});
	}
	else{
		/^#/ and next;
		if(/^\S+\/lc\s+LC_\d{4,6}_\d{2}_\w+/){
			my ($los,$lc_dir) = (/^(\S+)\/lc\s+(LC_\d{4,6}_\d{2}_\S+)/);
			$lc_dirs{$los} = $lc_dir;
		}
	}
}
close(FD);

die("platform not found\n") unless exists $lc_dirs{$os}; 

my $lc_dir = $lc_dirs{$os};
my $platform_path = $platforms{$os};
my $host = $hosts{$os};
my $pool_sub = $poolsubs{$os};

my ($lc_version,$mode) = ($lc_dir =~ /^LC_(\d+)_(\d{2})/); 
my ($lc_maj,$lc_min,$lc_cor) = ($lc_version =~ /^(\d)(\d)(\d\d)/);
my $lc_build;
if($^O =~ /mswin/i){
	$pool_sub =~ s/\\/\//g;
	$platform_path =~ s/\\/\//g;
	#$lc_dir = "//$host/${pool_sub}LC_POOL/${platform_path}$lc_dir";
	$lc_dir = "\\\\production\\SCM\\MAXDB\\LC_POOL\\$platform\\$lc_dir"; 
}
else{
	$lc_dir = "/sapmnt/production/makeresults/SCM/MaxDB/LC_POOL/$os/$lc_dir"; 
}
-d $lc_dir or die("cannot find liveCache dir \"$lc_dir\"\n");
-e "$lc_dir/LastBuild" or die("LastBuild not found in $lc_dir: $!\n");
if(-l "$lc_dir/LastBuild"){
	my $resolved = readlink("$lc_dir/LastBuild");
	$lc_dir = "$lc_dir/$resolved";
}
elsif(-f "$lc_dir/LastBuild" and $^O =~ /mswin/i){
	my $resolved = readLink("$lc_dir/LastBuild");
	$lc_dir = "$lc_dir/$resolved";
}
elsif(-d "$lc_dir/LastBuild"){
	$lc_dir = "$lc_dir/LastBuild";
}
else{
	die("LastBuild is a file\n");
}

if(length($lc_version) == 6){
	($lc_build) = ($lc_version =~ /(\d\d)$/);
}
else{
	-f "$lc_dir/VERSIONS" or die("cannot get liveCache build\n");
	my %lc_data = readini("$lc_dir/VERSIONS");
	my $version = $lc_data{'kernel'}->{'version'};
	$build = ($version =~ /\.(\d{2})$/);
}

print "liveCache version is $lc_maj.$lc_min.$lc_cor.$lc_build $mode bit\n";
print "LC Apps version is $lca_version.$lca_build\n";
print "liveCache directory is $lc_dir\n";



my %replacements = (
	'§BIT' => $lca_mode,
	'§APORELEASE' => "$lca_version.$lca_build",
	'§LC_RELEASE' => "$lc_maj.$lc_min.$lc_cor.$lc_build",
	'§SOEXT' =>	($pack_os eq 'hpux') ? '.sl' : '.so' 
);


my $script = "$lc_dir/sys/src/lcsrc/packages/apocom/instapocom";
my $packagedata = "$lc_dir/sys/src/lcsrc/packages/apocom/PACKAGEDATA";
my $niping = "$lc_dir/sys/src/lcsrc/sap/niping$EXEEXT";
my $buildroot = "$make_home/buildroot";

-d $buildroot and deltree($buildroot);
makedir($buildroot,0775) or die("cannot create buildroot $buildroot\n");
chdir($buildroot);

print "extracting LC Apps archive...\n";

my $outtext = `SAPCAR -xvf $make_home/$com_archive 2>&1`;

unless($? == 0){
	die("cannot unpack sar archive: $outtext (exitcode $?)\n");
}

copy($niping,$buildroot,{'binmode' => 1,'nochown' => 1});


#
#create file list;
#

print "creating file list with checksums...\n";

opendir(DH,$buildroot) or die("cannot look into directory $buildroot\n");
my @lca_files = grep { -f "$buildroot/$_"} readdir(DH);
closedir(DH);

my @file_list;
foreach my $file (@lca_files){
	my $sum = getMD5Sum("$buildroot/$file");
	push @file_list,"\"$file\" $sum";
}

open(FH,">$buildroot/apocom.lst") or die("cannot create file list\n");
foreach (@file_list){
	print FH $_."\n";
}
close(FH);

#
# create PACKAGEDATA
#

print "generating PACKAGEDATA...\n";

my @packagedata;
open(FH,$packagedata) or die("cannot open PACKAGEDATA");
while(<FH>){
	chomp;
	foreach my $repl (keys(%replacements)){
		s/$repl/$replacements{$repl}/g;
	}
	push @packagedata,$_;
}
close(FH);
open(FH,">$buildroot/PACKAGEDATA") or die("cannot create PACKAGEDATA");
foreach (@packagedata){
	print FH $_."\n";
}
close(FH);
my @statbuf = stat($packagedata);
utime ($statbuf[8],$statbuf[9],"$buildroot/PACKAGEDATA");
unless($^O =~ /mswin/i){
	chmod($statbuf[2],"$buildroot/PACKAGEDATA")
}

#
#	copy install script
#

print "copying install script...\n";

copy($script,$buildroot,{'nochown' => 1});


#
# pack lca package
#


my $tarcmd = 'gtar ';
$tarcmd = 'tar ' if ($^O =~ /linux|mswin/i);

if($^O =~ /mswin/i){
	$make_home =~ s/^[a-z]://i;
}

$tarcmd .= "-cvpzf $make_home/APOCOM.TGZ PACKAGEDATA apocom.lst instapocom ".join(' ',@lca_files);

print "creating APOCOM.TGZ...\n";


$outtext = `$tarcmd 2>&1`;
unless($? == 0){
	die("error during tar command $tarcmd: $outtext (exitcode $?)\n");
}


#
#	load package definition module
#

print "loading liveCache profile definition module...\n";

my $lca_rel = $lca_version;
$lca_rel =~ s/\.//g;
my $lca_short_vers = "$lca_rel$lca_build";
$lca_short_vers =~ tr/[A-Z]/[a-z]/;
local @INC = ("$lc_dir/SAPDB_COMPONENTS",@INC);
BuildPackage::Vars::SetVars({
'MAJ' => $lc_maj,
'MIN' => $lc_min,
'COR' => $lc_cor,
'BUILD' => $lc_build,
'RUNTIME_VERSION' => "$lc_maj$lc_min$lc_cor",
'APO_SHORT_VERS' => $lca_short_vers,
'APO_BUILD' => $lca_build,
'ARCH' => $arch,
'OS' => $pack_os,
'BIT' => $lca_mode
});

require BuildPackage::PackDefs;
import BuildPackage::PackDefs;
my @lc_components;
my $pcr_inside;

my $lca_archive_name = $profile_data{'APO LiveCache'}->{'archive_name'};
my $lca_inner_subdir = $profile_data{'APO LiveCache'}->{'inner_subdir'};

foreach my $package (@{$profiles{'Server'}}){
	my $found = 0;
	foreach my $lpackage (@allpackages){
		if($lpackage->{'name'} eq $package){
			$found = 1;
			$pcr_inside = $lpackage->{'archive'} if $package =~ /^PCR\s/;
			push @lc_components,$lpackage->{'archive'};
			push @lc_components,@{$lpackage->{'additional_installer_files'}} if ref($lpackage->{'additional_installer_files'}) eq 'ARRAY';
			last;
		}
	}
	die("archive for package $package not found\n") unless $found;
}
push @lc_components,@installer_files;

if(defined $pcr_inside){
	opendir(DH,"$lc_dir/SAPDB_COMPONENTS");
	@pcrs = grep {/^PCR/ and not /$pcr_inside/ and -f "$lc_dir/SAPDB_COMPONENTS/$_" } readdir(DH);
}

push @lc_components,@pcrs;


#
#	copy liveCache components
#

print "copying liveCache components...\n";

chdir($startdir);
deltree($buildroot);
makedir("$buildroot/$lca_inner_subdir",0775);
foreach my $comp (@lc_components){
	print "+ $comp\n";
	copy("$lc_dir/SAPDB_COMPONENTS/$comp","$buildroot/$lca_inner_subdir",{'binmode' => 1,'nochown' => 1});
}
print "+ APOCOM.TGZ\n";
copy("$make_home/APOCOM.TGZ","$buildroot/$lca_inner_subdir",{'binmode' => 1,'nochown' => 1});
chdir($buildroot);
print "packing $lca_archive_name.SAR...\n";
$outtext = `SAPCAR -cvf $make_home/$lca_archive_name.SAR $lca_inner_subdir 2>&1`;
unless($? == 0){
	die("sapcar error while $sarcmd: $outtext (exitcode $?)\n");
}

1;






