*   *ID* N00T0403 DBCMD    changed on 1992-09-28-13.24.47 by ADMIN     *
 ***  4.1.6  <lock option> !
connect kern identified by test isolation level  0 !
create table tab ( spalte1 fixed(5) key, spalte2 char(5) ) !
insert tab values ( 3, 'aaa' ) !
insert tab values ( 5, null  ) !
insert tab values ( 2, 'fff' ) !
insert tab values ( 4, 'ddd' ) !
insert tab values ( 1, 'bbb' ) !
insert tab values ( 6, 'aaa' ) !
select * from tab where spalte2 is null with lock isolation level 20 !
fetch !
select sum(spalte1) from tab where spalte2 < 'c' group by spalte2
   order by 1 with lock (nowait) !
fetch !
declare erg1 cursor for
select spalte2 from tab where spalte1 = 2
   with lock (nowait) exclusive for reuse !
declare erg2 cursor for
select * from tab where spalte2 > 'bbb' for update
   with lock (nowait) !
fetch erg2 !
rollback work release !
