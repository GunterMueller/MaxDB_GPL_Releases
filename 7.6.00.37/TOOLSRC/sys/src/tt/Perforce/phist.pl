#!/usr/bin/perl
#
# G. Arnold 
#  SAP AG
#####################
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
use Getopt::Std;
use PTools;
use File::Basename;

$SCRIPTNAME="PHIST";

getopts('h');


if ( @ARGV == 0 && !defined($opt_h))  {	Usage();}


@failed_modules=();

$rc=PTools::phist( \@ARGV, \@failed_modules);

caller() ? ($rc != 0) : exit($rc != 0) ;


sub Usage {
  print <<End;
usage: phist files
End
  exit 1;
}
