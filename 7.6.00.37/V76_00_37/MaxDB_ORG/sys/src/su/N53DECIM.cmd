*   *ID* N53DECIM DBCMD    changed on 1992-09-28-13.26.51 by ADMIN     *
file connect ( kern test sqlmode db2 !
create table tab ( spalte1 float(3), spalte2 float(18), spalte3 decimal(5),
   spalte4 decimal(8,3), spalte5 char(10) ) !
insert into tab values ( 5E13, 9E-12, 88, 4444.555, 'aaa' ) !
commit work !
declare c cursor for select decimal ( 8E3, 10 ),
    decimal ( -8E3, 10, 6 ), decimal ( 8E-3, 4, 4 )
    from tab !
fetch c into :a, :b, :c !
close c !
declare c cursor for select decimal ( 8E3, 3 ) from tab !
fetch c into :a !
close c !
declare c cursor for select decimal ( 8E3 ) from tab !
close c !
declare c cursor for select decimal ( 8E3, a ) from tab !
close c !
declare c cursor for select decimal ( 8E3, 'a' ) from tab !
close c !
declare c cursor for select decimal ( 8E3, 5, 6 ) from tab !
close c !
declare c cursor for select decimal ( 8E3, 5, a ) from tab !
close c !
declare c cursor for select decimal ( 8E3, 5+2 ) from tab !
close c !
declare c cursor for select decimal ( 8E3, 5, 5-4  ) from tab !
close c !
declare c cursor for select decimal ( 8E , 5 )  from tab !
close c !
declare c cursor for select decimal ( '8E' , 5 )  from tab !
close c !
declare c cursor for select decimal ( 8E3, -5 )  from tab !
close c !
declare c cursor for select decimal ( spalte5, 8 )  from tab !
close c !
declare c cursor for select decimal ( null, 8 )  from tab !
close c !
declare c cursor for select decimal ( current date, 8 )  from tab !
close c !
declare c cursor for select decimal ( 5, +8 )  from tab !
close c !
declare c cursor for select decimal ( 8+5, 6, 2  ) from tAB !
fetch c into :a !
close c !
declare c cursor for select decimal ( spalte1 + kern.tab.spalte4 , 14 )
from tab !
fetch c into :a !
close c !
declare c cursor for select decimal ( spalte1, 15, 1 ) +
     decimal ( 5E13 , 15, 1 ) from tab !
fetch c into :a !
close c !
declare c cursor for select decimal ( spalte1, 15, 1 ) +
     decimal ( 5E13 , 17 ) from tab !
fetch c into :a !
close c !
declare c cursor for select decimal ( decimal ( 5, 2 ) +
   decimal ( 5, 3 ), 5, 3 ) from tab !
fetch c into :a !
close c !
declare c cursor for select decimal ( sum ( spalte1 ), 18, 2 )
   from tab !
fetch c into :a !
close c !
declare c cursor for select spalte1 from tab where decimal( 7, 2 ) = 7 !
fetch c into :a !
close c !
declare c cursor for select spalte1 from tab where decimal( 7, 2 ) is null !
fetch c into :a !
close c !
declare c cursor for select spalte1 from tab group by spalte1
                        having decimal (count(*), 2 ) > 0 !
fetch c into :a !
close c !
insert into tab values ( null, null, null, null, null ) !
declare c cursor for select decimal (spalte1, 18, 2 )  from tab !
fetch c into :a !
close c !
declare c cursor for select decimal ( count(spalte1), 3, 1 ) from tab !
fetch c into :a !
close c !
declare c cursor for select sum ( spalte1, 18, 5 ) from tab !
close c !
declare c cursor for select avg ( spalte1, 18, 5 ) from tab !
close c !
declare c cursor for select min ( spalte1, 18, 5 ) from tab !
close c !
declare c cursor for select max ( spalte1, 18, 5 ) from tab !
close c !
declare c cursor for select count ( spalte1, 18, 5 ) from tab !
close c !
create table t1 (a decimal (7))!
insert into t1 values ( 3 )!
declare c cursor for select decimal ( spalte1, 15, 1 ) +
   decimal ( a , 15, 1 ) from tab, t1!
fetch c into :a !
close c!
declare c cursor for select decimal (spalte1 + a, 12, 4) from tab, t1!
fetch c into :a !
close c!
commit work release !
