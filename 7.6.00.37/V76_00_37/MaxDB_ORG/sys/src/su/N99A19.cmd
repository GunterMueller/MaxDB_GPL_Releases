*   *ID* N99A19   DBCMD    changed on 1992-09-28-13.27.15 by ADMIN     *
file connect ( kern test !
create table t ( a fixed (5), b fixed (6))!
insert t values (2, 5)!
insert t values (3, 4)!
insert t values (4, 3)!
insert t values (5, 2) !
select * from t where a <= 3 and a = 3+1 !
fetch !
select * from t where a <= 3 and a = 3+1 order by b!
fetch !
pars_then_ex !
select * from t where a <= 3 and a = 3+1 !
fetch into :a, :b !
select * from t where a <= 3 and a = 3+1 order by b!
fetch into :a, :b !
data !
select * from t where a <= :a and a = :a+1 !
n 5 3 n 38 3

nodata !
fetch into :a, :b !
data !
select * from t where a <= :a and a = :a+1 order by b !
n 5 3 n 38 3

nodata !
fetch into :a, :b !
pars_execute !
drop table t !
commit release !
