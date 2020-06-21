#!/devtool/local/bin/perl
# 
# fix_hashbang.pl - for fixing hashbang "on the fly"
#
# @(#)fix_hashbang.pl		2004-01-15
#
#    ========== licence begin  GPL
#    Copyright (C) 2001 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end
## 

use FileHandle;
use strict;

my @filenames=@ARGV;
my $perl_hashbang   = undef;
my $python_hashbang = undef;

foreach my $filename (@filenames)
{
	my $fh = new FileHandle $filename, "r+";
	die "Can't read $filename for fixing hashbash " 
		unless (defined $fh);
	my $firstline = $fh->getline();
	$firstline = ($firstline  =~ /^#!.+perl$/i) ? get_perlhashbang() :  
	             ($firstline  =~ /^#!.+python$/i) ? get_pythonhashbang() : $firstline;
	my @lastlines =	$fh->getlines();
	$fh->close();
	
	my $fh = new FileHandle $filename, "w";
	die "Can't write $filename for fixing hashbash " 
		unless (defined $fh);
	$fh->print($firstline);         
	foreach (@lastlines)
	{
		$fh->print($_);
	}
	$fh->close();
}


sub get_pythonhashbang 
{
	unless (defined $python_hashbang)
	{ 
		chomp (my $env = `which env`);
		$python_hashbang = "#!".$env." python\n";
	}
	return $python_hashbang;
}

sub get_perlhashbang 
{
	my $perlcmd = undef;
	unless (defined $perl_hashbang)
	{
		if (defined $ENV{'TOOLSHELL'})
		{
			$perlcmd = $ENV{'TOOLSHELL'};
		}
		else
		{
			my @perlexe = (
				"/devtool/local/bin/perl",
				"/apo/DevTools/local/bin/perl",
				"/usr/bin/perl",
				"/bin/env perl",
				"/usr/bin/env perl");
			my $major;
			my $minor;
			
			foreach my $cmd (@perlexe) 
			{
				(my $exe, @_) = split ' ', $cmd;
				
				# do they know thie executable?
				next unless ( -x $exe);
				
				# get version of perl by using the switch -v
				open (INPUT, $cmd." -v |");
				while (<INPUT>) {
					chomp;
					last if ($_ =~ /^This is perl, version/g);
					last if ($_ =~ /^This is perl, v\d\.\d\.\d/g);
				}
				close (INPUT);
				if ($_ =~ /^This is perl, version (\d+)\.(\d+)_(\d+)/) {
					$major = $1;	
					$minor = $2.$3;
				} elsif ($_ =~ /^This is perl, v(\d+)\.(\d+).(\d+)/) {
					$major = $1;	
					$minor = $2.$3;
				} elsif ($_ =~ /^This is perl, version (\d+)\.(\d+)/) {
					$major = $1;	
					$minor = $2;
				}
				
				# only use perl with major version >= 5
				next if ($major < 5);
				$perlcmd = $cmd;
				last;
			}
		}
		$perl_hashbang = "#!".$perlcmd."\n";
	}
			
	# tell them the hashbang line
	return $perl_hashbang;
}
