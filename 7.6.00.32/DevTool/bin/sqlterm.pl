#!/usr/bin/perl
# @(#) sqlterm.kt   1994-07-21   admin   kern
# Dient zum Erstellen von SQLTERM.
# Siehe auch $DESC/fast/sqlterm.mac
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

chdir("$WRK/fast/tmp") || die "Can't change to directory $WRK/fast/tmp: $!";

@ARGV = ("-f", "-trace=none", "vta44cpc");
do "$TOOL/bin/mfc$TOOLEXT";
if ( $@ ) { die "Error while executing mfc$TOOLEXT: $@" }
if ( -s "vta44cpc.c" ) { unlink "vta44cpc.cpc"; rename("vta44cpc.c", "vta44cpc.cpc");
   system("cpc -H nocheck -S sql-db vta44cpc -I $TOOL/incl -I $WRK/incl");
   if ( -s "vta44cpc.o" ) {
	unlink "$WRK/fast/obj/vta44cpc.o";
	rename("vta44cpc.o", "$WRK/fast/obj/vta44cpc.o"); unlink <vta44cpc*>;
	}
}

@ARGV = ("-f", "-trace=none", "vta45cpc");
do "$TOOL/bin/mfc$TOOLEXT";
if ( $@ ) { die "Error while executing mfc$TOOLEXT: $@" }
if ( -s "vta45cpc.c" ) { unlink "vta45cpc.cpc"; rename("vta45cpc.c", "vta45cpc.cpc");
   system("cpc -H nocheck -S ansi vta45cpc -I $TOOL/incl -I $WRK/incl");
   if ( -s "vta45cpc.o" ) {
	unlink "$WRK/fast/obj/vta45cpc.o"; rename("vta45cpc.o", "$WRK/fast/obj/vta45cpc.o");
	unlink <vta45cpc*>;
   }
}

@ARGV = ("-f", "-trace=none", "vta46cpc");
do "$TOOL/bin/mfc$TOOLEXT";
if ( $@ ) { die "Error while executing mfc$TOOLEXT: $@" }
if ( -s "vta46cpc.c" ) { unlink "vta46cpc.cpc"; rename("vta46cpc.c", "vta46cpc.cpc");
   system("cpc -H nocheck -S ansi vta46cpc -I $TOOL/incl -I $WRK/incl");
   if ( -s "vta46cpc.o" )  {
	unlink "$WRK/fast/obj/vta46cpc.o"; rename("vta46cpc.o", "$WRK/fast/obj/vta46cpc.o");
	unlink <vta46cpc*>;
   }
}

@ARGV = ("-f", "-trace=none", "vta47cpc");
do "$TOOL/bin/mfc$TOOLEXT";
if ( $@ ) { die "Error while executing mfc$TOOLEXT: $@" }
if ( -s "vta47cpc.c" ) { unlink "vta47cpc.cpc"; rename("vta47cpc.c", "vta47cpc.cpc");
   system("cpc -H nocheck -S oracle vta47cpc -I $TOOL/incl -I $WRK/incl");
   if ( -s "vta47cpc.o" ) {
	unlink "$WRK/fast/obj/vta47cpc.o"; rename("vta47cpc.o", "$WRK/fast/obj/vta47cpc.o");
	unlink <vta47cpc*>;
   }
}

@ARGUMENTS = qw( "-o"
		"$DBROOT/pgm/sqlterm"
        "$WRK/fast/obj/vta40.o"
		"$WRK/fast/obj/vta41.o"
		"$WRK/fast/obj/vta42.o"
		"$WRK/fast/obj/vta43.o"
		"$WRK/fast/obj/vsp40.o"
		"$WRK/fast/obj/vsp43.o"
		"$WRK/fast/obj/vgg17.o"
		"$WRK/fast/obj/vta44cpc.o"
		"$WRK/fast/obj/vta45cpc.o"
		"$WRK/fast/obj/vta46cpc.o"
		"$WRK/fast/obj/vta47cpc.o"
		"$WRK/fast/obj/enalib.a"
		"$WRK/fast/obj/enblib.a"
		"$WRK/fast/obj/splib.a"
		"$DBROOT/lib/pcrlib.a"
		"$DBROOT/lib/libsqlrte.a"
		"$DBROOT/lib/libsqlptc.a"
		"$DBROOT/lib/sqlca.a"
		"$DBROOT/lib/csci.a"
		"/usr/lib/libsocket.a" );

system("cc", @ARGUMENTS);
