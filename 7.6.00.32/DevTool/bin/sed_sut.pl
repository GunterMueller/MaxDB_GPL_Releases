#!/usr/bin/perl
#
# @(#)sed_sut		1997-11-18
#
# sed fuer die Nachbearbeitung der Singleusertests.
#
# 1994-10-27  cs  - 'xinitdb.save' entspricht nicht 8.3 Konvention,
#                    deshalb gibt es fuer OS/2 und NT condidional
#                    compiling
#		          - TERMID : kern wird fuer OS/2 und NT ersetzt, weil
#                   es NERVT!
# 1994-10-31  cs  - 'yinitdb.save' ... (s.o.)
# 1995-08-21  cs  - OS/2, NT: 'xinitdb.sav' wird nicht mehr verbogen
#                 - '&IF $OSSPEC IN ...'nach''&if $OS in ...' geaendert
# 1997-02-14  gr  - Yesterday & Vorgestern genau gerechnet
#                 - noch mehr Date- & Time-Formate verbogen
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


#==================================================================
use Env;
require "hostname.pl";

# Check input args
if ( !defined($ARGV[0]) || !defined($ARGV[1]) ) { die "usage: sed_sut infile outfile\n" }

$INFILE  = $ARGV[0];
$OUTFILE = $ARGV[1];

local($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst) = localtime(time);
$monthname = (JAN,FEB,MAR,APR,MAY,JUN,JUL,AUG,SEP,OCT,NOV,DEC)[$mon];
$year += 1900;
$mon++;
$mon = "0$mon" if $mon < 10;
$mday = "0$mday" if $mday < 10;

# Today
$YEAR  = $year;							# all 4 digits of year
$MON   = $mon;							# month 01..12
$DAY   = $mday;							# day 01..31
$TODAY = "$YEAR$MON$DAY";				# YYYYMMDD
$TODAY_YMD = "$YEAR-$MON-$DAY";			# YYYY-MM-DD
$TODAY_DMY = "$DAY\\.$MON\\.$YEAR";			# DD.MM.YYYY
$TODAY_MDY = "$MON/$DAY/$YEAR";			# MM/DD/YYYY
$TODAY_ORA = "$DAY-$monthname-" . substr($YEAR,2,2); # DD-MMM-YY
$TODAY_FY  = "";
$TODAY_F   = "";

local $i;
for ($i=0; $i<length($YEAR); $i++) {
	$TODAY_FY .= "F".substr($YEAR, $i, 1); # FYFYFYFY
}
$TODAY_F = $TODAY_FY;
for ($i=0; $i<length("$MON$DAY"); $i++) {
	$TODAY_F .= "F".substr("$MON$DAY", $i, 1); # FYFYFYFYFMFMFDFD
}
$TODAY_3 = $TODAY_F;
$TODAY_3 =~ tr/F/3/; # 3Y3Y3Y3Y3M3M3D3D

# Yesterday
($YEAR, $MON, $DAY) = prev_day($YEAR, $MON, $DAY);
$YESTERDAY = "$YEAR$MON$DAY";					# YYYYMMDD
$YESTR_YMD = "$YEAR-$MON-$DAY";				# YYYY-MM-DD
$YESTR_DMY = "$DAY\\.$MON\\.$YEAR";				# DD.MM.YYYY
$YESTR_MDY = "$MON/$DAY/$YEAR";				# MM/DD/YYYY
$YESTR_FY  = "";
$YESTR_F   = "";
for ($i=0; $i<length($YEAR); $i++) {
	$YESTR_FY .= "F".substr($YEAR, $i, 1); # FYFYFYFY
}
$YESTR_F = $YESTR_FY;
for ($i=0; $i<length("$MON$DAY"); $i++) {
	$YESTR_F .= "F".substr("$MON$DAY", $i, 1); # FYFYFYFYFMFMFDFD
}
$YESTR_3 = $YESTR_F;
$YESTR_3 =~ tr/F/3/; # 3Y3Y3Y3Y3M3M3D3D

# Vorgestern
($YEAR, $MON, $DAY) = prev_day($YEAR, $MON, $DAY);
$VORGESTERN = "$YEAR$MON$DAY";				# YYYYMMDD
$VORGE_YMD  = "$YEAR-$MON-$DAY";				# YYYY-MM-DD
$VORGE_DMY  = "$DAY\\.$MON\\.$YEAR";				# DD.MM.YYYY
$VORGE_MDY  = "$MON/$DAY/$YEAR";				# MM/DD/YYYY
$VORGE_FY   = "";
for ($i=0; $i<length($YEAR); $i++) {
	$VORGE_FY .= "F".substr($YEAR, $i, 1); # FYFYFYFY
}
$VORGE_F = $VORGE_FY;
for ($i=0; $i<length("$MON$DAY"); $i++) {
	$VORGE_F .= "F".substr("$MON$DAY", $i, 1); # FYFYFYFYFMFMFDFD
}
$VORGE_3 = $VORGE_F;
$VORGE_3 =~ tr/F/3/; # 3Y3Y3Y3Y3M3M3D3D

$NODE = $REMOTENODE ? $REMOTENODE : hostname();
$NODE=~ s/\..*//;    # UH 17-01-2000

$APPLNODE = hostname();
$APPLNODE=~ s/\..*//;

open(FILE_IN, "<$INFILE") || die "Can't open $INFILE (input): $!";
open(FILE_OUT, ">$OUTFILE") || die "Can't open $OUTFILE (output): $!";

