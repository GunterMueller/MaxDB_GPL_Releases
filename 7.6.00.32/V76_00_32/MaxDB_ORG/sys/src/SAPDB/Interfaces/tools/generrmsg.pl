#!/usr/bin/perl
#
# @(#)generrmsg   2003-04-02
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2001-2005 SAP AG
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

use Env;
$USAGE ="USAGE: generrmsg infile\n";
die $USAGE if (@ARGV < 1);

$WRKDATE=date();
$infile=$ARGV[0];
$outfile="$infile.tmp";

open(FILE_OUT, ">$outfile") || die "Error can't open '$outfile'.";
print FILE_OUT "++\n";
print FILE_OUT "++\@file           SQLDBCM.en\n";
print FILE_OUT "++\@author         DL SAP DB INTERFACES\n";
print FILE_OUT "++\@brief          System messages of the SAP Database Connectivity\n";
print FILE_OUT "++========== licence begin LGPL\n";
print FILE_OUT "++  <COPYRIGHT_STRING>\n";
print FILE_OUT "++\n";
print FILE_OUT "++  This library is free software; you can redistribute it and/or\n";
print FILE_OUT "++  modify it under the terms of the GNU Lesser General Public\n";
print FILE_OUT "++  License as published by the Free Software Foundation; either\n";
print FILE_OUT "++  version 2.1 of the License, or (at your option) any later version.\n";
print FILE_OUT "++  \n";
print FILE_OUT "++  This library is distributed in the hope that it will be useful,\n";
print FILE_OUT "++  but WITHOUT ANY WARRANTY; without even the implied warranty of\n";
print FILE_OUT "++  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n";
print FILE_OUT "++  Lesser General Public License for more details.\n";
print FILE_OUT "++\n";
print FILE_OUT "++  You should have received a copy of the GNU Lesser General Public\n";
print FILE_OUT "++  License along with this library; if not, write to the Free Software\n";
print FILE_OUT "++  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n";
print FILE_OUT "++  ========== licence end\n";
print FILE_OUT "++ =========================================================\n";
print FILE_OUT "++ \@(#) SQLDBCM.en(s) <FORMATED_BUILD_VERSION> Build <BUILD_NUMBER_STRING> $WRKDATE\n";
print FILE_OUT "++ =========================================================\n";
print FILE_OUT "++------------------ERRORS-----------------------------------------------\n";
print FILE_OUT "ENG -99999 SQLDBC ENGMSG <FORMATED_BUILD_VERSION> Build <BUILD_NUMBER_STRING>\n";
open(FILE_IN, "$infile") || die "Error: '$infile' not found.";

my $found = 0;
 LINE:
while(<FILE_IN>) {
    if (m/^\s*static\s*IFR_ErrorData\s.*=.*$/) {
        $found = 1;
        next LINE;
    }
    if ($found) {
        if (m/\}\s*;/) {
            $found = 0;
            last LINE;
        }
    }
    if ($found) {
	($white,$applcode,$errorcode,$sqlstate,$msg) = 
            m/^(\s*)\{\s*(\w*),\s*([\-0-9]*),\s*\"(\w*)",\s*"(.*)"\s*\}.*$/;
        if (defined $errorcode) {           
           print FILE_OUT "ENG $errorcode $msg\n";
        }
    }    
}
close(FILE_IN);
close(FILE_OUT);
unlink $infile;
rename $outfile, $infile;
1;

sub date
{
    $Year_index = \5;	#constant
    $Month_index = \4;	#constant
    $Day_index = \3;	#constant
    $Hour_index = \2;	#constant
    $Min_index = \1;	#constant
    $Sec_index = \0;	#constant
    @localtime = localtime();
    $localtime[$$Month_index]++;
    $localtime[$$Year_index]+=1900;
    return sprintf("%04d-%02d-%02d", $localtime[$$Year_index], $localtime[$$Month_index], $localtime[$$Day_index]);
}

