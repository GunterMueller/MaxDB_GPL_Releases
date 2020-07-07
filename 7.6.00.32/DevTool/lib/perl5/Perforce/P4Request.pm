#! /usr/bin/perl
#
#    P4Request.pm - Implements request functions for Perforce settings.
#
#    @(#)P4Request.pm     2003-04-24
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

package Perforce::P4Request;

require Exporter;

@ISA    = qw(Exporter);
@EXPORT = qw(P4RequestUser P4RequestClient P4RequestRoot P4RequestPasswd);

use strict;

use Tool::RequestInput;
use IO::CurrentDir;
use Perforce::P4Info;

##############################################################################
# P4RequestUser() - Requests a perforce user name.
##############################################################################

sub P4RequestUser
{
    my ($use_default) = @_;
    my $login = getlogin;
    my $p4user;

    $| = 1;

    $login = lc ($login);

    if ($use_default)
    {
        unless ($ENV{'P4USER'})
        {
            $ENV{'P4USER'} = $login;
        }

        return $ENV{'P4USER'};
    }

    print "\nPlease select which Perforce user you want to use:\n\n";
    print "[l] = use your login name ($login), default\n";

    if ($ENV{'P4USER'})
    {
        print "[p] = use current Perforce user ($ENV{'P4USER'})\n";
    }

    print "[s] = select a different user name manually\n\n";

    my $sel = RequestInput("Please select: ", "l", $ENV{'P4USER'} ? "[lLpPsS]" : "[lLsS]", "Input incorrect. Please, just use " . ($ENV{'P4USER'} ? "[l, p or s].\n" : "[l or s].\n"));

    if ($sel =~ /^l$/i)
    {
        $p4user = $login;
    }
    elsif ($sel =~ /^p$/i)
    {
        $p4user = $ENV{'P4USER'};
    }
    else
    {
        $p4user = RequestInput("Perforce user: ", "", "[a-zA-Z0-9_]+", "Input incorrect. Please, just use [a-z, A-Z, 0-9 or _].\n");
    }

    $ENV{'P4USER'} = $p4user;

    return $p4user;
}

##############################################################################
# P4RequestClient() - Requests a perforce client name.
##############################################################################

sub P4RequestClient
{
    my ($use_default) = @_;
    my $hostname = qx(hostname);
    my $p4client;

    $| = 1;

    chomp $hostname;

    if ($use_default)
    {
        unless ($ENV{'P4CLIENT'})
        {
            $ENV{'P4CLIENT'} = $hostname;
        }

        return $ENV{'P4CLIENT'};
    }

    print "\nPlease select which Perforce client you want to use:\n\n";
    print "[h] = use the local host name ($hostname), default\n";

    if ($ENV{'P4CLIENT'})
    {
        print "[p] = use current Perforce client ($ENV{'P4CLIENT'})\n";
    }

    print "[s] = select a different client name manually\n\n";

    my $sel = RequestInput("Please select: ", "h", $ENV{'P4CLIENT'} ? "[hHpPsS]" : "[hHsS]", "Input incorrect. Please, just use " . ($ENV{'P4CLIENT'} ? "[h, p or s].\n" : "[h or s].\n"));

    if ($sel =~ /^h$/i)
    {
        $p4client = $hostname;
    }
    elsif ($sel =~ /^p$/i)
    {
        $p4client = $ENV{'P4CLIENT'};
    }
    else
    {
        $p4client = RequestInput("Perforce client: ", "", "[a-zA-Z0-9_]+", "Input incorrect. Please, just use [a-z, A-Z, 0-9 or _].\n");
    }

    $ENV{'P4CLIENT'} = $p4client;

    return $p4client;
}

##############################################################################
# P4RequestPasswd() - Requests a perforce user passwd.
##############################################################################

sub P4RequestPasswd
{
    my ($p4user) = @_;
    print "\nPlease enter the password for user '$p4user':\n\n";

    my $pw = RequestInput("Password: ", "", ".+", "Input incorrect. Please enter any string which is not empty!\n");

    $ENV{'P4PASSWD'} = $pw;

    return $pw;
}

##############################################################################
# P4RequestRoot() - Requests a perforce root path.
##############################################################################

sub P4RequestRoot
{
    my ($p4port, $p4client, $use_default) = @_;

    $| = 1;

    my $clientroot;
    my $curdir      = GetCurrentDir();
    my $p4root;

    if ($p4port && $p4client)
    {
        if (P4ClientExists($p4port, $p4client))
        {
            my $clientinfo = P4GetClientInfo($p4port, $p4client);
            $clientroot = $clientinfo->{'Root'};
        }
    }

    if ($use_default)
    {
        if ($clientroot)
        {
            return $clientroot;
        }
        else
        {
            return $curdir;
        }
    }

    print "\nPlease select which client root you want to use for your P4 client:\n\n";

    print "[c] = use the current directory ($curdir)\n";

    if ($clientroot)
    {
        print "[p] = use current P4 client root ($clientroot)\n";
    }

    print "[s] = select a different client name manually\n\n";

    my $sel = RequestInput("Please select: ", "", $clientroot ? "[cCpPsS]" : "[cCsS]", "Input incorrect. Please, just use " . ($clientroot ? "[c, p or s].\n" : "[o or s].\n"));

    if ($sel =~ /^c$/i)
    {

        $p4root = $curdir;
    }
    elsif ($sel =~ /^p$/i)
    {
        $p4root = $clientroot;
    }
    else
    {
        $p4root = RequestInput("Perforce client root: ", "", ".+", "Input incorrect. Please, just use normal characters.\n");
    }

    return $p4root;
}