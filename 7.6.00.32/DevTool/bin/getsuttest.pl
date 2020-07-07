#!/usr/bin/perl
#
# @(#) getsuttest			1997-11-18
#
# G. Groﬂmann, SAP AG
#
# Kopiert alle Files, die zum Lauf eines SUT benoetigt werden,
# in das Verzeichnis $SUT. Dabei werden keine Versionseintraege
# geschrieben.
# Die einzelnen Komponenten werden dabei in der Hierarchie gesucht,
# die der Vmake-Pfad vorgibt.
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
use File::Copy;
use ICopy;
use Getopt::Std;

if ( !getopts('hu') || $opt_h || (@ARGV != 1) )
{
    print "\nUsage: $0 [-u] <testfile>\n\n";
    print "\tcopies <testfile> from GRP with reference and cmd files\n";
    print "\n\t\t-u ::= copies also unicode reference files\n";
    exit 1;
}

$SutTest = uc($ARGV[0]);
$SutTest = $1 if $SutTest =~ /(.+)\.VDNTS/;

chdir($SUT) || die "Can't change to directory $SUT: $!\n";

%TestFiles = ();

# Teststrom und Referenzprotokolle in sut.mac suchen
ICopy::icp("sut.mac", "$TMP", 0, 0, 1);
open(FILE_IN, "$TMP/sut.mac") || die "Can't open $TMP/sut.mac (input): $!\n";
while(<FILE_IN>) {
    $TestFiles{$1} = 1 if /^($SutTest\.(vdnts|punix|tpunx|diffscr))\s*/i
}
close(FILE_IN);

die "Der Teststrom $SutTest existiert nicht!\n" unless $TestFiles{"$SutTest.vdnts"};

if ( $opt_u ) {
    # Referenzprotokolle in sutuni.mac suchen
    ICopy::icp("sutuni.mac", "$TMP", 0, 0, 1);
    open(FILE_IN, "$TMP/sutuni.mac") || die "Can't open $TMP/sutuni.mac (input): $!\n";
    while(<FILE_IN>) {
        $TestFiles{$1} = 1 if /^($SutTest\.(upnix|cunix|cpnix|uunix))\s*/i
    }
    close(FILE_IN);
}

# CMD-Files ermitteln
ICopy::icp("$SutTest.punix", "$TMP", 0, 0, 1) || die "Das Protokoll $SutTest.punix existiert nicht!";
open(FILE_IN, "<$TMP/$SutTest.punix") || die "Can't open $TMP/SutTest.punix (input): $!\n";
while(<FILE_IN>) {
	local $file;
	$TestFiles{"$1.cmd"} = 1 if ( /^FILE\s+(\w+)/ );
}
close(FILE_IN);

foreach $File (keys %TestFiles) {
    print "FILE: $File\n";
	ICopy::icp("$File", "$SUT", 0, 0, 1) || warn "Die Datei $File ist nicht vorhanden!";
}

