*   *ID* N70LOCK1 DBCMD    changed on 1992-09-28-13.27.05 by ADMIN     *
connect kern identified by test isolation level  0 !
create table t ( a char (8) ascii key)!
insert t set a = 'aaa'!
insert t set a = 'bbb'!
insert t set a = 'ccc'!
commit !
select * from t !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
unlock row t key a = 'aaa'  in share mode !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
commit !
declare erg cursor for
select * from t for update !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
commit !
select * from t with lock isolation level 20 !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
commit !
select * from t with lock exclusive !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
commit release !
connect kern identified by test !
select * from t!
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
commit !
declare erg cursor for
select * from t for update!
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
commit !
declare erg cursor for
select * from t for update with lock exclusive !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
commit !
create table t1 ( a1 char (7) ascii)!
insert t1 set a1 = 'aaa'!
insert t1 set a1 = 'zzz'!
commit !
select * from t, t1 where a = a1 with lock exclusive !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
drop table t !
drop table t1 !
commit release !
