#!/usr/bin/perl
#
#    clear_prot - clears all protocol / tmp dirs etc.
#
#    @(#)build_tools.pl     2003-02-24
#
#    UliJ, SAP AG
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

################################################################################
# Includes:
################################################################################

use strict;
use Getopt::Long;

################################################################################
# Globals:
################################################################################

my $ProgramName     = "clear_prot";
my $Version         = "1";
my $Revision        = "01";
my $Date            = "2003-11-24";
my %Opts;

print "$ProgramName v$Version.$Revision ($Date)\n\n";

if (!GetOptions( \%Opts, 'h|?|help', 'own=s', 'prot=i', 'iclear=i', 'v') || @ARGV)
{
    Usage(1);
}

Usage(0) if ($Opts{'h'});

unless ($Opts{'own'})
{
    if ($ENV{'OWN'})
    {
        $Opts{'own'} = $ENV{'OWN'};
    }
    else
    {
        print "ERROR! Cannot determine \$OWN directory!\nExiting...\n";
        exit(2);
    }
}

$Opts{'prot'} = 10 unless (defined $Opts{'prot'});
$Opts{'iclear'} = 1 unless (defined $Opts{'iclear'});

if ($Opts{'iclear'})
{
    print "Running iclear...\n\n";
    local @ARGV;
    do "$ENV{'TOOL'}/bin/iclear.pl";
    print "\n";
}

if ($Opts{'prot'})
{
    my $count = 0;
    print "Removing protocols except latest $Opts{'prot'}...\n\n";
    
    if (open (IN_DIR, "ls -lat $Opts{'own'}/prot/*.prot |"))
    {
        while (<IN_DIR>)
        {
            next if (/^[^-].*/);
            
            if ($Opts{'prot'})
            {
                $Opts{'prot'} -= 1;
                next;
            }
        
		    if (/^.*\s+(\S+)$/)
		    {
		        my $file = $1;
		        if (($file ne ".") && ($file ne ".."))
		        {
		            print "Deleting file '$file'.." if ($Opts{'v'});
		            if (unlink ("$file"))
		            {
		                print "..OK\n" if ($Opts{'v'});
    		            $count ++;
		            }
		            else
		            {
		                print "..FAILED!\n" if ($Opts{'v'});
		            }
		        }
		    }
        }
        close (IN_DIR);
    }
    
    print "\nDeleted $count files.\n\n";
}

exit (0);

sub Usage
{
    my $rc = shift;
    print <<USAGE_END;

Usage:  clear_prot  [-(h|?|help)] [-prot <FILE-COUNT>] [-iclear <0|1>]
                    [-own <OWN-DIR>]

Where:
        -h or -? or -help
            Shows this help an the exits the program.

        -own <OWN-DIR>
            Takes <OWN-DIR> as root directory to look for files to delete.
            If not given, evironment variable \$OWN will be taken.

        -prot <FILE-COUNT>
            Kills all protocols in \$OWN/prot except for the latest <FILE-COUNT>
            files. If this option is not given, the latest 10 protocols will
            be kept.
            NOTE: Value '0' disables protocol deletion.
            
        -iclear <0|1>
            If set to '0', iclear.pl will not be called. Default is '1'.
            
        -v
            Verbose mode.
    

USAGE_END

    exit ($rc);   
}