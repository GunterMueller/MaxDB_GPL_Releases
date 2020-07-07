#! /usr/bin/perl
#
#    RequestInput.pm - Asks for user input.
#
#    @(#)RequestInput.pm     2003-04-23
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

package Tool::RequestInput;

require Exporter;

@ISA    = qw(Exporter);
@EXPORT = qw(RequestInput);

use strict;

##############################################################################
# RequestInput() - Asks for a user input
##############################################################################

sub RequestInput
{
    my ($outstr,$defval,$inpmask,$errstr) = @_;
    my $tempval;
    my $err_count = 0;

    if ($outstr)
    {
        print $outstr;
    }

    unless ((!$defval) || ($defval =~ /^$inpmask$/))
    {
        print STDERR "\nRequestInput: The input mask doesn't seem to be appropriate! ($inpmask)\n";
        exit(20);
    }

    $tempval = <STDIN>;
    chomp $tempval;

    until (($tempval =~ /^$inpmask$/) or ($defval and ($tempval =~ /^$/)))
    {
        $err_count += 1;
        print $errstr.$outstr;
        $tempval = <STDIN>;
        chomp $tempval;
        if ($err_count >= 10) {
            print "\nThere have been too many false inputs! Exiting...\n\n";
            exit(1);
        }
    }

    if ($tempval =~ /^$/)
    {
        $tempval = $defval;
    }

    return $tempval;
}

1;
