*   *ID* POWER01  DBCMD    changed on 1992-09-28-13.21.49 by ADMIN     *
file connect ( kern test sqlmode &1 !
declare c cursor for select power ('bla',2) from t1 where f1 = 1 !
declare c cursor for select power (2,'bla') from t1 where f1 = 1 !
declare c cursor for select power (2,f3) from t1 where f1 = 3 !
declare c cursor for select power (2,f2) from t1 where f1 = 3 !
declare c cursor for select power (2,f1) from t1 !
fetch c into :a !
close c !
declare c cursor for select power (999999,3) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select power (999999,4) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select power (-999999,3) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select power (3,3) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select power (-3,3) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select power (f1,3) from t1 !
fetch c into :a !
close c !
declare c cursor for select power (f2,3) from t1 !
fetch c into :a !
close c !
declare c cursor for select power (f3,3) from t1 !
fetch c into :a !
close c !
declare c cursor for select power (10,3) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select power (99,4) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select power (9.99,3) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select power (1.0000001,50) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select power (1.0000001,999) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select power (2,-2) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select power (-2,-3) from t1 where f1 = 1 !
fetch c into :a !
commit work release !
