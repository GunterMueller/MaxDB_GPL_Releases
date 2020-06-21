*   *ID* SQLMTST3 DBCMD    changed on 1992-09-28-13.28.10 by ADMIN     *
file connect (kern test sqlmode &1 !
file sqlmtst4 ( union     !
file sqlmtst4 ( except    !
file sqlmtst4 ( minus     !
file sqlmtst4 ( intersect !
*
pars_then_ex !
declare erg cursor for
select * from t1, t2 where a (+) = b     !
pars_execute !
close erg !
*
pars_then_ex !
declare erg cursor for
select * from t1, t2 where a     = b (+) !
pars_execute !
close erg !
*
pars_then_ex !
declare erg cursor for
select * from t1, t2 where a (+) = b (+) !
pars_execute !
close erg !
*
pars_then_ex !
declare erg cursor for
select * from (select * from t1), t2 where a = b !
pars_execute !
close erg !
*
rollback work release !
