#! /usr/bin/perl
#
#    SDB::Protocol::Tools.pm - some helpful functions for protcol handling
#    
#    @(#)SDB::Protocol::Tools.pm     2004-01-13
#
#    &gar, SAP AG
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

package SDB::Protocol::Tools;

use Cwd;
use strict;

######

sub PackScalar
{
	my $text = shift;
	my $file_name = shift;
	my $options = shift;
	my $extension = "zip";
	
	# don't copy scalar to convert 
	my $r_text = ref ($text) ? $text : \$text;
	( defined $file_name ) or $file_name = "protocol.prot";
	my $tmp_file = defined $ENV{'TMP'} ? $ENV{'TMP'} : (defined $ENV{'TEMP'} ? $ENV{'TEMP'} : ".");  
	my $zip_mode;
	my $zipped_text = "";
	
	if (ref ($options))
	{
		(defined ${$options}{'extension'}) and	$extension = ${$options}{'extension'};
		(defined ${$options}{'toDOS'}) and	ScalarToDos($r_text);
	}
	
	$tmp_file .= "/$file_name"; 
	
	open( TMP_FILE , ">$tmp_file" ) or last;
	print TMP_FILE  $$r_text;
	close (TMP_FILE);
	
	
	my $zipped_tmp_file = "${tmp_file}.zip";
	$zip_mode = 2;
	
	while ( $zip_mode == 2)
	{
		# write to a file
		$zip_mode = 0;
		open( TMP_FILE , ">$tmp_file" ) or last;
		binmode TMP_FILE;
		print TMP_FILE  $$r_text;
		close (TMP_FILE);
				
		# pack file
		( PackFile($tmp_file, $zipped_tmp_file) == 0) or last;
		
		# read zip file
		( open (ZIPFILE, $zipped_tmp_file)) or last;
		binmode ZIPFILE;
	    while(<ZIPFILE>) { $zipped_text .= $_; }
	    close ZIPFILE;
	    $$r_text = $zipped_text;
	    $zip_mode = 1;
	}
	unlink "$zipped_tmp_file", "$tmp_file";
	
	return (($text,$zip_mode));	
}

####

sub ScalarToDos
{
	my $text = shift;
	my $r_text = (ref ($text)) ? $text : \$text;
	#$$r_text =~ s/^\n/\015\012/g;
	$$r_text =~ s/^\012/\015\012/g;
	$$r_text =~ s/([^\015])\012/$1\015\012/g;
	$$r_text =~ s/\012\012/\012\015\012/g;
	return $text;	
}


##############

sub PackFile{

	my ($source, $target) = @_;
	require SAPDB::Install::Jar;
	import SAPDB::Install::Jar;
	
	
	(defined $target) or $target = "${source}.tgz";
	
	my $cwd = cwd();
	my $source_dir = ".";
	
	unless (-f $source)
	{
		print STDERR ("packfile: file $source not found\n");
		return -1;
	}
	
	if ($source =~ /^(.*)[\/\\]([^\/\\]*)$/)
	{
		$source_dir = $1;
		$source = $2;		
	}
	
	my $zip = SAPDB::Install::Jar->new();
	unless ($zip->OpenArchive($target,'w') == 0)
	{
		print STDERR ("cannot create zip archive $target\n");
		return -2;
	}	
	
	unless($source_dir =~ /^\.$/)
	{
		unless (chdir($source_dir))
		{
			print STDERR ("packfile: cannot change directory\n");
			return -3;
		}
	}
	unless ($zip->Add($source) == 0)
	{
	 	print STDERR ("packfile: cannot add file $source\n");
	 	$zip->CloseArchive();
	 	return -4;;
	}
	$zip->CloseArchive();
	chdir($cwd) if ($source_dir !~ /^\.$/);
	return (0);
}


####################

sub FileToDOS
{
	my $file = shift;
	my $tmpfile = "$file.u2d";

	$SIG{'INT'} = 'IGNORE';

	if ( -f $file && -T $file) 
	{
		unless (open(FILE_IN, "<$file") )
		{	
			print STDERR ("file_u2d:Can't open $file\n");
			return -1;
		}		
		
		binmode FILE_IN;
		unless (open(FILE_OUT, "+>$tmpfile"))
		{	
			print STDERR ("file_u2d: Can't open $tmpfile\n");
			return (-2);
		}
		binmode FILE_OUT;
		my($char1, $char2);
		while(<FILE_IN>) {
			$char1 = chop; $char2 = chop;
			if    ( ord($char1) != 10 ) { 
				$_ .= $char2.$char1;	    } 
			elsif ( ord($char2) == 13 ) { 
				$_ .= chr(13).chr(10)       }
			else {
				$_ .= $char2.chr(13).chr(10) }
			print FILE_OUT $_;
		}
		close FILE_IN; close FILE_OUT;
		unlink $file; 
		rename $tmpfile,$file; 
	}
	else { 
		print STDERR ("file_u2d: $file seems to be a binary or doesn't exist!\n");
		return (-3);
	}
	return (0);	
}

1;