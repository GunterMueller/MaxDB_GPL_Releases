*   *ID* N35GROU1 DBCMD    changed on 1992-09-28-13.26.18 by ADMIN     *
select spalte1 from tab group by spalte1 !
fetch !
select spalte2 from kern.tab group by spalte2, spalte3 !
fetch !
select spalte1, spalte2 , ( spalte3 ), spalte4 from tab
    group by tab.spalte1, spalte2, kern.tab.spalte3, spalte4 !
fetch !
select spalte2 from tab group by spalte1 !
fetch !
select 5 from tab group by spalte1 !
fetch !
declare erg1 cursor for 
select 2 * spalte2 from tab group by spalte2 !
fetch erg1 !
select sum ( spalte2 ) summe  from tab group by spalte2 !
fetch !
select ( spalte2 + spalte3 ) from tab group by spalte2, spalte3 !
fetch !
select distinct spalte1 from tab group by spalte1 !
fetch !
select spalte2+spalte3  from tab group by spalte2 !
select spalte1, sum ( spalte2 ) from tab group by spalte1 !
fetch !
rollback work !
