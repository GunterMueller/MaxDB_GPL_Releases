#!/devtool/local/bin/perl
#
#
#	installtools.pl
#
#	install sources: generate script initdev
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


package SAPDB::Sources::Install;

use Cwd;

$tool;
$curDir=cwd();

sub jmpDir{
	my ($dir)=@_;
	$dir =~ s/\\/\//g;
	my $scriptdir=$0;
	$scriptdir=~s/\\/\//g;
	$scriptdir=~s/\/[^\/]*$//;
	if($^O=~/mswin/i){
		$scriptdir=~/^$dir$/i or chdir($scriptdir);
	}
	else{
		$scriptdir=~/^$dir$/ or chdir($scriptdir);
	}
}

sub testSubDirs{
	my ($dir) = @_;
	my $ready=1;
	foreach my $subdir ('sys','sys/src','sys/desc'){
		-d "$dir/$subdir" or print "something wrong with directory structure: \"$dir/$subdir\" not found\n" and $ready = 0;
	}
	return $ready;
}

sub getTool{
	while(1){
		print "please enter the root directory of your devtools: ";
		chomp($_=<STDIN>);
		unless(-d $_){
			print "directory \"$_\" not found\n";
			print "abort installation (y/n)?: ";
			chomp($_=<STDIN>);
			/^y$/i and return 0;
			next;
		}
		$tool=$_ and return 1;
	}	
}

sub main{
	print "\n\n\t\tInstallation Of MaxDB Sources\n";
	print "\t\t******************************\n\n";
	my @ARGV=@_;
	my $ready=1;
	my $errtext="\nMaxDB sources not correctly installed\n";
	SAPDB::Sources::Install::Registry::read() or "cannot find devtool path\n";
	jmpDir($curDir);
	$curDir=cwd();
	testSubDirs("$curDir/MaxDB_ORG") or $ready=0;
	die $errtext unless $ready;
	print "your sources are in \"$curDir\"\n";
	$tool=$SAPDB::Sources::Install::Registry::values{'ToolPath'};
	-d $tool or print "DevTools not found \n" and $ready=0;
	unless($ready){
		$ready=getTool();
	}
	die $errtext unless $ready;
	print "your devtools are in \"$tool\"\n";
	$ready=SAPDB::Sources::Install::Version::getVersion();
	die $errtext unless $ready;
	SAPDB::Sources::Install::GoScript::readSkeleton() or $ready=0;
	die $errtext unless $ready;
	%replace=(
		'§MAJ' => $SAPDB::Sources::Install::Version::MAJOR,
		'§MIN' => $SAPDB::Sources::Install::Version::MINOR,
		'§COR' => $SAPDB::Sources::Install::Version::CORRECTION_LEVEL,
		'§BLDPRFX' => $SAPDB::Sources::Install::Version::BUILDPRAEFIX,
		'§DEV' => "$curDir/MaxDB_DEV",
		'§ORG' => "$curDir/MaxDB_ORG",
                '§LCSTATE' => 'RAMP'
	);
	if($^O=~/mswin/i){
		$replace{'§DEV'}=~s/\//\\/g;
		$replace{'§ORG'}=~s/\//\\/g;
	}
	SAPDB::Sources::Install::GoScript::write(%replace) or $ready=0;
	die $errtext unless $ready;
        my $tmpdir = "$curDir/MaxDB_DEV/tmp";
        if (! -d $tmpdir) {
            mkdir $tmpdir;
        }
	print "\nMaxDB sources correctly installed\n";
}



package SAPDB::Sources::Install::Registry;
#use Win32API::Registry 0.13 qw( :ALL );
$ini_file="/usr/spool/sql/ini/SAP_DBDev.ini";
$ini_key='software\SAP\SAP DBDev';
%values;

