*   *ID* N70LOCK3 DBCMD    changed on 1992-09-28-13.27.06 by ADMIN     *
connect kern identified by test !
create table t1 ( spalte1 fixed(5) key, spalte2 fixed(5) ) !
insert t1 values ( 2,2 ) !
insert t1 values ( 3,3 ) !
commit work !
select * from t1 where spalte1 = 2 with lock exclusive !
fetch !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
unlock row t1 key spalte1 = 2 in share mode !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
update t1 set spalte2 = 5 where spalte1 = 3 !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
unlock row t1 key spalte1 = 3 in exclusive mode !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
unlock row t1 key spalte1 = 2 in exclusive mode !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
commit work release !
connect kern identified by test isolation level  0 !
lock row t1 key spalte1 = 2 in exclusive mode !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
unlock row t1 key spalte1 = 2 in share mode !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
declare erg1 cursor for
select spalte1 from t1 where spalte1 = 3 for update for reuse !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
fetch erg1 !
unlock row t1 key spalte1 = 3 in share mode !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
commit work release !
connect kern identified by test isolation level  2 !
delete t1 !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
select * from t1 !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
rollback work !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
delete t1 where spalte1 = 3 !
unlock row t1 key spalte1 = 3 in share mode !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
rollback work !
lock row t1 key spalte1 = 2 in exclusive mode !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
commit work !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
unlock row t1 key spalte1 = 2 in share mode !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
rollback work !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
drop table t1 !
commit work release !
