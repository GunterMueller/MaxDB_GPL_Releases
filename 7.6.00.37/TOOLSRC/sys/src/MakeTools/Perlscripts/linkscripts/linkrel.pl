#!/usr/bin/perl
# linkrel       *** internal script (called by vmake !)
#
# @(#)linkrel  6.1.2  1995-09-20
#
# prelink object modules to relocatables
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

package linkrel;
use Env;
use Carp;
use ToolLib;

$DEBUG  = "";

do "$TOOLVARS";
if ( $@ ) { print "$@"; exit }
ToolTrace("linkrel called with @ARGV\n");
@LDCMD=();
$GenerateBuildNr=0;
$NOTHREAD=0;
$NOWHATSTRING = 0;
@APPENDFLAGS=();

$USAGE = "usage: linkrel relocatable objects/libraries\n";



while ($_ = $ARGV[0], /^-/) {
    shift;
    last if /^--$/;

	if(/^-g$/) { $DEBUG="y"; next}


    if(/^-g$|^-p$|^-Y$/) { next }	# ignored

	if (/^-k$/) { @KEEPFLG = "-k"; next }
	
	#CTS 1103488
	if(/^-BuildNrFunction\s*=\s*(.*)/i)
    {
        ToolLib::SetBuildNrModule($1);
        $GenerateBuildNr=1;		
        next;
    }
	if (/^-nothread/ ) 
	{ $NOTHREAD=1; next; }            #  CTS 1110865

} #while


if ( @ARGV < 2 ) { print $USAGE; exit 1 }

$REL = shift;

&if $MACH in [ NMP ]
# &gar PTS 1102494
foreach $OBJ (@ARGV)
{
   if ( $OBJ =~ /.*\.o/ )
   {
      system ("mcs -d -n .mdebug $OBJ") == 0 or 
        croak "Error while executing \"mcs -d -n .mdebug $OBBJ\"\n".
             "message: $!\n";
   } 
}
&endif

&if $OS in [ UNIX ]
@LDCMD=($LDREL, @LDREL_FLAGS, "-o", $REL, @ARGV);
&else
@ARFLAGS;
@LDCMD=($AR, @ARFLAGS, "-out:$REL", @ARGV);
&endif

# CTS 1103488
push @LDCMD, ToolLib::CreateBuildNrModule(@KEEPFLG,$DEBUG?("-g"):(),$NOTHREAD?("-nothread"):()) if ( $GenerateBuildNr );

push @LDCMD, @APPENDFLAGS;
$LINKCMD=join " ", @LDCMD;
$LINKCMD=~s/\s(\S*)\*(\S*)\s/ "$1\*$2" /g;
$LINKCMD=~s/\s(\S*)\?(\S*)\s/ "$1\?$2" /g;
$LINKCMD.=" ".ToolLib::getWhatModule($REL, $LINKCMD, @KEEPFLG,$DEBUG?("-g"):(),$NOTHREAD?("-nothread"):()) 
		unless ($ENV{APOMAKE} || $NOWHATSTRING );
print ("$LINKCMD\n") if ($ENV{NOQUIET});
ToolTrace("linkrel call: $LINKCMD\n");
open(LINK,"$LINKCMD 2>&1 |");
while(defined($line=<LINK>))
{
		# ignore lines with ignored values
		next if ( ($LNK_IGNORE ne "") && ($line=~/($LNK_IGNORE)/i));
		print $line; 
}
close(LINK);
unless ( $? == 0 )
{
		 die "Error while executing $LINKCMD \nmessage: $!\n";
}


__END__

Hier können lange Kommentare stehen
