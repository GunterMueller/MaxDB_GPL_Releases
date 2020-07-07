#!/usr/bin/perl
#
# @(#)suts			1997-11-18
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

use Env;
use Getopt::Std;

if ( !getopts('d:n:R:') ) { print "Usage: suts [-n <node> -R <remote root>] [-d <dbname>] test\n"; exit }



@ARGV = ("slow", @ARGV, $opt_d);
@ARGV = ("-R $opt_R", @ARGV) if ( $opt_R );
@ARGV = ("-n $opt_n", @ARGV) if ( $opt_n );
do "$TOOL/bin/sut$TOOLEXT";
if ( $@ ) { die "Error while executing sut:\n", "$@"; exit }