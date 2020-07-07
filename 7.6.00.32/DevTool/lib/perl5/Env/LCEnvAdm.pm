#! /usr/bin/perl
#
#    LCEnv.pm - Creates and syncs a complete SAPDB LC make environment.
#
#    @(#)LCEnv     2003-06-17
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
##############################################################################
# Static includes:

require Exporter;

@ISA        = qw(Exporter);
@EXPORT_OK  = qw(LCEAdmLoadProfiles LCEAdmGetProfilePath LCEAdmGetState LCEAdmCheckLock LCEAdmReadFileLine LCEAdmGetActProt LCEAdmLockRel LCEAdmUnlockRel LCEAdmKill LCEAdmKillVMake LCEAdmShowRels LCEAdmShowProt LCEAdmTailProt LCEAdmShowInfo);

use strict;
use IO::SimpleCfg;
use IO::FilePath;
use Tool::HashRef;
use File::Basename;

$= = (($^O =~ /MSWin32/i) ? 25 : 35);
1;

sub LCEAdmLoadProfiles
{
    my ($Path, @Profiles) = @_;
    my $href = {};
    
    unless ($Path)
    {
        $Path = LCEAdmGetProfilePath();
    }
    
    unless (-e $Path)
    {
        print "ERROR: Profile path '$Path' does not exist!\n";
        return undef;
    }
    
    unless (@Profiles)
    {
        if (opendir (DIR, "$Path"))
        {
            @Profiles = grep {/.*\.cfg$/} readdir(DIR);
            closedir(DIR);
        }
    }
    
    foreach my $prof (@Profiles)
    {
        my $profile = basename($prof, '.cfg');
        unless (-e "$Path/$profile.cfg")
        {
            print "Error: Profile '$Path/$profile.cfg' not found!\n";
            next;
        }
        $href->{$profile} = LoadSimpleCfg("$Path/$profile.cfg");
        $href->{$profile}->{'CFG_FILE'} = FixPathSep("$Path/$profile.cfg");
        LCEAdmGetState($href->{$profile});
    }
    
    return $href;
}

sub LCEAdmGetProfilePath
{
    my ($ToolDir) = @_;
    
    $ToolDir = $ENV{'TOOL'} unless ($ToolDir);
    
    my $defcfg = {};
    
    if (-e "$ToolDir/config/LCEnv.default")
    {
        $defcfg = LoadSimpleCfg("$ToolDir/config/LCEnv.default");
    }
    elsif (-e "$ToolDir/data/LCEnv.default")
    {
        $defcfg = LoadSimpleCfg("$ToolDir/data/LCEnv.default");
    }
    else
    {
        print "ERROR: Could not load default config!\n";
        return undef;
    }
    
    return ($defcfg->{'GLOBAL'}->{'CFG_PATH'} ? $defcfg->{'GLOBAL'}->{'CFG_PATH'} : $ENV{'OWN'});
}

sub LCEAdmGetState
{
    my $href = shift;
    
    _fixzerofile("$href->{'RELEASE'}->{'OWN'}/sys/wrk/vmake.pid");
    
    if ($href->{'GLOBAL'}->{'LOCKFILE'})
    {
        if (-e "$href->{'GLOBAL'}->{'LOCKFILE'}.blocked")
        {
            $href->{'STATE'}->{'RUN_STATE'} = "disabled";
        }
        else
        {
	        if (-e "$href->{'GLOBAL'}->{'LOCKFILE'}.lock")
	        {
	            $href->{'STATE'}->{'PID'} = _GetPID("$href->{'GLOBAL'}->{'LOCKFILE'}.pid");
		        if (LCEAdmCheckLock("$href->{'GLOBAL'}->{'LOCKFILE'}.lock"))
		        {
		            $href->{'STATE'}->{'RUN_STATE'} = (LCEAdmReadFileLine("$href->{'GLOBAL'}->{'LOCKFILE'}.state") or "running");
		        }
		        else
		        {
		            $href->{'STATE'}->{'RUN_STATE'} = "ZOMBIE";
		        }                
	        }
	        else
	        {
	            $href->{'STATE'}->{'RUN_STATE'} = "idle";            
	        }
        }
    }
    else
    {
        if (-e "$href->{'RELEASE'}->{'OWN'}/sys/wrk/vmake.pid")
        {
	        if (LCEAdmCheckLock("$href->{'RELEASE'}->{'OWN'}/sys/wrk/vmake.pid"))
	        {
	            $href->{'STATE'}->{'RUN_STATE'} = "running vmake";
	        }
	        else
	        {
	            $href->{'STATE'}->{'RUN_STATE'} = "ZOMBIE";
	        }
        }
        else
        {
	        $href->{'STATE'}->{'RUN_STATE'} = "idle";            
        }
    }
    
    my $runcmd = "$href->{'RELEASE'}->{'OWN'}" . ($^O =~ /Win32/ ? "\\irun.bat" : "/irun.sh");
    if (-e "$runcmd")
    {
	    my @run_stat = stat($runcmd);
	    my @loc = localtime $run_stat[9];
	    
	    $href->{'STATE'}->{'LAST_RUN'} = sprintf ("%02d:%02d %02d.%02d.%04d", $loc[2], $loc[1], $loc[3], $loc[4]+1, $loc[5]+1900);
    }
    else
    {
        $href->{'STATE'}->{'LAST_RUN'} = "never";
    }    
    
    return $href;
}

