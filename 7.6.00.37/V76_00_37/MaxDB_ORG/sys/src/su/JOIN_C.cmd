senderid DBM !
file connect ( sut sut ! 
set parameter OPTIMIZE_OPERATOR_JOIN = '&1' !
commit release !
senderid XCI !

*****************************************************************************
* C00ERROR.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
*
CREATE TABLE t01 (s11 CHAR(8) KEY, s12 CHAR(18) KEY)!
CREATE TABLE t02 (s21 CHAR(8) KEY, s22 CHAR(5)  KEY, s23 FIXED(3) KEY,
                  s24 CHAR(12),    s25 CHAR(18))!
COMMIT!
*
INSERT t01 VALUES ('AAAAAAA','BBBBBB')!
INSERT t02 VALUES ('AAAAAAA','CCCC',4,'AAAAAAA','BBBBBB')!
COMMIT!
*
explain SELECT * FROM <t02, t01>
  WHERE s11 = s24
    AND s12 = s25
    AND s22 = 'CCCC'
    AND s25 = 'BBBBBB'
    AND s24 = 'AAAAAAA'
    AND s11 = 'AAAAAAA'
    AND s12 = 'BBBBBB'!
SELECT * FROM <t02, t01>
  WHERE s11 = s24
    AND s12 = s25
    AND s22 = 'CCCC'
    AND s25 = 'BBBBBB'
    AND s24 = 'AAAAAAA'
    AND s11 = 'AAAAAAA'
    AND s12 = 'BBBBBB'!
FETCH !
commit release!
file cleark !
file connect ( kern test !
create table pe (
   pn       char(16) ascii key,
   gebname  char(100) ascii,
   gebdat   char(10) ascii,
   geschl   char(1) ascii,
   gebort   char(100) ascii,
   famname  char(100) ascii,
   vorname  char(100) ascii,
   titel    char(100) ascii,
   wohn     char(100) ascii,
   stras    char(100) ascii,
   land     char(100) ascii,
   staat    char(9) ascii,
   mutname  char(100) ascii,
   bem      char(200) ascii,
   bzr      char(1) ascii,
   vorl     char(1) ascii) !
commit !
create table vf (
   az       char(17) ascii key,
   vfart1   char(1) ascii,
   vfart2   char(1) ascii,
   frdaz    char(100) ascii,
   einlbeh  char(1) ascii,
   einldat  char(8) ascii,
   eingdat  char(8) ascii,
   delgr1   char(1) ascii,
   delgr2   char(1) ascii,
   delgr3   char(3) ascii,
   dezanw   char(1) ascii,
   dezkz1   char(3) ascii,
   dezkz2   char(1) ascii,
   tatvorwf char(100) ascii,
   tatort   char(100) ascii,
   tatzeit  char(17) ascii,
   asserv   char(1) ascii,
   assnum   char(8) ascii,
   anzb     char(4) ascii,
   bez      char(100) ascii,
   zknum    char(5) ascii,
   eintdat  char(8) ascii,
   einlbeh2 char(1) ascii,
   einldat2 char(8) ascii,
   wglanf   char(5) ascii,
   wgldauer char(2) ascii,
   pomap    char(1) ascii,
   bem      char(140) ascii,
   alttermin char(8) ascii,
   schaden  char(10) ascii,
   anze     char(1) ascii,
   vstatus  char(1) ascii,
   rstatus  char(1) ascii
   ) !
commit !
create table tf (
   az         char(17) ascii key,
   pn         char(16) ascii key,
   lfdnr      char(  2) ascii,
   tatvorw    char(100) ascii,
   erlart     char(  2) ascii,
   freitxt    char( 70) ascii,
   spruchk    char(  1) ascii,
   empf       char(  3) ascii,
   betrag     char(  9) ascii,
   auflerf    char(  1) ascii,
   vfgdat     char(  8) ascii,
   bescheid   char(  1) ascii,
   uebern     char(  1) ascii,
   beschw     char(  1) ascii,
   bescherl   char(  1) ascii,
   bem        char(140) ascii,
   entschantr char(  1) ascii,
   stordat    char(  8) ascii,
   amger      char( 22) ascii,
   amaz       char( 17) ascii,
   ament1     char(  1) ascii,
   ament2     char( 17) ascii,
   ament3     char(  2) ascii,
   amdat      char(  8) ascii,
   amrm       char(  1) ascii,
   lnger      char( 22) ascii,
   lnaz       char( 17) ascii,
   lnent1     char(  1) ascii,
   lnent2     char( 17) ascii,
   lnent3     char(  2) ascii,
   lndat      char(  8) ascii,
   lnrm       char(  1) ascii,
   revaz      char( 17) ascii,
   revent1    char(  1) ascii,
   revent2    char( 23) ascii,
   rkdat      char(  8) ascii,
   vrsaz      char( 17) ascii,
   pstatus    char(  1) ascii
) !
commit !
*
insert pe set pn = '1', famname = 'Eins' !
insert pe set pn = '2', famname = 'Zwei' !
insert vf set az = '5 Js 7/90', tatvorwf = 'ladendiebstahl' !
insert tf set az = '5 Js 7/90', pn = '1', lfdnr = '1' !
insert tf set az = '5 Js 7/90', pn = '2', lfdnr = '2' !
*
parsing!
select * from tf, vf, pe
 where tf.az = vf.az
   and tf.pn = pe.pn
   and tf.lfdnr = :a
   and vf.az = :b
   for reuse !
IF &rc = 0 THEN BEGIN
* Fuer Unicode sind die Ausgabespalten des letzten und des naechsten
* Staements zu lang. Mit Pars/Execute-Trennung laesst sich der
* Kontrollfluss fuer die naechsten Statements deshalb nicht zur Deckung
* bringen ausser mit dieser harten Massnahme... (CK)
executing!
data!
select!
1
c 2  '2'
c 17 '5 Js 7/90'

nodata!
END
pars_execute!
drop parseid !
1
parsing!
declare e1 cursor for 
select 
tf.az, pe.pn, lfdnr, tatvorw, erlart,
freitxt, spruchk, empf, betrag, auflerf,
vfgdat, bescheid, uebern, beschw, bescherl,
pe.bem, entschantr, stordat, amger, amaz,
ament1, ament2, ament3, amdat, amrm,
lnger, lnaz, lnent1, lnent2, lnent3,
lndat, lnrm, revaz, revent1, revent2,
rkdat, vrsaz, pstatus, vf.az, vfart1,
vfart2, frdaz, einlbeh, einldat, eingdat,
delgr1, delgr2, delgr3, dezanw, dezkz1,
dezkz2, tatvorwf, tatort, tatzeit, asserv,
assnum, anzb, bez, zknum, eintdat,
einlbeh2, einldat2, wglanf, wgldauer, pomap,
tf.bem, alttermin, schaden, anze, vstatus,
rstatus, tf.pn, gebname, gebdat, geschl,
gebort, famname, vorname, titel, wohn,
stras, land, staat, mutname, vf.bem,
bzr, vorl
from tf, vf, pe
 where tf.az = vf.az
   and tf.pn = pe.pn
   and tf.lfdnr = :a
   and vf.az = :b
   for reuse !
IF &rc = 0 THEN BEGIN
executing!
data!
select!
1
c 2  '2'
c 17 '5 Js 7/90'

nodata!
END
pars_execute!
drop parseid !
1
* - - -
* parsing!
declare e1 cursor for 
select 
tf.az, pe.pn, lfdnr, tatvorw, erlart,
freitxt, spruchk, empf, betrag, auflerf,
vfgdat, bescheid, uebern, beschw, bescherl,
pe.bem, entschantr, stordat, amger, amaz,
ament1, ament2, ament3, amdat, amrm,
lnger, lnaz, lnent1, lnent2, lnent3,
lndat, lnrm, revaz, revent1, revent2,
rkdat, vrsaz, pstatus, vf.az, vfart1,
vfart2, frdaz, einlbeh, einldat, eingdat,
delgr1, delgr2, delgr3, dezanw, dezkz1,
dezkz2, tatvorwf, tatort, tatzeit, asserv,
assnum, anzb, bez, zknum, eintdat,
einlbeh2, einldat2, wglanf, wgldauer, pomap,
tf.bem, alttermin, schaden, anze, vstatus,
rstatus, tf.pn, gebname, gebdat, geschl,
gebort, famname, vorname, titel, wohn,
stras, land, staat, mutname, vf.bem,
bzr, vorl
from tf, vf, pe
 where tf.az = vf.az
   and tf.pn = pe.pn
   and tf.lfdnr = '2'
   and vf.az = '5 Js 7/90'
   for reuse !
commit   release!
file cleark !
file connect ( kern test !
CREATE TABLE "RN0014            "
("CO_ID             " CHAR (4) KEY,
 "AST_GRP       " CHAR (3) KEY,
 "BUSI_YR           " FIXED(4,0)        KEY,
 "AST_ID        " CHAR (6) KEY,
 "D_TYP_ID       " CHAR (2) KEY,
 "AST_CLA       " CHAR (1)
 DEFAULT            ' ',
 "UNIT         " FIXED(4,0)
 DEFAULT                0,
 "UNIT_DSPL    " FIXED(4,0)
 DEFAULT                0,
 "UNIT_TRAN    " FIXED(4,0)
 DEFAULT                0,
 "UNIT_ADD     " FIXED(4,0)
 DEFAULT                0,
 "VAL          " FIXED(13,2)
 DEFAULT                       0.00,
 "VAL_HDM      " FIXED(13,2)
 DEFAULT                       0.00,
 "DEPR         " FIXED(13,2)
 DEFAULT                       0.00,
 "D_HDM     " FIXED(13,2)
 DEFAULT                       0.00,
 "ADJ          " FIXED(13,2)
 DEFAULT                       0.00,
 "ADJ_HDM      " FIXED(13,2)
 DEFAULT                       0.00,
 "DSPL         " FIXED(13,2)
 DEFAULT                       0.00,
 "DSPL_HDM     " FIXED(13,2)
 DEFAULT                       0.00,
 "TRAN         " FIXED(13,2)
 DEFAULT                       0.00,
 "TRAN_HDM     " FIXED(13,2)
 DEFAULT                       0.00,
 "ADD          " FIXED(13,2)
 DEFAULT                       0.00,
 "ADD_HDM      " FIXED(13,2)
 DEFAULT                       0.00,
 "D_BAS_VAL      " FIXED(13,2)
 DEFAULT                       0.00,
 "D_VAL          " FIXED(13,2)
 DEFAULT                       0.00,
 "D_BAS_HDM      " FIXED(13,2)
 DEFAULT                       0.00,
 "D_VAL_HDM      " FIXED(13,2)
 DEFAULT                       0.00,
 "D_IND          " CHAR (1)
 DEFAULT            ' ',
 "D_ADJ          " FIXED(13,2)
 DEFAULT                       0.00,
 "D_ADJ_HDM      " FIXED(13,2)
 DEFAULT                       0.00,
 "D_DL         " FIXED(13,2)
 DEFAULT                       0.00,
 "D_DL_HDM     " FIXED(13,2)
 DEFAULT                       0.00,
 "D_TR         " FIXED(13,2)
 DEFAULT                       0.00,
 "D_TR_HDM     " FIXED(13,2)
 DEFAULT                       0.00,
 "D_END_DT       " DATE
 DEFAULT            '19000101',
 "LAST_D_DT      " DATE
 DEFAULT            '19000101',
 "DT_STA_DEPR       " DATE
 DEFAULT            '19000101') !
CREATE INDEX RN0014_X1 ON RN0014
    (CO_ID ASC,
     BUSI_YR ASC,
     AST_GRP ASC,
     D_TYP_ID ASC,
     AST_ID ASC) !
commit work !
select RN0014.AST_id, RN0014.AST_grp, RN0014.val,
b.AST_id, b.AST_grp, b.val, RN0014.val+b.val
from <rn0014   , rn0014 b>
where RN0014.co_id = '507V' and RN0014.busi_yr = 1990
and b.co_id =RN0014.co_id and b.busi_yr = RN0014.busi_yr
and RN0014.AST_grp in
('410','420','430','490','511','520','530','610','620','630')
and b.AST_grp in
('410','420','430','490','511','520','530','610','620','630')
and RN0014.D_typ_id = '01' and   b.D_typ_id = '01'
and RN0014.val < 142499 and RN0014.AST_id <> b.AST_id
and RN0014.dspl = 0  and b.dspl = 0
and (RN0014.val + b.val) > 142499
and (RN0014.val + b.val) < 142501 !
select RN0014.AST_id, RN0014.AST_grp, RN0014.val,
b.AST_id, b.AST_grp, b.val, RN0014.val+b.val
from <rn0014  b, rn0014>
where RN0014.co_id = '507V' and RN0014.busi_yr = 1990
and b.co_id =RN0014.co_id and b.busi_yr = RN0014.busi_yr
and RN0014.AST_grp in
('410','420','430','490','511','520','530','610','620','630')
and b.AST_grp in
('410','420','430','490','511','520','530','610','620','630')
and RN0014.D_typ_id = '01' and   b.D_typ_id = '01'
and RN0014.val < 142499 and RN0014.AST_id <> b.AST_id
and RN0014.dspl = 0  and b.dspl = 0
and (RN0014.val + b.val) > 142499
and (RN0014.val + b.val) < 142501 !
commit   release!
file cleark !
file connect ( kern test !
create table tab1 (
cd_station fixed (3,0) key,
bdg_geg_station date range between '19900101' and '21001231',
srt_station char (1) ascii not null,
edg_geg_station date range between '19900101' and '21001231',
naam_station char (16) ascii not null,
nr_infopagina fixed (4) range between 1 and 9999,
kma_aftrek_grensp fixed (2) range between 0 and 20,
oms_zones char (19) ascii)!
insert tab1 values (2, '19910331', 'N', '19910818', 'abc', 567,4,'qw')!
create table tab2 (cd_station fixed (3) key range not in (1),
bdg_st_ap_afstand date key range between '19900101' and '21001231',
edg_st_ap_afstand date key range between '19900101' and '21001231',
nr_aa_p_1 fixed (3) not null,
kma_1e_a1 fixed(3) not null,
kma_2e_a1 fixed(3) not null,
nr_aa_p_2 fixed (3),
kma_1e_a2 fixed(3),
kma_2e_a2 fixed(3) )!
insert tab2 values (2, '19910522', '19910525', 3, 1, 2, null, null,null)!
create view cvi as select a.cd_station, a.bdg_geg_station,
a.edg_geg_station, b.nr_aa_p_1,
b.kma_1e_a1, b.kma_2e_a1, b.nr_aa_p_2, b.kma_1e_a2,
b.kma_2e_a2, b.bdg_st_ap_afstand, b.edg_st_ap_afstand
from tab1 a, tab2 b
where a.cd_station = b.cd_station and
a.srt_station = 'N' !
select cd_station, bdg_geg_station from cvi
order by nr_aa_p_1 !
select cd_station, nr_aa_p_1, bdg_geg_station from cvi
order by nr_aa_p_1 !
select a.cd_station, bdg_geg_station
from <tab1 a, tab2 b>
where a.cd_station = b.cd_station and
a.srt_station = 'N'
order by nr_aa_p_1 !
select b.cd_station, bdg_geg_station
from <tab2 b, tab1 a>
where a.cd_station = b.cd_station and
a.srt_station = 'N'
order by nr_aa_p_1 !
rollback release !
file cleark !
file connect ( kern test !
create table t ( a char (40),
                 b char (40))!
insert t set a = 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa', 
             b = 'bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb'!
insert t set a = 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa', 
             b = 'cccccccccccccccccccccccccccccccccccccccc'!
create table t1 ( a1 fixed(8), b1 char (40))!
insert t1 set a1 = 2, b1 = 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'!
insert t1 set a1 = 2, b1 = 'bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb'!
create view v1 as select * from t, t1
    where b = b1 !
select * from v1
  where b = 'bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb'
    and a = (select min (a) from v1 where b||b1 > 'b')!
fetch!
pars_then_ex !
data !
select * into :a, :b, :c, :d from v1
  where b = :b1
    and a = (select min (a) from v1 where b||b1 > :b2 and a <> :a)!
c 40 'bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb'
c 255 'b'
c 40 'z'

nodata !
pars_execute !
rollback release !
file cleark !
file connect ( kern test !
create table t1 ( 
 s1 char(40)ascii, s2 char(40)ascii,  s3 char(40)ascii,  s4 char(40)ascii,  
 s5 char(40)ascii, s6 char(40)ascii,  s7 char(40)ascii,  s8 char(40)ascii,  
 s9 char(40)ascii, s10 char(40)ascii,
s11 char(40)ascii, s12 char(40)ascii, s13 char(40)ascii, s14 char(40)ascii, 
s15 char(40)ascii, s16 char(40)ascii, s17 char(40)ascii, s18 char(40)ascii, 
s19 char(40)ascii, s20 char(40)ascii, 
s21 char(40)ascii, s22 char(40)ascii, s23 char(40)ascii, s24 char(40)ascii, 
s25 char(40)ascii, s26 char(40)ascii, s27 char(40)ascii, s28 char(40)ascii, 
s29 char(40)ascii, s30 char(40)ascii,
s31 char(40)ascii, s32 char(40)ascii, s33 char(40)ascii, s34 char(40)ascii, 
s35 char(40)ascii, s36 char(40)ascii, s37 char(40)ascii, s38 char(40)ascii, 
s39 char(40)ascii, s40 char(40)ascii,
s41 char(40)ascii, s42 char(40)ascii, s43 char(40)ascii, s44 char(40)ascii, 
s45 char(40)ascii, s46 char(40)ascii, s47 char(40)ascii, s48 char(40)ascii, 
s49 char(40)ascii, s50 char(40)ascii,
s51 char(40)ascii, s52 char(40)ascii, s53 char(40)ascii, s54 char(40)ascii, 
s55 char(40)ascii, s56 char(40)ascii, s57 char(40)ascii, s58 char(40)ascii, 
s59 char(40)ascii, s60 char(40)ascii,
s61 char(40)ascii, s62 char(40)ascii, s63 char(40)ascii, s64 char(40)ascii, 
s65 char(40)ascii, s66 char(40)ascii, s67 char(40)ascii, s68 char(40)ascii, 
s69 char(40)ascii, s70 char(40)ascii,
s71 char(40)ascii, s72 char(40)ascii, s73 char(40)ascii, s74 char(40)ascii, 
s75 char(40)ascii, s76 char(40)ascii, s77 char(40)ascii, s78 char(40)ascii, 
s79 char(40)ascii, s80 char(40)ascii,
s81 char(40)ascii, s82 char(40)ascii, s83 char(40)ascii, s84 char(40)ascii, 
s85 char(40)ascii, s86 char(40)ascii, s87 char(40)ascii, s88 char(40)ascii, 
s89 char(40)ascii, s90 char(40)ascii,
s91 char(40)ascii, s92 char(40)ascii, s93 char(40)ascii, s94 char(40)ascii, 
s95 char(40)ascii, s96 char(40)ascii, s97 char(40)ascii, s98 char(40)ascii, 
s99 char(40)ascii, s100 char(40)ascii)!
insert t1 set s1 = 'abcdef' !
select t1.*, r.* from t1, t1 r !
fetch !
rollback work release !
file cleark !
file connect ( kern test !
create table dbtypes(
dbtype        char(8) not null,
data_type     fixed (4) not null,
type_name     varchar(30) not null,
dbprecision   fixed (4),
dbextralength fixed (4),
dbscale       fixed (4),
dbradix       fixed (4)) !
insert into dbtypes values 
( 'CHAR',    1, 'CHAR',          NULL, 0, NULL, NULL) !
insert into dbtypes values 
( 'DATE',    9, 'DATE',            10, 2, NULL, NULL) !
insert into dbtypes values 
( 'FIXED',   3, 'DECIMAL',       NULL, 2,    0,   10)!
insert into dbtypes values 
( 'FLOAT',   6, 'FLOAT',           15, 0, NULL,   10)!
insert into dbtypes values 
( 'LONG',    -1, 'LONG VARCHAR', NULL, 0, NULL, NULL)!
insert into dbtypes values 
( 'TIME',    10, 'TIME',            8, 3, NULL, NULL)!
insert into dbtypes values 
( 'TIMESTMP',11, 'TIMESTAMP',      26, 6,    6, NULL)!
insert into dbtypes values 
( 'VARCHAR', 12, 'VARCHAR',      NULL, 0, NULL, NULL)!
create table columns ( owner         char(20),
                      tablename     char(20),
                      columnname    char(20),
                      len           fixed (6),
                      xxx           fixed (6),
                      mode          char (3),
                      datatype      char(8) not null,
                      ccomment      char (20))!         
insert into columns values
('SQLhugo', 'tab', 'col', 5, 3, 'OPT', 'CHAR', 'ohne worte') !
* das ueberfluessige (1=1) fuehrte in diesem 2/3-stufigen Befehl 
* abwechselnd zu 100 und kb stack type illegal, weil das Ende
* dieser Bedingung nicht erkannt, zwei Bedingungen zusammengefaszt
* wurden und dann hinten ein 'AND' uber war    !
select  null,
        col.owner, col.tablename,
        dbty.data_type, dbty.type_name,
        value (dbty.dbprecision, col.len),
        dbty.dbscale + col.xxx,
        dbty.dbradix,
        decode (col.mode, 'KEY', 0,
                          'OPT', 1,
                          'MAN', 0),
        col.ccomment

from    columns col,
        dbtypes dbty

where   (1=1)
and     col.OWNER like 'SQ%'
and     col.tablename like '%'
and     col.columnname like '%'
and     dbty.dbtype = col.datatype

order by col.owner, col.tablename !
fetch!
drop table dbtypes !
drop table columns !
commit release !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* C08.vdnts
file connect ( sut sut !
create user u1 password u1 dba !
commit release !
file connect ( u1 u1 !
CREATE TABLE t01 (s11 CHAR(5)  KEY)!
CREATE TABLE t02 (s21 FIXED(4) KEY, s22 CHAR(5))!
alter table  t02 FOREIGN KEY l01 (s22) REFERENCES  t01!
CREATE VIEW  v01 AS SELECT * FROM t02,t01 WHERE s22=s11!
CREATE INDEX "t02.s22"
	on t02( s22 ) !
INSERT t01 VALUES('00100')!
INSERT t02 VALUES(4711,'00100')!
COMMIT!
EXPLAIN SELECT * FROM v01 WHERE s11 = '00100'!
SELECT * FROM v01 WHERE s11 = '00100'!
fetch !
alter table t02 drop foreign key l01 !
COMMIT!
EXPLAIN SELECT * FROM v01 WHERE s11 = '00100'!
SELECT * FROM v01 WHERE s11 = '00100'!
fetch !
DROP INDEX "t02.s22" !
EXPLAIN SELECT * FROM v01 WHERE s11 = '00100'!
SELECT * FROM v01 WHERE s11 = '00100'!
fetch !
***** same test with multiple index
CREATE TABLE t03 (s11 CHAR(5)  KEY, s12 char(5) key)!
CREATE TABLE t04 (s21 FIXED(4) KEY, s22 CHAR(5), s23 char(5))!
alter table  t04 FOREIGN KEY l01 (s22, s23) REFERENCES  t03!
CREATE VIEW  v02 AS SELECT * FROM t04,t03 WHERE s22=s11 and s12=s23!
CREATE INDEX i1 on t04 (s22, s23) !
INSERT t03 VALUES('00100', 'aaaaa')!
INSERT t04 VALUES(4711,'00100', 'aaaaa')!
COMMIT!
EXPLAIN SELECT * FROM v02 WHERE s11 = '00100'!
SELECT * FROM v02 WHERE s11 = '00100'!
fetch !
alter table t04 drop foreign key l01 !
COMMIT!
EXPLAIN SELECT * FROM v02 WHERE s11 = '00100'!
SELECT * FROM v02 WHERE s11 = '00100'!
fetch !
DROP INDEX i1!
EXPLAIN SELECT * FROM v02 WHERE s11 = '00100'!
SELECT * FROM v02 WHERE s11 = '00100'!
fetch !
COMMIT RELEASE!
file connect ( sut sut !
drop user u1 !
commit release !

*****************************************************************************
* C106575.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
CREATE TABLE t1 (
s11 CHAR(5) KEY, s12 CHAR(9) KEY, s13 CHAR(4) KEY,
s14 CHAR(4), s15 CHAR(3))!
CREATE TABLE t2 (
s21 CHAR(5) KEY, s22 CHAR(9) KEY, s23 CHAR(4) KEY,
s24 CHAR(4), s25 CHAR(3))!
COMMIT!
INSERT t1 SET
s11 = 'AA1', s12 = 'AA2', s13 = 'AA3', s14 = 'AA4', s15 = 'AA5'!
INSERT t1 SET
s11 = 'AA1', s12 = 'AB2', s13 = 'AB3', s14 = 'AB4', s15 = 'AB5'!
INSERT t2 SET
s21 = 'AA1', s22 = 'AA2', s23 = 'AA3', s24 = 'AA4', s25 = 'AA5'!
INSERT t2 SET
s21 = 'AA1', s22 = 'AB2', s23 = 'AB3', s24 = 'AB4', s25 = 'AB5'!
COMMIT!
CREATE INDEX "t1.s12" on t1( s12 ) !
CREATE INDEX "t2.s22" on t2( s22 ) !
COMMIT!
EXPLAIN SELECT * FROM t1, t2
WHERE s11 BETWEEN 'AA1' AND 'AA2'
AND   s14 = 'AB4'
AND   SUBSTR(s15,1,2) = 'AB'
AND   s21 = s11
AND   s13 = 'AB3'
AND   s23 = s13
AND   s12 = 'AB2'
AND   s22 = s12!
*
SELECT * FROM t1, t2
WHERE s11 BETWEEN 'AA1' AND 'AA2'
AND   s14 = 'AB4'
AND   SUBSTR(s15,1,2) = 'AB'
AND   s21 = s11
AND   s13 = 'AB3'
AND   s23 = s13
AND   s12 = 'AB2'
AND   s22 = s12!
*
FETCH !
*
EXPLAIN SELECT * FROM t1, t2
WHERE s11 BETWEEN 'AA1' AND 'AA2'
AND   s21 = s11
AND   s13 = 'AB3'
AND   s23 = s13
AND   s12 = 'AB2'
AND   s22 = s12
AND   s14 = 'AB4'
AND   SUBSTR(s15,1,2) = 'AB'!
*
SELECT * FROM t1, t2
WHERE s11 BETWEEN 'AA1' AND 'AA2'
AND   s21 = s11
AND   s13 = 'AB3'
AND   s23 = s13
AND   s12 = 'AB2'
AND   s22 = s12
AND   s14 = 'AB4'
AND   SUBSTR(s15,1,2) = 'AB'!
*
FETCH !
COMMIT RELEASE!
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* CHARNULL.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table t ( k1 char (5) ascii key,
                 k2 char (3) ascii key,
                 a char (5)  ascii,
                 i1 char (5) ascii,
                 i2 char (3) ascii,
                 n  char (3) ascii)!
create index "t.i1" on t( i1 ) !
create index "t.i2" on t( i2 ) !
create index i on t (i1, i2) !
insert t values ('aaaa', '11', 'aaa', 'aaaa', '11', '  1')!
insert t values ('aaab', '12', 'aaa', 'aaab', '12', '  2')!
insert t values ('aaac', '13', 'aaa', 'aaac', '13', '  3')!
commit release!
file connect ( kern test sqlmode oracle !
parsing!

insert into t values (:k1, :k2, :a, :i1, :i2, :n)!
data!
executing!
insert!
1
b 10 61616161
b  6 313100
b 10 616161
b 10 61616161
b  6 313100
b  6 202034

executing!
insert!
1
b 10 6161616100
b  6 313100
b 10 616161
b 10 6161616100
b  6 313100
b  6 202035

executing!
insert!
1
b 10 6161616100
b  6 3131
b 10 616161
b 10 6161616100
b  6 3131
b  6 202036

executing!
insert!
1
b 10 616161611F
b  6 313100
b 10 616161
b 10 616161611F
b  6 313100
b  6 202037

executing!
insert!
1
b 10 616161611F
b  6 3131
b 10 616161
b 10 616161611F
b  6 3131
b  6 202038

executing!
insert!
1
b 10 61616161
b  6 313200
b 10 616161
b 10 61616161
b  6 313200
b  6 202039

executing!
insert!
1
b 10 6161616100
b  6 313200
b 10 616161
b 10 6161616100
b  6 313200
b  6 203130

executing!
insert!
1
b 10 6161616100
b  6 3132
b 10 616161
b 10 6161616100
b  6 3132
b  6 203131

executing!
insert!
1
b 10 616161611F
b  6 313200
b 10 616161
b 10 616161611F
b  6 313200
b  6 203132

executing!
insert!
1
b 10 616161611F
b  6 3132
b 10 616161
b 10 616161611F
b  6 3132
b  6 203133

executing!
insert!
1
b 10 61616162
b  6 313200
b 10 616161
b 10 61616162
b  6 313200
b  6 203134

executing!
insert!
1
b 10 6161616200
b  6 313200
b 10 616161
b 10 6161616200
b  6 313200
b  6 203135

executing!
insert!
1
b 10 6161616200
b  6 3132
b 10 616161
b 10 6161616200
b  6 3132
b  6 203136

executing!
insert!
1
b 10 616161621F
b  6 313200
b 10 616161
b 10 616161621F
b  6 313200
b  6 203137

executing!
insert!
1
b 10 616161621F
b  6 3132
b 10 616161
b 10 616161621F
b  6 3132
b  6 203138

executing!
insert!
1
b 10 61616163
b  6 313300
b 10 616161
b 10 61616163
b  6 313300
b  6 203139

executing!
insert!
1
b 10 6161616300
b  6 313300
b 10 616161
b 10 6161616300
b  6 313300
b  6 203230

executing!
insert!
1
b 10 6161616300
b  6 3133
b 10 616161
b 10 6161616300
b  6 3133
b  6 203231

executing!
insert!
1
b 10 616161631F
b  6 313300
b 10 616161
b 10 616161631F
b  6 313300
b  6 203232

executing!
insert!
1
b 10 616161631F
b  6 3133
b 10 616161
b 10 616161631F
b  6 3133
b  6 203233

executing!
insert!
1
b 10 61616163
b  6 313200
b 10 616161
b 10 61616163
b  6 313200
b  6 203234

executing!
insert!
1
b 10 6161616300
b  6 313200
b 10 616161
b 10 6161616300
b  6 313200
b  6 203235

executing!
insert!
1
b 10 6161616300
b  6 3132
b 10 616161
b 10 6161616300
b  6 3132
b  6 203236

executing!
insert!
1
b 10 616161631F
b  6 313200
b 10 616161
b 10 616161631F
b  6 313200
b  6 203237

executing!
insert!
1
b 10 616161631F
b  6 3132
b 10 616161
b 10 616161631F
b  6 3132
b  6 203238


nodata!
pars_execute!
commit release!
file connect ( kern test !
declare erg cursor for 
select hex(t1.k1), hex(t1.k2), hex(t2.k1) t21, hex(t2.k2) t22,
    'x', hex(t1.n), hex(t2.n) n2
from <t t1, t t2>  where t1.k2 > t2.k2 and t2.k1 like 'aaab%' 
order by t1.n, n2 !
fetch erg!

declare erg cursor for 
select hex(t1.k1), hex(t1.k2), hex(t2.k1) t21, hex(t2.k2) t22, 
    'x', hex(t1.n), hex(t2.n) n2
from <t t2, t t1>  where t1.k2 > t2.k2 and t2.k1 like 'aaab%' 
order by t1.n, n2 !
fetch erg!

declare erg cursor for 
select t1.k1, t1.k2, t2.k1 t21, t2.k2 t22, 'x', t1.n, t2.n n2
from t t1, t t2  !
select hex(k1), hex(k2), hex(t21), hex(t22), 
    'x', hex(n), hex(n2)
from erg
where k2 > t22 and t21 like 'aaab%' 
order by n, n2 !
fetch !

declare erg cursor for 
select hex(t1.k1), hex(t1.k2), hex(t2.k1) t21, hex(t2.k2) t22, 
    'x', hex(t1.n), hex(t2.n) n2
from <t t1, t t2>  where t1.k2 >= t2.k2 and t2.k1 like 'aaab%' 
order by t1.n, n2 !
fetch erg!

declare erg cursor for 
select hex(t1.k1), hex(t1.k2), hex(t2.k1) t21, hex(t2.k2) t22, 
    'x', hex(t1.n), hex(t2.n) n2
from <t t2, t t1>  where t1.k2 >= t2.k2 and t2.k1 like 'aaab%' 
order by t1.n, n2 !
fetch erg!

declare erg cursor for 
select t1.k1, t1.k2, t2.k1 t21, t2.k2 t22, 
    'x', t1.n, t2.n n2
from t t1, t t2 !
select hex(k1), hex(k2), hex(t21), hex(t22), 
    'x', hex(n), hex(n2)
from erg
where k2 >= t22 and t21 like 'aaab%' 
order by n, n2 !
fetch!

pars_execute!
commit!
declare erg cursor for 
select hex(t1.k1), hex(t1.k2), hex(t2.i1), hex(t2.i2) 
from t t1, t t2 
where t1.k1 >= t2.k1 and t1.k1 > 'aaab' and t2.k2 = '11' for reuse !
fetch erg!

declare erg cursor for 
select hex(t1.k1), hex(t1.k2), hex(t2.i1), hex(t2.i2) 
from t t1, t t2 
where t1.k1 = t2.k1 and t2.k2 = '11' for reuse !
fetch erg!

pars_then_ex !
nodata!
fetch into :a, :b, :c, :d!
data !
declare erg cursor for 
select hex(t1.k1), hex(t1.k2), hex(t2.i1), hex(t2.i2)
from t t1, t t2 
where t1.k1 >= t2.k1 and t1.k1 > :a and t2.k2 = :b for reuse !
c 5 'aaab' c 3 '11'

nodata!
fetch erg into :a, :b, :c, :d!
data !
declare erg cursor for 
select hex(t1.k1), hex(t1.k2), hex(t2.i1), hex(t2.i2) 
from t t1, t t2 
where t1.k1 = t2.k1 and t2.k2 = :a for reuse !
c 3 '11'

nodata!
pars_execute!
declare erg cursor for 
select hex(t1.i1), hex(t1.i2), hex(t2.i1), hex(t2.i2) 
from t t1, t t2 
where t1.i1 >= t2.i1 and t1.i1 > 'aaab' and t2.i2 = '11' for reuse !
fetch erg!

declare erg cursor for 
select hex(t1.i1), hex(t1.i2), hex(t2.i1), hex(t2.i2)
from t t1, t t2 
where t1.i1 = t2.i1 and t2.i2 = '11' for reuse !
fetch erg!

pars_then_ex !
nodata!
fetch into :a, :b, :c, :d!
data !
declare erg cursor for 
select hex(t1.i1), hex(t1.i2), hex(t2.i1), hex(t2.i2) 
from t t1, t t2 
where t1.i1 >= t2.i1 and t1.i1 > :a and t2.i2 = :b for reuse !
c 5 'aaab' c 3 '11'

nodata!
fetch erg into :a, :b, :c, :d!
data !
declare erg cursor for 
select hex(t1.i1), hex(t1.i2), hex(t2.i1), hex(t2.i2)
from t t1, t t2 
where t1.i1 = t2.i1 and t2.i2 = :a for reuse !
c 3 '11'

nodata!
pars_execute!
commit release!
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* CHECKIND.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !

FILE CONNECT ( KERN TEST !

CREATE TABLE TVEU12 (
K1N5 CHAR      (5) ASCII    ,
F1I4 FIXED     (10,0) NOT NULL,
F2F8 FLOAT     (18) NOT NULL,
F3CH CHAR      (30) ASCII    NOT NULL,
F4TI CHAR      (6) ASCII    NOT NULL )!


insert kern.tveu12 values ('00001',     1, 1.00000000000000010E-01, 
    'SATZNUMMER : 00001............', '093839') !
insert kern.tveu12 values ('00002',     9, 9.00000000000000020E-01, 
    'SATZNUMMER : 00002............', '093838') !
insert kern.tveu12 values ('00011',    40, 4.00000000000000000E+00, 
    'SATZNUMMER : 00011............', '093829') !
insert kern.tveu12 values ('00051',   100, 1.00000000000000000E+01, 
    'SATZNUMMER : 00051............', '093749') !
insert kern.tveu12 values ('00151',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00151............', '093609') !
insert kern.tveu12 values ('00153',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00153............', '093607') !
insert kern.tveu12 values ('00155',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00155............', '093605') !
insert kern.tveu12 values ('00157',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00157............', '093603') !
insert kern.tveu12 values ('00159',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00159............', '093601') !
insert kern.tveu12 values ('00161',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00161............', '093559') !
insert kern.tveu12 values ('00163',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00163............', '093557') !
insert kern.tveu12 values ('00165',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00165............', '093555') !
insert kern.tveu12 values ('00167',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00167............', '093553') !
insert kern.tveu12 values ('00169',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00169............', '093551') !
insert kern.tveu12 values ('00171',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00171............', '093549') !
insert kern.tveu12 values ('00173',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00173............', '093547') !
insert kern.tveu12 values ('00175',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00175............', '093545') !
insert kern.tveu12 values ('00177',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00177............', '093543') !
insert kern.tveu12 values ('00179',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00179............', '093541') !
insert kern.tveu12 values ('00181',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00181............', '093539') !
insert kern.tveu12 values ('00183',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00183............', '093537') !
insert kern.tveu12 values ('00185',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00185............', '093535') !
insert kern.tveu12 values ('00187',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00187............', '093533') !
insert kern.tveu12 values ('00189',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00189............', '093531') !
insert kern.tveu12 values ('00191',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00191............', '093529') !
insert kern.tveu12 values ('00193',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00193............', '093527') !
insert kern.tveu12 values ('00195',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00195............', '093525') !
insert kern.tveu12 values ('00197',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00197............', '093523') !
insert kern.tveu12 values ('00199',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00199............', '093521') !

CREATE TABLE TVEU13 (
K1N5 CHAR      (5) ASCII    ,
K2N5 CHAR      (5) ASCII    ,
F1I4 FIXED     (10,0) NOT NULL,
F2TX CHAR      (30) ASCII    NOT NULL,
F3TI CHAR      (6) ASCII    NOT NULL)!

insert KERN.TVEU13 values ('00001', '00001',   1, 
     '%Die Mitternacht  ............', '093839') !
insert KERN.TVEU13 values ('00002', '00002',   1, 
     '_zog naeher schon,............', '093838') !
insert KERN.TVEU13 values ('00002', '00003',   2, 
     '%in stiller Ruh   ............', '093837') !
insert KERN.TVEU13 values ('00002', '00004',   3, 
     '_lag Babylon.     ............', '093836') !
insert KERN.TVEU13 values ('00002', '00005',   4, 
     '%Die Mitternacht  ............', '093835') !
insert KERN.TVEU13 values ('00002', '00006',   5, 
     '_zog naeher schon,............', '093834') !
insert KERN.TVEU13 values ('00002', '00007',   6, 
     '%in stiller Ruh   ............', '093833') !
insert KERN.TVEU13 values ('00002', '00008',   7, 
     '_lag Babylon.     ............', '093832') !
insert KERN.TVEU13 values ('00002', '00009',   8, 
     '%Die Mitternacht  ............', '093831') !
insert KERN.TVEU13 values ('00002', '00010',   9, 
     '_zog naeher schon,............', '093830') !
insert KERN.TVEU13 values ('00011', '00011',   1, 
     '%in stiller Ruh   ............', '093829') !
insert KERN.TVEU13 values ('00011', '00012',   2, 
     '_lag Babylon.     ............', '093828') !
insert KERN.TVEU13 values ('00011', '00013',   3, 
     '%Die Mitternacht  ............', '093827') !
insert KERN.TVEU13 values ('00011', '00014',   4, 
     '_zog naeher schon,............', '093826') !
insert KERN.TVEU13 values ('00011', '00015',   5, 
     '%in stiller Ruh   ............', '093825') !
insert KERN.TVEU13 values ('00011', '00016',   6, 
     '_lag Babylon.     ............', '093824') !
insert KERN.TVEU13 values ('00011', '00017',   7, 
     '%Die Mitternacht  ............', '093823') !
insert KERN.TVEU13 values ('00011', '00018',   8, 
     '_zog naeher schon,............', '093822') !
insert KERN.TVEU13 values ('00011', '00019',   9, 
     '%in stiller Ruh   ............', '093821') !
insert KERN.TVEU13 values ('00011', '00020',  10, 
     '_lag Babylon.     ............', '093820') !
insert KERN.TVEU13 values ('00011', '00021',  11, 
     '%Die Mitternacht  ............', '093819') !
insert KERN.TVEU13 values ('00011', '00022',  12, 
     '_zog naeher schon,............', '093818') !
insert KERN.TVEU13 values ('00011', '00023',  13, 
     '%in stiller Ruh   ............', '093817') !
insert KERN.TVEU13 values ('00011', '00024',  14, 
     '_lag Babylon.     ............', '093816') !
insert KERN.TVEU13 values ('00011', '00025',  15, 
     '%Die Mitternacht  ............', '093815') !
insert KERN.TVEU13 values ('00011', '00026',  16, 
     '_zog naeher schon,............', '093814') !
insert KERN.TVEU13 values ('00011', '00027',  17, 
     '%in stiller Ruh   ............', '093813') !
insert KERN.TVEU13 values ('00011', '00028',  18, 
     '_lag Babylon.     ............', '093812') !
insert KERN.TVEU13 values ('00011', '00029',  19, 
     '%Die Mitternacht  ............', '093811') !
insert KERN.TVEU13 values ('00011', '00030',  20, 
     '_zog naeher schon,............', '093810') !
insert KERN.TVEU13 values ('00011', '00031',  21, 
     '%in stiller Ruh   ............', '093809') !
insert KERN.TVEU13 values ('00011', '00032',  22, 
     '_lag Babylon.     ............', '093808') !
insert KERN.TVEU13 values ('00011', '00033',  23, 
     '%Die Mitternacht  ............', '093807') !
insert KERN.TVEU13 values ('00011', '00034',  24, 
     '_zog naeher schon,............', '093806') !
insert KERN.TVEU13 values ('00011', '00035',  25, 
     '%in stiller Ruh   ............', '093805') !
insert KERN.TVEU13 values ('00011', '00036',  26, 
     '_lag Babylon.     ............', '093804') !
insert KERN.TVEU13 values ('00011', '00037',  27, 
     '%Die Mitternacht  ............', '093803') !
insert KERN.TVEU13 values ('00011', '00038',  28, 
     '_zog naeher schon,............', '093802') !
insert KERN.TVEU13 values ('00011', '00039',  29, 
     '%in stiller Ruh   ............', '093801') !
insert KERN.TVEU13 values ('00011', '00040',  30, 
     '_lag Babylon.     ............', '093800') !
insert KERN.TVEU13 values ('00011', '00041',  31, 
     '%Die Mitternacht  ............', '093759') !
insert KERN.TVEU13 values ('00011', '00042',  32, 
     '_zog naeher schon,............', '093758') !
insert KERN.TVEU13 values ('00011', '00043',  33, 
     '%in stiller Ruh   ............', '093757') !
insert KERN.TVEU13 values ('00011', '00044',  34, 
     '_lag Babylon.     ............', '093756') !
insert KERN.TVEU13 values ('00011', '00045',  35, 
     '%Die Mitternacht  ............', '093755') !
insert KERN.TVEU13 values ('00011', '00046',  36, 
     '_zog naeher schon,............', '093754') !
insert KERN.TVEU13 values ('00011', '00047',  37, 
     '%in stiller Ruh   ............', '093753') !
insert KERN.TVEU13 values ('00011', '00048',  38, 
     '_lag Babylon.     ............', '093752') !
insert KERN.TVEU13 values ('00011', '00049',  39, 
     '%Die Mitternacht  ............', '093751') !
insert KERN.TVEU13 values ('00011', '00050',  40, 
     '_zog naeher schon,............', '093750') !
insert KERN.TVEU13 values ('00051', '00051',   1, 
     '%in stiller Ruh   ............', '093749') !
insert KERN.TVEU13 values ('00051', '00052',   2, 
     '_lag Babylon.     ............', '093748') !
insert KERN.TVEU13 values ('00051', '00053',   3, 
     '%Die Mitternacht  ............', '093747') !
insert KERN.TVEU13 values ('00051', '00054',   4, 
     '_zog naeher schon,............', '093746') !
insert KERN.TVEU13 values ('00051', '00055',   5, 
     '%in stiller Ruh   ............', '093745') !
insert KERN.TVEU13 values ('00051', '00056',   6, 
     '_lag Babylon.     ............', '093744') !
insert KERN.TVEU13 values ('00051', '00057',   7, 
     '%Die Mitternacht  ............', '093743') !
insert KERN.TVEU13 values ('00051', '00058',   8, 
     '_zog naeher schon,............', '093742') !
insert KERN.TVEU13 values ('00051', '00059',   9, 
     '%in stiller Ruh   ............', '093741') !
insert KERN.TVEU13 values ('00051', '00060',  10, 
     '_lag Babylon.     ............', '093740') !
insert KERN.TVEU13 values ('00051', '00061',  11, 
     '%Die Mitternacht  ............', '093739') !
insert KERN.TVEU13 values ('00051', '00062',  12, 
     '_zog naeher schon,............', '093738') !
insert KERN.TVEU13 values ('00051', '00063',  13, 
     '%in stiller Ruh   ............', '093737') !
insert KERN.TVEU13 values ('00051', '00064',  14, 
     '_lag Babylon.     ............', '093736') !
insert KERN.TVEU13 values ('00051', '00065',  15, 
     '%Die Mitternacht  ............', '093735') !
insert KERN.TVEU13 values ('00051', '00066',  16, 
     '_zog naeher schon,............', '093734') !
insert KERN.TVEU13 values ('00051', '00067',  17, 
     '%in stiller Ruh   ............', '093733') !
insert KERN.TVEU13 values ('00051', '00068',  18, 
     '_lag Babylon.     ............', '093732') !
insert KERN.TVEU13 values ('00051', '00069',  19, 
     '%Die Mitternacht  ............', '093731') !
insert KERN.TVEU13 values ('00051', '00070',  20, 
     '_zog naeher schon,............', '093730') !
insert KERN.TVEU13 values ('00051', '00071',  21, 
     '%in stiller Ruh   ............', '093729') !
insert KERN.TVEU13 values ('00051', '00072',  22, 
     '_lag Babylon.     ............', '093728') !
insert KERN.TVEU13 values ('00051', '00073',  23, 
     '%Die Mitternacht  ............', '093727') !
insert KERN.TVEU13 values ('00051', '00074',  24, 
     '_zog naeher schon,............', '093726') !
insert KERN.TVEU13 values ('00051', '00075',  25, 
     '%in stiller Ruh   ............', '093725') !
insert KERN.TVEU13 values ('00051', '00076',  26, 
     '_lag Babylon.     ............', '093724') !
insert KERN.TVEU13 values ('00051', '00077',  27, 
     '%Die Mitternacht  ............', '093723') !
insert KERN.TVEU13 values ('00051', '00078',  28, 
     '_zog naeher schon,............', '093722') !
insert KERN.TVEU13 values ('00051', '00079',  29, 
     '%in stiller Ruh   ............', '093721') !
insert KERN.TVEU13 values ('00051', '00080',  30, 
     '_lag Babylon.     ............', '093720') !
insert KERN.TVEU13 values ('00051', '00081',  31, 
     '%Die Mitternacht  ............', '093719') !
insert KERN.TVEU13 values ('00051', '00082',  32, 
     '_zog naeher schon,............', '093718') !
insert KERN.TVEU13 values ('00051', '00083',  33, 
     '%in stiller Ruh   ............', '093717') !
insert KERN.TVEU13 values ('00051', '00084',  34, 
     '_lag Babylon.     ............', '093716') !
insert KERN.TVEU13 values ('00051', '00085',  35, 
     '%Die Mitternacht  ............', '093715') !
insert KERN.TVEU13 values ('00051', '00086',  36, 
     '_zog naeher schon,............', '093714') !
insert KERN.TVEU13 values ('00051', '00087',  37, 
     '%in stiller Ruh   ............', '093713') !
insert KERN.TVEU13 values ('00051', '00088',  38, 
     '_lag Babylon.     ............', '093712') !
insert KERN.TVEU13 values ('00051', '00089',  39, 
     '%Die Mitternacht  ............', '093711') !
insert KERN.TVEU13 values ('00051', '00090',  40, 
     '_zog naeher schon,............', '093710') !
insert KERN.TVEU13 values ('00051', '00091',  41, 
     '%in stiller Ruh   ............', '093709') !
insert KERN.TVEU13 values ('00051', '00092',  42, 
     '_lag Babylon.     ............', '093708') !
insert KERN.TVEU13 values ('00051', '00093',  43, 
     '%Die Mitternacht  ............', '093707') !
insert KERN.TVEU13 values ('00051', '00094',  44, 
     '_zog naeher schon,............', '093706') !
insert KERN.TVEU13 values ('00051', '00095',  45, 
     '%in stiller Ruh   ............', '093705') !
insert KERN.TVEU13 values ('00051', '00096',  46, 
     '_lag Babylon.     ............', '093704') !
insert KERN.TVEU13 values ('00051', '00097',  47, 
     '%Die Mitternacht  ............', '093703') !
insert KERN.TVEU13 values ('00051', '00098',  48, 
     '_zog naeher schon,............', '093702') !
insert KERN.TVEU13 values ('00051', '00099',  49, 
     '%in stiller Ruh   ............', '093701') !
insert KERN.TVEU13 values ('00051', '00100',  50, 
     '_lag Babylon.     ............', '093700') !
insert KERN.TVEU13 values ('00051', '00101',  51, 
     '%Die Mitternacht  ............', '093659') !
insert KERN.TVEU13 values ('00051', '00102',  52, 
     '_zog naeher schon,............', '093658') !
insert KERN.TVEU13 values ('00051', '00103',  53, 
     '%in stiller Ruh   ............', '093657') !
insert KERN.TVEU13 values ('00051', '00104',  54, 
     '_lag Babylon.     ............', '093656') !
insert KERN.TVEU13 values ('00051', '00105',  55, 
     '%Die Mitternacht  ............', '093655') !
insert KERN.TVEU13 values ('00051', '00106',  56, 
     '_zog naeher schon,............', '093654') !
insert KERN.TVEU13 values ('00051', '00107',  57, 
     '%in stiller Ruh   ............', '093653') !
insert KERN.TVEU13 values ('00051', '00108',  58, 
     '_lag Babylon.     ............', '093652') !
insert KERN.TVEU13 values ('00051', '00109',  59, 
     '%Die Mitternacht  ............', '093651') !
insert KERN.TVEU13 values ('00051', '00110',  60, 
     '_zog naeher schon,............', '093650') !
insert KERN.TVEU13 values ('00051', '00111',  61, 
     '%in stiller Ruh   ............', '093649') !
insert KERN.TVEU13 values ('00051', '00112',  62, 
     '_lag Babylon.     ............', '093648') !
insert KERN.TVEU13 values ('00051', '00113',  63, 
     '%Die Mitternacht  ............', '093647') !
insert KERN.TVEU13 values ('00051', '00114',  64, 
     '_zog naeher schon,............', '093646') !
insert KERN.TVEU13 values ('00051', '00115',  65, 
     '%in stiller Ruh   ............', '093645') !
insert KERN.TVEU13 values ('00051', '00116',  66, 
     '_lag Babylon.     ............', '093644') !
insert KERN.TVEU13 values ('00051', '00117',  67, 
     '%Die Mitternacht  ............', '093643') !
insert KERN.TVEU13 values ('00051', '00118',  68, 
     '_zog naeher schon,............', '093642') !
insert KERN.TVEU13 values ('00051', '00119',  69, 
     '%in stiller Ruh   ............', '093641') !
insert KERN.TVEU13 values ('00051', '00120',  70, 
     '_lag Babylon.     ............', '093640') !
insert KERN.TVEU13 values ('00051', '00121',  71, 
     '%Die Mitternacht  ............', '093639') !
insert KERN.TVEU13 values ('00051', '00122',  72, 
     '_zog naeher schon,............', '093638') !
insert KERN.TVEU13 values ('00051', '00123',  73, 
     '%in stiller Ruh   ............', '093637') !
insert KERN.TVEU13 values ('00051', '00124',  74, 
     '_lag Babylon.     ............', '093636') !
insert KERN.TVEU13 values ('00051', '00125',  75, 
     '%Die Mitternacht  ............', '093635') !
insert KERN.TVEU13 values ('00051', '00126',  76, 
     '_zog naeher schon,............', '093634') !
insert KERN.TVEU13 values ('00051', '00127',  77, 
     '%in stiller Ruh   ............', '093633') !
insert KERN.TVEU13 values ('00051', '00128',  78, 
     '_lag Babylon.     ............', '093632') !
insert KERN.TVEU13 values ('00051', '00129',  79, 
     '%Die Mitternacht  ............', '093631') !
insert KERN.TVEU13 values ('00051', '00130',  80, 
     '_zog naeher schon,............', '093630') !
insert KERN.TVEU13 values ('00051', '00131',  81, 
     '%in stiller Ruh   ............', '093629') !
insert KERN.TVEU13 values ('00051', '00132',  82, 
     '_lag Babylon.     ............', '093628') !
insert KERN.TVEU13 values ('00051', '00133',  83, 
     '%Die Mitternacht  ............', '093627') !
insert KERN.TVEU13 values ('00051', '00134',  84, 
     '_zog naeher schon,............', '093626') !
insert KERN.TVEU13 values ('00051', '00135',  85, 
     '%in stiller Ruh   ............', '093625') !
insert KERN.TVEU13 values ('00051', '00136',  86, 
     '_lag Babylon.     ............', '093624') !
insert KERN.TVEU13 values ('00051', '00137',  87, 
     '%Die Mitternacht  ............', '093623') !
insert KERN.TVEU13 values ('00051', '00138',  88, 
     '_zog naeher schon,............', '093622') !
insert KERN.TVEU13 values ('00051', '00139',  89, 
     '%in stiller Ruh   ............', '093621') !
insert KERN.TVEU13 values ('00051', '00140',  90, 
     '_lag Babylon.     ............', '093620') !
insert KERN.TVEU13 values ('00051', '00141',  91, 
     '%Die Mitternacht  ............', '093619') !
insert KERN.TVEU13 values ('00051', '00142',  92, 
     '_zog naeher schon,............', '093618') !
insert KERN.TVEU13 values ('00051', '00143',  93, 
     '%in stiller Ruh   ............', '093617') !
insert KERN.TVEU13 values ('00051', '00144',  94, 
     '_lag Babylon.     ............', '093616') !
insert KERN.TVEU13 values ('00051', '00145',  95, 
     '%Die Mitternacht  ............', '093615') !
insert KERN.TVEU13 values ('00051', '00146',  96, 
     '_zog naeher schon,............', '093614') !
insert KERN.TVEU13 values ('00051', '00147',  97, 
     '%in stiller Ruh   ............', '093613') !
insert KERN.TVEU13 values ('00051', '00148',  98, 
     '_lag Babylon.     ............', '093612') !
insert KERN.TVEU13 values ('00051', '00149',  99, 
     '%Die Mitternacht  ............', '093611') !
insert KERN.TVEU13 values ('00051', '00150', 100, 
     '_zog naeher schon,............', '093610') !
insert KERN.TVEU13 values ('00052', '00051',   1, 
     '%in stiller Ruh   ............', '093749') !
insert KERN.TVEU13 values ('00052', '00052',   2, 
     '_lag Babylon.     ............', '093748') !
insert KERN.TVEU13 values ('00052', '00053',   3, 
     '%Die Mitternacht  ............', '093747') !
insert KERN.TVEU13 values ('00052', '00054',   4, 
     '_zog naeher schon,............', '093746') !
insert KERN.TVEU13 values ('00052', '00055',   5, 
     '%in stiller Ruh   ............', '093745') !
insert KERN.TVEU13 values ('00052', '00056',   6, 
     '_lag Babylon.     ............', '093744') !
insert KERN.TVEU13 values ('00052', '00057',   7, 
     '%Die Mitternacht  ............', '093743') !
insert KERN.TVEU13 values ('00052', '00058',   8, 
     '_zog naeher schon,............', '093742') !
insert KERN.TVEU13 values ('00052', '00059',   9, 
     '%in stiller Ruh   ............', '093741') !
insert KERN.TVEU13 values ('00052', '00060',  10, 
     '_lag Babylon.     ............', '093740') !
insert KERN.TVEU13 values ('00052', '00061',  11, 
     '%Die Mitternacht  ............', '093739') !
insert KERN.TVEU13 values ('00052', '00062',  12, 
     '_zog naeher schon,............', '093738') !
insert KERN.TVEU13 values ('00052', '00063',  13, 
     '%in stiller Ruh   ............', '093737') !
insert KERN.TVEU13 values ('00052', '00064',  14, 
     '_lag Babylon.     ............', '093736') !
insert KERN.TVEU13 values ('00052', '00065',  15, 
     '%Die Mitternacht  ............', '093735') !
insert KERN.TVEU13 values ('00052', '00066',  16, 
     '_zog naeher schon,............', '093734') !
insert KERN.TVEU13 values ('00052', '00067',  17, 
     '%in stiller Ruh   ............', '093733') !
insert KERN.TVEU13 values ('00052', '00068',  18, 
     '_lag Babylon.     ............', '093732') !
insert KERN.TVEU13 values ('00052', '00069',  19, 
     '%Die Mitternacht  ............', '093731') !
insert KERN.TVEU13 values ('00052', '00070',  20, 
     '_zog naeher schon,............', '093730') !
insert KERN.TVEU13 values ('00052', '00071',  21, 
     '%in stiller Ruh   ............', '093729') !
insert KERN.TVEU13 values ('00052', '00072',  22, 
     '_lag Babylon.     ............', '093728') !
insert KERN.TVEU13 values ('00052', '00073',  23, 
     '%Die Mitternacht  ............', '093727') !
insert KERN.TVEU13 values ('00052', '00074',  24, 
     '_zog naeher schon,............', '093726') !
insert KERN.TVEU13 values ('00052', '00075',  25, 
     '%in stiller Ruh   ............', '093725') !
insert KERN.TVEU13 values ('00052', '00076',  26, 
     '_lag Babylon.     ............', '093724') !
insert KERN.TVEU13 values ('00052', '00077',  27, 
     '%Die Mitternacht  ............', '093723') !
insert KERN.TVEU13 values ('00052', '00078',  28, 
     '_zog naeher schon,............', '093722') !
insert KERN.TVEU13 values ('00052', '00079',  29, 
     '%in stiller Ruh   ............', '093721') !
insert KERN.TVEU13 values ('00052', '00080',  30, 
     '_lag Babylon.     ............', '093720') !
insert KERN.TVEU13 values ('00052', '00081',  31, 
     '%Die Mitternacht  ............', '093719') !
insert KERN.TVEU13 values ('00052', '00082',  32, 
     '_zog naeher schon,............', '093718') !
insert KERN.TVEU13 values ('00052', '00083',  33, 
     '%in stiller Ruh   ............', '093717') !
insert KERN.TVEU13 values ('00052', '00084',  34, 
     '_lag Babylon.     ............', '093716') !
insert KERN.TVEU13 values ('00052', '00085',  35, 
     '%Die Mitternacht  ............', '093715') !
insert KERN.TVEU13 values ('00052', '00086',  36, 
     '_zog naeher schon,............', '093714') !
insert KERN.TVEU13 values ('00052', '00087',  37, 
     '%in stiller Ruh   ............', '093713') !
insert KERN.TVEU13 values ('00052', '00088',  38, 
     '_lag Babylon.     ............', '093712') !
insert KERN.TVEU13 values ('00052', '00089',  39, 
     '%Die Mitternacht  ............', '093711') !
insert KERN.TVEU13 values ('00052', '00090',  40, 
     '_zog naeher schon,............', '093710') !
insert KERN.TVEU13 values ('00052', '00091',  41, 
     '%in stiller Ruh   ............', '093709') !
insert KERN.TVEU13 values ('00052', '00092',  42, 
     '_lag Babylon.     ............', '093708') !
insert KERN.TVEU13 values ('00052', '00093',  43, 
     '%Die Mitternacht  ............', '093707') !
insert KERN.TVEU13 values ('00052', '00094',  44, 
     '_zog naeher schon,............', '093706') !
insert KERN.TVEU13 values ('00052', '00095',  45, 
     '%in stiller Ruh   ............', '093705') !
insert KERN.TVEU13 values ('00052', '00096',  46, 
     '_lag Babylon.     ............', '093704') !
insert KERN.TVEU13 values ('00052', '00097',  47, 
     '%Die Mitternacht  ............', '093703') !
insert KERN.TVEU13 values ('00052', '00098',  48, 
     '_zog naeher schon,............', '093702') !
insert KERN.TVEU13 values ('00052', '00099',  49, 
     '%in stiller Ruh   ............', '093701') !
insert KERN.TVEU13 values ('00052', '00100',  50, 
     '_lag Babylon.     ............', '093700') !
insert KERN.TVEU13 values ('00052', '00101',  51, 
     '%Die Mitternacht  ............', '093659') !
insert KERN.TVEU13 values ('00052', '00102',  52, 
     '_zog naeher schon,............', '093658') !
insert KERN.TVEU13 values ('00052', '00103',  53, 
     '%in stiller Ruh   ............', '093657') !
insert KERN.TVEU13 values ('00052', '00104',  54, 
     '_lag Babylon.     ............', '093656') !
insert KERN.TVEU13 values ('00052', '00105',  55, 
     '%Die Mitternacht  ............', '093655') !
insert KERN.TVEU13 values ('00052', '00106',  56, 
     '_zog naeher schon,............', '093654') !
insert KERN.TVEU13 values ('00052', '00107',  57, 
     '%in stiller Ruh   ............', '093653') !
insert KERN.TVEU13 values ('00052', '00108',  58, 
     '_lag Babylon.     ............', '093652') !
insert KERN.TVEU13 values ('00052', '00109',  59, 
     '%Die Mitternacht  ............', '093651') !
insert KERN.TVEU13 values ('00052', '00110',  60, 
     '_zog naeher schon,............', '093650') !
insert KERN.TVEU13 values ('00052', '00111',  61, 
     '%in stiller Ruh   ............', '093649') !
insert KERN.TVEU13 values ('00052', '00112',  62, 
     '_lag Babylon.     ............', '093648') !
insert KERN.TVEU13 values ('00052', '00113',  63, 
     '%Die Mitternacht  ............', '093647') !
insert KERN.TVEU13 values ('00052', '00114',  64, 
     '_zog naeher schon,............', '093646') !
insert KERN.TVEU13 values ('00052', '00115',  65, 
     '%in stiller Ruh   ............', '093645') !
insert KERN.TVEU13 values ('00052', '00116',  66, 
     '_lag Babylon.     ............', '093644') !
insert KERN.TVEU13 values ('00052', '00117',  67, 
     '%Die Mitternacht  ............', '093643') !
insert KERN.TVEU13 values ('00052', '00118',  68, 
     '_zog naeher schon,............', '093642') !
insert KERN.TVEU13 values ('00052', '00119',  69, 
     '%in stiller Ruh   ............', '093641') !
insert KERN.TVEU13 values ('00052', '00120',  70, 
     '_lag Babylon.     ............', '093640') !
insert KERN.TVEU13 values ('00052', '00121',  71, 
     '%Die Mitternacht  ............', '093639') !
insert KERN.TVEU13 values ('00052', '00122',  72, 
     '_zog naeher schon,............', '093638') !
insert KERN.TVEU13 values ('00052', '00123',  73, 
     '%in stiller Ruh   ............', '093637') !
insert KERN.TVEU13 values ('00052', '00124',  74, 
     '_lag Babylon.     ............', '093636') !
insert KERN.TVEU13 values ('00052', '00125',  75, 
     '%Die Mitternacht  ............', '093635') !
insert KERN.TVEU13 values ('00052', '00126',  76, 
     '_zog naeher schon,............', '093634') !
insert KERN.TVEU13 values ('00052', '00127',  77, 
     '%in stiller Ruh   ............', '093633') !
insert KERN.TVEU13 values ('00052', '00128',  78, 
     '_lag Babylon.     ............', '093632') !
insert KERN.TVEU13 values ('00052', '00129',  79, 
     '%Die Mitternacht  ............', '093631') !
insert KERN.TVEU13 values ('00052', '00130',  80, 
     '_zog naeher schon,............', '093630') !
insert KERN.TVEU13 values ('00052', '00131',  81, 
     '%in stiller Ruh   ............', '093629') !
insert KERN.TVEU13 values ('00052', '00132',  82, 
     '_lag Babylon.     ............', '093628') !
insert KERN.TVEU13 values ('00052', '00133',  83, 
     '%Die Mitternacht  ............', '093627') !
insert KERN.TVEU13 values ('00052', '00134',  84, 
     '_zog naeher schon,............', '093626') !
insert KERN.TVEU13 values ('00052', '00135',  85, 
     '%in stiller Ruh   ............', '093625') !
insert KERN.TVEU13 values ('00052', '00136',  86, 
     '_lag Babylon.     ............', '093624') !
insert KERN.TVEU13 values ('00052', '00137',  87, 
     '%Die Mitternacht  ............', '093623') !
insert KERN.TVEU13 values ('00052', '00138',  88, 
     '_zog naeher schon,............', '093622') !
insert KERN.TVEU13 values ('00052', '00139',  89, 
     '%in stiller Ruh   ............', '093621') !
insert KERN.TVEU13 values ('00052', '00140',  90, 
     '_lag Babylon.     ............', '093620') !
insert KERN.TVEU13 values ('00052', '00141',  91, 
     '%Die Mitternacht  ............', '093619') !
insert KERN.TVEU13 values ('00052', '00142',  92, 
     '_zog naeher schon,............', '093618') !
insert KERN.TVEU13 values ('00052', '00143',  93, 
     '%in stiller Ruh   ............', '093617') !
insert KERN.TVEU13 values ('00052', '00144',  94, 
     '_lag Babylon.     ............', '093616') !
insert KERN.TVEU13 values ('00052', '00145',  95, 
     '%Die Mitternacht  ............', '093615') !
insert KERN.TVEU13 values ('00052', '00146',  96, 
     '_zog naeher schon,............', '093614') !
insert KERN.TVEU13 values ('00052', '00147',  97, 
     '%in stiller Ruh   ............', '093613') !
insert KERN.TVEU13 values ('00052', '00148',  98, 
     '_lag Babylon.     ............', '093612') !
insert KERN.TVEU13 values ('00052', '00149',  99, 
     '%Die Mitternacht  ............', '093611') !
insert KERN.TVEU13 values ('00052', '00150', 100, 
     '_zog naeher schon,............', '093610') !
insert KERN.TVEU13 values ('00053', '00051',   1, 
     '%in stiller Ruh   ............', '093749') !
insert KERN.TVEU13 values ('00053', '00052',   2, 
     '_lag Babylon.     ............', '093748') !
insert KERN.TVEU13 values ('00053', '00053',   3, 
     '%Die Mitternacht  ............', '093747') !
insert KERN.TVEU13 values ('00053', '00054',   4, 
     '_zog naeher schon,............', '093746') !
insert KERN.TVEU13 values ('00053', '00055',   5, 
     '%in stiller Ruh   ............', '093745') !
insert KERN.TVEU13 values ('00053', '00056',   6, 
     '_lag Babylon.     ............', '093744') !
insert KERN.TVEU13 values ('00053', '00057',   7, 
     '%Die Mitternacht  ............', '093743') !
insert KERN.TVEU13 values ('00053', '00058',   8, 
     '_zog naeher schon,............', '093742') !
insert KERN.TVEU13 values ('00053', '00059',   9, 
     '%in stiller Ruh   ............', '093741') !
insert KERN.TVEU13 values ('00053', '00060',  10, 
     '_lag Babylon.     ............', '093740') !
insert KERN.TVEU13 values ('00053', '00061',  11, 
     '%Die Mitternacht  ............', '093739') !
insert KERN.TVEU13 values ('00053', '00062',  12, 
     '_zog naeher schon,............', '093738') !
insert KERN.TVEU13 values ('00053', '00063',  13, 
     '%in stiller Ruh   ............', '093737') !
insert KERN.TVEU13 values ('00053', '00064',  14, 
     '_lag Babylon.     ............', '093736') !
insert KERN.TVEU13 values ('00053', '00065',  15, 
     '%Die Mitternacht  ............', '093735') !
insert KERN.TVEU13 values ('00053', '00066',  16, 
     '_zog naeher schon,............', '093734') !
insert KERN.TVEU13 values ('00053', '00067',  17, 
     '%in stiller Ruh   ............', '093733') !
insert KERN.TVEU13 values ('00053', '00068',  18, 
     '_lag Babylon.     ............', '093732') !
insert KERN.TVEU13 values ('00053', '00069',  19, 
     '%Die Mitternacht  ............', '093731') !
insert KERN.TVEU13 values ('00053', '00070',  20, 
     '_zog naeher schon,............', '093730') !
insert KERN.TVEU13 values ('00053', '00071',  21, 
     '%in stiller Ruh   ............', '093729') !
insert KERN.TVEU13 values ('00053', '00072',  22, 
     '_lag Babylon.     ............', '093728') !
insert KERN.TVEU13 values ('00053', '00073',  23, 
     '%Die Mitternacht  ............', '093727') !
insert KERN.TVEU13 values ('00053', '00074',  24, 
     '_zog naeher schon,............', '093726') !
insert KERN.TVEU13 values ('00053', '00075',  25, 
     '%in stiller Ruh   ............', '093725') !
insert KERN.TVEU13 values ('00053', '00076',  26, 
     '_lag Babylon.     ............', '093724') !
insert KERN.TVEU13 values ('00053', '00077',  27, 
     '%Die Mitternacht  ............', '093723') !
insert KERN.TVEU13 values ('00053', '00078',  28, 
     '_zog naeher schon,............', '093722') !
insert KERN.TVEU13 values ('00053', '00079',  29, 
     '%in stiller Ruh   ............', '093721') !
insert KERN.TVEU13 values ('00053', '00080',  30, 
     '_lag Babylon.     ............', '093720') !
insert KERN.TVEU13 values ('00053', '00081',  31, 
     '%Die Mitternacht  ............', '093719') !
insert KERN.TVEU13 values ('00053', '00082',  32, 
     '_zog naeher schon,............', '093718') !
insert KERN.TVEU13 values ('00053', '00083',  33, 
     '%in stiller Ruh   ............', '093717') !
insert KERN.TVEU13 values ('00053', '00084',  34, 
     '_lag Babylon.     ............', '093716') !
insert KERN.TVEU13 values ('00053', '00085',  35, 
     '%Die Mitternacht  ............', '093715') !
insert KERN.TVEU13 values ('00053', '00086',  36, 
     '_zog naeher schon,............', '093714') !
insert KERN.TVEU13 values ('00053', '00087',  37, 
     '%in stiller Ruh   ............', '093713') !
insert KERN.TVEU13 values ('00053', '00088',  38, 
     '_lag Babylon.     ............', '093712') !
insert KERN.TVEU13 values ('00053', '00089',  39, 
     '%Die Mitternacht  ............', '093711') !
insert KERN.TVEU13 values ('00053', '00090',  40, 
     '_zog naeher schon,............', '093710') !
insert KERN.TVEU13 values ('00053', '00091',  41, 
     '%in stiller Ruh   ............', '093709') !
insert KERN.TVEU13 values ('00053', '00092',  42, 
     '_lag Babylon.     ............', '093708') !
insert KERN.TVEU13 values ('00053', '00093',  43, 
     '%Die Mitternacht  ............', '093707') !
insert KERN.TVEU13 values ('00053', '00094',  44, 
     '_zog naeher schon,............', '093706') !
insert KERN.TVEU13 values ('00053', '00095',  45, 
     '%in stiller Ruh   ............', '093705') !
insert KERN.TVEU13 values ('00053', '00096',  46, 
     '_lag Babylon.     ............', '093704') !
insert KERN.TVEU13 values ('00053', '00097',  47, 
     '%Die Mitternacht  ............', '093703') !
insert KERN.TVEU13 values ('00053', '00098',  48, 
     '_zog naeher schon,............', '093702') !
insert KERN.TVEU13 values ('00053', '00099',  49, 
     '%in stiller Ruh   ............', '093701') !
insert KERN.TVEU13 values ('00053', '00100',  50, 
     '_lag Babylon.     ............', '093700') !
insert KERN.TVEU13 values ('00053', '00101',  51, 
     '%Die Mitternacht  ............', '093659') !
insert KERN.TVEU13 values ('00053', '00102',  52, 
     '_zog naeher schon,............', '093658') !
insert KERN.TVEU13 values ('00053', '00103',  53, 
     '%in stiller Ruh   ............', '093657') !
insert KERN.TVEU13 values ('00053', '00104',  54, 
     '_lag Babylon.     ............', '093656') !
insert KERN.TVEU13 values ('00053', '00105',  55, 
     '%Die Mitternacht  ............', '093655') !
insert KERN.TVEU13 values ('00053', '00106',  56, 
     '_zog naeher schon,............', '093654') !
insert KERN.TVEU13 values ('00053', '00107',  57, 
     '%in stiller Ruh   ............', '093653') !
insert KERN.TVEU13 values ('00053', '00108',  58, 
     '_lag Babylon.     ............', '093652') !
insert KERN.TVEU13 values ('00053', '00109',  59, 
     '%Die Mitternacht  ............', '093651') !
insert KERN.TVEU13 values ('00053', '00110',  60, 
     '_zog naeher schon,............', '093650') !
insert KERN.TVEU13 values ('00053', '00111',  61, 
     '%in stiller Ruh   ............', '093649') !
insert KERN.TVEU13 values ('00053', '00112',  62, 
     '_lag Babylon.     ............', '093648') !
insert KERN.TVEU13 values ('00053', '00113',  63, 
     '%Die Mitternacht  ............', '093647') !
insert KERN.TVEU13 values ('00053', '00114',  64, 
     '_zog naeher schon,............', '093646') !
insert KERN.TVEU13 values ('00053', '00115',  65, 
     '%in stiller Ruh   ............', '093645') !
insert KERN.TVEU13 values ('00053', '00116',  66, 
     '_lag Babylon.     ............', '093644') !
insert KERN.TVEU13 values ('00053', '00117',  67, 
     '%Die Mitternacht  ............', '093643') !
insert KERN.TVEU13 values ('00053', '00118',  68, 
     '_zog naeher schon,............', '093642') !
insert KERN.TVEU13 values ('00053', '00119',  69, 
     '%in stiller Ruh   ............', '093641') !
insert KERN.TVEU13 values ('00053', '00120',  70, 
     '_lag Babylon.     ............', '093640') !
insert KERN.TVEU13 values ('00053', '00121',  71, 
     '%Die Mitternacht  ............', '093639') !
insert KERN.TVEU13 values ('00053', '00122',  72, 
     '_zog naeher schon,............', '093638') !
insert KERN.TVEU13 values ('00053', '00123',  73, 
     '%in stiller Ruh   ............', '093637') !
insert KERN.TVEU13 values ('00053', '00124',  74, 
     '_lag Babylon.     ............', '093636') !
insert KERN.TVEU13 values ('00053', '00125',  75, 
     '%Die Mitternacht  ............', '093635') !
insert KERN.TVEU13 values ('00053', '00126',  76, 
     '_zog naeher schon,............', '093634') !
insert KERN.TVEU13 values ('00053', '00127',  77, 
     '%in stiller Ruh   ............', '093633') !
insert KERN.TVEU13 values ('00053', '00128',  78, 
     '_lag Babylon.     ............', '093632') !
insert KERN.TVEU13 values ('00053', '00129',  79, 
     '%Die Mitternacht  ............', '093631') !
insert KERN.TVEU13 values ('00053', '00130',  80, 
     '_zog naeher schon,............', '093630') !
insert KERN.TVEU13 values ('00053', '00131',  81, 
     '%in stiller Ruh   ............', '093629') !
insert KERN.TVEU13 values ('00053', '00132',  82, 
     '_lag Babylon.     ............', '093628') !
insert KERN.TVEU13 values ('00053', '00133',  83, 
     '%Die Mitternacht  ............', '093627') !
insert KERN.TVEU13 values ('00053', '00134',  84, 
     '_zog naeher schon,............', '093626') !
insert KERN.TVEU13 values ('00053', '00135',  85, 
     '%in stiller Ruh   ............', '093625') !
insert KERN.TVEU13 values ('00053', '00136',  86, 
     '_lag Babylon.     ............', '093624') !
insert KERN.TVEU13 values ('00053', '00137',  87, 
     '%Die Mitternacht  ............', '093623') !
insert KERN.TVEU13 values ('00053', '00138',  88, 
     '_zog naeher schon,............', '093622') !
insert KERN.TVEU13 values ('00053', '00139',  89, 
     '%in stiller Ruh   ............', '093621') !
insert KERN.TVEU13 values ('00053', '00140',  90, 
     '_lag Babylon.     ............', '093620') !
insert KERN.TVEU13 values ('00053', '00141',  91, 
     '%Die Mitternacht  ............', '093619') !
insert KERN.TVEU13 values ('00053', '00142',  92, 
     '_zog naeher schon,............', '093618') !
insert KERN.TVEU13 values ('00053', '00143',  93, 
     '%in stiller Ruh   ............', '093617') !
insert KERN.TVEU13 values ('00053', '00144',  94, 
     '_lag Babylon.     ............', '093616') !
insert KERN.TVEU13 values ('00053', '00145',  95, 
     '%Die Mitternacht  ............', '093615') !
insert KERN.TVEU13 values ('00053', '00146',  96, 
     '_zog naeher schon,............', '093614') !
insert KERN.TVEU13 values ('00053', '00147',  97, 
     '%in stiller Ruh   ............', '093613') !
insert KERN.TVEU13 values ('00053', '00148',  98, 
     '_lag Babylon.     ............', '093612') !
insert KERN.TVEU13 values ('00053', '00149',  99, 
     '%Die Mitternacht  ............', '093611') !
insert KERN.TVEU13 values ('00053', '00150', 100, 
     '_zog naeher schon,............', '093610') !
insert KERN.TVEU13 values ('00054', '00051',   1, 
     '%in stiller Ruh   ............', '093749') !
insert KERN.TVEU13 values ('00054', '00052',   2, 
     '_lag Babylon.     ............', '093748') !
insert KERN.TVEU13 values ('00054', '00053',   3, 
     '%Die Mitternacht  ............', '093747') !
insert KERN.TVEU13 values ('00054', '00054',   4, 
     '_zog naeher schon,............', '093746') !
insert KERN.TVEU13 values ('00054', '00055',   5, 
     '%in stiller Ruh   ............', '093745') !
insert KERN.TVEU13 values ('00054', '00056',   6, 
     '_lag Babylon.     ............', '093744') !
insert KERN.TVEU13 values ('00054', '00057',   7, 
     '%Die Mitternacht  ............', '093743') !
insert KERN.TVEU13 values ('00054', '00058',   8, 
     '_zog naeher schon,............', '093742') !
insert KERN.TVEU13 values ('00054', '00059',   9, 
     '%in stiller Ruh   ............', '093741') !
insert KERN.TVEU13 values ('00054', '00060',  10, 
     '_lag Babylon.     ............', '093740') !
insert KERN.TVEU13 values ('00054', '00061',  11, 
     '%Die Mitternacht  ............', '093739') !
insert KERN.TVEU13 values ('00054', '00062',  12, 
     '_zog naeher schon,............', '093738') !
insert KERN.TVEU13 values ('00054', '00063',  13, 
     '%in stiller Ruh   ............', '093737') !
insert KERN.TVEU13 values ('00054', '00064',  14, 
     '_lag Babylon.     ............', '093736') !
insert KERN.TVEU13 values ('00054', '00065',  15, 
     '%Die Mitternacht  ............', '093735') !
insert KERN.TVEU13 values ('00054', '00066',  16, 
     '_zog naeher schon,............', '093734') !
insert KERN.TVEU13 values ('00054', '00067',  17, 
     '%in stiller Ruh   ............', '093733') !
insert KERN.TVEU13 values ('00054', '00068',  18, 
     '_lag Babylon.     ............', '093732') !
insert KERN.TVEU13 values ('00054', '00069',  19, 
     '%Die Mitternacht  ............', '093731') !
insert KERN.TVEU13 values ('00054', '00070',  20, 
     '_zog naeher schon,............', '093730') !
insert KERN.TVEU13 values ('00054', '00071',  21, 
     '%in stiller Ruh   ............', '093729') !
insert KERN.TVEU13 values ('00054', '00072',  22, 
     '_lag Babylon.     ............', '093728') !
insert KERN.TVEU13 values ('00054', '00073',  23, 
     '%Die Mitternacht  ............', '093727') !
insert KERN.TVEU13 values ('00054', '00074',  24, 
     '_zog naeher schon,............', '093726') !
insert KERN.TVEU13 values ('00054', '00075',  25, 
     '%in stiller Ruh   ............', '093725') !
insert KERN.TVEU13 values ('00054', '00076',  26, 
     '_lag Babylon.     ............', '093724') !
insert KERN.TVEU13 values ('00054', '00077',  27, 
     '%Die Mitternacht  ............', '093723') !
insert KERN.TVEU13 values ('00054', '00078',  28, 
     '_zog naeher schon,............', '093722') !
insert KERN.TVEU13 values ('00054', '00079',  29, 
     '%in stiller Ruh   ............', '093721') !
insert KERN.TVEU13 values ('00054', '00080',  30, 
     '_lag Babylon.     ............', '093720') !
insert KERN.TVEU13 values ('00054', '00081',  31, 
     '%Die Mitternacht  ............', '093719') !
insert KERN.TVEU13 values ('00054', '00082',  32, 
     '_zog naeher schon,............', '093718') !
insert KERN.TVEU13 values ('00054', '00083',  33, 
     '%in stiller Ruh   ............', '093717') !
insert KERN.TVEU13 values ('00054', '00084',  34, 
     '_lag Babylon.     ............', '093716') !
insert KERN.TVEU13 values ('00054', '00085',  35, 
     '%Die Mitternacht  ............', '093715') !
insert KERN.TVEU13 values ('00054', '00086',  36, 
     '_zog naeher schon,............', '093714') !
insert KERN.TVEU13 values ('00054', '00087',  37, 
     '%in stiller Ruh   ............', '093713') !
insert KERN.TVEU13 values ('00054', '00088',  38, 
     '_lag Babylon.     ............', '093712') !
insert KERN.TVEU13 values ('00054', '00089',  39, 
     '%Die Mitternacht  ............', '093711') !
insert KERN.TVEU13 values ('00054', '00090',  40, 
     '_zog naeher schon,............', '093710') !
insert KERN.TVEU13 values ('00054', '00091',  41, 
     '%in stiller Ruh   ............', '093709') !
insert KERN.TVEU13 values ('00054', '00092',  42, 
     '_lag Babylon.     ............', '093708') !
insert KERN.TVEU13 values ('00054', '00093',  43, 
     '%Die Mitternacht  ............', '093707') !
insert KERN.TVEU13 values ('00054', '00094',  44, 
     '_zog naeher schon,............', '093706') !
insert KERN.TVEU13 values ('00054', '00095',  45, 
     '%in stiller Ruh   ............', '093705') !
insert KERN.TVEU13 values ('00054', '00096',  46, 
     '_lag Babylon.     ............', '093704') !
insert KERN.TVEU13 values ('00054', '00097',  47, 
     '%Die Mitternacht  ............', '093703') !
insert KERN.TVEU13 values ('00054', '00098',  48, 
     '_zog naeher schon,............', '093702') !
insert KERN.TVEU13 values ('00054', '00099',  49, 
     '%in stiller Ruh   ............', '093701') !
insert KERN.TVEU13 values ('00054', '00100',  50, 
     '_lag Babylon.     ............', '093700') !
insert KERN.TVEU13 values ('00054', '00101',  51, 
     '%Die Mitternacht  ............', '093659') !
insert KERN.TVEU13 values ('00054', '00102',  52, 
     '_zog naeher schon,............', '093658') !
insert KERN.TVEU13 values ('00054', '00103',  53, 
     '%in stiller Ruh   ............', '093657') !
insert KERN.TVEU13 values ('00054', '00104',  54, 
     '_lag Babylon.     ............', '093656') !
insert KERN.TVEU13 values ('00054', '00105',  55, 
     '%Die Mitternacht  ............', '093655') !
insert KERN.TVEU13 values ('00054', '00106',  56, 
     '_zog naeher schon,............', '093654') !
insert KERN.TVEU13 values ('00054', '00107',  57, 
     '%in stiller Ruh   ............', '093653') !
insert KERN.TVEU13 values ('00054', '00108',  58, 
     '_lag Babylon.     ............', '093652') !
insert KERN.TVEU13 values ('00054', '00109',  59, 
     '%Die Mitternacht  ............', '093651') !
insert KERN.TVEU13 values ('00054', '00110',  60, 
     '_zog naeher schon,............', '093650') !
insert KERN.TVEU13 values ('00054', '00111',  61, 
     '%in stiller Ruh   ............', '093649') !
insert KERN.TVEU13 values ('00054', '00112',  62, 
     '_lag Babylon.     ............', '093648') !
insert KERN.TVEU13 values ('00054', '00113',  63, 
     '%Die Mitternacht  ............', '093647') !
insert KERN.TVEU13 values ('00054', '00114',  64, 
     '_zog naeher schon,............', '093646') !
insert KERN.TVEU13 values ('00054', '00115',  65, 
     '%in stiller Ruh   ............', '093645') !
insert KERN.TVEU13 values ('00054', '00116',  66, 
     '_lag Babylon.     ............', '093644') !
insert KERN.TVEU13 values ('00054', '00117',  67, 
     '%Die Mitternacht  ............', '093643') !
insert KERN.TVEU13 values ('00054', '00118',  68, 
     '_zog naeher schon,............', '093642') !
insert KERN.TVEU13 values ('00054', '00119',  69, 
     '%in stiller Ruh   ............', '093641') !
insert KERN.TVEU13 values ('00054', '00120',  70, 
     '_lag Babylon.     ............', '093640') !
insert KERN.TVEU13 values ('00054', '00121',  71, 
     '%Die Mitternacht  ............', '093639') !
insert KERN.TVEU13 values ('00054', '00122',  72, 
     '_zog naeher schon,............', '093638') !
insert KERN.TVEU13 values ('00054', '00123',  73, 
     '%in stiller Ruh   ............', '093637') !
insert KERN.TVEU13 values ('00054', '00124',  74, 
     '_lag Babylon.     ............', '093636') !
insert KERN.TVEU13 values ('00054', '00125',  75, 
     '%Die Mitternacht  ............', '093635') !
insert KERN.TVEU13 values ('00054', '00126',  76, 
     '_zog naeher schon,............', '093634') !
insert KERN.TVEU13 values ('00054', '00127',  77, 
     '%in stiller Ruh   ............', '093633') !
insert KERN.TVEU13 values ('00054', '00128',  78, 
     '_lag Babylon.     ............', '093632') !
insert KERN.TVEU13 values ('00054', '00129',  79, 
     '%Die Mitternacht  ............', '093631') !
insert KERN.TVEU13 values ('00054', '00130',  80, 
     '_zog naeher schon,............', '093630') !
insert KERN.TVEU13 values ('00054', '00131',  81, 
     '%in stiller Ruh   ............', '093629') !
insert KERN.TVEU13 values ('00054', '00132',  82, 
     '_lag Babylon.     ............', '093628') !
insert KERN.TVEU13 values ('00054', '00133',  83, 
     '%Die Mitternacht  ............', '093627') !
insert KERN.TVEU13 values ('00054', '00134',  84, 
     '_zog naeher schon,............', '093626') !
insert KERN.TVEU13 values ('00054', '00135',  85, 
     '%in stiller Ruh   ............', '093625') !
insert KERN.TVEU13 values ('00054', '00136',  86, 
     '_lag Babylon.     ............', '093624') !
insert KERN.TVEU13 values ('00054', '00137',  87, 
     '%Die Mitternacht  ............', '093623') !
insert KERN.TVEU13 values ('00054', '00138',  88, 
     '_zog naeher schon,............', '093622') !
insert KERN.TVEU13 values ('00054', '00139',  89, 
     '%in stiller Ruh   ............', '093621') !
insert KERN.TVEU13 values ('00054', '00140',  90, 
     '_lag Babylon.     ............', '093620') !
insert KERN.TVEU13 values ('00054', '00141',  91, 
     '%Die Mitternacht  ............', '093619') !
insert KERN.TVEU13 values ('00054', '00142',  92, 
     '_zog naeher schon,............', '093618') !
insert KERN.TVEU13 values ('00054', '00143',  93, 
     '%in stiller Ruh   ............', '093617') !
insert KERN.TVEU13 values ('00054', '00144',  94, 
     '_lag Babylon.     ............', '093616') !
insert KERN.TVEU13 values ('00054', '00145',  95, 
     '%Die Mitternacht  ............', '093615') !
insert KERN.TVEU13 values ('00054', '00146',  96, 
     '_zog naeher schon,............', '093614') !
insert KERN.TVEU13 values ('00054', '00147',  97, 
     '%in stiller Ruh   ............', '093613') !
insert KERN.TVEU13 values ('00054', '00148',  98, 
     '_lag Babylon.     ............', '093612') !
insert KERN.TVEU13 values ('00054', '00149',  99, 
     '%Die Mitternacht  ............', '093611') !
insert KERN.TVEU13 values ('00054', '00150', 100, 
     '_zog naeher schon,............', '093610') !
insert KERN.TVEU13 values ('00151', '00151',   1, 
     '%in stiller Ruh   ............', '093609') !
insert KERN.TVEU13 values ('00151', '00152',   2, 
     '_lag Babylon.     ............', '093608') !
insert KERN.TVEU13 values ('00153', '00153',   1, 
     '%Die Mitternacht  ............', '093607') !
insert KERN.TVEU13 values ('00153', '00154',   2, 
     '_zog naeher schon,............', '093606') !
insert KERN.TVEU13 values ('00155', '00155',   1, 
     '%in stiller Ruh   ............', '093605') !
insert KERN.TVEU13 values ('00155', '00156',   2, 
     '_lag Babylon.     ............', '093604') !
insert KERN.TVEU13 values ('00157', '00157',   1, 
     '%Die Mitternacht  ............', '093603') !
insert KERN.TVEU13 values ('00157', '00158',   2, 
     '_zog naeher schon,............', '093602') !
insert KERN.TVEU13 values ('00159', '00159',   1, 
     '%in stiller Ruh   ............', '093601') !
insert KERN.TVEU13 values ('00159', '00160',   2, 
     '_lag Babylon.     ............', '093600') !
insert KERN.TVEU13 values ('00161', '00161',   1, 
     '%Die Mitternacht  ............', '093559') !
insert KERN.TVEU13 values ('00161', '00162',   2, 
     '_zog naeher schon,............', '093558') !
insert KERN.TVEU13 values ('00163', '00163',   1, 
     '%in stiller Ruh   ............', '093557') !
insert KERN.TVEU13 values ('00163', '00164',   2, 
     '_lag Babylon.     ............', '093556') !
insert KERN.TVEU13 values ('00165', '00165',   1, 
     '%Die Mitternacht  ............', '093555') !
insert KERN.TVEU13 values ('00165', '00166',   2, 
     '_zog naeher schon,............', '093554') !
insert KERN.TVEU13 values ('00167', '00167',   1, 
     '%in stiller Ruh   ............', '093553') !
insert KERN.TVEU13 values ('00167', '00168',   2, 
     '_lag Babylon.     ............', '093552') !
insert KERN.TVEU13 values ('00169', '00169',   1, 
     '%Die Mitternacht  ............', '093551') !
insert KERN.TVEU13 values ('00169', '00170',   2, 
     '_zog naeher schon,............', '093550') !
insert KERN.TVEU13 values ('00171', '00171',   1, 
     '%in stiller Ruh   ............', '093549') !
insert KERN.TVEU13 values ('00171', '00172',   2, 
     '_lag Babylon.     ............', '093548') !
insert KERN.TVEU13 values ('00173', '00173',   1, 
     '%Die Mitternacht  ............', '093547') !
insert KERN.TVEU13 values ('00173', '00174',   2, 
     '_zog naeher schon,............', '093546') !
insert KERN.TVEU13 values ('00175', '00175',   1, 
     '%in stiller Ruh   ............', '093545') !
insert KERN.TVEU13 values ('00175', '00176',   2, 
     '_lag Babylon.     ............', '093544') !
insert KERN.TVEU13 values ('00177', '00177',   1, 
     '%Die Mitternacht  ............', '093543') !
insert KERN.TVEU13 values ('00177', '00178',   2, 
     '_zog naeher schon,............', '093542') !
insert KERN.TVEU13 values ('00179', '00179',   1, 
     '%in stiller Ruh   ............', '093541') !
insert KERN.TVEU13 values ('00179', '00180',   2, 
     '_lag Babylon.     ............', '093540') !
insert KERN.TVEU13 values ('00181', '00181',   1, 
     '%Die Mitternacht  ............', '093539') !
insert KERN.TVEU13 values ('00181', '00182',   2, 
     '_zog naeher schon,............', '093538') !
insert KERN.TVEU13 values ('00183', '00183',   1, 
     '%in stiller Ruh   ............', '093537') !
insert KERN.TVEU13 values ('00183', '00184',   2, 
     '_lag Babylon.     ............', '093536') !
insert KERN.TVEU13 values ('00185', '00185',   1, 
     '%Die Mitternacht  ............', '093535') !
insert KERN.TVEU13 values ('00185', '00186',   2, 
     '_zog naeher schon,............', '093534') !
insert KERN.TVEU13 values ('00187', '00187',   1, 
     '%in stiller Ruh   ............', '093533') !
insert KERN.TVEU13 values ('00187', '00188',   2, 
     '_lag Babylon.     ............', '093532') !
insert KERN.TVEU13 values ('00189', '00189',   1, 
     '%Die Mitternacht  ............', '093531') !
insert KERN.TVEU13 values ('00189', '00190',   2, 
     '_zog naeher schon,............', '093530') !
insert KERN.TVEU13 values ('00191', '00191',   1, 
     '%in stiller Ruh   ............', '093529') !
insert KERN.TVEU13 values ('00191', '00192',   2, 
     '_lag Babylon.     ............', '093528') !
insert KERN.TVEU13 values ('00193', '00193',   1, 
     '%Die Mitternacht  ............', '093527') !
insert KERN.TVEU13 values ('00193', '00194',   2, 
     '_zog naeher schon,............', '093526') !
insert KERN.TVEU13 values ('00195', '00195',   1, 
     '%in stiller Ruh   ............', '093525') !
insert KERN.TVEU13 values ('00195', '00196',   2, 
     '_lag Babylon.     ............', '093524') !
insert KERN.TVEU13 values ('00197', '00197',   1, 
     '%Die Mitternacht  ............', '093523') !
insert KERN.TVEU13 values ('00197', '00198',   2, 
     '_zog naeher schon,............', '093522') !
insert KERN.TVEU13 values ('00199', '00199',   1, 
     '%in stiller Ruh   ............', '093521') !
insert KERN.TVEU13 values ('00199', '00200',   2, 
     '_lag Babylon.     ............', '093520') !

CREATE UNIQUE INDEX TEST1 ON TVEU12 (K1N5) !
CREATE UNIQUE INDEX TEST2 ON TVEU13 (K1N5, K2N5) !

UPDATE STAT * !

COMMIT WORK !

EXPLAIN SELECT T0001.K1N5, T0002.K2N5, T0001.F1I4, T0001.F3CH, 
          T0002.F1I4, T0002.F2TX
FROM TVEU12 T0001, TVEU13 T0002 
WHERE T0001.K1N5 = T0002.K1N5 AND 
     ( T0002.K1N5 <= '00050' OR T0002.K1N5 > '00150' ) !

SELECT T0001.K1N5, T0002.K2N5, T0001.F1I4, T0001.F3CH, 
          T0002.F1I4, T0002.F2TX
FROM TVEU12 T0001, TVEU13 T0002 
WHERE T0001.K1N5 = T0002.K1N5 AND 
     ( T0002.K1N5 <= '00050' OR T0002.K1N5 > '00150' ) !
fetch!

DROP INDEX TEST1!
DROP INDEX TEST2!

CREATE UNIQUE INDEX TEST1 ON TVEU12 (K1N5) !
CREATE UNIQUE INDEX TEST2 ON TVEU13 (K1N5, K2N5) !

COMMIT WORK !

PARSING !

SELECT T0001.K1N5, T0002.K2N5, T0001.F1I4, T0001.F3CH, 
          T0002.F1I4, T0002.F2TX
FROM TVEU12 T0001, TVEU13 T0002 
WHERE T0001.K1N5 = T0002.K1N5 AND 
     ( T0002.K1N5 <= :a OR T0002.K1N5 > :b ) 
WITH LOCK ISOLATION LEVEL 0 !

EXECUTING !
DATA!

SELECT !
1
c 5 '00050' c 5 '00150' 

NODATA !
PARS_EXECUTE!

DROP INDEX TEST1!
DROP INDEX TEST2!

CREATE UNIQUE INDEX TEST1 ON TVEU12 (K1N5) !
CREATE UNIQUE INDEX TEST2 ON TVEU13 (K1N5, K2N5) !

create table instest (
A CHAR      (5) ASCII,
B CHAR      (5) ASCII,
C FIXED     (10,0),
D CHAR      (30) ASCII,
E FIXED     (10,0),
F CHAR      (30) ASCII)!

COMMIT WORK !

insert instest  (
SELECT T0001.K1N5, T0002.K2N5, T0001.F1I4, T0001.F3CH, 
          T0002.F1I4, T0002.F2TX
FROM TVEU12 T0001, TVEU13 T0002 
WHERE T0001.K1N5 = T0002.K1N5 AND 
     ( T0002.K1N5 <= '00050' OR T0002.K1N5 > '00150' ) )!
select * from instest!
fetch!

DROP TABLE TVEU12 ! 
DROP TABLE TVEU13 !

COMMIT WORK RELEASE !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* COLUMNS_1024.vdnts
dbmcli superdba,colduser !
param_directput _PACKET_SIZE 72000 !
param_startsession !
param_checkall !
param_commitsession !
db_restart !
*db_admin!
bye !
senderid DBM !
file connect ( sut sut ! 
create user kern password test dba not exclusive !
set parameter OPTIMIZE_OPERATOR_JOIN = '&1' !
commit release !
senderid XCI !

file connect ( kern test !
create table "tabcdefghijklmno" (
CS_SITEM char (8),
CURRENCY char (8),
SID_0FISCPER3 char (30),
FISCYEAR smallint,
UNIT int,
"/BIC/ZKBBBPOS" int,
"/BIC/ZKBBMPROD" int,
"/BIC/ZKBBMSNME" int,
"/BIC/ZKBBOUNIT" int,
"/BIC/ZKBBPSVER" int,
"/BIC/ZKBBSPIND" int,
CS_TRN_GC int,
CS_TRN_LC int,
CS_TRN_QTY int,
SID_ZKBBOUNIT int)!

CREATE VIEW "/BI0/03 00346233"
AS
SELECT
"S5"."CS_SITEM" AS "0CS_SITEM",
"S6"."CS_SITEM" AS "0CS_SITEMCT",
"S7"."CURRENCY" AS "0CURKEY_GC",
"S8"."CURRENCY" AS "0CURKEY_LC",
"DT"."SID_0FISCPER3" AS "0FISCPER3",
"S9"."FISCYEAR" AS "0FISCYEAR",
"S10"."UNIT" AS "0UNIT",
"S11"."/BIC/ZKBBBPOS" AS "ZKBBBPOS",
"S12"."/BIC/ZKBBMPROD" AS "ZKBBMPROD",
"S13"."/BIC/ZKBBMSNME" AS"ZKBBMSNME",
"S14"."/BIC/ZKBBOUNIT" AS "ZKBBOPART",
"S15"."/BIC/ZKBBOUNIT" AS "ZKBBOUNIT",
"S16"."/BIC/ZKBBPSVER" AS  "ZKBBPSVER",
"S17"."/BIC/ZKBBSPIND" AS "ZKBBSPIND",
 SUM ("F"."CS_TRN_GC") AS "0CS_TRN_GC",
 SUM ("F"."CS_TRN_LC") AS "0CS_TRN_LC",
 SUM ("F"."CS_TRN_QTY") AS "0CS_TRN_QTY",
 COUNT(*) AS  "1ROWCOUNT"
FROM
"tabcdefghijklmno" "F",   "tabcdefghijklmno" "D5",
"tabcdefghijklmno" "S5",   "tabcdefghijklmno" "S6",
"tabcdefghijklmno"  "DU","tabcdefghijklmno" "S7",
"tabcdefghijklmno" "S8",
"tabcdefghijklmno" "DT",  "tabcdefghijklmno" "S9",
"tabcdefghijklmno" "S10",
"tabcdefghijklmno" "D4", "tabcdefghijklmno" "S11",
"tabcdefghijklmno" "D9", "tabcdefghijklmno" "S12",
"tabcdefghijklmno" "S13",
"tabcdefghijklmno" "D3", "tabcdefghijklmno" "S14",
"tabcdefghijklmno" "D2", "tabcdefghijklmno" "S15",
"tabcdefghijklmno" "D1",
"tabcdefghijklmno" "S16", "tabcdefghijklmno" "S17",
"tabcdefghijklmno" "D6", "tabcdefghijklmno" "DP",
"tabcdefghijklmno" "DB",
"tabcdefghijklmno" "D7", "tabcdefghijklmno" "Z1"
WHERE
("D1"."CS_TRN_GC" = 12) AND (
"D2"."SID_ZKBBOUNIT" = 7491 OR "D2"."SID_ZKBBOUNIT" =7490 OR
"D2"."SID_ZKBBOUNIT" = 7492 OR "D2"."SID_ZKBBOUNIT"= 7493 OR
"D2"."SID_ZKBBOUNIT" = 7489 OR "D2"."SID_ZKBBOUNIT" = 7488 OR
"D2"."SID_ZKBBOUNIT" = 7487 OR "D2"."SID_ZKBBOUNIT" = 7486 OR
"D2"."SID_ZKBBOUNIT" = 7494 OR "D2"."SID_ZKBBOUNIT" = 7495 OR
"D2"."SID_ZKBBOUNIT" = 7496 OR "D2"."SID_ZKBBOUNIT" = 7497 OR
"D2"."SID_ZKBBOUNIT" =7498 OR "D2"."SID_ZKBBOUNIT" = 7499 OR
"D2"."SID_ZKBBOUNIT"= 7500 OR "D2"."SID_ZKBBOUNIT" = 7501 OR
"D2"."SID_ZKBBOUNIT" = 7502 OR "D2"."SID_ZKBBOUNIT" = 7485 OR
"D2"."SID_ZKBBOUNIT" = 7468 OR "D2"."SID_ZKBBOUNIT" = 7469 OR
"D2"."SID_ZKBBOUNIT" = 7470 OR "D2"."SID_ZKBBOUNIT" =7471 OR
"D2"."SID_ZKBBOUNIT" = 7472 OR "D2"."SID_ZKBBOUNIT"= 7473 OR
"D2"."SID_ZKBBOUNIT" = 7474 OR "D2"."SID_ZKBBOUNIT" = 7475 OR
"D2"."SID_ZKBBOUNIT" = 7476 OR "D2"."SID_ZKBBOUNIT" = 7477 OR
"D2"."SID_ZKBBOUNIT" = 7478 OR "D2"."SID_ZKBBOUNIT" = 7479 OR
"D2"."SID_ZKBBOUNIT" = 7480 OR "D2"."SID_ZKBBOUNIT" = 7481 OR
"D2"."SID_ZKBBOUNIT" =7482 OR "D2"."SID_ZKBBOUNIT" = 7483 OR
"D2"."SID_ZKBBOUNIT"= 7484 OR "D2"."SID_ZKBBOUNIT" = 7521 OR
"D2"."SID_ZKBBOUNIT" = 7522 OR "D2"."SID_ZKBBOUNIT" = 7523 OR
"D2"."SID_ZKBBOUNIT" = 7524 OR "D2"."SID_ZKBBOUNIT" = 7525 OR
"D2"."SID_ZKBBOUNIT" = 7526 OR "D2"."SID_ZKBBOUNIT" =7527 OR
"D2"."SID_ZKBBOUNIT" = 7528 OR "D2"."SID_ZKBBOUNIT"= 7529 OR
"D2"."SID_ZKBBOUNIT" = 7530 OR "D2"."SID_ZKBBOUNIT" = 7531 OR
"D2"."SID_ZKBBOUNIT" = 7491 OR "D2"."SID_ZKBBOUNIT" =7490 OR
"D2"."SID_ZKBBOUNIT" = 7492 OR "D2"."SID_ZKBBOUNIT"= 7493 OR
"D2"."SID_ZKBBOUNIT" = 7489 OR "D2"."SID_ZKBBOUNIT" = 7488 OR
"D2"."SID_ZKBBOUNIT" = 7487 OR "D2"."SID_ZKBBOUNIT" = 7486 OR
"D2"."SID_ZKBBOUNIT" = 7494 OR "D2"."SID_ZKBBOUNIT" = 7495 OR
"D2"."SID_ZKBBOUNIT" = 7496 OR "D2"."SID_ZKBBOUNIT" = 7497 OR
"D2"."SID_ZKBBOUNIT" =7498 OR "D2"."SID_ZKBBOUNIT" = 7499 OR
"D2"."SID_ZKBBOUNIT"= 7500 OR "D2"."SID_ZKBBOUNIT" = 7501 OR
"D2"."SID_ZKBBOUNIT" = 7502 OR "D2"."SID_ZKBBOUNIT" = 7485 OR
"D2"."SID_ZKBBOUNIT" = 7468 OR "D2"."SID_ZKBBOUNIT" = 7469 OR
"D2"."SID_ZKBBOUNIT" = 7470 OR "D2"."SID_ZKBBOUNIT" =7471 OR
"D2"."SID_ZKBBOUNIT" = 7472 OR "D2"."SID_ZKBBOUNIT"= 7473 OR
"D2"."SID_ZKBBOUNIT" = 7474 OR "D2"."SID_ZKBBOUNIT" = 7475 OR
"D2"."SID_ZKBBOUNIT" = 7476 OR "D2"."SID_ZKBBOUNIT" = 7477 OR
"D2"."SID_ZKBBOUNIT" = 7478 OR "D2"."SID_ZKBBOUNIT" = 7479 OR
"D2"."SID_ZKBBOUNIT" = 7480 OR "D2"."SID_ZKBBOUNIT" = 7481 OR
"D2"."SID_ZKBBOUNIT" =7482 OR "D2"."SID_ZKBBOUNIT" = 7483 OR
"D2"."SID_ZKBBOUNIT"= 7484 OR "D2"."SID_ZKBBOUNIT" = 7521 OR
"D2"."SID_ZKBBOUNIT" = 7522 OR "D2"."SID_ZKBBOUNIT" = 7523 OR
"D2"."SID_ZKBBOUNIT" = 7524 OR "D2"."SID_ZKBBOUNIT" = 7525 OR
"D2"."SID_ZKBBOUNIT" = 7526 OR "D2"."SID_ZKBBOUNIT" =7527 OR
"D2"."SID_ZKBBOUNIT" = 7528 OR "D2"."SID_ZKBBOUNIT"= 7529 OR
"D2"."SID_ZKBBOUNIT" = 7530 OR "D2"."SID_ZKBBOUNIT" = 7531 OR
"D2"."SID_ZKBBOUNIT" = 7532 OR "D2"."SID_ZKBBOUNIT" = 7533 OR
"D2"."SID_ZKBBOUNIT" = 7534 OR "D2"."SID_ZKBBOUNIT" = 7535 OR
"D2"."SID_ZKBBOUNIT" = 7536 OR "D2"."SID_ZKBBOUNIT" = 7537 OR
"D2"."SID_ZKBBOUNIT" =7520 OR "D2"."SID_ZKBBOUNIT" = 7503 OR
"D2"."SID_ZKBBOUNIT"= 7504 OR "D2"."SID_ZKBBOUNIT" = 7505 OR
"D2"."SID_ZKBBOUNIT" = 7506 OR "D2"."SID_ZKBBOUNIT" = 7507 OR
"D2"."SID_ZKBBOUNIT" = 7508 OR "D2"."SID_ZKBBOUNIT" = 7509 OR
"D2"."SID_ZKBBOUNIT" = 7510 OR "D2"."SID_ZKBBOUNIT" =7511 OR
"D2"."SID_ZKBBOUNIT" = 7512 OR "D2"."SID_ZKBBOUNIT"= 7513 OR
"D2"."SID_ZKBBOUNIT" = 7514 OR "D2"."SID_ZKBBOUNIT" = 7515 OR
"D2"."SID_ZKBBOUNIT" = 7516 OR "D2"."SID_ZKBBOUNIT" = 7517 OR
"D2"."SID_ZKBBOUNIT" = 7518 OR "D2"."SID_ZKBBOUNIT" = 7519 OR
"D2"."SID_ZKBBOUNIT" = 7467 OR "D2"."SID_ZKBBOUNIT" = 7418 OR
"D2"."SID_ZKBBOUNIT" =7419 OR "D2"."SID_ZKBBOUNIT" = 7420 OR
"D2"."SID_ZKBBOUNIT"= 7421 OR "D2"."SID_ZKBBOUNIT" = 7422 OR
"D2"."SID_ZKBBOUNIT" = 7423 OR "D2"."SID_ZKBBOUNIT" = 7424 OR
"D2"."SID_ZKBBOUNIT" = 7425 OR "D2"."SID_ZKBBOUNIT" = 7426 OR
"D2"."SID_ZKBBOUNIT" = 7427 OR "D2"."SID_ZKBBOUNIT" =7428 OR
"D2"."SID_ZKBBOUNIT" = 7429 OR "D2"."SID_ZKBBOUNIT"= 7430 OR
"D2"."SID_ZKBBOUNIT" = 7431 OR "D2"."SID_ZKBBOUNIT" = 7432 OR
"D2"."SID_ZKBBOUNIT" = 7433 OR "D2"."SID_ZKBBOUNIT" = 7434 OR
"D2"."SID_ZKBBOUNIT" = 7472 OR "D2"."SID_ZKBBOUNIT"= 7473 OR
"D2"."SID_ZKBBOUNIT" = 7474 OR "D2"."SID_ZKBBOUNIT" = 7475 OR
"D2"."SID_ZKBBOUNIT" = 7476 OR "D2"."SID_ZKBBOUNIT" = 7477 OR
"D2"."SID_ZKBBOUNIT" = 7478 OR "D2"."SID_ZKBBOUNIT" = 7479 OR
"D2"."SID_ZKBBOUNIT" = 7480 OR "D2"."SID_ZKBBOUNIT" = 7481 OR
"D2"."SID_ZKBBOUNIT" =7482 OR "D2"."SID_ZKBBOUNIT" = 7483 OR
"D2"."SID_ZKBBOUNIT"= 7484 OR "D2"."SID_ZKBBOUNIT" = 7521 OR
"D2"."SID_ZKBBOUNIT" = 7522 OR "D2"."SID_ZKBBOUNIT" = 7523 OR
"D2"."SID_ZKBBOUNIT" = 7524 OR "D2"."SID_ZKBBOUNIT" = 7525 OR
"D2"."SID_ZKBBOUNIT" = 7526 OR "D2"."SID_ZKBBOUNIT" =7527 OR
"D2"."SID_ZKBBOUNIT" = 7528 OR "D2"."SID_ZKBBOUNIT"= 7529 OR
"D2"."SID_ZKBBOUNIT" = 7530 OR "D2"."SID_ZKBBOUNIT" = 7531 OR
"D2"."SID_ZKBBOUNIT" = 7532 OR "D2"."SID_ZKBBOUNIT" = 7533 OR
"D2"."SID_ZKBBOUNIT" = 7534 OR "D2"."SID_ZKBBOUNIT" = 7535 OR
"D2"."SID_ZKBBOUNIT" = 7536 OR "D2"."SID_ZKBBOUNIT" = 7537 OR
"D2"."SID_ZKBBOUNIT" =7520 OR "D2"."SID_ZKBBOUNIT" = 7503 OR
"D2"."SID_ZKBBOUNIT"= 7504 OR "D2"."SID_ZKBBOUNIT" = 7505 OR
"D2"."SID_ZKBBOUNIT" = 7506 OR "D2"."SID_ZKBBOUNIT" = 7507 OR
"D2"."SID_ZKBBOUNIT" = 7508 OR "D2"."SID_ZKBBOUNIT" = 7509 OR
"D2"."SID_ZKBBOUNIT" = 7510 OR "D2"."SID_ZKBBOUNIT" =7511 OR
"D2"."SID_ZKBBOUNIT" = 7512 OR "D2"."SID_ZKBBOUNIT"= 7513 OR
"D2"."SID_ZKBBOUNIT" = 7514 OR "D2"."SID_ZKBBOUNIT" = 7515 OR
"D2"."SID_ZKBBOUNIT" = 7516 OR "D2"."SID_ZKBBOUNIT" = 7517 OR
"D2"."SID_ZKBBOUNIT" = 7518 OR "D2"."SID_ZKBBOUNIT" = 7519 OR
"D2"."SID_ZKBBOUNIT" = 7467 OR "D2"."SID_ZKBBOUNIT" = 7418 OR
"D2"."SID_ZKBBOUNIT" = 7472 OR "D2"."SID_ZKBBOUNIT"= 7473 OR
"D2"."SID_ZKBBOUNIT" = 7474 OR "D2"."SID_ZKBBOUNIT" = 7475 OR
"D2"."SID_ZKBBOUNIT" = 7476 OR "D2"."SID_ZKBBOUNIT" = 7477 OR
"D2"."SID_ZKBBOUNIT" = 7478 OR "D2"."SID_ZKBBOUNIT" = 7479 OR
"D2"."SID_ZKBBOUNIT" = 7480 OR "D2"."SID_ZKBBOUNIT" = 7481 OR
"D2"."SID_ZKBBOUNIT" =7482 OR "D2"."SID_ZKBBOUNIT" = 7483 OR
"D2"."SID_ZKBBOUNIT"= 7484 OR "D2"."SID_ZKBBOUNIT" = 7521 OR
"D2"."SID_ZKBBOUNIT" = 7522 OR "D2"."SID_ZKBBOUNIT" = 7523 OR
"D2"."SID_ZKBBOUNIT" = 7524 OR "D2"."SID_ZKBBOUNIT" = 7525 OR
"D2"."SID_ZKBBOUNIT" = 7526 OR "D2"."SID_ZKBBOUNIT" =7527 OR
"D2"."SID_ZKBBOUNIT" = 7528 OR "D2"."SID_ZKBBOUNIT"= 7529 OR
"D2"."SID_ZKBBOUNIT" = 7530 OR "D2"."SID_ZKBBOUNIT" = 7531 OR
"D2"."SID_ZKBBOUNIT" = 7532 OR "D2"."SID_ZKBBOUNIT" = 7533 OR
"D2"."SID_ZKBBOUNIT" = 7534 OR "D2"."SID_ZKBBOUNIT" = 7535 OR
"D2"."SID_ZKBBOUNIT" = 7536 OR "D2"."SID_ZKBBOUNIT" = 7537 OR
"D2"."SID_ZKBBOUNIT" =7520 OR "D2"."SID_ZKBBOUNIT" = 7503 OR
"D2"."SID_ZKBBOUNIT"= 7504 OR "D2"."SID_ZKBBOUNIT" = 7505 OR
"D2"."SID_ZKBBOUNIT" = 7506 OR "D2"."SID_ZKBBOUNIT" = 7507 OR
"D2"."SID_ZKBBOUNIT" = 7508 OR "D2"."SID_ZKBBOUNIT" = 7509 OR
"D2"."SID_ZKBBOUNIT" = 7510 OR "D2"."SID_ZKBBOUNIT" =7511 OR
"D2"."SID_ZKBBOUNIT" = 7512 OR "D2"."SID_ZKBBOUNIT"= 7513 OR
"D2"."SID_ZKBBOUNIT" = 7514 OR "D2"."SID_ZKBBOUNIT" = 7515 OR
"D2"."SID_ZKBBOUNIT" = 7516 OR "D2"."SID_ZKBBOUNIT" = 7517 OR
"D2"."SID_ZKBBOUNIT" = 7518 OR "D2"."SID_ZKBBOUNIT" = 7519 OR
"D2"."SID_ZKBBOUNIT" = 7467 OR "D2"."SID_ZKBBOUNIT" = 7418 OR
"D2"."SID_ZKBBOUNIT" = 7472 OR "D2"."SID_ZKBBOUNIT"= 7473 OR
"D2"."SID_ZKBBOUNIT" = 7474 OR "D2"."SID_ZKBBOUNIT" = 7475 OR
"D2"."SID_ZKBBOUNIT" = 7476 OR "D2"."SID_ZKBBOUNIT" = 7477 OR
"D2"."SID_ZKBBOUNIT" = 7478 OR "D2"."SID_ZKBBOUNIT" = 7479 OR
"D2"."SID_ZKBBOUNIT" = 7480 OR "D2"."SID_ZKBBOUNIT" = 7481 OR
"D2"."SID_ZKBBOUNIT" =7482 OR "D2"."SID_ZKBBOUNIT" = 7483 OR
"D2"."SID_ZKBBOUNIT"= 7484 OR "D2"."SID_ZKBBOUNIT" = 7521 OR
"D2"."SID_ZKBBOUNIT" = 7522 OR "D2"."SID_ZKBBOUNIT" = 7523 OR
"D2"."SID_ZKBBOUNIT" = 7524 OR "D2"."SID_ZKBBOUNIT" = 7525 OR
"D2"."SID_ZKBBOUNIT" = 7526 OR "D2"."SID_ZKBBOUNIT" =7527 OR
"D2"."SID_ZKBBOUNIT" = 7528 OR "D2"."SID_ZKBBOUNIT"= 7529 OR
"D2"."SID_ZKBBOUNIT" = 7530 OR "D2"."SID_ZKBBOUNIT" = 7531 OR
"D2"."SID_ZKBBOUNIT" = 7532 OR "D2"."SID_ZKBBOUNIT" = 7533 OR
"D2"."SID_ZKBBOUNIT" = 7534 OR "D2"."SID_ZKBBOUNIT" = 7535 OR
"D2"."SID_ZKBBOUNIT" = 7536 OR "D2"."SID_ZKBBOUNIT" = 7537 OR
"D2"."SID_ZKBBOUNIT" =7520 OR "D2"."SID_ZKBBOUNIT" = 7503 OR
"D2"."SID_ZKBBOUNIT"= 7504 OR "D2"."SID_ZKBBOUNIT" = 7505 OR
"D2"."SID_ZKBBOUNIT" = 7506 OR "D2"."SID_ZKBBOUNIT" = 7507 OR
"D2"."SID_ZKBBOUNIT" = 7508 OR "D2"."SID_ZKBBOUNIT" = 7509 OR
"D2"."SID_ZKBBOUNIT" = 7510 OR "D2"."SID_ZKBBOUNIT" =7511 OR
"D2"."SID_ZKBBOUNIT" = 7512 OR "D2"."SID_ZKBBOUNIT"= 7513 OR
"D2"."SID_ZKBBOUNIT" = 7514 OR "D2"."SID_ZKBBOUNIT" = 7515 OR
"D2"."SID_ZKBBOUNIT" = 7516 OR "D2"."SID_ZKBBOUNIT" = 7517 OR
"D2"."SID_ZKBBOUNIT" = 7518 OR "D2"."SID_ZKBBOUNIT" = 7519 OR
"D2"."SID_ZKBBOUNIT" = 7467 OR "D2"."SID_ZKBBOUNIT" = 7418 OR
"D2"."SID_ZKBBOUNIT" = 7472 OR "D2"."SID_ZKBBOUNIT"= 7473 OR
"D2"."SID_ZKBBOUNIT" = 7474 OR "D2"."SID_ZKBBOUNIT" = 7475 OR
"D2"."SID_ZKBBOUNIT" = 7476 OR "D2"."SID_ZKBBOUNIT" = 7477 OR
"D2"."SID_ZKBBOUNIT" = 7478 OR "D2"."SID_ZKBBOUNIT" = 7479 OR
"D2"."SID_ZKBBOUNIT" = 7480 OR "D2"."SID_ZKBBOUNIT" = 7481 OR
"D2"."SID_ZKBBOUNIT" =7482 OR "D2"."SID_ZKBBOUNIT" = 7483 OR
"D2"."SID_ZKBBOUNIT"= 7484 OR "D2"."SID_ZKBBOUNIT" = 7521 OR
"D2"."SID_ZKBBOUNIT" = 7522 OR "D2"."SID_ZKBBOUNIT" = 7523 OR
"D2"."SID_ZKBBOUNIT" = 7524 OR "D2"."SID_ZKBBOUNIT" = 7525 OR
"D2"."SID_ZKBBOUNIT" = 7526 OR "D2"."SID_ZKBBOUNIT" =7527 OR
"D2"."SID_ZKBBOUNIT" = 7528 OR "D2"."SID_ZKBBOUNIT"= 7529 OR
"D2"."SID_ZKBBOUNIT" = 7530 OR "D2"."SID_ZKBBOUNIT" = 7531 OR
"D2"."SID_ZKBBOUNIT" = 7532 OR "D2"."SID_ZKBBOUNIT" = 7533 OR
"D2"."SID_ZKBBOUNIT" = 7534 OR "D2"."SID_ZKBBOUNIT" = 7535 OR
"D2"."SID_ZKBBOUNIT" = 7536 OR "D2"."SID_ZKBBOUNIT" = 7537 OR
"D2"."SID_ZKBBOUNIT" =7520 OR "D2"."SID_ZKBBOUNIT" = 7503 OR
"D2"."SID_ZKBBOUNIT"= 7504 OR "D2"."SID_ZKBBOUNIT" = 7505 OR
"D2"."SID_ZKBBOUNIT" = 7506 OR "D2"."SID_ZKBBOUNIT" = 7507 OR
"D2"."SID_ZKBBOUNIT" = 7508 OR "D2"."SID_ZKBBOUNIT" = 7509 OR
"D2"."SID_ZKBBOUNIT" = 7510 OR "D2"."SID_ZKBBOUNIT" =7511 OR
"D2"."SID_ZKBBOUNIT" = 7512 OR "D2"."SID_ZKBBOUNIT"= 7513 OR
"D2"."SID_ZKBBOUNIT" = 7514 OR "D2"."SID_ZKBBOUNIT" = 7515 OR
"D2"."SID_ZKBBOUNIT" = 7516 OR "D2"."SID_ZKBBOUNIT" = 7517 OR
"D2"."SID_ZKBBOUNIT" = 7518 OR "D2"."SID_ZKBBOUNIT" = 7519 OR
"D2"."SID_ZKBBOUNIT" = 7467 OR "D2"."SID_ZKBBOUNIT" = 7418 OR
"D2"."SID_ZKBBOUNIT" = 7472 OR "D2"."SID_ZKBBOUNIT"= 7473 OR
"D2"."SID_ZKBBOUNIT" = 7474 OR "D2"."SID_ZKBBOUNIT" = 7475 OR
"D2"."SID_ZKBBOUNIT" = 7476 OR "D2"."SID_ZKBBOUNIT" = 7477 OR
"D2"."SID_ZKBBOUNIT" = 7478 OR "D2"."SID_ZKBBOUNIT" = 7479 OR
"D2"."SID_ZKBBOUNIT" = 7480 OR "D2"."SID_ZKBBOUNIT" = 7481 OR
"D2"."SID_ZKBBOUNIT" =7482 OR "D2"."SID_ZKBBOUNIT" = 7483 OR
"D2"."SID_ZKBBOUNIT"= 7484 OR "D2"."SID_ZKBBOUNIT" = 7521 OR
"D2"."SID_ZKBBOUNIT" = 7522 OR "D2"."SID_ZKBBOUNIT" = 7523 OR
"D2"."SID_ZKBBOUNIT" = 7524 OR "D2"."SID_ZKBBOUNIT" = 7525 OR
"D2"."SID_ZKBBOUNIT" = 7526 OR "D2"."SID_ZKBBOUNIT" =7527 OR
"D2"."SID_ZKBBOUNIT" = 7528 OR "D2"."SID_ZKBBOUNIT"= 7529 OR
"D2"."SID_ZKBBOUNIT" = 7530 OR "D2"."SID_ZKBBOUNIT" = 7531 OR
"D2"."SID_ZKBBOUNIT" = 7532 OR "D2"."SID_ZKBBOUNIT" = 7533 OR
"D2"."SID_ZKBBOUNIT" = 7534 OR "D2"."SID_ZKBBOUNIT" = 7535 OR
"D2"."SID_ZKBBOUNIT" = 7536 OR "D2"."SID_ZKBBOUNIT" = 7537 OR
"D2"."SID_ZKBBOUNIT" =7520 OR "D2"."SID_ZKBBOUNIT" = 7503 OR
"D2"."SID_ZKBBOUNIT"= 7504 OR "D2"."SID_ZKBBOUNIT" = 7505 OR
"D2"."SID_ZKBBOUNIT" = 7506 OR "D2"."SID_ZKBBOUNIT" = 7507 OR
"D2"."SID_ZKBBOUNIT" = 7508 OR "D2"."SID_ZKBBOUNIT" = 7509 OR
"D2"."SID_ZKBBOUNIT" = 7510 OR "D2"."SID_ZKBBOUNIT" =7511 OR
"D2"."SID_ZKBBOUNIT" = 7512 OR "D2"."SID_ZKBBOUNIT"= 7513 OR
"D2"."SID_ZKBBOUNIT" = 7514 OR "D2"."SID_ZKBBOUNIT" = 7515 OR
"D2"."SID_ZKBBOUNIT" = 7516 OR "D2"."SID_ZKBBOUNIT" = 7517 OR
"D2"."SID_ZKBBOUNIT" = 7518 OR "D2"."SID_ZKBBOUNIT" = 7519 OR
"D2"."SID_ZKBBOUNIT" = 7467 OR "D2"."SID_ZKBBOUNIT" = 7418 OR
"D2"."SID_ZKBBOUNIT" = 7472 OR "D2"."SID_ZKBBOUNIT"= 7473 OR
"D2"."SID_ZKBBOUNIT" = 7474 OR "D2"."SID_ZKBBOUNIT" = 7475 OR
"D2"."SID_ZKBBOUNIT" = 7476 OR "D2"."SID_ZKBBOUNIT" = 7477 OR
"D2"."SID_ZKBBOUNIT" = 7478 OR "D2"."SID_ZKBBOUNIT" = 7479 OR
"D2"."SID_ZKBBOUNIT" = 7480 OR "D2"."SID_ZKBBOUNIT" = 7481 OR
"D2"."SID_ZKBBOUNIT" =7482 OR "D2"."SID_ZKBBOUNIT" = 7483 OR
"D2"."SID_ZKBBOUNIT"= 7484 OR "D2"."SID_ZKBBOUNIT" = 7521 OR
"D2"."SID_ZKBBOUNIT" = 7522 OR "D2"."SID_ZKBBOUNIT" = 7523 OR
"D2"."SID_ZKBBOUNIT" = 7524 OR "D2"."SID_ZKBBOUNIT" = 7525 OR
"D2"."SID_ZKBBOUNIT" = 7526 OR "D2"."SID_ZKBBOUNIT" =7527 OR
"D2"."SID_ZKBBOUNIT" = 7528 OR "D2"."SID_ZKBBOUNIT"= 7529 OR
"D2"."SID_ZKBBOUNIT" = 7530 OR "D2"."SID_ZKBBOUNIT" = 7531 OR
"D2"."SID_ZKBBOUNIT" = 7532 OR "D2"."SID_ZKBBOUNIT" = 7533 OR
"D2"."SID_ZKBBOUNIT" = 7534 OR "D2"."SID_ZKBBOUNIT" = 7535 OR
"D2"."SID_ZKBBOUNIT" = 7536 OR "D2"."SID_ZKBBOUNIT" = 7537 OR
"D2"."SID_ZKBBOUNIT" =7520 OR "D2"."SID_ZKBBOUNIT" = 7503 OR
"D2"."SID_ZKBBOUNIT"= 7504 OR "D2"."SID_ZKBBOUNIT" = 7505 OR
"D2"."SID_ZKBBOUNIT" = 7506 OR "D2"."SID_ZKBBOUNIT" = 7507 OR
"D2"."SID_ZKBBOUNIT" = 7508 OR "D2"."SID_ZKBBOUNIT" = 7509 OR
"D2"."SID_ZKBBOUNIT" = 7510 OR "D2"."SID_ZKBBOUNIT" =7511 OR
"D2"."SID_ZKBBOUNIT" = 7512 OR "D2"."SID_ZKBBOUNIT"= 7513 OR
"D2"."SID_ZKBBOUNIT" = 7514 OR "D2"."SID_ZKBBOUNIT" = 7515 OR
"D2"."SID_ZKBBOUNIT" = 7516 OR "D2"."SID_ZKBBOUNIT" = 7517 OR
"D2"."SID_ZKBBOUNIT" = 7518 OR "D2"."SID_ZKBBOUNIT" = 7519 OR
"D2"."SID_ZKBBOUNIT" = 7467 OR "D2"."SID_ZKBBOUNIT" = 7418 OR
"D2"."SID_ZKBBOUNIT" = 7472 OR "D2"."SID_ZKBBOUNIT"= 7473 OR
"D2"."SID_ZKBBOUNIT" = 7474 OR "D2"."SID_ZKBBOUNIT" = 7475 OR
"D2"."SID_ZKBBOUNIT" = 7476 OR "D2"."SID_ZKBBOUNIT" = 7477 OR
"D2"."SID_ZKBBOUNIT" = 7478 OR "D2"."SID_ZKBBOUNIT" = 7479 OR
"D2"."SID_ZKBBOUNIT" = 7480 OR "D2"."SID_ZKBBOUNIT" = 7481 OR
"D2"."SID_ZKBBOUNIT" =7482 OR "D2"."SID_ZKBBOUNIT" = 7483 OR
"D2"."SID_ZKBBOUNIT"= 7484 OR "D2"."SID_ZKBBOUNIT" = 7521 OR
"D2"."SID_ZKBBOUNIT" = 7522 OR "D2"."SID_ZKBBOUNIT" = 7523 OR
"D2"."SID_ZKBBOUNIT" = 7524 OR "D2"."SID_ZKBBOUNIT" = 7525 OR
"D2"."SID_ZKBBOUNIT" = 7526 OR "D2"."SID_ZKBBOUNIT" =7527 OR
"D2"."SID_ZKBBOUNIT" = 7528 OR "D2"."SID_ZKBBOUNIT"= 7529 OR
"D2"."SID_ZKBBOUNIT" = 7530 OR "D2"."SID_ZKBBOUNIT" = 7531 OR
"D2"."SID_ZKBBOUNIT" = 7532 OR "D2"."SID_ZKBBOUNIT" = 7533 OR
"D2"."SID_ZKBBOUNIT" = 7534 OR "D2"."SID_ZKBBOUNIT" = 7535 OR
"D2"."SID_ZKBBOUNIT" = 7536 OR "D2"."SID_ZKBBOUNIT" = 7537 OR
"D2"."SID_ZKBBOUNIT" =7520 OR "D2"."SID_ZKBBOUNIT" = 7503 OR
"D2"."SID_ZKBBOUNIT"= 7504 OR "D2"."SID_ZKBBOUNIT" = 7505 OR
"D2"."SID_ZKBBOUNIT" = 7506 OR "D2"."SID_ZKBBOUNIT" = 7507 OR
"D2"."SID_ZKBBOUNIT" = 7508 OR "D2"."SID_ZKBBOUNIT" = 7509 OR
"D2"."SID_ZKBBOUNIT" = 7510 OR "D2"."SID_ZKBBOUNIT" =7511 OR
"D2"."SID_ZKBBOUNIT" = 7512 OR "D2"."SID_ZKBBOUNIT"= 7513 OR
"D2"."SID_ZKBBOUNIT" = 7514 OR "D2"."SID_ZKBBOUNIT" = 7515 OR
"D2"."SID_ZKBBOUNIT" = 7516 OR "D2"."SID_ZKBBOUNIT" = 7517 OR
"D2"."SID_ZKBBOUNIT" = 7518 OR "D2"."SID_ZKBBOUNIT" = 7519 OR
"D2"."SID_ZKBBOUNIT" = 7467 OR "D2"."SID_ZKBBOUNIT" = 7418 OR
"D2"."SID_ZKBBOUNIT" = 7472 OR "D2"."SID_ZKBBOUNIT"= 7473 OR
"D2"."SID_ZKBBOUNIT" = 7474 OR "D2"."SID_ZKBBOUNIT" = 7475 OR
"D2"."SID_ZKBBOUNIT" = 7476 OR "D2"."SID_ZKBBOUNIT" = 7477 OR
"D2"."SID_ZKBBOUNIT" = 7478 OR "D2"."SID_ZKBBOUNIT" = 7479 OR
"D2"."SID_ZKBBOUNIT" = 7480 OR "D2"."SID_ZKBBOUNIT" = 7481 OR
"D2"."SID_ZKBBOUNIT" =7482 OR "D2"."SID_ZKBBOUNIT" = 7483 OR
"D2"."SID_ZKBBOUNIT"= 7484 OR "D2"."SID_ZKBBOUNIT" = 7521 OR
"D2"."SID_ZKBBOUNIT" = 7522 OR "D2"."SID_ZKBBOUNIT" = 7523 OR
"D2"."SID_ZKBBOUNIT" = 7524 OR "D2"."SID_ZKBBOUNIT" = 7525 OR
"D2"."SID_ZKBBOUNIT" = 7526 OR "D2"."SID_ZKBBOUNIT" =7527 OR
"D2"."SID_ZKBBOUNIT" = 7528 OR "D2"."SID_ZKBBOUNIT"= 7529 OR
"D2"."SID_ZKBBOUNIT" = 7530 OR "D2"."SID_ZKBBOUNIT" = 7531 OR
"D2"."SID_ZKBBOUNIT" = 7532 OR "D2"."SID_ZKBBOUNIT" = 7533 OR
"D2"."SID_ZKBBOUNIT" = 7534 OR "D2"."SID_ZKBBOUNIT" = 7535 OR
"D2"."SID_ZKBBOUNIT" = 7536 OR "D2"."SID_ZKBBOUNIT" = 7537 OR
"D2"."SID_ZKBBOUNIT" =7520 OR "D2"."SID_ZKBBOUNIT" = 7503 OR
"D2"."SID_ZKBBOUNIT"= 7504 OR "D2"."SID_ZKBBOUNIT" = 7505 OR
"D2"."SID_ZKBBOUNIT" = 7506 OR "D2"."SID_ZKBBOUNIT" = 7507 OR
"D2"."SID_ZKBBOUNIT" = 7508 OR "D2"."SID_ZKBBOUNIT" = 7509 OR
"D2"."SID_ZKBBOUNIT" = 7510 OR "D2"."SID_ZKBBOUNIT" =7511 OR
"D2"."SID_ZKBBOUNIT" = 7512 OR "D2"."SID_ZKBBOUNIT"= 7513 OR
"D2"."SID_ZKBBOUNIT" = 7514 OR "D2"."SID_ZKBBOUNIT" = 7515 OR
"D2"."SID_ZKBBOUNIT" = 7516 OR "D2"."SID_ZKBBOUNIT" = 7517 OR
"D2"."SID_ZKBBOUNIT" = 7518 OR "D2"."SID_ZKBBOUNIT" = 7519 OR
"D2"."SID_ZKBBOUNIT" = 7467 OR "D2"."SID_ZKBBOUNIT" = 7418 OR
"D2"."SID_ZKBBOUNIT" = 7472 OR "D2"."SID_ZKBBOUNIT"= 7473 OR
"D2"."SID_ZKBBOUNIT" = 7474 OR "D2"."SID_ZKBBOUNIT" = 7475 OR
"D2"."SID_ZKBBOUNIT" = 7476 OR "D2"."SID_ZKBBOUNIT" = 7477 OR
"D2"."SID_ZKBBOUNIT" = 7478 OR "D2"."SID_ZKBBOUNIT" = 7479 OR
"D2"."SID_ZKBBOUNIT" = 7480 OR "D2"."SID_ZKBBOUNIT" = 7481 OR
"D2"."SID_ZKBBOUNIT" =7482 OR "D2"."SID_ZKBBOUNIT" = 7483 OR
"D2"."SID_ZKBBOUNIT"= 7484 OR "D2"."SID_ZKBBOUNIT" = 7521 OR
"D2"."SID_ZKBBOUNIT" = 7522 OR "D2"."SID_ZKBBOUNIT" = 7523 OR
"D2"."SID_ZKBBOUNIT" = 7524 OR "D2"."SID_ZKBBOUNIT" = 7525 OR
"D2"."SID_ZKBBOUNIT" = 7526 OR "D2"."SID_ZKBBOUNIT" =7527 OR
"D2"."SID_ZKBBOUNIT" = 7528 OR "D2"."SID_ZKBBOUNIT"= 7529 OR
"D2"."SID_ZKBBOUNIT" = 7530 OR "D2"."SID_ZKBBOUNIT" = 7531 OR
"D2"."SID_ZKBBOUNIT" = 7532 OR "D2"."SID_ZKBBOUNIT" = 7533 OR
"D2"."SID_ZKBBOUNIT" = 7534 OR "D2"."SID_ZKBBOUNIT" = 7535 OR
"D2"."SID_ZKBBOUNIT" = 7536 OR "D2"."SID_ZKBBOUNIT" = 7537 OR
"D2"."SID_ZKBBOUNIT" =7520 OR "D2"."SID_ZKBBOUNIT" = 7503 OR
"D2"."SID_ZKBBOUNIT"= 7504 OR "D2"."SID_ZKBBOUNIT" = 7505 OR
"D2"."SID_ZKBBOUNIT" = 7506 OR "D2"."SID_ZKBBOUNIT" = 7507 OR
"D2"."SID_ZKBBOUNIT" = 7508 OR "D2"."SID_ZKBBOUNIT" = 7509 OR
"D2"."SID_ZKBBOUNIT" = 7510 OR "D2"."SID_ZKBBOUNIT" =7511 OR
"D2"."SID_ZKBBOUNIT" = 7512 OR "D2"."SID_ZKBBOUNIT"= 7513 OR
"D2"."SID_ZKBBOUNIT" = 7514 OR "D2"."SID_ZKBBOUNIT" = 7515 OR
"D2"."SID_ZKBBOUNIT" = 7516 OR "D2"."SID_ZKBBOUNIT" = 7517 OR
"D2"."SID_ZKBBOUNIT" = 7518 OR "D2"."SID_ZKBBOUNIT" = 7519 OR
"D2"."SID_ZKBBOUNIT" = 7467 OR "D2"."SID_ZKBBOUNIT" = 7418 OR
"D2"."SID_ZKBBOUNIT" = 7472 OR "D2"."SID_ZKBBOUNIT"= 7473 OR
"D2"."SID_ZKBBOUNIT" = 7474 OR "D2"."SID_ZKBBOUNIT" = 7475 OR
"D2"."SID_ZKBBOUNIT" = 7476 OR "D2"."SID_ZKBBOUNIT" = 7477 OR
"D2"."SID_ZKBBOUNIT" = 7478 OR "D2"."SID_ZKBBOUNIT" = 7479 OR
"D2"."SID_ZKBBOUNIT" = 7480 OR "D2"."SID_ZKBBOUNIT" = 7481 OR
"D2"."SID_ZKBBOUNIT" =7482 OR "D2"."SID_ZKBBOUNIT" = 7483 OR
"D2"."SID_ZKBBOUNIT"= 7484 OR "D2"."SID_ZKBBOUNIT" = 7521 OR
"D2"."SID_ZKBBOUNIT" = 7522 OR "D2"."SID_ZKBBOUNIT" = 7523 OR
"D2"."SID_ZKBBOUNIT" = 7524 OR "D2"."SID_ZKBBOUNIT" = 7525 OR
"D2"."SID_ZKBBOUNIT" = 7526 OR "D2"."SID_ZKBBOUNIT" =7527 OR
"D2"."SID_ZKBBOUNIT" = 7528 OR "D2"."SID_ZKBBOUNIT"= 7529 OR
"D2"."SID_ZKBBOUNIT" = 7530 OR "D2"."SID_ZKBBOUNIT" = 7531 OR
"D2"."SID_ZKBBOUNIT" = 7532 OR "D2"."SID_ZKBBOUNIT" = 7533 OR
"D2"."SID_ZKBBOUNIT" = 7534 OR "D2"."SID_ZKBBOUNIT" = 7535 OR
"D2"."SID_ZKBBOUNIT" = 7536 OR "D2"."SID_ZKBBOUNIT" = 7537 OR
"D2"."SID_ZKBBOUNIT" =7520 OR "D2"."SID_ZKBBOUNIT" = 7503 OR
"D2"."SID_ZKBBOUNIT"= 7504 OR "D2"."SID_ZKBBOUNIT" = 7505 OR
"D2"."SID_ZKBBOUNIT" = 7506 OR "D2"."SID_ZKBBOUNIT" = 7507 OR
"D2"."SID_ZKBBOUNIT" = 7508 OR "D2"."SID_ZKBBOUNIT" = 7509 OR
"D2"."SID_ZKBBOUNIT" = 7510 OR "D2"."SID_ZKBBOUNIT" =7511 OR
"D2"."SID_ZKBBOUNIT" = 7512 OR "D2"."SID_ZKBBOUNIT"= 7513 OR
"D2"."SID_ZKBBOUNIT" = 7514 OR "D2"."SID_ZKBBOUNIT" = 7515 OR
"D2"."SID_ZKBBOUNIT" = 7516 OR "D2"."SID_ZKBBOUNIT" = 7517 OR
"D2"."SID_ZKBBOUNIT" = 7518 OR "D2"."SID_ZKBBOUNIT" = 7519 OR
"D2"."SID_ZKBBOUNIT" = 7467 OR "D2"."SID_ZKBBOUNIT" = 7418 OR
"D2"."SID_ZKBBOUNIT" =7310 OR "D2"."SID_ZKBBOUNIT" = 7293 OR
"D2"."SID_ZKBBOUNIT"= 7294 OR "D2"."SID_ZKBBOUNIT" = 7295 OR
"D2"."SID_ZKBBOUNIT" = 7296 OR "D2"."SID_ZKBBOUNIT" = 7297 OR
"D2"."SID_ZKBBOUNIT" = 7298 OR "D2"."SID_ZKBBOUNIT" = 7299 OR
"D2"."SID_ZKBBOUNIT" = 7300 OR "D2"."SID_ZKBBOUNIT" =7301 OR
"D2"."SID_ZKBBOUNIT" = 7309 OR "D2"."SID_ZKBBOUNIT"= 7308 OR
"D2"."SID_ZKBBOUNIT" = 7307 OR "D2"."SID_ZKBBOUNIT" = 7306 OR
"D2"."SID_ZKBBOUNIT" = 7305 OR "D2"."SID_ZKBBOUNIT" = 7304 OR
"D2"."SID_ZKBBOUNIT" = 7309 OR "D2"."SID_ZKBBOUNIT"= 7308 OR
"D2"."SID_ZKBBOUNIT" = 7307 OR "D2"."SID_ZKBBOUNIT" = 7306 OR
"D2"."SID_ZKBBOUNIT" = 7305 OR "D2"."SID_ZKBBOUNIT" = 7304 OR
"D2"."SID_ZKBBOUNIT" = 7303 OR "D2"."SID_ZKBBOUNIT" = 7302) AND
"Z1"."UNIT" <> 2000008999
GROUP BY "S5"."CS_SITEM","S6"."CS_SITEM",
"S7"."CURRENCY","S8"."CURRENCY","DT"."SID_0FISCPER3","S9".
"FISCYEAR","S10"."UNIT","S11"."/BIC/ZKBBBPOS",
"S12"."/BIC/ZKBBMPROD","S13"."/BIC/ZKBBMSNME","S14"."/BIC/ZKBBOUNIT",
"S15"."/BIC/ZKBBOUNIT","S16"."/BIC/ZKBBPSVER","S17"."/BIC/ZKBBSPIND"
!
select * from "/BI0/03 00346233"  !
fetch!
rollback release !

CONNECT SUT IDENTIFIED BY SUT !
DROP USER KERN !
COMMIT WORK RELEASE !
dbmcli superdba,colduser !
param_directput _PACKET_SIZE 32768 !
param_directput _CAT_CACHE_MINSIZE 131072 !
param_directput CAT_CACHE_SUPPLY 96 !
param_startsession !
param_checkall !
param_commitsession !
db_restart !
bye !
senderid DBM !
file connect ( sut sut ! 
set parameter OPTIMIZE_OPERATOR_JOIN = '&1' !
commit release !
senderid XCI !

*****************************************************************************
* CONSTR.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create user bubu password baba resource !
commit work release!
file connect ( bubu baba !
create table a
 (a1 fixed  (10) key
                 constraint a1 > a2 and a2 > a3 or a1=1 or a3=a2,
  a2 fixed   (4) default 2
                 constraint a2 > a3 and (a2 = a1 or a1 > a3 and
                 (a1 <> 0 or a1 = 2)),
  a3 fixed   (4) default 1
                 constraint a3 = a1 or a3 is null or a3 = 1,
  a4 char  (200) constraint a4 = 'ein' & chr(a3, 1) & chr(a2, 1)
                 & chr(a1, 1) & 'aus' or a4 is null,
  a5 char  (200) check
                 a5 = substr(a4, 6, 4) and substr(a4, 4, 3) > '012'
                 or substr(a6, 1) > 'm' or a5 is null,
  a6 char  (200) default 'm'
                 range between 'e' and 'o' )!
insert a values (6, default, default, 'ein126aus', '6aus', default) !
select * from a!
fetch!
create table b
 (b1 int            key
                    constraint b1 > b2 and b2 > b3 or b1 = 1 or b3 = b2,
  b2 int            key
                    default 2
                    constraint b2 > b3 and (b2 = b1 or b1 > b3 and
                    (b1 <> 0 or b1 = 2)),
  b3 int            key
                    default 1
                    constraint b3 = b1 or b3 is null or b3 > 0,
  b31 varchar (254) default 'c',
  b4  varchar (254) ascii default 'c'
                    constraint b4 > 'a' or b4 is null,
  b5  varchar (254) check b5 = substr(b31, 2, 3) and
                    substr(b4, 2, 3) = 'abc' or
                    substr(b6, 2, 1) > 'a' or b5 is null,
  b6  varchar (254) default 'n'
                    range between 'f' and 'p' )!
insert b values (6, 3, 2, ' abc', 'xabcx', 'abc', default) !
alter table b add constraint b31 check b31 > 'd'!
alter table b foreign key l (b1) references a!
create view c (c1, c2, c3, c4, c5) as select a.a1, b.b2, b.b3, a.a4,
               a.a5 from a, b where a.a1 = b.b1 with check option!
select * from c !
fetch!
insert c values (8, 4, 2, 'ein128aus', '8aus')!
select * from c!
fetch!
drop table a !
drop table b !
commit work release !
file connect (kern test !
drop user bubu !
commit work release !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* CORR.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
* corr subquery mit or !
create table rel(bky fixed (6), name char (12))!
create table att(key_opt char (1), bky fixed (6), name char (12))!
create table sch(name char (16), bky fixed (6))!
insert rel values (3, 'mmmmm')!
insert rel values (4, 'aaaaa')!
insert att values ('Y', 4, 'attname')!
insert att values ('N', 3, 'attname')!
insert sch values ('test', 3)!
select fixed (rel.bky, 5) bk,
    rel.name nam,
    att.name att,
    att.key_opt p
from rel, att, sch
where sch.name = 'test'
  and rel.bky = sch.bky
  and ((rel.name not in
            (select name from rel where bky in
                  ( select bky from att
                    where key_opt = 'Y'))
        and att.bky = rel.bky )
       or ( not exists
           (select rel.bky
              from att, rel, att ref
             where rel.bky = att.bky)))
order by rel.bky !
fetch !
drop table att !
drop table rel !
drop table sch !
commit release !
file connect ( kern test !
* corr subquery mit joinview !
create table csd_domain
(
DM_BKY                   FLOAT        (  15 ) key,
DM_VERS              FIXED        (  10 ) key,
ENTY_VERS            FLOAT        (  15 ) key)!
create table csd_enty
(
enty_BKY                   FLOAT        (  15 ) key)!
create table csd_attribute
(
AT_BKY                     FLOAT        (  15 ) key,
AT_VERSION_NO              FIXED        (  10 ) key,
ENTY_VERS            FLOAT        (  15 ) key,
DM_BKY                     FLOAT        (  15 ) ) !
insert csd_domain values (   10, 1, 1 )!
insert csd_domain values (   11, 1, 1 )!
insert csd_domain values (   12, 1, 1 )!
insert csd_domain values (   13, 1, 1 )!
insert csd_domain values (   14, 1, 1 )!
insert csd_enty values (10)!
insert csd_enty values (11)!
insert csd_enty values (12)!
insert csd_enty values (13)!
insert csd_enty values (14)!
create view msv_domain as select * from csd_enty, csd_domain
where dm_bky = enty_bky !
insert csd_attribute values (   10, 3, 4,  10)!
insert csd_attribute values (   11, 3, 4,  11)!
insert csd_attribute values (   13, 3, 4,  13)!
insert csd_attribute values (   14, 3, 4,  14)!
insert csd_attribute values (   20, 3, 4,null)!
select * from csd_enty, csd_domain domn
where  not exists
       (select * from csd_attribute
        where csd_attribute.dm_bky = domn.dm_bky)
  and dm_bky = enty_bky !
fetch !
drop table csd_enty !
drop table csd_attribute !
drop table csd_domain !
commit release !
file connect ( kern test !
* Join mit OR und Correlated Subquery !
create table tab1 (a char (8), b char (8))!
create table tab2 (c char (8), d char (8))!
create table tab3 (e char (8), f char (8))!
insert tab1 set a = 'a', b = 'b1'!
insert tab1 set a = 'b', b = 'b2'!
insert tab2 set c = 'a', d = 'd1'!
insert tab2 set c = 'q', d = 'd2'!
insert tab3 set e = 'c', f = 'f1'!
insert tab3 set e = 'q', f = 'f2'!
SELECT 0, '???', TAB2.*, TAB3.*
FROM TAB2, TAB3
WHERE TAB2.C = TAB3.E OR
      NOT EXISTS
      ( SELECT *
        FROM TAB1
        WHERE TAB1.A = TAB2.C )!
fetch !
SELECT count(*)
FROM TAB2, TAB3
WHERE TAB2.C = TAB3.E OR
      NOT EXISTS
      ( SELECT *
        FROM TAB1
        WHERE TAB1.A = TAB2.C )!
fetch !
SELECT 0, '???', TAB2.*, TAB3.*
FROM TAB2, TAB3
WHERE TAB2.C = TAB3.E OR
      NOT TAB2.C =
      ( SELECT value(min(TAB1.A), 'k')
        FROM TAB1
        WHERE TAB1.A = TAB2.C )!
fetch !
SELECT count(*)
FROM TAB2, TAB3
WHERE TAB2.C = TAB3.E OR
      NOT TAB2.C =
      ( SELECT value(min(TAB1.A), 'k')
        FROM TAB1
        WHERE TAB1.A = TAB2.C )!
fetch !
SELECT count(*)
FROM TAB2, TAB3
WHERE
   (
   TAB2.C <> TAB3.E OR
   TAB2.C > ALL (SELECT TAB1.A FROM TAB1)
   )
   AND 
      NOT TAB2.C =
      ( SELECT value(min(TAB1.A), 'k')
        FROM TAB1
        WHERE TAB1.A = TAB2.C )!
fetch!
SELECT 0, '???', TAB2.*, TAB3.*
FROM TAB2, TAB3
WHERE
   (
   TAB2.C <> TAB3.E OR
   TAB2.C > ALL (SELECT TAB1.A FROM TAB1)
   )
   AND 
      NOT TAB2.C =
      ( SELECT value(min(TAB1.A), 'k')
        FROM TAB1
        WHERE TAB1.A = TAB2.C )!
fetch!
drop table tab1 !
drop table tab2 !
drop table tab3 !
commit release !
file connect ( kern test !
create table t1 (a1 char(05),
                 a2 char(10)) !
insert t1 values ('value',  'bbbbb')!
insert t1 values ('aaaa1',  'value')!
insert t1 values ('aaaa2',  'bbbbb')!
insert t1 values ('aaaa3',  'ccccc')!
create table t2 (b1 char(05),
                 b2 char(10)) !
insert t2 values ('value',  'bbbbb')!
insert t2 values ('aaaa1',  'value')!
insert t2 values ('aaaa2',  'bbbbb')!
create table t3 (c1 char(05),
                 c2 char(10)) !
insert t3 values ('value',  'bbbbb')!
insert t3 values ('aaaa1',  'value')!
insert t3 values ('aaaa2',  'bbbbb')!
update statistics kern.t1 !
update statistics kern.t2 !
update statistics kern.t3 !
commit work !
select count (*) from t1, t2 where
  (a1 = 'value'
  or 
    (a1 = 'aaaa2' and
     a1 = b1))
  and
  ((not exists (select c1 from t3
               where c1 = 'xxxxx' AND t2.b1 <> c1))
   or 
   (a2 = 'ccccc')) 
! 
fetch !
select * from t1, t2 where
  (a1 = 'value'
  or 
    (a1 = 'aaaa2' and
     a1 = b1))
  and
  ((not exists (select c1 from t3
               where c1 = 'xxxxx' AND t2.b1 <> c1))
   or 
   (a2 = 'ccccc')) 
! 
fetch !
drop table t1 !
drop table t2 !
drop table t3 !
commit release !
file connect ( kern test !
create domain dm_blgr   char(2) range in ('0', 'A', 'B', 'AB')!
create domain dm_c20    char(20) !
create domain dm_c7     char(7) !
create domain dm_c9     char(9) !
create domain dm_c9def  char(9) default '999999999'!
create domain dm_dat    date range between '19600401'
                                       and '20101231' !
create domain dm_dtb    date range between '18500101'
                                       and '20101231' !
create domain dm_d2     decimal(2) !
create domain dm_ins    char(3) range in ('NO', 'YES') !
create domain dm_inva_2 decimal(8) range between 24000 and 99999999 !
create domain dm_nob    decimal(2) range between 0 and 15 !
create domain dm_pnr    decimal(6) range between 100000 and 999999 !
create domain dm_rhf    char(1) range in ('+', '-') !
create domain dm_sex    char(1) range in ('M', 'F') !
create domain dm_stat   char(4) range in ('CURR', 'PLAN', 'FORM') !
create domain dm_torm   char(4) range in ('TRM', 'NRS', 'RCPT',
                                          'STR', 'VAR') !
*
create table pat (
  pnr    dm_pnr key,
  nm     dm_c20 not null,
  adr    dm_c20,
  zp     dm_c7,
  cty    dm_c20 not null,
  plb    dm_c20,
  dtb    dm_c20 not null,
  regd   dm_dat not null,
  blgr   dm_blgr,
  rhf    dm_rhf,
  sex    dm_sex) !
*
create table rm (
  rnr    dm_c9 key,
  torm   dm_torm not null,
  flsp   dm_d2 not null,
  status dm_stat not null,
  nob    dm_nob)!
*
create table trf (
  pnr    dm_pnr not null,
  tdat   dm_dat not null,
  rnr    dm_c9def,
  com    dm_c20)!
*
create table adm (
  pnr    dm_pnr not null,
  admd   dm_dat not null,
  rnr    dm_c9def,
  disd   dm_dat,
  dsch   dm_ins not null,
  dcsd   dm_ins not null,
  inva   dm_inva_2,
  aftc   dm_c20)!
*
commit work!
*
insert pat set pnr = 100000,     nm   = 'Werner',    cty = 'Kiel',
               dtb = '19800401', regd = date !
insert pat set pnr = 100001,     nm   = 'Holgi',     cty = 'Kiel',
               dtb = '19800401', regd = date !
insert pat set pnr = 100002,     nm   = 'Oelfuss',   cty = 'Kiel',
               dtb = '19800401', regd = date !
*
insert rm  set rnr = '100000000', torm='TRM', flsp = 0, status = 'CURR'!
insert rm  set rnr = '100000001', torm='TRM', flsp = 1, status = 'CURR'!
insert rm  set rnr = '100000002', torm='TRM', flsp = 2, status = 'CURR'!
*
insert trf set pnr = 100000, tdat = date !
insert trf set pnr = 100002, tdat = date !
*
insert adm set pnr  = 100000, admd = date,
               dsch = 'NO',   dcsd = 'NO' !
*
pars_then_ex!
data!
declare pseudo_cursor cursor for
select pat.nm, rm.rnr
  from adm a, pat, rm, trf
 where pat.pnr = a.pnr
   and a.dsch = :B
   and a.admd > ALL (
                select tdat
                  from trf
                 where trf.pnr = a.pnr)
   and rm.rnr = a.rnr !
c 3 'NO'

nodata!
pars_execute!
fetch pseudo_cursor!
drop table pat!
drop table rm !
drop table trf!
drop table adm!
drop domain dm_blgr !
drop domain dm_c20 !
drop domain dm_c7 !
drop domain dm_c9 !
drop domain dm_c9def !
drop domain dm_dat !
drop domain dm_dtb !
drop domain dm_d2 !
drop domain dm_ins !
drop domain dm_inva_2 !
drop domain dm_nob !
drop domain dm_pnr !
drop domain dm_rhf !
drop domain dm_sex !
drop domain dm_stat !
drop domain dm_torm !
commit work release !
file connect ( kern test !
create table t ( a fixed (8)key )!
insert t set a = 1 !
insert t set a = 2 !
insert t set a = 3 !
insert t set a = 5 !
insert t set a = 6 !
insert t set a = 7 !
insert t set a = 8 !
insert t set a = 9 !
insert t set a = 10!
create table t1 ( b fixed (8) key )!
insert t1 set b = 3 !
insert t1 set b = 5 !
insert t1 set b = 7 !
insert t1 set b = 9 !
select a+1 from t x where not exists (select * from t where
          t.a = x.a+1 and t.a > x.a  and a <= 9) and a <= 9 !
fetch !
select a+1 from t x where not exists
          (select a from t,t1
          where
               t.a = x.a+1 and t.a > x.a
           and a = b
           and a <= 9 and b <= 9)
           and a <= 9 !
fetch !
pars_then_ex !
data !
select a+1 from t x where not exists
          (select a from t,t1
          where
               t.a = x.a+1 and t.a > x.a
           and a = b
           and a <= :a and b <= :a)
           and a <= :a !
n 8 9 n 8 9 n 8 9

nodata !
fetch into :a !
pars_execute !
******** !
select a-1 from t x where not exists
          (select a from t,t1
          where
               t.a = x.a-1 and t.a <= x.a
           and a = b
           )
           and a <= 9 !
fetch !
pars_then_ex !
data !
select a-1 from t x where not exists
          (select a from <t,t1>
          where
               t.a = x.a-1 and t.a <= x.a
           and a = b
           )
           and a <= :a !
n 8 9

select a-1 from t x where not exists
          (select a from <t1,t>
          where
               t.a = x.a-1 and t.a <= x.a
           and a = b
           )
           and a <= :a !
n 8 9

nodata !
fetch into :a !
pars_execute !
******** !
select a+1 from t x where not exists
          (select a from t,t1
          where
               t.a = x.a
           and a = b
           and a <= 9 and b <= 9)
           and a <= 9 !
fetch !
pars_then_ex !
data !
select a+1 from t x where not exists
          (select a from t,t1
          where
               t.a = x.a
           and a = b
           and a <= :a and b <= :a)
           and a <= :a !
n 8 9 n 8 9 n 8 9

nodata !
fetch into :a !
pars_execute !
******** !
create table t2 ( a fixed (8) )!
create index "t2.a" on t2( a ) !
insert t2 set a = 3 !
insert t2 set a = 5 !
insert t2 set a = 7 !
insert t2 set a = 9 !
******** !
select a+1 from t x where not exists
          (select a from <t2,t1>
          where
               t2.a = x.a+1 and t2.a > x.a
           and a = b
           and a <= 9 and b <= 9)
           and a <= 9 !
fetch !
select a+1 from t x where not exists
          (select a from <t1,t2>
          where
               t2.a = x.a+1 and t2.a > x.a
           and a = b
           and a <= 9 and b <= 9)
           and a <= 9 !
fetch !
pars_then_ex !
data !
select a+1 from t x where not exists
          (select a from <t2,t1>
          where
               t2.a = x.a+1 and t2.a > x.a
           and a = b
           and a <= :a and b <= :a)
           and a <= :a !
n 8 9 n 8 9 n 8 9

nodata !
fetch into :a !
data!
select a+1 from t x where not exists
          (select a from <t1,t2>
          where
               t2.a = x.a+1 and t2.a > x.a
           and a = b
           and a <= :a and b <= :a)
           and a <= :a !
n 8 9 n 8 9 n 8 9

nodata !
fetch into :a !
pars_execute !
******** !
select a-1 from t x where not exists
          (select a from <t2,t1>
          where
               t2.a = x.a-1 and t2.a <= x.a
           and a = b
           )
           and a <= 9 !
fetch!
select a-1 from t x where not exists
          (select a from <t1,t2>
          where
               t2.a = x.a-1 and t2.a <= x.a
           and a = b
           )
           and a <= 9 !
fetch !
pars_then_ex !
data !
select a-1 from t x where not exists
          (select a from <t2,t1>
          where
               t2.a = x.a-1 and t2.a <= x.a
           and a = b
           )
           and a <= :a !
n 8 9

nodata !
fetch into :a !
data!
select a-1 from t x where not exists
          (select a from <t1,t2>
          where
               t2.a = x.a-1 and t2.a <= x.a
           and a = b
           )
           and a <= :a !
n 8 9

nodata !
fetch into :a !
pars_execute !
******** !
select a+1 from t x where not exists
          (select a from t2,t1
          where
               t2.a = x.a
           and a = b
           and a <= 9 and b <= 9)
           and a <= 9 !
fetch !
pars_then_ex !
data !
select a+1 from t x where not exists
          (select a from t2,t1
          where
               t2.a = x.a
           and a = b
           and a <= :a and b <= :a)
           and a <= :a !
n 8 9 n 8 9 n 8 9

nodata !
fetch into :a !
pars_execute !
drop table t !
drop table t1 !
drop table t2 !
commit release !


file connect ( kern test !
create table t ( a char (8)) !
insert t set a = 'aaa'!
insert t set a = 'xxx'!
create table t1 ( b char (8))!
insert t1 set b = 'aaa'!
insert t1 set b = 'xxx'!
create table t3 (c char (8))!
insert t3 set c = 'bbb'!
insert t3 set c = 'ccc'!
select * from t,t1
  where rowno <= 3!
select * from t,t1
  where not exists (select * from t3)!
fetch!
select * from t, t1
  where not exists (select * from t3 where c = t.a )!
fetch!
insert t3 set c = 'aaa'!
insert t3 set c = 'aaa'!
insert t3 set c = 'bbb'!
select * from t, t1
     where exists (select distinct * from t3
                             where c = t.a) !
fetch!
select * from t, t1
  where not exists 
         (select distinct * from t3 x where
             exists (select * from t3 where c = t.a
                             and c = x.c ) )!
fetch!
select * from t, t1
  where not exists 
         (select distinct c from t3 x where
             exists (select * from t3 where c = t.a
                             and c = x.c ) )!
fetch!
create index "t3.c" on t3( c ) ! 
select * from t, t1
  where not exists 
         (select distinct c from t3 x where
             exists (select * from t3 where c = t.a
                             and c = x.c ) )!
fetch!
select * from t, t1
  where not exists 
         (select distinct * from t3 x where
             exists (select * from t3 where c = t.a
                             and c = x.c ) )!
fetch!
create view v as select distinct * from t3 !
pars_then_ex !
select * from t, t1
  where not exists 
         (select * from v x where
             exists (select * from t3 where c = t.a
                             and c = x.c ) )!
fetch!
select * from t, t1
  where not exists 
         (select * from v x where
             exists (select * from t3 where c = 'bbb') )!
fetch!
select * from t, t1
  where not exists 
         (select * from v x where x.c = t.a and
             exists (select * from t3 where c = 'bbb') )!
fetch!
select * from t, t1
  where not exists 
         (select * from t3 x where x.c = 'bbb' and
             exists (select * from v where v.c = 'bbb') )!
fetch!
select * from t, t1
  where not exists 
         (select c from v x where
             exists (select * from t3 where c = t.a
                             and c = x.c ) )!
fetch!
create view v1 as select distinct c from t3 !
select * from t, t1
  where not exists 
         (select * from v1 x where
             exists (select * from t3 where c = t.a
                             and c = x.c ) )!
fetch!
select * from t, t1
  where not exists 
         (select c from v1 x where
             exists (select * from t3 where c = t.a
                             and c = x.c ) )!
fetch!
pars_execute!
rollback release !
file connect ( kern test !
create table t ( a date, b char (8))!
insert t values ('18940512', 'aaaa')!
insert t values ('18940131', 'aaaa')!
insert t values ('18940512', 'bbbb')!
insert t values ('18940131', 'bbbb')!
create table t1 ( c char (8))!
insert t1 set c = 'ccc'!
set format eur !
select * from t where a = (select max (a) from t,t1 )!
fetch!
set format internal !
rollback release !
file connect ( kern test !
CREATE TABLE KOM
("SP1               " FIXED (4,0)        ,
 "SP2               " CHAR (1) ASCII  ,
 "GREIF_KNZ         " CHAR (1) ASCII  ) !
insert kom values ( 100 , '1' , '1') !
insert kom values ( 200 , '2' , '2') !
insert kom values ( 300 , '3' , '1') !
CREATE TABLE KOP
("TEILE_NR          " FIXED (4,0)        ,
 "GREIF_KNZ         " CHAR (1) ASCII  ) !
insert kop values (     1 , '1' )!
insert kop values (     1 , '1' )!
insert kop values (     1 , '1' )!
insert kop values (     2 , '1' )!
insert kop values (     2 , '1' )!
insert kop values (     2 , '2' )!
insert kop values (     2 , '2' )!
select  km.greif_knz, teile_nr
from kop kp, kom km
where kp.greif_knz = '1' !
fetch !
select  km.greif_knz
from kop kp, kom km
where kp.greif_knz = '1'
and teile_nr in
  (select teile_nr
          from kop kp
          where kp.greif_knz = km.greif_knz
          group by teile_nr
          having count (kp.teile_nr) >= 2) !
fetch !
pars_then_ex !
data !
select  km.greif_knz
from kop kp, kom km
where kp.greif_knz = '1'
and teile_nr in
  (select teile_nr
          from kop kp
          where kp.greif_knz = km.greif_knz
          group by teile_nr
          having count (kp.teile_nr) >= :tn) !
n 38 2

nodata !
fetch into :a !
pars_execute !
rollback release !

file CONNECT ( kern test!
CREATE TABLE a (a1 CHAR (2))!
CREATE TABLE b (b1 CHAR (2))!
CREATE TABLE c (c1 CHAR (2))!
CREATE TABLE d (d1 CHAR (2))!
CREATE TABLE e (e1 CHAR (2))!
CREATE TABLE f (f1 CHAR (2))!
CREATE TABLE g (g1 CHAR (2))!
CREATE TABLE h (h1 CHAR (2))!
CREATE TABLE i (i1 CHAR (2))!
CREATE TABLE j (j1 CHAR (2))!
CREATE TABLE k (k1 CHAR (2))!
CREATE TABLE l (l1 CHAR (2))!

INSERT INTO a VALUES ('ab')!
INSERT INTO b VALUES ('ab')!
INSERT INTO c VALUES ('ab')!
INSERT INTO d VALUES ('ab')!
INSERT INTO e VALUES ('ab')!
INSERT INTO f VALUES ('ab')!
INSERT INTO g VALUES ('ab')!
INSERT INTO h VALUES ('ab')!
INSERT INTO i VALUES ('ab')!
INSERT INTO j VALUES ('ab')!
INSERT INTO k VALUES ('ab')!
INSERT INTO l VALUES ('ab')!

* the "old-style" of a corelation column with tablename still works!
SELECT a1 FROM a, b WHERE a1=b1 AND a1 IN
  (SELECT c1 FROM c, d WHERE c1=d1 AND d1=b.b1 AND c1 IN
    (SELECT e1 FROM e WHERE e1=a.a1)) AND b1 IN
  (SELECT f1 FROM f WHERE b.b1=f1 AND f1 IN
    (SELECT g1 FROM g, h WHERE g1=f.f1 AND h1 IN
      (SELECT i1 FROM i, j, k WHERE i1=b.b1 AND i1=j1 AND j1=k1) AND g1 IN
      (SELECT l1 FROM l WHERE l1=h.h1)))!
FETCH!

* and now the "new-style" without tablename prefix!
SELECT a1 FROM a, b WHERE a1=b1 AND a1 IN
  (SELECT c1 FROM c, d WHERE c1=d1 AND d1=b1 AND c1 IN
    (SELECT e1 FROM e WHERE e1=a1)) AND b1 IN
  (SELECT f1 FROM f WHERE b1=f1 AND f1 IN
    (SELECT g1 FROM g, h WHERE g1=f1 AND h1 IN
      (SELECT i1 FROM i, j, k WHERE i1=b1 AND i1=j1 AND j1=k1) AND g1 IN
      (SELECT l1 FROM l WHERE l1=h1)))!
FETCH!

DROP TABLE a!
DROP TABLE b!
DROP TABLE c!
DROP TABLE d!
DROP TABLE e!
DROP TABLE f!
DROP TABLE g!
DROP TABLE h!
DROP TABLE i!
DROP TABLE j!
DROP TABLE k!
DROP TABLE l!
COMMIT WORK RELEASE!

file connect ( kern test!
create table rel(rky fixed (6), name char (12))!
create table att(key_opt char (1), aky fixed (6), name char (12))!
create table sch(name char (16), sky fixed (6))!
insert rel values (3, 'mmmmm')!
insert rel values (4, 'aaaaa')!
insert att values ('Y', 4, 'attname')!
insert att values ('N', 3, 'attname')!
insert sch values ('test', 3)!

select fixed (rky, 5) bk,
    rel.name nam,
    att.name att,
    att.key_opt p
from rel, att, sch
where sch.name = 'test'
  and rky = sky
  and ((rel.name not in
            (select name from rel where rky in
                  ( select aky from att
                    where key_opt = 'Y'))
        and aky = rky )
       or ( not exists
           (select rky
              from att, rel, att ref
             where rky = att.aky)))
order by rel.rky !
fetch !
drop table att !
drop table rel !
drop table sch !
commit release !

file connect ( kern test !
create table tab1 (a char (8), b char (8))!
create table tab2 (c char (8), d char (8))!
create table tab3 (e char (8), f char (8))!
insert tab1 set a = 'a', b = 'b1'!
insert tab1 set a = 'b', b = 'b2'!
insert tab2 set c = 'a', d = 'd1'!
insert tab2 set c = 'q', d = 'd2'!
insert tab3 set e = 'c', f = 'f1'!
insert tab3 set e = 'q', f = 'f2'!
SELECT 0, '???', TAB2.*, TAB3.*
FROM TAB2, TAB3
WHERE C = E OR
      NOT EXISTS ( SELECT * FROM TAB1 WHERE A = C )!
fetch !
SELECT count(*)
FROM TAB2, TAB3
WHERE C = E OR
      NOT EXISTS
      ( SELECT * FROM TAB1 WHERE A = C )!
fetch !
SELECT 0, '???', TAB2.*, TAB3.*
FROM TAB2, TAB3
WHERE C = E OR
      NOT C =
      ( SELECT value(min(A), 'k') FROM TAB1 WHERE A = C )!
fetch !
SELECT count(*)
FROM TAB2, TAB3
WHERE C = E OR
      NOT C =
      ( SELECT value(min(A), 'k') FROM TAB1 WHERE A = C )!
fetch !
SELECT count(*)
FROM TAB2, TAB3
WHERE
   (
   C <> E OR
   C > ALL (SELECT A FROM TAB1)
   )
   AND 
      NOT C =
      ( SELECT value(min(A), 'k') FROM TAB1 WHERE A = C )!
fetch!
SELECT 0, '???', TAB2.*, TAB3.*
FROM TAB2, TAB3
WHERE
   (
   C <> E OR
   C > ALL (SELECT A FROM TAB1)
   )
   AND 
      NOT C =
      ( SELECT value(min(A), 'k')
        FROM TAB1
        WHERE A = C )!
fetch!
drop table tab1 !
drop table tab2 !
drop table tab3 !
commit release !

file connect ( kern test !
CREATE TABLE CH (
     START_LPNO       FIXED(10),
     STOP_LPNO        FIXED(10)
     )!
insert into ch values (123456, 123460)!
insert into ch values (234567, 234890)!
CREATE TABLE CR (
     CMD_ID          CHAR(15),
     REP_DATE        CHAR(5)  ASCII,
     REP_TIME        CHAR(8),
     LINE            CHAR(80) ASCII
     )!
insert cr values ('abcdefghij','12-12','17:30:00',
  'abcdefghijklmnopqrstuv 123456 zyxw')!
insert cr values ('abcdefghij','12-11','18:30:00',
  'abcdefghIJKLMNOPQRSTUV 123456 zyxw')!
insert cr values ('mnopqrstuv','12-10','19:30:00',
  'abcdefghIJKLMNOPQRSTUV 234567 zyxw')!
select rep_date, rep_time, line
from ch h, cr r
where r.cmd_id   = any (select cmd_id
                  from cr
                  where index(line,rtrim(chr(h.start_lpno)),20) > 0) !
fetch !
commit release !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
