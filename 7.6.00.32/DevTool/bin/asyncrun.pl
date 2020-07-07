#!/usr/bin/perl
#
#    To use this script for WebDAV tests it has to be copied to 
#    the /usr/pgm directory of the release, that has to be tested
#
#    asyncrun.pl - A script to run commands asynchronously.
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
################################################################################
# Globals:
################################################################################

my $ProgramName = "ASyncRun";
my $Version     = "1";
my $Revision    = "01";
my $Date        = "2004-04-14";

my %Opts;
my @Cmd;

################################################################################
# Main:
################################################################################

while (@ARGV)
{
    my $arg = shift @ARGV;

    if ($arg eq "-help" or $arg eq "-h" or $arg eq "-?")
    {
        Usage();
    }
    elsif ($arg eq "-lock")
    {
        $Opts{'lock'} = shift @ARGV;
    }
    elsif ($arg eq "-wait")
    {
        $Opts{'wait'} = shift @ARGV;
    }
    elsif ($arg eq "-exec")
    {
        $Opts{'exec'} = 1;
    }
    else
    {
        push @Cmd, ($arg);
    }
}

if ($Opts{'exec'})
{
    GoRunner();
}
else
{
    print "\n$ProgramName v$Version.$Revision ($Date), (c)2003 SAP AG\n\n";

    if ($^O =~ /MSWin32/i)
    {
        require Win32::Process;
        import  Win32::Process;

        my $ProcessObj;

        my $cmdline = "perl $0 -exec";

        $cmdline .= " -lock $Opts{'lock'}" if ($Opts{'lock'});
        $cmdline .= " -wait $Opts{'wait'}" if ($Opts{'wait'});

        foreach my $str (@Cmd) { $cmdline .= " $str"; }

        close(STDOUT);
        close(STDERR);

        Win32::Process::Create($ProcessObj, "$^X", $cmdline, 0, 0, ".") or die "FATAL ERROR! Could not start process! Exiting...\n\n";

        exit(0);
    }
    else
    {
        my $pid = fork();

        unless (defined($pid))
        {
            die "FATAL ERROR: Could not start process! Exiting...\n\n";
        }

        if ($pid == 0)
        {
            GoRunner();
        }
        else
        {
	    sleep(3);
            exit(0);
        }
    }
}


################################################################################
# GoRunner() - Runs the process.
################################################################################

sub GoRunner
{
    print "GoRunner started\n";   
    my $rc = 0;

    if ($Opts{'lock'})
    {
        my $time = ($Opts{'wait'} ? $Opts{'wait'} : 0);

        open (HLOCK, "> $Opts{'lock'}") or die "FATAL ERROR! Can't open lock file ($Opts{'lock'})!!\n\n";

        my $lock = flock(HLOCK, 6);

        unless ($lock)
        {
            while ($time > 0)
            {
                sleep (1);
                $time -= 1;
                $lock = flock(HLOCK, 6);
            }
        }

        if ($lock)
        {
            print "AsyncRun: Process ($$) started.\n";
            if (open (LOCKPID, "> $Opts{'lock'}.pid"))
            {
                print LOCKPID "$$";
                close (LOCKPID);
            }
            $rc = (0xffff & system (@Cmd));
            unlink ("$Opts{'lock'}.pid");
        }
        else
        {
            if (open (LOCKPID, "< $Opts{'lock'}.pid"))
            {
                my $other_pid = <LOCKPID>;
                close (LOCKPID);
                print "AsyncRun: Process is already running (PID $other_pid)!\n";
            }
            else
            {
                print "AsyncRun: Process is already running (PID unknown)!\n";
            }
        }

        close (HLOCK);
        unlink ($Opts{'lock'});
    }
    else
    {
        $rc = (0xffff & system (@Cmd));
    }
    
    print "return code of @Cmd\nwas: $rc\n";
    print "shifted rc: ";
    print $rc>>8 ;
    print "\nGoRunner finished\n";
    exit($rc);
}

################################################################################
# Usage() - Prits out usage and exits program.
################################################################################

sub Usage
{
    print <<USAGE_END;

Usage: asyncrun [-lock <LOCK_FILE>] [-wait <SECONDS>] [-(h|help)] cmd line

Where:
        -h or -help or -?
            Displays this help file and ends program execution.

        -lock <LOCK_FILE>
            Uses a lock file to prevent tasks from starting twice.

        -wait <SECONDS>
            When given a lock file, you can set the maximum time to wait
            for the other process to end in seconds. If not given, this
            parameter will be zero, by default.

        cmd line
            Is the command line to be exeuted.

NOTE: You should give any option IN FRONT OF the command line.

USAGE_END

    exit(1);
}