while(<FILE_IN>) {

#$IF $OS in [ OS2, WIN32 ]
    s/^(TERMID            :  )$TERMID/$1<termid>/g;
#$ENDIF
    s/^(CPUTIME           : )(.*)/$1   <cputime>/;
    s/^(LOCK INFO         :     ...............)(\(....s\) )(\d\d\.\d\d\d\d\d\d\d\d\.\d\d\d\d\d\d )(............) ...... (.*) \d\d\d\d(.*)/$1(TIME)  $3$4 -termid- -process/;
    s/^(LOCK INFO         : .*)    tty\d\d    \d\d\d\d/$1-termid- -process/;
    s/^(LOCK INFO         : .*)             \d\d\d\d(.*)/$1-termid- -process/;
    s/^(LOCK INFO         :     ...............)(\(....s\) )(\d\d\.\d\d\d\d\d\d\d\d\.\d\d\d\d\d\d )(.............)(.*) \d\d\d\d(.*)/$1(TIME)  $3$4-termid- -process/;
if ($RELVER ge "R72"){
    s/^(LOCK INFO         :     ...............)(\(....s\) )(\d\d\d\d\d\d\d\d\.\d\d\d\d\d\d )(............) ...... (.*) \d\d\d\d(.*)/$1(TIME)  $3$4 -termid- -process/;
    s/^(LOCK INFO         : .*)    tty\d\d    \d\d\d\d/$1-termid- -process/;
    s/^(LOCK INFO         : .*)             \d\d\d\d(.*)/$1-termid- -process/;
    s/^(LOCK INFO         :     ...............)(\(....s\) )(\d\d\d\d\d\d\d\d\.\d\d\d\d\d\d )(.............)(.*) \d\d\d\d(.*)/$1(TIME)  $3$4-termid- -process/;
    s/^(LOCK INFO         :     .+)(\d\d\d\d\d\d\d\d\.\d\d\d\d\d\d )(\S* +)(.+)\d\d\d\d(.*)/$1  $2$3-termid- -process/;
}
if ($RELVER eq "R61"){
    s/^(LOCK INFO         :     ...............)(\(...s\)  )(\d\d\.\d\d\d\d\d\d\d\d\.\d\d\d\d\d\d )(............) ...... (.*) \d\d\d\d(.*)/$1(TIME)  $3$4 -termid- -process/;
    s/^(LOCK INFO         :     ...............)(\(...s\)  )(\d\d\.\d\d\d\d\d\d\d\d\.\d\d\d\d\d\d )(.............)(.*) \d\d\d\d(.*)/$1(TIME)  $3$4-termid- -process/;
	s/^(CURRENT VERSION .. KERNEL) 6\.1\.\d\d\.\d\d   ..(.*)/$1 6.1.XX.XX     $2/;
	s/^(KERNEL VERSION ... KERNEL) 6\.1\.\d\d\.\d\d   ..(.*)/$1 6.1.XX.XX     $2/;
}


    s/^(TERMID            :  )(.*)/$1<termid>/;
    s/^(OBJECTID          :  )(.*)/$1<objectid>/;
    s/^(SENDERID          :)(.*)/$1  <senderid>/;
    s/^(SERVERDB          :  )$DBNAME/$1<serverdb>/i;

    s/^(\s*Root pno\s*\|\s*)\d+/$1<root>/i;
    s/^(\s*ROOT\d?\s*:)\s*\d+/$1        <root>/i;

    s/^TASKID            :           \d\d/TASKID            :           ID/;
    s/^FILE_ID           :  ........FFFFFFFF7FFFFFFF0000FFFF........0100000A....000000000000..../FILE_ID           :  ........FFFFFFFF7FFFFFFF0000FFFF........0100000A....000000000000..../;

    s/^(PARSEID           :  00000)...0000(.*)/$1XYZ0000$2/;
    s/^(SELECT_PARSEID    :  00000)...0000(.*)/$1XYZ0000$2/;
    s/^CMDID             :  0000000000000.../CMDID             :  0000000000000XYZ/;
    s/^SESSION           :  00000.../SESSION           :  00000XYZ/;

    s/^(Kernel\s*Version\s*:\s*|\s*Kernel\s*|\s*)\d\.\d\.\d(\s*|\s*)\d\d\d-\d\d\d-\d\d\d-\d\d\d/$1x.x.x$2xxx-xxx-xxx-xxx/;
    s/$TODAY/DATEXXXX/g;
    s/$YESTERDAY/DATEXXXX/g;
    s/$VORGESTERN/DATEXXXX/g;
    s/19991231/DATEXXXX/g;
    s/DATEXXXX\d{12}/TIMESTAMPXXXXXXXXXXX/g;
    s/^CONNECT ([A-Z]*) IDENTIFIED BY :X/CONNECT $1/g;
    s/ 20\d\d-[01]\d-[0-3]\d / YYYY-MM-DD /g;
    s/ 20\d\d-[01]\d-[0-3]\d$/ YYYY-MM-DD/;
    s/ [0-3]\d\.[01]\d\.20\d\d / DD.MM.YYYY /g;
    s/ [0-3]\d\.[01]\d\.20\d\d$/ DD.MM.YYYY/;
    s/^[0-3]\d\.[01]\d\.20\d\d /DD.MM.YYYY /;
    s/ [01]\d\/[0-3]\d\/9\d / MM\/DD\/YY /g;
    s/ [01]\d\/[0-3]\d\/9\d$/ MM\/DD\/YY/;
    s/^[01]\d\/[0-3]\d\/9\d / MM\/DD\/YY /;
    s/ 20\d\d-[01]\d-[0-3]\d / YYYY-MM-DD /g;
    s/ 20\d\d-[01]\d-[0-3]\d$/ YYYY-MM-DD/;
    s/ [0-3]\d\.[01]\d\.20\d\d / DD.MM.YYYY /g;
    s/ [0-3]\d\.[01]\d\.20\d\d$/ DD.MM.YYYY/;
    s/^[0-3]\d\.[01]\d\.20\d\d /DD.MM.YYYY /;
    s/ [01]\d\/[0-3]\d\/0\d / MM\/DD\/YY /g;
    s/ [01]\d\/[0-3]\d\/0\d$/ MM\/DD\/YY/;
    s/^[01]\d\/[0-3]\d\/0\d / MM\/DD\/YY /;
	s/^DATETIME          :  [0-3]\d-...-99/DATETIME          :  DD-MON-YY/;
	s/^DATETIME          :  [0-3]\d-...-00/DATETIME          :  DD-MON-YY/;
	s/^DATETIME          :  [0-3]\d-...-01/DATETIME          :  DD-MON-YY/;
	s/^DATETIME          :  [0-3]\d-...-02/DATETIME          :  DD-MON-YY/;
	s/^DATETIME          :  [0-3]\d-...-03/DATETIME          :  DD-MON-YY/;
	s/^DATETIME          :  [0-3]\d-...-04/DATETIME          :  DD-MON-YY/;
	s/^DATETIME          :  [0-3]\d-...-05/DATETIME          :  DD-MON-YY/;
	s/^DATETIME          :  [0-3]\d-...-06/DATETIME          :  DD-MON-YY/;
    s/ 000\d[0-5]\d[0-5]\d / TIMEYYYY /g;
    s/ 001\d[0-5]\d[0-5]\d / TIMEYYYY /g;
    s/ 002[0-3][0-5]\d[0-5]\d / TIMEYYYY /g;
    s/ 000\d[0-5]\d[0-5]\d$/ TIMEYYYY/;
    s/ 001\d[0-5]\d[0-5]\d$/ TIMEYYYY/;
    s/ 002[0-3][0-5]\d[0-5]\d$/ TIMEYYYY/;
    s/=000\d[0-5]\d[0-5]\d /=TIMEYYYY /g;
    s/=001\d[0-5]\d[0-5]\d /=TIMEYYYY /g;
    s/=002[0-3][0-5]\d[0-5]\d /=TIMEYYYY /g;
    s/=000\d[0-5]\d[0-5]\d$/=TIMEYYYY/;
    s/=001\d[0-5]\d[0-5]\d$/=TIMEYYYY/;
    s/=002[0-3][0-5]\d[0-5]\d$/=TIMEYYYY/;
    s/TIME=0\d[0-5]\d[0-5]\d$/TIME=TIMEYYYY/;
    s/TIME=1\d[0-5]\d[0-5]\d$/TIME=TIMEYYYY/;
    s/TIME=2[0-3][0-5]\d[0-5]\d$/TIME=TIMEYYYY/;
    s/ [0-2]\d:[0-5]\d:[0-5]\d / hh:mm:ss /g;
    s/ [0-2]\d:[0-5]\d:[0-5]\d$/ hh:mm:ss/;
    s/ [0-2]\d\.[0-5]\d\.[0-5]\d / hh.mm.ss /g;
    s/ [0-2]\d\.[0-5]\d\.[0-5]\d$/ hh.mm.ss/;
    s/^[0-2]\d\.[0-5]\d\.[0-5]\d /hh.mm.ss /;
    s/^[0-2]\d:[0-5]\d:[0-5]\d /hh:mm:ss /;
    s/ ker2/ kern/;
    s/\.\.\/DEV150/DEV150/;
    s/\.\.\/DEV151/DEV151/;
#
# jrg 5.6.2000 Die sut '.punix' Files enthalten hart kodiert '.bea.sap-ag.de'
# Mit der Umstellung auf 'no sockets' im Kern (PTS 1106771) ist der Domainname nicht mehr
# ermittelbar (da in der libsocket enthalten). Daher wird hier künstlich ein .bea.sap-ag.de
# angefügt und in einem zweiten Schritt eventuelle doppelte .bea.sap-ag.de.bea.sap-ag.de auf
# ein einzelnes .bea.sap-ag.de reduziert. Betroffen waren
# &gar 22.06.2000
# beim zweiten Schritt wird neben der Domaine bea auch auf wdf getestet und diese abgeschnitten
#
# SERVERNODE, APPL_NODEID, APPL_NODE, DB_IDENT, Database ID und Server
#
    s/^(SERVERNODE        :  )$NODE.*/$1<servernode>/i;
    s/^(APPL_NODEID       :  )$APPLNODE.*/$1<appl_nodeid>/i;
    s/^(APPL_NODE *: +)$APPLNODE.*/$1<appl_node>/i;
    s/^(APPL_PROCESS      :  )[- 0-9]*/$1<appl_process>/;
    s/^(SERVERDB \.\.\.\.\.\.\.\.\. )$DBNAME.*/$1<serverdb>/i;
    s/^(SERVERNODE \.\.\.\.\.\.\. )$NODE.*/$1<servernode>/i;
    s/(serverdb  : )$DBNAME.*/$1<serverdb>/i;
    s/(servernode: )$NODE.*/$1<servernode>/i;
    s/^(\s*DB_IDENT\.\.\.\.\.\.\.\.\.\.\s*)$NODE:$DBNAME(_DATEXXXX_)\d{6}/$1<servernode>:<serverdb>$2<timestamp>/i;
    s/^(\s*DB_IDENT\.\.\.\.\.\.\.\.\.\.\s*)$NODE\..*\.de:$DBNAME(_DATEXXXX_)\d{6}/$1<servernode>:<serverdb>$2<timestamp>/i;
    s/^(\s*DB IDENT FROM RESTARTRECORD\s*=\s*)$NODE.*:$DBNAME(_DATEXXXX_)\d{6}/$1<servernode>:<serverdb>$2<timestamp>/i;
    s/^(\s*DB IDENT FROM LOGINFO\s*=\s*)$NODE.*:$DBNAME(_DATEXXXX_)\d{6}/$1<servernode>:<serverdb>$2<timestamp>/i;
if ($RELVER ge "R72")
{
# new save output with dbmcli
    s/^(Database ID          )$NODE\..*\.corp:$DBNAME(_DATEXXXX_)\d{6}/$1<servernode>:<serverdb>$2<timestamp>/i;
    s/^(Server               )$NODE\..*\.corp/$1<servernode>/i;
    s/^(Database             )$DBNAME/$1<serverdb>/i;
#
# jrg 5.6.2000 siehe oben
#
    s/^(Database ID          )$NODE:$DBNAME(_DATEXXXX_)\d{6}/$1<servernode>:<serverdb>$2<timestamp>/i;
    s/^(Server               )$NODE/$1<servernode>/i;
    s/^(Max Used Data Page  ) \d+/${1}000/i;

    s/^(Label *)DAT_\d+/${1}<label>/i;

    # installation as part of xinitdb

	if ( $TFN =~ /^(xinitdb|vzloadsys)/i ) {
        s/^0,.*xload.*$/0,"calling xload"/;
        s/^.*n"$//;
    #  Unix ohne Anführungsstriche
		s/^.*domain$//;
    }
    s'^/ M LOAD.*$'/ M LOAD';
    s'^/ M "[^"]+" on.*$'/ M "XXX" on XXX';
    s'^/ F.*$'/ F';
    s/^(. M Last transaction committed at input line) .*$/$1 000/;
    s/^(. M Sum of inserted lines) \d+(, sum of rejected lines) \d+.*$/$1 000$2 000/;
}
if ($RELVER eq "R61")
{
    # in 6.1 the servernode is not known by the application in Windows NT
    s/^(SERVERNODE        :)$/$1  <servernode>/i;
    s/^(SERVERNODE \.\.\.\.\.\.\.)$/$1 <servernode>/i;
    s/(servernode:)$/$1 <servernode>/i;
}
    s/(FOR ')$DBNAME(')/$1<serverdb>$2/i;
    s/tty\d\d.*${DBNAME}/<termid>           \| <serverdb>/;
    s/^(PROCESS *: *)\d+/${1}1/;
    s/^(EXPRESSION1       :  )$DBNAME/${1}<serverdb>/i;
    s/^(KERNEL *: *)(SLOWKNL|KERNEL|QUICKKNL) *\d+\.\d+.\d+ *Build *\d+-\d+-\d+-\d+/${1}KERNEL    vv.vv.vv Build bbb-bbb-bbb-bbb/i;
    s/^(VALUE *: *)(SLOWKNL|KERNEL|QUICKKNL) *\d+\.\d+.\d+ *Build *\d+-\d+-\d+-\d+/${1}KERNEL    vv.vv.vv Build bbb-bbb-bbb-bbb/i;
    s/^(KERNEL VERSION *\.* *)(SLOWKNL|KERNEL|QUICKKNL) *\d+\.\d+.\d+ *Build *\d+-\d+-\d+-\d+/${1}KERNEL    vv.vv.vv Build bbb-bbb-bbb-bbb/i;
    s/^(\d+ *CON.rel_vers: *)(SLOWKNL|KERNEL|QUICKKNL) *\d+\.\d+.\d+ *Build *\d+-\d+-\d+-\d+/${1}KERNEL    vv.vv.vv Build bbb-bbb-bbb-bbb/i;
    s/^(\d+ *release_vers: *)(SLOWKNL|KERNEL|QUICKKNL) *\d+\.\d+.\d+ *Build *\d+-\d+-\d+-\d+/${1}KERNEL    vv.vv.vv Build bbb-bbb-bbb-bbb/i;
    s/^(Kernel Version : \| )(QuickKnl)/${1}Kernel  /i;
    s/^(RUNTIMEENVIRONMENT *: *).* *\d+\.\d+.\d+ *Build *\d+-\d+-\d+-\d+/${1}RTE       vv.vv.vv Build bbb-bbb-bbb-bbb/i;
    s/^(UTILITY *)\d+\.\d+.\d+ *Build *\d+-\d+-\d+-\d+/${1}vv.vv.vv Build bbb-bbb-bbb-bbb/i;
    s/^(LOCK INFO\s*:\s*)\d{2}\.\d{8}\.\d{6}\s*(\S*)\s*$NODE\s*\S*\s*\S*\s*(LOCK.*)/$1<trans> $2 <node> <termid> <process> $3/i;
if (($RELVER ge "R73") || (($RELVER eq "R72") && (($ENV{CORRECTION_LEVEL} > 1) || ($ENV{BUILDPRAEFIX} >= 5)))) {
    s/^(Pages Transferred *)\d+$/${1}000/;
    s/^(First LOG Page *)\d+$/${1}000/;
    s/^(Page Count *)\d+$/${1}000/;
}
if (($RELVER gt "R74") || (($RELVER eq "R74") && ($CORRECTION_LEVEL gt "1")))
{
    # to avoid differences with explain join
    s/^(MAX ADDNL\. ROWS *: *\d\.\d{7})\d{10}(E\+\d\d)/${1}0000000000${2}/i;
    s/^(ADDITIONAL ROWS *: *\d\.\d{7})\d{10}(E\+\d\d)/${1}0000000000${2}/i;
    s/^(ACCUMULATED_COSTS *: *\d\.\d{7})\d{10}(E\+\d\d)/${1}0000000000${2}/i;
    s/^(1A000000000000\d\d\d\d0000000)(000000)(\d)/${1}${3}${2}/i;
}

if ($RELVER ge "R74"){
    s/^(Location[ ]*).*xinitdb.sav/$1xinitdb.sav/g;
	s/^(Location[ ]*).*yinitdb.sav/$1yinitdb.sav/g;
	s/^(Location[ ]*).*(savetest.*)/$1$2/g;
	s/^(Location[ ]*).*(host.\.host.*)/$1$2/g;

    s/^(CAT_C_SIZ\s*:\s*)\d+/$1<catsize>/i;
    s/^(CATALOG_CACHE_SIZE\s*:\s*)\d+/$1<catsize>/i;

    s/(.*Build )\d\d\d-\d\d\d-\d\d\d-\d\d\d/$1xxx-xxx-xxx-xxx/i;


}

if ($RELVER ge "R75"){
    s/^(TABLEID           :  0000000000000).../$1XYZ/g;
    s/^(SESSION           :          ).../$1XYZ/g;
    s/^(WRITE_TRANS       :  000000000).../$1XYZ/g;
    s/^(TRANSCOUNT        :                   ).../$1XYZ/g;
}

if ( $TFN =~ /^(sysdd)/i ) {
    s/ACTIVATE_COUNT    : *\d+/ACTIVATE_COUNT    :                    XYZ/;
    s/DBPID             : *\d+/DBPID             :          XYZ/;
    s/LOCKS             :                   *\d+/LOCKS             :                    XYZ/;
        s/WAITS             :                   *\d+/WAITS             :                    XYZ/;
    s/COLLISIONS        : *\d+/COLLISIONS        :          XYZ/;
        s/COLLISION_RATE    : *\d+.\d+/COLLISION_RATE    :       AB.XYZ/;
    s/TOTAL_SPIN_LOOPS  : *\d+/TOTAL_SPIN_LOOPS  :          XYZ/;
    s/MAX_SPIN_LOOPS    : *\d+/MAX_SPIN_LOOPS    :          XYZ/;
    s/DISPATCHCNT       : *\d+/DISPATCHCNT       :                    XYZ/;
    s/TASKSWITCHCNT     : *\d+/TASKSWITCHCNT     :                    XYZ/;
    s/IDLECOUNT         : *\d+/IDLECOUNT         :                    XYZ/;
    s/TOTAL_YIELD_LOOPS : *\d+/TOTAL_YIELD_LOOPS :          XYZ/;
    s/MAX_YIELD_LOOPS   : *\d+/MAX_YIELD_LOOPS   :          XYZ/;
        s/UKTID             :            \d/UKTID             :            X/;
        s/APPLPID           : *\d+/APPLPID           :<applprocess>/i;
        s/PEERPID           : *\d+/PEERPID           :          XYZ/;
        s/OSPID             : *\d+/OSPID             :          XYZ/;
        s/^TASKINDEX         :           \d\d/TASKINDEX         :           ID/;
        s/^TEST_AND_SET      :                      \d/TEST_AND_SET      :                      X/;
    }

	if ( $TFN =~ /^(sysdd80)/i ) {
        s/PREPARETIME       : *\d+/PREPARETIME       :                    XYZ/;
        s/^(NODE              :  )$NODE.*/$1<servernode>/i;
        s/^(LOCALNODE         :  )$NODE.*/$1<servernode>/i;
        s/PHYSICALMEMORYU...: *\d+/PHYSICALMEMORYU...:          XYZ/;
        s/VIRTUALMEMORYUS...: *\d+/VIRTUALMEMORYUS...:          XYZ/;
        s/PROCESSUSEDUSER...: *\d+/PROCESSUSEDUSER...:          XYZ/;
        s/MACHINEUSEDUSER...: *\d+/MACHINEUSEDUSER...:          XYZ/;
        s/MACHINEUSEDSYST...: *\d+/MACHINEUSEDSYST...:          XYZ/;
        s/MACHINEIDLETIME   : *\d+/MACHINEIDLETIME   :          XYZ/;
        s/CPULOAD           : *\d+.\d+/CPULOAD           :       AB.XYZ/;
        s/^(APPLICATIONNODE   :  )$APPLNODE.*/$1<appl_nodeid>/i;
        s/^(APPLICATIONPROCESS:  )[- 0-9]*/$1<appl_process>/i;
        s/COLLISIONCOUNT    : *\d+/COLLISIONCOUNT    :                    XYZ/;
        s/LOCKCOUNT         : *\d+/LOCKCOUNT         :                    XYZ/;
        s/SPINLOOPCOUNT     : *\d+/SPINLOOPCOUNT     :          XYZ/;
        s/MAXSPINLOOPCOUNT  : *\d+/MAXSPINLOOPCOUNT  :          XYZ/;
        s/OPERATINGSYSTEM   : .*/OPERATINGSYSTEM   :  <operating system>/;
        s/ADDRESS           :  0x......../ADDRESS           :  0x01234567/;
        s/YIELDLOOPCOUNT    : *\d+/YIELDLOOPCOUNT    :          XYZ/;
        s/MAXYIELDLOOPCOUNT : *\d+/MAXYIELDLOOPCOUNT :          XYZ/;
    }

	if ( $TFN =~ /^(n31strin)/i ) {
        s/DATEXXXX \| 003\d00300030003\d003\d003\d003\d003\d/DATEXXXX | 000d000a000t000e000x000x000x000x/;
        s/EXPRESSION1       :  003200300030003\d003\d003\d003\d003\d003\d003\d003\d003\d003\d003\d003\d003\d/EXPRESSION1       :  000t000i000m000e000s000t000a000m000p000x000x000x000x000x000x000x/;
    }

    print FILE_OUT $_;


}

