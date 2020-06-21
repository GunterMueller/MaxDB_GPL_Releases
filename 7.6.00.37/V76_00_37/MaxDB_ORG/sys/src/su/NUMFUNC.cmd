*   *ID* NUMFUNC  DBCMD    changed on 1992-05-21-14.38.27 by DIRK      *
file connect ( kern test sqlmode &1 !
declare c cursor for select fixed(f1,5,3) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select decimal (f1,5,3) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select float (f1) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select trunc (f2) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select integer (f2) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select round (f2) from t1 where f1 = 1 !
fetch c into :a !
close c !
declare c cursor for select ceil (f2) from t1 where f1 = 2 !
fetch c into :a !
close c !
declare c cursor for select floor (f2) from t1 where f1 = 2 !
fetch c into :a !
close c !
declare c cursor for select sign (f2) from t1 where f1 = 2 !
fetch c into :a !
close c !
declare c cursor for select abs (f2) from t1 where f1 = 2 !
fetch c into :a !
close c !
declare c cursor for select power (f1,4) from t1 where f1 = 2 !
fetch c into :a !
close c !
declare c cursor for select sqrt (f1) from t1 where f1 = 2 !
fetch c into :a !
close c !
declare c cursor for select mod (f1,3) from t1 where f1 = 2 !
fetch c into :a !
close c !
declare c cursor for select length (f1), length('bla'), length (x'234567')
from t1 where f1 = 2 !
fetch c into :a, :b, :c !
close c !
declare c cursor for select index ('INTERNAL','RN',1,1) from t1 where f1 = 2 !
fetch c into :a !
close c !
declare c cursor for select instr ('INTERNAL','RN',1,1) from t1 where f1 = 2 !
fetch c into :a !
close c !
rollback work release !
