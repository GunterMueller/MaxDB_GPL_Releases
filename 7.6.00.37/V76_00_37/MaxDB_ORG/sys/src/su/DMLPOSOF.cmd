* Testet ADIS 929 POS OF <resname> IS ... !
file connect ( kern test !
create table ref ( refk char (8) key)!
insert ref values ('aaa')!
insert ref values ('bbb')!
insert ref values ('ccc')!
insert ref values ('ddd')!
insert ref values ('eee')!
insert ref values ('fff')!
insert ref values ('ggg')!
insert ref values ('hhh')!
create table t (a char (8), k char (12) primary key) !
insert t set a = 'aaa', k = 'ka'!
insert t set a = 'bbb', k = 'kb'!
insert t set a = 'ccc', k = 'kc'!
insert t set a = 'ddd', k = 'kd'!
insert t set a = 'eee', k = 'ke'!
insert t set a = 'fff', k = 'kf'!
insert t set a = 'ggg', k = 'kg'!
insert t set a = 'hhh', k = 'kh'!
create unique index i on t (a) !
create table t1 as select * from t!
create table t2 (primary key (k,a)) as select * from t !
commit release !
connect kern identified by test isolation level  1 !
declare erg cursor for select k from t for update !
select LOCK_MODE, substr(TABLENAME,1,10), substr(ROWID, 1, 12)
 from sysdd.locks where tablename = 'T'!
fetch!
select LOCK_MODE, substr(TABLENAME,1,10), substr(ROWID, 1, 12)
 from sysdd.locks where tablename = 'T'!
fetch!
select LOCK_MODE, substr(TABLENAME,1,10), substr(ROWID, 1, 12)
 from sysdd.locks where tablename = 'T'!
fetch!
rollback !
declare erg cursor for select k from t for update !
select LOCK_MODE, substr(TABLENAME,1,10), substr(ROWID, 1, 12)
 from sysdd.locks where tablename = 'T'!
fetch!
lock 
 table ref in share mode 
 row t pos of erg is 3
 row t pos of erg is 7
 row ref key refk = 'eee' in exclusive mode !
select LOCK_MODE, substr(TABLENAME,1,10), substr(ROWID, 1, 12)
 from sysdd.locks !
fetch!
rollback !
declare erg cursor for select k from t for update !
declare res cursor for select k from t for update !
select LOCK_MODE, substr(TABLENAME,1,10), substr(ROWID, 1, 12)
 from sysdd.locks where tablename = 'T'!
fetch!
pars_then_ex !
data !
lock 
 table ref in share mode 
 row t pos of erg is :a 
 row t pos of res is :b
 row ref key refk = :k in exclusive mode !
n 10 3
n 10 7
c 8 'eee'

nodata!
pars_execute!
select LOCK_MODE, substr(TABLENAME,1,10), substr(ROWID, 1, 12)
 from sysdd.locks !
fetch!
rollback release !
file connect ( kern test !
declare erg cursor for select k from t for update!
fetch1 erg !
select * from t!
fetch !
pars_then_ex !
data !
* ADIS 1000840/1 !
update t set a = (select '12345' from t where a = :e)
 where current of erg!
c 8 'eee'

nodata !
pars_execute!
* ADIS 1000842/3 !
update t set a = (select '98765' from t1 where t.a = t1.a)
 key k = 'kb' !
select * from t !
fetch!
update t2 set a = (select '98765' from t1 where t2.a = t1.a)
 key k = 'ka' , a = 'aaa'!
select * from t2 !
fetch!
fetch1 erg !
fetch1 erg !
update t set a = (select '98765' from t1 where t.a = t1.a)
 where current of erg !
select * from t !
fetch !
rollback !
diagnose share parse drop !
alter table t foreign key (a) references ref !
declare erg cursor for select k from t for update!
fetch1 erg !
update t set a = (select '98765' from t1 where t1.a = 'eee')
 where current of erg !
insert ref values ('98765') !
update t set a = (select '98765' from t1 where t1.a = 'eee')
 where current of erg !
select * from t !
fetch !
rollback !
* WHERE POS OF !
select * from t !
fetch!
diagnose share parse drop !
declare erg cursor for select k from t for update!
fetch1 erg !
update t set a = 'neuer' 
 where pos of erg is 3 !
select * from t !
fetch!
pars_then_ex !
data !
update t set a = :a 
 where pos of erg is :cnt !
c 8 'ganz neu'
n 10 2

pars_execute !
nodata !
select * from t !
fetch!
pars_then_ex !
data !
update t set a = (select '12345' from t where a = :e)
 where pos of erg is :cnt!
c 8 'eee'
n 10 5

pars_execute !
nodata !
select * from t !
fetch!
update t set a = (select '98765' from t1 where t.a = t1.a)
 where pos of erg is 6!
select * from t !
fetch!
update t set a = (select '98765' from t1 where t.a = t1.a)
 where current of erg !
select * from t !
fetch!
delete t where pos of erg is 7 !
select * from t !
fetch!
pars_then_ex !
data !
delete t where pos of erg is :cnt !
n 10 3

pars_execute !
nodata !
select * from t !
fetch!
rollback !
alter table t foreign key (a) references ref !
declare erg cursor for select k from t for update for reuse!
pars_then_ex !
data !
update t set a = (select '98765' from t1 where t1.a = 'eee')
 where pos of erg is :a!
n 10 6

insert ref values ('98765') !

update t set a = (select '98765' from t1 where t1.a = 'eee')
 where pos of erg is :a!
n 10 6

pars_execute !
nodata!
select * from t !
fetch !
rollback !
diagnose share parse drop !
alter table ref foreign key (refk) references t (a) 
   on delete cascade !
declare erg cursor for select k from t for update!
select * from ref !
fetch!
pars_then_ex !
data !
delete t where pos of erg is :cnt !
n 10 3

pars_execute !
nodata !
select * from t !
fetch!
select * from ref !
fetch !
drop table t !
drop table t1 !
drop table t2 !
drop table ref !
commit release !
* jetzt gucken wir das mal mit updatebarer Joinview an !
* Joinviews können (noch) keine Subqueries in Set-Klausel !
* (März 98) !
file connect ( kern test !
create table t1 (a1 int key, a2 int, a3 int) !
create table t2 (b1 int key, b2 int key, b3 int) !
alter table t1 foreign key l (a2, a3) references t2 !
create view v1 as select a1, a2, a3, b3 from t1, t2 where a2 = b1 and
               a3 = b2 with check option !
commit !
insert v1 values (1,2,3,4) !
insert v1 values (2,3,3,5) !
commit !
update v1 set a1 = (select 8765 from t1 where v1.a1 = t1.a1)
 key a1 = 1 !
select * from v1 !
fetch !
rollback !
declare erg cursor for select * from v1 for update !
fetch1 erg !
update v1 set a1 = (select 8765 from t1 where v1.a1 = t1.a1)
 where current of erg !
select * from v1 !
fetch !
rollback !
declare erg cursor for select * from v1 for update !
fetch1 erg !
update v1 set a1 = (select 8765 from t1 where t1.a1 = 1)
 where current of erg !
select * from v1 !
fetch !
rollback !
declare erg cursor for select * from v1 for update !
fetch1 erg !
pars_then_ex !
data !
update v1 set a1 = :a
 where current of erg!
n 10 369

pars_execute !
nodata !
select * from v1 !
fetch !
rollback !
declare erg cursor for select * from v1 for update !
pars_then_ex !
data !
update v1 set a1 = :a
 where pos of erg is 1!
n 10 9630

pars_execute !
nodata !
select * from v1 !
fetch !
rollback !
create table ref ( refa int key)!
insert ref set refa = 1 !
insert ref set refa = 2 !
diagnose share parse drop !
alter table t1 foreign key fk (a1) references ref !
declare erg cursor for select * from v1 for update !
pars_then_ex !
data !
update v1 set a1 = :a
 where pos of erg is 1!
n 10 9630

pars_execute !
nodata !
insert ref set refa = 9630 !
pars_then_ex !
data !
update v1 set a1 = :a
 where pos of erg is 1!
n 10 9630

pars_execute !
nodata !
select * from v1 !
fetch !
rollback !
declare erg cursor for select * from v1 for update !
update v1 set a1 = (select 8765 from t1 where t1.a1 = 1)
 where pos of erg is 1!
select * from v1 !
fetch !
rollback !
declare erg cursor for select * from v1 for update !
pars_then_ex !
data!
delete v1 
 where pos of erg is :a !
n 10 2

pars_execute!
nodata!
select * from v1 !
fetch !
rollback !
create table ref (refa int key references t1) !
insert ref set refa = 2 !
diagnose share parse drop !
declare erg cursor for select * from v1 for update !
pars_then_ex !
data!
delete v1 
 where pos of erg is :a !
n 10 2

pars_execute!
nodata!
select * from v1 !
fetch !
select * from ref !
fetch!
rollback !
drop table t1 !
drop table t2 !
commit work release !
* PTS 1113924-6 !
file connect ( kern test !
create table err9005_1(
  fieldchar1 varchar(10) not null,
  fieldchar2 varchar(20),
  fieldnum1 smallint,
  fieldnum2 smallint,
  fieldchar3 long ascii,
  fieldbin1 long byte,
  fieldbin2 long byte,
  fieldbin3 long byte,
  primary key(fieldchar1)) !
insert into err9005_1(fieldchar1,fieldchar2,fieldnum1, fieldnum2) 
  values ('one','row one',1,1) !
insert into err9005_1(fieldchar1,fieldchar2,fieldnum1, fieldnum2)
  values ('two','row two',2,2) !
insert into err9005_1(fieldchar1,fieldchar2,fieldnum1, fieldnum2)
  values ('three','row three',3,3) !
create table err9005_2(
  fieldchar0 varchar(10) not null,
  fieldchar1 varchar(10) not null,
  fieldchar2 varchar(20),
  fieldnum1 smallint,
  fieldnum2 smallint,
  fieldchar3 long ascii,
  fieldbin1 long byte,
  fieldbin2 long byte,
  fieldbin3 long byte,
  primary key(fieldchar0,fieldchar1),
   foreign key(fieldchar0) references err9005_1(fieldchar1)) !
insert into err9005_2(fieldchar0,fieldchar1,fieldchar2,fieldnum1, fieldnum2)
   values ('one','oneone','row one',1,1) !
insert into err9005_2(fieldchar0,fieldchar1,fieldchar2,fieldnum1, fieldnum2)
  values ('two','twotwo','row two',2,2) !
insert into err9005_2(fieldchar0,fieldchar1,fieldchar2,fieldnum1, fieldnum2)
  values ('three','threethree','row three',3,3) !
select fieldchar1,fieldchar2 from err9005_1 !
fetch !
declare mycursor cursor for
  select fieldchar1,fieldchar2 ,fieldnum1,fieldnum2 from err9005_1
      for update for reuse !
commit !
fetch1 mycursor !
pars_then_ex !
update err9005_1 set fieldchar1 = 'three', fieldchar2 = 'newval'
  where current of mycursor !
pars_execute!
update err9005_1 set fieldchar1 = "three", fieldchar2 = 'newval'
  where pos of mycursor is 2 !
update err9005_1 set fieldchar1 = 'three', fieldchar2 = 'newval'
  where pos of mycursor is 2 !
pars_execute !
select fieldchar1,fieldchar2 from err9005_1 !
fetch !
rollback !
pars_then_ex !
update err9005_1 set fieldchar1 = "three", fieldchar2 = 'newval'
  where pos of mycursor is 2 !
update err9005_1 set fieldchar1 = 'three', fieldchar2 = 'newval'
  where pos of mycursor is 2 !
pars_execute !
select fieldchar1,fieldchar2 from err9005_1 !
fetch !
drop table err9005_1 !
drop table err9005_2 !
commit release !
* PTS 1114511 - 13 !
file connect ( kern test !
create table t ( pos integer)!
insert t values (1)!
update t set pos = 7 where pos = 1!
select * into :a from t !
rollback release !
