*   *ID* INVSEL4  DBCMD    changed on 1992-09-28-13.24.08 by ADMIN     *
select min(u1), max(u1), min(u2), max(u2), min(u3),
       max(u3), min(u4), max(u4), min(u5), max(u5),
       min(uk1), max(uk1),min(uk2), max(uk2)
from u !
fetch!
select count(u1), count(distinct u1),
       count(u2), count(distinct u2),
       count(u3), count(distinct u3),
       count(u4), count(distinct u4),
       count(u5), count(distinct u5),
       count(uk1), count(distinct uk1),
       count(uk2), count(distinct uk2)
from u !
fetch!
select sum(u1), sum(distinct u1),
       sum(u3), sum(distinct u3),
       sum(u4), sum(distinct u4),
       sum(uk2), sum(distinct uk2)
from u !
fetch!
select avg(u1), avg(distinct u1),
       avg(u3), avg(distinct u3),
       avg(u4), avg(distinct u4),
       avg(uk2), avg(distinct uk2)
from u !
fetch!
select min(u1) + max(u1) + sum(u1) + avg(u1) + count(u1) +
       min(u3) + max(u3) + sum(u3) + avg(u3) + count(u3) +
       min(u4) + max(u4) + sum(u4) + avg(u4) + count(u4) + count(*)
from u !
fetch!
select min(u1) + max(u1) + sum(u1) + avg(u1) + count(u1) +
       min(u3) + max(u3) + sum(u3) + avg(u3) + count(u3) +
       min(u4) + max(u4) + sum(u4) + avg(u4) + count(u4) + count(*),
       count(*)
from u !
fetch!