close(FILE_OUT); close(FILE_IN);

if ( $DBREMOTE ne "" ) {
	$tmpFile = "$TMP/ss_rem$$";
	unlink $tmpFile; rename($OUTFILE, $tmpFile) || die "Can't rename $OUTFILE to $tmpFile: $!";

	open(FILE_IN, "<$tmpFile") || die "Can't open $tmpFile (input): $!";
	open(FILE_OUT, ">$OUTFILE") || die "Can't open $OUTFILE (output): $!";

	while(<FILE_IN>) {

		chop;
		s/^(SERVERDB          :  )$DBLOCAL/$1<localdb>/;
		if (/\/\*\@\@CLEAR LOG/) {
			$_ .= "\n";
			$len1 = length;
			$_ .= <FILE_IN>;
			chop if $len1 < length;
		}
		if ($iter = (/\/\*\@\@CLEAR LOG *\n/)) {
				if ($iter == 1) { print FILE_OUT "CLEAR LOG\n"; }
				next;
		}
		s/\/\*@#@/ /g;
		s/\/\*@@//;
	}
	continue {
		print FILE_OUT "$_\n";
	}
	close(FILE_OUT); close(FILE_IN);
	unlink $tmpFile;
}

# YYYY-MM-DD
# DD.MM.YYYY
# MM/DD/YYYY
# Today, yesterday, vorgestern interspersed with 'F' become FXFXFXFXFXFXFXFX
# Today, yesterday, vorgestern interspersed with '3' become 3X3X3X3X3X3X3X3X
# Today/yesterday/vorgestern's year interspersed with 'F' becomes FXFXFXFX
$YMD = "YYYY-MM-DD";
$MDY = "MM/DD/YYYY";
open(FILE_IN, "<$OUTFILE") || die "Can't open $OUTFILE (input): $!";
open(FILE_OUT, ">$TMP/sed.$$") || die "Can't open $TMP/sed.$$ (output): $!";

