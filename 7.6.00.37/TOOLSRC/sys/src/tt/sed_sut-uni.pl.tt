
#!/usr/bin/perl
# @(#) sed_sut-uni   6.2   1996-04-18   ckr krischan
#
# sed fuer die Behandlung der Protokolle von UNICODE-Testlaeufen
# Wichtig ist, dass nicht alles einem sed vorgeworfen wird (arg list 2 long)
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
use File::Copy;
require "hostname.pl";

$FILE = $ARGV[0];
my $TEMP = "$TMP/uni.$$";
$NODE = hostname();
copy($FILE ,$TEMP);
open(FILE_IN, "<$TEMP"); open(FILE_OUT, ">$FILE");

if (
($RELVER lt "R74")
||
(($RELVER eq "R74") && ($CORRECTION_LEVEL lt "3"))
)
{
    while(<FILE_IN>)
    {
        # entfernen Differenzen, die durch unterschiedlich groﬂes Initialisierungs-
        # save (zur nicht UNICODE DB) entstehen
        if (($. == 22) || ($. == 19) ||
            ($. == 26) || ($. == 27) ||
            ($. == 34) || ($. == 25)) {
    	s/^(Pages Transferred    )48/${1}40/;
    	s/^(Pages Transferred    )64/${1}56/;
        }
        if (($. == 27) || ($. == 30) ||
            ($. == 31) || ($. == 33) ||
            ($. == 34) || ($. == 35) ||
            ($. == 36) || ($. == 37) ||
            ($. == 42) || ($. == 49) ||
            ($. == 52)) {
            s/^(First LOG Page       )22/${1}19/;
            s/^(First LOG Page       )46/${1}43/;
            s/^(First LOG Page       )49/${1}43/;
        }
        if (($. == 33) || ($. == 36) ||
            ($. == 37) || ($. == 39) ||
            ($. == 40) || ($. == 41) ||
            ($. == 42) || ($. == 48) ||
            ($. == 58)) {
            s/^(Page Count           )25/${1}23/;
            s/^(Page Count           )42/${1}40/;
            s/^(Page Count           )46/${1}40/;
        }

    	s/^\(SERVERNODE        :  \)$NODE/\1<servernode>/;
    	s/^CODE              :  UNICODE/CODE              :  ASCII/g;
    	s/^CODETYPE          :  UNICODE/CODETYPE          :  ASCII/g;
    	next if /^TERMINAL UNICODE!$/;
    	next if /^TERMINAL ASCII!$/;
    	s/ - ALL CO$/ - ALL COLUMNS -/g;
    	s/SYS_UPDST$/SYS_UPDSTATISTICS/;
    	s/SYSUPDSTA$/SYSUPDSTATWANTED/;
    	s/SYSACCOUN$/SYSACCOUNT/;
    	s/SYSAUTOSA$/SYSAUTOSAVE/;
    	s/SYS_PACKA$/SYS_PACKAGE/;
    	s/TABLE STA$/TABLE STATISTICS/;
    	s/UPDSTATDA$/UPDSTATDATE/;
    	s/UPDSTATTI$/UPDSTATTIME/;
    	s/REPLICATI$/REPLICATION/;
    	s/SERVERNOD$/SERVERNODE/;
    	s/COLUMNNAM$/COLUMNNAME/;
    	s/DISTINCTV$/DISTINCTVALUES/;
    	s/AVGLISTLE$/AVGLISTLENGTH/;
    	s/COLUMNPRI$/COLUMNPRIVILEGES/;
    	s/REFTABLEN$/REFTABLENAME/;
    	s/REFCOLUMN$/REFCOLUMNNAME/;
    	s/SYNONYMNA$/SYNONYMNAME/;
    	s/MAXTIMEOU$/MAXTIMEOUT/;
    	s/PWCREATED$/PWCREATEDATE/;
    	s/PWCREATET$/PWCREATETIME/;
    	s/CATALOG_C$/CATALOG_CACHE_SIZE/;
    	s/DBPROC_CA$/DBPROC_CACHE_SIZE/;
    	s/TEMP_CACH$/TEMP_CACHE_SIZE/;
    	s/COSTWARNI$/COSTWARNING/;
    	s/CACHELIMI$/CACHELIMIT/;
    	s/TRIGGERNA$/TRIGGERNAME/;
    	s/SEQUENCE_$/SEQUENCE_NAME/;
    	s/INCREMENT$/INCREMENT_BY/;
    	s/CYCLE_FLA$/CYCLE_FLAG/;
    	s/ORDER_FLA$/ORDER_FLAG/;
    	s/CACHE_SIZ$/CACHE_SIZE/;
    	s/LAST_NUMB$/LAST_NUMBER/;
    	s/DESCRIPTI$/DESCRIPTION/;
    	s/ERROR_COD$/ERROR_CODE/;
    	s/PARTNER S$/PARTNER SERVERDB/;
    	s/COORDINAT$/COORDINATOR/;
    	s/TRANSSTAT$/TRANSSTATE/;
    	s/TRANSCOUN$/TRANSCOUNT/;
    	s/DBPROCEDU$/DBPROCEDURE/;
    	s/RUNTIMEEN$/RUNTIMEENVIRONMENT/;
    	s/EXECUTABL$/EXECUTABLE/;
    	s/DEFINITIO$/DEFINITION/;
    	s/DOMAINNAM$/DOMAINNAME/;
    	s/CONSTRAIN$/CONSTRAINTNAME/;
    	s/EXPRESSIO$/EXPRESSION1/;
    	s/:  PRIVILEGE$/:  PRIVILEGES/;
    	print FILE_OUT $_;
    }
}
else
{
    while(<FILE_IN>)
    {
        # entfernen Differenzen, die durch unterschiedlich groﬂes Initialisierungs-
        # save (zur nicht UNICODE DB) entstehen
        if (($. == 22) || ($. == 19) ||
            ($. == 26) || ($. == 27) ||
            ($. == 34) || ($. == 25)) {
    	s/^(Pages Transferred    )48/${1}40/;
    	s/^(Pages Transferred    )64/${1}56/;
        }
        if (($. == 27) || ($. == 30) ||
            ($. == 31) || ($. == 33) ||
            ($. == 34) || ($. == 35) ||
            ($. == 36) || ($. == 37) ||
            ($. == 42) || ($. == 49) ||
            ($. == 52)) {
            s/^(First LOG Page       )22/${1}19/;
            s/^(First LOG Page       )46/${1}43/;
            s/^(First LOG Page       )49/${1}43/;
        }
        if (($. == 33) || ($. == 36) ||
            ($. == 37) || ($. == 39) ||
            ($. == 40) || ($. == 41) ||
            ($. == 42) || ($. == 48) ||
            ($. == 58)) {
            s/^(Page Count           )25/${1}23/;
            s/^(Page Count           )42/${1}40/;
            s/^(Page Count           )46/${1}40/;
        }

    	s/^\(SERVERNODE        :  \)$NODE/\1<servernode>/;
    	s/^CODE              :  UNICODE/CODE              :  ASCII/g;
    	s/^CODETYPE          :  UNICODE/CODETYPE          :  ASCII/g;
    	next if /^TERMINAL UNICODE!$/;
    	next if /^TERMINAL ASCII!$/;
    	print FILE_OUT $_;
    }
}

close(FILE_IN); close(FILE_OUT);
unlink $TEMP;
