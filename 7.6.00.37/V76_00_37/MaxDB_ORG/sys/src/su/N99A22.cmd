*   *ID* N99A22   DBCMD    changed on 1992-09-28-13.27.17 by ADMIN     *
file connect ( kern test !
* Fehlerprotokoll 89 !
create table t ( a char (30), b fixed(18) )!
insert t set a = 'abcdefghijklmnopqrstuvwxyzabcd',
       b = 333333333333333333!
create table t1 ( b1 fixed(18))!
insert t1 set b1 = 333333333333333333 !
parsing !
select * from t, t1 where b = b1 !
pars_execute !
select * from t !
fetch1 !
executing !
select * !
1
pars_execute !
rollback release !