while(<FILE_IN>) {

    s/$TODAY_ORA/DD-MMM-YY/g;
    s/$TODAY_YMD/$YMD/g;
    s/$YESTR_YMD/$YMD/g;
    s/$VORGE_YMD/$YMD/g;
    s/$TODAY_DMY/DD.MM.YYYY/g;
    s/$YESTR_DMY/DD.MM.YYYY/g;
    s/$VORGE_DMY/DD.MM.YYYY/g;
    s/$TODAY_MDY/$MDY/g;
    s/$YESTR_MDY/$MDY/g;
    s/$VORGE_MDY/$MDY/g;
    s/$TODAY_F/FXFXFXFXFXFXFXFX/g;
    s/$YESTR_F/FXFXFXFXFXFXFXFX/g;
    s/$VORGE_F/FXFXFXFXFXFXFXFX/g;
    s/$TODAY_3/3X3X3X3X3X3X3X3X/g;
    s/$YESTR_3/3X3X3X3X3X3X3X3X/g;
    s/$VORGE_3/3X3X3X3X3X3X3X3X/g;
    s/$TODAY_FY/FXFXFXFX/g;
    s/$YESTR_FY/FXFXFXFX/g;
    s/$VORGE_FY/FXFXFXFX/g;
    print FILE_OUT $_;

}

