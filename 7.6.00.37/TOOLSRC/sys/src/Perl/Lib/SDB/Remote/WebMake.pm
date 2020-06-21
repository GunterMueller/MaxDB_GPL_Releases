#!/usr/bin/perl
#
#    WebMake.pm - Handler for web requests 
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

package SDB::Remote::WebMake;

require Exporter;
@ISA    = qw(Exporter);
@EXPORT = qw(open_connection close_connection run_make get_next_line convert_to_httpstring add_to_all_adresses show_all_adresses convert_to_httpstring);

use Cwd;
#use strict;
use Net::Telnet;

my $all_addresses;



sub open_connection
{
	my $host = shift;
	my $cmd = shift;
	my $options = shift;
	my $port = (defined $options->{'port'}) ? int($options->{'port'}) : 1080;
	my $timeout = (defined $options->{'opentimeout'}) ? int($options->{'opentimeout'}) : 100000;
	my $webobj = new Net::Telnet (Telnetmode => 0);
	$webobj->open(Host => $host,
	Timeout => 100000,
	Port => 1080);
	$webobj->print("GET $cmd&mode=telnet");
	return $webobj;
}

sub close_connection
{
	my $webobj = shift;
	$webobj->close();
}

sub get_next_line
{
	my $webobj = shift;
	my $options = shift;
	my $retries = (defined $options->{'retries'}) ? int($options->{'retries'}) + 1 : 6;
	my $timeout = (defined $options->{'readtimeout'}) ? int($options->{'readtimeout'}) : 600;
	my $line = undef;
	
	while ($retries > 0)
	{
		eval ('$line =$webobj->getline (Timeout => $timeout)');
		(defined $line) and	last;
		$retries--;
		sleep (1);
		if (! defined $options->{'nowarn'}) 
		{
			if ($retries > 0)
			{	print "Info: timeout while reading - left retries: $retries\n";}
			else
			{	print "Error: timeout while reading (server connection broken?)\n";}
		}
	}
	return $line;
}

sub run_make
{
	my $host = shift;
	my $cmd = shift;
	my $options = shift;
	my $line = "";
	my $rc = -1;
	
	my $webobj = &open_connection($host, $cmd, $options);
	if ($webobj)
	{		
		# look for start tag 
		while ( defined ($line) &&  $line !~ /Remcall was started/i)
		{	
			$line = &get_next_line($webobj, $options);	
		}
		if (defined ($line))
		{
			$line ="";
			while ( defined ($line) &&  $line !~ /Remcall was finished/i)
			{
				print ("$line");				
				$line = &get_next_line($webobj, $options);	
			}
		}
		(defined ($line)) and $rc = 0;
	}
	else
	{
		print "Error: Can't open connection to host '$host'\n";
	}
	return ($rc);
}

sub convert_to_httpstring
{
	my $string = shift;
	my $http_string = $string ;
	$http_string =~ s/ /%20/g;
	$http_string =~ s/&/%26/g;
	$http_string =~ s/;/%3B/g;
	$http_string =~ s/=/%3D/g;
	$http_string =~ s/\+/%2B/g;
	return ($http_string);
}

sub add_to_all_adresses
{
	my $text = shift;
	#print "Webmake: add '$text'\n";
	$all_addresses .= " \"$text\"";
	#print "Webmake all_adresses:  $all_addresses\n";
}

sub show_all_adresses
{
	if ($all_addresses ne "")
	{
		#print "Webmake call:\n   \"C:\\Program Files\\Mozilla Firefox\\firefox.exe\" $all_addresses\n";
		my $browser = $ENV{REM_DEFAULT_BROWSER};
		if (($browser =~ / /) && ($browser !~ /^".*"$/))
		{ $browser = "\"$browser\""; }
			
		my $command = "$browser $all_addresses";
		#print "Webmake call:\n$command\n";
		system ("$command");
		#system ("\"C:\\Program Files\\Mozilla Firefox\\firefox.exe\" $all_addresses");
	}
}

1;