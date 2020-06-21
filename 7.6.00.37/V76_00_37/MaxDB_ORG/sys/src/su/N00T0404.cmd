*   *ID* N00T0404 DBCMD    changed on 1992-09-28-13.24.47 by ADMIN     *
 ***  4.1.3  <subquery> !
file connect ( kern test !
create table t1 ( t1xc1 fixed(5) ) !
create table t2 ( t2xc1 fixed(5) ) !
create table t3 ( t3xc1 fixed(5) ) !
create table t4 ( t4xc1 fixed(5) ) !
create table t5 ( t5xc1 fixed(5) ) !
insert t1 values ( 5 ) !
insert t1 values ( 1 ) !
insert t2 values ( 6 ) !
insert t2 values ( 2 ) !
insert t2 values ( 5 ) !
insert t3 values ( 2 ) !
insert t3 values ( null ) !
insert t4 values ( 6 ) !
insert t4 values ( 1 ) !
insert t5 values ( 5 ) !
select * from t1
   where t1xc1 = ( select count(*) from t2
   where ( t2xc1 not in ( select * from t3
   where t3xc1 < any ( select sum(t4xc1) from t4
   where exists ( select * from t5 ) group by t4xc1 )
     and t3xc1 > 0 ) ) group by t2xc1 having t2xc1 > 5 ) !
fetch !
rollback work release !
