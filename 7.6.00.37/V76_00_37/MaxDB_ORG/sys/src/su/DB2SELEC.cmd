*   *ID* DB2SELEC DBCMD    changed on 1992-09-28-13.23.18 by ADMIN     *
file connect ( kern test sqlmode db2 !
create table t ( a char (8), b decimal (8))!
insert into t values ('aaa', 12)!
insert into t values ('mmm', 0 )!
commit !
declare c cursor for select * from t !
fetch1 c into :a, :b !
update t set a = 'new' where current of c !
close c !
rollback !
declare c cursor for select * from t for fetch only !
fetch1 c into :a, :b !
update t set a = 'new' where current of c !
close c !
rollback !
declare c cursor for select * from t for update of a for fetch only !
declare c cursor for select * from t optimize for 12 rows!
fetch c into :a, :b !
close c !
declare c cursor for select * from t optimize for 12 row for fetch only!
fetch c into :a, :b !
close c !
declare c cursor for select * from t for fetch only optimize for 12 row !
fetch c into :a, :b !
close c !
declare c cursor for select * from t order by a
     for fetch only optimize for 12 row !
fetch c into :a, :b !
close c !
declare c cursor for select * from t
     for fetch only order by b optimize for 12 row !
fetch c into :a, :b !
close c !
declare c cursor for select * from t order by a for update of a !
rollback release !
file connect ( kern test !
drop table t !
commit release !
