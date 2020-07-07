#! /usr/bin/perl
#
# $Header: //prodprojects/vmake/dev/src/_tools/maketools/sys/src/BuildPackage/Old.pm#2 $
# $DateTime: 2005/05/25 08:19:10 $
# $Change: 560346 $
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


package BuildPackage::Old;

use Getopt::Std;


@FILELIST;

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

$logfilename='';

sub getLogFileName{
	if($logfilename eq ''){
		$logfilename="$ENV{'WRK'}/fast/install/buildpackage_".$opt_n."_".timestamp.'.log'
	}
	return $logfilename;
}

sub printout{
	my ($print_string)=@_;
	my $logfile=getLogFileName();
	if( ! -f $logfile){
		open(FDLOG,">$logfile") or die "cannot create logfile \"$logfile\"\n";
	}
	else{
		open(FDLOG,">>$logfile") or die "cannot open logfile \"$logfile\"\n";
	}
	print($print_string);
	print FDLOG $print_string;
	close(FDLOG) 
}



sub makedir{
    #create directory recursively
    #on error return 0 else 1
    my $dir=$_[0]; #arg1: directoryname
    my $mode=$_[1];#arg2: accessrights
	unless($mode){
		$mode=0775; #needed to have write access under unix 
	}
	$_=$dir;
	s/\\/\//g;
	/(.*)\/$/ and (($dir ne "/") or !($dir =~ /^[a-zA-Z]:\/.*/)) and $dir=$1;#remove slash at eol
	$_=$dir;
    unless(-d $dir){
		/(.*)\/[^\/]*$/  and makedir($1,$mode) or return 0;
	
		unless(mkdir $dir , $mode){
			printout("cannot create $dir\n");
			return 0;
		}
	}
    return 1;
}

sub copy{
	
	my $src = $_[0];
	my $dest = $_[1];
	-f $src or printout("cp: file \"$src\" not found\n") and return 0;
	$dest =~ /^[\ ]*$/ and printout("cp: there are no destination\n") and return 0;
	if($^O =~ /.*win32.*/i){
		$src=~s/\//\\/g;
		$dest=~s/\//\\/g;
		$rc=`copy $src $dest`;    
	}
	else{
		$rc=`cp -p $src $dest`; 
	}
	#print "OUT: $rc\n";
}

sub deltree{
	my $tree = $_[0];
	my $rc;
	if($^O=~/.*mswin.*/i){
		$tree=~s/\//\\/g;
		system("rm", "-rf", $tree);
	}
	else{
		$rc= `rm -rf $tree`;
	} 
}
	
sub packtgz{
	my $tgzname=$_[0];
	if($^O=~/.*win32.*/i){
		$tgzname=~s/\//\\/g;
	}
	$rc = `tar -cpf - . | gzip -9 > $tgzname`;
	#print "OUT: $rc\n";
}


sub packsar{
	my $sarname=$_[0];
	if($^O=~/.*win32.*/i){
		$tgzname=~s/\//\\/g;
	}
	$rc = `SAPCAR -cf $sarname *`;
}


