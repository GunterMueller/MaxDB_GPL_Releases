*   *ID* N61V2    DBCMD    changed on 1992-09-28-13.26.59 by ADMIN     *
file connect ( kern test !
create table t1 ( t1xc1 fixed(3) key, t1xc2 fixed(3) ) !
create table t2 ( t2xc1 fixed(3), t2xc2 fixed(3) ) !
create view v1 as select * from t1,t2 where t1xc1 = t2xc1 !
insert t1 values ( 3, 11 ) !
insert t1 values ( 5, 44 ) !
insert t1 values ( 7, 11 ) !
insert t1 values ( 1, 11 ) !
insert t2 values ( 5, 66 ) !
insert t2 values ( 5, 66 ) !
insert t2 values ( 1, 11 ) !
insert t2 values ( 5, 11 ) !
insert t2 values ( 7, 22 ) !
insert t2 values ( 7, 11 ) !
select * from v1 !
fetch !
create view v3 as
select count(*) * 3, ((max(t1xc1 + 6) + sum(t2xc1)) * t1xc2) mod 4 col
   from v1 where t2xc2 + t1xc1 > t1xc2
   group by t1xc2, t2xc1
   having count(*) + 30 < t1xc2 + sum(t2xc2) !
select * from v3 !
fetch !
rollback work release !
