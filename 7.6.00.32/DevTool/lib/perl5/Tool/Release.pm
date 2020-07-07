
#
#    ========== licence begin LGPL
#    Copyright (C) 2003 SAP AG
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
use strict;
use Env;

#
# return true if the release of the current build environment
#    matches the release given as arguments.
# examples: releaseOrLater (7, 4, 3, 10)
#           releaseOrLater (7, 5)
#           releaseOrLater (8)
sub releaseOrLater {
    #
    # always false in TOOL tree
    #
    if (substr ($RELVER, 0, 1) ne 'R') {
        return 0;
    }
    #
    # all numbers not given as arguments default to 0
    #
    my (@earliest) = @_;
    for (my $i = 0; $i < 4; ++$i) {
        if (! defined ($earliest[$i])) {
            $earliest[$i] = 0;
        }
    }
    #
    # get current release from environment
    #
    my @envRelease = &releaseAsTuple;
    #
    # compare releases
    #
    for (my $i = 0; $i < 4; ++$i) {
        if ($envRelease [$i] < $earliest[$i]) {
            return 0;
        }
    }
    return 1;
}

#
# return the release defined in the build environment
#    as a 4 value tuple (major, minor, correction, build)
#
sub releaseAsTuple {
    return (substr ($RELVER, 1, 1), substr ($RELVER, 2, 1), $CORRECTION_LEVEL, $BUILDPRAEFIX);
}

sub releaseString {
    my ($major, $minor, $corr, $build) = @_;
    return "$major.$minor.$corr.$build";
}

1;

