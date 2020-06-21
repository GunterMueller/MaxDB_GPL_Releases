*   *ID* N32SUM1  DBCMD    changed on 1992-09-28-13.26.13 by ADMIN     *
select sum (5), ( sum ( all 5 )), sum ( distinct ( + 05.0)),
       sum ( -5 ), sum (0)
       from tab !
fetch !
select all sum ( distinct 3.1 ) from tab !
fetch !
select all (sum(all 3) )  from tab !
fetch !
select sum ( distinct 3 ), sum ( distinct 5 ) from tab !
fetch !
select sum ( distinct 5 ), sum ( 5 ), sum ( distinct 5 ) from tab !
fetch !
select sum ( 3 ), sum ( distinct 5 ), sum ( 5 ) from tab !
fetch !
select sum(spalte2), sum ( tab.spalte3 ) , SUM ( kern.tab.spalte4 )
   from tab !
fetch !
select sum ( tab.spalte3 ) , sum ( kern.tab.spalte2 ) , sum(spalte4)
   from tab where spalte1 = 'a' !
fetch !
select sum ( all spalte2 ) , sum ( all tab.spalte3 ) ,
       sum  (   all  ( kern.tab.spalte4 ) )
   from tab where spalte2 is not null !
fetch !
select ( sum ( all kern.tab.spalte3 ) ), sum ( distinct spalte3 )
   from tab !
fetch !
select sum ( DISTINCT ( spalte2)), sum(distinct kern.tab.spalte4),
       sum ( distinct tab.spalte3 )
   from tab !
fetch !
select sum ( DISTINCT ( spalte2)), sum(distinct kern.tab.spalte4),
       sum ( distinct tab.spalte3 )
   into :a, :b, :c
   from tab !
select sum(spalte2 ) from tab where spalte2 is null !
fetch !
select sum(spalte2 ) from tab where spalte1 = 'd' !
fetch !
select sum(spalte2 ), spalte1 from tab where spalte1 = 'd' !
fetch !
select SUM ( spalte3 + tab.spalte4 * 2 ),
       sum ( DISTINCT kern.tab.spalte2/2 )
       from tab !
fetch !
select SUM ( spalte3 + tab.spalte4 * 2 ),
       sum ( DISTINCT kern.tab.spalte2/2 )
       into :a, :b
       from tab !
select distinct sum ( distinct spalte2 ) from tab !
fetch !
select distinct sum ( spalte2), sum ( distinct spalte2 ) from tab !
fetch !
select spalte2, sum ( spalte2) from tab !
select sum (spalte2), 5 from tab !
select 5, sum (spalte2) from tab !
select * , sum (spalte2) from tab !
select sum (*) from tab !
select sum ( spalte1 ) from tab !
commit work !