sub start{
	local @ARGV=@_;

	$LIBPATH="lib";

	if($^O=~/.*win32.*/i){
		$ARCH="i386";
		$OS="win";
		$CDIMAGEOS='NT';
		$LIBPATH="pgm";
		$SOEXT="dll";
		$OS32 = "NT\\I386";
	}
	elsif($^O=~/linux/i){
		$ARCH="i386";
		$OS="linux";
		$SOEXT="so";
		$OS32="UNIX/LINUX_32";
	}
	elsif($^O=~/.*aix.*/i){
		$ARCH="ppc";
		$SOEXT="so";
		$OS32="UNIX/AIX_32";
		my $outtext = `oslevel`;
		if($outtext =~ /^5/){
			$OS = 'aix5';
			$OS64 = 'UNIX/AIX5_64';
		}
		else{
			$OS="aix";
			$OS64="UNIX/AIX_64";
		}	
	}
	elsif($^O=~/.*solaris.*/i){
		$ARCH="sparc";
		$OS="sun";
		$SOEXT="so";
		$OS32="UNIX/SUNOS_32";
		$OS64="UNIX/SUNOS_64";
	}
	elsif($^O=~/.*osf.*/i){
		$ARCH="alpha";
		$OS="tru64";
		$SOEXT="so";
		$OS64="UNIX/DEC_64";
	}
	elsif($^O=~/.*hp.*/i){
		$ARCH="hppa";
		$OS="hpux";
		$SOEXT="sl";
		$OS32="UNIX/HP11_32";
		$OS64="UNIX/HP11_64";
	}
	elsif($^O=~/svr4/i){
		$ARCH="mips";
		$OS="reliant";
		$SOEXT="so";
		$OS32="UNIX/RLX_32";
		$OS64="UNIX/RLX_64";
	}
	
	my $usage = "\nusage: buildpackage [-h] -n srv|web|dba|client [-f sar|tgz]\n";	
	getopt('hn:f:');
	if($opt_h or ($opt_n ne 'srv' and $opt_n ne 'web' and $opt_n ne 'dba' and $opt_n ne 'client') or ($opt_f ne 'sar' and $opt_f ne 'tgz' and  $opt_f )){
	        print $usage;
	        die "\n";
	}
	
	$INSTROOT=$ENV{INSTROOT};
	$INSTROOT=~s/\\/\//g;
	$WRK=$ENV{WRK};
	$WRK=~s/\\/\//g;
	$INSTSCRIPT="insapdb";
	
	
	if($^O=~/.*win32.*/i){
	       $INSTEXEC="SDBINST.EXE";
	}
	else{
	       $INSTEXEC="SDBINST";
	}
	
	$BUILDROOT="$WRK/fast/buildroot";
	
	# delete all old stuff 
	-d $BUILDROOT and deltree($BUILDROOT);
	
	
	$BUILDROOT =~ s/\\/\//g;
	$SRCPATH="$WRK/fast/install";
	$SRCPATH=~ s/\\/\//g;



	@NEEDEDFILES=(
	       "$SRCPATH/$INSTSCRIPT",
	        "$SRCPATH/$INSTEXEC"
	        );
	
	$PACKAGEPATH="$WRK/fast/install";
	$PACKAGEPATH =~ s/\\/\//g;
	($MAJ,$MIN) = ($ENV{'RELVER'} =~ /^R(\d)(\d)$/);
	($COR) = $ENV{CORRECTION_LEVEL};
	$COR = "0$COR" if (length($COR) == 1);
	$BUILD = $ENV{'BUILDPRAEFIX'};
	$BUILD = "0$BUILD" if (length($BUILD) == 1);
	$_RELEASE=$ENV{RELVER};
	$_RELEASE=~ s/^R([0-9])([0-9])$/$1.$2/;
	$REL4PATH=$_RELEASE;
	$REL4PATH=~s/\.//g;
	$REL4PATH=$REL4PATH.$ENV{CORRECTION_LEVEL}."0";
	$_RELEASE=$_RELEASE.".".$ENV{CORRECTION_LEVEL}.".".$ENV{BUILDPRAEFIX};
	if($ENV{BIT64} or $^O=~/.*osf.*/i){
	        $BIT="64bit";
			$bit_num=64;
	        $^O=~/.*osf.*/i or $LIBPATH="$LIBPATH/lib64";
	}
	else{
	         $BIT="32bit";
				 $bit_num=32;
	}
	$REL4NAME=$_RELEASE;
	$REL4NAME =~ s/\./_/g;
	
	if($opt_n eq 'srv' || $opt_n eq 'client' ){
		$LISTE{'independent'}="$INSTROOT/etc/CAR.LISTE.IND";
		$SCRIPT{'independent'}='instind';
		$NAME{'independent'}='SAPDBIND.TGZ';
		my $partofusage1;
		my $partofusage2;
		if($^O=~/mswin/i){
			$partofusage1='';
			$partofusage2='';
			$partofopt='';
		}
		else{
			$partofusage1=' [-o <dbown>] [-g <dbgrp>]';
			$partofusage2='\t-o <dbown>\t\towner name for database programs\n\t-g <dbgrp>\t\tgroup name for database programs\n';	
			$partofopt='g:o:';
		}
	
		if($opt_n eq 'srv'){
			push(@NEEDEDFILES,"$SRCPATH/frame");
			$PACKNAME='server';
			$LISTE{'dependent'}="$INSTROOT/etc/CAR.LISTE.SRV";
			$SCRIPT{'dependent'}='instsrv';
			$NAME{'dependent'}='SAPDBDEP.TGZ';
			my %replace = (	
				'�RELEASE' => $_RELEASE,
				'�MINRELEASE' => '7.2.4.0',
				'�NAME' => 'Server', 
				'�USAGE' => 'SDBINST [-h] [-C] [-N] [-U <depPath>] [-d <depPath>] [-I <indepDataPath>] [-i <indepProgPath>]'.$partofusage1.' [-n <node>]\n\t-h\t\t\tshow this\n\t-N\t\t\tnew installation -> no update of existing installation\n\t-U <depPath>\tupdate installation in installation root <depPath>\n\t-d <depPath>\tpath to install release dependent files\n\t-I <indepDataPath>\tpath for release independent data\n\t-i <indepProgPath>\tpath to install release independent programs\n'.$partofusage2.'\t-n <node>\t\thost name\n\n'
			);  
			$REPLACEHASH{'dependent'}=\%replace;
			$REPLACEHASH{'independent'}=\%replace; # for inner package scripts
			%REPLACE = ( 			# for script 'insapdb'
				'�IMPORT' => "import SAPDB::Install::IND;\nunless(\$opt_C){\n\timport  SAPDB::Install::SRV;\n}",
				'�PACKAGE' => "\$package[0]=\"$NAME{'independent'}\";\n\$script[0]=\"$SCRIPT{'independent'}\";\nunless(\$opt_C){\n\t\$package[1]=\"$NAME{'dependent'}\";\n\$script[1]=\"$SCRIPT{'dependent'}\";\n}",
				'�USAGE' => 'SDBINST [-h] [-C] [-b] [-X [-D <db instance>] [-c <control user>,<control password>] [-p <domain password>] ] [-d <depPath>] [-I <indepDataPath>] [-i <indepProgPath>]'.$partofusage1.' [-n <node>]\n\n\t-h\t\t\tshow this\n\t-C\t\t\tclient installation\n\t-d <depPath>\tpath to install release dependent files\n\t-I <indepDataPath>\tpath for release independent data\n\t-i <indepProgPath>\tpath to install release independent programs\n'.$partofusage2.'\t-n <node>\t\thost name\n\t-X \t\tdatabase instance update\n\t-D <database instance>\t\tdb instance to update, only one instance per installation allowed\n\t-c <control user>,<control password>\t\tuser password combination of database instance\n\t-p <domain password>\t\tdomain password of database instance\n\n',
				'�OPTSTRING' => 'hCbXd:i:I:n:D:p:c:'.$partofopt,
				'�RELEASE' => $_RELEASE,
				'�PRODUCT' => 'SAP DB Server'
			);
		}
		else{
			#client
			$PACKNAME='client';
			my %replace = (	
				'�RELEASE' => $_RELEASE,
				'�MINRELEASE' => '7.2.4.0',
				'�NAME' => 'Client', 
				'�USAGE' => 'SDBINST [-h] [-I <indepDataPath>] [-i <indepProgPath>]'.$partofusage1.' [-n <node>]\n\t-h\t\t\tshow this\n\t-I <indepDataPath>\tpath for release independent data\n\t-i <indepProgPath>\tpath to install release independent programs\n'.$partofusage2.'\t-n <node>\t\thost name\n\n'
			);
			$REPLACEHASH{'independent'}=\%replace; # for inner package scripts
			%REPLACE = ( 			# for script 'insapdb'
				'�IMPORT' => "import SAPDB::Install::IND;",
				'�PACKAGE' => "\$package[0]=\"$NAME{'independent'}\";\n\$script[0]=\"$SCRIPT{'independent'}\";",
				'�USAGE' => 'SDBINST [-h] [-b] [-I <indepDataPath>] [-i <indepProgPath>]'.$partofusage1.' [-n <node>]\n\n\t-h\t\t\tshow this\n\t-I <indepDataPath>\tpath for release independent data\n\t-i <indepProgPath>\tpath to install release independent programs\n'.$partofusage2.'\t-n <node>\t\thost name\n\n\n',
				'�OPTSTRING' => 'hbi:I:n:'.$partofopt,
				'�RELEASE' => $_RELEASE,
				'�PRODUCT' => 'SAP DB Client'
			);
		}
	}	
	elsif($opt_n eq 'dba') {
		$PACKNAME='dbanalyzer';
		$LISTE{'dbanalyzer'}="$INSTROOT/etc/DBanalyzer.lst";
		$SCRIPT{'dbanalyzer'}='instdba';
		$NAME{'dbanalyzer'}='SAPDBDBA.TGZ';
		%REPLACE = (                    # for script 'insapdb'
	                '�IMPORT' => "import SAPDB::Install::DBA;",
	                '�PACKAGE' => "\$package[0]=\"$NAME{'dbanalyzer'}\";\n\$script[0]=\"$SCRIPT{'dbanalyzer'}\";",
					'�USAGE' => 'SDBINST [-h]\n',
					'�OPTSTRING' => 'h'.$partofopt,
					'�RELEASE' => $_RELEASE,
					'�PRODUCT' => 'SAP DB DBanalyzer'
	        );
	}
	else{
		$PACKNAME='webtools';
		$LISTE{'webtools'}="$INSTROOT/etc/wahttp.lst";
		$SCRIPT{'webtools'}='instweb';
		$NAME{'webtools'}='SAPDBWEB.TGZ';
		%REPLACE = (                    # for script 'insapdb'
		            '�IMPORT' => "import SAPDB::Install::Web;",
		            '�PACKAGE' => "\$package[0]=\"$NAME{'webtools'}\";\n\$script[0]=\"$SCRIPT{'webtools'}\";",
					'�USAGE' => 'SDBINST [-h]\n',
					'�OPTSTRING' => 'h'.$partofopt,
					'�RELEASE' => $_RELEASE,
					'�PRODUCT' => 'SAP DB Web Tools'
	        );

	}


	if($opt_f eq 'sar'){
		$PACKEXT='sar';
	}
	else{
	 	$PACKEXT='tgz';
	}
	

	if($opt_n eq 'srv'){
		$SUBDIR="/sapdb-$PACKNAME-$OS-$BIT-$ARCH-$REL4NAME";
		-d "${BUILDROOT}${SUBDIR}" or 
			(makedir ("${BUILDROOT}${SUBDIR}", 0775) or 
			print STDERR "cannot create inner subdir\n" and
			exit -1);
		$PACKNAME="SAPDB${MAJ}${MIN}${COR}_${BUILD}".'.'.$PACKEXT;
		$PACKNAME =~ tr/[a-z]/[A-Z]/;
	}
	else{
		$PACKNAME="sapdb-$PACKNAME-$OS-$BIT-$ARCH-$REL4NAME.$PACKEXT";
	}

	$FILELISTNAME="$PACKNAME.filelist";
	
	foreach my $list (values(%LISTE)){
		push @NEEDEDFILES, $list;
	}
	
	foreach my $script (values(%SCRIPT)){
		 push @NEEDEDFILES, "$SRCPATH/$script";
	}
	
	
	

	#############################################################################################
	#here begins section for collecting old precompiler runtimes

	use IO::File;
	use XML::Parser::Expat;
	use GME::XMLElement;
	import XMLParser;

	# sub parse the XML file with release/file information and return a tree of XMLElemets

	sub ParseXMLFile {
		my $HostOs = "UNIX";
		my ($fh, $rc);
		my $XMLFile = "$ENV{'WRK'}/fast/install/pcr_releases.xml";
		$^O=~/mswin/i && $XMLFile=~s/\//\\/g;
		$fh = IO::File::new();
		$rc = open ($fh, "< ".$XMLFile);
		unless ($rc) {
			printout("cannot open ".$XMLFile."\n");
			printout("WARNING: NO XML FILE FOR OLD RUNTIMES FOUND\n"); 
			return 0;
		}
		my $Parser = XMLParser::new ();
		my $Tree = $Parser->ParseStream ($fh);
		$fh->close ();
		return ($Tree);
	}	



	# sub return a subtree (XMLElement) specified by entity name and its content

	sub getSubTree{
		my ($tree,$type,$content)=@_;
		$tree==0 && return 0;
		while(1){
			if($tree->GetType eq $type){
				while(1){
					if($tree->GetContent eq $content){
						return $tree;
					}	
					$tree=$tree->VisitNeighbor;
					if($tree==0){
						print "\"$content\" not found in type \"$type\" in XML tree\n";
						return 0;
					}
				}
				last;
			}
			$tree=$tree->VisitChild;
			if($tree == 0){
				print("entity \"$type\" not found in tree\n");
				return 0;
			} 
		}
	}


	# return all childentities (subtrees) in a list
	
	sub getChildren{
		my($tree)=@_;
		$tree==0 && return;
		$tree=$tree->VisitChild;
		$tree==0 && return;
		my @treelist;
		while(1){
			push @treelist,$tree unless($tree==0);
			$tree=$tree->VisitNeighbor;
			$tree ==0 && return @treelist;
		}
	}


	sub OldPCRs{
		my ($release,$runtimepath)=@_;
		$release=~/^([0-9]*)\.([0-9]*)\.([0-9]*)\.([0-9]*)$/ or die "wrong release format\n";
		my $maj=$1;
		my $min=$2;
		my $cor=$3;
		my $pat=$4;
		#$maj = "0$maj" if (length($maj)==1);
		#$min = "0$min" if (length($min)==1);
		$cor = "0$cor" if (length($cor)==1);
		$pat = "0$pat" if (length($pat)==1);
		$release="$maj$min$cor$pat";
		printout("now try to insert old precompiler runtimes...\n");
		my $tree=ParseXMLFile();
		$tree==0 and printout("WARNING: no valid XML tree generated\n") and return 0;	
		
		# $tree is root of tree with XML entities
		
		my $subtree=getSubTree($tree,'current_release',$release);
		
		#
		#	 change os true64 -> tru64 26.08.2002
		#	 workaround for already closed builds with 'true64' in pcr_releases.xml 	
		#
	
		my $backup_tree = $subtree;
		$subtree=getSubTree($subtree,'os',$OS); # subtree for related os
		if($subtree == 0 && $OS eq 'tru64'){
			$subtree=getSubTree($backup_tree,'os','true64');
		}
	
		$subtree=getSubTree($subtree,'arch',$ARCH);	   # subtree for related architecture 	
		$subtree=getSubTree($subtree,'objectmode',$bit_num); # subtree for objectmode
		$subtree == 0 && printout("WARNING: no valid XML sub tree found\n") and return 0;
		my @oldReleaseTrees=getChildren($subtree);
		my @releases;
		my $count=getCDImageReleases(\@releases); 
		my @tmp;
		
		# check commented out, here only 
	#foreach my $rel (@releases){
	#	if($rel<=$release){
	#		push @tmp,$rel;		
	#	}
	#}
    #@releases=@tmp;
	
	if ($#releases==-1){
		printout("WARNING: NO old precompiler runtime release found on CDImages\n");
		return 1;
	}
	
	@tmp=();
	foreach my $rel_tree (@oldReleaseTrees){
		my $found=0;
		foreach my $rel (@releases){
			if($rel eq $rel_tree->GetContent){
				push @tmp,$rel_tree;
				$found=1;
				last;
			}
		}
		$found == 0 && printout("WARNING: NO DIRECTORY FOR RELEASE ".$rel_tree->GetContent." ON CDImages FOUND\n");	
	}

	@oldReleaseTrees=@tmp;

	if ($#oldReleaseTrees==-1){
		printout("WARNING: NO valid precompiler runtime release found on CDImages\n");
		return 1;
	}
	
	foreach my $rel_tree (@oldReleaseTrees){
		my @file_trees=getChildren($rel_tree);
		my $rel = $rel_tree->GetContent;
		foreach my $file_tree (@file_trees){
			my $type=$file_tree->GetAttribute('type');
			my $bitmode=$file_tree->GetAttribute('bit');
			my $access=$file_tree->GetAttribute('access');
			my $source=$file_tree->GetContent;
			my $dest_tree=$file_tree->VisitChild;
			my $dest;
			$dest = $dest_tree->GetContent if($dest_tree != 0);
			($dest=~/^\s*$/ || !defined $dest) and $dest = $source;
			printout("get $rel $type $bitmode bit: $dest\n");
			my $os_path;
			if($bitmode == 64){
				$os_path=$OS64;
			}
			elsif($bitmode == 32){
				$os_path=$OS32;
			}
			else{
				printout("WARNING: bitmode not set! - skipping file\n");
				next;
			}
			$os_path=~s/\\/\//g;
			getit("$rel/$os_path/$source",$dest,$access);
		}
	}

	}


	my $image_server="p26325";
	
	
	sub getCDImageReleases{
		my ($array_ref)=@_;
		*returnarray=$array_ref;
		my $counter=0;
		if($^O=~/mswin/i){
			my $dirs = `cmd /C dir \\\\$image_server\\CD-Images\\PCR_REL`;
			# use "cmd \C" because of existing "dir.exe" in devtools 
			$? and die "error reading dirs on cd-images: $!\n";
			my @dirs = split "\n",$dirs;
			foreach my $dir (@dirs){
				$dir=~/^.*\<DIR\>\s*([0-9]*)$/ and push @returnarray,$1 and $counter++;	
			}		
		}
		else{
			my $dirs =  `rem sh $image_server \"ls -l d:/CD-Images/PCR_REL\"`; 
			$? and die "error reading dirs on cd-images: $!\n";
			my @dirs = split "\n",$dirs;
			foreach my $dir (@dirs){
				$dir=~/^d.*\s([0-9]+)$/ and push @returnarray,$1 and $counter++;	
			}
		}
		return $counter;
	}

	sub getPCRFilenames{
		my ($rel,$array_ref)=@_;
		*returnarray=$array_ref;
		my $counter=0;
		if($^O=~/mswin/i){
			my $dirs;
			if($OS32){
				$dirs = `cmd /C dir \\\\$image_server\\CD-Images\\PCR_REL\\$rel\\$OS32`;
				$! and die "error: $!\n";
				my @dirs = split "\n",$dirs;
				foreach my $dir (@dirs){
					my %dest;
					#print "TEST: LINE=$dir\n";
					$dir=~/\s*(\S*\.$SOEXT)$/ and $dest{"\\\\$image_server\\CD-Images\\PCR_REL\\$rel\\$OS32\\$1"} = "pgm\\$1" and push @returnarray,\%dest and $counter++;	
				}
			}
			if($OS64){
				$dirs = `cmd /C dir \\\\$image_server\\CD-Images\\PCR_REL\\$rel\\$OS64`;
				$! and die "error: $!\n";
				my @dirs = split "\n",$dirs;
				foreach my $dir (@dirs){
					my %dest;
					$dir=~/\s*(\S*\.$SOEXT)$/ and $helpstr="\\\\$image_server\\CD-Images\\PCR_REL\\$rel\\$OS64\\$1" and $dest{$helpstr} = "pgm\\pgm64\\$1" and push @returnarray,\%dest and $counter++;	
				}
			}
					
		}
		else{
			my $dirs;
			if($OS32){
				$dirs =  `rem sh p26325 \"ls -l d:/CD-Images/PCR_REL/$rel/$OS32\"`; 
				$! and die "error: $!\n";
				my @dirs = split "\n",$dirs;
				foreach my $dir (@dirs){
					my %dest;
					$dir=~/\s*(\S*\.$SOEXT)$/ and $helpstr="d:/CD-Images/PCR_REL/$rel/$OS32/$1" and $dest{$helpstr} = "lib/$1" and push @returnarray,\%dest and $counter++;	
				}
			}
			if($OS64){
				$dirs =  `rem sh p26325 \"ls -l d:/CD-Images/PCR_REL/$rel/$OS64\"`; 
				$! and die "error: $!\n";
				my @dirs = split "\n",$dirs;
				foreach my $dir (@dirs){
					my $mylib='lib/lib64';
					if($^O=~/osf/i){
						$mylib='lib';
					}
					my %dest;
					$dir=~/\s*(\S*\.$SOEXT)$/ and $helpstr="d:/CD-Images/PCR_REL/$rel/$OS64/$1" and $dest{$helpstr} = "$mylib/$1"  and push @returnarray,\%dest and $counter++;	
				}
			}
		}
		return $counter;
	}

	sub getit{
		my ($source,$destination,$access)=@_;
		my $dir = $destination;
		my $buildroot="$BUILDROOT/independent";
		#$destination="$buildroot/$destination";
		$buildroot=~s/\\/\//g;
		$dir =~ s/\\[^\\]*$//;
		$dir =~ s/\/[^\/]*$//;
		-d "$buildroot/$dir" or makedir("$buildroot/$dir") or die "cannot create dir $buildroot/$dir\n";
		if($^O=~/mswin/i){
			$buildroot=~s/\//\\/g;
			$destination=~s/\//\\/g;
			$source=~s/\//\\/g;
			$dir=~s/\//\\/g;
			my $cmd= "copy \\\\$image_server\\CD-IMAGES\\PCR_REL\\$source $buildroot\\$dir";
			`$cmd`;
			($? != 0) and printout("FILE COPY ERROR: $!\n") and return 0;
			$buildroot=~s/\\/\//g;
			$destination=~s/\\/\//g;
			-f "$buildroot/$destination" or printout("cannot copy PCR \\\\$image_server\\CD-IMAGES\\PCR_REL\\$source: $!\n") and return 0;
		}
		else{
			`rem cp $image_server:d:/CD-IMAGES/PCR_REL/$source $buildroot/$destination`;	
			($? != 0) and printout("FILE COPY ERROR: $!\n") and return 0;
			-f "$buildroot/$destination" || printout("cannot copy PCR $image_server:CD-IMAGES/PCR_REL/$source: $!\n") && return 0;
			if($access=~/^\d{3,4}$/){
				`chmod $access $buildroot/$destination`;
				($? !=0 ) and printout("ERROR setting access right for $buildroot/$destination to $access: $!\n") and exit(1);
			}
		}
		# now appand files to filelists
		push @FILELIST,"<independent>/$destination\n";
		$buildroot=~s/\\/\//g;
		$destination=~s/\\/\//g;
		open(FD,">>$buildroot/CAR.LISTE.IND") or die "cannot open \"$buildroot/CAR.LISTE.IND\"\n";
		my $sum = `md5sum $buildroot/$destination`;
		$sum=~s/^([^\ ]*)\ \*.*$/$1/;
		print FD "\"$destination\" $sum\n";
		close(FD);
	}
	

	#check needed files
	#######################################################
	printout("check needed files: "); 
	foreach my $file (@NEEDEDFILES){
		-f $file or printout("failed - file \"$file\" not found\n") and die "\n";
	}
	printout("ok\n");
	




	#package specific part
	######################################################
	if($opt_n eq 'srv' || $opt_n eq 'client' ){
	    if($_RELEASE=~/^7\.2\./){
			makedir("$INSTROOT/runtime/$REL4PATH/$LIBPATH") or die "cannot create directory \"$INSTROOT/runtime/$REL4PATH/$LIBPATH\"\n";
	    }
		#append libpcr so to indlist and copy 
		###########################################################
	    my $list=$LISTE{'independent'};
		$list=~/([^\/]*$)/ and $list=$1;
		copy($LISTE{'independent'}, "$INSTROOT/$list");
		$LISTE{'independent'}="$INSTROOT/$list";
	    if($_RELEASE=~/^7\.2\./){
			open(TMP,">>$INSTROOT/$list") or die "cannot open file $INSTROOT/$list\n";
			print TMP "runtime/$REL4PATH/$LIBPATH/libpcr.$SOEXT\n";
			close(TMP);
			copy("$INSTROOT/$LIBPATH/libpcr.$SOEXT", "$INSTROOT/runtime/$REL4PATH/$LIBPATH");
		}
	}



	#generate buildroot
	######################################################
	foreach my $bez (keys(%LISTE)){
		my $SUBPACKPATH="$BUILDROOT/$bez";
		#generate directory tree
		###################################################
		makedir($SUBPACKPATH);
		if($opt_n eq 'srv'){
	        ($bez eq 'dependent') and copy("$PACKAGEPATH/frame",$SUBPACKPATH);
		}
		# copy files from  list
		##################################################
		printout("copy files from $bez file list\n");
		open(FD,$LISTE{$bez}) or die "cannot open $LISTE{$bez}\n";
		while(my $line=<FD>){
        	chomp($line);
			push @FILELIST,"<$bez>/$line\n";
        	my $path= $line;
        	$path =~ s/(.*)\/[^\/]*/$1/g;
        	if( -d "$INSTROOT/$path" and ! -d "$SUBPACKPATH/$path"){
        	        makedir ("$SUBPACKPATH/$path");
        	}
        	copy ("$INSTROOT/$line", "$SUBPACKPATH/$line"); 
        
		}

		#copy file list and attach md5sums
		###################################################
		printout("copy $bez file list and get md5sums\n");
		seek(FD,0,0);
		my $listname=$LISTE{$bez}; 
		$listname=~ s/^.*\/([^\/]*)$/$1/; #filename without path
		open(NFD,">$SUBPACKPATH/$listname") or die "cannot create file \"$SUBPACKPATH/$listname\"\n";
		while($line=<FD>){
			chomp($line);
		    my $sum = `md5sum $INSTROOT/$line`;
	        $sum=~s/^([^\ ]*)\ \*.*$/$1/;
	        print NFD "\"$line\" $sum\n";
		}
		close(NFD);
		close(FD);
	
		#modify package script and copy
		############################################
		printout("copy and modify script for $bez package\n");
		if($REPLACEHASH{$bez}){
			open(ORG,"$SRCPATH/$SCRIPT{$bez}") or die "cannot open file \"$SRCPATH/$SCRIPT{$bez}\"\n";
			open(TRG,">$SUBPACKPATH/$SCRIPT{$bez}") or die "cannot create file \"$SUBPACKPATH/$SCRIPT{$bez}\"\n";
			while(my $line=<ORG>){
		    	chomp($line);
				my $hashref=$REPLACEHASH{$bez};
				my %to = %$hashref;
				foreach my $from (keys(%to)){
					$line=~s/$from/$to{$from}/g;	
				}
				print TRG "$line\n";
			}
			close(ORG);
			close(TRG);
	
		} 
		else{
			copy("$SRCPATH/$SCRIPT{$bez}",$SUBPACKPATH);
		}
	
		#pack inner package
		if($bez eq 'independent'){
			OldPCRs($_RELEASE);
		}
		printout("build $NAME{$bez}\n");
		chdir($SUBPACKPATH);
		packtgz("${BUILDROOT}${SUBDIR}/$NAME{$bez}");
		chdir($BUILDROOT);
		deltree($SUBPACKPATH);
	
	}

	#copy installation files
	##################################################
	printout("copy installation files\n");
	open(ORG,"$SRCPATH/$INSTSCRIPT") or die "cannot open file \"$SRCPATH/$INSTSCRIPT\"\n";
	$INSTSCRIPT=~ tr/a-z/A-Z/;
	open(TRG,">${BUILDROOT}${SUBDIR}/$INSTSCRIPT")or die "cannot create file \"$${BUILDROOT}${SUBDIR}/$INSTSCRIPT\"\n";
	while($line=<ORG>){
		chomp($line);
		foreach my $from (keys(%REPLACE)){
			$line =~ s/$from/$REPLACE{$from}/g;
		}
		print TRG "$line\n";
	}
	close(ORG);
	close(TRG);
	copy("$SRCPATH/$INSTEXEC","${BUILDROOT}${SUBDIR}");
	if($^O=~/.*mswin.*/i){
		copy("$SRCPATH/perl.dll","${BUILDROOT}${SUBDIR}");
		copy("$SRCPATH/INSTALL.txt" ,"${BUILDROOT}${SUBDIR}");
	}
	else{
		copy("$SRCPATH/INSTALL" ,"${BUILDROOT}${SUBDIR}");
	} 



	#copy PDFs
	##################################################
	#echo "copy PDFs"
	#cp $TGZPATH/*.pdf $DOCPATH
	
	
	#copy demo files
	#################################################
	#echo "copy demo files"
	#cp -R $TGZPATH/demo $INDPATH
	

	#set permissions
	####################################################

	#build main package 
	#########################################################
	printout("build $PACKAGEPATH/$PACKNAME\n");
	chdir($BUILDROOT);
	if($opt_f eq 'sar'){
		packsar("$PACKAGEPATH/$PACKNAME");
	}
	else{
		packtgz("$PACKAGEPATH/$PACKNAME");
	}	
	open(LST,">$PACKAGEPATH/$FILELISTNAME") or printout("cannot create file \"$PACKAGEPATH/$FILELISTNAME\"\n");
	foreach my $file (@FILELIST){
		print LST $file;	
	}
	close(LST);
	chdir($SRCPATH);
	
	#clean
	########################################################
	printout("cleaning buildroot and temporary files\n");
	chdir $ENV{OWN};
	deltree($BUILDROOT);
	if($_RELEASE=~/^7\.2\./){
		deltree("$INSTROOT/runtime");
	}
	
}

