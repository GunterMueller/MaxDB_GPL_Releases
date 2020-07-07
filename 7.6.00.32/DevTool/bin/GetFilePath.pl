#!/usr/bin/perl
#
# @(#) GetFilePath   10-11-1997
#
# G. Groﬂmann, SAP, AG
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
use ICopy;

$Usage = "usage: GetFilePath -h -l[0-9][x] [-q] file\n";
getopts('hql:');

if ( @ARGV == 0 && !defined($opt_h))  {
	die $Usage;
}

if ( $opt_h ) {
	print <DATA>;
	exit 0;
}

if ( defined($opt_l) ) {
	die $Usage unless $opt_l =~ /^(\d)x?$/i;
	$level = $1; $exclusive = 1 if $opt_l =~ /x$/i;
}
$level = 0 unless defined($level);
$exclusive = 0 unless defined($exclusive);
if ( defined($opt_q) ) { $quiet = 1 } else { $quiet = 0 }
$source = $ARGV[0];

$Source_completed = ICopy::GetFilePath($source, $level, $exclusive, $quiet);
print $Source_completed unless $quiet;
exit($Source_completed ? 0 : 1);

__DATA__

usage: GetFilePath -h -l[0-9][x] [-q] file

Optionen:
	-l[0-9]  : es wird ab dem [0-9]. Pfad gesucht

	-l[0-9]x : es wird NUR in dem [0-9]. Pfad gesucht (exclusive)

    -q       : quiet (nur der Returncode ist von Interesse )

Source-Angaben werden auf jedem Pfad im %VMAKE_PATH%
folgendermaszen gesucht :

mit Pfadangabe:
   Modulname             -> Pfad/Modulname
   Datei mit/ohne Endung -> Pfad/Datei
   eindeutige rel. Pfade werden expandiert nach folgenden Regeln:
      1. {fast|quick|slow} -> sys/desc/{fast|quick|slow},
      (bspw. fast/cpc.mac -> sys/desc/fast/cpc.mac)
      2. [a-z][a-z] -> sys/src/[a-z][a-z]
      (bspw. wx/Demo.ico -> sys/src/wx/Demo.ico)

keine Pfadangabe:
   Modulname              -> sys/src/[a-z][a-z]/Datei
   Datei mit !Desc-Endung -> %LAY%/Datei,
                             sys/src/Extra-Layer/Datei
   Datei mit Desc-Endung  -> sys/desc/{fast|quick|slow}/Datei
   Datei "Extra"          -> sys/desc/{fast|quick|slow}/Extra
   Datei ohne Endung      -> %LAY%,
                             sys/desc/{fast|quick|slow}/
                             Datei.{mac|shm|lnk|dld|shr|rel|lib}
