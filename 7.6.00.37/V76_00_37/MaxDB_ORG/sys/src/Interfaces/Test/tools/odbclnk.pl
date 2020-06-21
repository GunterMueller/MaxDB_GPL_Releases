#!/usr/bin/perl
# odbclnk 
#
# @(#)odbclnk  7.2  2001-05-29 
#
#    ========== licence begin  GPL
#    Copyright (c) 2001-2005 SAP AG
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
&if $OSSPEC in [ WIN32 ]
  $LINKER=$CC;
  $OEXT=".obj";
  $LEXT=".lib";
  $EEXT=".exe";
&else
&if $OSSPEC in [ OSF1 ]
  $LINKER=cxx;
&else
  $LINKER=$LD;
&endif
  $options="";
  $OEXT=".o";
  $LEXT=".a";
  $EEXT="";
&endif

$odbclibpath=$INSTLIB;

# Wdf-tests on customer like installation
if (defined $ENV{ISWDFNACHT}) {
    $INSTROOT=$ENV{INDEPPATH};
    $odbclibpath=$INDEPLIB;
}

# odbclibrary
&if $OSSPEC in [ WIN32 ]
  $PCSQLDB="odbc32".$LEXT;
&else
  $PCSQLDB=$odbclibpath."/libsqlod".$LEXT;
# -U: use Unicode ODBC driver
foreach $i (@ARGV) {
  if ($i eq "Unicode") {
    $PCSQLDB=$odbclibpath."/libsqlodw".$LEXT;
  }
}
&endif

#if libefence exists, add to lib list
$efence="$ENV{OWN}/test/pc/wrk/libefence.a";
if (-e $efence)  {
&if $OSSPEC in [ LINUX, AIX ]
    $PCSQLDB=$PCSQLDB." ".$efence." -lpthread";
#&else
#    $PCSQLDB=$PCSQLDB." ".$efence;
&endif
} else {
&if $OSSPEC in [ LINUX, AIX ]
    $PCSQLDB=$PCSQLDB." -lpthread";
&endif
}

&if $OSSPEC in [ LINUX ]
    $PCSQLDB=$PCSQLDB." -lrt -lm";
&endif

&if $OSSPEC in [ AIX ]
    $PCSQLDB=$PCSQLDB." -lperfstat";
&endif


# system librarys and options
&if $OSSPEC in [ WIN32 ]
$LFLAGS="-link ";
&if $MACH in [ AMD64 ]  ||  $MACHINE in [ IA64 ]
$LIBS="bufferoverflowu.lib ";
&endif
&endif
&if $OSSPEC in [ OSF1 ]
$LIBS="-lm -lpthread -lrt";
&endif
&if $MACH == SUN || $OSSPEC == SOLARIS
$LIBS="-lm -lnsl -lsocket -lrt -lkstat -ldl";
  if (defined $ENV{BIT64}) {
&if $MACH in [ SPARC, SUN ]
      $LFLAGS="-xtarget=ultra -xarch=v9 ";
&elif $MACH == X86_64
      $LFLAGS="-xtarget=opteron -xarch=amd64 ";
&endif
  }
&endif
&if $MACH = NMP
&if $OSSPEC = SVR4
$LIBS="-lm -lnsl -lsocket";
if (defined $ENV{BIT64}) {
    $LFLAGS="-Kpthread -Klp64";
}
else {
    $LFLAGS="-Kpthread";
}
&else
$LIBS="-lm -lnsl -lsocket -lrt";
&endif
&endif
&if $OSSPEC = HPUX
if (defined $ENV{BIT64}) {
  $LFLAGS="-lpthread -lnsl ";
&if $MACH = HP_IA64
    $LFLAGS=$LFLAGS . "+DD64";
&else
    $LFLAGS=$LFLAGS . "+DA2.0W +DS2.0";
&endif
}
&endif

&if $MACH = SDBonPPC64
$LFLAGS="-m64 -Wl,-melf64ppc";
&endif

$options.="$LFLAGS $LIBS";

while ($_ = $ARGV[0], /^[-\/]/) {
    shift;
    last if /^--$/;
    if ( /^-b$/ ) {
        $LNKVER .= " -b";
        next;
    }
    if ( /^-iodbc$/ ) {
        $PCSQLDB=$odbclibpath."/libiodbc".$LEXT;
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
&if $OSSPEC in [ WIN32 ]
    $files="$ARGV[0]$OEXT";
&else
#    $main="$ARGV[0]";
    $files="$ARGV[0]$OEXT";
&endif
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
    elsif ($i ne "Unicode" ) {
	$files .= " $i";
    }
}
print "files=$files" if ( $Trace );

unlink $exec if ( -f "$exec" );

if ( "$PCSQLDB" ne "" ) {
&if $OSSPEC in [ WIN32 ]
    $CMD="$LINKER $main $files $options $PCSQLDB ";
&elif $MACH = NMP && $OSSPEC = SVR4
    $CMD="$LINKER -o $exec $main $files $PCSQLDB $options ";
&else
    $CMD="$LINKER -o $exec $main $files $options $PCSQLDB ";
&endif
    print $CMD;
    system ($CMD);
}
else {
    print "Use sqllist.";
}
print "odbclnk $exec finished."
__END__
