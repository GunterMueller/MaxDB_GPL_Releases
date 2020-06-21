*   *ID* A17SEL22 DBCMD    changed on 1997-04-29-16.53.45 by ADMIN     *
* PTS 4254 !
file CONNECT (kern test!
CREATE TABLE TABX
( K FIXED(5),
  I FIXED(6),
  N FIXED(7),
  PRIMARY KEY( K,N ))!
INSERT INTO TABX VALUES( 1, 1, 1)!
CREATE TABLE TABY
( K FIXED(8),
  I FIXED(9),
  N FIXED(10),
  PRIMARY KEY( K,N ))!
INSERT INTO TABY VALUES( 1, 1, 1)!
CREATE VIEW C1
AS SELECT T2.K,T1.I,T1.N FROM TABX T1, TABY T2
WHERE T1.K = 1 OR T2.N = 1 !
SELECT * FROM C1!
FETCH!
PARSING!
DECLARE ERG CURSOR FOR 
SELECT T1.K COL1, T1.N COL2 FROM TABX T1
WHERE T1.K
IN (SELECT T3.N FROM C1 T3 ) FOR REUSE!
EXECUTING!
DATA!
SELECT     X!
1

pars_then_ex !
fetch erg into :a, :b, :c !
nodata !
pars_execute!
rollback release!
* ADIS Error ID 4290, 4296 !
file connect ( kern test !
create table t ( a char (3),  b char (8), primary key (e,b),
c varchar (10), d varchar (10), e varchar (12))!
insert into t values ('001', 'b0 ', 'ccc', 'ddd',  'SUPER')!
insert into t values ('001', 'b1 ', 'ccc', 'ddd',  'NICHT-SUPER')!
insert into t values ('001', 'b2 ', 'ccc', 'ddd',  'SUPER')!
create table t1 ( a char (8))!
insert into t1 values ('unsinn')!
pars_then_ex !
data !
select * from t where a = :a and not e = :e !
c 3 '001'
c 12 'SUPER'

nodata !
fetch into :a, :b, :c, :d, :e!
pars_execute !
create index i1 on t (e) !
pars_then_ex !
data !
select * from t where a = :a and not e = :ee !
c 3 '001'
c 12 'SUPER'

nodata !
fetch into :a, :b, :c, :d, :e!
pars_then_ex !
data !
select * from t,t1 where t.a = :a and not e = :e !
c 3 '001'
c 12 'SUPER'

nodata !
fetch into :a, :b, :c, :d, :e, :f!
pars_execute !
rollback !
create table t ( a char (3),  b char (8), primary key (e,b),
c varchar (10), d varchar (10), e int)!
insert into t values ('001', 'b0 ', 'ccc', 'ddd',  0)!
insert into t values ('001', 'b1 ', 'ccc', 'ddd',  1)!
insert into t values ('001', 'b2 ', 'ccc', 'ddd',  0)!
create table t1 ( a char (8))!
insert into t1 values ('unsinn')!
select * from t where a = '001' and not 3*e = 0 !
fetch!
create index i1 on t (e) !
select * from t where a = '001' and not 3*e = 0 !
fetch!
select * from t,t1 where t.a = '001' and not 3*e = 0 !
fetch!
rollback release !
* das letzte MFETCH lieferte in 7.2.0.0 kein row not found,
* wenn es wie hier über den Index gezwungen wurde (bd03-Problem)!
file connect (kern test !
create table t ( a char (8), b char (30), c char (30), 
     d char (30), e char (8))!
insert t values ('aaa', 'b', 'c', 'd', 'eee')!
insert t values ('aaa', 'b', 'c', 'd', 'eee')!
insert t values ('aaa', 'b', 'c', 'd', 'eee')!
insert t values ('aaa', 'b', 'c', 'd', 'eee')!
insert t values ('aaa', 'b', 'c', 'd', 'eee')!
insert t values ('aaa', 'b', 'c', 'd', 'eee')!
insert t values ('aaa', 'b', 'c', 'd', 'eee')!
insert t values ('aaa', 'b', 'c', 'd', 'eee')!
insert t values ('aaa', 'b', 'c', 'd', 'eee')!
insert t values ('aaa', 'b', 'c', 'd', 'eee')!
insert t values ('aaa', 'b', 'c', 'd', 'eee')!
insert t values ('aaa', 'b', 'c', 'd', 'eee')!
insert t values ('aaa', 'b', 'c', 'd', 'eee')!
insert t values ('aaa', 'b', 'c', 'd', 'eee')!
insert t values ('aaa1', 'b', 'c', 'd', 'eee')!
insert t values ('aaa1', 'b', 'c', 'd', 'eee')!
insert t values ('aaa1', 'b', 'c', 'd', 'eee')!
insert t values ('aaa1', 'b', 'c', 'd', 'eee')!
insert t values ('aaa1', 'b', 'c', 'd', 'eee')!
insert t values ('aaa1', 'b', 'c', 'd', 'eee')!
insert t values ('aaa1', 'b', 'c', 'd', 'eee')!
insert t values ('aaa1', 'b', 'c', 'd', 'eee')!
insert t values ('aaa1', 'b', 'c', 'd', 'eee')!
insert t values ('aaa1', 'b', 'c', 'd', 'eee')!
insert t values ('aaa1', 'b', 'c', 'd', 'eee')!
insert t values ('aaa1', 'b', 'c', 'd', 'eee')!
insert t values ('aaa1', 'b', 'c', 'd', 'eee')!
insert t values ('aaa1', 'b', 'c', 'd', 'eee')!
insert t values ('aaa1', 'b', 'c', 'd', 'eee')!
insert t values ('aaa1', 'b', 'c', 'd', 'eee')!
insert t values ('aaa2', 'b', 'c', 'd', 'eee')!
create index i1 on t (a,e)!
parsing !
select a,b,e from t order by a,e!
executing !
select a,b,e from t order by a,e!
1
pars_then_ex !
mfetch !
20

pars_then_ex !
mfetch !
20

pars_then_ex !
mfetch !
20

pars_then_ex !
mfetch prev!
2

pars_execute!
rollback release !
 * PTS 1103134 !
file connect ( kern test !
create table "" ( a char (8))!
create table x ("" char (7))!
create table t ( a char (8))!
insert t set a = 'aaa'!
insert t set a = 'bbb'!
declare "" cursor for 
select * from t !
fetch1 !
fetch1 ""!
describe ""!
close ""!
fetch !
fetch ""!
rollback release !
