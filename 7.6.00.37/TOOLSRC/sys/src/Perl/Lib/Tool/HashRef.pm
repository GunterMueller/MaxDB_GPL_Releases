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

package Tool::HashRef;

require Exporter;

@ISA    = qw(Exporter);
@EXPORT = qw(CopyHashRef ShowHashRef);

use strict;

##############################################################################
# CopyHashRef() - Copies one hash reference into another.
##############################################################################

sub CopyHashRef
{
    my ($src, $dest) = @_;

    #unless (ref($src))  { die "CopyHashRef: ERROR! First parameter (source reference) is not set correctly!\n" }
    #unless (ref($dest)) { die "CopyHashRef: ERROR! Second parameter (dest. reference) is not set correctly!\n" }

    foreach my $key (keys %{$src})
    {
        $dest->{$key} = $src->{$key};
    }
}

##############################################################################
# ShowHashRef() - Displays a hash reference.
##############################################################################

sub ShowHashRef
{
    my $ref = shift;
    my $header = shift;
    my $do_print = shift;
    my $ret = "";

    unless (ref($ref)) { die "ShowHashRef: ERROR! First parameter (hash reference) is not set correctly!\n" }

    $do_print = 1 unless (defined $do_print);

    foreach my $key (sort keys %{$ref})
    {
        unless (ref($ref->{$key}))
        {
            $ret .= "$header$key=$ref->{$key}\n"; 
            print "$header$key=$ref->{$key}\n" if ($do_print);
        }
    }

    $ret .= "\n";
    print "\n" if ($do_print);

    foreach my $key (sort keys %{$ref})
    {
        if (ref($ref->{$key}) eq "HASH")
        {
            $ret .= "$header>>$key:\n\n";
            print "$header>>$key:\n\n" if ($do_print);
            $ret .= ShowHashRef ($ref->{$key}, ($header . "    "), $do_print);
        }
        elsif (ref($ref->{$key}) eq "ARRAY")
        {
            $ret .= "$header>>$key:\n\n";
            print "$header>>$key:\n\n" if ($do_print);
            my $count = 0;
            foreach my $elem (@{$ref->{$key}})
            {
                $ret .= "$header\[$count] : $elem\n";
                print "$header\[$count] : $elem\n" if ($do_print);
                $count ++;
            }
        }
    }
    
    return $ret;
}

1;