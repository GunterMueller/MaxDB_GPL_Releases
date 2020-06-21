*   *ID* MOD01    DBCMD    changed on 1992-09-28-13.21.42 by ADMIN     *
file connect ( kern test sqlmode oracle!
declare c cursor for select MOD ('modular',2) from t1  where f1 = 1 !
declare c cursor for select MOD (2,'modular') from t1  where f1 = 1 !
declare c cursor for select MOD (7,F3) from t1 !
declare c cursor for select MOD (7,F2) from t1 !
declare c cursor for select MOD (7,F1) from t1 !
fetch c into :a !
close c !
declare c cursor for select MOD (5,2) from t1  where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select MOD (5,6) from t1  where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select MOD (5,0) from t1  where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select MOD (f1,6) from t1 !
fetch c into :a !
close c !
declare c cursor for select MOD (9,f1) from t1 !
fetch c into :a !
close c !
declare c cursor for select MOD (9,f1/0) from t1 !
fetch c into :a !
close c !
declare c cursor for select MOD (9/0,f1) from t1 !
fetch c into :a !
commit work release !
