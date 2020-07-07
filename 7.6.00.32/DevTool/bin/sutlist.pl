#!/usr/bin/perl
# @(#) sutlist    29.01.1998  h.b.
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

if ( !getopts ('hd:v:l:') or ($opt_h) ) {

  print <DATA>;
  exit 1;
}

if ( ! $opt_v =~ /(quick|slow|fast)/ ) {

    print "invalid kernel version!\n";
    exit;
}
my $cmd = $opt_v ? lc $opt_v : "slow";
$cmd =~ s/^(.).*$/sut$1/;

my $db = $opt_d ? $opt_d : "$SERVERDB";

my $ListFile = $opt_l ? $opt_l : "test.lst";

chdir($SUT) || die "can't find SUT directory";

unless ( open (INFILE, "<$ListFile") ) {

  print "Can't open $ListFile";
  exit 1;
}


while (<INFILE>) {

  chop;
  if ( /^[^#].*/ )
  {
    s/(.*)\.vdnts/$1/;
    system ("$cmd -d $db $_.");
  }

}

__END__


 ruft sutsingle mit einer Liste von Teststr�men aus einer Datei auf.
 Wird kein Dateiname beim Aufruf angegeben, wird "$SUT/test.lst" genommen.

USAGE:

 sutlist [-d <dbname>] [-v slow|quick|fast] [-l <filename>]

Option:

     -d <dbname>        ::= Name der Datenbank
     -v slow|quick|fast ::= Kern Version
     -l <filename>      ::= Liste der zu testenden Teststr�me

     in der Liste k�nnen Testfiles mit '#' am Zeilenanfang auskommentiert
     werden
