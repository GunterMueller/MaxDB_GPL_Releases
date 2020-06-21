*   *ID* N35GRO10 DBCMD    changed on 1992-09-28-13.26.22 by ADMIN     *
file connect ( kern test !
create table t (woche fixed (4) key, monat fixed (2))!
insert t values (5000, 12)!
insert t values ( 100,  4)!
create table t1 (a1 fixed(8)key)!
insert t1 set a1 = 100!
create table t2 (a2 fixed(8)key)!
insert t2 set a2 = 100!
create view v1 as select t1.*, t2.* from t1, t2
    where t1.a1 = t2.a2 !
file sel_table1 ( V1 !
declare mon cursor for 
select 100* trunc(woche/100) + monat jjmm, woche 
from t where woche > 2 for reuse!
fetch mon!
declare auf cursor for 
select jjmm, sum(trunc(a2)) aw, sum(trunc(a1)) rs
from v1 a, mon b
where a.a2 > 1 and a.a1 = b.woche
group by jjmm for reuse !
fetch auf!
declare auf1 cursor for 
select jjmm, sum(trunc(a2)) aw, sum(trunc(a1)) rs
from v1 a, mon b
where a.a1 = b.woche and a.a2 > 1
group by jjmm for reuse !
fetch auf1!
drop table t!
drop table t1!
drop table t2!
create table a (salary fixed (6,2))!
create view v (sal) as select sum(salary) from a !
insert a values (100)!
insert a values (500)!
select salary / sal from a, v !
select a.salary / sum(a1.salary) from a, a a1 !
select a.salary / sum(a1.salary) from a, a a1 group by a.salary!
fetch !
drop table a!
commit release !
* GROUP BY Refname sollte auch gehen !
file connect ( kern test !
create table t ( a char (8), b char (6))!
insert t set a = 'aaa', b = 'bbb'!
select a x from t 
   order by x !
select a x from t 
   group by a  !
select a x from t 
   group by x  !
select a x from t 
   group by x 
   order by x !
drop table t !
commit release !
