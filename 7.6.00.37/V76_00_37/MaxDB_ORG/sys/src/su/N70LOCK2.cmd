*   *ID* N70LOCK2 DBCMD    changed on 1992-09-28-13.27.05 by ADMIN     *
connect kern identified by test isolation level  2 !
create table t1 ( t1xc1 fixed(5) key, t1xc2 fixed(5) ) !
insert t1 values ( 2,2 ) !
insert t1 values ( 3,3 ) !
commit work !
select * from t1 !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
commit work release !
connect kern identified by test isolation level  1 !
select * from t1 !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
commit work release !
connect kern identified by test isolation level  0 !
select * from t1 !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
commit work release !
connect kern identified by test isolation level  2 !
select * from t1 with lock isolation level 20 !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
commit work release !
connect kern identified by test isolation level  2 !
select * from t1 with lock exclusive !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
commit work release !
connect kern identified by test isolation level  1 !
select * from t1 with lock isolation level 20 !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
commit work release !
connect kern identified by test isolation level  1 !
select * from t1 with lock exclusive !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
commit work release !
connect kern identified by test isolation level  0 !
select * from t1 with lock isolation level 20 !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
commit work release !
connect kern identified by test isolation level  0 !
select * from t1 with lock exclusive !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
drop table t1 !
commit work release !
