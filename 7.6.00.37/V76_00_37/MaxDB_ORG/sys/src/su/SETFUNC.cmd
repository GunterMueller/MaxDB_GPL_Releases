*   *ID* SETFUNC  DBCMD    changed on 1992-05-22-12.54.51 by DIRK      *
file connect ( kern test sqlmode &1 !
declare c cursor for select count ( f1 ) from t1 !
fetch c into :a !
close c !
declare c cursor for select min ( f1 ) from t1 !
fetch c into :a !
close c !
declare c cursor for select max ( f1 ) from t1 !
fetch c into :a !
close c !
declare c cursor for select sum ( f4 ) from t1 !
fetch c into :a !
close c !
declare c cursor for select avg ( f4 ) from t1 !
fetch c into :a !
close c !
declare c cursor for select stddev ( f4 ) from t1 !
fetch c into :a !
close c !
declare c cursor for select variance ( f4 ) from t1 !
fetch c into :a !
close c !
commit work release !
