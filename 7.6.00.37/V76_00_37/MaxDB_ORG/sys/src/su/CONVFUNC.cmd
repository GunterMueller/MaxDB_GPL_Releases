*   *ID* CONVFUNC DBCMD    changed on 1992-06-01-14.53.02 by DIRK      *
file connect ( kern test sqlmode &1 !
declare c cursor for select num (c1) from t2 !
fetch c into :a !
close c !
declare c cursor for select to_number ( c1) from t2 !
fetch c into :a !
close c !
declare c cursor for select chr (6), chr(c1) from t2 !
fetch c into :a, :b !
close c !
declare c cursor for select chr (76) from t2 !
fetch c into :a !
close c !
declare c cursor for select digits (f4) from t1 !
fetch c into :a !
close c !
declare c cursor for select hex (c2) from t2 !
fetch c into :a !
close c !
declare c cursor for select rawtohex (c2) from t2 !
fetch c into :a !
close c !
declare c cursor for select hextoraw ('48') from t2 !
fetch c into :a !
close c !
declare c cursor for select char (c4) from t2 !
fetch c into :a !
close c !
commit work release !
