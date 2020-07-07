#!/usr/bin/perl
# @(#)  stack-test   1998-12-29   kern   admin
#
# Ermittlung des Stackbedarfs des Kerns, anhand der Singleusertests.
#
# $DBNAME durch $SERVERDB ersetzt ( 1997-11-18 GG )
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
use File::Basename;
use File::Copy;
use File::Find;
use ICopy;
use Cwd;
use ToolLib;
use Getopt::Std;
require "hostname.pl"; # sonst gibt es Schwierigkeiten auf DEC

# check options
if ( !getopts('had:') || $opt_h )
{
    print <DATA>;
    exit 1;
}

print "\n\n===============================================================\n";
print "               MESSUNG DES STACKVERBRAUCHS\n";
print "===============================================================\n\n";

print "Optionen:\n\n";
print "-a -> test all\n" if ($opt_a);
print "-d $opt_d\n" if ($opt_d);
print "\n";


# get serverdb
$db = $ENV{SERVERDB};
$db = $opt_d if $opt_d;

if ( ! $db )
{
    print "\$SERVERDB not set -- use '-d <dbname>'\n";
    exit 1;
}

$|=1; # print wird mit <CR> abgeschlossen



local $OldDir     = cwd();
local $SPEED	  = "$OWN/test/kern/sut/stack";
local $AllStack   = "$TEST/kern/allstack.prt";
local $SAVE	  = "$OWN/test/kern/sut/stack-save";
local $DATE       = "0"x(2-length((localtime)[3])).(localtime)[3].
    "0"x(2-length(((localtime)[4])+1)).((localtime)[4]+1); #DDMM

chdir($SUT) || warn "Can't change to directory $SUT: $!\n";

# Aktivieren der Stackmessung:
# Abschalten des default Vtrace und Einschalten von order standard.
# Letzteres schreibt den Header und den Anfang des Var_parts
# eines Kernauftrags in den Vtrace.
local @ARGV = ( "-u", "XRESTORE.cmd" );
do "$TOOL/bin/imf$TOOLEXT";
if ( $@ ) { warn "Error while executing imf$TOOLEXT: $@"; }

copy("XRESTORE.cmd", "XRESTORE.orig");
open(FILE_IN, "XRESTORE.orig") || warn "Can't open (input|output|append): $!\n";
open(FILE_OUT, ">XRESTORE.cmd") || warn "Can't open (input|output|append): $!\n";
while(<FILE_IN>) {
    s/^\*\*//;
    print FILE_OUT $_;
} #while
close(FILE_IN);	close(FILE_OUT);

    # Stueckweises loeschen von Testergebnissen da dieses SCO Unix
    # sonst die Meldung 'arg list too long' ausgiebt.
    unlink <*.prot>;
    unlink <*.vprot>;
    unlink <*.sort>;
    unlink <*.core>;
    unlink <*.dump>;


# Bei der Option '-a' werden saemtliche Teststrome behandelt,
# sonst werden nur einige Stack-Tests gemacht, wobei hier eine
# Liste mit relevanten Testfiles aufgebaut wird.
open(FILE_IN, "$TEST/kern/stack_hit") || warn "Can't open $TEST/kern/stack_hit(input): $!\n";
local $Stacktest;
chomp($Stacktest = <FILE_IN>);
close(FILE_IN);
if ( $Stacktest eq "" ) {
    if ( $OWN =~ /V61/i ) { $Stacktest = "C00ERROR ULINK1 NIST4 N00TEST2 N50LINK CORR SAPR3" }
    if ( $OWN =~ /V62/i ) { $Stacktest = "SUBQUERY SNAPSHOT DDLMIX EXPRLIST FOKUS MULFLDS" }
    if ( $OWN =~ /V71/i ) { $Stacktest = "SUBQUERY EXPRLIST FOKUS MULFLDS" }
    if ( $OWN =~ /V72/i ) { $Stacktest = "SUBQUERY EXPRLIST FOKUS MULFLDS" }
    if ( $OWN =~ /V73/i ) { $Stacktest = "SUBQUERY EXPRLIST FOKUS MULFLDS" }
    if ( $OWN =~ /V74/i ) { $Stacktest = "SUBQUERY EXPRLIST FOKUS MULFLDS" }
}

print "STACKTEST:\n";

