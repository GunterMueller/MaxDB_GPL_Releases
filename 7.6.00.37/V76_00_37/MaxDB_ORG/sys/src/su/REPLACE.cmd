*   *ID* REPLACE  DBCMD    changed on 1992-09-28-13.27.57 by ADMIN     *
file connect ( kern test sqlmode &1 !
declare c cursor for select replace(c1,null,'b') from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace(c1,'b',null) from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace(c3,'b','bla') from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('b',c3,'bla') from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('b','b',c3) from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('ba',c3,'bla') from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('ba','b',c3) from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('bbbbbbbb','b','bla')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('la$31ber ','$31','*')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('la$31a$31er ','$31','*')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('la$31$31er ','$31','*')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace(c1,c2,c3) from t2 where c1 <> '12' !
fetch c into :a !
close c !
declare c cursor for select replace('$31laer','$31','*')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('laer$31','$31','*')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('$31l$31aer','$31','*')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('$31laer$31','$31','*')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('la$31ber ','$31','*=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('la$31a$31er ','$31','*=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('la$31$31er ','$31','*=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace(c1,c2,c3) from t2 where c1 <> '12' !
fetch c into :a !
close c !
declare c cursor for select replace('$31laer','$31','*=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('laer$31','$31','*=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('$31l$31aer','$31','*=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('$31laer$31','$31','*=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('la$31ber ','$31','*++=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('la$31a$31er ','$31','*++=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('la$31$31er ','$31','*++=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace(c1,c2,c3) from t2 where c1 <> '12' !
fetch c into :a !
close c !
declare c cursor for select replace('$31laer','$31','*++=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('laer$31','$31','*++=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('$31l$31aer','$31','*++=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select replace('$31laer$31','$31','*++=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
set format eur !
 * 'set format' klappt nur bei mode = internal, entspr. liefert das folgen-
 * de select unterschiedliche ergebnisse !
declare c cursor for select replace(d1, '.', ':') from t2 where c1 = '12' !
fetch c into :a !
close c !
set format internal !
declare c cursor for select
replace( 'bbbbbbbbb bbbbbbbbb bbbbbbbbb bbbbbbbbb bbbbbbbbb '
'bbbbbbbbb bbbbbbbbb bbbbbbbbb bbbbbbbbb bbbbbbbbb ',
'bbbbbbbbb ','1la2la3la4la5la6la7la8la9la0la') from t2 !
commit work release !
