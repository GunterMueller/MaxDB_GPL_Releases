*   *ID* N35GROU8 DBCMD    changed on 1992-09-28-13.26.21 by ADMIN     *
* PTS 1127182, 1127594-5 !
declare erg4 cursor for
select count(*) spalte1, sum ( spalte2) spalte2,
       avg ( spalte2 ) spalte3 
    from tab group by spalte1 !
fetch erg4 !
select * from erg4 !
close erg4 !
declare erg4 cursor for
select count(*) spalte1, sum ( spalte2) spalte2,
       avg ( spalte2 ) spalte3 
    from tab group by spalte1 for reuse !
fetch erg4 !
select * from erg4 !
fetch !
pars_then_ex !
declare erg4 cursor for
select count(*) spalte1, sum ( spalte2) spalte2,
       avg ( spalte2 ) spalte3 
    from tab group by spalte1 !
fetch erg4 !
select * from erg4 !
fetch !
close erg4 !
declare erg4 cursor for
select count(*) spalte1, sum ( spalte2) spalte2,
       avg ( spalte2 ) spalte3 
    from tab group by spalte1 for reuse !
fetch erg4 !
select * from erg4 !
fetch !
pars_execute !
select 3 * spalte1 + erg4.spalte2 from erg4 where erg4.spalte1 > 1 !
fetch !
select sum ( spalte1 ) , count ( distinct spalte2 ) , spalte3
    from erg4 group by spalte3 !
fetch !
declare erg5 cursor for 
select sum(spalte1) col1, avg( spalte1 + 3 ) col2 
    from erg4 group by erg4.spalte3 for reuse !
fetch erg5 !
declare erg6 cursor for 
select distinct * from erg5 !
fetch erg6 !
create view v1 as select spalte2 from tab !
select spalte2, count(spalte2) from v1 group by spalte2 !
fetch !
rollback work !
