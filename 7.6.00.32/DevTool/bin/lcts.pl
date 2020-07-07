#!/usr/bin/perl
#
# @(#)lct(s/q/f)		1999-02-25   Holger Becker
#
# LiveCache Tests
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

if ( !getopts('htd:') || $opt_h )
{
    print "Usage: $0 [-d <dbname>] <testfile>\n";
    exit 1;
}


chdir("$TEST/kern/LiveCache/tool");

CASE : {

    if ( $0 =~ /lcts/i ) { @ARGV = ("-v slow", @ARGV);  last CASE; }
    if ( $0 =~ /lctq/i ) { @ARGV = ("-v quick", @ARGV); last CASE; }
    if ( $0 =~ /lctf/i ) { @ARGV = ("-v fast", @ARGV);  last CASE; }

}

@ARGV = ("-d $opt_d", @ARGV) if ( $opt_d );

@ARGV = ("-t", @ARGV) if ( $opt_t );


do "lctsingle$TOOLEXT";
if ( $@ ) { die "Error while executing $0:\n", "$@"; exit }
