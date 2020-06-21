#!/usr/bin/perl -w
#
# checkmake.pl Script
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

# Nur vorübergehend!!!! BITTE LÖSCHEN!!!!!!
BEGIN {
	unshift @INC, "$ENV{TOOL}/lib/Perl" if ($^O =~ /win32/i);
}


use strict;
use Getopt::Long;
use Make::CheckMake;

select(STDERR); $| = 1;
select(STDOUT); $| = 1;

my $ProgramName     = "Checkmake";
my $Version         = "2.02";
my $VerDate         = "21.09.2004";

my %Opts;
my $checker;

print "\n$ProgramName v$Version ($VerDate), SAP AG\n\n";

Usage() unless (GetOptions( \%Opts, 'qaid=i', 'chlist=i', 'email=s', 'outfile=s'));
unless (@ARGV) {
    print "There is nothing to check...exiting.\n";
    exit(0);
}

$Opts{'release'} = ($ENV{'RELVER'} =~ /R\d+/ ? substr($ENV{'RELVER'}, 1, 2) : ($ENV{'RELVER'} =~ /TOOL/ ? 'TOOL' : $ENV{'RELVER'})) . ($ENV{'RELVER'} =~ /R\d+/ ? ($ENV{'CORRECTION_LEVEL'} =~ /\d\d/ ? $ENV{'CORRECTION_LEVEL'} : "0$ENV{'CORRECTION_LEVEL'}") : ".") . ($ENV{'RELVER'} =~ /R\d+/ ? ($ENV{'LC_STATE'} ? $ENV{'LC_STATE'} : "DEV") : ($ENV{'TOOLREL'} ? $ENV{'TOOLREL'} : "DEV"));

$checker = Make::CheckMake->new(\%Opts, @ARGV);

print "Release                = $Opts{'release'}\n";
print "Changelist             = $Opts{'chlist'}\n" if ($Opts{'chlist'});
print "QADB Id                = $Opts{'qaid'}\n" if ($Opts{'qaid'});
print "Additional output file = $Opts{'outfile'}\n" if ($Opts{'outfile'});
print "Manual email address   = $Opts{'email'}\n" if ($Opts{'email'});
print "\nTargets to scan:\n";
foreach my $file (@{$checker->{'FILES'}}) { print "  $file\n"; }
print "\n";

if ($checker->scanProtocols()) {
    print "\n*** ERRORS FOUND IN PROTOCOLS ***\n\n";
} else {
    print "\nNo errors found in protocols. Exiting...\n\n";
    exit(0);
}

if ($checker->getErrorTargets()) {
    $checker->getResponsibles();
    $checker->uploadErrProt();
    $checker->sendMails();
}

exit(0);

sub Usage {

    print <<HELP_END;

Usage: Checkmake [-qaid <ID>] [-chlist <NUM>] [-email <ADDR>]
                 [-outfile <NAME>] TARGET-LIST
                 
Where:  -qaid <ID>          Is the number of an existing QADB entry.
        -chlist <NUM>       Is the changelist number used for this make.
        -email <ADDR>       Is the email address to send mail to rather
                            than the default list.
        -outfile <NAME>     Is a file name to copy output to.
        
        TARGET-LIST Specifies the protocol files to search through.
    
HELP_END
    exit(-1);
}

