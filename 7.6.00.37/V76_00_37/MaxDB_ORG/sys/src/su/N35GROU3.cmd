*   *ID* N35GROU3 DBCMD    changed on 1992-09-28-13.26.19 by ADMIN     *
select sum( spalte2) , avg(spalte2) , count( spalte2 )
    from tab where spalte1 <'c' group by spalte1 !
fetch !
select Count  ( *   ), count ( all spalte4 ) , count(distinct spalte4)
    from tab where spalte2 <= 100 group by spalte1 !
fetch !
select ( avg( tab.spalte3))
   from tab
   where spalte1 is null
      or tab.spalte3 is null
   group by spalte1, spalte2 !
fetch !
rollback work !
