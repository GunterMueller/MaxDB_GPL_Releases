*   *ID* N99A20   DBCMD    changed on 1992-09-28-13.27.16 by ADMIN     *
file connect ( kern test !
create table t (s1 fixed (5), s2 fixed(5))!
insert t values (1,1)!
insert t values (2,2)!
select s1 from t order by s2 !
fetch !
select distinct(s1) from t order by s2 !
fetch !
select sum(s1) from t order by s2 !
fetch !
select sum(s1) from t order by s1 !
fetch !
select sum(s1), s2 from t !
fetch !
select sum(s1), s2 from t group by s2 !
fetch !
select sum(s1), s2 from t group by s2 order by s2 !
fetch !
create view v (a,b) as select sum(s1), sum(s2) from t!
select a from v!
fetch!
select a from v order by a !
fetch !
create view v1 as select distinct s1, s2 from t!
create table t1 ( a fixed (6))!
insert t1 set a = 3 !
select * from v1, t1 !
fetch !
select distinct * from v1, t1 !
fetch !
create view v2 as select distinct * from v1, t1 !
select * from v2, t1 !
fetch !
select distinct * from v2, t1 !
fetch !
drop table t !
drop table t1 !
commit release !
