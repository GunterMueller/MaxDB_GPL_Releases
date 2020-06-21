*   *ID* TIMEFUNC DBCMD    changed on 1992-04-13-09.05.48 by DIRK      *
file connect ( kern test sqlmode &1 !
declare c cursor for select addtime(f2,'00120000') from t1 !
fetch c into :a !
close c !
declare c cursor for select subtime (f2,'00120000') from t1 !
fetch c into :a !
close c !
declare c cursor for select timediff (f2, '00120000') from t1 !
fetch c into :a !
close c !
declare c cursor for select maketime (19,f0,30) from t1 !
fetch c into :a !
close c !
rollback work release !
