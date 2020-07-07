#! /usr/bin/perl
#
#    P4Admin.pm - Implements Perforce administrative functions.
#
#    @(#)P4Admin.pm     2003-04-24
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

package Perforce::P4Admin;

require Exporter;

@ISA    = qw(Exporter);
@EXPORT = qw(P4SetUser P4RemUser P4SetClient P4RemClient);

use strict;
use Perforce::P4Info;

##############################################################################
# P4SetUser() - Adds / updates a perforce user.
##############################################################################

sub P4SetUser
{
    my ($p4port, $p4user, $fullname, $email) = @_;

    my $href;

    unless ($p4port) { die "P4SetUser: ERROR! First parameter (p4port) is not set!\n" }
    unless ($p4user) { die "P4SetUser: ERROR! Second parameter (p4user) is not set!\n" }

    $| = 1;

    $ENV{'P4USER'} = $p4user;

    $href = P4GetUserInfo($p4port, $p4user);

    $href->{'User'}     = $p4user;
    $href->{'Email'}    = $email if ($email);
    $href->{'FullName'} = $fullname if ($fullname);

    if (open USER_IN, "| p4 -p $p4port user -i > p4out.tmp")
    {
        print USER_IN "User:\t$href->{'User'}\n";
        print USER_IN "Email:\t$href->{'Email'}\n";
        print USER_IN "FullName:\t$href->{'FullName'}\n";
        unless(close(USER_IN))
        {
            open (ERROUT, "< p4out.tmp");
            while (<ERROUT>) { print "$_"; }
            close (ERROUT);
            unlink "p4out.tmp";
            return 0;
        }

        unlink "p4out.tmp";
    }
    else
    {
        print STDERR "P4SetUser: Can't open 'p4 user -i'!\n$!\n";
        return 0;
    }

    $ENV{'P4USER'} = $p4user;

    return 1;
}

##############################################################################
# P4RemUser() - Deletes a perforce user.
##############################################################################

sub P4RemUser
{
    my ($p4port, $p4user) = @_;
    my $rem_OK = 0;

    unless ($p4port) { die "P4RemUser: ERROR! First parameter (p4port) is not set!\n" }
    unless ($p4user) { die "P4RemUser: ERROR! Second parameter (p4user) is not set!\n" }

    $| = 1;

    $ENV{'P4USER'} = $p4user;

    if (open USER_OUT, "p4 -p $p4port user -d $p4user |")
    {
        while(<USER_OUT>)
        {
            $rem_OK = 1 if(/^.*deleted\.$/);
        }
        close USER_OUT;
    }
    else
    {
        print STDERR "P4RemUser: Can't open 'p4 -p $p4port user -d $p4user'!\n$!\n";
    }

    $ENV{'P4USER'} = undef if ($rem_OK);

    return $rem_OK;
}

##############################################################################
# P4SetClient() - Adds / edits a perforce client.
##############################################################################

sub P4SetClient
{
    my ($p4port, $p4client, $rootdir, $owner, $host, $opts, $desc_ref, $view_ref) = @_;

    unless ($p4port)   { die "P4SetClient: ERROR! First parameter (p4port) is not set!\n" }
    unless ($p4client) { die "P4SetClient: ERROR! Second parameter (p4client) is not set!\n" }

    if ($desc_ref && !ref($desc_ref)) { die "P4SetClient: ERROR! Seventh parameter (desc_ref) is set but is no reference! Please use a reference like \\\@desc here." }
    if ($view_ref && !ref($view_ref)) { die "P4SetClient: ERROR! Eigth parameter (view_ref) is set but is no reference! Please use a reference like \\\@views here." }

    $| = 1;

    my $href = P4GetClientInfo($p4port, $p4client);

    $href->{'Client'}       = $p4client;
    $href->{'Owner'}        = $owner if ($owner);
    $href->{'Host'}         = $host if ($host);
    $href->{'Description'}  = $desc_ref if ($desc_ref);
    $href->{'Root'}         = $rootdir if ($rootdir);
    $href->{'Options'}      = $opts if ($opts);
    $href->{'View'}         = $view_ref if ($view_ref);

    if (open CLIENT_IN, "| p4 -p $p4port client -i > p4out.tmp")
    {
        print CLIENT_IN "Client:\t$href->{'Client'}\n";
        print CLIENT_IN "Owner:\t$href->{'Owner'}\n";
        print CLIENT_IN "Host:\t$href->{'Host'}\n";
        print CLIENT_IN "Description:\n";
        foreach my $desc (@{$href->{'Description'}}) { print CLIENT_IN "\t$desc\n" }
        print CLIENT_IN "Root:\t$href->{'Root'}\n";
        print CLIENT_IN "Options:\t$href->{'Options'}\n";
        print CLIENT_IN "View:\n";
        foreach my $view (@{$href->{'View'}}) { print CLIENT_IN "\t$view\n" }

        unless (close(CLIENT_IN))
        {
            open (ERROUT, "< p4out.tmp");
            while (<ERROUT>) { print "$_"; }
            close (ERROUT);
            unlink "p4out.tmp";
            return 0;
        }

        unlink "p4out.tmp";
    }
    else
    {
        print STDERR "P4SetClient: Can't open 'p4 -p $p4port client -i'!\n$!\n";
        return 0;
    }

    $ENV{'P4CLIENT'} = $p4client;

    return 1;
}

##############################################################################
# P4SetClient() - Adds / edits a perforce client.
##############################################################################

sub P4RemClient
{
    my ($p4port, $p4client) = @_;
    my $rem_OK = 0;

    unless ($p4port)   { die "P4RemClient: ERROR! First parameter (p4port) is not set!\n" }
    unless ($p4client) { die "P4RemClient: ERROR! Second parameter (p4client) is not set!\n" }

    $| = 1;

    if (open CLIENT_OUT, "p4 -p $p4port client -d $p4client |")
    {
        while(<CLIENT_OUT>)
        {
            $rem_OK = 1 if(/^.*deleted\.$/);
        }
        close CLIENT_OUT;
    }
    else
    {
        print STDERR "P4RemClient: Can't open 'p4 -p $p4port client -d $p4client'!\n$!\n";
    }

    $ENV{'P4CLIENT'} = undef if ($rem_OK);

    return $rem_OK;
}

1;