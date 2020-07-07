#!/usr/bin/perl
# @(#) LCEnvAdm	2003-11-22  Ulrich Jansen
#
# Admin Tool für prod. LCEnv makes.
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

BEGIN
{
    unless (defined $ENV{'TOOL'})
    {
    	my $callers_path = (caller())[1];
	    $callers_path  =~ tr/\//\\/ if ($^O=~/.*win32.*/i);
		$ENV{'TOOL'} = $1 if ($callers_path =~ /^(.*)[\\\/]bin[\\\/]LCEnv.*/);
    }

    unshift @INC, "$ENV{'TOOL'}/lib/perl5";
    unshift @INC, "$ENV{'TOOL'}/bin";
    
    if (-e "/devtool/local/bin")
    {
        $ENV{'PATH'} = "/devtool/local/bin:$ENV{'PATH'}";
    }
    
    # Fix for Perforce path:
    
    if (-e "/usr/local/bin")
    {
        $ENV{'PATH'} = "/usr/local/bin:$ENV{'PATH'}";
    }
}

select (STDERR);
$|=1;
select (STDOUT);
$|=1;

use strict;
use Env::LCEnvAdm;
use File::Basename;
use Getopt::Long;

my %Opts;
my $Data = {};

print "\nLCEnvAdm v3.02 (2004-02-19)\n\n";

if (!GetOptions( \%Opts,
                'h|help|?',
                'd|dir=s',
                's|show=s',
                't|tail=s',
                'i|info',
                'lock',
                'unlock',
                'k|kill') || $Opts{'h'})
{
    Usage();
    exit(0);
}

my ($profile, $path, $suff) = fileparse($ARGV[0], '.cfg') if ($ARGV[0]);
$path = ($path =~ /\.[\\\/]/ ? ($Opts{'d'} ? $Opts{'d'} : "") : $path);

if (($Opts{'s'} || $Opts{'t'} || $Opts{'i'} || $Opts{'lock'} || $Opts{'unlock'} || $Opts{'k'}) && !$profile)
{
    print "Error: Missing <Release> as argument!\n\n";
    Usage();
    exit(0);
}

my @profiles;
push @profiles, $profile if ($profile);
$Data = LCEAdmLoadProfiles($path, @profiles);

if ($Opts{'s'} || $Opts{'t'})
{
    my $cmd = ($Opts{'s'} ? $Opts{'s'} : $Opts{'t'});
    my $protfile;
#    my $pre = $1 if ($profile =~ /^\D*(\d+)\D*$/);

    if ($cmd eq 'prot')
    {
        $protfile = LCEAdmGetActProt("$Data->{$profile}->{'RELEASE'}->{'OWN'}/prot", '.prot');
    }
    elsif ($cmd eq 'lastrun')
    {
        $protfile = LCEAdmGetActProt("$Data->{$profile}->{'GLOBAL'}->{'XPROT'}", $profile . '_Last');
    }
    elsif ($cmd eq 'dayrun')
    {
        $protfile = LCEAdmGetActProt("$Data->{$profile}->{'GLOBAL'}->{'XPROT'}", $profile . '_LastRun');
    }
    elsif ($cmd eq 'nightrun')
    {
        $protfile = LCEAdmGetActProt("$Data->{$profile}->{'GLOBAL'}->{'XPROT'}", $profile . '_LastNightRun');
    }
    elsif ($cmd eq 'weekendrun')
    {
        $protfile = LCEAdmGetActProt("$Data->{$profile}->{'GLOBAL'}->{'XPROT'}", $profile . '_LastWeekendRun');
    }
    else
    {
        print "ERROR: Show / tail command '$cmd' unknown!\n";
        Usage();
        exit(0);
    }
    
    unless (-e $protfile)
    {
        print "Warning: No valid protocol found!\nExiting...\n";
        exit(0);
    }
    
    if ($Opts{'s'})
    {
        LCEAdmShowProt($protfile);
    }
    else
    {
        LCEAdmTailProt($protfile);
    }
    
    exit(0);
}
elsif ($Opts{'i'})
{
    print "Showing release information for '$profile'...\n\n";
    LCEAdmShowInfo($Data->{$profile});
}
elsif ($Opts{'lock'})
{
    print "Locking release '$profile'..";
    print (LCEAdmLockRel($Data->{$profile}) ? "..OK\n" : "..FAILED!\n");
}
elsif ($Opts{'unlock'})
{
    print "Unlocking release '$profile'..";
    print (LCEAdmUnlockRel($Data->{$profile}) ? "..OK\n" : "..FAILED!\n");
}
elsif ($Opts{'k'})
{
    print "Killing '$profile'...\n";
    LCEAdmKill($Data->{$profile});
}
else
{
    LCEAdmShowRels($Data);
}

exit(0);

sub Usage
{
    print <<USAGE_END;
    
Usage: LCEnvAdm [-d <profile dir>] [-(h|help|?)] [-(k|kill)] [-(s|show <type>]
                [-(t|tail) <type>] [-lock] [-unlock] [-(i|info)] [<release>]

Where:
        -d <profile dir>
            Selects an alternative profile directory. If not set, the default 
            will be used (stored under TOOL/config/LCEnv.default).
            
        -h or -help -?
            Does something you would NEVER expect...sagte die Henne zum Ei.
            
        -k  or -kill *
            Kills a process chain identified by its lock name.
            
        -s <type> or -show <type>*
            Shows a protocol as the 'more' command would do. Valid <type>'s
            are:
            
            prot :       normal protocol file from 'OWN/prot'
            lastrun :    latest debug protocol (day/night/weekend)
            dayrun :     latest day-run debug protocol
            nightrun :   latest night-run debug protocol
            weekendrun : latest weekend-run debug protocol
            
        -t <type> or -tail <type>*
            Links directly to protocol output like 'tail -f' would do. 
            Protocol types are the same as described above.
        
        -lock *
            Locks a profile permanently - even 'LCEnv -lockforce' has no
            effect.
            
        -unlock *
            Enables a profile again.
            
        -i or -info *
            Shows configuration information for a release.
            
Note:
        All options marked by an asterix (*) need a release to be given.
        Release names are same as the .cfg file names (ie. '7500dev').
            
USAGE_END
}