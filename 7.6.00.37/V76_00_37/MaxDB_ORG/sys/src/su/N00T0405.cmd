*   *ID* N00T0405 DBCMD    changed on 1992-09-28-13.24.48 by ADMIN     *
 ***  4.8.1  <rowno predicate> !
file connect ( kern test !
create table tab ( spalte fixed(5) ) !
insert tab values ( 3 ) !
insert tab values ( 4 ) !
insert tab values ( 7 ) !
insert tab values ( 1 ) !
insert tab values ( 9 ) !
insert tab values ( 3 ) !
select * from tab where spalte > 2 and rowno < 3 !
fetch !
select spalte, sum(spalte) from tab where ( spalte > 2 ) and rowno <= 3
   group by spalte order by 1 !
fetch !
rollback work release !
