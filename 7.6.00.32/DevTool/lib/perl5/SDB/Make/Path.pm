#!/devtool/local/bin/perl
#
#    SDB::Make::Path.pm - some function to get right pathes in make environment
#    
#    @(#)SDB::Make::Path.pm     2004-01-18
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

package SDB::Make::Path;

use Cwd;
use File::Basename;
use strict;

my %extra_hash;

######
sub GetQualifiedTargetName
{
	my $original_filename = shift;	
	# looks like a qualified target
	($original_filename =~ /^:/) and return $original_filename;
	my $qualified_filename = undef;
	my $variant_string = "";
	my ($filename, $option_string) = CutOptionString($original_filename);
	($filename, $variant_string) = CutVariantString($filename);
	$filename =~ s/\\/\//g;
	# not found
	if (-e $filename)
	{
		(my $cwd = cwd()) =~ s/\\/\//g;
		(my $own = $ENV{'OWN'}) =~ s/\\/\//g;
		my $full_path = ($filename =~ /^$own/i) ? "$filename" : "$cwd/$filename";
		$full_path = NormalizePath($full_path);
		my $basename = "";
		my $layer = "";
		my $prefix = "";
		my $extension = "";
		($basename, $full_path, $extension) = fileparse($full_path, '\..*');
		if ($full_path =~ /^$own\/sys\/(desc|src)\/(.*)$/i)
		{
			my $area = $1;
			($2) and $layer = $2;
				
			if ($layer =~ /\//)
			{	
				$prefix = ( $area =~ /desc/ ) ? "::" : ":"; 
				if ( $area =~ /src/ )
				{
					# no check of extra file for standard extensions and descriptions
					if ( $extension =~ /^.(mac|shm|lnk|dld|shr|rel|lib|jpr)$/)
					{	$prefix ="::";	}
					elsif ( $extension !~ /^.(cpp|c|h|hpp)$/i )
					{
						my $Extra = GetExtraEntries();
						my $last_ext = $extension;
						$last_ext =~ s/^.*\.([^\.]*)/$1/;
						if (defined ${$Extra}{$last_ext} && $layer eq "${$Extra}{$last_ext}/" )
						{
							# oh,oh found an old extar file (delete it ? ;-)
							$layer = "";
							$prefix=""; 
						}
					}	
				}
			}
			$qualified_filename = "$prefix$layer$basename$variant_string$extension$option_string";
		}	
	}
			
	return (defined $qualified_filename) ? $qualified_filename : $original_filename; 
}	
	
# remove ".." and "." from path
sub NormalizePath
{
	my $path = shift;
	# remove "/./"
	my @subdirs = split /\//, $path;
	my @new_subdirs = ();
	
	foreach (@subdirs)
	{
		/^\.$/ and next;
		if (/^\.\.$/)
		{	pop @new_subdirs;	}
		else
		{	push @new_subdirs, $_; }
	}
	$path = join "/", @new_subdirs;
	
	return $path;
}

####################
# i hate extra files
####################
sub GetExtraEntries
{
	if (! scalar(keys(%extra_hash)))
	{	
		if ( open(EXTRA, "<$ENV{'OWN'}/sys/desc/Extra") )
	    {
		    while(<EXTRA>)
	        {
			    if ( /^\.(\w+)\s+(\w+)\s+/ )
	            {
	            	$extra_hash{$1} = $2;
			    }
		    }
		    close(EXTRA);
	    }
	}
    return \%extra_hash;
}

####
sub CutVariantString
{
	my $fullname = shift;
	my $variant_string = "";
	if ($fullname =~ /[\+-][^\\\/]*$/)
	{
		
		my ($base, $path, $ext) = fileparse($fullname, '\..*');
		if ($base =~ /^([^-\+]*)([-\+].*)$/)
		{
			$base = $1;
			$variant_string = $2;
		}
		$fullname = "$path$base$ext";
	}
	return ($fullname, $variant_string);
}

####
sub CutOptionString
{
	my $fullname = shift;
	my $option_string = "";
	if ($fullname =~ /^(.*)(\/[\/-].*)$/)
	{
		$fullname = $1;
		$option_string = $2;
	}
	return ($fullname, $option_string);
}

1;