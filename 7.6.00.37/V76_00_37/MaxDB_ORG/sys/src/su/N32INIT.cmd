*   *ID* N32INIT  DBCMD    changed on 1992-09-28-13.26.12 by ADMIN     *
create table tab ( spalte1 char(10) ,
                   spalte2 fixed(5,2),
                   spalte3 float(3),
                   spalte4 fixed(18) ) !
insert tab values ( 'a', 82.3 , -12E12, 300000 ) !
insert tab values ( 'b', 82.3 , 1e-36, -998877665432100000 ) !
insert tab values ( 'a', null , 3800000, 900000000000000000 ) !
insert tab values ( null, 200.01, null, -8200000 ) !
insert tab values ( 'a', 82.3, +1E-36 , 800000000000000000 ) !
insert tab values ( null, null, null, null ) !
insert tab values ( 'c', -723.1, -12E12, null ) !
insert tab values ( 'a', 82.3, +1E-36 , 800000000000000000 ) !
insert tab values ( 'b', null, null, + 900000000000000000 ) !
select * from tab !
fetch !
commit work !
