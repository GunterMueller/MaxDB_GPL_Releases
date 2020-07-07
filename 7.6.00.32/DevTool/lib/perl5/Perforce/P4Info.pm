#! /usr/bin/perl
#
#    P4Info.pm - Implements Perforce info functions.
#
#    @(#)P4GetSet.pm     2003-04-24
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

package Perforce::P4Info;

require Exporter;

@ISA    = qw(Exporter);
@EXPORT = qw(P4UserExists P4GetUserInfo P4ClientExists P4GetClientInfo P4PasswdNeeded P4GetDir P4GetChangeList P4GetChangeListAge P4GetToolVer P4GetChanges);

use strict;
use Time::Local;

##############################################################################
# P4UserExists() - Checks whether a user exists or not.
##############################################################################

sub P4UserExists
{
    my ($p4port, $p4user) = @_;

    unless ($p4port)   { die "P4UserExists: ERROR! First parameter (p4port) is not set!\n" }
    unless ($p4user)   { die "P4UserExists: ERROR! Second parameter (p4user) is not set!\n" }

    $| = 1;

    my $found = 0;

    open USER_OUT, "p4 -p $p4port user -o $p4user |" or print STDERR "Can't open 'p4 user -o $p4user'!\n$!\n";
    while (<USER_OUT>)
    {
        $found = 1 if (/^Access:/);
    }
    close USER_OUT or print STDERR "Can't close 'p4 user -i $p4user'!\n$!\n";

    return $found;
}

##############################################################################
# P4GetUserInfo() - Retrieves some user information.
##############################################################################

sub P4GetUserInfo
{
    my ($p4port, $p4user) = @_;

    unless ($p4port)   { die "P4GetUserInfo: ERROR! First parameter (p4port) is not set!\n" }
    unless ($p4user)   { die "P4GetUserInfo: ERROR! Second parameter (p4user) is not set!\n" }

    $| = 1;

    my $href = {};

    open USER_OUT, "p4 -p $p4port user -o $p4user |" or print STDERR "Can't open 'p4 user -o $p4user'!\n$!\n";
    while (<USER_OUT>)
    {
        if (/^(\S+):\s+(\S.*)$/)
        {
            $href->{$1} = $2;
        }
    }

    close USER_OUT or print STDERR "Can't close 'p4 user -i $p4user'!\n$!\n";

    return $href;
}

##############################################################################
# P4ClientExists() - Checks whether a perforce client exists or not.
##############################################################################

sub P4ClientExists
{
    my ($p4port, $p4client) = @_;

    unless ($p4port)   { die "P4ClientExists: ERROR! First parameter (p4port) is not set!\n" }
    unless ($p4client) { die "P4ClientExists: ERROR! Second parameter (p4client) is not set!\n" }

    $| = 1;

    my $found = 0;

    open CLIENT_OUT, "p4 -p $p4port client -o $p4client |" or print STDERR "Can't open 'p4 client -o $p4client'!\n$!\n";
    while (<CLIENT_OUT>)
    {
        $found = 1 if (/^Access:/);
    }
    close CLIENT_OUT or print STDERR "Can't close 'p4 client -i $p4client'!\n$!\n";

    return $found;
}

##############################################################################
# P4GetClientInfo() - Retrieves information about a client.
##############################################################################

sub P4GetClientInfo
{
    my ($p4port, $p4client) = @_;

    unless ($p4port)   { die "P4GetClientInfo: ERROR! First parameter (p4port) is not set!\n" }
    unless ($p4client) { die "P4GetClientInfo: ERROR! Second parameter (p4client) is not set!\n" }

    $| = 1;

    my $href = {};
    my $refname;
    my @tmparr;

    open CLIENT_OUT, "p4 -p $p4port client -o $p4client |" or print STDERR "Can't open 'p4 client -o $p4client'!\n$!\n";
    while (<CLIENT_OUT>)
    {
        if (/^(\S+):\s+(\S.*)$/)
        {
            $href->{$1} = $2;
        }
        elsif (/^(\S+):$/)
        {
            $refname = $1;
        }
        elsif (/^$/)
        {
            if ($refname)
            {
                @{$href->{$refname}} = @tmparr;
                $refname = undef;
                undef @tmparr;
            }
        }
        elsif (/^\s+(\S.*)$/)
        {
            push @tmparr, $1;
        }
    }
    close CLIENT_OUT or print STDERR "Can't close 'p4 client -i $p4client'!\n$!\n";

    return $href;
}

##############################################################################
# P4PasswdNeeded() - Checks wether a password is needed or not.
##############################################################################

