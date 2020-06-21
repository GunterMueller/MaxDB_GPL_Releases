#! /usr/bin/perl
#
#    DelTree.pm - Deletes a complete directory tree safely.
#
#    @(#)DelTree.pm     2003-04-22
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

package IO::DelTree;

require Exporter;

@ISA    = qw(Exporter);
@EXPORT = qw(DelTree);

use strict;

##############################################################################
# DelTree() - Deletes a complete directory tree recursively.
##############################################################################

sub DelTree
{
    my ($dir, $verbose) = @_;
    my @dirlist;
    my $pathsep = ($^O =~ /MSWin32/i) ? "\\" : "/";
    my $delcmd  = ($^O =~ /MSWin32/i) ? "del /S /Q" : "rm -f -R";
    my $postcmd = ($^O =~ /MSWin32/i) ? ">NUL" : ">/dev/null";
    my $rc = 0;

    unless ($dir) { die "DelTree: ERROR! First parameter (file name) is not set!\n" }

    unless (opendir (DIR, $dir)) { print STDERR "DelTree: Can't open $dir for reading!\n$!\n"; return; }
    @dirlist = readdir(DIR);
    unless (closedir DIR) { print STDERR "DelTree: Can't close $dir!\n$!\n"; return; }

    foreach my $entry (@dirlist)
    {
        next if (($entry eq ".") or ($entry eq ".."));
        if(-d $dir.$pathsep.$entry)
        {
            DelTree ($dir.$pathsep.$entry, $verbose);
        }
        else
        {
            chmod 0777, $dir.$pathsep.$entry;
        }
    }

    print "Removing: $dir\n" if $verbose;
    $rc += system("$delcmd $dir" . ($verbose ? "" : $postcmd));
    rmdir $dir;
    return (($rc > 0) ? 0 : 1);
}

1;
