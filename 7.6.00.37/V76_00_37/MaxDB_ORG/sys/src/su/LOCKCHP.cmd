*   *ID* LOCKCHP  DBCMD    changed on 1992-09-28-13.24.32 by ADMIN     *
pars_execute !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
&1 !
pars_then_ex !
