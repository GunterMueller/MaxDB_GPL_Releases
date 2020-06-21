file connect ( kern test !
create table l ( k char (8) key,
a char (9),
s &1,
b char (10))!
file sel_cols_tab ( L !
parsing !
comment on table l is :aaa !
executing!
data!
insert !
1
l 36 'hier steht der String drin '  

 *insert mit kurzer 'Long'-Column!
parsing !
insert l values (:k, :a, :s, :b)!
executing!
data!
insert !
1
c 8 'keyvalue'
c 9 'a9a9a9a9a'
l 100 'hier steht der String drin '  
c 10 'b10b10b10b'

 *insert mit langer 'Long'-Column !
parsing !
nodata !
insert l set k = :k, s = :s, a = :a, b = :b !
data!
executing!
insert !
1
c 8 'second_k'
l 8200 'hier steht der String drin '  
c 9 'a9a9a9a9a'
c 10 'b10b10b10b'

 *insert mit Long-Column, die durch putvals aufgefuellt wird !
insert !
1
c 8 'third_k'
l 54,0 'hier steht der String drin '  
c 9 'a9a9a9a9a'
c 10 'b10b10b10b'

putval !
1
l 120,0 'Nachschlag 1'

putval !
1
l 100,2 'Rest'
 
putval !
1
l 1,5 'R'
 
executing!
 * insert mit NULL-Wert als Dateneingabe !
insert !
1
c 8 'fourth_k'
c 32
c 9 'aaaaaaaaa'
c 10 'b10b10b10b'

 * abgebrochenes putval PTS 1113250 !
insert !
1
c 8 'fifth_k'
l 54,0 'hier steht der String drin '  
c 9 'a9a9a9a9a'
c 10 'b10b10b10b'

putval !
1
l 120,0 'Nachschlag 1'

putval !
1
l 100,8 'Rest'
 
 *Kontrollesen der Saetze !
pars_execute!
nodata !
select * from l !
fetch !
 *Append von kurzem Wert in kurzen, längeren Wert !
parsing !
append l set s = :s key k = 'keyvalue' !
executing !
data !
update !
1
l 50 'appended  '

pars_execute!
nodata !
select * from l !
fetch !
 *Append von kurzem Wert in auch noch kurzen Wert
 *mit Long-Column, die durch putvals aufgefuellt wird ! 
parsing !
append l set s = :s key k = 'keyvalue' !
executing !
data !
update !
1
l 34,0 'das ist der 1. Teil '  

putval !
1
l 80,2 'Rest vom putval '
 
putval !
1
l 1,5 'R'
 
pars_execute!
nodata !
select * from l !
fetch !
 *Append von kurzem Wert in langen Wert !
parsing !
append l set s = :s key k = 'keyvalue' !
executing !
data !
update !
1
l 9000 'looong appended '

pars_execute!
nodata !
select * from l !
fetch !
 *Append von langem Wert in noch längeren Wert !
parsing !
append l set s = :s key k = 'keyvalue' !
executing !
data !
update !
1
l 900 'das ist der Rest '

pars_execute!
nodata !
select * from l !
fetch !
 *Update von bestehendem Wert in explizit angegebenes NULL ! 
parsing !
update l set s = NULL key k = 'keyvalue' !
executing !
update !
1
 *Testlesen !
pars_execute!
nodata !
select * from l !
fetch!
 *Update von bestehendem Wert in NULL-Wert durch Dateneingabe !
parsing !
update l set s = :s key k = 'second_k' !
executing !
data!
update !
1
c 32

 *Testlesen !
pars_execute!
nodata !
select * from l !
fetch!
 *Vorablesen des bestehenden Wertes !
select * from l where k = 'third_k' !
fetch !
 *Update eines bestehenden Wertes durch einen neuen Wert !
 *Angabe nicht durch KEY ..., sondern durch WHERE !
nodata!
parsing!
update l set s = :s where k = 'third_k' !
executing !
data!
update !
1
l 300 'neuer Text !!!'

 *Kontrollesen !
pars_execute!
nodata !
select * from l !
fetch!
 *Update eines NULL-Wertes durch neuen Wert, der durch putvals
 *aufgef?ullt wird !
parsing !
update l set s = :s where a = 'aaaaaaaaa' !
executing !
data!
update !
1
l 1,3 'v'

putval !
1
l 30,0 'neuer Text !!!'

putval !
1
l 120,0 'Nachschlag 1'

putval !
1
l 100,2 'Rest'

 *Testlesen !
pars_execute!
nodata !
select * from l !
fetch!
commit !
 *mehrere Saetze duerfen doch gleichzeitig geaendert werden !
parsing!
update l set s = :s where k <= 'st' and a = 'a9a9a9a9a'!
executing !
data!
update !
1
l 300 'neuer Text fuer mehrere Saetze !'

nodata!
 *Testlesen !
pars_execute!
nodata !
select * from l !
fetch!
rollback !
parsing!
update l (s) values (:s) !
executing !
data!
update !
1
l 300 'noch neuerer Text fuer mehrere Saetze !'

 *Testlesen !
pars_execute!
nodata !
select * from l !
fetch!
rollback !
 * test with correlation and more than one record !
 create table l1 as select k, a, b from l!
parsing !
update l set s = :s where a = 'a9a9a9a9a'
and k in (select k from l1 where l1.k = l.k) !
 *Testlesen !
pars_execute!
nodata !
select * from l !
fetch!
rollback !
 * test with array command and more than one record !
parsing !
mupdate l set s = :s where k between :k1 and :k2 !
executing !
data !
mupdate    t!
32767
-2
1
l 240 'die ersten zwei keys kriegen den long-value' 
c 8 'fourth_k' c 8 'keyvalue'
l 300 'die letzten zwei keys kriegen einen anderen long-value' 
c 8 'second_k' c 8 'third_k'

 *Testlesen !
pars_execute!
nodata !
select * from l !
fetch!
rollback !
select k, a, b from l where s is NULL !
select k, a, b from l where s is not NULL !
select k, a, b from l where s = 'neuer Text'!
pars_then_ex !
select k, a, b from l where s is NULL !
fetch !
select k, a, b from l where s is not NULL !
fetch !
select k, a, b from l where s = 'neuer Text'!
select k, a, b from l where s like 'neuer Text%'!
select k, a, b from l where default < s !
select k, a, b from l where s = default !
commit !
select * from l !
fetch!
delete l key k = 'second_k' !
select * from l !
fetch!
rollback !
create table t ( a char ( 8) key, b &1, b1 &1, c char(32)) !
parsing !
minsert t values (:a, :b, :b1, :c) !
executing !
data !
minsert    t!
32767
2
1
c 8 'aaa' l 1000 'a' l 10 'b1eins    ' c 32 'eins'
c 8 'bbb' l 500  'a' l 10,0 'b1zweib1  ' c 32 'zwei'
 
putval !
1
l 1000,1 'erster Teil für zwei '
 
putval !
1
l 1,5 'R'
 
nodata !
pars_execute!
select * from t !
fetch !
check table t with long check !
rollback !
pars_execute!
 *Geht das auch mit mehreren Long-Columns in einer Tabelle ? !
create table t2 (
   k  char (8) key,
   s1 &1 ,
   a  char (2),
   s2 &1 byte)!
file sel_cols_tab ( T2 ! 
 * in case putval is done separately for both columns, the second one
 * uses the description of the first one --> in case of unicode-defcode
 * s2-input will not be handled as byte, but as unicode --> 
 * 'l' x'00' '2' x'00' !
parsing !
insert t2 values (:k, :s1, :a, :s2)!
executing !
data !
insert !
1
c 8 'keyvalue'
l 100 'tri tra tralala...'
c 2 'a9'
l 100 "hier steht der String drin "  

executing!
insert !
1
c 8 'second_k'
l 2500 'tritratralala...'
c 2 'a9'
c 32

insert !
1
c 8 'third_k'
l 1,3 't'
c 2 'a9'
l 1,3 "m"

putval !
1
l 120 'l1 l1 l1 '
l 1,3 "m"

putval !
1
l 420 "l2 l2 l2 "

putval !
1
l 1,5 "c"

insert !
1
c 8 'fourth_k'
l 100,0 't'
c 2 'a9'
l 1,3 "m"

putval !
1
l 120,2 'l1 l1 l1 '
l 1,3 "m"

putval !
1
l 420 "l2 l2 l2 "

putval !
1
l 1,5 "c"

pars_execute!
nodata !
select * from t2 !
fetch!
delete t2 !
diagnose share parse drop !
select * from t2 !
fetch!
parsing !
create view vlong as select * from l !
parsing !
select rl.k, rl.a, rl.s, rl.b
        into :k1, :a1, :s1, :b1,
             :k0, :a0, :s0, :b0
        from l, l rl
        where l.k = 'second_k' AND rl.k = 'second_k'!

select *
        into :k1, :a1, :s1, :b1,
             :k0, :a0, :s0, :b0
        from l, l rl
        where l.k = 'second_k' AND rl.k = 'second_k'!
select distinct *
        into :k1, :a1, :s1, :b1
        from l
        where k = 'second_k'!
select k, a, s, b 
        from l
        where k = 'second_k'
union
select k, a, s, b
        from l
        where k = 'keyvalue'!
select * from (select k, a, s, b
        from l
        where k = 'keyvalue') !
pars_then_ex !
select * from l !
fetch into :a, :b, :c, :d!
declare erg cursor for 
select * from l for reuse !
drop table l !
diagnose share parse drop !
fetch erg into :a, :b, :c, :d !
pars_execute!
nodata !
drop table t2 !
commit work release !
file connect ( kern test !
create table x (k char (8) key)!
create table t ( a char (8),
    foreign key (a) references x (k))!
pars_then_ex !
data !
insert t set a = :a!
c 8 'fff'

pars_execute !
nodata !
create table t1 (a char (8), 
                 l &1,
    foreign key (a) references x (k))!
pars_then_ex !
data !
insert t1 set a = :a, l = :l!
c 8 'fff'
l 7800,0 'Text '

pars_execute !
nodata !
rollback release !
file connect ( kern test !
create table t ( a char (8), s &1, b char (8))!
insert t set a = 'firstval', b = 'bbbb'!
parsing !
insert t values (:a, :s, :b)!
executing!
data!
insert !
1
c 8 'keyvalue'
l 100 'hier steht der String drin '  
c 8 'b10b10b1'

pars_execute !
nodata !
select * from t !
fetch !
mfetch first !
7
select a, s, b from t for reuse!
fetch!
mfetch first !
7
pars_then_ex !
pars_execute !
create view v1 as select * from t !
create table t1 as select * from t !
create table t2 as select * from v1 !
select * from t!
fetch!
select * from t1!
fetch!
select * from v1!
fetch!
select * from t2!
fetch!
    CREATE TABLE SYSDDMODULE
      (
       OWNER       CHAR(64) ASCII DEFAULT USERGROUP,
       MODULNAM1   CHAR(64) ASCII DEFAULT ' ',
       MODULNAM2   CHAR(64) ASCII DEFAULT ' ',
       PROGLANG    CHAR(64) ASCII DEFAULT ' ',
       OBJTYPE     CHAR(18) ASCII DEFAULT 'MODULE',
       SUBTYPE     CHAR(12) ASCII DEFAULT ' ',
       VERSION     CHAR(4)  ASCII DEFAULT ' ',
       LONGNAME    CHAR(40) ASCII DEFAULT ' ',
       TYPE        CHAR(18) ASCII DEFAULT ' ',
       CREATEDATE  DATE DEFAULT DATE,
       CREATETIME  TIME DEFAULT TIME,
       UPDDATE     DATE DEFAULT DATE,
       UPDTIME     TIME DEFAULT TIME,
       COMMENT     &1     ASCII
      ) !
    CREATE TABLE SYSDDUSR_USES_PRO
      (
       USER_ID     FIXED(10)       KEY,
       ROW_ID      CHAR(8)   BYTE  KEY,
       DEFOWNER    CHAR(64)  ASCII,
       DEFOBJN1    CHAR(64)  ASCII DEFAULT ' ',
       DEFOBJN2    CHAR(64)  ASCII DEFAULT ' ',
       DEFOBJN3    CHAR(64)  ASCII DEFAULT ' ',
       DEFOBJTYPE  CHAR(18)  ASCII DEFAULT 'USER',
       DEFSUBTYPE  CHAR(12)  ASCII DEFAULT ' ',
       DEFVERSION  CHAR(4)   ASCII DEFAULT ' ',
       RELTYPE     CHAR(18)  ASCII DEFAULT 'USES',
       REFOWNER    CHAR(64)  ASCII,
       REFOBJN1    CHAR(64)  ASCII DEFAULT ' ',
       REFOBJN2    CHAR(64)  ASCII DEFAULT ' ',
       REFOBJN3    CHAR(64)  ASCII DEFAULT ' ',
       REFOBJTYPE  CHAR(18)  ASCII DEFAULT 'PROGRAM',
       REFSUBTYPE  CHAR(12)  ASCII DEFAULT ' ',
       REFVERSION  CHAR(4)   ASCII DEFAULT ' ',
       PRIVILEGES  CHAR(40)  ASCII DEFAULT 'EXECUTE',
       CREATEDATE  DATE DEFAULT DATE,
       CREATETIME  TIME DEFAULT TIME
      ) !
    GRANT SELECT ON SYSDDUSR_USES_PRO TO PUBLIC !
CREATE VIEW SYSDD_USR_USES_PRO
  (DEFOBJTYPE,DEFOWNER,DEFUSERNAME,DEFGROUPNAME,RELTYPE,
   REFOBJTYPE,REFOWNER,REFPROGRAMNAME,REFPROGLANG,CREATEDATE,CREATETIME)
AS SELECT
     DEFOBJTYPE,DEFOWNER,DEFOBJN1,DEFOBJN2,RELTYPE,
     REFOBJTYPE,REFOWNER,REFOBJN1,REFOBJN3,CREATEDATE,CREATETIME
   FROM SYSDDUSR_USES_PRO
   WHERE defOBJN1 IN(USERGROUP,'*PUBLIC*') OR defOBJN2 = USERGROUP
WITH CHECK OPTION !
insert sysddmodule set
       OWNER       = user,
       MODULNAM1 = 'module',
       MODULNAM2 = 'noch ein module',
       PROGLANG    = 'programm',
       OBJTYPE     = 'objekt',
       SUBTYPE     = 'subtype',
       VERSION     = '1.',
       LONGNAME    = 'recht lang',
       TYPE        = 'typ',
       CREATEDATE  = DATE,
       CREATETIME  = TIME,
       UPDDATE     = DATE,
       UPDTIME     = TIME!
parsing !
update sysddmodule set comment = :a where owner = user !
executing !
data !
update !
1
l 500 'der Kommentar: '

pars_execute!
nodata !
insert SYSDDUSR_USES_PRO values
(1, x'0000000000000001', usergroup, usergroup, usergroup, usergroup,
'objekt', 'subtype', '1.', 'needs',
usergroup, usergroup, usergroup, usergroup,
'objekt 2', 'subtype 2', '2.', 'keine', date, time) !
GRANT ALL ON SYSDD_USR_USES_PRO TO PUBLIC !
SELECT
  T1.OWNER,T1.MODULNAM1,T1.MODULNAM2,T1.PROGLANG,
  T1.CREATEDATE,T1.CREATETIME,T1.COMMENT
FROM SYSDDMODULE T1, SYSDD_USR_USES_PRO T2
WHERE T1.OWNER = T2.REFOWNER OR T1.OWNER = USERGROUP !
fetch !
rollback !
create table t ( a char (8) key, b &1, c fixed (8))!
parsing !
insert t values ('aaa', :b, 4 * 3)!
executing !
data !
insert t values ('aaa', :b, 4 * 3)!
1
l 400 'xyz '

pars_execute!
nodata!
select * from t !
fetch!
create table t1 ( a char (8), c fixed (8))!
insert t1 values ('aaa', 25)!
declare invisible cursor for 
select description,
       value (chr(numeric_value), char_value) "VALUE"
  from sysdba.tablestatistics
 where owner = 'KERN' 
   and tablename = 'T' !
declare erg cursor for 
select count(*) cnt from t where b is not null!
select cnt, value from invisible, erg
where description = 'DEFINED LONG COLUMNS'
  and num(value) <> cnt !
fetch !
close invisible !
insert t(a,c) select a, c from t1 !
insert t(a,c) select a, c from t1 update duplicates !
declare invisible cursor for 
select description,
       value (chr(numeric_value), char_value) "VALUE"
  from sysdba.tablestatistics
 where owner = 'KERN' 
   and tablename = 'T' !
declare erg cursor for 
select count(*) cnt from t where b is not null!
* has to result in 'row not found'!
select cnt, value from invisible, erg
where description = 'DEFINED LONG COLUMNS'
  and num(value) <> cnt !
fetch !
close invisible !
rollback release !
* Jetzt wird noch ein wenig DDL mit Longs getestet!
FILE connect (kern test!
CREATE TABLE ta (a CHAR)!
ALTER  TABLE ta ADD  l &1!
ALTER  TABLE ta DROP l!
DROP   TABLE ta!
CREATE TABLE ta (a CHAR, l &1)!
ALTER  TABLE ta ADD  l2 &1!
ALTER  TABLE ta DROP l2!
DROP   TABLE ta!
CREATE TABLE ta (a CHAR, l &1, l2 &1)!
ALTER  TABLE ta ADD  l2 &1!
ALTER  TABLE ta DROP l2!
DROP   TABLE ta!
ROLLBACK WORK RELEASE!
FILE connect (kern test SQLMODE ORACLE!
CREATE TABLE ta (a CHAR)!
ALTER  TABLE ta ADD  l &1!
ALTER  TABLE ta DROP (l)!
DROP   TABLE ta!
CREATE TABLE ta (a CHAR, l &1)!
ALTER  TABLE ta ADD  l2 &1!
ALTER  TABLE ta DROP (l2)!
DROP   TABLE ta!
CREATE TABLE ta (a CHAR, l &1, l2 &1)!
DROP   TABLE ta !
COMMIT WORK RELEASE!
* Tests mit LONG Literalen!
FILE connect (kern test!

CREATE TABLE LT1 (
k1 char key , 
k2 char key , 
L11 &1 byte default x'31323334', 
L12 &1 ) !

CREATE TABLE LT2 (
L21 &1 byte default x'35363738', 
L22 &1 ascii ) !

CREATE VIEW LV1 AS SELECT * FROM LT1 !
CREATE VIEW LV2 AS SELECT * FROM LT2 !

CREATE VIEW LV12 AS SELECT * FROM LT1, lT2 !

file sel_cols_tab ( LV12 !

INSERT lt1 set k1 = 'a', k2 = 'z', l11 = DEFAULT, l12 = '012' !
INSERT lt1 set k1 = 'z', k2 = 'a', l12 = '012' !

INSERT lt2 set l21 = DEFAULT, l22 = x'333435' !
INSERT lt2 set l22 = x'333435' !

SELECT * FROM LV12 !
FETCH !

insert lt1 set k1 = 'b', k2 = 'y',
l11 =
'Dies sind 1400 Zeichen insgesamt (viele As und ein X): aaaaaaaaaaaaaaa'
'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaax'
, l12 = 'hallo' !

 *Kontrollesen der Saetze !
pars_execute!
nodata !
select * from lt1 !
fetch !
ROLLBACK WORK RELEASE!
file connect ( kern test !
* LONG with Default was funny by 1995-02-01 !
create table t ( a &1 default 'das ist der Long-Default-Wert',
b char (8) primary key )!
insert t set b = 'erstens' !
select * from t !
fetch !
insert t values (default, 'zweitens') !
select * from t !
fetch !
select * from t where a = default !
fetch!
file sel_cols_tab ( T !
rollback release !
* PTS 1000873 !
file connect ( kern test !
create table t ( 
a char (8) key, 
b char (5) key, 
c char (3) key,
d char (12) key,
e fixed (2) key,
f char (20),
g char (30),
h &1 )!
create view v (a,b,c,d,e) as select a,b,c,d,e from t !
delete v where a = 'a' and b = 'b' and c = 'c' !
rollback release !
* PTS 1000987 !
file connect ( kern test !
create table t ( a char (8), b &1, c &1)!
pars_then_ex !
data !
insert t set a = 'abc', 
b = 'das soll der long sein', c = 'das ist der neuere Long'!

insert t set a = '2a2b2c', b = :b, 
 c = 'das ist der neuere Long'!
 l 5000 'das soll der long sein '

nodata !
pars_execute!
select * from t !
fetch!
declare erg cursor for 
select * from t for reuse !
drop table t !
diagnose share parse drop !
fetch erg !
commit !
 * just to spent some time to let garbage collector clean long-file !
 * > 3 min as garbage-collector will work at least after 3 min !
sleep 10 !
commit release !
* PTS 1001032 !
file connect ( kern test !
create table ttt ( l &1 )!
insert into ttt set l = 1!
rollback release!
* PTS 1109806 !
file connect (kern test !
CREATE TABLE HUGO ( I INT, L &1 default 'kkk')!
INSERT INTO HUGO VALUES( 1, '111' )!
select * into :a, :b from hugo !
UPDATE HUGO SET I=2,L='2222'!
select * into :a, :b from hugo !
update hugo set l = null!
select * into :a, :b from hugo !
update hugo set l = default!
select * into :a, :b from hugo !
rollback release !
* PTS 1113258 - 260  LONG as primary KEY !
file connect (kern test !
CREATE TABLE TAB170
 ( COL1  &1,
   COL2  &1 CONSTRAINT fk_COL1_selfREF REFERENCES  TAB170(COL1),
     CONSTRAINT pk_COL1 PRIMARY  KEY (COL1)) !
rollback release !
* PTS 1114777-9 default for long !
file connect ( kern test !
create table t ( a char (8), b fixed (7))!
insert t values ('aaa', 1)!
create table t2 ( a varchar (8)not null default '', 
a1 &1 not null default '', b fixed(7))!
insert into t2 (b) values (17)!
pars_then_ex !
insert into t2 (b) values (44)!
pars_execute !
insert into t2 (b) select b from t !
select * from t2 !
fetch !
create table t3 ( a varchar (8)not null default 'r', 
a1 &1 not null default 'r', b fixed(7))!
insert into t3 (b) values (17)!
pars_then_ex !
insert into t3 (b) values (44)!
pars_execute !
insert into t3 (b) select b from t !
select * from t3 !
fetch !
rollback release !
* PTS 1116474 !
file connect ( kern test !
create table t1 (s1 int, s2 char (10), s3 &1) !
    insert t1 values (1, 'Hallo', 'long value') !
    insert t1 values (1, 'BYE BYE', NULL) !
create table t2
    as select (s1 - 1) s1, s2 from t1 !
create view v1
    as select * from t1 !
select * from v1 !
fetch !
create view v2
    as select * from t1 where t1.s1 not in (select s1 from t2) !
select * from v2!
fetch !
rollback release !
* PTS 1119108 !
file connect ( kern test !
create table tab6 (
	a1 &1 ,
	a2 char(515) 
  	)!
insert into tab6 values( '¤bY@O£{of[]}²³èîùÖÜ¢¥Púñ¿½¼»¦',
   		    '¤bY@O£{of[]}²³èîùÖÜ¢¥Púñ¿½¼»¦' )!
pars_then_ex !
data !
insert into tab6 values( :p,
   		    '¤bY@O£{of[]}²³èîùÖÜ¢¥Púñ¿½¼»¦' )!
l 30 '¤bY@O£{of[]}²³èîùÖÜ¢¥Púñ¿½¼»¦'

pars_execute !
nodata !
select * from tab6!
fetch!
rollback release !
* PTS 1120259-1120261 !
file connect ( kern test !
create table t ( a int, b int, c int,
                 primary key (a,b,c),
                 d int,
                 e &1)!
insert into t values (1,1,1,1,'alter Wert')!
pars_then_ex !
data !
update t set c =c+1, e = :long
   where a = 1 and b = 1 and d = 1!
l 300 'neuer Wert '

nodata !
select * from t!
fetch!
update t set c =c+1
   where a = 1 and b = 1 and d = 1!
pars_execute !
select * from t!
fetch!
update t set c =c+1, e = null
   where a = 1 and b = 1 and d = 1!
select * from t!
fetch!
rollback release !
file connect ( kern test !
create table t ( k char (8) primary key,a char (8), 
   b fixed (7), c &1 byte)!
insert into t values ('key', 'aaa', 1, null)!
create table dummy_long (dummylong &1 byte)!
insert dummy_long values (null)!
select k,a,b,c from t 
union all 
select 'k2', null, null, null from dual, dummy_long
order by 1!
mfetch!
2

select k,a,b,c from t 
union all 
select 'k2', null, null, dummylong from dual, dummy_long
order by 1!
mfetch!
2

rollback release !
* PTS 1123551-3!
senderid ODB!
file connect ( kern test !
 CREATE TABLE test2 (
    fragmentApplet  &1 VARCHAR DEFAULT ' ',
    fragmentCreated date         DEFAULT date,
    fragmentId      integer      NOT NULL PRIMARY KEY
 )!
 INSERT INTO test2 (
    fragmentCreated,fragmentId
    ) VALUES (
    '1999-01-01',-1 )
! 
 INSERT INTO test2 (
    fragmentCreated,fragmentId
    )
    select date, 4711 from dual !
 INSERT INTO test2 (
    fragmentCreated,fragmentId
    ) VALUES (
    date('1999-01-01'),0 )
! 
 INSERT INTO test2 (
    fragmentApplet,fragmentCreated,fragmentId
    ) VALUES (
    'test applet',date('1999-01-23'),3 )
!
 INSERT INTO test2 (
    fragmentApplet,fragmentCreated,fragmentId
    ) VALUES (
    'test applet','1999-01-23',4 )!
insert test2 select fragmentApplet,fragmentCreated,fragmentId+100
from test2!
declare c cursor for select * from test2 for reuse!
fetch c!
rollback release !
senderid XCI!
 * PTS 1123714-7 !
file connect ( kern test !
create table test (id integer, long_col &1 byte)!
insert into test values (1, 'aa')!
insert into test values (1, 'bb')!
insert into test values (2, 'cc')!

SELECT DISTINCT id FROM test WHERE id IS NOT NULL!
fetch!
SELECT DISTINCT id from test WHERE long_col IS NOT NULL!
fetch!
SELECT DISTINCT id FROM 
   (select id from test WHERE long_col IS NOT NULL) !
fetch!
SELECT DISTINCT id FROM test where id in
   (select id from test WHERE long_col IS NOT NULL) !
fetch!
SELECT DISTINCT id FROM test where id in
   (select id from test WHERE long_col IS NOT NULL AND rowno <= 1000000) !
fetch!
SELECT DISTINCT id, long_col from test WHERE long_col IS NOT NULL!
fetch!
SELECT DISTINCT id FROM 
   (select id, long_col from test WHERE long_col IS NOT NULL) !
fetch!
SELECT DISTINCT id FROM test where (id, long_col) in
   (select id, long_col from test WHERE long_col IS NOT NULL) !
fetch!
SELECT DISTINCT id FROM test where (id , long_col) in
   (select id, long_col from test 
     WHERE long_col IS NOT NULL AND rowno <= 1000000) !
fetch!
rollback release !
 * PTS 1123726-9 !
file connect ( kern test !
CREATE TABLE prima (
   oid INTEGER,
   CONSTRAINT prima_pk PRIMARY KEY(oid),
   val &1 DEFAULT '')
!
 CREATE TABLE sec1 (
   oid INTEGER DEFAULT SERIAL,
   CONSTRAINT sec1_pk PRIMARY KEY(oid),
   CONSTRAINT sec1_fk FOREIGN KEY(oid) REFERENCES prima(oid) ON DELETE
CASCADE,
   val &1 DEFAULT '')
!
 CREATE VIEW V2 (OID, val, VAL1) AS
   SELECT P.OID, p.val, S1.VAL
   FROM PRIMA P, SEC1 S1
   WHERE P.OID = S1.OID
   WITH CHECK OPTION
!
 INSERT INTO v2 (oid, val, val1) VALUES (1, 'prima a', 'sec1 a')!
pars_then_ex !
INSERT INTO v2 (oid, val, val1) VALUES (2, 'prima b', 'sec1 b') !
data !
INSERT INTO v2 (oid, val, val1) VALUES (3, :a, :b ) !
l 7 'prima c'
l 6 'sec1 c'

nodata !
pars_execute!
 select * from v2!
 fetch!
rollback release !
* PTS 1124111-4 !
file connect ( kern test !
CREATE TABLE DBM350_SM (MSN SERIAL(8) PRIMARY KEY,
StatusCode INTEGER, DIST_FILE &1 BYTE, DIST_NUM INTEGER)!
insert DBM350_SM (StatusCode, DIST_FILE, DIST_NUM) values
(3, null, 11)!
insert DBM350_SM (StatusCode, DIST_FILE, DIST_NUM) values
(3, null, 11)!
select * from DBM350_SM!
fetch!
pars_then_ex!
data!
UPDATE DBM350_SM SET StatusCode=-2, DIST_FILE=?, 
DIST_NUM=DIST_NUM+1 WHERE MSN=9!
l 917 'abcdefg'

pars_execute!
nodata !
select * from DBM350_SM!
fetch!
rollback release !
* PTS 1126089/1126130-3 !
file connect ( kern test !
create table t ( a char (8), b long byte default 'abc')!
pars_then_ex !
insert t (a) values ('xyz')!
pars_execute !
select * into :a, :b from t !
rollback release !
* functions for longs !
file connect ( kern test !
create table t ( k int, a char (30), b &1)!
insert into t values (1, 'abcdef', 'abcdef')!
pars_then_ex !
data !
insert into t values (2, 'kurzer Long', :a)!
l 1234 'das ist der 1234 Bytes lange String'

pars_then_ex !
data !
insert into t values (2, 'langer long', :a)!
l 12345 'das ist der 12345 Bytes lange LOOOONG String'

nodata !
pars_execute!
select *, substr(b, 5) from t!
select *, substr(b, 5, 100) from t!
fetch!
select *, substr(b, 1, 20) from t!
fetch!
select *, substr(b, 10, 20) from t!
fetch!
select *, substr(b, 1, 20000) from t!
fetch!
select *, substr(b, 10000, 3000) from t!
fetch!

select *, length(a), length (b) from t!
fetch!
rollback release !
* PTS 1130372-4 !
file connect ( kern test sqlmode oracle !
CREATE TABLE DOCUMENT_FORMAT ( 
E_DOC_VER_K_DOCUMENT  NUMBER (9) NOT NULL, 
B_TEXT                LONG byte)!
parsing !
INSERT INTO DOCUMENT_FORMAT (E_DOC_VER_K_DOCUMENT,B_TEXT ) 
         SELECT TO_NUMBER(?), ? FROM DUAL!
INSERT INTO DOCUMENT_FORMAT (E_DOC_VER_K_DOCUMENT,B_TEXT ) 
         VALUES ( TO_NUMBER(?),?  ) !                
executing !
data !
insert !
1
c 254 '1'
l 80 'abcd '
         
nodata !         
pars_execute !
select * from DOCUMENT_FORMAT!
fetch into :a, :b !
drop table DOCUMENT_FORMAT!
commit release !
* PTS 1130554-5 !
file connect ( kern test !
create table t ( a char (8), lb long byte, l long, la long ascii, b char (8))!
insert into t values ('abcdefgh', x'313233616263', '414243616263', 
'515253616263', 'bcdefghi')!
insert into t values ('abcdefgh', x'313233616263', '414243616263', 
'515253616263', 'bcd' || 'efg 2')!

pars_then_ex !
insert into t values ('abcdefgh', x'313233616263', '414243616263', 
'515253616263', 'bcdefg 3')!
insert into t values ('abcdefgh', x'313233616263', '414243616263', 
'515253616263', 'bcd' || 'efg 4')!

data !
insert into t values ('abcdefgh', x'313233616263', :p, 
'515253616263', 'bcdefg 5')!
l 12 '414243616263'

insert into t values ('abcdefgh', x'313233616263', :p,  
'515253616263', 'bcd' || 'efg 6')!
l 12 '414243616263'

pars_execute !
nodata !
select * from t !
fetch!
rollback release !
* PTS 1132769/ 1132814 !
file connect ( kern test !
CREATE TABLE T1 (S1 INT, S2 CHAR (10), S3 LONG) ! 
INSERT T1 VALUES (1, 'Hallo', 'long value') !
INSERT T1 VALUES (1, 'BYE BYE', NULL) !
CREATE TABLE T2 AS SELECT (S1 - 1) S1, S2 FROM T1 !    
CREATE VIEW V2 AS SELECT * FROM T1 WHERE T1.S1 NOT IN (SELECT S1 FROM T2) ! 
SELECT * FROM V2!
fetch!
SELECT * FROM (SELECT * FROM T1)!
fetch!
rollback release !
* PTS 1133127 !
file connect ( kern test !
create table t ( a int, b &1)!
insert t values (1, 'das ist der long')!
comment on table kern.t is 'das ist der Kommentar'!
select a, substr(b,1,8) from t !
fetch!
select *, substr(comment,1,8) from sysdd.tables
where tablename = 'T' and schemaname = 'KERN'!
rollback release !
* PTS 1133749 !
file connect ( kern test !
create table t ( l &1, a char (300))!
insert t values ('das ist der long-wert', 'abc')!
pars_then_ex !
data !
insert t values (:l, 'abc')!
l 12000 'das ist der 2. long-wert'

pars_execute!
nodata !
update t set a = substr (l, 1,20)!
select * from t !
fetch!
update t set a = substr (l, 1,400)!
update t set a = substr (l, 13,100)!
select * from t !
fetch!
rollback release !
* PTS 1133782/3 !
file connect ( kern test !
create table tab1 (a int primary key, b char(10), c &1, d fixed)!
create table tab2 (a int, b char(10), c &1, d int)!
insert into tab1 values (1, 'bla', null, 5.2)!
insert into tab2 values (1, 'bla', null, 5)!
create view seltab1 as select * from tab1!
create view seltab2 as select * from tab2!
create view seltab1_v1 as 
select * from seltab1 s where s.a in (select a from seltab2)!
create view cc1 as select * from seltab1_v1 !
create view seltab1_v2 as 
select * from seltab1 s !
select * from seltab1 s where s.a in (select a from seltab2)!
fetch!
select * from seltab1_v2!
fetch!
select * from seltab1_v1!
fetch !
select * from cc1!
fetch!
rollback work release !
 * PTS 1130589, 1136988 !
file connect ( kern test !
create table TAB 
(
    ID char(24) byte,
    PROP char(24) byte,
    SHORT_COL char(24) ascii,
    LONG_COL long byte
)!
insert tab values (x'414243', x'515253', 'xyz', x'31323334')!
select * into :a, :b, :c, :d from tab !
pars_then_ex!
data !
update TAB set SHORT_COL=?, LONG_COL=? where ID=? and PROP=?!
c 24 'NEWVALUE'
l 120 'newvalue NEWVALUE '
b 24 414243
b 24 515253

pars_execute
nodata !
select * into :a, :b, :c, :d from tab !
pars_then_ex!
data !
update TAB set (SHORT_COL, LONG_COL) = (?,?) where ID=? and PROP=?!
c 24 'NEWER VALUE'
l 120 'newervalue NEWERVALUE '
b 24 414243
b 24 515253

pars_execute
nodata !
select * into :a, :b, :c, :d from tab !
pars_execute !
rollback release !
* PTS 1139198/9 !
file connect ( kern test !
create table t ( a int , b &1, c &1, d int,
e &1, f int, g &1, h int)!
insert t values (1,'default','default',1,'default',1,'default',1)!
insert t values (2,'default','default',2,'default',2,'default',2)!
insert t values (3,'default','default',3,'default',3,'default',3)!
pars_then_ex !
data !
insert t values (:a,:b,:c,:d,:e,:f,:g,:h)!
n 10 4
l 40 
l 1000 'alter Wert '
n 10 4
l 40 
n 10 4
l 40 
n 10 4

update t set
a =:a, b = :b, c = :c, d = :d, e = :e, f = :f, g = :g, h = :h
where a <  3 !
n 10 4711
l 40 
l 1000 'neuer Wert '
n 10 4711
l 40 
n 10 4711
l 40 
n 10 4711

pars_execute !
nodata !
select * from t !
fetch!
rollback release !
file connect ( kern test !
create table test ( i int , c char(10) , l long ) !
insert test values (1,'hallo', 'xxx')!
parsing !
update test set c = ? , l = ? where i = ? !
insert test values (2, 'hallu', ?) !
executing !
data!

update !
1
c 10 'halli'
c 32
n 10 1

insert !
2
l 10 'test'

pars_execute !
rollback release !
file connect ( kern test !
create table t1 (s1 int key, s2 long) !
commit !
parsing !
insert t1 values (1, 'Dies ist ein doofer test') !
pars_execute !
rollback !
executing !
insert !
1


pars_execute !
drop table t1 !

* PTS 1140810!
create table s ( CID char(24)) !
insert s values ('3031')!

create table t ( 
 CID char (24) , 
 NAME_ID char (24) , 
 SHORT_VALUE char (499) not null , 
 LONG_VALUE  long byte,
 viewowner char(32) default usergroup,
 primary key (NAME_ID,CID)) ! 

pars_then_ex !
data !

insert t columns (cid, name_id, short_value, long_value) 
values ((select CID from s where CID = ?), ?, ?, ?)!
c 24 '3031' 
c 24 '3031' 
c 499 'hallo'
c 32 

nodata!
pars_execute!

select * from t!
fetch !

drop table t!
drop table s!

commit release !
* PTS 1141095/6, 1141121 !
file connect ( kern test !
create table t ( a char (8), b &1 default 'abcdef',
 c &1 default 'mopqr', d &1, e fixed (8))!
insert t values ('aaaa', default, default, 'abc', 1)!
pars_then_ex !
data !
insert t values (:a, :b, default, :d, 2)!
c 8 'bbb'
d
l 120 'long'

insert t values ('a2', default, :c, :d, :e)!
d
l 300 'dddd'
n 8 4

pars_execute!
nodata !
select * from t !
fetch!

update t set b = 'das ist neues b' where e = 1!

pars_then_ex !
data !
update t set b = :b where e = 2!
d

pars_execute!
nodata !
select * from t !
fetch!
rollback release !
* PTS 1141452-3 !
file connect ( kern test !
create table dummy1 (
"Column1" char (8), "Memo" LONG) !
pars_then_ex !
data !
insert dummy1 values ('abc', :l)!
l 120 'das ist der lob '

insert dummy1 values ('def', :l)!
l 10200 'das ist der loooooong lob of 10200 bytes '

pars_execute !
nodata!
create table dummy2 ("Column1" char (8)) !
insert dummy2 values ('abc')!
insert dummy2 values ('def')!
create view v1 as 
SELECT T0."Column1", SUBSTR(T0."Memo", 1, 50) 
    FROM DUMMY1 T0 
         INNER JOIN 
         DUMMY2 T1 
         ON T0."Column1" = T1."Column1" !
CREATE TABLE TEMP.TEST3 ("Column1", "ColumnMemo") AS 
  SELECT T0."Column1", SUBSTR(T0."Memo", 1, 50) 
    FROM DUMMY1 T0 
         INNER JOIN 
         DUMMY2 T1 
         ON T0."Column1" = T1."Column1" !
CREATE TABLE TEMP.TEST3A ("Column1", "ColumnMemo") AS 
  SELECT T0."Column1", substr_lob
    FROM (select "Column1", SUBSTR("Memo", 1, 50) substr_lob from DUMMY1) T0
         INNER JOIN 
         DUMMY2 T1
         ON T0."Column1" = T1."Column1" !
CREATE TABLE TEMP.TEST5("Column1", "ColumnMemo") AS 
  SELECT * from v1 !
select * from TEMP.TEST3!
fetch!         
select * from TEMP.TEST3A!
fetch!         
select * from TEMP.TEST5!
fetch!         
rollback release !
