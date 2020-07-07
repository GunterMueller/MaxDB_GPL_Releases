#!/usr/bin/perl
#
# pstat - perl script to list submit history for perforce
#
# 2001/04/03   h.b.       
#
#    ========== licence begin LGPL
#    Copyright (C) 2002 SAP AG
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#    ========== licence end
#


use Env;
use Getopt::Std;
use DUser;

if ( !getopts('j:u:hd:') || $opt_h ) {
    print <DATA>;
    exit 1;
}

my $depotpath = $ENV{DEPOT};
if ($opt_d) {
   $depotpath = $opt_d; 
}

if ($opt_u) {
    if ($opt_u !~ /^\s*d\d+\s*$/) {
        @UidList = DUser::userlookup($opt_u);
        if (@UidList == 1) {
            $WantedUid = $UidList[0];
        } else {
            print "unknown or ambiguous user: $opt_u!\n\n";
            print <DATA>;
            exit 1;
        }
    } else {
        $WantedUid = $opt_u;
    }
}


# build compare date 
$Year_index  = \5;	
$Month_index = \4;
$Day_index   = \3;
@localtime = localtime(); 
$Y = $localtime[$$Year_index];
$M = $localtime[$$Month_index] + 1;
$D = $localtime[$$Day_index];
if ($opt_j) {
    $DAYS = $opt_j;
    while ( $DAYS > 0 ) {
        if ( $D > $DAYS ) {
            $D -= $DAYS;
            $DAYS = 0;
        } else {
            $DAYS -= $D;
            if ( $M > 1 ) { $M-- }
            else { $Y--; $M = 12 }

            CASE: {
                if ( $M =~ /^(1|3|5|7|8|10|12)$/ ) { $D = 31; last CASE }
                if ( $M =~ /^(4|6|9|11)$/ ) { $D = 30; last CASE }
                if ( $M =~ /^2$/ ) {
                    if ( $Y % 4 == 0 ) { $D = 29 } else { $D = 28 }
                    last CASE;
                }
            } #CASE
        }
    } #while
}
$D  = "0$D" if length($D) < 2;
$M  = "0$M" if length($M) < 2;
$Y += 1900;

open (CHANGE_PIPE, "p4 -s changes $depotpath/... |") 
   or die "can't open pipe for p4 changes\n";

while (<CHANGE_PIPE>) {

    if (/^error:/) {
        print $_;
        last;
    }
    
    if (/^info:/) {
        
        # should match:
        # info: Change 3474 on 2001/03/30 by d024849@p47368 'PTS 1109860 '"
        
        unless (/^info:\s*Change\s+(\d+)\s+on\s+(\d+)\/(\d+)\/(\d+)\s+by\s+(\w+)@(\S+)\s+(.*)/) {
        
            die "unexpected info format\n";
        
        } else {
        
            $ChangeNr   = $1;
            $ChangeYear = $2;
            $ChangeMon  = $3;
            $ChangeDay  = $4;
            $ChangeUid  = $5;
            $ChangeNode = $6;
            $ChangeDesc = $7;

            last if ("$ChangeYear$ChangeMon$ChangeDay" lt "$Y$M$D");
            
            if (!defined($WantedUid) || ($WantedUid =~ /$ChangeUid/i)) {
                if ($ChangeUid =~ /d(\d+)/i ) {
                    my @realuser = DUser::userlookup($ChangeUid);
                    $ChangeUid = $realuser[0] if ( scalar (@realuser) == 1 );
                }
                foreach $File (GetDetailInfo($ChangeNr, \$ChangeTime)) {
                    printf ("%s.%s.%s %-8s #%-5s %-20s %s\n", 
                    $ChangeDay,
                    $ChangeMon,
                    $ChangeYear,
                    $ChangeTime,
                    $ChangeNr,
                    $ChangeUid,
                    $File); 
                }
            }

            
        } 
        
    }

}

close (CHANGE_PIPE);


####################################################################

sub GetDetailInfo {

    my ($ChangeNr,$TimeStamp) = @_;

    my @ModulList;
    
    open (DESC_PIPE, "p4 -s describe $ChangeNr |") 
        or die "can't open pipe for p4 describe\n";

    while (<DESC_PIPE>) {
       
        #text: Change 3602 by d024855@p47355 on 2001/04/03 08:04:22
        if (/^text:\s+Change\s+\d+\s+by\s+.*\s+on\s+\d{4}\/\d{2}\/\d{2}\s+(.*)/) {
            $$TimeStamp = $1;
        }

        #info1: //sapdb/V74/develop/sys/desc/SAPDB/Logging.com#128 edit
        if (/^info1:\s+$depotpath\/.*\/([^#]+)#\d+\s+.*/) {
            @ModulList = (@ModulList, $1);
        }
    }
    
    close (DESC_PIPE);

    return @ModulList;
    
}

__DATA__

Usage:  pstat [-u user] [-j <days>] [-d <depot>]

        lists the submits from the last days (default current day)
        
         Options: -u <user>  ::= submits from user <user> 
                  -j <days>  ::= submits not older then <days> days
                  -d <depot> ::= submits in <depot> path (default $DEPOT)

