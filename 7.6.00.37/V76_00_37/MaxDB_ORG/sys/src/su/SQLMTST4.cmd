*   *ID* SQLMTST4 DBCMD    changed on 1992-09-28-13.28.11 by ADMIN     *
pars_then_ex !
declare erg cursor for
     select * from t3
     union
     select * from t1
     &1
     select * from t2 !
fetch erg into :a !
pars_execute !
close erg !
*
pars_then_ex !
declare erg cursor for
     select * from t3
     union
     select * from t1
     &1 all
     select * from t2 !
fetch erg into :a !
pars_execute !
close erg !
*
