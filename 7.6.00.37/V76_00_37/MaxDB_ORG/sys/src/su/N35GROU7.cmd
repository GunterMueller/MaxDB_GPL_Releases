*   *ID* N35GROU7 DBCMD    changed on 1992-09-28-13.26.20 by ADMIN     *
select spalte2, sum ( spalte2 )
   from tab
   where spalte4 > 0
      or spalte4 is null
   group by spalte2
   order by 2 !
fetch !
select avg(spalte4),sum(spalte4)
   from tab
   group by spalte1
   order by 1 desc, 2 asc !
fetch !
select count ( *), sum ( spalte2 ) summe 
   from tab
   group by spalte3
   having count ( distinct spalte1 ) > 1
   order by summe !
fetch !
select spalte1, sum ( spalte2 )
   from tab
   where spalte2 = 100
   group by spalte1
   order by 2 !
fetch !
select spalte4, sum ( spalte2 )
   from tab
   where spalte3 = 1E-36
   group by spalte4
   having count ( spalte4 ) > 1 !
fetch !
select spalte1, spalte2, spalte3, count ( all tab.spalte2 )
   from tab
   where spalte1 is not null
   group by spalte1, kern.tab.spalte2, spalte3, tab.spalte4
   order by spalte3 asc, 4 desc !
fetch !
declare erg0 cursor for 
select distinct sum (spalte2)
   from tab
   where spalte4 <> 400000
   group by tab.spalte1
   order by 1 !
fetch erg0 !
declare erg3 cursor for 
select all sum(tab.spalte2), avg( spalte2 ) summe
   from tab
   group by spalte4, spalte3
   having tab.spalte3 <> 50
   order by 2 desc, summe DESC !
fetch erg3 !
rollback work !
