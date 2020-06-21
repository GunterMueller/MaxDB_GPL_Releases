*   *ID* OPMODE   DBCMD    changed on 1992-05-22-13.18.47 by DIRK      *
file connect ( kern test sqlmode &1 !
declare c cursor for select 5 + f4 from t1 !
fetch c into :a !
close c !
declare c cursor for select 100000 - f4 from t1 !
fetch c into :a !
close c !
declare c cursor for select 4 * f4 from t1 !
fetch c into :a !
close c !
declare c cursor for select f4 / 5 from t1 !
fetch c into :a !
close c !
declare c cursor for select c1 || '**' from t2 !
fetch c into :a !
close c !
declare c cursor for select c1 & '**' from t2 !
fetch c into :a !
close c !
declare c cursor for select (345 DIV 10) from t1 !
fetch c into :a !
close c !
declare c cursor for select (345 MOD 10) from t1 !
fetch c into :a !
close c !
*** Special Null
declare c cursor for select f4 / 0 from t1 !
fetch c into :a !
close c !
commit work release !