1;



# following package specify XML handlers to generate XML tree

package XMLParser;

use Exporter;
use XML::Parser::Expat;
use GME::XMLElement;

@ISA = ('Exporter');

@Export = ('new', 'ParseFile', 'ParseStream');

my %ParserHash;

sub new {
	my $self = {};
	bless $self;

	$self->{Parser} = new XML::Parser::Expat;
	$self->{Parser}->setHandlers (
	        Start  => \&HandleStart,
       		Char   => \&HandleChar,
	        End    => \&HandleEnd );
	$self->{Current} = 0;
	$self->{Parent} = 0;
	$self->{Tree} = 0;

	$ParserHash{$self->{Parser}} = $self;
	return $self;
}

sub ParseFile {
	my $self = shift;
	my $filename = shift;	
	$self->{Parser}->parsefile ($filename);
	return $self->{Tree};
}

sub ParseStream {
	my $self = shift;
	my $fh_ref = shift;	
	undef $bla;
	XML::Parser::Expat::ParseStream 
		($self->{Parser}->{Parser}, $fh_ref, $bla);
	return $self->{Tree};
}

sub HandleStart {
	my ($ExpatParser, $Type, %Attr) = @_;
	my $Parser = $ParserHash{$ExpatParser};
	$Parser->{Current} = XMLElement->new ($Type);        	
	if ($Type eq 'sap_db_releases') {
		# set root of tree
		$Parser->{Tree} = $Parser->{Current};
	}
	else{
		$Parser->{Current}->SetParent ($Parser->{Parent});	
	}
	foreach my $attrib (keys(%Attr)){
		$Parser->{Current}->SetAttribute($attrib, $Attr{$attrib});
	}
	$Parser->{Parent} = $Parser->{Current};
}

sub HandleEnd {
	my ($ExpatParser, $Type) = @_;
	my $Parser = $ParserHash{$ExpatParser};;
	$Parser->{Current}->Insert();
	$Parser->{Parent} = $Parser->{Current}->GetParent();
	$Parser->{Current} = $Parser->{Parent};
}

sub HandleChar {
	my  ($ExpatParser, $Content) = @_;
	my $Parser = $ParserHash{$ExpatParser};;

	# don't care empty Content
	return unless ($Content =~ /\S/);

	$Content =~ /(\S+)/;
	$Parser->{Current}->SetContent ($1);
}

1;

