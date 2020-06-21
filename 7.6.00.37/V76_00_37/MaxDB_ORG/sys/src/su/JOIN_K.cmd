senderid DBM !
file connect ( sut sut ! 
set parameter OPTIMIZE_OPERATOR_JOIN = '&1' !
commit release !
senderid XCI !

*****************************************************************************
* K03MAX.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table t ( a char (255))!
insert into t values ('abcdefghijklm')!
insert into t values ('nopqrstuvwxyz')!
create table t1 ( b char (300))!
insert into t1 values ('abcdefghijklm')!
insert into t1 values ('t1-t1-t1-t1')!
select * from t a, t b where a.a = b.a !
fetch !
select * from t, t1 where t.a || t1.b = 'abcdefghijklmabcdefghijklm' !
fetch!
create table j1 ( a char (80), b char (173)) !
insert into j1 values ('aaa', 'bbb')!
select * from j1, j1 j2, t where j1.a > j2.a or t.a = 'abcdefghijklm' !
fetch!
close!
select * from j1 x, j1 y where x.a=y.a and x.b=y.b !
fetch !
select * from t, t1 where t.a = t1.b !
fetch !
create table e ( e1 char (8)) !
insert e set e1 = 'e1'!
create view k3 as select chr(t.a,18), chr(t.a,257), t.a from e, t !
select * from t, k3 !
fetch!
select * from t, k3 where expression1 = 'abcdefghijklm'!
fetch!
drop table e !
commit release !
file connect ( kern test sqlmode oracle !
drop table t !
drop table t1 !
commit work release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* PTS 1124653 
* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124653
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !

CREATE TABLE TAB1
(
	"K0"    Char   (240) ASCII NOT NULL,
	"K1"    Fixed  (10,0) NOT NULL,
	"I1"    Fixed  (10,0),
	"I2"    Fixed  (10,0),
	"C1"    Char   (20) ASCII,
	"C2"    Varchar(20) ASCII NOT NULL,
	"I3"    Fixed  (10,0) NOT NULL,
	"CLLEN" Fixed  (10,0),
	"CF0"   Char   (1500) ASCII,
	"CLB"   Long ASCII,
	PRIMARY KEY ("K0", "K1")
)!
CREATE INDEX TAB1_K1 ON TAB1("K1" ASC)!

insert into tab1(k0, k1, c2, i3) values ('0', 0,'cf', 40)!
insert into tab1(k0, k1, c2, i3) values ('0', 1,'cf', 50)!
insert into tab1(k0, k1, c2, i3) values ('0', 2,'cf', 59)!

CREATE VIEW "SBVIEW_I_P_16" (
	"K0", "K1", "I1", "I2", "C1", "C2", "I3", "CLLEN", "CF0", "CLB" )
AS SELECT T1.K0, T1.K1, T1.I1, T1.I2, T1.C1, T1.C2, T1.I3, 
T1.CLLEN, T1.CF0, T1.CLB 
FROM TAB1 T1, TAB1 T2, TAB1 T3, TAB1 T4, 
TAB1 T5, TAB1 T6, TAB1 T7, TAB1 T8, TAB1 T9, TAB1 T10, TAB1 T11, 
TAB1 T12, TAB1 T13, TAB1 T14, TAB1 T15, TAB1 T16 
WHERE 
T2.K1 = T1.K1   AND T3.K1 = T2.K1   AND 
T4.K1 = T3.K1   AND T5.K1 = T4.K1   AND T6.K1 = T5.K1   AND 
T7.K1 = T6.K1   AND T8.K1 = T7.K1   AND T9.K1 = T8.K1   AND 
T10.K1 = T9.K1  AND T11.K1 = T10.K1 AND T12.K1 = T11.K1 AND 
T13.K1 = T12.K1 AND T14.K1 = T13.K1 AND T15.K1 = T14.K1 AND 
T16.K1 = T15.K1 !

explain SELECT count(i3) INTO :P_1 
FROM SBVIEW_I_P_16 WHERE I3 BETWEEN 48 and 58 !

SELECT count(i3) INTO :P_1 
FROM SBVIEW_I_P_16 WHERE I3 BETWEEN 48 and 58 !
rollback work release!

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* PTS 1129187
* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1129187
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !

CREATE TABLE INOB
(
	"MANDT"  Varchar (3) ASCII    NOT NULL    DEFAULT '000',
	"CUOBJ"  Varchar (18) ASCII    NOT NULL    DEFAULT '000000000000000000',
	"KLART"  Varchar (3) ASCII    NOT NULL    DEFAULT ' ',
	"OBTAB"  Varchar (30) ASCII    NOT NULL    DEFAULT ' ',
	"OBJEK"  Varchar (50) ASCII    NOT NULL    DEFAULT ' ',
	"ROBTAB" Varchar (30) ASCII    NOT NULL    DEFAULT ' ',
	"ROBJEK" Varchar (50) ASCII    NOT NULL    DEFAULT ' ',
	"CLINT"  Varchar (10) ASCII    NOT NULL    DEFAULT '0000000000',
	"STATU"  Varchar (1) ASCII    NOT NULL    DEFAULT ' ',
	"CUCOZHL" Varchar (4) ASCII    NOT NULL    DEFAULT '0000',
	"PARENT" Varchar (18) ASCII    NOT NULL    DEFAULT '000000000000000000',
	"ROOT"   Varchar (18) ASCII    NOT NULL    DEFAULT '000000000000000000',
	"EXPERTE" Varchar (1) ASCII    NOT NULL    DEFAULT ' ',
	"MATNR"  Varchar (18) ASCII    NOT NULL    DEFAULT ' ',
	"DATUV"  Varchar (8) ASCII    NOT NULL    DEFAULT '00000000',
	"TECHS"  Varchar (12) ASCII    NOT NULL    DEFAULT ' ',
	PRIMARY KEY ("MANDT", "CUOBJ")
)!
CREATE INDEX "INOB~1" ON INOB(
"MANDT" ASC, "OBTAB" ASC, "OBJEK" ASC, "KLART" ASC)!

CREATE TABLE KSSK
(
	"MANDT" Varchar (3) ASCII    NOT NULL    DEFAULT '000',
	"OBJEK" Varchar (50) ASCII    NOT NULL    DEFAULT ' ',
	"MAFID" Varchar (1) ASCII    NOT NULL    DEFAULT ' ',
	"KLART" Varchar (3) ASCII    NOT NULL    DEFAULT ' ',
	"CLINT" Varchar (10) ASCII    NOT NULL    DEFAULT '0000000000',
	"ADZHL" Varchar (4) ASCII    NOT NULL    DEFAULT '0000',
	"ZAEHL" Fixed (5,0)    NOT NULL    DEFAULT      0,
	"STATU" Varchar (1) ASCII    NOT NULL    DEFAULT ' ',
	"STDCL" Varchar (1) ASCII    NOT NULL    DEFAULT ' ',
	"REKRI" Varchar (1) ASCII    NOT NULL    DEFAULT ' ',
	"AENNR" Varchar (12) ASCII    NOT NULL    DEFAULT ' ',
	"DATUV" Varchar (8) ASCII    NOT NULL    DEFAULT '00000000',
	"LKENZ" Varchar (1) ASCII    NOT NULL    DEFAULT ' ',
	PRIMARY KEY ("MANDT", "OBJEK", "MAFID", "KLART", "CLINT", "ADZHL")
)!
EXPLAIN
SELECT /*+ordered,join(access=indexaccess;keyaccess)*/
  DISTINCT  T_01."CLINT"                                                     
FROM                                                                           
  "INOB" T_00, "KSSK" T_01                                                    
WHERE                                                                          
  T_01."MANDT" = '200' AND T_01."OBJEK" = T_00 . "CUOBJ" AND 
  T_00."MANDT" = '200' AND T_00."OBTAB" = '/SAPAPO/RES_HEAD' AND 
  T_00."KLART" = '400'        
  !
SELECT /*+ordered,join(access=indexaccess;keyaccess)*/
  DISTINCT  T_01."CLINT"                                                     
FROM                                                                           
  "INOB" T_00, "KSSK" T_01                                                    
WHERE                                                                          
  T_01."MANDT" = '200' AND T_01."OBJEK" = T_00 . "CUOBJ" AND 
  T_00."MANDT" = '200' AND T_00."OBTAB" = '/SAPAPO/RES_HEAD' AND 
  T_00."KLART" = '400'        
  !
commit work release!

file connect ( sut sut !
drop user kern !
commit work release !
*****************************************************************************
* test IN fetch access for access operator
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !

create table tab ( k1 int, k2 int, k3 int, primary key(k1, k2))!
create index i1 on tab(k1)!

insert into tab values(0, 0, 0)!
insert into tab values(1, 8, 0)!
insert into tab values(1, 9, 0)!
insert into tab values(2, 10, 0)!
insert into tab values(2, 0, 0)!

** keyaccess
explain 
select /*+ordered,join(access=keyaccess,in_strategy)*/ * 
from tab t1, tab t2 where
t1.k1 = 1 and t1.k2 in ( 10, 10, 9, 9, 8, 8, 10, 10 )!

select /*+ordered,join(access=keyaccess,in_strategy)*/ * 
from tab t1, tab t2 where
t1.k1 = 1 and t1.k2 in ( 10, 10, 9, 9, 8, 8, 10, 10 )!

explain 
select /*+ordered,join(access=keyaccess,in_strategy)*/ * 
from tab t1, tab t2 where
t1.k1 in ( 0, 1, 2, 0, 1, 2, 2, 1, 0 )!

select /*+ordered,join(access=keyaccess,in_strategy)*/ * 
from tab t1, tab t2 where
t1.k1 in ( 0, 1, 2, 0, 1, 2, 2, 1, 0 )!

* check right handling of small buffer
explain sequence 
select /*+ordered,join(buffersize(2),access=keyaccess,in_strategy)*/ * 
from tab t1, tab t2 where
t1.k1 in ( 0, 1, 2, 0, 1, 2, 2, 1, 0 )!
explain 
select /*+ordered,join(buffersize(2),access=keyaccess,in_strategy)*/ * 
from tab t1, tab t2 where
t1.k1 in ( 0, 1, 2, 0, 1, 2, 2, 1, 0 )!
select /*+ordered,join(buffersize(2),access=keyaccess,in_strategy)*/ * 
from tab t1, tab t2 where
t1.k1 in ( 0, 1, 2, 0, 1, 2, 2, 1, 0 )!

** indexaccess
explain
select /*+ordered,join(access=indexaccess,in_strategy)*/ 
t1.k1 as "t1.k1", t1.k2 as "t1.k2", t2.k1 as "t2.k1", t2.k2 as "t2.k2"
from tab t1, tab t2 where
t1.k1 in ( 0, 1, 2, 0, 1, 2, 2, 1, 0 )!
select /*+ordered,join(access=indexaccess,in_strategy)*/ 
t1.k1 as "t1.k1", t1.k2 as "t1.k2", t2.k1 as "t2.k1", t2.k2 as "t2.k2"
from tab t1, tab t2 where
t1.k1 in ( 0, 1, 2, 0, 1, 2, 2, 1, 0 )!

* check right handling of small buffer
explain sequence
select /*+ordered,join(buffersize(2),access=indexaccess,in_strategy)*/ 
t1.k1 as "t1.k1", t1.k2 as "t1.k2", t2.k1 as "t2.k1", t2.k2 as "t2.k2"
from tab t1, tab t2 where
t1.k1 in ( 0, 1, 2, 0, 1, 2, 2, 1, 0 )!
explain
select /*+ordered,join(buffersize(2),access=indexaccess,in_strategy)*/ 
t1.k1 as "t1.k1", t1.k2 as "t1.k2", t2.k1 as "t2.k1", t2.k2 as "t2.k2"
from tab t1, tab t2 where
t1.k1 in ( 0, 1, 2, 0, 1, 2, 2, 1, 0 )!
select /*+ordered,join(buffersize(2),access=indexaccess,in_strategy)*/ 
t1.k1 as "t1.k1", t1.k2 as "t1.k2", t2.k1 as "t2.k1", t2.k2 as "t2.k2"
from tab t1, tab t2 where
t1.k1 in ( 0, 1, 2, 0, 1, 2, 2, 1, 0 )!
commit work release!

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* PTS 1129336
* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1129336
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !

create table t1 (c11 int, c12 int)!
create table t2 (c21 int, c22 int, c23 char(5))!

insert into t1 values( 1, 2)!
insert into t1 values( 2, 2)!
insert into t1 values( 3, 5)!

insert into t2 values( 2, 2, 'asdfg')!
insert into t2 values( 2, 4, 'aswtq')!

select * from t1, t2
where
  t2.c21 (+) = t1.c11 AND
  t2.c22 IN (1, 2)!
fetch!
select * from t1, t2
where
  t2.c21 (+) = t1.c11 AND
  t2.c22 between 2 and 5 !
fetch!
select * from t1, t2
where
  t2.c21 (+) = t1.c11 AND
  t2.c23 like 'as%' !
fetch!

rollback work release!

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* function test for Join_SortOperator in interaction with ROWNO
senderid DBM!
file connect ( sut sut !
set parameter OPTIMIZE_JOIN_OPERATOR_SORT = 'NO' !

create table tab( c1 int, c2 int )!

explain 
select * from tab t1, tab t2 where t1.c1 = t2.c1 !
explain 
select * from tab t1, tab t2 where t1.c1=t2.c1 and t1.c2=t2.c2!

set parameter OPTIMIZE_JOIN_OPERATOR_SORT = 'YES' !
explain 
select * from tab t1, tab t2 where t1.c1 = t2.c1 !
explain 
select * from tab t1, tab t2 where t1.c1=t2.c1 and t1.c2=t2.c2!
* asuming tab contains cak_initrows records
explain 
select * from tab t1, tab t2 where t1.c1=t2.c1 and t1.c2=t2.c2
and rowno <= 1!

rollback work release!

*****************************************************************************
* test SUBQ fetch access for access operator, same test case as for IN
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !

create table tab ( k1 int, k2 int, k3 int, primary key(k1, k2))!
create index i1 on tab(k1)!

insert into tab values(0, 0, 0)!
insert into tab values(1, 8, 0)!
insert into tab values(1, 9, 0)!
insert into tab values(2, 10, 0)!
insert into tab values(2, 0, 0)!

create table subqtab1 ( c1 int )!
insert into subqtab1 values(10)!
insert into subqtab1 values(10)!
insert into subqtab1 values(9)!
insert into subqtab1 values(9)!
insert into subqtab1 values(8)!
insert into subqtab1 values(8)!
insert into subqtab1 values(10)!
insert into subqtab1 values(10)!

create table subqtab2 ( c1 int )!
insert into subqtab2 values(0)!
insert into subqtab2 values(1)!
insert into subqtab2 values(2)!
insert into subqtab2 values(0)!
insert into subqtab2 values(1)!
insert into subqtab2 values(2)!
insert into subqtab2 values(2)!
insert into subqtab2 values(1)!
insert into subqtab2 values(0)!

** keyaccess
explain 
select /*+ordered,join(access=keyaccess,subq_strategy)*/ * 
from tab t1, tab t2 where
t1.k1 = 1 and t1.k2 in ( select c1 from subqtab1 )!

select /*+ordered,join(access=keyaccess,subq_strategy)*/ * 
from tab t1, tab t2 where
t1.k1 = 1 and t1.k2 in ( select c1 from subqtab1 )!

explain 
select /*+ordered,join(access=keyaccess,subq_strategy)*/ * 
from tab t1, tab t2 where
t1.k1 in ( select c1 from subqtab2 )!

select /*+ordered,join(access=keyaccess,subq_strategy)*/ * 
from tab t1, tab t2 where
t1.k1 in ( select c1 from subqtab2 )!

* check right handling of small buffer
select /*+ordered,join(buffersize(2),access=keyaccess,subq_strategy)*/ * 
from tab t1, tab t2 where
t1.k1 in ( select c1 from subqtab2 )!

** indexaccess
explain
select /*+ordered,join(access=indexaccess,subq_strategy)*/ 
t1.k1 as "t1.k1", t1.k2 as "t1.k2", t2.k1 as "t2.k1", t2.k2 as "t2.k2"
from tab t1, tab t2 where
t1.k1 in ( select c1 from subqtab2 )!
select /*+ordered,join(access=indexaccess,subq_strategy)*/ 
t1.k1 as "t1.k1", t1.k2 as "t1.k2", t2.k1 as "t2.k1", t2.k2 as "t2.k2"
from tab t1, tab t2 where
t1.k1 in ( select c1 from subqtab2 )!

* check right handling of small buffer
select /*+ordered,join(buffersize(2),access=indexaccess,subq_strategy)*/ 
t1.k1 as "t1.k1", t1.k2 as "t1.k2", t2.k1 as "t2.k1", t2.k2 as "t2.k2"
from tab t1, tab t2 where
t1.k1 in ( select c1 from subqtab2 )!

commit work release!

file connect ( sut sut !
drop user kern !
commit work release !
*****************************************************************************

senderid XCI!
