#!/usr/bin/perl
#
# SDBRemInfo
#
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
#
require Exporter;

@ISA    = qw(Exporter);
@EXPORT = qw(printHostKeyList getAllRemuserHosts);

use strict;

sub printHostKeyList
{
	my $ref_to_REMUSER_HOST = shift;
	my $ref_to_REMUSER_ALIAS = shift;
	my $ref_REMUSER_HOSTMODE = shift;
	my $display_host = shift;
	
	if ($ref_to_REMUSER_ALIAS)
	{
		print STDERR "\nExisting aliases:\n";
		#my $hostline = "";
		foreach (sort keys %$ref_to_REMUSER_ALIAS)
		{
			my $hostline = " $_";
			
			$hostline .= (" " x (15-length($hostline))) . ": " ;
			if (length($ref_to_REMUSER_ALIAS->{$_}) > 45 )
			{
				my @plf_list = split / /, $ref_to_REMUSER_ALIAS->{$_};
				my $lcount = 0;
				foreach (@plf_list)
				{
					if ($lcount+length($_) > 45)
					{
						$hostline .= "\n".(" " x 17);					
						$lcount = 0;					
					}
					$hostline .= "$_ ";
					$lcount += length($_);
				}				
			}
			else
			{
				$hostline .= "$ref_to_REMUSER_ALIAS->{$_}";
			}
			print STDERR "$hostline\n";			
		}	
		print STDERR "\n";		
	}
	print STDERR "\nExisting platforms:\n";
	my $ref_to_pltfrms = getAllRemuserHosts($ref_to_REMUSER_HOST);
	my $pltfrm_string = undef;
	foreach (sort keys %$ref_to_pltfrms)
	{	
		if ($display_host)
		{
			my $hostline = "$_";
			
			if ($ref_REMUSER_HOSTMODE && $ref_REMUSER_HOSTMODE->{$ref_to_pltfrms->{$_}})
			{ $hostline .= " ($ref_REMUSER_HOSTMODE->{$ref_to_pltfrms->{$_}})"; }	
			
			$hostline .= (" " x (22-length($hostline)));
			
			$hostline .= ": $ref_to_pltfrms->{$_}";
			
			$pltfrm_string .= "     $hostline\n";
		}
		else
		{
			if (length ($pltfrm_string) + length ($_) > 70)
			{ 
				print STDERR "$pltfrm_string\n";
				$pltfrm_string = undef;		
			}				
			
			if ( $pltfrm_string )
			{ $pltfrm_string .= ", $_"; }
			else
			{ $pltfrm_string .= "     $_"; }
			if ($ref_REMUSER_HOSTMODE && $ref_REMUSER_HOSTMODE->{$ref_to_pltfrms->{$_}})
			{
				$pltfrm_string .= "($ref_REMUSER_HOSTMODE->{$ref_to_pltfrms->{$_}})";
			}
		}
	}
	print STDERR "$pltfrm_string\n";
}

sub getAllRemuserHosts
{
	my $ref_to_REMUSER_HOST = shift;
	my %hostlist = ();
	my $Release = undef;
	
	if ( defined $ENV{IGME_VERSION} ) 
	{	$Release = substr( $ENV{IGME_VERSION}, 0, 3 );	} 
	else 
	{
		if ( defined($ENV{RELVER})
		  && defined($ENV{CORRECTION_LEVEL}) ) 
		{
			$Release = "$ENV{RELVER}";
			$Release =~ s/R(\d)(\d+)/$1.$2/;			
		}
	}
		
	foreach my $pltfrmkey (sort keys %$ref_to_REMUSER_HOST)
	{
		if ($pltfrmkey =~ /^(.*)_$Release/)
		{
			my $pltfrm = $1;
			if ($ref_to_REMUSER_HOST->{$pltfrmkey} =~ /no_.*supported/ ) 
			{
				delete $hostlist{$pltfrm};
			}
			else
			{
				$hostlist{$pltfrm} = $ref_to_REMUSER_HOST->{$pltfrmkey};	
			}
		}
		elsif ($pltfrmkey =~ /web_(.*)$/)
		{
			next;
			#$pltfrmkey
		}
		# other release
		elsif ($pltfrmkey =~ /_\d\.\d/)
		{
			next;
		}
		else
		{
			unless ($hostlist{$pltfrmkey} ||  ($ref_to_REMUSER_HOST->{$pltfrmkey} =~ /no_.*supported/) )
			{
				$hostlist{$pltfrmkey} = $ref_to_REMUSER_HOST->{$pltfrmkey};	
			}
		}
	}
	return \%hostlist;
}

1;
