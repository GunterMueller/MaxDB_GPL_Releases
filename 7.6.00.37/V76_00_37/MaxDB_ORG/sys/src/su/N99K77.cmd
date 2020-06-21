*   *ID* N99K77   DBCMD    changed on 1992-09-28-13.27.32 by ADMIN     *
file connect ( kern test !
create table tab9 ( spalte fixed(5) ) !
insert tab9 values (1) !
pars_then_ex !
select spalte into :spalte from tab9 where spalte = 1 !
pars_execute !
rollback work release !
