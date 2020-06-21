*   *ID* RAWHEX   DBCMD    changed on 1992-09-28-13.27.54 by ADMIN     *
file connect ( kern test sqlmode &1 !
declare c cursor for select a,&3(&2(a)) from tab !
fetch c into :a, :b !
close c!
declare c cursor for select a, &3(&2(substr(a,1,4))) from tab !
fetch c into :a, :b !
close c!
declare c cursor for select b, &3(&2(b)) from tab !
fetch c into :a, :b !
close c!
declare c cursor for select a, b,&3(&2(a || b )) from tab !
fetch c into :a, :b, :c !
close c!
declare c cursor for select a, b, &2(b || a ) from tab !
fetch c into :a, :b, :c !
close c!
declare c cursor for select c, &2(c) from tab !
fetch c into :a, :b !
close c!
declare c cursor for select
c, length(c), &2(c), length(&2(c)) from tab !
fetch c into :a, :b, :c, :d !
close c!
* The following call of a user defined function is only possible in a
* database with that function installed. So here we better ignore that!
* declare c cursor for select &3(&2(round(123,3))) from tab !
* fetch c into :a, :b !
* close c!
declare c cursor for select &3(&2(123)) from tab !
fetch c into :a, :b !
close c!
declare c cursor for select &3(&2('123')) from tab !
fetch c into :a, :b !
close c!
declare c cursor for select * from tab where &2(a) = c !
close c!
commit work release !
END
