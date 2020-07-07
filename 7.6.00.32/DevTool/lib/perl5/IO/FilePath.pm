#! /usr/bin/perl
#
#    HashRef.pm - Implements reading / writing simple config files.
#
#    @(#)HashRef.pm     2003-04-22
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

package IO::FilePath;

require Exporter;

@ISA    = qw(Exporter);
@EXPORT = qw(JoinPath FixPathSep InterpolatePath);

use strict;

##############################################################################
# JoinPath() - Concatinates two or more strings into one.
##############################################################################

sub JoinPath
{
    my $outpath = shift;
    my $part    = shift;

    while ($part)
    {
        if ($outpath)
        {
            $outpath  = InterpolatePath($outpath);
            $outpath .= "/" . $part;
        }
        else
        {
            $outpath = $part;
        }

        $part = shift;
    }

    return FixPathSep ($outpath);
}

##############################################################################
# FixPathSep() - Replaces all path separators with the correct one.
##############################################################################

sub FixPathSep
{
    my $fixstr = shift;

    unless ($fixstr) { return "" }

    if ($^O =~ /MSWin32/i)
    {
        $fixstr =~ tr/\//\\/;
    }
    else
    {
        $fixstr =~ tr/\\/\//;
    }

    return $fixstr;
}

##############################################################################
# InterpolatePath() - Checks a path for a trailing "/" and removes it
##############################################################################

sub InterpolatePath
{
    my $path = shift;

    if (($path =~ /^(.*)\/$/) or ($path =~ /^(.*)\\$/))
    {
        $path = $1;
    }

    return $path;
}
