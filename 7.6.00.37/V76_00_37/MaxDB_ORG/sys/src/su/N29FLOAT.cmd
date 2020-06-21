*   *ID* N29FLOAT DBCMD    changed on 1992-09-28-13.25.57 by ADMIN     *
file connect ( kern test sqlmode db2 !
create table tab ( spalte1 decimal(5,2),
                   spalte2 float(7),
                   spalte3 float(16),
                   spalte4 char(10),
                   spalte5 char(10) ) !
insert into tab values ( 32, 12, null, 'Paul', null ) !
declare c cursor for select float(17), float ((15.2) ), float(-5)
   from tab !
fetch c into :a, :b, :c !
close c !
declare c cursor for select float ( 999999999999999999 ),
       float (-0.999999999999999999), float ( -999999999999999999  )
       from tab !
fetch c into :a, :b, :c !
close c !
declare c cursor for select float (1e18), float (0E18),
       float(  -1e+18 ), float ( 12E32 ),
       float ( -18e24 )
       from tab !
fetch c into :a, :b, :c, :d, :e !
close c !
declare c cursor for select float (-9.99999999999999999E+62),
       float (-1e-64 ), float (1e-64),
       float ( + 9.99999999999999999E+62 )
       from tab !
fetch c into :a, :b, :c, :d !
close c !
declare c cursor for select float ( spalte1 ),float(spalte2+6.07)
     from tab !
fetch c into :a, :b !
close c !
declare c cursor for select float ( ) from tab !
declare c cursor for select float ( * ) from tab !
declare c cursor for select float ( NULL ) from tab !
declare c cursor for select float ( spalte3) from tab !
fetch c into :a !
close c !
declare c cursor for select float ( 'Otto') from tab !
declare c cursor for select float (spalte4 ) from tab !
declare c cursor for select float (spalte5) from tab !
declare c cursor for select 3 +float(17.2)-12, 17 / float(6.23 ) * 6,
       8*float (0.645)+spalte2 ,  spalte1 - float ((( 66.66)))/(-3.4)
   from tab !
fetch c into :a, :b, :c, :d !
close c !
declare c cursor for select float ( float(float( -82.123 +2.90)))
    from tab !
fetch c into :a !
close c !
declare c cursor for select 7, floatfloat (7.2) from tab !
declare c cursor for select 7, spalte2float(-32) from tab !
declare c cursor for select floatsp2 from tab !
declare c cursor for select float( 18E76 ) from tab !
drop table tab !
commit work release !
