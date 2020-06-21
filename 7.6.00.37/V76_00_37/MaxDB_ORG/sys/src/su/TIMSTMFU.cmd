*   *ID* TIMSTMFU DBCMD    changed on 1992-05-22-11.02.21 by DIRK      *
file connect ( kern test sqlmode &1 !
declare c cursor for select days(f3) from t1 !
fetch c into :a !
close c !
declare c cursor for select timestamp (f3) from t1 !
fetch c into :a !
close c !
declare c cursor for select date (f3) from t1 !
fetch c into :a !
close c !
declare c cursor for select year (f3) from t1 !
fetch c into :a !
close c !
declare c cursor for select month(f3) from t1 !
fetch c into :a !
close c !
declare c cursor for select day (f3) from t1 !
fetch c into :a !
close c !
declare c cursor for select time (f3) from t1 !
fetch c into :a !
close c !
declare c cursor for select hour (f3) from t1 !
fetch c into :a !
close c !
declare c cursor for select minute (f3) from t1 !
fetch c into :a !
close c !
declare c cursor for select second (f3) from t1 !
fetch c into :a !
close c !
declare c cursor for select microsecond (f3) from t1 !
fetch c into :a !
close c !
commit work release !
