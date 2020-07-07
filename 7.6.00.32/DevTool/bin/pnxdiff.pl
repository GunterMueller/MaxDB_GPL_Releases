
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


if ( @ARGV == 0 ) {
  print "Aufruf: pnxdiff <name> ...\n";
  print "        pnxdiff C05TIME.punix\n";
  print "\n";
  print "In Teststroemen mit zu erwartenden Standard-Differenzen sind\n";
  print "einleitende Kommentare formuliert, die - falls sie nicht im Punix\n";
  print "stehen - ins pdiff uebernommen werden; dadurch geben sie etwas\n";
  print "Auskunft ueber die Art der Differenz.\n";
  print "Sie fangen alle mit \" *++ \" an, so dass dieses Kommando sie aus\n";
  print "einem evtl. neu (durch Umbenennung) erzeugten File entfernt!\n";
  exit 10;
}


$Status = 0;

foreach $File (@ARGV) {
  if ( -f "$File" ) {
    open (INFILE,"<$File"); open (OUTFILE,">$File.$$");
    while (<INFILE>) {
      print OUTFILE $_ unless /^ \*\+\+ /;
    }
    close (INFILE); close (OUTFILE);
    unlink ("$File"); rename ("$File.$$", "$File");
  }
  else {
    $Status = 2;
  }
}


exit $Status;
