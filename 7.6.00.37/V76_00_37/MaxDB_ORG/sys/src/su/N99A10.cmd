*   *ID* N99A10   DBCMD    changed on 1992-09-28-13.27.11 by ADMIN     *
file connect ( kern test !
create table tab ( spalte1 fixed(5), spalte2 fixed(5) ) !
insert tab values ( 1, 1 ) !
insert tab values ( 2, 2 ) !
select spalte1 from tab order by spalte2 !
select distinct(spalte1) from tab order by spalte2 !
select sum(spalte1) from tab order by spalte2 !
select sum(spalte1) from tab order by spalte1 !
create view v(a,b) as select sum( spalte1), sum(spalte2) from tab !
select a from v !
select a from v order by a !
select a from v order by b !
select distinct a from v order by b !
rollback work release !
