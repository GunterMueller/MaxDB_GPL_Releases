*   *ID* N61V1    DBCMD    changed on 1992-09-28-13.26.58 by ADMIN     *
file connect ( kern test !
create table t1 ( spalte1 char(10),
                  spalte2 fixed(5),
                  spalte3 fixed(5) ) !
insert t1 values ( null, 5, 60 ) !
insert t1 values ( 'aa', 5, 40 ) !
insert t1 values ( 'bb', null, 40 ) !
insert t1 values ( 'aa', 1, 10 ) !
insert t1 values ( 'aa', 1, null ) !
create view v1 as
select count(*), sum ( distinct 2*spalte2 ),
       max(spalte2), count ( all 'd' )
   from t1 !
select * from v1 !
fetch !
create view v2 as
select spalte1, count ( distinct spalte2), count (spalte2),
       min ( all ( 2*spalte3 )), avg ( distinct ( spalte3 ) )
   from t1 group by spalte1 having sum ( spalte3 ) >= 50 !
select * from v2 !
fetch !
declare erg1 cursor for 
select v1.expression1 n1, v2.expression1 n2
   from v1, kern.v2
   where v2.expression2 + v2.expression1 = v1.expression3 !
select * from erg1 !
fetch !
rollback work release !
