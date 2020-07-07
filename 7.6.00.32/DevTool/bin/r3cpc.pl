#!/usr/bin/perl
#
# @(#)r3cpc		1997-11-18
#
# G. Gro�mann, SAP AG
#
# 1998/04/22 cba: ntr3cpc.pl.tt und rc3cpc.r3t als r3cpc.pl.tt
#                 wieder zusammengef�hrt
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

package r3cpc;
use Env;
use Carp;


my $DBROOT;
$DBROOT = $ENV{"DBROOT"};
chdir $DBROOT."/sap" ||
        die "Can't change directory to '$DBROOT/sap': $!";
system "cpc -H nocheck -S SQL-DB altkey"  ||
        die "Can't execute: cpc (...) altkey : $!";
system "cpclnk altkey" ||
        die "Can't execute: cpc (...) altkey : $!";
unlink <altkey.*>;



__END__
