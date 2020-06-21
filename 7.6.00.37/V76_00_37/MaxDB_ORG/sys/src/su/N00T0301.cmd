*   *ID* N00T0301 DBCMD    changed on 1992-09-28-13.24.44 by ADMIN     *
 ***  3.8.1 <arit function> !
file connect ( kern test !
create table tab1 ( spalte1 fixed(5),
                    spalte2 fixed(11,6),
                    spalte3 fixed(3,3),
                    spalte4 float(7) ) !
create table tab2 ( spalte1 float(12), col2 fixed(5,2) ) !
insert tab1 values ( 348  , 76543.21, - 0.623, -12.3E12 ) !
insert tab1 values ( -9999,        0,   0.003, 12345e-7 ) !
insert tab1 values (  null,  -5.1234,       0,     null ) !
insert tab1 values (     0,     null,    null,    3E-32 ) !
insert tab2 values ( 1.34E+1,   22.33 ) !
insert tab2 values (   -0.18, -123.07 ) !
select *  from tab1
   where trunc(spalte2 ) between trunc (- 5.1) and abs ( -76543.1 ) !
fetch !
declare erg cursor for 
select * from tab1
   where - trunc( ABS( spalte4 ) ) +3*ABS(-.0)
         +abs  (-abs  (TRUNC (6*tab1.spalte3 )/ 2)+ 18 )
       < trunc(28.71* 1000 ) or trunc ( spalte2)>0 !
fetch erg !
select  spalte1 + abs(2*spalte4),
        spalte3 + TRUNC ( kern.tab1.spalte2 ),
        trunc (6 +abs (-12.7))
        from tab1 !
fetch !
select *  from tab1, tab2 ref
   where +abs(tab1.spalte1 + ref.spalte1 )
       > trunc(ref.spalte1 -  ABS( -ref.col2 )) !
fetch !
select  3*trunc(  spalte3 ) / abs(ref.col2 * abs(-1.0)),
        abs ( col2 ) neu,
        kern.tab1.spalte4 + trunc(-32.7 )
  from tab1,tab2 ref where tab1.spalte3 is not null !
fetch !
rollback work !
create table tab3 ( spalte1 fixed(5,2),
                    spalte2 fixed(7)  ,
                    spalte3 float(7)  ,
                    spalte4 float(3)  ) !
insert tab3 values (     0, -62, 33E16,   18E-3 ) !
insert tab3 values ( -0.21, 128, -16E8, -890E-6 ) !
insert tab3 values ( null ,   0,     0,    null ) !
select * from tab3
   where -spalte2 mod 18*5 -tab3.spalte3 div 0.2E4
      <> (-spalte3 ) MOD ( ( 999999999999999999) DIV
         (abs((kern.tab3.spalte3)) -tab3.spalte2 + 1) ) !
fetch !
select * from tab3
   where 0 div(100* spalte1 + 1 ) is null
     and spalte3 * 1.e+7 + 7 mod abs(spalte4)  is null !
fetch !
select * from tab3
   where 13E12 div ( trunc ( spalte1) + 26 )
         between Abs(-5) mod 6 and 10E30 !
fetch !
select ( spalte3 + tAB3.spalte2 ) MOD
   (+abs (trunc(( kern.tab3.spalte3+1) *1e3/(abs (spalte3 )+32e1)))),
       spalte2 *spalte2 div (-1)
   from tab3 !
fetch !
rollback work release !
