#!/usr/bin/perl
#
# 1998-04-17 cba
#
# 2000-08-31 &gar (link without complete path for nfs mounted tools)
#
# 2000-10-18 cba : moved body to instperltools.pl,
#                  PlAway.pl only for backward compatibility
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


if ( @ARGV < 1 ) {
        die "usage: PlAway <directory>\n";
}

do "instperltools.pl";
