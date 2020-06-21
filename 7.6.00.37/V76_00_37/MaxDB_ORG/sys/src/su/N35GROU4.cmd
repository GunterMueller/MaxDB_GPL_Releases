*   *ID* N35GROU4 DBCMD    changed on 1992-09-28-13.26.19 by ADMIN     *
select sum ( spalte2 ) from tab having count(*) > 8 !
fetch !
select sum (spalte2) from tab having count ( * ) > 9 !
select sum (spalte2) from tab having sum ( spalte2 ) > -700 !
fetch !
select spalte2 from tab having COUNT (*) > 1 !
select avg( distinct spalte2) , sum ( SPALTE3 )
   from tab having count ( *) > 0 and count ( distinct spalte2) <6.2 !
fetch !
select all count(*) , count ( all spalte4 )
   from tab having count ( all spalte3 ) = 6 !
fetch !
select distinct ( sum ( tab.spalte2 )) from tab having 5 < 6 !
fetch !
rollback work !