close(FILE_OUT); close(FILE_IN);
unlink $OUTFILE; rename("$TMP/sed.$$", $OUTFILE) || die "Can't rename $TMP/sed.$$ to $OUTFILE: $!";

if ($RELVER ge "R72"){
# FUNC: the last 2 digits of real values will be set to zero
# to avoid differences

	if ( $TFN =~ /^(func|sapr3)/i ) {
	open(FILE_IN, "<$OUTFILE") || die "Can't open $OUTFILE (input): $!";
	open(FILE_OUT, ">$TMP/sed.$$") || die "Can't open $TMP/sed.$$ (output): $!";
	$ZERO = "0";

	while(<FILE_IN>) {

		s/^(EXPRESSION1       :  .\d\.\d{12})\d\d(0{23}E.\d\d)/$1$ZERO$ZERO$2/g;
		print FILE_OUT $_;
		}
	close(FILE_OUT); close(FILE_IN);
	unlink $OUTFILE; rename("$TMP/sed.$$", $OUTFILE) || die "Can't rename $TMP/sed.$$ to $OUTFILE: $!";
	}
}

# C05TIME, UNICODE: Get current time from "DATEXXXXhhmmsszzzzzz"
if ( $TFN =~ /^(c05time|unicode)/i ) {
	undef %WORD_LIST;
	open(FILE_IN, "<$OUTFILE") || die "Can't open $OUTFILE (input): $!";
	while(<FILE_IN>) {
		chop;
		if (/DATEXXXX\d{12}/) {
			$WORD_LIST{$&} = 1;
		}
	}
	close(FILE_IN);

	# Then look for these kinds of current time
	# in a 12 hour format
	#   "MM/DD/YYYY | hh:mm [AP]M"
	#   ">hh:mm [AP]M<"
	# and in a 24 hour format
	#   ">hh.mm.ss<"
	#   ">hh:mm:ss<"
	#   ">00hhmmss<"
	#   "00hhmmss" alone on line

	foreach $WORD ( keys %WORD_LIST) {

		$NOW_24H = substr($WORD, 8, 2);

		if ( $NOW_24H <= 11 ) {
			$MERIDIAN = "AM";				# morning
			if ( $NOW_24H eq "00" ) {
				$NOW_12H = "12";
			}								# midnight is 12 AM
			else {
				$NOW_12H = $NOW_24H	}		# 01..11 AM
		}
		else {
			$MERIDIAN = "PM";				# afternoon & night
			if ( $NOW_24H eq "12" ) {
				$NOW_12H = "12";			# noon is 12 PM
			}
			else {
				$NOW_12H = $NOW_24H - 12;	# 01..11 PM
				$NOW_12H = "0$NOW_12H" if length($NOW_12H)==1;
			}
		}
		$NOW_M = substr($WORD, 10, 2);	# Minuten
		$NOW_S = substr($WORD, 12, 2);	# Sekunden

		open(FILE_IN, "<$OUTFILE") || die "Can't open $OUTFILE (input): $!";
		open(FILE_OUT, ">$TMP/sed.$$") || die "Can't open $TMP/sed.$$ (output): $!";

		while(<FILE_IN>) {

			s/($MDY | )$NOW_12H:$NOW_M $MERIDIAN/$1hh:mm XM/;
			s/>$NOW_12H:$NOW_M $MERIDIAN</>hh:mm XM</g;
			s/>$NOW_24H\.$NOW_M\.$NOW_S</>hh.mm.ss</g;
			s/>$NOW_24H:$NOW_M:$NOW_S</>hh:mm:ss</g;
			s/>00$NOW_24H$NOW_M$NOW_S</>00hhmmss</g;
			s/^00$NOW_24H$NOW_M$NOW_S$/00hhmmss/;
			print FILE_OUT $_;

		}
		close(FILE_IN); close(FILE_OUT);
		unlink $OUTFILE; rename("$TMP/sed.$$", $OUTFILE) || die "Can't rename $TMP/sed.$$ to $OUTFILE: $!";
	}
}

