*   *ID* N29TRUNC DBCMD    changed on 1992-09-28-13.26.00 by ADMIN     *
file connect ( kern test !
create table tab ( spalte1 fixed(5,2),
                   spalte2 float(7),
                   spalte3 float(16),
                   spalte4 char(10),
                   spalte5 char(10) ) !
insert tab values ( 32, 12, null, 'Paul', null ) !
select trunc(17),trunc ((15.2) ),TRUNC (0 ), TRUNC ( - 0.340 ),trunc(-5)
   from tab !
fetch !
select trunc ( 999999999999999999 ), TRUNC( - 0.000000000000000001 ),
       trunc (-0.999999999999999999), trunc ( -999999999999999999  )
       from tab !
fetch !
select trunc (1e18), trunc (0E18), trunc(  -1e+18 ), trunc ( 12E32 ),
       trunc ( -18e24 )
       from tab !
fetch !
select trunc (-9.99999999999999999E+62), trunc (-1e-64 ), trunc (1e-64),
       trunc ( + 9.99999999999999999E+62 )
       from tab !
fetch !
select trunc ( spalte1 ),trunc(spalte2+6.07) from tab !
fetch !
select trunc ( ) from tab !
select trunc ( * ) from tab !
select trunc ( NULL ) from tab !
fetch!
select trunc ( spalte3) from tab !
fetch !
select trunc ( 'Otto') from tab !
select trunc (spalte4 ) from tab !
select trunc (spalte5) from tab !
select 3 +trunc(17.2)-12, 17 / trunc(6.23 ) * 6,
       8*trunc (0.645)+spalte2 ,  spalte1 - trunc ((( 66.66)))/(-3.4)
   from tab !
fetch !
select trunc ( trunc(trunc( -82.123 +2.90))) from tab !
fetch !
select 7, trunctrunc (7.2) from tab !
select 7, spalte2trunc(-32) from tab !
select trspalte2 from tab !
select trunc( 18E76 ) from tab !
select trunc (0.00000678, -3) into :a from dual !
drop table tab !
commit work release !
