#! /usr/bin/perl
#
#    P4Sync.pm - Implements Perforce sync functions.
#
#    @(#)P4Sync.pm     2003-04-29
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

package Perforce::P4Sync;

require Exporter;

@ISA    = qw(Exporter);
@EXPORT = qw(P4Sync);

use strict;

##############################################################################
# P4Sync() - Synchronizes a perforce branch
##############################################################################

sub P4Sync
{
    my ($p4port, $p4user, $p4client, $p4branch, $chlist, $force, $verbose, @flist) = @_;

    unless ($p4port)   { die "P4Sync: ERROR! First parameter (p4port) is not set!\n" }
    unless ($p4user)   { die "P4Sync: ERROR! Second parameter (p4user) is not set!\n" }
    unless ($p4client) { die "P4Sync: ERROR! Third parameter (p4client) is not set!\n" }
    unless ($p4branch) { die "P4Sync: ERROR! Fourth parameter (p4branch) is not set!\n" }
    unless ($chlist)   { $chlist = 0 }

    my $cmdparam = "";

    if (@flist)
    {
        if ($p4branch =~ /(.*)\/\.\.\./) { $p4branch = "$1" }
        foreach my $file (@flist)
        {
            $cmdparam .= " $p4branch/$file" . ($chlist ? ($chlist ne "none" ? "\@$chlist" : "#none") : "");
        }
    }
    else
    {
        unless ($p4branch =~ /.*\/\.\.\./) { $p4branch .= "/..." }
        $cmdparam = ($chlist ? ($chlist ne "none" ? "$p4branch\@$chlist" : "$p4branch#none") : "$p4branch");
    }

    my $rc = 0;

    if ($force)
    {
        $cmdparam = "-f " . $cmdparam;
    }

    $| = 1;

    open CMD_OUT, "p4 -s -p $p4port -u $p4user -c $p4client sync $cmdparam 2>&1 |" or print STDERR "Can't open p4 sync $cmdparam!\n$!\n";

    while (<CMD_OUT>)
    {
        if    (/^(.*):\s(.*)\s-\sadded/)       { print "Add     : $2\n" if ($verbose) }
        elsif (/^(.*):\s(.*)\s-\srefreshing/)  { print "Refresh : $2\n" if ($verbose) }
        elsif (/^(.*):\s(.*)\s-\supdating/)    { print "Update  : $2\n" if ($verbose) }
        elsif (/^(.*):\s(.*)\s-\sdeleted/)     { print "Deleted : $2\n" if ($verbose) }
        elsif (/^(.*):.*up-to-date/)           { print "Info    : All files are up-to-date.\n" if ($verbose) }
        elsif (/^error:\s(.*)/)                { print "Error   : $1\n" if ($verbose) }
        elsif (/^info:\s(.*)/)                 { print "Info    : $1\n" if ($verbose) }
        elsif (/^exit:\s(\d+)/)                { $rc = $1;                 }
        else                                   { print "$_" if ($verbose) }
    }
    close CMD_OUT or $rc = 40;

    return ($rc ? 0 : 1);
}