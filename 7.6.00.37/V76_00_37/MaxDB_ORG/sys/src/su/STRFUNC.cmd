*   *ID* STRFUNC  DBCMD    changed on 1992-05-11-14.29.42 by DIRK      *
file connect ( kern test sqlmode &1 !
declare c cursor for select substr (c2,4,2) from t2 !
fetch c into :a !
close c !
declare c cursor for select c1 || c2 from t2 !
fetch c into :a !
close c !
declare c cursor for select c1 & c2 from t2 !
fetch c into :a !
close c !
declare c cursor for select trim (c2) from t2 !
fetch c into :a !
close c !
declare c cursor for select rtrim (c2) from t2 !
fetch c into :a !
close c !
declare c cursor for select ltrim (c2) from t2 !
fetch c into :a !
close c !
declare c cursor for select lfill (c2,'a') from t2 !
fetch c into :a !
close c !
declare c cursor for select rfill (c2,'a') from t2 !
fetch c into :a !
close c !
declare c cursor for select lpad (c2,8,'a') from t2 !
fetch c into :a !
close c !
declare c cursor for select rpad (c2,8,'a') from t2 !
fetch c into :a !
close c !
declare c cursor for select lpad (c2,8,'a',12) from t2 !
fetch c into :a !
close c !
declare c cursor for select rpad (c2,8,'a',12) from t2 !
fetch c into :a !
close c !
declare c cursor for select expand (c2,12) from t2 !
fetch c into :a !
close c !
declare c cursor for select upper (c2) from t2 !
fetch c into :a !
close c !
declare c cursor for select lower (c2) from t2 !
fetch c into :a !
close c !
declare c cursor for select initcap (c2) from t2 !
fetch c into :a !
close c !
declare c cursor for select replace (c2,'l','%') from t2 !
fetch c into :a !
close c !
declare c cursor for select translate (c2,'l','%') from t2 !
fetch c into :a !
close c !
declare c cursor for select alpha (c2) from t2 !
fetch c into :a !
close c !
declare c cursor for select ascii (c2) from t2 !
fetch c into :a !
close c !
declare c cursor for select soundex (c2) from t2 !
fetch c into :a !
close c !
commit work release !
