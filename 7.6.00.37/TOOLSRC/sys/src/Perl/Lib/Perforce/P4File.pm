#!/usr/bin/perl
#
#    P4File - Implements Perfoce file manipulation functions
#
#    @(#)P4File.pm     2003-12-01
#
#    U. Jansen, SAP AG
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

##############################################################################
# Static includes:

package Perforce::P4File;

require Exporter;

@ISA    = qw(Exporter);
@EXPORT = qw(P4Add P4Edit P4Submit P4Revert P4FStat);

use strict;

sub P4Edit
{
    my ($p4port, $p4user, $p4client, $p4file) = @_;
    my $rc = 0;

    unless ($p4port)   { die "P4Edit: ERROR! First parameter (p4port) is not set!\n" }
    unless ($p4user)   { die "P4Edit: ERROR! Second parameter (p4user) is not set!\n" }
    unless ($p4client) { die "P4Edit: ERROR! Third parameter (p4client) is not set!\n" }
    unless ($p4file)    { die "P4Edit: ERROR! Fourth parameter (flist) is not set!\n" }

    open (CMD_IN, "p4 -p $p4port -u $p4user -c $p4client edit $p4file |") or return 0;
    
    while (<CMD_IN>)
    {
        if (/.*opened\sfor\sedit$/)
        {
            $rc = 1;
        }
    }
    close (CMD_IN);
    return $rc;   
}

sub P4Add
{
    my ($p4port, $p4user, $p4client, $p4file) = @_;
    my $rc = 0;

    unless ($p4port)   { die "P4Edit: ERROR! First parameter (p4port) is not set!\n" }
    unless ($p4user)   { die "P4Edit: ERROR! Second parameter (p4user) is not set!\n" }
    unless ($p4client) { die "P4Edit: ERROR! Third parameter (p4client) is not set!\n" }
    unless ($p4file)    { die "P4Edit: ERROR! Fourth parameter (flist) is not set!\n" }

    open (CMD_IN, "p4 -p $p4port -u $p4user -c $p4client add $p4file |") or return 0;
    
    while (<CMD_IN>)
    {
        if (/.*opened\sfor\sadd$/)
        {
            $rc = 1;
        }
    }
    close (CMD_IN);
    return $rc;   
}

sub P4Diff
{

}

sub P4Revert
{
    my ($p4port, $p4user, $p4client, $p4file) = @_;
    my $rc = 0;

    unless ($p4port)   { die "P4Edit: ERROR! First parameter (p4port) is not set!\n" }
    unless ($p4user)   { die "P4Edit: ERROR! Second parameter (p4user) is not set!\n" }
    unless ($p4client) { die "P4Edit: ERROR! Third parameter (p4client) is not set!\n" }
    unless ($p4file)    { die "P4Edit: ERROR! Fourth parameter (flist) is not set!\n" }

    open (CMD_IN, "p4 -p $p4port -u $p4user -c $p4client revert $p4file 2>&1 |") or return 0;
    
    while (<CMD_IN>)
    {
        if (/.*reverted$/)
        {
            $rc = 1;
        }
    }
    close (CMD_IN);
    return $rc;   

}

sub P4FStat
{
    my ($p4port, $p4user, $p4client, $p4file) = @_;

    unless ($p4port)   { die "P4Edit: ERROR! First parameter (p4port) is not set!\n" }
    unless ($p4user)   { die "P4Edit: ERROR! Second parameter (p4user) is not set!\n" }
    unless ($p4client) { die "P4Edit: ERROR! Third parameter (p4client) is not set!\n" }
    unless ($p4file)    { die "P4Edit: ERROR! Fourth parameter (flist) is not set!\n" }

    my $href = {};

    open (CMD_IN, "p4 -p $p4port -u $p4user -c $p4client fstat $p4file 2>&1 |") or return 0;
    
    while (<CMD_IN>)
    {
        if (/^\.\.\.\s(\S+)\s(.*)$/)
        {
            $href->{$1} = $2;
        }
    }
    close (CMD_IN);

    return $href;
}

sub P4Submit
{
    my ($p4port, $p4user, $p4client, $p4file, $p4desc) = @_;
    my $rc = 0;
    my $clist = 0;

    unless ($p4port)   { die "P4Submit: ERROR! First parameter (p4port) is not set!\n" }
    unless ($p4user)   { die "P4Submit: ERROR! Second parameter (p4user) is not set!\n" }
    unless ($p4client) { die "P4Submit: ERROR! Third parameter (p4client) is not set!\n" }
    unless ($p4file)   { die "P4Submit: ERROR! Fourth parameter (flist) is not set!\n" }
    unless ($p4desc)   { $p4desc = "P4Submit: Implicit changelist created for file '$p4file'" }

    unless (open (CMD_OUT, "| p4 -p $p4port -u $p4user -c $p4client submit -i > tmpout.txt")) { return 0; }

    print CMD_OUT "Change: new\n\n";
    print CMD_OUT "Description: $p4desc\n\n";
    print CMD_OUT "Files:\n";
    print CMD_OUT "\t$p4file\n";
    
    close (CMD_OUT);

    open (FILE_IN, "< tmpout.txt") or return 0;

    while (<FILE_IN>)
    {
        if (/.*submitted\.$/)
        {
            $rc = 1;
        }
#        if (/^Change\s(\d+)\screated/)
#        {
#            $clist = $1;
#        }
    }
    
    close (FILE_IN);
    unlink ("tmpout.txt");
    
#    return 0 unless ($clist > 0);  

#    open (CMD_IN, "p4 -p $p4port -u $p4user -c $p4client submit -c $clist |") or return 0;
    
#    while (<CMD_IN>)
#    {
#        if (/.*submitted\.$/)
#        {
#            $rc = 1;
#        }
#    }
#    close (CMD_IN);
    return $rc;   
}