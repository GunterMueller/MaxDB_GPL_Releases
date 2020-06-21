*   *ID* A16PARS1 DBCMD    changed on 1992-09-28-13.22.37 by ADMIN     *
file CONNECT ( KERN TEST !
create table tab7 ( spalte fixed(5) ) !
insert tab7 values ( 1 ) !
insert tab7 values ( 2 ) !
insert tab7 values ( 3 ) !
insert tab7 values ( 4 ) !
delete tab7 where spalte > 2 !
pars_execute !
select * from tab7 !
fetch !
 pars_then_ex !
delete tab7 !
pars_execute !
select * from tab7 !
fetch !
create table tab8 ( spalte fixed(5) ) !
insert tab8 values ( 1 ) !
insert tab8 values ( 2 ) !
insert tab8 values ( 3 ) !
insert tab8 values ( 4 ) !
 pars_then_ex !
update tab8 set spalte = 5 where spalte > 2 !
pars_execute !
select * from tab8 !
fetch !
 pars_then_ex !
update tab8 set spalte = 6 !
pars_execute !
select * from tab8 !
fetch !
create table tab9 ( spalte fixed(5) key ) !
insert tab9 values ( 1 ) !
insert tab9 values ( 2 ) !
insert tab9 values ( 3 ) !
insert tab9 values ( 4 ) !
pars_execute !
commit  work release !
