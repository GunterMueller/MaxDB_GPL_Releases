#!/usr/bin/perl
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

package PCTestTools;

use Env;
my $PCTestPath = getPathOf("PCTest.pm");
if ( "$PCTestPath" ne "") {
    @INC = ("$PCTestPath", @INC);
}
require PCTest;
import PCTest;

require CPCTest;
import CPCTest;

require PCConf;
import PCConf;

require Purify;
import Purify;

$Trace = 1 if ($ENV{PCTOOL_TRACE});

sub getPathOf
{
    local $name = shift;
    local @dirs;
    push (@dirs, "$ENV{OWN}/test/pc") if (defined $ENV{OWN});
    push (@dirs, "$ENV{TEST_ROOT}/../test/pc") if (defined $ENV{TEST_ROOT});
    push (@dirs, "$ENV{JTEST_ROOT}/../../test/pc") if (defined $ENV{JTEST_ROOT});
    foreach my $dir (@dirs) {
        if (-f "$dir/bin/$name") {
            print "Found $name in $dir/bin.\n" if ($Trace);
            return "$dir/bin";
        }
    }
    return ""
}

1;
