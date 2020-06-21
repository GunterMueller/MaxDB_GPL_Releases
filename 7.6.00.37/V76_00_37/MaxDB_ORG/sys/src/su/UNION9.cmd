*   *ID* UNION9   DBCMD    changed on 1992-09-28-13.28.49 by ADMIN     *
file connect ( kern test !
create table tab5 ( spalte1 fixed(5),
                    spalte2 fixed(11,6),
                    spalte3 fixed(3,3),
                    spalte4 float(7) ) !
create table tab6 ( spalte1 float(12), col2 fixed(5,2) ) !
insert tab5 values ( 348  , 76543.21, - 0.623, -12.3E12 ) !
insert tab5 values ( -9999,        0,   0.003, 12345e-7 ) !
insert tab5 values (  null,  -5.1234,       0,     null ) !
insert tab5 values (     0,     null,    null,    3E-32 ) !
insert tab6 values ( 1.34E+1,   22.33 ) !
insert tab6 values (   -0.18, -123.07 ) !
select *  from tab5
   where trunc(spalte2 ) between trunc (- 5.1) and abs ( -76543.1 ) !
fetch !
select *  from tab5
   where trunc(spalte2 ) between trunc (- 5.1) and abs ( -76543.1 )
union
select *  from tab5
   where trunc(spalte2 ) between trunc (- 5.1) and abs ( -76543.1 ) !
fetch !
select *  from tab5
   where trunc(spalte2 ) between trunc (- 5.1) and abs ( -76543.1 )
union all
select *  from tab5
   where trunc(spalte2 ) between trunc (- 5.1) and abs ( -76543.1 ) !
fetch !
declare erg cursor for
select * from tab5
   where - trunc( ABS( spalte4 ) ) +3*ABS(-.0)
         +abs  (-abs  (TRUNC (6*tab5.spalte3 )/ 2)+ 18 )
       < trunc(28.71* 1000 ) or trunc ( spalte2)>0 !
fetch erg !
declare erg cursor for
select * from tab5
   where - trunc( ABS( spalte4 ) ) +3*ABS(-.0)
         +abs  (-abs  (TRUNC (6*tab5.spalte3 )/ 2)+ 18 )
       < trunc(28.71* 1000 ) or trunc ( spalte2)>0
union
select *  from tab5
   where - trunc( ABS( spalte4 ) ) +3*ABS(-.0)
         +abs  (-abs  (TRUNC (6*tab5.spalte3 )/ 2)+ 18 )
       < trunc(28.71* 1000 ) or trunc ( spalte2)>0 !
fetch erg !
declare erg cursor for
select * from tab5
   where - trunc( ABS( spalte4 ) ) +3*ABS(-.0)
         +abs  (-abs  (TRUNC (6*tab5.spalte3 )/ 2)+ 18 )
       < trunc(28.71* 1000 ) or trunc ( spalte2)>0
union all
select * from tab5
   where - trunc( ABS( spalte4 ) ) +3*ABS(-.0)
         +abs  (-abs  (TRUNC (6*tab5.spalte3 )/ 2)+ 18 )
       < trunc(28.71* 1000 ) or trunc ( spalte2)>0 !
fetch erg !
select  spalte1 + abs(2*spalte4),
        spalte3 + TRUNC ( kern.tab5.spalte2 ),
        trunc (6 +abs (-12.7))
        from tab5 !
fetch !
select  spalte1 + abs(2*spalte4),
        spalte3 + TRUNC ( kern.tab5.spalte2 ),
        trunc (6 +abs (-12.7))
        from tab5
union
select  spalte1 + abs(2*spalte4),
        spalte3 + TRUNC ( kern.tab5.spalte2 ),
        trunc (6 +abs (-12.7))
        from tab5 !
fetch !
select  spalte1 + abs(2*spalte4),
        spalte3 + TRUNC ( kern.tab5.spalte2 ),
        trunc (6 +abs (-12.7))
        from tab5
union all
select  spalte1 + abs(2*spalte4),
        spalte3 + TRUNC ( kern.tab5.spalte2 ),
        trunc (6 +abs (-12.7))
        from tab5 !
fetch !
select *  from tab5, tab6 ref
   where +abs(tab5.spalte1 + ref.spalte1 )
       > trunc(ref.spalte1 -  ABS( -ref.col2 )) !
fetch !
select *  from tab5, tab6 ref
   where +abs(tab5.spalte1 + ref.spalte1 )
       > trunc(ref.spalte1 -  ABS( -ref.col2 ))
union
select *  from tab5, tab6 ref
   where +abs(tab5.spalte1 + ref.spalte1 )
       > trunc(ref.spalte1 -  ABS( -ref.col2 )) !
fetch !
select *  from tab5, tab6 ref
   where +abs(tab5.spalte1 + ref.spalte1 )
       > trunc(ref.spalte1 -  ABS( -ref.col2 ))
union all
select *  from tab5, tab6 ref
   where +abs(tab5.spalte1 + ref.spalte1 )
       > trunc(ref.spalte1 -  ABS( -ref.col2 )) !
fetch !
select  3*trunc(  spalte3 ) / abs(ref.col2 * abs(-1.0)),
        abs ( col2 ) neu,
        kern.tab5.spalte4 + trunc(-32.7 )
  from tab5,tab6 ref where tab5.spalte3 is not null !
fetch !
select  3*trunc(  spalte3 ) / abs(ref.col2 * abs(-1.0)),
        abs ( col2 ) neu,
        kern.tab5.spalte4 + trunc(-32.7 )
  from tab5,tab6 ref where tab5.spalte3 is not null
union
select  3*trunc(  spalte3 ) / abs(ref.col2 * abs(-1.0)),
        abs ( col2 ),
        kern.tab5.spalte4 + trunc(-32.7 )
  from tab5,tab6 ref where tab5.spalte3 is not null !
fetch !
select  3*trunc(  spalte3 ) / abs(ref.col2 * abs(-1.0)),
        abs ( col2 ) neu,
        kern.tab5.spalte4 + trunc(-32.7 )
  from tab5,tab6 ref where tab5.spalte3 is not null
union all
select  3*trunc(  spalte3 ) / abs(ref.col2 * abs(-1.0)),
        abs ( col2 ),
        kern.tab5.spalte4 + trunc(-32.7 )
  from tab5,tab6 ref where tab5.spalte3 is not null !
fetch !
drop table tab5 !
drop table tab6 !
commit release !
