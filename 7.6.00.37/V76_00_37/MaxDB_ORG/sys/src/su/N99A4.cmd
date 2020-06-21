*   *ID* N99A4    DBCMD    changed on 1992-09-28-13.27.21 by ADMIN     *
file connect ( kern test !
create table tab1 ( spalte fixed(5) ) !
create table tab2 ( col fixed(5) ) !
insert tab1 values ( 2) !
select * from tab1 where spalte < ( select * from tab2 ) !
select * from tab1 where spalte < ( select * from tab2 ) or spalte = 2 !
insert tab2 values ( 3 ) !
select * from tab1 where spalte < ( select * from tab2 where col < 3 ) !
select * from tab1 where not ( spalte <
   ( select * from tab2 where col < 3 )
   and spalte < 1 ) !
select * from tab1 where not ( spalte <
   ( select * from tab2 where col < 3 ))  !
rollback work release !
