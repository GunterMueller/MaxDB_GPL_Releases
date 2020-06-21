file connect ( kern test sqlmode &1 !
create table t ( a char (8) )!
insert into t values ('aaa')!
create table t1 (a1 char (8))!
insert into t1 values ('bbb')!
commit work !
* db2, oracle, sql_db !
lock table t in share mode !
lock table t in exclusive mode !
* oracle !
lock table t in share mode nowait !
lock table t in row share mode nowait !
lock table t in share update mode nowait !
lock table t in exclusive mode nowait !
lock table t in row exclusive mode nowait !
lock table t in share row exclusive mode nowait !
* sql-db !
lock (nowait) table t, t1 in share mode
              row t2 key a = 'aaa' in exclusive mode !
commit work release !
file connect (kern test !
drop table t  !
drop table t1 !
commit work release !