sub P4PasswdNeeded
{
    my ($p4port, $p4user) = @_;

    unless ($p4port)    { die "P4PasswdNeeded: ERROR! First parameter (p4port) is not set!\n" }
    unless ($p4user)    { die "P4PasswdNeeded: ERROR! Second parameter (p4user) is not set!\n" }

    $| = 1;

    my $needed = 0;

    open (CMD_OUT, "p4 -p $p4port -u $p4user changes -m 1 2>&1 |") or return 1;
    while (<CMD_OUT>)
    {
        if ((/\(P4PASSWD\)/) or (/^Password:.*/))
        {
            $needed = 1;
            last;
        }
    }

    close (CMD_OUT);

    if ($needed) {
        my $login = getlogin || getpwuid($<);
        my $pwfile = ($^O =~ /win32/i ? "\\\\production\\info\\make\\$login\\getp4pw" : "/sapmnt/production/info/make/$login/getp4pw");
        if ((!exists($ENV{'P4USER'}) || $ENV{'P4USER'} eq $login) && -f $pwfile) {
            if (open(PW_FILE, "<$pwfile")) {
                while (<PW_FILE>) {
                    if (/^echo\s+"(.*)"/) {
                        $ENV{'P4PASSWD'} = $1;
                        $needed = 0;
                        last;
                    }
                }
                close (PW_FILE);
            }
        }
    }    
    return $needed;
}

##############################################################################
# P4GetDir() - Retrieves all sub-dirs / files of a perforce directory.
##############################################################################

sub P4GetDir
{
    my ($p4port, $p4user, $dir) = @_;

    unless ($p4port)    { die "P4GetDir: ERROR! First parameter (p4port) is not set!\n" }
    unless ($p4user)    { die "P4GetDir: ERROR! Second parameter (p4user) is not set!\n" }
    unless ($dir)       { die "P4GetDir: ERROR! Third parameter (dir) is not set!\n" }

    $| = 1;

    my $href = {};

    $dir =~ tr/\\/\//;
    $dir = "//" . $dir unless ($dir =~ /^\/\/(.*)$/);
    $dir = $1 if ($dir =~ /^(\/\/.*)\/$/);

    $href->{'DIRS'} = ();

    #print "DEBUG: dir = $dir\n";

    open DIR_OUT, "p4 -p $p4port -u $p4user dirs $dir/* 2>&1 |" or print STDERR "P4GetDir: Can't open 'p4 -p $p4port -u $p4user dirs $dir/*'!\n$!\n";

    while (<DIR_OUT>)
    {
        last if (/.*no\ssuch\sfile/);
        push @{$href->{'DIRS'}}, $1 if (/^$dir\/(.*)$/);
    }

    close DIR_OUT;

    $href->{'FILES'} = ();

    open FILE_OUT, "p4 -p $p4port -u $p4user fstat $dir/* 2>&1 |" or print STDERR "P4GetDir: Can't open 'p4 -p $p4port -u $p4user fstat $dir/*'!\n$!\n";

    while (<FILE_OUT>)
    {
        last if (/.*no\ssuch\sfile/);
        push @{$href->{'FILES'}}, $1 if (/^\.\.\.\s+depotFile\s+$dir\/(.*)$/);
        pop @{$href->{'FILES'}} if (/^\.\.\.\s+headAction\s+delete$/);
    }

    close FILE_OUT;

    return $href;
}

##############################################################################
# P4GetChangeList() - Retrieves the latest change number for a branch.
##############################################################################

sub P4GetChangeList
{
    my ($p4port, $p4user, $p4branch) = @_;

    unless ($p4port)   { die "P4GetChangeList: ERROR! First parameter (p4port) is not set!\n" }
    unless ($p4user)   { die "P4GetChangeList: ERROR! Second parameter (p4user) is not set!\n" }
    unless ($p4branch) { die "P4GetChangeList: ERROR! Third parameter (p4branch) is not set!\n" }

    unless ($p4branch =~ /.*\/\.\.\./) { $p4branch .= "/..." }

    my $chlist = 0;

    open CHLIST_OUT, "p4 -p $p4port -u $p4user changes -m 1 -s submitted $p4branch |" or print STDERR "Can't open 'p4 -p $p4port -u $p4user changes -m 1 -s submitted $p4branch'.\n$!\n";
    while(<CHLIST_OUT>)
    {
        if (/^Change\s*(\d+)\s/)
        {
            $chlist = $1;
        }
    }
    close CHLIST_OUT;

    return $chlist;
}


