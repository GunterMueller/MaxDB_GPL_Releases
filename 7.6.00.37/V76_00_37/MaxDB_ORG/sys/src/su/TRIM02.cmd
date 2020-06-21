*   *ID* TRIM02   DBCMD    changed on 1992-09-28-13.28.27 by ADMIN     *
file connect ( kern test sqlmode &1 !
declare c cursor for select trim(s1) from t1 !
fetch c into :a !
close c !
declare c cursor for select trim(date) from t1 !
fetch c into :a !
close c !
declare c cursor for 
select trim(s1), trim(s2), trim(s4), trim(s5), trim(s6) 
from t1 !
fetch c into :a !
close c !
declare c cursor for 
select trim(s1) || '+', trim(s2) || '+', trim(s4) 
from t1 !
fetch c into :a !
close c !
declare c cursor for select trim(s1) || trim(s2) from t1 !
fetch c into :a, :b !
close c !
declare c cursor for select ltrim(s1) || trim(s2) from t1 !
fetch c into :a, :b !
close c !
declare c cursor for select ltrim(s5) || ltrim(s6) from t1 !
fetch c into :a !
close c !
declare c cursor for select rtrim(s5) || rtrim(s6) from t1 !
fetch c into :a !
close c !
declare c cursor for select ltrim(s1) || rtrim(s2) from t1 !
fetch c into :a !
close c !
declare c cursor for select ltrim(s1) || ltrim(s2) from t1 !
fetch c into :a !
close c !
declare c cursor for select rtrim(s1) || rtrim(s2) from t1 !
fetch c into :a !
close c !
declare c cursor for select trim(s1,'b') || '+' from t1 !
fetch c into :a !
close c !
declare c cursor for select rtrim(s1,'b') || '+' from t1 !
fetch c into :a !
close c !
declare c cursor for select ltrim(rtrim(s1)) || '+' from t1 !
fetch c into :a !
close c !
declare c cursor for select rtrim(ltrim(s1)) || '+' from t1 !
fetch c into :a !
close c !
declare c cursor for select ltrim(rtrim(s4)) || X'EE' from t1 !
fetch c into :a !
close c !
declare c cursor for select rtrim(ltrim(s4)) || X'EE' from t1 !
fetch c into :a !
close c !
declare c cursor for select s1, s2, s4, s5, s6 from t1 !
fetch c into :a, :b, :c, :d, :e!
rollback release!
