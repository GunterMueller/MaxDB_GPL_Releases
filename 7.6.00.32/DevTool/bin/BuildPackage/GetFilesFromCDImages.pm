#! /usr/bin/perl
#
# $Header: //prodprojects/vmake/dev/src/_tools/maketools/sys/src/BuildPackage/GetFilesFromCDImages.pm#7 $
# $DateTime: 2006/01/03 15:46:49 $
# $Change: 581457 $
#
# Desc: 	- copy files from CDImage server to make environment
#			- files to copy are described in pcr_releases.xml
#			- part of buildpackage tool		 
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


package BuildPackage::GetFilesFromCDImages;

use IO::File;
use XML::Parser::Expat;
use GME::XMLElement;
use Exporter;
use BuildPackage::XMLParser;
use BuildPackage::Tools;
use BuildPackage::Vars;
use File::Copy;

@ISA = ('Exporter');
@EXPORT=('getFiles');


sub setErrMsgHandler{
	my ($handler) = @_;
	*err = $handler;
}

sub setMsgHandler{
	my ($handler) = @_;
	*msg = $handler;
}

local @ARGV=@_;


# sub parse the XML file with release/file information and return a tree of XMLElemets
sub ParseXMLFile {
	my ($fh, $rc);
	$^O=~/mswin/i && $XMLFile=~s/\//\\/g;
	$fh = IO::File::new();
	$rc = open ($fh, "< ".$XMLFile);
	unless ($rc) {
		err("cannot open ".$XMLFile."\n");
		err("WARNING: NO XML FILE FOR OTHER RUNTIMES FOUND\n"); 
		return 0;
	}
	my $Parser = BuildPackage::XMLParser::new ();
	my $Tree = $Parser->ParseStream ($fh);
	$fh->close ();
	return ($Tree);
}	


