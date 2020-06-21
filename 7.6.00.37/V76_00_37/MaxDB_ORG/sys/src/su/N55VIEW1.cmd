*   *ID* N55VIEW1 DBCMD    changed on 1992-09-28-13.26.53 by ADMIN     *
file connect ( kern test !
create table t1 ( t1xc1 fixed(2) key, t1xc2 fixed(2) key,
                  t1xc3 fixed(2) ) !
create table t2 ( t2xc1 fixed(2) key, t2xc2 fixed(2), t2xc3 fixed(2) ) !
create table t3 ( t3xc1 fixed(2), t3xc2 fixed(2), t3xc3 fixed(2) ) !
create view v1 as select * from t1, t2, t3
   where t1xc1 = t2xc1 and t2xc2 = t3xc1 !
create view v2 as select * from t1,t2,t3
   where t1.t1xc3 = kern.t2.t2xc2 and (( t3xc1 + 1 ) < t2xc3 ) !
insert t1 values ( 2, 3, 5 ) !
insert t1 values ( 4, 1, 3 ) !
insert t1 values ( 8, 3, 1 ) !
insert t2 values ( 4, 2, 6 ) !
insert t2 values ( 8, 1, 5 ) !
insert t2 values ( 2, 5, 5 ) !
insert t3 values ( 2, 3, 3 ) !
insert t3 values ( 1, 6, 6 ) !
insert t3 values ( 1, 2, 4 ) !
insert t3 values ( 5, 2, 3 ) !
insert t3 values ( 3, 1, 7 ) !
commit work !
select * from v1 !
fetch !
select * from v2 !
fetch !
commit work !
create view v3 as select
   v1.t1xc1 v1t11, v1.t1xc2 v1t12, v1.t2xc1 v1t21,
   v1.t3xc1 v1t31, v1.t3xc3 v1t33,
   v2.t1xc1 v2t11, v2.t1xc2 v2t12, v2.t2xc1 v2t21,
   v2.t3xc1 v2t31, v2.t3xc3 v2t33
   from v1,v2
   where v1.t1xc1 = v2.t1xc1 and v1.t1xc2 = v2.t3xc1 !
select * from v3 !
fetch !
select * from v3 where v1t33 < v2t21 !
fetch !
select k.v1t12, v1t33, v2t21 neu from v3 k
   where k.v1t12 < v1t33 and ( v1t33 * 2 = k.v2t21 ) !
fetch !
declare erg1 cursor for 
select v3.v1t11, kern.v3.v1t33 , k.v2t33
   from v3, v3 k
   where v3.v1t11 < k.v2t33 !
select * from erg1 !
fetch !
select v3.v1t11, sum (v1t33) from v3 group by v3.v1t11 !
fetch !
select v1t33 + v2t21 from v3 !
fetch !
drop table t1 !
drop table t2 !
drop table t3 !
commit work release !
