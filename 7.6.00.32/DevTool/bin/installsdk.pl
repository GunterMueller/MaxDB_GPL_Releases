#!/usr/bin/perl
# pcmktest
#
# @(#)installsdk
#
# extract a car archive (contains a softwaredevelopmentkit) into a
# releasetree. This archive can be generated with pccar.pl because
# if a arcive contains binarys it is OS dependend. The name of the
# OS is automaticly append using $MACH and OSSPEC.
#
#
# Burkhard Diesing
#
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

package installsdk;
use Env;

$Trace=1 if ($ENV{PCTOOL_TRACE});
$CMD_EXTRACTCAR = "SAPCAR -xvf ";
$ARCHIVEDIR="$ENV{OWN}/sys/wrk/fast/obj/sdk";
$INSTDIR="$ENV{OWN}/sys/wrk/sdk";
sub _mkdir;

$USAGE ="USAGE: installsdk <archivename>\n";
if (@ARGV < 1 ) {
  print $USAGE;
  exit -1;
}

my ($ARCHIVE) = @ARGV;
print $INSTDIR if ($Trace);
_mkdir ("$INSTDIR");
chdir $INSTDIR;

if (defined $ENV{BIT64}) {
    $ARCHIVE="${MACH}_${OSSPEC}_64.$ARCHIVE";
}
else {
    $ARCHIVE="${MACH}_${OSSPEC}_32.$ARCHIVE";
}

system ("$CMD_EXTRACTCAR $ARCHIVEDIR/$ARCHIVE\n");

sub _mkdir
{
    if (! -d $_[0] ) {
	print "_mkdir $_[0]" if ( $Trace );
	mkdir ($_[0], 0775);
    }
}

