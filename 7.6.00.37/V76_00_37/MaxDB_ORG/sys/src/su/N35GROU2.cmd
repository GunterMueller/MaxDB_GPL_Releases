*   *ID* N35GROU2 DBCMD    changed on 1992-09-28-13.26.18 by ADMIN     *
select sum ( spalte2), sum (tab.spalte3 ), sum ( kern.tab.spalte4 )
       from tab group by spalte1 !
fetch !
select  avg ( kern.tab.spalte2 ),AVG(  tab.spalte3), avg(spalte4)
   from tab GROUP BY  kern.tab.spalte1 !
fetch !
select  count(*), count (distinct spalte2), count (all tab.spalte3 ),
        count( kern.tab.spalte4 )
      from tab   group  by   tab.spalte1 !
fetch !
select sum ( distinct spalte2 ), avg(spalte2), count(spalte1)
     from kern.tab group by spalte2 !
fetch !
select count(6), sum ( 5 ), avg (4)
     from tab group by spalte1, tab.spalte2 !
fetch !
select ( sum (2*spalte2) )
  from tab group by tab.spalte2, spalte1, kern.tab.spalte3, spalte4 !
fetch !
declare erg2 cursor for 
select avg ( spalte2 + spalte3 ), count ( *) ,
              count ( distinct   spalte4 * tab.spalte4 )
   from tab group by spalte1 !
fetch erg2 !
select avg ( tab.spalte4 ) average , count(*),
       count( distinct spalte4 ) , count (spalte4) anzahl
       from tab group by spalte4 !
fetch !
select distinct sum ( spalte2), ( sum( 3*spalte2 ) )
    from tab group  by spalte2 !
fetch !
select all ( sum (spalte2) ) , sum ( 3*  spalte2 )
    from tab group by spalte2 !
fetch !
rollback work !
