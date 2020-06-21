#!/usr/bin/perl
#
#    CollectInfo.pm - Handler for collection data.
#
#    @(#)CollectInfo.pm     2005-05-24
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

package SDB::Make::CollectInfo;

require Exporter;
@ISA    = qw(Exporter);
@EXPORT = qw(read_collection_lines get_next_target_info);

use strict;

# default values
my $start_string = "start of vmake collection data";
my $end_string   = "end of vmake collection data";
my @collect_lines = ();


#######################################################################
# read full collection output of a target to an array (collect_lines)
# parameters:
#      version (opt):  fast| quick|slow
#      target  (opt):  collecting target (default all.mac) 
# return 
#      reference to array of collection lines  
########################################################################
sub read_collection_lines
{
	my $target = shift;
	my $options = shift;
	my $found_collinfo = 0;
	my $version = (defined $options->{'config'}) ? $options->{'config'} :  'fast';
	my $cmd = "$ENV{'TOOLSHELL'} -S ";
	my @prelines = ();
	my $errorcount = 0;
	(defined $target) or $target = "all";
	
	@collect_lines = ();
	if (defined $options->{'platform'} && ($options->{'platform'} !~ /^local$/)) 
	{	$cmd .= "remim". substr($options->{'config'},0,1) . ".pl $options->{'platform'}"; }
	else
	{	$cmd .= "lcmake.pl "; }
	$cmd .= " --config=$version -c $target " . '2>&1';
		
	open(IN_COLLECT , "$cmd |" ) || die ( "Can't read collection info\n" );
	while (<IN_COLLECT>)
	{
		if (/$start_string/)
		{	
			$found_collinfo = 1;
			last;
		}
		else
		{
			chomp; 
			push @prelines, $_;
		}
	}
	if (! $found_collinfo)
	{
		print "Error while reading collecting lines:\n";
		print (join "\n", @prelines) . "\n";
		$errorcount++;
	}
	else
	{
		while (<IN_COLLECT>) 
		{
			chomp;
			push @collect_lines, $_;
			/^error collecting/ and $errorcount++;
		}
		close (IN_COLLECT);
		if ($errorcount > 0)
		{
			print "Error while reading collecting lines:\n";
			print (join "\n", @collect_lines) . "\n";
			@collect_lines = ();
		}
	}
		
	return ($errorcount > 0 ? undef : \@collect_lines);
}

#########################################################################################
# shift lines of next target from collection and create a hash of traget infomation 
# parameters:
#      r_lines (opt):  reference to array of collection lines (default \@collect_lines)
# return 
#      reference to the target info (hash)
#########################################################################################
sub get_next_target_info
{
	my $r_lines = shift;
	defined ($r_lines) or $r_lines = \@collect_lines;
	my $current_line;
	while (@$r_lines && $r_lines->[0] =~ /^$/) 
	{ shift @$r_lines; }
	($r_lines->[0] =~ /$end_string/) and return undef; 
	my %collect_info = ();
	$collect_info{collect_type} = shift @$r_lines;
	chomp $collect_info{collect_type};
	$collect_info{collect_type} =~ s/\s*$//;
	while (@$r_lines && ($current_line = shift @$r_lines) !~ /^$/)
	{
		if (($current_line =~ /^\s{8}(\S+)\s+\'(\S+)\'\s*$/)  || ($current_line =~ /^\s{8}(\S+)\s+(\S+)\s*$/))
		{
			$collect_info{$1} = $2;			
		}
		elsif ($current_line =~ /^\s{8}(\S+)\s*$/)
		{
			my $coll_spec = $1;
			while (@$r_lines && $r_lines->[0] =~ /^\s{17}\'(.*)\'\s*$/)	
			{
				my $coll_spec_line = $1;
				if ($coll_spec_line =~ /(.*)\'\s*\'(.*)$/)
				{
					$collect_info{$coll_spec}{$1} = $2;
				}
				else
				{
					$collect_info{$coll_spec}{$coll_spec_line} = "";
				}
				shift @$r_lines;
			}
		}
	}
	return \%collect_info;
}	

1;