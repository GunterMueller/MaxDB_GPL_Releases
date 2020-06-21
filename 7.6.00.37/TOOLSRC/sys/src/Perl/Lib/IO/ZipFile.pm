#!/usr/bin/perl
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

package IO::ZipFile;

require Exporter;

@ISA    = qw(Exporter);
@EXPORT = qw(UnzipFile);

use strict;

use Archive::Zip;

sub UnzipFile
{
    my ($fname, $destroot, $verbose) = @_;
    my $rc = 1;
    my $zipfile = Archive::Zip->new();
    
    if ($zipfile->read($fname))
    {
        print "Failed to open file '$fname'!\n";
        return undef;
    }
    
    my @members = $zipfile->memberNames();
    
    foreach my $file (@members)
    {
        print "File: $file\n" if ($verbose);
        if ($zipfile->extractMember($file, ($destroot ? "$destroot/$file" : $file)) != 0)
        {
            print "Failed to extract file '$file'!\n";
            $rc = 0;
        }
    }
    return $rc;
}
