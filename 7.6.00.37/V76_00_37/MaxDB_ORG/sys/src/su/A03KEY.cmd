*   *ID* A03KEY   DBCMD    changed on 1992-09-28-13.22.21 by ADMIN     *
file connect ( kern test !
create table kern.tab1
   ( c1 char(15) key, c2 fixed(5) key, c3 fixed(5) ) !
insert kern.tab1 values ( null, null , null ) !
insert kern.tab1 values ( 'Hans', null, 1 ) !
insert kern.tab1 values ( 'Hans', null, 1 ) !
insert kern.tab1 set c1 = null, c2 = 1, c3 = 2 !
drop table tab1 !
commit work release !
