*   *ID* A03INSEL DBCMD    changed on 1992-09-28-13.22.19 by ADMIN     *
 ***************************
 ***   A 0 3 I N S E L   ***
 ***************************!
file connect ( kern test !
drop table kern.t1 !
drop table kern.t2 !
drop table kern.i1 !
drop table kern.i2 !
create table kern.t5 (f1 fixed (3,0), f2 float (7), f3 char(10),
      f4 char (5) byte ) !
create table kern.t6 (f1 fixed (3,0) key, f2 float (7),
   f3 char (10),   f4 char (5) byte ) !
create table kern.i1 (f2 float (7),
      f4 char (5)  byte) !
create table kern.i2 (f3 char (10) key ,
      f1 fixed (3,0) ) !
commit work !
insert kern.t5 values (1, 100, 'feld100', x'010101' )!
insert kern.t5 values (2, 200, 'feld200', x'020201' )!
insert kern.t5 values (3, 300, 'feld300', x'030203' )!
insert kern.t5 values (4, 400, 'feld400', x'040203' )!
insert kern.t5 values (5, 500, 'feld500', x'050203' )!
insert kern.t5 values (6, 600, 'feld600', x'060203' )!
insert kern.t6 values (21, 100, 'feld100', x'010101' )!
insert kern.t6 values (22, 200, 'feld200', x'020201' )!
insert kern.t6 values (23, 300, 'feld302', x'030203' )!
insert kern.t6 values (24, 400, 'feld402', x'040203' )!
insert kern.t6 values (25, 500, 'feld500', x'050203' )!
insert kern.t6 values (26, 600, 'feld600', x'060203' )!
commit work !
insert kern.i1 select f2, f4 from kern.t5 where  f1 >= 3!
insert kern.i1 select f2, f4 from kern.t6 where  f1 >= 23!
insert kern.i2 select f3, f1 from kern.t5 where  f1 >= 3!
insert kern.i2 select f3, f1 from kern.t6 where  f1 >= 23!
commit work !
select * from kern.i1 !
fetch !
select * from kern.i2 !
fetch !
select * from kern.t5 !
fetch !
delete kern.i1 !
create index "kern.t5.f1"
	on kern.t5( f1 ) !
insert kern.i1 select f2, f4 from kern.t5 where
  f1 = 3 or f1 = 4 or f1 = 5 ignore duplicates !
select * from kern.i1 !
fetch !
 *********************************************!
create table withkey ( w1 char (4) key,
                       w2 float(8),
                       w3 char (7) )!
create table ohnekey ( o1 char (4),
                       o2 fixed(7,2))!
 ******** falsche insert select
 ******** syntaktisch falsch !
insert withkey select * into :a,:b from ohnekey !
insert withkey select * from ohnekey order by o1 !
 *semantisch falsch !
insert withkey (w1, w3) select w1, w1 from withkey !
insert withkey select * from ohnekey !
insert ohnekey (o1) select w1,w2 from withkey !
insert ohnekey select w1, w2, w3 from withkey !
insert ohnekey (o1) select w3 from withkey !
insert ohnekey (o2) select w2 from withkey !
 **************************************************
 *********** semantisch teilweise richtig !
insert ohnekey values ('aaaa', 23.7)!
insert ohnekey values ('aaaa', 15)!
insert ohnekey values ('fff' ,30)!
insert withkey (w1, w2) select * from ohnekey reject duplicates !
select * from withkey !
fetch !
insert withkey (w1, w2) select * from ohnekey ignore duplicates !
select * from withkey !
fetch !
delete withkey !
insert withkey (w1, w2) select * from ohnekey update duplicates !
select * from withkey !
fetch !
delete withkey !
insert withkey (w1, w2) select * from ohnekey where o2 > 20.0
          reject duplicates !
select * from withkey !
fetch !
insert ohnekey (o1) select w1 from withkey !
select * from ohnekey !
fetch !
delete withkey !
create view ohneview as select * from ohnekey where o2 > 25 !
commit work !
insert ohnekey select * from ohneview !
rollback work !
insert withkey (w1, w2) select * from ohneview !
select * from withkey !
fetch !
delete withkey !
insert withkey (w1, w2) select * from ohneview where o2 > 20 !
select * from withkey !
fetch !
delete withkey !
create view withview as select * from withkey where w3 is not null
     with check option !
insert withview (w1, w2) select * from ohnekey !
select * from withview !
drop table ohnekey !
drop table withkey !
commit !
* check, ob richtige Baserecords bei INSERT SELECT genommen werden !
create table t ( a date, b char (15), c fixed (8))!
insert t set a = date, b = 'ddd'!
create index i on t (a,b)!
create table t1 (a1 char (15) key)!
insert t1 set a1 = 'ddd'!
alter table t foreign key l1 (b) references t1 !
create view v as select c, a, b from t !
create view v1 as select * from v !
create synonym syn for v1 !
declare erg cursor for 
select * from v1 for reuse!
insert v1 select * from erg!
insert v1 select * from v1!
insert syn select * from v1!
select * from t !
fetch!
rollback work !
create table t ( a char (8), b char (9))!
insert t values ('aaa', 'bbb')!
insert t values ('bbb', 'ccc')!
create table t1 (a1 char (8), b1 char (9))!
pars_then_ex !
declare ergebdel cursor for 
select a,b from t for reuse !
insert t1 select * from ergebdel !
pars_execute !
select * from t1 !
fetch !
rollback !
create table t ( a char (8), b char (9))!
insert t values ('aaa', 'bbb')!
insert t values ('bbb', 'ccc')!
create table t1 (a1 char (8), b1 char (9)) !
pars_then_ex !
parse declare ergebdel cursor for 
select a,b from t for reuse !
parse insert t1 select * from ergebdel !
pars_execute !
select * from t1 !
fetch !
rollback work release !
file connect ( kern test !
create table tabx (key1 fixed(9), key2 fixed(9))!
create index ind2 on tabx (key2, key1)!
commit !
insert into tabx set key1 = 1, key2 = 1 !
commit !
select * from tabx!
fetch!
insert into tabx select key1+1, key2+1 from tabx !
select * from tabx!
fetch!
drop table tabx !
commit work release !
file connect ( kern test !
* implicit conversion of char <--> number in Oracle during
* (implicit) INSERT .. SELECT !
create table t ( a char (10), b char (4))!
insert into t values ('12345678', 'abcd')!
create table t1 (a fixed (8), b char(5))!
create table t2 (a fixed(8,1), b char (8))!
create table tfc (a fixed(8), c char (3))!
commit !
insert into t1 select * from t !
commit release !
file connect ( kern test sqlmode oracle !
insert into t1 select * from t !
select * from t1!
fetch into :a, :b !
insert into t2 select * from t !
insert into tfc select * from t !
insert into t select * from t1 !
insert into t select 543,b from t1 !
select * from t !
fetch into :a, :b !
drop table t !
drop table t1!
drop table t2!
drop table tfc!
commit work release !
file connect ( kern test !
create table t ( a char (1))!
insert t set a = 'a'!
create table u ( a char (1))!
INSERT INTO U SELECT a FROM T A WHERE NOT EXISTS 
            (SELECT * FROM T B WHERE b.a <> a.a) !
select * from u!
fetch!
pars_then_ex !
INSERT INTO U SELECT a FROM T A WHERE NOT EXISTS 
            (SELECT * FROM T B WHERE b.a <> a.a) !
pars_execute !
select * from u!
fetch!
drop table t!
drop table u!
* PTS 1001154/55/61 !
create table t ( a char (8) not null, b char (8))!
insert t values (substr('hallo', null), 'hallo')!
drop table t !
commit work release !
* PTS 1115407-9 !
file connect ( kern test !

CREATE TABLE TBMAHIERARJOB
(
MANDANT         INTEGER  NOT NULL DEFAULT 0,
ID              INTEGER  NOT NULL DEFAULT 0,
HISTNR          INTEGER  NOT NULL DEFAULT 0,
STATUS          SMALLINT NOT NULL DEFAULT 0,
GUELTIGAB       DATE     NOT NULL DEFAULT DATE,
FKOFSTAMM       INTEGER  NOT NULL DEFAULT 0,
FKMAMITARBEITER INTEGER  NOT NULL DEFAULT 0,
FKFGUSER        INTEGER  NOT NULL DEFAULT 0,
FKFGUSERHISTNR  INTEGER  NOT NULL DEFAULT 0,
ERFASST         DATE     NOT NULL DEFAULT DATE,
MANUMMER        CHAR(10) NOT NULL DEFAULT ' ',
BEARBSTATUS     SMALLINT NOT NULL DEFAULT 0,
TABLENAME       CHAR(18) NOT NULL DEFAULT ' ',
BEARBTYP        SMALLINT NOT NULL DEFAULT 0,
BEARBVALUE      CHAR(50) NOT NULL DEFAULT ' ',
BEARBGRUND      INTEGER  NOT NULL DEFAULT 0,
CONSTRAINT P_KEY_1 PRIMARY KEY (MANDANT, ID, HISTNR)
)!

CREATE TABLE TBMAMITARBEITER
(
MANDANT         INTEGER  NOT NULL DEFAULT 0,
ID              INTEGER  NOT NULL DEFAULT 0,
HISTNR          INTEGER  NOT NULL DEFAULT 0,
STATUS          SMALLINT NOT NULL DEFAULT 0,
GUELTIGAB       DATE     NOT NULL DEFAULT DATE,
FKOFSTAMM       INTEGER  NOT NULL DEFAULT 0,
FKFGVERTRIEB    INTEGER  NOT NULL DEFAULT 0,
FKFAFIBU        INTEGER  NOT NULL DEFAULT 0,
MANUMMER        CHAR(10) NOT NULL DEFAULT ' ',
MAUEBERID       INTEGER  NOT NULL DEFAULT 0,
MAUEBERNR       CHAR(10) NOT NULL DEFAULT ' ',
MAHAUPTID       INTEGER  NOT NULL DEFAULT 0,
MAHAUPTNR       CHAR(10) NOT NULL DEFAULT ' ',
MASTATUS        INTEGER  NOT NULL DEFAULT 0,
MAHAUPTSTATUS   SMALLINT NOT NULL DEFAULT 0,
MAART           INTEGER  NOT NULL DEFAULT 0,
MASTUFE         SMALLINT NOT NULL DEFAULT 0,
MAAUSZAHLSTOP   SMALLINT NOT NULL DEFAULT 0,
MAEINTRITT      DATE     NOT NULL DEFAULT DATE,
MAAUSTRITT      DATE     NOT NULL DEFAULT DATE,
MAAUSTRITTGRUND INTEGER  NOT NULL DEFAULT 0,
MACHECK         SMALLINT NOT NULL DEFAULT 0,
MACHANGE        SMALLINT NOT NULL DEFAULT 0,
CONSTRAINT P_PKMA01 PRIMARY KEY (MANDANT, ID, HISTNR)
)!
parsing !

INSERT INTO TBMAHIERARJOB (    
MANDANT, ID, HISTNR, STATUS, GUELTIGAB, FKOFSTAMM,
FKMAMITARBEITER, FKFGUSER, FKFGUSERHISTNR, ERFASST,
MANUMMER, BEARBSTATUS, TABLENAME, BEARBTYP,
BEARBVALUE, BEARBGRUND  )
(SELECT 
  MANDANT MANDANT,      
  1 + ID  ID,             
  1       HISTNR,         
  2       STATUS,         
  ?       GUELTIGAB,   
  FKOFSTAMM          FKOFSTAMM, 
  ID                 FKMAMITARBEITER,
  0                  FKFGUSER,
  0                  FKFGUSERHISTNR, 
  ?       ERFASST,           
  manummer           MANUMMER,       
  0                  BEARBSTATUS,    
  'TBMAMITARBEITER'  TABLENAME,      
  1                  BEARBTYP,       
  mauebernr          BEARBVALUE,     
  0             BEARBGRUND           
from tbmamitarbeiter 
where 
 mandant = 0 and status = 2)!

pars_execute !

rollback release !
