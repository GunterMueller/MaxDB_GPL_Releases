*   *ID* N29ABS   DBCMD    changed on 1992-09-28-13.25.56 by ADMIN     *
file connect ( kern test !
create table tab ( spalte1 fixed(5,2),
                   spalte2 float(7),
                   spalte3 float(16),
                   spalte4 char(10),
                   spalte5 char(10) ) !
insert tab values ( 32, 12, null, 'Paul', null ) !
select abs (17), abs (16.543), abs(0), ABS(-0), abs( -7 ), abs ( - 5.21)
   from tab !
fetch !
select abs (-0.00000000000000001 ), ABS(-999999999999999999 ),
       abs ( 1e18 ), abs(-1E18 ), abs  ((-12.E32 )),abs(+00.8E-6 )
    from tab !
fetch !
select abs ( -1E-64), abs ( + 9.99999999999999999e62 ) from tab !
fetch !
select abs (-9.99999999999999999E+63) from tab !
select abs ( # ) from tab !
select abs (   ) from tab !
select abs ( -12.3e65 ) from tab !
select abs ( spalte1) from tab !
fetch !
select  abs (spalte3 ) from tab !
fetch !
select abs ('Otto' ) from tab !
select abs ( spalte4 ) from tab !
select abs ( spalte5  ) from tab !
select abs ( abs (-7.3)),abs ( abs(abs(spalte2 ))) from tab !
fetch !
select 3 + abs(-7)-32, 5*abs(-0.72 )/ 6, abs (-12) /abs(6.5)*3
    from tab !
fetch !
drop table tab !
commit work release !