# C05TIME: Get current time from "YYYY-MM-DD-hh.mm.ss.zzzzzz"
if ( $TFN =~ /^c05time/i ) {
	undef %WORD_LIST;
	open(FILE_IN, "<$OUTFILE") || die "Can't open $OUTFILE (input): $!";
	while(<FILE_IN>) {
		chop;
		if (/$YMD-(\d\d\.){3}\d{6}/) {
			$_ = $&;
			if (/^$YMD-[0-2][0-3]\.[0-5]\d\.[0-5]\d\.(\d{6})$/ or
				/^$YMD-[0-1][4-9]\.[0-5]\d\.[0-5]\d\.(\d{6})$/ ) {
				$tmp = $1; ( $tmp ) = $tmp =~ /^0*(\d+)/;
				$WORD_LIST{$tmp} = 1;
			}
		}
# rel >= 7.3
		if (/$YMD \d\d:\d\d:\d\d\.\d{6}/) {
			$_ = $&;
			if (/^$YMD [0-2][0-3]:[0-5]\d:[0-5]\d\.(\d{6})$/ or
				/^$YMD [0-1][4-9]:[0-5]\d:[0-5]\d\.(\d{6})$/ ) {
				$tmp = $1; ( $tmp ) = $tmp =~ /^0*(\d+)/;
				$WORD_LIST{$tmp} = 1;
			}
		}
	}
	close(FILE_IN);

	# Milliseconds: their leading zeroes have become blanks
	open(FILE_IN, "<$OUTFILE") || die "Can't open $OUTFILE (input): $!";
	open(FILE_OUT, ">$TMP/sed.$$") || die "Can't open $TMP/sed.$$ (output): $!";
	while(<FILE_IN>) {
		chop;
		if ( /(       : +)/ ) {
			foreach $WORD (keys %WORD_LIST) {
				s/(       : +)$WORD$/$1zzzzzz/;
			}
		}
		print FILE_OUT "$_\n";
	}
	close(FILE_OUT); close(FILE_IN);
	unlink $OUTFILE; rename("$TMP/sed.$$", $OUTFILE) || die "Can't rename $TMP/sed.$$ to $OUTFILE: $!";
}

