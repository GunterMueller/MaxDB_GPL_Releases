#!/usr/bin/perl
# p4replace
# replace options in p4 clients
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

use Carp;

$error_occurred = 0 ;
@CLIENTS=();
open(P4CMD,"p4 clients |");
while(defined($line=<P4CMD>))
{
	if ($line =~ /Client\s+(\S*)\s+/ )
	{
		push @CLIENTS, $1;
	}
	else
	{
		die  "Interpret error in line \"$line\"\n";
	}
}
close(P4CMD);
unless ( $? == 0 )
{
	die "Error while executing \"p4 clients\"\n"."message: $!\n";
}

@LOCKEDCLIENTS=();
open(P4PROT, ">p4replace.prot") or die "can't open p4replace.prot\n"; 
foreach $client (@CLIENTS) 
{
	@LINES=();
	open(P4CMD,"p4 client -o $client |");

    $next_line_is_description = 0;
	$description ="";
	$found_modtime = 0;
	$is_locked = 0;
	$host="";
	while(defined($line=<P4CMD>) && ($found_modtime == 0) )
	{
		if ($next_line_is_description == 1)
		{
			$description = $line;
			$next_line_is_description=0;
			if ( $description =~ /^[\s\t]*(\S.*\S)\s*$/)
			{ $description = $1; }

		}
		if ($line =~ /^Description:\s*$/)
		{
			$next_line_is_description=1;
		}
		if ($line =~ /\slocked\s/)
		{
			$description =~ /^[\s\t]*(\S.*\S)\s*$/;
			print P4PROT "$client on $host is locked\n";
			$is_locked = 1;
		}
		if ($line =~ /^Host:[\s\t]*(\S*)\s*$/)
		{
			$host=$1;
		}

		if ($line =~ /\smodtime\s/)
		{
			$description =~ /^[\s\t]*(\S.*\S)\s*$/;
			print "Client $client ($1) have already set modtime\n";
			print P4PROT "modtime already set for $client on $host\n";
			$found_modtime = 1;
			break;
		}
		$line =~ s/nomodtime/modtime/;
		
		push @LINES, $line;
	}
	close(P4CMD);
	unless ( $? == 0 )
	{
		print P4PROT "Error while executing \"p4 client -o $client\"\n"."message: $!\n";
		print "Error while executing \"p4 client -o $client\"\n"."message: $!\n";
		$error_occurred=1;
	}

	push @LOCKEDCLIENTS, "$client on $host($description)" if ( $is_locked && ! $found_modtime );
	
	next if( $found_modtime || $is_locked );

	open (P4CMD, "| p4 -c $client -H $host client -i");
	foreach $clientline (@LINES) 
	{
		print P4CMD "$clientline";	
	}
	close(P4CMD);
	if ( $? == 0 )
	{
			print P4PROT "$client on $host ok\n";
	}
	else
	{
		die "Error while executing \"p4 client $client\"\n"."message: $!\n";
	}
}

print "locked clients without modtime:\n";
print P4PROT "locked clients without modtime:\n";
foreach $lockedclient (@LOCKEDCLIENTS) 
{
	print "$lockedclient\n";
	print P4PROT "$lockedclient\n";
}

close (P4PROT);

print ("error(s) occurred \n") if ($error_occurred);


