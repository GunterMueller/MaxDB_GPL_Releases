*   *ID* INVSEL3  DBCMD    changed on 1992-09-28-13.24.07 by ADMIN     *
declare erg cursor for select min(u1), max(u1) from u &1!
fetch erg!
declare erg cursor for select min(u3), max(u3) from u &1!
fetch erg!
declare erg cursor for select min(u4), max(u4) from u &1!
fetch erg!
declare erg cursor for select min(u1), max(u1), count(u1), count(distinct u1),
       sum(u1), sum(distinct u1), avg(u1), avg(distinct u1), count(*)
from u &1!
fetch erg!
declare erg cursor for select min(u3), max(u3), count(u3), count(distinct u3),
       sum(u3), sum(distinct u3), avg(u3), avg(distinct u3), count(*)
from u &1!
fetch erg!
declare erg cursor for select min(u4), max(u4), count(u4), count(distinct u4),
       sum(u4), sum(distinct u4), avg(u4), avg(distinct u4), count(*)
from u &1!
fetch erg!
declare erg cursor for select min(u1), max(u1) from u
where uk1 = 'a1' and uk2 = 3 &1!
fetch erg!
declare erg cursor for select min(u3), max(u3) from u
where uk1 = 'a2' and uk2 > 1 and u3 between 2 and 4 &1!
fetch erg!
declare erg cursor for select min(u4), max(u4) from u
where uk1 >= 'a' and uk2 < 5 and u4 > 111 &1!
fetch erg!
declare erg cursor for select min(u1), max(u1), count(u1), count(distinct u1),
       sum(u1), sum(distinct u1), avg(u1), avg(distinct u1), count(*)
from u where u1 > 12 and u3 < 4 and u4 between 100 and 600 &1!
fetch erg!
declare erg cursor for select min(u3), max(u3), count(u3), count(distinct u3),
       sum(u3), sum(distinct u3), avg(u3), avg(distinct u3), count(*)
from u where u1 <= 33.333 and u3 >= 2.22222 &1!
fetch erg!
declare erg cursor for select min(u4), max(u4), count(u4), count(distinct u4),
       sum(u4), sum(distinct u4), avg(u4), avg(distinct u4), count(*)
from u where u4 > 111 and uk1 > 'a' and uk2 > 2 &1!
fetch erg!
