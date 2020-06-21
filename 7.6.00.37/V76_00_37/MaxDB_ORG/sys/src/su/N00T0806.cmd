*   *ID* N00T0806 DBCMD    changed on 1992-09-28-13.24.56 by ADMIN     *
 ***  8.9 <create link statement> !
file connect ( kern test !
create table t1 ( t1xc1 fixed(3) key ) !
create table t2 ( t2xc1 fixed(3) key,
   t2xc2 fixed(3), t2xc3 fixed(3), t2xc4 fixed(3) ) !
create table t3 ( t3xc1 fixed(3) key, t3xc2 fixed(3) ) !
create table t4 ( t4xc1 fixed(3) key, t4xc2 fixed(3) ) !
create table t5 ( t5xc1 fixed(3) key,
   t5xc2 fixed(3), t5xc3 fixed(3) default 0 ) !
create table t6 ( t6xc1 fixed(3) key, t6xc2 fixed(3) ) !
alter table t2 foreign key l12c1 ( t2xc2 )
   references t1 on delete cascade !
alter table t3 foreign key l13c1 ( t3xc2 )
   references t1 on delete cascade !
alter table t2 foreign key l32c1 ( t2xc3 )
   references t3 on delete cascade !
alter table t2 foreign key l22c1 ( t2xc4 )
   references t2 on delete cascade !
alter table t6 foreign key l36r1 ( t6xc2 )
   references t3 on delete restrict !
alter table t6 foreign key l26r1 ( t6xc1 )
   references t2 on delete restrict !
alter table t4 foreign key l34c1 ( t4xc2 )
   references t3 on delete cascade !
alter table t5 foreign key l45n1 ( t5xc2 )
   references t4 on delete set null !
alter table t5 foreign key l55d1 ( t5xc3 )
   references t5 on delete set default !
alter table t3 foreign key l53c1 ( t3xc2 )
   references t5 on delete restrict !
commit work !
insert t1 values ( 5 ) !
insert t5 values ( 5, null, 5 ) !
insert t3 values ( 1, 5 ) !
insert t2 values ( 4, null, 1, null ) !
insert t2 values ( 7, null, null, 4 ) !
insert t2 values ( 3, 5, null, 3 ) !
insert t2 values ( 8, null, null, 3 ) !
insert t4 values ( 3, 1 ) !
insert t5 values ( 6, 3, 5 ) !
insert t2 values ( 6, null, null, 6 ) !
insert t3 values ( 4, null ) !
insert t6 values ( 6, 4 ) !
commit work !
delete t1 where t1xc1 = 5 !
insert t4 values ( 3, null ) !
delete t5 key t5xc1 = 5 where t5xc2 is null !
commit work !
select * from t2 !
fetch !
select * from t3 !
fetch !
select * from t5 !
fetch !
drop table t1 !
drop table t2 !
drop table t3 !
drop table t4 !
drop table t5 !
drop table t6 !
commit work release !
