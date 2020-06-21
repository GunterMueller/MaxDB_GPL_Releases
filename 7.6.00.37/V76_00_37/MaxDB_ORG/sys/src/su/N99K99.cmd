*   *ID* N99K99   DBCMD    changed on 1992-09-28-13.27.33 by ADMIN     *
file connect ( kern test !
create table tab13 ( spalte fixed(5) ) !
insert tab13 set spalte = 1 !
insert tab13 set spalte = 5 !
parsing !
delete tab13 where spalte between :untergrenze and :obergrenze !
executing !
data !
delete !
1
n 5 3 n 5 6

nodata !
parsing !
delete tab13 where spalte between :untergrenze and 1 !
data !
executing !
delete !
1
n 5 1

nodata !
pars_execute !
select * from tab13 !
fetch !
rollback work release !
