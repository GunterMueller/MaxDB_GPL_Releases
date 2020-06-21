*   *ID* N55VIEW2 DBCMD    changed on 1992-09-28-13.26.53 by ADMIN     *
file connect ( kern test !
create table t1 ( t1xc1 fixed(2) key, t1xc2 fixed(2) key,
                  t1xc3 fixed(2) ) !
create table t2 ( t2xc1 fixed(2) key, t2xc2 fixed(2), t2xc3 fixed(2) ) !
insert t1 values ( 2, 3, 5 ) !
insert t1 values ( 4, 1, 3 ) !
insert t1 values ( 8, 3, 1 ) !
insert t2 values ( 4, 2, 6 ) !
insert t2 values ( 8, 1, 5 ) !
insert t2 values ( 2, 5, 5 ) !
commit work !
create view schrott1 ( sp ) as select t1xc1, 2 + t2xc3 from t1,t2 !
create view schrott2 ( sp1, sp2 ) as select 3 + t2xc3 from t1,t2 !
create view schrott3 ( sp1, sp1 ) as select 3,  t2xc3 from t1,t2 !
create view v1 as select
   3 * t1xc2 neu1, t2xc1 + t2xc2, t2xc3 expression1
   from t1,t2 !
create view v1 as select
   3 * t1xc2 neu1, t2Xc3 expression1, t2xc1 + t2xc2
   from t1,t2 !
create view v1 as select 3 * t1xc2 neu1, t2xc3, t2xc1 + t2xc2
   from t1,t2 !
select * from v1 !
fetch !
declare erg1 cursor for 
select neu1 neu2, expression1 + 1 
   from v1 where neu1 < t2xc3 !
fetch erg1 !
create view v2 as select ( t1xc3 + t2.t2xc1 ) neu1 from t1,t2
   where t1xc1 < t2xc3 !
select * from v2 !
fetch !
select neu1 from v2 !
fetch !
create view v3 ( s1, s2, s3, s4, s5 )
   as select t1xc2 + t2xc3, t1xc2, t2.t2xc3, t1xc1 spalte, t2xc1
   from t1,t2 where t1xc1 = t2xc1 !
select * from v3 !
fetch !
select s1, s2, s3, v3.s4, kern.v3.s5 from v3 !
fetch !
select s1 + s2, s4 + s5 neu from v3 !
fetch !
select t2xc1 from v3 !
select s3, count(s3), sum ( s4 ) from v3 group by s3 order by 3 asc !
fetch !
select sum ( s2+s5), sum ( s1 +s5 ) from v3
   group by s3 having avg ( s4) = 5 !
fetch !
create view v4 ( c1, c2 ) as select sum ( t1xc2 ), count ( t2xc1 )
   from t1,t2 !
create view v5 ( c1,c2,c3,c4,c5,c6 ) as select * from t1,t2
   where t1xc2 + 2 = t2xc3 !
select * from v5 !
fetch !
select distinct c4, c5 , 7 from v5 !
fetch !
drop table t1 !
drop table t2 !
commit work release !