##############################################################################
# P4GetChangeListAge() - Retrieves the age in minutes of a change list.
##############################################################################

sub P4GetChangeListAge
{
    my ($p4port, $p4user, $chlist) = @_;

    unless ($p4port)   { die "P4GetChangeList: ERROR! First parameter (p4port) is not set!\n" }
    unless ($p4user)   { die "P4GetChangeList: ERROR! Second parameter (p4user) is not set!\n" }
    unless ($chlist)   { die "P4GetChangeList: ERROR! Third parameter (chlist) is not set!\n" }
    my $tdiff = 0;
    open CHLIST_OUT, "p4 -p $p4port -u $p4user describe $chlist |" or print STDERR "Can't open 'p4 -p $p4port -u $p4user $chlist'.\n$!\n";
    while(<CHLIST_OUT>)
    {
        if (/.*\son\s(\d+)\/(\d+)\/(\d+)\s(\d+):(\d+):(\d+)\s/)
        {
            $tdiff = (time() - timelocal($6, $5, $4, $3, ($2 - 1), ($1 - 1900))) / 60;
        }
    }
    close CHLIST_OUT;

    return $tdiff;
}

##############################################################################
# P4GetChanges() - Returns a list of all changes betw. latest and oldest given change.
##############################################################################

sub P4GetChanges
{
    my ($p4port, $p4user, $p4branch, $p4latest, $p4oldest, $maxentries) = @_;

    unless ($p4port)   { die "P4GetChangeList: ERROR! First parameter (p4port) is not set!\n" }
    unless ($p4user)   { die "P4GetChangeList: ERROR! Second parameter (p4user) is not set!\n" }
    unless ($p4branch) { die "P4GetChangeList: ERROR! Third parameter (p4branch) is not set!\n" }

    unless ($p4branch =~ /.*\/\.\.\./) { $p4branch .= "/..." }

    $maxentries = 0 if ($maxentries < 0);

    my $do_log = ($p4latest ? 0 : 1);
    my $ready_for_close = 0;
    my @log_lines;
    my $entries = 0;

    open CHLIST_OUT, "p4 -p $p4port -u $p4user changes -s submitted $p4branch |" or print STDERR "Can't open 'p4 -p $p4port -u $p4user changes -m 1 -s submitted $p4branch'.\n$!\n";
    while(<CHLIST_OUT>)
    {
        if (/^Change\s*(\d+)\s(.*)/)
        {
            my $chlist = $1;
            my $chtext = $2;

            $do_log = 1 if ($chlist <= $p4latest);
            if ($chlist < $p4oldest)
            {
                $do_log = 0;
                $ready_for_close = 1;
            }

            if ($do_log)
            {
                next if ($chtext =~ /implicit\ssubmit\sfrom\scheck/);
                push @log_lines, "Change $chlist $chtext";
                $entries += 1;
                if ($maxentries && ($entries >= $maxentries))
                {
                    $do_log = 0;
                    $ready_for_close = 1;
                    push @log_lines, "...(skipped)";
                }
            }
        }

        if ($ready_for_close)
        {
            close (CHLIST_OUT);
        }
    }

    close CHLIST_OUT if (!$ready_for_close);

    return (wantarray ? @log_lines : join("\n", @log_lines));
}

##############################################################################
# P4GetToolVer() - Retrieves the correct tool version for a lc release.
##############################################################################

sub P4GetToolVer
{
    my ($lcrel) = @_;

    unless ($lcrel)   { die "P4GetToolVer: ERROR! First parameter (lcrel) is not set!\n" }

    if ($lcrel =~ /^(\d)\.?(\d)\.?(\d\d)\.?\d?\d?\.?_?(dev|cor|ramp)$/i)
    {
        my $rel = "$1$2$3" . (($4 and ($4 !~ /ramp/i)) ? "_$4" : "");
        my $ret;

        open (MAP_OUT, "p4 -p perforce3006.wdf.sap.corp:3006 -u remuser print -q //sapdb/V74/toolmap.cfg |") or print "Can't read toolmap.cfg!\n$!\n";
        while (<MAP_OUT>)
        {
            if (/^$rel\s+=\s+(\S+)/i)
            {
                $ret = $1;
                last;
            }
        }
        close (MAP_OUT);

        unless ($ret)
        {
            print "Version '$rel' not found. Taking Tools 'dev'.\n";
            $ret = "dev";
        }

        return $ret;
    }
    else
    {
        print "LC-Version format incorrect: $lcrel\n";
        return "dev";
    }
}