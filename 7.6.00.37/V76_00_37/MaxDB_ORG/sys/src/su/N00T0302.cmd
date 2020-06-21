*   *ID* N00T0302 DBCMD    changed on 1992-09-28-13.24.44 by ADMIN     *
 *** 3.9  <set function spec> !
file connect ( kern test !
create table t1 ( spalte1 char(10),
                  spalte2 fixed(5),
                  spalte3 fixed(5) ) !
insert t1 values ( null, 5, 60 ) !
insert t1 values ( 'aa', 5, 40 ) !
insert t1 values ( 'bb', null, 40 ) !
insert t1 values ( 'aa', 1, 10 ) !
insert t1 values ( 'aa', 1, null ) !
select count(*), sum ( distinct 2*spalte2 ),
       max(spalte2), count ( all 'd' )
   from t1 !
fetch !
select spalte1, count ( distinct spalte2), count (spalte2),
       min ( all ( 2*spalte3 )), avg ( distinct ( spalte3 ) )
   from t1 group by spalte1 having sum ( spalte3 ) >= 50 !
fetch !
rollback work release !
