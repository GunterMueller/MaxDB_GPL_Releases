*   *ID* N99K60   DBCMD    changed on 1992-09-28-13.27.30 by ADMIN     *
file connect ( kern test !
create table tab8 ( spalte fixed(5) ) !
insert tab8 values ( 1 ) !
pars_then_ex !
update tab8 set spalte = 5 !
pars_execute !
rollback work release !
