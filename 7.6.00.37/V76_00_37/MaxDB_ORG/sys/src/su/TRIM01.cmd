*   *ID* TRIM01   DBCMD    changed on 1992-09-28-13.28.26 by ADMIN     *
file connect (kern test sqlmode &2 !
declare c cursor for select &1('blabla') from t1 where s2 = 'key' !
fetch c into :a !
close c !
declare c cursor for select &1('a') from t1 where s2 = 'key' !
fetch c into :a !
close c !
declare c cursor for select
      &1('        ') || '+'  from t1 where s2 = 'key' !
fetch c into :a !
close c !
declare c cursor for select &1(' ') || '+'  from t1 where s2 = 'key' !
fetch c into :a !
close c !
declare c cursor for select
     &1('        ',' ') || '+'  from t1 where s2 = 'key' !
fetch c into :a !
close c !
declare c cursor for select &1(' ',' ') || '+'  from t1 where s2 = 'key' !
fetch c into :a !
close c !
declare c cursor for select &1('n','n') || '+'  from t1 where s2 = 'key' !
fetch c into :a !
close c !
declare c cursor for select
     &1('nnnnnnnn','n') || '+'  from t1 where s2 = 'key' !
fetch c into :a !
close c !
declare c cursor for select
     &1('npacbcapn','npc') || '+'  from t1 where s2 = 'key' !
fetch c into :a !
close c !
declare c cursor for select
     &1 (ascii('npacbcapn'),'npc') || '+'  from t1 where s2 = 'key' !
fetch c into :a !
close c !
declare c cursor for select
     &1 (ascii('    b    '),' ') || '+'  from t1 where s2 = 'key' !
fetch c into :a !
close c !
declare c cursor for select
     &1 (ascii('    b    ')) || '+'  from t1 where s2 = 'key' !
fetch c into :a !
close c !
declare c cursor for select
     &1 (X'0000000000') || X'FF' from t1 where s2 = 'key' !
fetch c into :a !
close c !
declare c cursor for select
     &1 (X'06060206060606',X'06') || X'FF' from t1 where s2 = 'key' !
fetch c into :a !
close c !
declare c cursor for select
     &1 (X'06060206060606',X'06') || X'FF' from t1 where s2 = 'key' !
fetch c into :a !
close c !
declare c cursor for select &1 (s1) from t1 where s2 = 'null' !
fetch c into :a !
close c !
pars_execute !
declare c cursor for select &1 (5) from t1 where s2 = 'key' !
fetch c into :a !
close c !
declare c cursor for select &1 (3*5) from t1 where s2 = 'key' !
fetch c into :a !
close c !
commit work release !
