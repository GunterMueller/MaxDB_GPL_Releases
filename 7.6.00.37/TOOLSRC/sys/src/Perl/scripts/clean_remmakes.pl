#!/usr/bin/perl
#    clean_remmakes.pl - script for cleaning remote makes areas
#
#    @(#)clean_remmakes.pl 2003-12-09
#    GAR, SAP AG
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

use strict;
use Getopt::Long;

my $remroot = "/rel";
my $days = 30;
my %Options;
my $sendmail = undef;
my $check = undef;
my $debug = undef;
my $outdate =undef;
my $profilename = ".iprofile";
my $tool = $ENV{'TOOL'} ? $ENV{'TOOL'} : "/rel/TOOL/tool";


$profilename = "iprofile.bat" if ($^O =~ /mswin/i);

if (!Getopt::Long::GetOptions( \%Options, 'h|?', 'path=s', 'tool=s','days=i', 'mail', 'debug', 'check') || $Options{'h'} || @ARGV )
{
	print "\nclean_remmakes.pl [-h|-?] [-check] [-mail] [-days <dd>]\n";
	print "                  [-path <directory>] [-tool <direcory>]\n"; 
    print "Where:\n";
    print "     -check\n";
	print "          list all directories to delete (no execute)\n";
    print "     -mail\n";
	print "          send a mail to the users of old directories\n";
	print "     -days <dd>\n";
	print "          go <dd> days back (default is $days)\n";
	print "     -path <direcory>\n";
	print "          remote make directory (default is '/rel')\n";
	print "     -tool <direcory>\n";
	print "          tooldir to find perllib for sendmail (default is '$tool')\n";
	exit;
}

$days     = $Options{'days'} if (defined $Options{'days'});
$remroot  = $Options{'path'} if (defined $Options{'path'});
$tool     = $Options{'tool'} if (defined $Options{'tool'});
$sendmail = 1 if (defined $Options{'mail'});
$check    = 1 if (defined $Options{'check'});
$debug    = 1 if (defined $Options{'debug'});

-d $remroot or die "$remroot not found!\n";

opendir(REMROOT,$remroot) || die "Can't open $remroot for reading\n";
my @remrootfiles = readdir(REMROOT);
closedir(REMROOT);

my @remuserdirs = grep { -d "$remroot/$_" && /[Dd]\d{6}$/} @remrootfiles;
$outdate = compute_outdate ("remote makes", $days,1);

my @dirs_to_delete = ();

# look for date of iprofile and $WRK
# look for date of iprofile and $WRK
foreach my $remuserdir (@remuserdirs)
{
    my %dirdata;
    $dirdata{'name'}=$remuserdir;
    $dirdata{'date'} = 0;
    if ( -f "$remroot/$remuserdir/$profilename" )
    {
        $dirdata{'date'} = (stat("$remroot/$remuserdir/$profilename"))[9];
    }
    if ( (-d "$remroot/$remuserdir/sys/wrk") &&
        ((stat("$remroot/$remuserdir/sys/wrk"))[9] >  $dirdata{'date'} ))
    {
            $dirdata{'date'} = (stat("$remroot/$remuserdir/sys/wrk"))[9];
    }
    push @dirs_to_delete, \%dirdata if ( $dirdata{'date'} <  $outdate );
}


print "Directories to delete:\n";
my $diskspace = 0;
my $count     = 0;
my %users_to_mail;
my $errors = 0;
foreach (@dirs_to_delete)
{
	my %dirdata = %$_;
	$count++;
	if ($check)
	{
		if ($^O =~ /mswin/i)
		{
			tr /\//\\/;
			print ("To delete $dirdata{'name'}\n");
		}
		else
		{
			my $output=`du -sk $remroot/$dirdata{'name'}`;
			chomp $output;
			if ($output =~ /^(\d*)\s*(.*)$/)
			{
				print ("To delete $2($1)\n");
				$diskspace+=$1;
			}
			$dirdata{'space'} = $1 + 0;
		}
	}
	else
	{
		print ("Delete $remroot/$dirdata{'name'}\n");
		if ($^O =~ /mswin/i)
		{
			tr /\//\\/;
			system ("rd /Q /S $remroot\\$dirdata{'name'}") == 0 or $errors++;
		}
		else
		{
			system ("rm -rf $remroot/$dirdata{'name'}") == 0 or $errors++;
		}
	}
	if ( $sendmail && ($dirdata{'name'} =~ /.*([Dd]\d{6})$/))
	{
		my $duser = $1; 
		if ( defined $users_to_mail{$duser} )
		{	$users_to_mail{$duser} .= "\n$dirdata{'name'}";		}
		else
		{	$users_to_mail{$duser} .= "$dirdata{'name'}"; }
	}
}

# mail
if ( $sendmail )
{
	eval "require Mail::Sendmail";
	if ($@)
	{
		push @INC, "$tool/lib/perl5";
		eval "require Mail::Sendmail";
		if ($@)
		{
			die "Can't find perllib for sendmail ($@)\n" ;
		}
	}
	import Mail::Sendmail;
	my %mail;
	my $hostname =  `hostname`; 
	$hostname =~ s/\n$//;
	foreach (keys %users_to_mail)
	{
		my $receiver = $_;
		my $sub = "Old remote make directories found ($hostname)";
		my $msg = "Hello $receiver,\nI ". 
		          ($check ?  "found": "have deleted").
		          " following old make directories on $hostname:\n".
		          $users_to_mail{$receiver}."\n\n";
		$msg .=  "The directories will be deleted in the next time!\n" if ($check);		          
		$msg .=  "The current outdate ".($check ?  "is ": "was "). gmtime ($outdate). ".\n";
		
		$msg .= "\nHave a nice day!\nThe automatic cleaner";
		
		$receiver.="\@exchange.sap.corp";
		
		%mail = ( To      => "$receiver",
		      From    => "remcleaner\@sap.corp",
              Message => "$msg",
              Smtp    => 'mail.sap.corp',
              Subject => "$sub" );

		print "Send info mail to $receiver\n";
		
		if ( ! sendmail(%mail) ) {
		   print "FATAL: sendmail error $Mail::Sendmail::error\n";
		}
	}
}

if ( $check )
{
	if ($^O =~ /mswin/i)
	{
		print "$count directories to delete\n";
	}
	else
	{
		print "More devspace after delete: $diskspace KB in $count directories\n";
	}
}

sub compute_outdate
{
	my ($description, $days, $output) = @_;
	my $outdate = 0;
	$outdate = time () - ($days * 3600 * 24) if ($days >0);
	if ($output)
	{
		if ( $outdate > 0 )
		{	print "outdate ( $description ) = ". gmtime ($outdate). "\n"; }
		else
		{	print "$description ignored\n"; }
	}
	return $outdate;
}
