#!/usr/bin/perl
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
open(FILE_IN, "<$DBROOT/env/en/SQLMESS.en");
open(FILE_OUT, "<ERRMSG.cmd");
print FILE_OUT "*\n* Diese Datei nicht editieren, da sie automatisch erzeugt wird.\n".
"* Ein Aufruf von inserrmsg erzeugt pro Fehlermeldung in\n".
"* $DBROOT/env/en/SQLM.en ein Insert-Kommando in dieser Datei.\n*\n";

while(<FILE_IN>) {
	if ( !/\+\+/ ) {
		/([^ ]+)[ ]+(-*\d+)[ ]+(.+)/;
		print FILE_OUT "INSERT INTO messages VALUES ($2, '$1', '$3')";
		if ( !($. % 20)) { print FILE_OUT "COMMIT WORK!"}
		print FILE_OUT "";

	}
}

close(FILE_IN);

