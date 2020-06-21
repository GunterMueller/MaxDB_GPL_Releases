*   *ID* N99A1    DBCMD    changed on 1992-09-28-13.27.10 by ADMIN     *
file connect ( kern test !
create table tab ( spalte fixed(18,17) ) !
pars_then_ex !
data !
insert tab set spalte = :a !
n 18,17 1.23456789012345678

nodata !
pars_execute !
select * from tab !
fetch !
rollback work release !