sub _fixzerofile
{
    my $fname = shift;
    
    if (-e "$fname" && -z "$fname")
    {
        unlink ("$fname");
    }
}

sub LCEAdmCheckLock
{
    my $lockfile = shift;
    
    return 0 unless (-e "$lockfile");

    if (open (IN_FILE, "< $lockfile"))
    {
        unless (flock(IN_FILE, 6))
        {
            close (IN_FILE);
            return 1;
        }
        
        close (IN_FILE);
    }
    return 0;    
}

sub LCEAdmReadFileLine
{
    my $lockfile = shift;
    my $outline = "";
    if (open (IN_FILE, "< $lockfile"))
    {
        while (<IN_FILE>)
        {
            if (/^^(.*)$/)
            {
                $outline = $1;
            }
        }
        
        close (IN_FILE);
    }
    return $outline;
}

sub LCEAdmGetActProt
{
    my ($dir,$mask) = @_;
    my $protocol;
    
    opendir (IN_DIR, "$dir");
    my @prots = grep { /$mask/ } readdir(IN_DIR);
    closedir(IN_DIR);
    my ($latest, $ltime) = ("",0);
    foreach my $prot (@prots)
    {
        my @fstats = stat("$dir/$prot");

        if ($fstats[9] > $ltime)
        {
            $ltime = $fstats[9];
            $latest = $prot;
        }    
    }
    return undef unless ($latest);
    return FixPathSep("$dir/$latest");
}

sub LCEAdmLockRel
{
    my $href = shift;
    
    return 0 unless ($href->{'GLOBAL'}->{'LOCKFILE'});
    open (OUT_FILE, ">$href->{'GLOBAL'}->{'LOCKFILE'}.blocked");
    print OUT_FILE "Disabled at: " . (scalar localtime) . "\n";
    close (OUT_FILE);
    return (-e "$href->{'GLOBAL'}->{'LOCKFILE'}.blocked" ? 1 : 0);
}

sub LCEAdmUnlockRel
{
    my $href = shift;
    
    return 0 unless ($href->{'GLOBAL'}->{'LOCKFILE'});
    unlink ("$href->{'GLOBAL'}->{'LOCKFILE'}.blocked");
    return (-e "$href->{'GLOBAL'}->{'LOCKFILE'}.blocked" ? 0 : 1);
}

sub LCEAdmKill
{
    my $href = shift;
    
    if ($href->{'GLOBAL'}->{'PROD_MAKE'})
    {
        print "Terminating 'NACHT.ALL'..";
        if (_WaitCondition("-e '$href->{'RELEASE'}->{'OWN'}/prot/build.pid'", 25, 1))
        {
            LCELockKill("", "$href->{'RELEASE'}->{'OWN'}/prot/build.pid");
            unlink ("$href->{'RELEASE'}->{'OWN'}/prot/build.pid");
        }
        else
        {
            print "..NOT FOUND!\n";
        }
        
        LCEAdmKillVMake($href);
        
        if (-e "$href->{'RELEASE'}->{'OWN'}/irun.pid")
        {
            print "Waiting for outer script to terminate..";
            if (_WaitCondition("! -e '$href->{'RELEASE'}->{'OWN'}/irun.pid'", 10, 1))
            {
                print "..OK\n";
            }
            else
            {
                print "..seems to hang!\n";
                LCELockKill("irun.sh", "$href->{'RELEASE'}->{'OWN'}/irun.pid");
                unlink ("$href->{'RELEASE'}->{'OWN'}/irun.pid");
            }
        }
    }
    else
    {
        LCEAdmKillVMake($href);
    }
    
    if (-e "$href->{'GLOBAL'}->{'LOCKFILE'}.pid")
    {
        print "Waiting for 'LCEnv' to terminate..";
        if (_WaitCondition("! -e '$href->{'GLOBAL'}->{'LOCKFILE'}.pid'", 60, 1))
        {
            print "..OK\n";
        }
        else
        {
            print "..seems to hang.\nTerminating 'LCEnv'..";
            LCELockKill('LCEnv', "$href->{'GLOBAL'}->{'LOCKFILE'}.pid");
        }
    }
    unlink ("$href->{'GLOBAL'}->{'LOCKFILE'}.pid") if (-e "$href->{'GLOBAL'}->{'LOCKFILE'}.pid");
    if (-e "$href->{'GLOBAL'}->{'LOCKFILE'}.lock") {  unlink ("$href->{'GLOBAL'}->{'LOCKFILE'}.lock"); }
    if (-e "$href->{'GLOBAL'}->{'LOCKFILE'}.state") { unlink ("$href->{'GLOBAL'}->{'LOCKFILE'}.state");}
}

