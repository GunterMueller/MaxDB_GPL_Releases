*   *ID* N00T0810 DBCMD    changed on 1992-09-28-13.24.58 by ADMIN     *
 *** 8.15 <create index statement>
 *** 8.16 <drop index statement>   !
file connect ( kern test !
create table kern.tabelle1
   ( spalte1 char(10)  key ,
     spalte2 float (5 ) range in ( 1,2,3,4,5),
     spalte3 float (5) not null ) !
create index "kern.tabelle1.spalte2"
	on kern.tabelle1( spalte2 ) !
create unique index indexa on kern.tabelle1
   ( spalte2 asc, spalte3 desc, spalte1 ) !
create table tabelle2 ( column1 fixed(5,3),
                        column2 char(10) ascii,
                        column3 float(9) ) !
insert tabelle2 values ( 8.2, 'Hans', 88 ) !
insert tabelle2 values ( 8.2, 'Paul', 88 ) !
create index index21 on tabelle2 ( column2 ) !
create unique index index22 on kern.tabelle2
   ( column2 desc, column1 ) !
drop index index22 on tabelle2 !
insert tabelle2 values ( 8.2, 'Hans', 88 ) !
drop index "kern.tabelle1.spalte2" !
commit work release !
