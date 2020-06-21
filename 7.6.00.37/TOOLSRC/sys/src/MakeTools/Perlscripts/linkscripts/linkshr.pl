#!/usr/bin/perl
# linkshr       *** internal script (called by vmake !)
#
# @(#)linkshr           1997-12-12
#
# prelink a relocatable, which uses shared memory
#
# TOOL          directory for tools
# LDFLAGS       optional loader flags
# KEEPFLG       optional keep intermediate files flag
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


package linkshr;
use Env;
use File::Copy;
use Carp;

# Get LDFLAGS, LD, LIB*
do "$TOOL/bin/toolvars$TOOLEXT";
if ( $@ ) { print "$@"; exit }
ToolTrace("linkshr called with @ARGV\n");

$USAGE = "usage: linkshr [-k] [ld-flags] relocatable objects\n";


while ($_ = $ARGV[0], /^-/) {
    shift;
    last if /^--$/;


    if (/^-Y$/) { $SHRGLOB="-Y"; next }

    if (/^-k$/) { @KEEPFLG = "-k"; next }

    if (/^-.*/) { push @LDFLAGS, $_; next }

 } #while

if ( @ARGV < 1 ) { print $USAGE;  exit 1; }

$OUTFILE = $ARGV[0];
shift;

push @OBJECTS, "gsize.o";
@OBJECTS = @ARGV;

do "$TOOL/bin/ptc_gsiz$TOOLEXT";

&if $OS = UNIX
@CFLAGS;
system ("$CC", @CCFLAGS, @CFLAGS, "-c", "gsize.c");
&else
system ("$CC", @CCFLAGS, "-c", "gsize.c");
&endif

push @CMD, "ld", "-r", "-o", "$OUTFILE";
push @CMD, "-L$TOOL_LIB";
push @CMD, @OBJECTS;
system(@CMD) == 0
         or croak "Error while executing @CMD\n".
         "message: $!\n";

print "$OUTFILE\n";

if ( @KEEPFLG == 0 ) {
        unlink "gsize";
        unlink "allsiz";
        unlink "gsize.siz";
        unlink "gsize.c";
        unlink "gsize.o";
}                                                                               
                                                                                
                                                                                                                                                                
__END__                                                                         