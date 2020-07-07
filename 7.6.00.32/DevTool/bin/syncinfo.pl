#!/usr/bin/perl
#
# syncinfo - perl script to determine the highesr synced change number
#
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

use Env;
use p4infolib;

unless (open (OUT,">$OWN/syncinfo.dat")) {
    warn "can't open file $OWN/syncinfo.dat\n";
    exit -1;
}

my $maxchangenr = p4infolib::GetMaxChangeNr();
print OUT "<maxchangenr>$maxchangenr</maxchangenr>\n";

my @localsrc = p4infolib::GetLocalSource();
foreach my $src (@localsrc) {
    print OUT "<local>$src</local>\n";
}

my @notathead = p4infolib::GetNotAtHeadList();
foreach my $src (@localsrc) {
    print OUT "<outdated>$src</outdated>\n";
}
close (OUT);


