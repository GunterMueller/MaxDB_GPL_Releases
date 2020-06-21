*   *ID* DATEFUNC DBCMD    changed on 1992-04-13-08.49.40 by DIRK      *
file connect ( kern test sqlmode &1 !
declare c cursor for select adddate(f1,30) from t1 !
fetch c into :a !
close c !
declare c cursor for select subdate (f1,25) from t1 !
fetch c into :a !
close c !
declare c cursor for select datediff (f1, '19920101') from t1 !
fetch c into :a !
close c !
declare c cursor for select dayofweek (f1) from t1 !
fetch c into :a !
close c !
declare c cursor for select weekofyear (f1) from t1 !
fetch c into :a !
close c !
declare c cursor for select dayofyear (f1) from t1 !
fetch c into :a !
close c !
declare c cursor for select makedate (1992,f0) from t1 !
fetch c into :a !
close c !
rollback work release !
