#!/usr/bin/perl
# @(#) sed_sut-lvl   3.1.3   1993-08-13   kern   admin
#
# sed fuer die Behandlung der CONNECT-Zeilen bei ISOLATION LEVEL x<> 1
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
use File::Copy;

$lIsoLevel = $ARGV[0];
$lProtFile = $ARGV[1];
$lTmpFile  = "$TMP/lvl.$$";

copy($lProtFile, $lTmpFile);

open(FILE_IN, "<$lTmpFile");
open(FILE_OUT, ">$lProtFile");

while(<FILE_IN>) {
    s/^CONNECT (.*) IDENTIFIED BY (.*) ISOLATION LEVEL [0-5]+ !/CONNECT $1 IDENTIFIED BY $2 ISOLATION LEVEL 1 !/;
    print FILE_OUT $_;
}

close(FILE_IN);
close(FILE_OUT);

unlink ($lTmpFile);


if ( $lIsoLevel == 15 ) {

  $SaveInputSep = $/;

  copy($lProtFile, $lTmpFile);

  open(FILE_IN, "<$lTmpFile");
  open(FILE_OUT, ">$lProtFile");


  $ISO15_Error_String =  "*---> ES ERFOLGT EIN ERNEUTES CONNECT, DAS DEN DURCH FALSCHEN";

  $/ = $ISO15_Error_String;

  while(<FILE_IN>) {

    if ( !eof(FILE_IN)) {
      chomp;
      s/^(.*)CONNECT.*/$1/s;
      print FILE_OUT;
      $/ = "\n";
      <FILE_IN>;
      <FILE_IN>;
      <FILE_IN>;
      <FILE_IN>;
      $/ = $ISO15_Error_String;
    }
    else {
      print FILE_OUT;
    }

  }

  $/ = $SaveInputSep;

  close(FILE_IN);
  close(FILE_OUT);

  unlink ($lTmpFile);

}
