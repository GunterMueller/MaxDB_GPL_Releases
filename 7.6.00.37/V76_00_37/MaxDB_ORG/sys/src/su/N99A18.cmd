*   *ID* N99A18   DBCMD    changed on 1992-09-28-13.27.14 by ADMIN     *
file connect ( kern test !
CREATE TABLE T1 (T FIXED(6,2))!
CREATE TABLE T2 (T FIXED(6,2))!
CREATE VIEW  V1 (V) AS SELECT SUM(T) FROM T1!
CREATE VIEW  V2 (V) AS SELECT FIXED(SUM(T),6,2) FROM T1!
COMMIT WORK!
INSERT T1 SET T = 1!
INSERT T1 SET T = 2!
INSERT T1 SET T = 3!
COMMIT WORK!
INSERT T2 SET T = 1!
INSERT T2 SET T = 2!
INSERT T2 SET T = 3!
COMMIT WORK!
*************************
select v+3 from v1 !
fetch !
select v from v1 order by v !
fetch!
select max(v) from v1 !
fetch !
create table x ( a fixed (4), b fixed (7))!
create view vx as select sum(a), b from x !
create view vx as select sum(a) vx1, b from x group by b !
insert x values (3, 1)!
insert x values (4, 1)!
update vx set b = vx1 !
drop table t1 !
drop table t2 !
drop table x !
commit release !
