#!/usr/bin/perl
# compp         *** internal script (called by vmake !)
#
# @(#)compp			1997-11-18
#
# compile pascal source module to object
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

package compp;
use Env;
use File::Basename;
use Carp;
do "$TOOLVARS";
if ( $@ ) { print "$@"; exit }
ToolTrace("compp called with @ARGV\n");

$USAGE = "usage: compp.pl [-k] [compiler_options] source\n";

@FLAGS = ();

while ($_ = $ARGV[0], /^[-+]/) {
    shift;
    last if /^--$/;

    if (/^-language/)   { next } # ignored

    if (/^-E$/ ) { next }     # ignored

    if (/^-c$/ ) { next }     # used implicitly

    if (/^-k$/ ) { @KEEPFLG = "-k"; next }

    if (/^-g$/ ) { push  @FLAGS, $_; next } # PTS 1109484 @KEEPFLG = "-k"; next }

    if (/^-.*/ ) { push @FLAGS, $_; next }

    if (/^\+.*/ ) { push @FLAGS, $_; next }

} #while

if ( @ARGV != 1 ) { croak "$USAGE"; exit 1 }

$SOURCE = $ARGV[0];
$BASE = basename $SOURCE;

if ($BASE =~ /(.*)\.p/) { $BASE = $1 }

@ARGV = ( @FLAGS, @KEEPFLG, "$SOURCE" );
do "$TOOL/bin/comppc$TOOLEXT";
if ( $@ ) { warn "Error while executing comppc:\n", "$@"; exit }


# &gar CTS 1102757

@ARGV = ( @FLAGS, @KEEPFLG, "$BASE.c" );
do "$TOOL/bin/compc$TOOLEXT";
if ( $@ ) { warn "Error while executing compc:\n", "$@"; exit }

if ( @KEEPFLG == 0 ) { unlink "$BASE.c" }

__END__


Hier k�nnen lange Kommentare stehen
