*   *ID* SPECFUNC DBCMD    changed on 1992-05-22-12.54.48 by DIRK      *
file connect ( kern test sqlmode &1 !
declare c cursor for select value ( f1, f2, f3) from t1 !
fetch c into :a !
close c !
declare c cursor for select nvl (f1 , f2 ) from t1 !
fetch c into :a !
close c !
declare c cursor for select greatest ( f1, f2, f3) from t1 !
fetch c into :a !
close c !
declare c cursor for select least ( f1, f2, f3) from t1 !
fetch c into :a !
close c !
declare c cursor for select decode ( f1, f2, f3) from t1 !
fetch c into :a !
close c !
commit work release !
