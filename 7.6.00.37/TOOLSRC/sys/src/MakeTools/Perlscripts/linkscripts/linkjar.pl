#!/usr/bin/perl
# linkjar       *** internal script (called by vmake !)
#
# @(#)linkjar 2001-03-13
#
# zip java classes to java archive
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

package linkjar;
use Env;
use Carp;
use ToolLib;
use FileHandle;

do "$TOOLVARS";
if ( $@ ) { print "$@"; exit }
ToolTrace("linkjar called with @ARGV\n");

do "$TOOL/bin/link_opts$TOOLEXT";
if ( $@ ) { print "$@"; exit }

$USAGE = "usage: linkjar jarfile <java classes>...\n";

if ( @ARGV < 2 ) { print $USAGE; exit 1 }

$JarFile     = shift;
$ContentFile = shift;


push @JCMD, "jar", "cf";

$cf = new FileHandle;
if ( $cf->open( "< $ContentFile") )
{
    $created = 1;
	
	# should be called with "...MANIFEST.FM demand" in *.jar
	# if ( -f "MANIFEST.MF" )
	#{
	#	open(FILE_OUT, ">>MANIFEST.MF");
	#	# handle RELVER 
	#	print ( FILE_OUT, "$RELVER.$ENV{BUILDSTRING}\n");
	#	close (FILE_OUT);
	#	push @JCMD
	#}

    while( $_ = $cf->getline )
    {
        system("@JCMD $JarFile $_") == 0
            or croak "Error while executing \"@JCMD $JarFile $_\"\n".
            "message: $!\n";
        if ( $created ) { pop @JCMD; push @JCMD, "uf"; $created = 0 };
    }
    $cf->close;
}
else
{ die "can't open content file: $!"; }



__END__

Hier können lange Kommentare stehen
