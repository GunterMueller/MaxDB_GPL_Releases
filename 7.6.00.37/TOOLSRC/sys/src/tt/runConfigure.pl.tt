#!/usr/bin/perl
#
# generates RTE_Configure from RTE_Configure.ac if requested
# generates $WRK/incl/sapdbconfig.h by running RTE_Configure
#

use strict;

sub vsystem ()
{
    my ($cmdline) = @_;
    print "system ($cmdline)\n";
    system ($cmdline);
}

sub main ()
{
    my $configure = $ARGV[0];
    my $configureObjDir = "$ENV{WRK}/obj/SAPDB/RunTime";
    if ($ENV{'USE_AUTOCONF'}) {
        # $ENV{'autom4te_perllibdir'} = "$ENV{HOME}/build/autoconf-2.53/lib";
        # $ENV{'AC_MACRODIR'} = "$ENV{HOME}/build/autoconf-2.53/lib";
        system ("autoconf "
        #        . "--include=$ENV{HOME}/build/autoconf-2.53/lib "
                . "$configureObjDir/RTE_Configure.ac"
                . "> ./$configure")                ;
        system ("cp ./$configure $configureObjDir");
    }
    system ("/bin/sh ./$configure");
}

main ();