sub readWin{
	require Win32API::Registry;
	import Win32API::Registry qw( :ALL );
	sub readValues{
		#sub retrun hash of values
		my $root=$_[0];
		unless($root){
			 $root = &HKEY_LOCAL_MACHINE;
		}
		my $keyname=$_[1];
		my $notexpand=$_[2];# true -> dont expand expandstring
		RegOpenKeyEx($root, $keyname, 0, &KEY_ALL_ACCESS , $key )	
                    or  print "Can't open $root\\$keyname: $^E\n" and return 0;
		my $i=0;
		my %returnvalue;
		RegQueryInfoKey ($key, [], [], [], [], [], [], $ValCount, [], [], [], []);
		for ($index = 0; $index < $ValCount; $index++){	
			RegEnumValue( $key, $index ,$sValName, 20, [], $piType, $pValData, 50 );
			#if(Win32::GetLastError()==259){last;}
			if($piType==&REG_BINARY){
				$pValData=unpack("H*",$pValData);	
			}
			elsif($piType==&REG_EXPAND_SZ){
				ExpandEnvironmentStrings($pValData,$pValData,[]) unless $notexpand;
				$pValData =~ s/^([\w:\/\ ]*).*/$1/g;
			}
			elsif($piType==&REG_SZ){
				#$pValData =~ s/^([\w:\/\ ]*).*/$1/g;
                # this s/ truncates directories with -, so it has been removed
			}
            $pValData =~ s/\000+$//;  # strip terminating 0
			$returnvalue{$sValName}=$pValData;
			
		}
		RegCloseKey( $key ) or  print "Can't close $root\\$keyname: $^E\n" and return 0;	
		return %returnvalue;
	}
	%values = readValues(&HKEY_CURRENT_USER,$ini_key)
            or readValues(&HKEY_LOCAL_MACHINE,$ini_key)
            or print "cannot find devtools values in registry - please install\n" and return 0;
	return 1;	
}


sub readUX{
	-f $ini_file or print "ini file \"$ini_file\" not found - please install devtools first\n" and return 0;
	open(FD,$ini_file) or print "cannot open \"$ini_file\"\n" and return 0;
	while(<FD>){
		chomp($_);
		/^([^=]*)=([^=]*)$/ or next;
		my $key=$1;
		my $value=$2;
		$key=~s/^\s*//;
		$key=~s/\s*$//;
		$value=~s/^\s*//;
		$value=~s/\s*$//;
		$values{$key}=$value;
	}
	close(FD);
}


*read =   $^O =~ /^mswin/i ? \&readWin :
	\&readUX;




package SAPDB::Sources::Install::Version;
use FileHandle;
$RELVER;
$MAJOR;
$MINOR;
$CORRECTION_LEVEL;
$BUILDPRAEFIX;

sub validEntries {
    foreach my $entry (@_) {
        if (!defined ($entry)) {
            return 0;
        }
        if ($entry == -1) {
            return 0;
        }
    }
    return 1;
}

sub getVersion{
        my ($MaV,$MiV,$CoL,$BP);
        if (-e "$SAPDB::Sources::Install::curDir/MaxDB_ORG/sys/src/config/Buildinfo/VERSIONS") {
            ($MaV,$MiV,$CoL,$BP) = GetVersionInfo("$SAPDB::Sources::Install::curDir/MaxDB_ORG/sys/src/config/Buildinfo/VERSIONS");
        }
        elsif (-e "$SAPDB::Sources::Install::curDir/SAPDB_ORG/sys/src/config/Buildinfo/VERSIONS") {
            ($MaV,$MiV,$CoL,$BP) = GetVersionInfo("$SAPDB::Sources::Install::curDir/SAPDB_ORG/sys/src/config/Buildinfo/VERSIONS");
        }
        else {
            ($MaV,$MiV,$CoL,$BP) = GetBuildVersion("$SAPDB::Sources::Install::curDir/SAPDB_ORG/sys/src/sp/gsp100.h");
        }
        if (!validEntries ($MaV, $MiV, $CoL, $BP)) {
            print "no valid information about source version found\n";
            return 0;
        }
	$RELVER="R$MaV$MiV";
	$MAJOR=$MaV;
	$MINOR=$MiV;
	$CORRECTION_LEVEL="$CoL";
	$BUILDPRAEFIX="$BP";
	return 1;
}


