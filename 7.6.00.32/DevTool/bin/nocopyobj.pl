#!/usr/bin/perl
# nocopyobj.pl
#
# @(#)src  7.2  1999-03-17
#
# Modifiziert den in $VMAKE_PATH angegebenen Pfad, so das keine Objektfiles
# vom Server kopiert werden. Dazu wir eine Datei (nocpyobj.bat) erstellt,
# die den notwnedigen Befehl zum setzten des Pfades beinhaltet.
# In das Startup Batchfile (iuser.bat) muessen dann folgende Zeilen eingefuegt
# werden, um das kopieren der Objektdateien zu verhindern.
#
# nocopyobj.pl
# if exist nocpyobj.bat (call nocpyobj.bat & rm nocpyobj.bat)
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

use Env;
use ICopy;
local @vmakepath = ICopy::_GetPathList($VMAKE_PATH);
foreach $vmakepath ( @vmakepath ) {
    if (!($vmakepath =~ m!(.*//$)!) ) {
	if ( ! $_vmakepath ) {
	    $_vmakepath .= "$vmakepath";
	}
	else {
	    $_vmakepath .= ",$vmakepath//";
	}
    }
}
$ENV{VMAKE_PATH} = $_vmakepath;		# can be exported even if not set
    push(@EXPORT, $ENV{VMAKE_PATH});
    local $Exporter::ExportLevel = 1;
    import Getopt::Std;
    open(FILE_OUT, ">nocpyobj.bat") || die "Can't open nocpyobj.bat (output): $!";
    print FILE_OUT "set vmake_path=$_vmakepath";
    system("nocpyobj.bat");
    close FILE_OUT;
