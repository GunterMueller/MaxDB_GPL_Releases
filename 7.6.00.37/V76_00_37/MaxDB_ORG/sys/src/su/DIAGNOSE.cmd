*   *ID* DIAGNOSE DBCMD    changed on 1992-11-11-17.01.57 by KERN      *
file connect ( sut sut !
create user
* maximallaenge456789212345678931234567894123456789512345678961234
maximallaenge45678
password ddb4 dba !
commit release !
connect
* maximallaenge456789212345678931234567894123456789512345678961234
maximallaenge45678
identified by ddb4 !
CREATE TABLE T1 (S1 CHAR(5) ASCII KEY, S2 CHAR(5) ASCII, 
                 S3 CHAR(5) ASCII    , S4 CHAR(5) ASCII)!
CREATE INDEX "T1.S2"
	on T1( S2 ) !
CREATE INDEX I1 ON T1 (S3, S4)!
INSERT T1 VALUES ('aaaaa','aaaaa','aaaaa','aaaaa')!
file sel_tablestat ( MAXIMALLAENGE45678 T1 !
file sel_indexstat ( T1 T1.S2 !
file sel_indexstat ( T1 I1 !
commit work release !
utility !
connect sut identified by sut !
*
*
* NODE 1/01000000  level 0  entries: 17  perm   [page 1]
*      root  : 1/01000000             right: nil
*      bottom: 585   write_cnt: 3     cvers: 4  list_top: 4061
*      site  : 0     file_vers: dummy
*  
*   1: (pos 00041) root 4  SYS1CAT
*   2: (pos 00073) root 5  SYS2CAT
*   3: (pos 00105) root 6  TRANSFILE
*   4: (pos 00521) root 38  INV(2) 0-79
*   5: (pos 00233) root 15  INV(5) 0-69
*   6: (pos 00201) root 14  INV(8) 0-69
*   7: (pos 00553) root 39  MULT_INV(1) 0-79
*   8: (pos 00137) root 10  TABID 0-68
*   9: (pos 00169) root 13  TABID 0-69
*  
*  10: (pos 00265) root 16  TABID 0-6A
*  11: (pos 00297) root 11  TABID 0-74
*  12: (pos 00329) root 17  TABID 0-75
*  13: (pos 00361) root 23  TABID 0-76
*  14: (pos 00393) root 20  TABID 0-77
*  15: (pos 00457) root 19  TABID 0-78
*  16: (pos 00489) root 37  TABID 0-79
*  17: (pos 00425) root 25  SCOL 0-77
*
*
* write dump (warm)
DIAGNOSE DATA TO 'diagtest.tmp'
*
* get surrogate of table name
*DIAGNOSE TABID maximallaenge456789212345678931234567894123456789512345678961234.t1
DIAGNOSE TABID maximallaenge45678.t1
*
* get table name of surrogate
DIAGNOSE TABLE x'256'
*
exit
dbmcli superdba,colduser!
db_admin!
bye!

utility !
connect superdba           identified by colduser
*
* write dump (cold)
DIAGNOSE DATA TO 'diagtest.tmp'
*
DIAGNOSE DATA BUFFER ON
*
DIAGNOSE DATA BUFFER OFF
*
* ----- DIAGNOSE EXTRACT -----
*
* read data page 14888
DIAGNOSE TO 'diagtest.tmp' DATA 14888
*
* read conv page 0
DIAGNOSE TO 'diagtest.tmp' PSM 1
*
* read RESTART RECORD
DIAGNOSE TO 'diagtest.tmp' RESTART
* 
*
* read log volume pages 0 - 4
DIAGNOSE TO 'diagtest.tmp' volume LOG 1 FROM 0 TO 4
*
* read system volume pages 0 - 4
DIAGNOSE TO 'diagtest.tmp' volume CONFIG 1 FROM 0 TO 4
*
* read mirrored log devpspace pages 0 - 4
DIAGNOSE TO 'diagtest.tmp' volume SECOND LOG 1 FROM 0 TO 4
*
* SECOND CONFIG = mirrored sys dev     --> IO ERROR (not mirrored)
DIAGNOSE TO 'error.902' volume SECOND CONFIG 1 FROM 0 TO 4
*
* read SYSDEV pages 0 - 4
DIAGNOSE TO 'diagtest.tmp' volume '../SYSDEV' FROM 0 TO 4
*
* read LOG pages 17 - 19  [first lpno = 17  --> 2 pages]
DIAGNOSE TO 'diagtest.tmp' LOG FROM 17 TO 19
*
* read LOG: last 2 pages
DIAGNOSE TO 'diagtest.tmp' LOG LAST 2
*
* read SYS1CAT
DIAGNOSE TO 'diagtest.tmp' TABLE X'0000000000000001'
*
* read SYS2CAT
DIAGNOSE TO 'diagtest.tmp' TABLE X'0000000000000002'
/*
* get surrogate of table name
DIAGNOSE TABID
* maximallaenge456789212345678931234567894123456789512345678961234.t1
maximallaenge45678.t1
/*
* get table name of surrogate
DIAGNOSE TABLE x'256'
/*
* read table T1 (TABID 0-254) via root 33
DIAGNOSE TO 'diagtest.tmp' TABLE 33
/*
* read table T1 (ROOT 37) via tabid 254
DIAGNOSE TO 'diagtest.tmp' TABLE X'256'
/*
* read table T1 (ROOT 37)
DIAGNOSE TO 'diagtest.tmp' TABLE 
* maximallaenge456789212345678931234567894123456789512345678961234.T1
maximallaenge45678.T1
/*
* read second single index (ROOT 38) of table T1
DIAGNOSE TO 'diagtest.tmp' INDEX
* maximallaenge456789212345678931234567894123456789512345678961234.T1 2
maximallaenge45678.T1 2
/*
* read first multiple index (ROOT 39) of table T1
DIAGNOSE TO 'diagtest.tmp' INDEX
* maximallaenge456789212345678931234567894123456789512345678961234.T1
maximallaenge45678.T1
2   M
/*
* read second single index (ROOT 38) of table T1 via tabid 0-254
DIAGNOSE TO 'diagtest.tmp' INDEX X'256' 2
/*
* read first multiple index (ROOT 39) of table T1 via tabid 0-254
DIAGNOSE TO 'diagtest.tmp' INDEX X'256' 2 M
/*
exit
*
* ----- DIAGNOSE REPAIR -----
*
utility !
connect superdba           identified by colduser
DIAGNOSE REPAIR
colduser
superdba
*
* edit data page 14888
*        first byte := ff
*        first int4 := 14888   [reset]
EDIT PAGE
DATA PAGE
14888
hex:  ff
int4:             14888
save
*
* edit restart record
*        converter version := 0
*        int4 of converter version := 4   [reset]
EDIT PAGE
RESTART RECORD
00033 i4i4 rstCurrBupVs: 0
int4:                 5
save
*
EXIT
*
* -----------------------
*
exit

DBMCLI SUPERDBA,COLDUSER!
db_offline!
db_online!
BYE!

* ----- aufraeumen !
file connect ( sut sut !
drop user
* maximallaenge456789212345678931234567894123456789512345678961234 !
maximallaenge45678 !
commit release !
* ---ende---
file shutdown !
