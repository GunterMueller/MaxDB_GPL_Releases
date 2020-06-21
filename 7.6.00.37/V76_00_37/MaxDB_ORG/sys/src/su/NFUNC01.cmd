*   *ID* NFUNC01  DBCMD    changed on 1992-09-28-13.24.41 by ADMIN     *
file connect ( kern test sqlmode &2 !
declare c cursor for select &1 ('1.25') from t1 where s1 = 0 !
declare c cursor for select &1 (1.25) from t1 where s1 = 0 !
fetch c into :a !
close c !
declare c cursor for select &1 (-1.25) from t1 where s1 = 0 !
fetch c into :a !
close c !
declare c cursor for select &1 (0) from t1 where s1 = 0 !
fetch c into :a !
close c !
declare c cursor for select &1 (0.78) from t1 where s1 = 0 !
fetch c into :a !
close c !
declare c cursor for select &1 (-0.78) from t1 where s1 = 0 !
fetch c into :a !
close c !
declare c cursor for select &1 (45/0) from t1 where s1 = 0 !
fetch c into :a !
close c !
declare c cursor for select &1 (s1) from t1 !
fetch c into :a !
close c !
declare c cursor for select &1(4+ 5) from t1 where s1 = 0 !
fetch c into :a !
close c !
declare c cursor for select &1(23/5) from t1 where s1 = 0 !
fetch c into :a !
commit work release !
