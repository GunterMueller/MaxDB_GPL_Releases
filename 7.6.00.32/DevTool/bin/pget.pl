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


$SCRIPTNAME="PGET";

getopts('mxvse:');

if ( @ARGV == 0 && !defined($opt_h))  {Usage();}

if ($opt_s && $opt_v)
{
	die "Error-$SCRIPTNAME: Option -v with -s not allowed\n";
}

$verbose=$opt_v ? 2 : $opt_s ? 0 : 1;

$exclusive = $opt_x;
if ($opt_m && $opt_x)
{
	die "Error-$SCRIPTNAME: Option -x with -m not allowed\n";
}

$excl_get =  $exclusive  ? 1 : 0 ;

@failed_modules=();
$rc=PTools::pget(\@ARGV, $excl_get, $opt_e, \@failed_modules, $verbose );

caller() ? ($rc != 0) : exit($rc != 0) ;


sub Usage {

  print <<End;
usage: pget [-m|x] [-e editor] [-v|-s] files";
         -v verbose
         -s silent
         -x checkout exclusive
The default checkout modus is exclusive. If the environment variable MGET 
is set the default modus is shared. 
End
  exit 1;
}