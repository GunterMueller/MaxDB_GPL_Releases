*   *ID* N29INTEG DBCMD    changed on 1992-09-28-13.25.58 by ADMIN     *
file connect ( kern test sqlmode db2 !
create table tab ( spalte1 decimal(5,2),
                   spalte2 float(7),
                   spalte3 float(16),
                   spalte4 char(10),
                   spalte5 char(10) ) !
insert into tab values ( 32, 12, null, 'Paul', null ) !
declare c cursor for select integer(17), integer ((15.2) ), integer(-5)
   from tab !
fetch c into :a, :b, :c !
close c !
declare c cursor for select integer ( 999999999999999999 ),
       integer (-0.999999999999999999), integer ( -999999999999999999  )
       from tab !
fetch c into :a, :b, :c !
close c !
declare c cursor for select integer ( -18e24 )
       from tab !
fetch c into :a !
close c !
declare c cursor for select integer (1e-64 )
       from tab !
fetch c into :a !
close c !
declare c cursor for select integer ( spalte1 ),integer(spalte2+6.07)
from tab !
fetch c into :a, :b !
close c !
declare c cursor for select integer ( ) from tab !
declare c cursor for select integer ( * ) from tab !
declare c cursor for select integer ( NULL ) from tab !
declare c cursor for select integer ( spalte3) from tab !
fetch c into :a !
close c !
declare c cursor for select integer ( 'Otto') from tab !
declare c cursor for select integer (spalte4 ) from tab !
declare c cursor for select integer (spalte5) from tab !
declare c cursor for select 3 +integer(17.2)-12, 17 / integer(6.23 ) * 6,
       8*integer (0.645)+spalte2 ,  spalte1 - integer ((( 66.66)))/(-3.4)
   from tab !
fetch c into :a, :b, :c, :d !
close c !
declare c cursor for select integer ( integer(integer( -82.123 +2.90)))
from tab !
fetch c into :a !
close c !
declare c cursor for select 7, integerinteger (7.2) from tab !
declare c cursor for select 7, spalte2integer(-32) from tab !
declare c cursor for select intsp2 from tab !
declare c cursor for select integer( 18E76 ) from tab !
drop table tab !
commit work release !
