*   *ID* SQRT01   DBCMD    changed on 1992-09-28-13.21.52 by ADMIN     *
file connect ( kern test sqlmode &1 !
declare c cursor for select sqrt ('bla') from t1 where f1 = 1 !
declare c cursor for select sqrt (0) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select sqrt (1) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select sqrt (9) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select sqrt (6.25) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select sqrt (-9) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select sqrt (f1) from t1 !
fetch c into :a !
close c !
declare c cursor for select sqrt (f2) from t1 !
fetch c into :a !
close c !
declare c cursor for select sqrt (f3) from t1 !
fetch c into :a !
commit work release !