sub LCELockKill {
    my ($title, $file, $is_pidfile) = @_;
    my $ok = 0;
    
    print "Terminating '$title'.." if ($title ne "");
    if (system("$ENV{'TOOLSHELL'} $ENV{'TOOL'}/bin/killpstree.pl -quiet -lockfile $file") == 0) {
        print "..Locks Killed";
        $ok = 1;
    } else { print "..Lock kill failed"; }
    if ($ok && !$is_pidfile) {
        print "\n";
        return 1;
    } else { print ".."; }
    
    my $pid = 0;
    if (open(F_IN, "<$file")) {
        while(<F_IN>) { chomp($pid = $_); }
        close (F_IN);
    }
    if ($pid) {
        if (system("$ENV{'TOOLSHELL'} $ENV{'TOOL'}/bin/killpstree.pl -quiet $pid") == 0) {
            print "..pstree killed\n";
            return 1;
        } else {
            print "..killing pstree failed..";
            if ($^O =~ /win32/i) {
                if (kill(9, $pid)) { print "..'kill' OK\n"; return 1; } else { print "..'kill' FAILED!\n"; }
            } else {
                $ok = 0;
                if (open (CMD, "kill -9 $pid 2>&1 |")) {
                    while (<CMD>) {
                        if (/no\ssuch\sprocess/) {
                            $ok = 1;
                        }
                    }
                    close (CMD);
                } 
                if ($ok) { print "..OK\n"; return 1; }
                else {
                    $ok = $? >> 8;
                    if($ok != 0) { print "..'kill' FAILED!\n"; }
                    else {
                        print "..'kill' OK\n";
                        return 1;
                    }
                }
            }
        }
    } else { print "..FAILED: No PID found!\n"; }
    return 0;
}
sub LCEAdmKillVMake
{
    my $href = shift;
    my $first = 1;
    
    while (-e "$href->{'RELEASE'}->{'OWN'}/sys/wrk/vmake.pid")
    {
        print "..found!\n" unless ($first);
        $first = 0;
        LCELockKill("vmake", "$href->{'RELEASE'}->{'OWN'}/sys/wrk/vmake.pid");
        sleep(1);
        unless (unlink ("$href->{'RELEASE'}->{'OWN'}/sys/wrk/vmake.pid")) {
            print "WARNING: vmake.pid could not be removed!\n";
            $first = 1;
            last;
        }
        print "Waiting for any 'respawning' vmake..";
        sleep(2);        
    }
    print "..none found.\n" unless ($first);
    return (1);
}

sub LCEAdmShowRels
{
    my $href = shift;

    print "Process overview:\n\n";
    
    print "Cfg Name     State        PID     Last Run            Own\n";
    print "------------------------------------------------------------------\n";
    
    foreach my $key (keys %{$href})
    {
        print pack ('A13 A13 A8 A20 A*', ($key, $href->{$key}->{'STATE'}->{'RUN_STATE'}, $href->{$key}->{'STATE'}->{'PID'}, $href->{$key}->{'STATE'}->{'LAST_RUN'}, $href->{$key}->{'RELEASE'}->{'OWN'})) . "\n";
    }
    print "\n";    
}

sub LCEAdmShowProt
{
    my $protfile = shift;
    
    print "Showing protocol '$protfile'.\nPress <CTRL-C> to abort...\n";
    $protfile = FixPathSep($protfile);
    system("more $protfile");
}

sub LCEAdmTailProt
{
    my $protfile = shift;
    my @printarr;
    my $linecount = ($= - 3);
    
    print "Linking protocol '$protfile'.\nPress <CTRL-C> to abort...\n";

    open (IN_FILE, "< $protfile") or return 0;
    while (<IN_FILE>)
    {
        push @printarr, $_;
        if ($linecount)
        {
            $linecount = $linecount - 1;
        }
        else
        {
            shift @printarr;
        }
    }
    
    print @printarr;
    
    while (1)
    {
        sleep(1);
        while (<IN_FILE>)
        {
            print "$_";
        }
    }
    
    close (IN_FILE);
}

sub LCEAdmShowInfo
{
    my $href = shift;
    
    ShowHashRef($href);
}

sub _WaitCondition
{
    my ($cond, $time, $output) = @_;
    my $cond_match = 0;
    
    while (!$cond_match && ($time > 0))
    {
        eval ('$cond_match = ' . "$cond");
        if ($@)
        {
            print "Eval Error: $@\n" if ($output);
            return 0;
        }
        
        return 1 if ($cond_match);
        $time = $time - 1;
        sleep (1);
        print "." if ($output);
    }
    return 0;
}

sub _GetPID
{
    my $file = shift;
    
    open (IN_FILE, "< $file") or return 0;
    while (<IN_FILE>)
    {
        if (/^(\d+)\D*/)
        {
            close (IN_FILE);
            return $1;
        }
    }
    close (IN_FILE);
    return undef;
}
