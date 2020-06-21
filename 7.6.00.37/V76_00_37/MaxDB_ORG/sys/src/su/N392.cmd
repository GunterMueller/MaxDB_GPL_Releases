*   *ID* N39SUB2  DBCMD    changed on 1992-09-28-13.26.28 by ADMIN     *
file CONNECT ( KERN TEST !
create table tab1 ( spalte1 fixed(5) key, spalte2 char(10) ) !
insert tab1 values ( 2, 'Hans' ) !
insert tab1 values ( 4,  null  ) !
insert tab1 values ( 8, 'Paul' ) !
create table tab2 ( col1 fixed(5) ) !
insert tab2 values ( 1 ) !
insert tab2 values ( 2 ) !
insert tab2 values ( 3 ) !
select * into :a, :b from tab1
   where spalte1 > all ( select * from tab2 ) and spalte1 < 7 !
create view v as
    select * from tab1 where spalte1 in (select spalte1 from tab1 )!
select * from v !
fetch !
pars_then_ex !
select spalte1, tab1.spalte2 from tab1 where spalte1 < 5
    and spalte1 > any ( select count ( spalte2 ) from v group
    by spalte2 ) and spalte1 < 10 !
fetch into :a, :b !
pars_execute !
select spalte1, tab1.spalte2 from tab1 where spalte1 < 5
    and spalte1 > any ( select count ( spalte2 ) from v group
    by spalte2 ) and spalte1 < 10 !
fetch !
rollback work release !
