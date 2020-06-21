*   *ID* N00T0604 DBCMD    changed on 1992-09-28-13.24.53 by ADMIN     *
 ***  6.4  <unlock statement> !
connect kern identified by test isolation level  2 !
create table tab ( spalte1 fixed(5) key, spalte2 char(5) ) !
insert tab values ( 1, 'aaa' ) !
insert tab values ( 2, 'bbb' ) !
insert tab values ( 3, 'ccc' ) !
insert tab values ( 4, 'ddd' ) !
commit !
select * from tab where spalte1 = 1 with lock (nowait)
   isolation level 30 !
fetch !
select * from sysdd.locks where tablename <> 'DUAL'!
fetch !
unlock table tab in share mode !
select * from sysdd.locks where tablename <> 'DUAL'!
fetch !
select * from tab where spalte1 = 4 !
select * from sysdd.locks where tablename <> 'DUAL'!
fetch !
unlock row tab key spalte1 = 4 in share mode !
select * from sysdd.locks where tablename <> 'DUAL'!
fetch !
select * from sysdd.locks where tablename <> 'DUAL'!
fetch !
unlock row tab key spalte1 = 2 row tab key spalte1 = 3 in exclusive mode!
select * from sysdd.locks where tablename <> 'DUAL'!
fetch !
select * from sysdd.locks where tablename <> 'DUAL'!
fetch !
unlock row tab key spalte1 = 1 in share mode !
unlock row tab key spalte1 = 2 row tab key spalte1 = 3 in exclusive mode!
select * from sysdd.locks where tablename <> 'DUAL'!
fetch !
create user thomasz password th !
grant select on tab to thomasz !
commit release !
connect thomasz identified by th !
lock row tab key spalte1 = 1 in share mode !
unlock row tab key spalte1 = 1 in exclusive mode !
unlock row tab key spalte1 = 1 in share mode !
commit release !
file connect ( kern test !
drop user thomasz !
drop table tab !
commit work release !