sub GetVersionInfo
{
	my ($MajorVersion,$MinorVersion,$CorrLevel,$BuildPref);
	local($filename) = @_;
	local $fd_gsp100 = new FileHandle;
	if ( ! $fd_gsp100->open($filename, "r") )
	{
		warn "Can't open $filename (input) :$!\n";
		return undef;	
	}
        $MajorVersion = $MinorVersion = $CorrLevel = $BuildPref = -1;
        my ($inKernelSection) = undef;
        while ( <$fd_gsp100> )
        {
                if (/[[](.*)[]]/) {
                    if ($1 eq "kernel") {
                        $inKernelSection = 1;
                    }
                    else {
                        $inKernelSection = undef;
                    }
                }
                else {
                    if ($inKernelSection) {
                        if (/version=([0-9]+)[.]([0-9]+)[.]([0-9]+)[.]([0-9]+)/) {
                            $MajorVersion = $1;
                            $MinorVersion = $2;
                            $CorrLevel    = $3;
                            $BuildPref    = $4;
                            last;
                        }
                    }
                }
        }
        $fd_gsp100->close;
        $BuildPref+=0;
        return ($MajorVersion, $MinorVersion, $CorrLevel, $BuildPref);
}

sub GetBuildVersion
{
	my ($MajorVersion,$MinorVersion,$CorrLevel,$BuildPref);
	local($filename) = @_;
	local $fd_gsp100 = new FileHandle;
	if ( ! $fd_gsp100->open($filename, "r") )
	{
		warn "Can't open $filename (input) :$!\n";
		return undef;	
	}
	else
	{
	   $MajorVersion = $MinorVersion = $CorrLevel = $BuildPref = -1;
	   while ( <$fd_gsp100> )
	   {
			if ( /^\s*#\s*define\s*MAJOR_VERSION_NO_STR_SP100\s*"(.*)"/ )
			{	$MajorVersion=$1;	}
			if ( /^\s*#\s*define\s*MINOR_VERSION_NO_STR_SP100\s*"(.*)"/ )
			{   $MinorVersion=$1;	}
			if ( /^\s*#\s*define\s*CORRECTION_LEVEL_STR_SP100\s*"(.*)"/ )
			{	$CorrLevel=$1;		}
			if ( /^\s*#\s*define\s*BUILD_PREFIX_STR_SP100\s*"(.*)"/ )
			{	$BuildPref=$1;		}
		}
		$fd_gsp100->close;
		$BuildPref+=0;
	}
	return ($MajorVersion, $MinorVersion, $CorrLevel, $BuildPref);
}

package SAPDB::Sources::Install::GoScript;
$skeleton_file='iprofile.tmp';
$ext = $^O =~ /^mswin/i ? '.bat' : '';
$goscript="initDev_MaxDB$ext";
@filebuf;

sub readSkeleton{
	$skeleton_file="$SAPDB::Sources::Install::tool/$skeleton_file";
	open(FD,$skeleton_file) or print "cannot open skeleton file\n" and return 0;
	while(<FD>){
		chomp($_);
		push @filebuf,$_;
	}	
	close(FD);
	return 1;
}

sub write{
	my (%replace)=@_;
	open(FD,">$goscript") or print "cannot create file \"$goscript\"\n" and return 0;
	foreach my $line (@filebuf){
		foreach my $keyword (keys(%replace)){
			$line=~s/$keyword/$replace{$keyword}/g;
		}
		print FD "$line\n";
	}
	close(FD);
	return 1;
}

SAPDB::Sources::Install::main(@ARGV);
