*   *ID* N99K2    DBCMD    changed on 1992-09-28-13.27.27 by ADMIN     *
file connect ( kern test !
create table tab1 ( spalte1 fixed(5) , spalte2 fixed(5) ) !
create table tab2 ( column1 fixed(5) , column2 fixed(5) ) !
insert tab2 values ( 1,1 ) !
insert tab1 ( spalte2 ) select column2 from tab2 !
select * from tab1 !
fetch !
rollback work release !