if ( $opt_a ) {
    print "Alle Teststroeme\n";
    open(FILE_OUT, ">$AllStack") || warn "Can't open $AllStack(output): $!\n";
    print FILE_OUT "XINITDB\n";
    opendir(DIR, ".");
    local @Inp = map  { ($_) = /(.*)\.vdnts/ } grep /^[A-VZ].*\.vdnts/i ,readdir(DIR);
    closedir(DIR);
    foreach $SUTFILE ( @Inp )
    {
	print FILE_OUT "$SUTFILE\n";
    }
    print FILE_OUT "YINIT\n";
    opendir(DIR, ".");
    @Inp = map  { ($_) = /(.*)\.vdnts/ } grep /^[Y].*\.vdnts/i ,readdir(DIR);
    closedir(DIR);
    foreach $SUTFILE ( @Inp )
    {
	print FILE_OUT "$SUTFILE\n";
    }
    close(FILE_OUT);
}
else {
    open(FILE_OUT, ">$AllStack") || warn "Can't open $AllStack(output): $!\n";
    print FILE_OUT "XINITDB\n";
    foreach $ST ( split / /, $Stacktest )
    {
	print "$ST\n";
	print FILE_OUT "$ST\n";
    }
    close(FILE_OUT);
}

local @ARGV = ( "-u", "vta01//STACKCHK" );
do "$TOOL/bin/imf$TOOLEXT";
if ( $@ ) { warn "Error while executing imf$TOOLEXT: $@"; }

if ( $RELVER ge "R74" ) {
  local @ARGV = ( "-u", "SAPDBTrace_MethodBrace.cpp/-DSAPDB_STACKCHK" );
  do "$TOOL/bin/ims$TOOLEXT";
  if ( $@ ) { warn "Error while executing ims$TOOLEXT: $@"; }
}

local @ARGV = ( "slowknl" );
do "$TOOL/bin/ims$TOOLEXT";
if ( $@ ) { warn "Error while executing ims$TOOLEXT: $@"; }

open(FILE_IN, "$AllStack") || warn "Can't open $AllStack(input): $!\n";
local @AllStack = <FILE_IN>;
close(FILE_IN);
chop (@AllStack);
map { $_ .= ".vdnts" } @AllStack;

local @ARGV = ( "-stack" , "slow", "slowci", "$db", @AllStack );
print "calling sutsingle with:\n@ARGV\n";
do "$TOOL/bin/sutsingle$TOOLEXT";
if ( $@ ) { warn "Error while executing sutsingle$TOOLEXT -stack: $@"; }

if ( ! -d $SAVE ) { system("mkdir $SAVE") }

# Der Stackverbrauch jedes einzelnen Befehls wird in einer Liste
# gespeichert. Die Dateien mit dem Suffix 'sort' enthalten den
# Stackverbrauch fuer jeden Befehl des entsprechenden Testfiles.
print "\n>>>  Jetzt wird Stacksort aufgerufen\n";

system ("cat $SPEED/*.sort > $SPEED/allstack");
if ( $@ ) { warn "Error while executing  cat $SPEED/*.sort: $@"; }

system ("$TOOL/Posix/sort", "-r", "-o", $SPEED."/stack_all", $SPEED."/allstack");

# Die Stackmessung wieder abschalten.
local @ARGV = ( "-u", "vta01" );
do "$TOOL/bin/imf$TOOLEXT";
if ( $@ ) { warn "Error while executing imf$TOOLEXT: $@"; }

if ( $RELVER ge "R74" ) {
  local @ARGV = ( "-u", "SAPDBTrace_MethodBrace.cpp" );
  do "$TOOL/bin/ims$TOOLEXT";
  if ( $@ ) { warn "Error while executing ims$TOOLEXT: $@"; }
}

local @ARGV = ( "slowknl" );
do "$TOOL/bin/ims$TOOLEXT";
if ( $@ ) { warn "Error while executing ims$TOOLEXT: $@"; }

local @ARGV = ( "-u", "XRESTORE.cmd" );
do "$TOOL/bin/imf$TOOLEXT";
if ( $@ ) { warn "Error while executing imf$TOOLEXT: $@"; }

chdir($OldDir) || warn "Can't change to directory $OldDir: $!\n";

__END__

 Ermittelt den verbrauchten TASKSTACK des Kerns innerhalb der SUTs.

 Die zu testenden SUTs stehen in der Datei $TEST/kern/stack_hit.
 Ist diese nicht vorhanden oder leer werden einige Default
 Teststroeme  getestet.

 Die Ergebnisse werden unter $TEST/kern/sut/stack gespeichert.

 Achtung! VTA01, SLOWKNL und XRESTORE.cmd werden neu uebersetzt

Usage:

 stacktest [-d <dbname>] [-a]

Options:

 -d <dbname> -- spezifiziert den Datenbanknamen (default ::= $SERVERDB)
 -a          -- wenn angegeben werden alle SUTs getestet
