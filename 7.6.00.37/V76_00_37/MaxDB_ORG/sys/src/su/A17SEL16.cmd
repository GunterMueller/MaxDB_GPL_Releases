*   *ID* A17SEL16 DBCMD    changed on 1992-09-28-13.22.43 by ADMIN     *
file connect ( kern test !
create table t ( a char(5))!
create table t1 (b char (7) ascii key)!
insert t set a = 'aaa'!
insert t set a = 'bbb'!
insert t set a = 'ccc'!
insert t set a = 'ddd'!
insert t1 set b = 'aaa'!
insert t1 set b = 'ccc'!
select * from t, t1 where t.a = t1.b!
fetch!
select * from t, t1 where t1.b = t.a!
fetch!
create table t2 (b char (7) ascii key,
                 c fixed (8) key, d char (7) ascii)!
insert t2 values ('bbb', 4, 'bbb')!
insert t2 values ('bbb', 9, 'ddd')!
insert t2 values ('ccc', 9, 'eee')!
insert t2 values ('ddd', 9, 'ddd')!
select * from t, t2 where t.a = t2.b!
fetch !
select * from t, t2 where t2.b = t.a!
fetch !
select * from t, t2 where t.a = t2.d!
fetch !
select * from t, t2 where t2.d = t.a!
fetch !
create index "t2.d"
	on t2( d ) !
select * from t, t2 where t.a = t2.d!
fetch !
select * from t, t2 where t2.d = t.a!
fetch !
drop table t !
drop table t1 !
drop table t2 !
commit release !
