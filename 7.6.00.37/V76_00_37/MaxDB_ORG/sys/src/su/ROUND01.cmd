*   *ID* ROUND01  DBCMD    changed on 1992-09-28-13.27.59 by ADMIN     *
file connect ( kern test sqlmode &1!
declare c cursor for select round('INTERNAL') from t1 where f1 = 1 !
declare c cursor for select round(67.7,'a') from t1 where f1 = 1 !
declare c cursor for select round(5.2) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select round(5.2,0) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select round(5.123,2) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select round(5.123/0,2) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select round(23/7,9/3) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select round(12345.123,-2) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select round(12345.123,-2+4) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select round(12345.123,-1) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select round(f2,1) from t1 !
fetch c into :a !
rollback work release !
