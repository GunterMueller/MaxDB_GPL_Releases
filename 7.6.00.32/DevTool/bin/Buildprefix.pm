#!/usr/bin/perl
#
# Buildprefix.pm
#
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


package Buildprefix;

sub set_buildprefix {

	return unless ($ENV{RELVER} ge "R72");


	my $gsp100h = $ENV{OWN}."/sys/wrk/incl/gsp100.h";
	my $buildprefix = undef;

	if (-r $gsp100h) {
		open (IN, $gsp100h);
		while (<IN>) {
			chomp;
			if ($_ =~ /define\s+BUILD_PREFIX_SP100\s+(\d+)/) {
				$buildprefix = $1;
				last;
			}
		}
		close (IN);
	}

	if (defined $buildprefix) {
		$ENV{BUILDPRAEFIX} = $buildprefix;
		return;
	}

	my $txt = "";
	$txt .= "cannot get BUILDPRAEFIX\n";
	$txt .= "please make sure that ".$gsp100h." is readable\n";
	$txt .= "or set BUILDPRAEFIX\n";
	die $txt;
}

1;

