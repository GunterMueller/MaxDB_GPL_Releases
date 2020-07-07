#!/usr/bin/perl
# odbclnk
#
# @(#)odbclnk  7.2  2001-05-29
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

package odbclnk;
use Env;
use File::Basename;
#$Trace=1
$Trace++ if ($Trace);
$\="\n";  # Output seperator fuer print */
$USAGE="USAGE: odbclnk [-b] [linkopts] file [file...]";
if (@ARGV < 1) {
    print $USAGE;
    exit -1;
}

do "$TOOLVARS";

$LNKVER= basename $ARGV[0];
$ENV{LNKVER} = $LNKVER;		# export $LNKVER
  $CC=$LD;
  $options="";
  $OEXT=".o";
  $LEXT=".a";
  $EEXT="";

# Wdf-tests on customer like installation
if (defined $ENV{INDEPPATH}) {
    $INSTROOT=$ENV{INDEPPATH};
}

# odbclibrary
if (defined $ENV{BIT64}) {
  $PCSQLDB="$INSTROOT/lib/lib64/libsqlod".$LEXT;
}
else {
  $PCSQLDB="$INSTROOT/lib/libsqlod".$LEXT;
}

#if libefence exists, add to lib list
$efence="$ENV{OWN}/test/pc/wrk/libefence.a";
if (-e $efence)  {
    $PCSQLDB=$PCSQLDB." ".$efence." -lpthread";
}
else {
    $PCSQLDB=$PCSQLDB." -lpthread";
}

# system librarys and options



$options.="$LFLAGS $LIBS";

while ($_ = $ARGV[0], /^[-\/]/) {
    shift;
    last if /^--$/;
    if ( /^-b$/ ) {
        $LNKVER .= " -b";
        next;
    }
    if ( /^-iodbc$/ ) {
        $PCSQLDB="$INSTROOT/lib/libiodbc".$LEXT;
        next;
    }
    else {
        $options .= " $_";
        next;
    }
}

print "\$options=$options" if ( $Trace );


$exec="$ARGV[0]$EEXT";
print "exec=$exec" if ( $Trace );

$files="";
if (  -f "$ARGV[0]$OEXT" )
{
#    $main="$ARGV[0]";
    $files="$ARGV[0]$OEXT";
}
print "main=$main" if ( $Trace );

shift;

foreach $i (@ARGV) {
    if ( -f "$i$OEXT" ) {
	$files .= " $i$OEXT";
    }
    elsif ( -f "$i$LEXT" ) {
	$files .= " $i$LEXT";
    }
    elsif ( -f "$i.c" ) {
	$files .= "$i.c";
    }
    else {
	$files .= " $i";
    }
}
print "files=$files" if ( $Trace );

unlink $exec if ( -f "$exec" );

if ( "$PCSQLDB" ne "" ) {
    $CMD="$CC -o $exec $main $files $options $PCSQLDB ";
    print $CMD;
    system ($CMD);
}
else {
    print "Use sqllist.";
}
print "odbclnk $exec finished."
__END__