# CONSTR: Get current time from "^MM/DD/YYYY | hh:mm [AP]M$"
if ( $TFN =~ /^constr$/i ) {
	@WORD_LIST = ();
	open(FILE_IN, "<$OUTFILE") || die "Can't open $OUTFILE (input): $!";
	while(<FILE_IN>) {
		chomp;
		if ( /^$MDY | \d\d:\d\d [AP]M$/ ) {
			s/^$MDY//;
			s/ \| //;
			s/ [AP]M$//;
			push @INPUT, $_;
		}
	}
	close(FILE_IN);
	foreach $WORD (@INPUT) {
			if ( $WORD =~ /^[0-2][0-3]:[0-5][0-9]$/ ) { push @WORD_LIST, $WORD }
			elsif ( $WORD =~ /^[0-1][4-9]:[0-5][0-9]$/ ) { push @WORD_LIST, $WORD }
	}

	# Then look for these kinds of current time
	#   "^MM/DD/YYYY | hh:mm [AP]M$"
	#   " :  hh:mm [AP]M$"
	foreach $WORD (@WORD_LIST) {

		next if $OLD_WORD eq $WORD;
		$OLD_WORD = $WORD;

		open(FILE_IN, "<$OUTFILE") || die "Can't open $OUTFILE (input): $!";
		open(FILE_OUT, ">$TMP/sed.$$") || die "Can't open $TMP/sed.$$ (output): $!";

		while(<FILE_IN>) {

			s/^($MDY \| )$WORD [AP]M$/$1hh:ss XM/;
			s/ :  $WORD [AP]M$/ :  hh:ss XM/;
			print FILE_OUT $_;

		}
		close(FILE_IN); close(FILE_OUT);

		unlink $OUTFILE; rename("$TMP/sed.$$", $OUTFILE) || die "Can't rename $TMP/sed.$$ to $OUTFILE: $!";
	}
}

