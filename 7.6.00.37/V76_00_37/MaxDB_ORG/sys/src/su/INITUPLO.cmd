*   *ID* INITUPLO DBCMD    changed on 1992-09-28-13.23.56 by ADMIN     *
file connect (kern test sqlmode &2 !
declare c cursor for select
&1 (' vertreiben  dEr GeistER, Die WIR riefen! ') || '+'
from t1 where s2 = 'key' !
fetch c into :a !
close c !
declare c cursor for select
&1 ('e.t.a.hoffmann,dick&doof,mueller-luedenscheid')
from t1 where s2 = 'key' !
fetch c into :a !
close c !
declare c cursor for select &1 (s1) || '+' from t1 where s2 = 'key' !
fetch c into :a !
close c !
declare c cursor for select &1 (s2) || '+' from t1 where s2 = 'key' !
fetch c into :a !
close c !
declare c cursor for select &1 (s3) || '+' from t1 where s2 = 'key' !
fetch c into :a !
close c !
declare c cursor for select &1 (s4) || '+' from t1 where s2 = 'key' !
declare c cursor for select &1 (s5) || '+' from t1 where s2 = 'key' !
declare c cursor for select
rtrim(ltrim(&1 ('  vertreiben  dEr GeistER, Die WIR riefen!  ')))
|| '+' from t1 where s2 = 'key' !
fetch c into :a !
close c !
pars_then_ex !
data !
declare c cursor for select &1 (:A) from t1 where s2 = 'key' !
c 5
 
nodata !
fetch c into :a!
pars_execute !
commit work release !
