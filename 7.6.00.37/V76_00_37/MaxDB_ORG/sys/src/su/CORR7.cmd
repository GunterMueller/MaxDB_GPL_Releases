*   *ID* CORR7    DBCMD    changed on 1992-09-28-13.21.30 by ADMIN     *
file connect ( kern test !
* Fehlerprotokoll 235 !
create table t ( a char (8), long1 char (60), long2 char (70))!
insert t values ('aaa', 'long1', 'long2')!
create table t1 (a1 char (8))!
insert t1 set a1 = 'aaa'!
select * from t where exists (select * from t1 where t.a = t1.a1)!
fetch!
select a, long2 from t where exists (select * from t1 where t.a = t1.a1)!
fetch!
create table u ( a char (8), b char (6), c char (7))!
insert u values ('aaa', 'long1', 'long2')!
insert u values (null, 'long3', 'long4')!
create index "u.a"
	on u( a ) !
select * from u x where length(a) >
(select count(b) from u where x.a = a)!
fetch!
drop table t !
drop table u !
drop table t1 !
commit release !
