*   *ID* N35GROU5 DBCMD    changed on 1992-09-28-13.26.19 by ADMIN     *
select avg( spalte2 ) , tab.spalte1
    from tab group by spalte1 having count(*) > 1 !
fetch !
select sum(spalte4)
    from tab group by spalte1, spalte4 having tab.spalte1 > 'c' !
select spalte4
    from tab group by spalte4 having count ( distinct spalte4) >2 !
fetch !
select spalte1
    from tab group by spalte1 having count ( spalte2 ) > 0 !
fetch !
select sum ( spalte2 ), kern.tab.spalte2, avg ( tab.spalte2 )
    from tab group by spalte2 having sum ( spalte2 ) > 0 !
fetch !
select avg ( spalte2 )
    from tab group by spalte2 having avg ( spalte2) = spalte2 !
fetch !
select sum ( spalte2 + 100 )
    from tab group by tab.spalte4 having sum ( distinct spaLTE2 ) > 0 !
fetch !
select sum ( distinct spalte2 )
    from tab group by tab.spalte4 having sum ( distinct spalte2 ) > 0 !
fetch !
select sum ( spalte3 ) , avg ( spalte3 )
    from tab group by spalte2
    having sum ( spalte3) <0 or avg(spalte3 ) = sum ( spalte3 ) !
fetch !
select sum (spalte2)
    from tab group by spalte1 having spalte1 is null !
fetch !
select spalte2, spalte3, sum ( spalte2 )
    from tab group by spalte2, spalte3
    having  sum ( spalte2 + spalte3 ) = 3 *(100 + 1E-36 ) !
fetch !
rollback work !
