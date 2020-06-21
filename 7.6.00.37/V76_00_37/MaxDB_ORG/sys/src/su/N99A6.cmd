*   *ID* N99A6    DBCMD    changed on 1992-09-28-13.27.22 by ADMIN     *
file connect ( kern test !
create table t ( a char (6), b fixed(3), c fixed(3), d fixed(3) )!
insert t set a = 'aaa', b = 2, c = 2 , d = 2!
create index i1 on t (a, b, d)!
create index i2 on t (c, b )!
update t set d = d+3 where a = 'aaa' and b > 1!
select * from t !
fetch !
update t set d = d+3 where a = 'aaa' !
select * from t !
fetch !
update t set d = d+3 where a = 'aaa' and c >= 2 !
select * from t !
fetch !
rollback release !
