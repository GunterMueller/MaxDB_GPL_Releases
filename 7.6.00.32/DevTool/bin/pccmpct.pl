# pccmpct
#
# @(#)pccmpct  6.2  1997-11-27
#
# Builds a compact Referencprotokoll
#
# Burkhard Diesing
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

use PCTest;
$Trace=1; # Switch to 1 to enable trace
$Trace++ if ($Trace);

$USAGE ="USAGE: pccmpct dirname/filename [extensions]\n";
if (@ARGV < 1) {
print $USAGE;
exit;
}

local ($FNAME, $FDIR, @EXTENSSIONS, $PREF);

foreach $_ (@ARGV) {
    if (m!(.*)/(.*)!) {
	$FDIR=$1;
	$FNAME=$2;
    }
    else {
	$EXTENSIONS=$_;
    }
}

print "FDIR=$FDIR;FNAME=$FNAME;EXTENSIONS=$EXTENSIONS" if ( $Trace );

for ($i=0; $i < (length $EXTENSIONS); $i++) {
    @EXTENSIONS=(@EXTENSIONS, substr($EXTENSIONS, $i, 1));
    print @EXTENSIONS if ( $Trace );
}

$PREF=PCTest::GetPrefix($FDIR);

if (@EXTENSIONS == 0) {
    @EXTENSIONS=("l", "o", "t");
    print "set default Extensions to @EXTENSIONS" if ( $Trace );
}

print "GetPrefix returns $PREF\n" if ( $Trace );
PCTest::MakeCompactProt ( $PREF, $FNAME, @EXTENSIONS );