if ( $TFN =~ /^(c05time|fokus|odbc|db2|sqlmode|n40date|monitor)/i ) {
	# YYYY-MM-DD[- ]hh[.:]mm[.:]ss.zzzzzz before space or eol
	# DATEXXXXTIMEYYYYYYYY before space or eol
	# DATEXXXXTIMEYYYY before eol
	$DOT   = "hh.mm.ss.zzzzzz";
	$COLON = "hh:mm:ss.zzzzzz";
	$DX    = "DATEXXXX";

	open(FILE_IN, "<$OUTFILE") || die "Can't open $OUTFILE (input): $!";
	open(FILE_OUT, ">$TMP/sed.$$") || die "Can't open $TMP/sed.$$ (output): $!";

	while(<FILE_IN>) {

		s/($YMD[- ])[0-2][0-3](\.[0-5]\d){2}\.\d{6} /$1$DOT /g;
		s/($YMD[- ])[0-1][4-9](\.[0-5]\d){2}\.\d{6} /$1$DOT /g;
		s/($YMD[- ])[0-2][0-3](\.[0-5]\d){2}\.\d{6}$/$1$DOT/;
		s/($YMD[- ])[0-1][4-9](\.[0-5]\d){2}\.\d{6}$/$1$DOT/;
		s/($YMD[- ])[0-2][0-3](:[0-5]\d){2}\.\d{6} /$1$COLON /g;
		s/($YMD[- ])[0-1][4-9](:[0-5]\d){2}\.\d{6} /$1$COLON /g;
		s/($YMD[- ])[0-2][0-3](:[0-5]\d){2}\.\d{6}$/$1$COLON/;
		s/($YMD[- ])[0-1][4-9](:[0-5]\d){2}\.\d{6}$/$1$COLON/;
		s/($DX)[0-2][0-3]([0-5]\d){2}\d{6} /$1TIMEYYYYYYYY /g;
		s/($DX)[0-1][4-9]([0-5]\d){2}\d{6} /$1TIMEYYYYYYYY /g;
		s/($DX)[0-2][0-3]([0-5]\d){2}\d{6}$/$1TIMEYYYYYYYY/;
		s/($DX)[0-1][4-9]([0-5]\d){2}\d{6}$/$1TIMEYYYYYYYY/;
		s/($DX)00[0-2][0-3]([0-5]\d){2}$/$1TIMEYYYY/;
		s/($DX)00[0-1][4-9]([0-5]\d){2}$/$1TIMEYYYY/;
		print FILE_OUT $_;

	}
	close(FILE_IN); close(FILE_OUT);
	unlink $OUTFILE; rename("$TMP/sed.$$", $OUTFILE) || die "Can't rename $TMP/sed.$$ to $OUTFILE: $!";
}


######################## Subroutines #################################

# Compute the date ($YEAR, $MON=01..12, $DAY=01..31) previous to given date
sub prev_day {

	my($YEAR, $MON, $DAY) = @_;
	if ( $DAY > 1 ) {
		# Same year, same month
		$DAY = $DAY - 1;
		if ( $DAY < 10 ) { $DAY = "0$DAY" };
	}
	else {
		if ( $MON == 1 ) {

			# Prev year, prev month
			$YEAR = $YEAR - 1;
			$MON = 12;
			$DAY = 31;
		}
		else {
			# Same year, prev month
			$MON = $MON - 1;
			if ( $MON < 10 ) {
				$MON = "0$MON" }

			$_ = $MON;
			CASE: {

				if ( /01|03|05|07|08|10|12/ ) {
					$DAY = 31;
					last CASE;
				}

				if ( /02/ ) {
					if ( $YEAR % 4 == 0 ) { $DAY = 29 }
						else { $DAY = 28 }
					last CASE;
				}

				if ( /04|06|09|11/ ) {
					$DAY = 30;
					last CASE;
				}
			} #CASE
		}
	}
	return ("$YEAR", "$MON", "$DAY");
}


