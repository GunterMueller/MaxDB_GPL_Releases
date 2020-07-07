#!/usr/bin/perl
# @(#) sed_term	3.1.3   1993-12-14   kern   admin
#
# Aus den Protokollen werden die Fehlerpositionen herausgeschnitten.
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


open(FILE_IN, "<$ARGV[0]") || die "Can't open input file$ARGV[0]: $!";
open(FILE_OUT, ">$ARGV[1]");
while(<FILE_IN>) {
	if ( /\*\*\*ERROR/ ) {
		s/[0-9][0-9][0-9][0-9]$//g;
	}
	next if /^BIGCMD/;
	print FILE_OUT $_;
}
close(FILE_IN); close(FILE_OUT);
