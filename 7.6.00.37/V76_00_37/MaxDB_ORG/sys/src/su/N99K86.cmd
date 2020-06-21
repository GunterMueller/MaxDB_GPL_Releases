*   *ID* N99K86   DBCMD    changed on 1992-09-28-13.27.32 by ADMIN     *
file connect ( kern test !
create table tab12 ( spalte char(10) ) !
insert tab12 values ( 'Hans' ) !
parsing !
update tab12 set spalte = 'Otto' where spalte like :a1 !
data !
executing !
update !
1
C 10 'Ha%'

nodata !
pars_execute !
select * from tab12 !
fetch !
rollback work release !
