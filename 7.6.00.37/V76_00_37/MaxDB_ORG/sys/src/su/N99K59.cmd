*   *ID* N99K59   DBCMD    changed on 1992-09-28-13.27.30 by ADMIN     *
file connect ( kern test !
create table tab7 ( spalte fixed(5) ) !
insert tab7 values ( 1 ) !
pars_then_ex !
delete tab7 !
pars_execute !
rollback work release !
