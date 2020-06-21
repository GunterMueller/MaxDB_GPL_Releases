*   *ID* N35GROU6 DBCMD    changed on 1992-09-28-13.26.20 by ADMIN     *
select distinct  sum (spalte2 )
   from tab where spalte4 <> 400000 group by tab.spalte1
   having kern.tab.spalte1 is  null or not avg(spalte4) is null !
fetch !
select distinct ( sum( distinct spalte3 ) )
   from tab where spalte3 not between 40 and 60 or spalte3 is null
   group by tab.spalte1 having ( not (avg(tab.spalte3) is null )) !
fetch !
