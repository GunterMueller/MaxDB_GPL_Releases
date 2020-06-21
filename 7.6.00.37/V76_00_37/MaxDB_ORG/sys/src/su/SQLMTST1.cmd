*   *ID* SQLMTST1 DBCMD    changed on 1992-09-28-13.28.10 by ADMIN     *
file connect (kern test sqlmode &1 !
file sqlmtst2 ( union     !
file sqlmtst2 ( except    !
file sqlmtst2 ( minus     !
file sqlmtst2 ( intersect !
*
declare c cursor for select * from t1, t2 where a (+) = b     !
fetch c into :a, :b !
close c !
*
declare c cursor for select * from t1, t2 where a     = b (+) !
fetch c into :a, :b !
close c !
*
declare c cursor for select * from t1, t2 where a (+) = b (+) !
fetch c into :a, :b !
close c !
*
declare c cursor for select * from (select * from t1), t2 where a = b !
fetch c into :a, :b !
close c !
*
rollback work release !
