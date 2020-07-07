#!/usr/bin/perl
# compa         *** internal script (called by vmake !)
#
# @(#)compa			1997-11-18
#
# compile assembler source module
#
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

package compa;
use Env;
use File::Basename;
use File::Copy;
use Carp;
use Cwd;
# Get AS, ASFLAGS, OPTIMIZE
do "$TOOLVARS";
if ( $@ ) { print "$@"; exit }
ToolTrace("compa called with @ARGV\n");

$USAGE = "usage: compa [as-flags] source\n";

$NO = "";
while ($_ = $ARGV[0], /^-/) {
    shift;
    last if /^--$/;

    if (/^-NO$/)  { $NO = "-NO"; next        }

    if (/^-E$/)   { next                     } # ignored

    if (/^-language/)   { next               } # ignored

    if (/^-except/)   { next               } # ignored

	if (/^-optdebug/)   { next               } # ignored

    if (/^-c$/)   { next                     } # used implicitly

    if (/^-save_sources/) 	{ next;	}

	if (/^-suppress_variant_name$/) 	{ next;	}

	if (/^-pdbfile=(.+)$/) { next; }

   	if (/^-nothread/ ) { @THREAD_CCFLAGS=(); $LINKMODE = "-MD"; next; }            # CTS 1110865

    if (/^-k$/)   { @KEEPFLG = "-k"; next    }


	if (/^-g$/)   {
# B.D 1998-02-23 the kernel will not be linked if you push -Zi
#	    push @ASFLAGS, "-Zi";
	    next }

    if (/^-.*/)   { push @ASFLAGS, $_; next  }

} #while

if ( @ARGV != 1 ) { print $USAGE; exit 1 }

$SOURCE = $ARGV[0];
$SOURCE =~ tr!/!\\!;


print ("$AS @ASFLAGS -c $SOURCE\n") if ($ENV{'NOQUIET'});
system("$AS @ASFLAGS -c $SOURCE") == 0
     or croak "Error while executing \"$AS @ASFLAGS -c $SOURCE\"\n".
	 "message: $!\n";


__END__

Hier kann ein langer Kommentar stehen!