# sub return a subtree (XMLElement) specified by entity name and its content
	sub getSubTree{
	my ($tree,$type,$content,$no_err)=@_;
	$tree==0 && return 0;
	while(1){
		if($tree->GetType eq $type){
			while(1){
				if($tree->GetContent eq $content){
					return $tree;
				}	
				$tree=$tree->VisitNeighbor;
				if($tree==0){
					if($no_err){
						msg("WRN: \"$content\" not found in type \"$type\" in XML tree\n");
					}
					else{
						err ("\"$content\" not found in type \"$type\" in XML tree\n");
					}
					return 0;
				}
			}
			last;
		}
		$tree=$tree->VisitChild;
		if($tree == 0){
			err("entity \"$type\" not found in tree\n");
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

sub getFiles{
	my ($release,$destroot)=@_;
	my @returnvalue;
	$release=~/^([0-9]+)\.([0-9]+)\.([0-9]+)\.([0-9]+)$/ or err("wrong release format\n");
	my $maj=$1;
	my $min=$2;
	my $cor=$3;
	my $pat=$4;
	#$maj = "0$maj" if (length($maj)==1);
	#$min = "0$min" if (length($min)==1);
	$cor = "0$cor" if (length($cor)==1);
	$pat = "0$pat" if (length($pat)==1);
	$release="$maj$min$cor$pat";
	
	my $ftp;
	my $extra_msg;
	if($USE_FTP){
		$extra_msg = '(using FTP host '.$image_server.')';
		require Net::FTP;
		$ftp = Net::FTP->new($image_server)
			or err("cannot get ftp connection\n");
		$ftp->login('anonymous') or err('FTP: '.$ftp->message . "\n");
		$ftp->binary() or err('FTP: '.$ftp->message."\n");
	}
	msg("inserting compatibility files... $extra_msg\n");
	my $tree=ParseXMLFile();
	$tree==0 and err("WARNING: no valid XML tree generated\n") and return 0;	
	
	# $tree is root of tree with XML entities
	
	my $subtree=getSubTree($tree,'current_release',$release);
	
	#
	#	 change os true64 -> tru64 26.08.2002
	#	 workaround for already closed builds with 'true64' in pcr_releases.xml 	
	#
	
	my $backup_tree = $subtree;
	$subtree=getSubTree($subtree,'os',$OS,$OS eq 'tru64' ? 1 : 0); # subtree for related os
	if($subtree == 0 && $OS eq 'tru64'){
		$subtree=getSubTree($backup_tree,'os','true64');
	}
	
	$subtree=getSubTree($subtree,'arch',$ARCH);	   # subtree for related architecture 	
	$subtree=getSubTree($subtree,'objectmode',$BIT); # subtree for objectmode
	$subtree == 0 && err("WARNING: no valid XML sub tree found (current_release: $release | os: $OS | arch: $ARCH | object mode | $bit_num )\n") and return 0;
	my @oldReleaseTrees=getChildren($subtree);
	
	unless($USE_FTP){
		my @releases;
		my $count=getCDImageReleases(\@releases); 
		my @tmp;
		if ($#releases==-1 && not $USE_FTP){
			msg("WARNING: NO files found on CDImages\n");
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
			$found == 0 && err("WARNING: NO DIRECTORY FOR RELEASE ".$rel_tree->GetContent." ON CDImages FOUND\n");	
		}
	
		@oldReleaseTrees=@tmp;
	}

	if ($#oldReleaseTrees==-1){
		msg("no files defined for $OS $ARCH in release $release $bit_num bit\n");
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
			msg("get $rel \"$type\" $bitmode bit: $dest\n");
			my $os_path;
			if($bitmode == 64){
				$os_path=$OS64_PATH;
			}
			elsif($bitmode == 32){
				$os_path=$OS32_PATH;
			}
			else{
				err("WARNING: bitmode not set! - skipping file\n");
				next;
			}
			$os_path=~s/\\/\//g;
			my @fix_level = split ('/',"$rel/$os_path");
			getit("$rel/$os_path/$source",$dest,$destroot,$access,$ftp,$#fix_level + 1);
			push @returnvalue,$source;
		}
	}
	if ($USE_FTP){
		$ftp->quit or err('FTP: '.$ftp->message."\n");
	}
	return @returnvalue;
}

sub getCDImageReleases{
	my ($array_ref)=@_;
	*returnarray=$array_ref;
	my $counter=0;
	if($^O=~/mswin/i){
		my $dirs = `cmd /C dir \\\\$image_server\\$CDImagePath`;
		# use "cmd \C" because of existing "dir.exe" in devtools 
		$? and err("error reading dirs on cd-images: $!\n");
		my @dirs = split "\n",$dirs;
		foreach my $dir (@dirs){
			$dir=~/^.*\<DIR\>\s*([0-9]*)$/ and push @returnarray,$1 and $counter++;	
		}		
	}
	else{
		my $dirs =  `rem sh $image_server \"ls -l d:/$CDImagePath\"`; 
		$? and err("error reading dirs on cd-images: $!\n");
		my @dirs = split "\n",$dirs;
		foreach my $dir (@dirs){
			$dir=~/^d.*\s([0-9]+)$/ and push @returnarray,$1 and $counter++;	
		}
	}
	return $counter;
}

$rpath_cache = {};

sub getRealFTPPath{
	my ($ftp,$path,$is_dir,$max_depth) = @_;
	if (defined $max_depth){
		if ($max_depth == 0){
			return $path;
		}
		$max_depth--;	
	}

	my $test = $path;
	$test =~ tr/A-Z/a-z/;
	
	if (exists $rpath_cache->{$test}){
		return $rpath_cache->{$test};
	}
	
	my $rvalue;
	
	if ($path eq '/'){
		return $path;
	}
	
	my ($dir,$node) = ($path =~ /^(.*)\/([^\/]+)$/);
	
	if ($dir eq ''){
		$dir = '/';
	} 
	
	unless (defined $dir){
		err ("invalid argument\n");
	}
		
	my $rpath = getRealFTPPath($ftp,$dir,1,$max_depth);
	
	unless (defined $rpath){
		return undef;
	}

	my %entries = remoteLs($ftp,$rpath);
	my $found = 0;
	my $last_match;
	my $pattern = '^'.$node.'$';
	foreach my $entry (keys (%entries)){
		if ($entry eq $node){
			$found = 1;
			last;
		}
		if ($entry =~ /$pattern/i){
			if ($is_dir){
				next if $entries{$entry}->{'type'} eq 'f';
			}
			else{
				next if $entries{$entry}->{'type'} eq 'd';	
			}
			$last_match = $entry;
		}	
	}
	if ($found){
		$rvalue = $rpath.'/'.$node;
	}
	else{
		if (defined $last_match){
			$rvalue = $rpath.'/'.$last_match;	
		}
		else{
			err ("ERR: ".($is_dir ? 'directory' : 'file')." $path not found on $image_server\n");
		}
	}
	$rpath_cache->{$test} = $rvalue;
	return $rvalue;
		
}


sub getit{
	my ($source,$destination,$destroot,$access,$ftp,$fix_level)=@_;
	my $dir = $destination;
	#$destination="$buildroot/$destination";
	$destroot=~s/\\/\//g;
	$dir =~ s/\\[^\\]*$//;
	$dir =~ s/\/[^\/]*$//;
	-d "$destroot/$dir" or makedir("$destroot/$dir",0775) or err("cannot create dir $destroot/$dir\n");
		
	if($USE_FTP){
		my $FTPDST = "$destroot/$destination";
		$FTPDST =~ s/\\/\//g;
		my ($localDir,$localFile) = ($FTPDST =~ /^(.*)\/([^\/]+)$/);  
		
		
		my $FTPSRC = '/'.$CDImagePath.'/'.$source;
		$FTPSRC =~ s/\\/\//g;
		if ($^O =~ /mswin/i){
			my @nodes = split('/',$FTPSRC);
			my $level = $#nodes + 1;
			$cdi = $CDImagePath;
			$cdi =~ s/\\/\//g;
			my @cdi_level = split ('/',$cdi);
			$fix_level += $#cdi_level + 1;  
			if ($level > 7){
				$FTPSRC = getRealFTPPath($ftp,$FTPSRC,0,$level - $fix_level - 1);
			}
		}	
		$ftp->get($FTPSRC,$FTPDST) or err('FTP: '.$ftp->message."\n");
	}
	else{
		if($^O=~/mswin/i){
			$destroot=~s/\//\\/g;
			$destination=~s/\//\\/g;
			$source=~s/\//\\/g;
			$dir=~s/\//\\/g;
			copy("\\\\$image_server\\$CDImagePath\\$source","$destroot\\$dir",{'binmode' => 1}) or err("FILE COPY ERROR: $!\n") and return;
			$destroot=~s/\\/\//g;
			$destination=~s/\\/\//g;
			-f "$destroot/$destination" or err("cannot copy file \\\\$image_server\\$CDImagePath\\$source: $!\n") and return;
		}
		else{
			-f "$destroot/$destination" && (unlink("$destroot/$destination") || err("ERR: cannot remove \"$destroot/$destination\"\n"));
			`rem cp $image_server:d:/$CDImagePath/$source $destroot/$destination`;	
			($? != 0) and err("FILE COPY ERROR: $!\n") and return 0;
			-f "$destroot/$destination" || err("cannot copy PCR $image_server:$CDImagePath/$source: $!\n") && return;
			
		}	
	}
	unless($^O=~/mswin/i){
		if($access=~/^\d{3,4}$/){
			chmod(oct($access), "$destroot/$destination") or err("ERROR setting access right for $destroot/$destination to $access: $!\n") and return ;
		}
	}		
}	

1;

