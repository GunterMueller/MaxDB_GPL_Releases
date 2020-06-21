#!/usr/bin/perl
#
#    OptimizationInfo.pm - Handler for optimization data.
#
#    @(#)OptimizationInfo.pm     2005-05-24
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

package SDB::Make::OptimizationInfo;

require Exporter;
@ISA    = qw(Exporter);
@EXPORT = qw(get_modified_optimization get_all_optimization);

use strict;
use SDB::Make::CollectInfo;

#######################################################################
# print all modules withmodified optimization 
# parameters:
#      version (opt):  fast| quick|slow
#      target  (opt):  collecting target (default all.mac) 
# return 
#      count of modified targets  
#              -1 if collection failed
########################################################################

sub get_modified_optimization
{
	my $target = shift;
	my $options = shift;
		
	my $opt_string = (defined $options->{'optstring'}) ? $options->{'optstring'} :  '-NO|-OPT|-OPTSUB|-OPTADD';
	my $r_targetinfo ;
	my $count = 0;
	my %OptHash = ();
	
	(defined $target) or $target = "all";
		
	if (defined SDB::Make::CollectInfo::read_collection_lines($target, $options))
	{
		while (defined ($r_targetinfo = SDB::Make::CollectInfo::get_next_target_info()))
		{
			
			if (($r_targetinfo->{'collect_type'} =~ /module/) &&  defined $r_targetinfo->{descriptions})
			{
				my @found = grep {/^$opt_string$/} keys %{$r_targetinfo->{descriptions}};
				if (@found)
				{
					my $OptHashKey = (length ($r_targetinfo->{layer}) > 0 ? "$r_targetinfo->{layer}/" : "")."$r_targetinfo->{name}";
					$OptHash{$OptHashKey} = "";
					my $output_string = "$OptHashKey: ";
					foreach (@found)
					{
						$OptHash{$OptHashKey} .= "$_$r_targetinfo->{descriptions}->{$_} ";
					}
					$options->{'debug'} and print ("$OptHashKey: $OptHash{$OptHashKey}\n");
					$count++;
				}			
			}
		}
	}
	else
	{ $count = -1 ; }
	return wantarray ? ($count, \%OptHash) : $count; 
}
	
1;