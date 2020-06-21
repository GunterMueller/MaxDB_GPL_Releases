*   *ID* N35INIT  DBCMD    changed on 1992-09-28-13.26.22 by ADMIN     *
create table tab ( spalte1 char(10) ,
                   spalte2 fixed(5,2),
                   spalte3 float(3),
                   spalte4 fixed(18) ) !
insert tab values ( 'a', 100 , -12E12, 400000 ) !
insert tab values ( 'b', 100 , 1e-36, -700000000000000000 ) !
insert tab values ( 'a', null , 50, 900000000000000000 ) !
insert tab values ( null, 200, null, -800000 ) !
insert tab values ( 'a', 100, +1E-36 , 800000000000000000 ) !
insert tab values ( null, null, null, null ) !
insert tab values ( 'c', -600, -12E12, null ) !
insert tab values ( 'a', 100, +1E-36 , 800000000000000000 ) !
insert tab values ( 'b', null, null, + 900000000000000000 ) !
select * from tab !
fetch !
create index "tab.spalte2"
	on tab( spalte2 ) !
create index "tab.spalte4"
	on tab( spalte4 ) !
commit work !
