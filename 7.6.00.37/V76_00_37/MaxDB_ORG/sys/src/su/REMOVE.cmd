*   *ID* REMOVE   DBCMD    changed on 1992-09-28-13.27.55 by ADMIN     *
file connect ( kern test sqlmode &1 !
declare c cursor for select replace('la$31ber ','$31')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('la$31a$31er ','$31')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('la$31$31er ','$31')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace(c1,c2) from t2 where c1 <> '12' !
fetch c into :a !
close c !
declare c cursor for select replace('$31laer','$31')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('laer$31','$31')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('$31l$31aer','$31')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('$31laer$31','$31')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
commit work release !
