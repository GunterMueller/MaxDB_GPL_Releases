# tr_cpc
#!/usr/bin/perl
#
# @(#)tr_cpc	1997-12-04
#
# Compile a vxxnnc.c file which contains precompiler statements.
# Renames (copies) .c to .cpc and calls cpc, leaving .c.
# The result can then be compiled.
#
# Example: rename vni90c.c to vni90c.cpc and call cpc which creates vni90c.c
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

package tr_cpc;
use Env;
use File::Basename;
use File::Copy;

$SOURCE = basename($ARGV[0], ".c");

$CPC64="";
if ( $ENV{RELVER} gt "R73" ) {
    $CPC64="-BIT64" if (defined $ENV{BIT64});
}

# The source file may be local (e.g. $FTMP) or elsewere (e.g. $CSRC)
if ( $ARGV[0] = "$SOURCE.c" ) {
    unlink "$SOURCE.cpc"; rename($ARGV[0], "$SOURCE.cpc");
} else {
	File::copy($ARGV[0], "$SOURCE.cpc") || die "Can't copy $ARGV[0] to $SOURCE.cpc : $!\n";
}
    $RET = system("cpc $CPC64 -H nocheck -S INTERNAL -c $SOURCE");
# MUST NOT overwrite the original ($CSRC) file.
# cp $SOURCE.c $1

if ( $RET != 0 ) { 1 } else { unlink "$SOURCE.cpc"; 0 }

__END__

