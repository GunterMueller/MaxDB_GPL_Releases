#!/usr/bin/perl
# comprc        *** internal script (NOT called by vmake !)
#
# @(#)compj      1997-11-18
#
# Compile java file
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

package compj;

use Env;
use File::Basename;
use File::Copy;
use ICopy;


do "$TOOLVARS";
if ( $@ ) { print "$@"; exit }
ToolTrace("compj called with @ARGV\n");


$USAGE = "usage: compj [java options] javafile\n";
@FLAGS=();


while ($_ = $ARGV[0], /^-/)
{
	shift;
	last if /^--$/;
	if(/^-output=(.*)$/) { push @output, $1 ; next }

	if(/^-classpath=(.*)$/) { push @classpath, $1 ; next }

	if(/^(-.*)/) { push @FLAGS, $1, next}
} #while


$SOURCE = shift;

@CCMD = ("javac");
if (@classpath)
{
	push @CCMD, "-classpath" , join( ";", @classpath )
}

push @CCMD, @FLAGS, $SOURCE;

system(@CCMD) == 0
         or die "Error while executing \"@CCMD\"\n".
         "message: $!\n";
