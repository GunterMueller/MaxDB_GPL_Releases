#! /usr/bin/perl
#
# $Header: //prodprojects/vmake/dev/src/_tools/maketools/sys/src/BuildPackage/PackZip.pm#3 $
# $DateTime: 2005/05/25 08:19:10 $
# $Change: 560346 $
#
# Desc: 	module maniputates sar header
#		needed to set unix file permissions for a sar archive packed by windows SAPCAR	
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


package BuildPackage::PackZip;

use SAPDB::Install::Jar;
use BuildPackage::Tools;
use Exporter;
use Cwd;

@ISA = qw(Exporter); 
@EXPORT = qw(packzip pack_selfextracting_archive);

$DEBUG = 0;

sub packzip{
	my ($archive,$source_tree,@files) = @_;
	-d $source_tree or die("directory $source_tree not found\n");
	my @allfiles = ();
	foreach my $file (@files){
		-e "$source_tree/$file"	or die("$source_tree/$file not found\n");
		if(-d "$source_tree/$file") {
			my $pattern = $source_tree;
			$pattern =~ s/\\/\//g;
			$pattern =~ s/\//\[\\\/\\\\\]/g;
			$pattern = '^'.$pattern.'\\/';
			my @add_files =  grep {s/$pattern//} find("$source_tree/$file",'f');
			push @allfiles,@add_files;
			next;
		}
		push @allfiles,$file;
	}
	$DEBUG && print ">>>>".join("\n>>>>",@allfiles)."\n";	
	my $zip = SAPDB::Install::Jar->new();
	($zip->OpenArchive($archive,'w') == 0) or die("cannot create zip archive $archive\n");
	my $cwd;
	unless($source_tree eq '.'){
		 $cwd = cwd();
		chdir($source_tree) or die("packzip(): cannot change directory\n");
	}
	foreach my $file (@allfiles){
		($zip->Add($file) == 0) or die("packzip(): cannot add file $file\n");
	}
	$zip->CloseArchive();
	chdir($cwd) unless($source_tree eq '.');
}

sub pack_selfextracting_archive{
	my ($archive,$extractor,$source_tree,@files) = @_;
	
	-d $source_tree or die("directory $source_tree not found\n");
	my @allfiles = ();
	foreach my $file (@files){
		-e "$source_tree/$file"	or die("$source_tree/$file not found\n");
		if(-d "$source_tree/$file") {
			my $pattern = $source_tree;
			$pattern =~ s/\\/\//g;
			$pattern =~ s/\//\[\\\/\\\\\]/g;
			$pattern = '^'.$pattern.'\\/';
			my @add_files =  grep {s/$pattern//} find("$source_tree/$file",'f');
			push @allfiles,@add_files;
			next;
		}
		push @allfiles,$file;
	}
	$DEBUG && print ">>>>".join("\n>>>>",@allfiles)."\n";	
	my $zip = SAPDB::Install::Jar->new();
	my $cwd = cwd();
	unless($^O =~ /mswin/i and $archive =~ /^[a-z]:/i or $archive =~ /^[\/\\]/){
		$archive = "$cwd/$archive";
	}
	
	unless($zip->CreateSelfExtractingArchive($archive,$extractor)){
		die("cannot create zip archive $archive\n");
	}
	
	unless($source_tree eq '.'){
		chdir($source_tree) or die("packzip(): cannot change directory\n");
	}
	
	foreach my $file (@allfiles){
		($zip->Add($file) == 0) or die("packzip(): cannot add file $file\n");
	}
	$zip->CloseArchive();
	chdir($cwd) unless($source_tree eq '.');
	return 1;
			
}



1;






