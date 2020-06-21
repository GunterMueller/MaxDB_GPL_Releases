*   *ID* VIEW2    DBCMD    changed on 1992-09-28-13.28.57 by ADMIN     *
file connect ( kern test !
drop table t !
drop table t1 !
create table t ( a fixed(4))!
insert t set a = 3!
create table t1 ( b fixed (5))!
insert t1 set b = 8!
insert t1 set b = 4!
create view v1 as select * from t1 !
select * from v1 order by b !
fetch !
drop view v1 !
select rowno, a, b from t, t1!
create view v1 (a, b, r) as select a, b, rowno from t, t1!
drop view v1!
create view v1 (a, b) as select a, b from t, t1 where rowno < 5!
drop view v1!
create view v1 (a, b, x) as select a, b, a+b from t, t1 where a+b > 6!
select * from v1 where x > 10!
fetch !
select * from v1 where a < x!
fetch !
select * from v1 where a = 3 and x > 10!
fetch!
select * from v1 where b = 4 or x > 10 !
fetch !
select * from v1 where not (x <= 10)!
fetch !
select * from v1 where a = 3 and (b = 4 or x > 10)!
fetch !
select x from v1 !
fetch !
select avg(x) from v1!
fetch !
select avg(x), b from v1 group by b !
fetch!
select avg(x), b from v1 group by b order by 1!
fetch!
select sum(x), b from v1 where x > 6 group by b having avg(a) > 0!
fetch!
select min (x) from v1 group by b having avg(x) > 8!
fetch!
select min (x) from v1 group by b having avg(a) > 2 order by 1!
fetch !
insert t1 set b = 5!
drop view v1 !
create view v1 (a, b, x) as select a, b, a+b from t, t1 where a+b > 7 !
select * from v1 where x > 10!
fetch !
select * from v1 where a < x!
fetch !
select * from v1 where a = 3 and x > 10!
fetch!
select * from v1 where b = 4 or x > 10 !
fetch !
select * from v1 where not (x <= 10)!
fetch !
select * from v1 where a = 3 and (b = 4 or x > 10)!
fetch !
select x from v1 !
fetch !
select avg(x) from v1!
fetch !
select avg(x), b from v1 group by b !
fetch!
select avg(x), b from v1 group by b order by 1!
fetch!
select sum(x), b from v1 where x > 6 group by b having avg(a) > 0!
fetch!
select min (x) from v1 group by b having avg(x) > 8!
fetch!
select min (x) from v1 group by b having avg(a) > 2 order by 1!
fetch !
drop table t!
drop table t1 !
commit release !
