*   *ID* TRANSLAT DBCMD    changed on 1992-09-28-13.28.25 by ADMIN     *
file connect ( kern test sqlmode &1 !
declare c cursor for select translate(c1,null,'b') from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate(c1,'b',null) from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate(c3,'b','bla') from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate('b',c3,'bla') from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate('b','b',c3) from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate('ba',c3,'bla') from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate('ba','b',c3) from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate('bbbbbbbb','b','a')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate('la$31ber ','$','*')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate('la$31a$31er ','$31','*')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate('la$31$31er ','$31','*')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate(c1,c2,c3) from t2 where c1 <> '12' !
fetch c into :a !
close c !
declare c cursor for select translate('$31laer','$31','*')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate('laer$31','$31','*')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate('$31l$31aer','$31','*')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate('$31laer$31','$31','*')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate('la$31ber ','$31','*=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate('la$31a$31er ','$31','*=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate('la$31$31er ','$31','*=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate(c1,c2,c3) from t2 where c1 <> '12' !
fetch c into :a !
close c !
declare c cursor for select translate('$31laer','$31','*=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate('laer$31','$31','*=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate('$31l$31aer','$31','*=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate('$31laer$31','$31','*=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate('la$31ber ','$31','*++=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate('la$31a$31er ','$31','*++=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate('la$31$31er ','$31','*++=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate(c1,c2,c3) from t2 where c1 <> '12' !
fetch c into :a !
close c !
declare c cursor for select translate('$31laer','$31','*++=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate('laer$31','$31','*++=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate('$31l$31aer','$31','*++=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
declare c cursor for select translate('$31laer$31','$31','*++=')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
set format eur !
 * 'set format' klappt nur bei mode = internal, entspr. liefert das folgen-
 * de select unterschiedliche ergebnisse !
declare c cursor for select translate (d1, '.', ':')
      from t2 where c1 = '12' !
fetch c into :a !
close c !
set format internal !
declare c cursor for select
translate( 'bbbbbbbbb bbbbbbbbb bbbbbbbbb bbbbbbbbb bbbbbbbbb '
'bbbbbbbbb bbbbbbbbb bbbbbbbbb bbbbbbbbb bbbbbbbbb ',
'bbbbbbbbb ','1la2la3la4la5la6la7la8la9la0la') from t2 !
commit work release !
