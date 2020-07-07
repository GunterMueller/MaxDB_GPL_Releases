#!/usr/bin/perl
# pcxuser
#
# @(#)pctest  6.2  1998-01-09
#
# Generates the default XUSER Entry
#
# Burkhard Diesing
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

package pcxuser;
use PCTestTools;
local $RELTOOL = PCTestTools::getPathOf("PCTest.pm");
if ( "$RELTOOL" ne "") {
    @INC = ("$RELTOOL", @INC);
    do "$RELTOOL/pcxuser.pl" or die "ERROR: $0 $!";
    exit;
}
use Env;
use CPCTest;
use Getopt::Std;
use ICopy;
$Trace=1; # Switch to 1 to enable trace

if (@ARGV < 2 || !getopts ('hc:') || $opt_h ) {
  print <DATA>;
  exit;
}

print "@ARGV" if ($Trace);

if ($opt_c) {
# Get configurations from xml file
    require PCConf;
    import PCConf;
    $configfile = $opt_c;
    my $configuration = new PCConf::Configuration $configfile;
    die "No databases in configuration found." if ($configuration->countDB <= 0);
    my $database = $configuration->getDatabase;
    local ($serverdb) = $database->DBName;
    local ($servernode) = $database->DBHost;
    for ($database->enumUser) {
        if ($_->Kind eq "TEST") {
            $uid = $_->UID;
            $pwd = $_->PWD;
        }
    }
    $temp=$\;$\="\n";
  CPCTest::XUserMakeData( "$uid", "$pwd", "$serverdb", "$servernode" );
    ;$\=$temp;
}
else {
    $uid=$ARGV[0];
    $pwd=$ARGV[1];
    $temp=$\;$\="\n";
  CPCTest::XUserMakeData( "$uid", "$pwd" );
    ;$\=$temp;
}

__DATA__

 USAGE: pcxuser [-h] | -c<config> | [uid pwd]

           Generates the default XUSER entry.

 ARGUMENTS:
   -h           : Prints this help
   -c <config>  : Filename of the XML file with database configuration.
                  If ommited uid and pwd has set by argument serverdb
                  and servernode has to set by environment.
