#! /usr/bin/perl
#
#    build_tools.pl - Performs a productive tool make with QADB logging.
#
#    @(#)build_tools.pl     2003-05-19
#
#    U. Jansen, SAP AG
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

use strict;
use imake;

my $CMDString       = "$ENV{'TOOLSHELL'} $ENV{'TOOL'}/bin/build_tools.runner.pl @ARGV";
my $RC              = 0;
my $ConsoleOutput   = 1;
my $FileOutput      = 0;
my $FileName;
my $imf_exit;

select(STDERR);$| = 1;
select(STDOUT);$| = 1;

if ($^O =~ /MSWin32/i)
{
    $CMDString =~ s/\//\\/g;
}

unless (open (CMD_IN, "$CMDString 2>&1 |"))
{
    print STDERR "Can't open '$CMDString' for reading!\n$!\n";

    goto PROGRAM_END;
}

while (<CMD_IN>)
{
    chomp $_;

    if (/^OUTPUT_FILTER:\s+(OPEN|CLOSE)\s+(CONSOLE|FILE)\s*(.*)$/)
    {
        if ($2 eq "CONSOLE")
        {
            $ConsoleOutput = ($1 eq "OPEN" ? 1 : 0);
        }
        else
        {
            if ($1 eq "CLOSE")
            {
                close FILE_OUT or print STDERR "ERROR: Can't close file '$FileName'\n$!\n";
                $FileOutput = 0;
            }
            else
            {
                close FILE_OUT if ($FileOutput);

                $FileName = $3;

                if (open (FILE_OUT, ">$3"))
                {
                    $FileOutput = 1;
                    select(FILE_OUT);   $| = 1;
                    select(STDERR);     $| = 1;
                    select(STDOUT);     $| = 1;
                }
                else
                {
                    print STDERR "ERROR: Can't open $3 for writing!\n$!\n";
                    $FileOutput = 0;
                }
            }
        }
    }
    else
    {
        print STDOUT "$_\n" if ($ConsoleOutput);
        print FILE_OUT "$_\n" if ($FileOutput);
    }
}

PROGRAM_END:

$RC = ($? >> 8);

print "\nRETURN CODE = $RC\n\n";

caller () ? return ( $RC ) : exit ( $RC );

