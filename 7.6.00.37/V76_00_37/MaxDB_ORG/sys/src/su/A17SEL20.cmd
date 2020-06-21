*   *ID* A17SEL20 DBCMD    changed on 1992-09-28-13.22.45 by ADMIN     *
file connect ( sut sut !
create user u1 password u1 dba !
create user public password publ !
commit release !
file connect ( kern test !
create user u password u resource !
create table t ( a char (8) key,
  b char (8))!
insert t set a = 'aaa', b = 'bbb'!
grant all on t to u, u1 !
commit release !
file connect ( u u !
create table t ( a char (8) key)!
insert t set a = 'aaa'!
select * from t, kern.t where t.a = 'aaa'!
select * from t, kern.t where t.b = 'bbb'!
rollback !
create table t ( a char (8) key,
  b char (8))!
insert t set a = 'aaa', b = 'bbb'!
select * from t, kern.t where t.a = 'aaa'!
select * from t, kern.t where kern.t.a = 'aaa'!
fetch !
select * from t, kern.t where superdba.t.a = 'aaa'!
fetch !
grant all on t to u1 !
commit release !
file connect ( u1 u1 !
create table t ( a2 char (8))!
insert t set a2 = 'aaa'!
select * from t, u.t where u1.t.a2 = 'aaa'!
select * from t, u.t where u.t.a = 'aaa'!
fetch!
select * from t , u.t where u1.t.a2 = 'aaa'!
fetch!
select * from t , kern.t where t.a2 = 'aaa'!
fetch!
select * from t , kern.t where kern.t.a = 'aaa'!
fetch!
select * from t , kern.t where u1.t.a2 = 'aaa'!
fetch!
drop table t !
create view t as select a from  u.t !
commit release !
file connect ( sut sut !
drop user u1 !
commit release !
file connect ( kern test !
drop table t !
drop user u !
commit release !
file connect ( kern test !
create table t1 (a int)!
insert t1 values (1)!
insert t1 values (2)!
create table t2 (b int)!
insert t2 values (4)!
insert t2 values (5)!
select t1.a as b, t2.b as a from t1,t2 order by b!
fetch!
select t1.a as b, t2.b as a from t1,t2 order by a!
fetch!
rollback release!
