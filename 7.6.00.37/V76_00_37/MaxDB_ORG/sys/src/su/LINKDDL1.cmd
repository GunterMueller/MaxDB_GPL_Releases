*   *ID* LINKDDL1 DBCMD    changed on 1992-09-28-13.24.30 by ADMIN     *
file connect ( kern test !
create table t1 (k1 integer key, s1 integer) !
create table f_t1 (k1 integer key, s1 integer) !
create table f_t2 (k1 integer key, s1 integer) !
create table f_t3 (k1 integer key, s1 integer) !
create table f_t4 (k1 integer key, s1 integer) !
commit !
create table f_t5 (k1 integer key, s1 integer) !
create table f_t6 (k1 integer key, s1 integer) !
create table f_t7 (k1 integer key, s1 integer) !
create table f_t8 (k1 integer key, s1 integer) !
create table f_t9 (k1 integer key, s1 integer) !
commit !
create table f_t10 (k1 integer key, s1 integer) !
create table f_t11 (k1 integer key, s1 integer) !
create table f_t12 (k1 integer key, s1 integer) !
create table f_t13 (k1 integer key, s1 integer) !
create table f_t14 (k1 integer key, s1 integer) !
commit !
create table f_t15 (k1 integer key, s1 integer) !
create table f_t16 (k1 integer key, s1 integer) !
create table k_t1 (k1 integer key, s1 integer) !
create table k_t2 (k1 integer key, s1 integer) !
commit !
create table k_t3 (k1 integer key, s1 integer) !
create table k_t4 (k1 integer key, s1 integer) !
create table k_t5 (k1 integer key, s1 integer) !
create table k_t6 (k1 integer key, s1 integer) !
commit !
create table k_t7 (k1 integer key, s1 integer) !
create table k_t8 (k1 integer key, s1 integer) !
create table k_t9 (k1 integer key, s1 integer) !
create table k_t10 (k1 integer key, s1 integer) !
create table k_t11 (k1 integer key, s1 integer) !
commit !
create table k_t12 (k1 integer key, s1 integer) !
create table k_t13 (k5 integer key, s1 integer) !
create table k_t14 (k5 integer key, s1 integer) !
create table k_t15 (k5 integer key, s1 integer) !
create table k_t16 (k5 integer key, s1 integer) !
commit !
create view v1 as select * from f_t1 !
create view v2 as select * from f_t2 !
create view v3 as select * from f_t3 !
create view v4 as select * from f_t4 !
create view v5 as select * from f_t5 !
create view v6 as select * from f_t6 !
commit !
create view v7 as select * from f_t7 !
create view v8 as select * from f_t8 !
create view v9 as select * from f_t9 !
create view v10 as select * from f_t10 !
create view v11 as select * from f_t11 !
create view v12 as select * from f_t12 !
create view v13 as select * from f_t13 !
commit !
create view v14 as select * from f_t14 !
create view v15 as select * from f_t15 !
create view v16 as select * from f_t16 !
create view vv1 as select * from f_t1 !
commit !
create view vv2 as select * from f_t2 !
create view vv3 as select * from f_t3 !
create view vv4 as select * from f_t4 !
create view vv5 as select * from f_t5 !
commit !
create view vv6 as select * from f_t6 !
create view vv7 as select * from f_t7 !
create view vv8 as select * from f_t8 !
create view vv9 as select * from f_t9 !
commit !
create view vv10 as select * from f_t10 !
create view vv11 as select * from f_t11 !
create view vv12 as select * from f_t12 !
create view vv13 as select * from f_t13 !
create view vv14 as select * from f_t14 !
commit !
create view vv15 as select * from f_t15 !
create view vv16 as select * from f_t16 !
alter table f_t1 foreign key l1 (s1) references t1 on delete restrict !
commit !
alter table f_t2 foreign key l2 (s1) references t1 on delete restrict !
commit !
alter table f_t3 foreign key l3 (s1) references t1 on delete restrict !
commit !
alter table f_t4 foreign key l4 (s1) references t1 on delete restrict !
commit !
alter table f_t5 foreign key l5 (s1) references t1 on delete restrict !
commit !
alter table f_t6 foreign key l6 (s1) references t1 on delete restrict !
commit !
alter table f_t7 foreign key l7 (s1) references t1 on delete restrict !
commit !
alter table f_t8 foreign key l8 (s1) references t1 on delete restrict !
commit !
alter table f_t9 foreign key l9 (s1) references t1 on delete restrict !
commit !
alter table f_t10 foreign key l10 (s1) references t1 on delete restrict !
commit !
alter table f_t11 foreign key l11 (s1) references t1 on delete restrict !
commit !
alter table f_t12 foreign key l12 (s1) references t1 on delete restrict !
commit !
alter table f_t13 foreign key l13 (s1) references t1 on delete restrict !
commit !
alter table f_t14 foreign key l14 (s1) references t1 on delete restrict !
commit !
alter table f_t15 foreign key l15 (s1) references t1 on delete restrict !
commit !
alter table f_t16 foreign key l16 (s1) references t1 on delete restrict !
commit !
alter table t1 foreign key l1 (s1) references k_t1 on delete restrict !
commit !
alter table t1 foreign key l1 (s1) references k_t2 on delete restrict !
commit !
alter table t1 foreign key l1 (s1) references k_t3 on delete restrict !
commit !
alter table t1 foreign key l1 (s1) references k_t4 on delete restrict !
commit !
alter table t1 foreign key l1 (s1) references k_t5 on delete restrict !
commit !
alter table t1 foreign key l1 (s1) references k_t6 on delete restrict !
commit !
alter table t1 foreign key l1 (s1) references k_t7 on delete restrict !
commit !
alter table t1 foreign key l1 (s1) references k_t8 on delete restrict !
commit !
alter table t1 foreign key l1 (s1) references k_t9 on delete restrict !
commit !
alter table t1 foreign key l1 (s1) references k_t10 on delete restrict !
commit !
alter table t1 foreign key l1 (s1) references k_t11 on delete restrict !
commit !
alter table t1 foreign key l1 (s1) references k_t12 on delete restrict !
commit !
alter table t1 foreign key l1 (s1) references k_t13 on delete restrict !
commit !
alter table t1 foreign key l1 (s1) references k_t14 on delete restrict !
commit !
alter table t1 foreign key l1 (s1) references k_t15 on delete restrict !
commit !
alter table t1 foreign key l1 (s1) references k_t16 on delete restrict !
commit !
insert k_t1  values (1, 2) !
insert k_t2  values (1, 2) !
insert k_t3  values (1, 2) !
insert k_t4  values (1, 2) !
insert k_t5  values (1, 2) !
insert k_t6  values (1, 2) !
insert k_t7  values (1, 2) !
insert k_t8  values (1, 2) !
insert k_t9  values (1, 2) !
insert k_t10 values (1, 2) !
insert k_t11 values (1, 2) !
insert k_t12 values (1, 2) !
insert k_t13 values (1, 2) !
insert k_t14 values (1, 2) !
insert k_t15 values (1, 2) !
insert k_t16 values (1, 2) !
commit !
insert t1 values (100, 1) !
insert t1 values (200, 2) !
commit !
insert f_t1 values (501, 100) !
insert f_t2 values (502, 100) !
insert f_t3 values (503, 100) !
insert f_t4 values (504, 100) !
insert f_t5 values (505, 100) !
insert f_t6 values (506, 100) !
insert f_t7 values (507, 100) !
insert f_t8 values (508, 100) !
insert f_t9 values (509, 100) !
insert f_t10 values (510, 100) !
insert f_t11 values (512, 100) !
insert f_t12 values (513, 100) !
insert f_t13 values (514, 100) !
insert f_t14 values (515, 100) !
insert f_t15 values (516, 100) !
insert f_t16 values (517, 100) !
create view foreign_tables (a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p)
  as select vv1.k1, vv2.k1, vv3.k1, vv4.k1, vv5.k1,
            vv6.k1, vv7.k1, vv8.k1, vv9.k1, vv10.k1,
            vv11.k1, vv12.k1, vv13.k1, vv14.k1, vv15.k1, vv16.k1
  from vv1, vv2, vv3, vv4, vv5, vv6, vv7, vv8, vv9, vv10,
       vv11, vv12, vv13, vv14, vv15, vv16
  where
       vv1.s1 = vv2.s1 and  vv3.s1 = vv4.s1 and vv5.s1 =
       vv6.s1 and vv7.s1 = vv8.s1 and vv9.s1 = vv10.s1 and
       vv11.s1 = vv12.s1 and vv13.s1 = vv14.s1 and vv15.s1 = vv16.s1 !
commit !
select * from foreign_tables !
fetch !
drop table t1 !
SELECT OWNER,TABLENAME,COLUMNNAME,REFNAME,
       REFOWNER,REFTABLENAME,REFCOLUMNNAME,
       RULE,"DATE","TIME"
  FROM SYSDD.FOREIGN_KEYS!
fetch !
commit !
select * from v1 !
fetch !
select * from v2 !
fetch !
select * from v3 !
fetch !
select * from v4 !
fetch !
select * from v5 !
fetch !
select * from v6 !
fetch !
select * from v7 !
fetch !
select * from v8 !
fetch !
select * from v9 !
fetch !
select * from v10 !
fetch !
select * from v11 !
fetch !
select * from v12 !
fetch !
select * from v13 !
fetch !
select * from v14 !
fetch !
select * from v15 !
fetch !
select * from v16 !
fetch !
select * from vv1 !
fetch !
select * from vv2 !
fetch !
select * from vv3 !
fetch !
select * from vv4 !
fetch !
select * from vv5 !
fetch !
select * from vv6 !
fetch !
select * from vv7 !
fetch !
select * from vv8 !
fetch !
select * from vv9 !
fetch !
select * from vv10 !
fetch !
select * from vv11 !
fetch !
select * from vv12 !
fetch !
select * from vv13 !
fetch !
select * from vv14 !
fetch !
select * from vv15 !
fetch !
select * from vv16 !
fetch !
select * from  foreign_tables !
fetch !
commit work !
drop table f_t1 !
drop table f_t2 !
drop table f_t3 !
drop table f_t4 !
drop table f_t5 !
drop table f_t6 !
drop table f_t7 !
drop table f_t8 !
drop table f_t9 !
drop table f_t10 !
drop table f_t11 !
drop table f_t12 !
drop table f_t13 !
drop table f_t14 !
drop table f_t15 !
drop table f_t16 !
drop table k_t1 !
drop table k_t2 !
drop table k_t3 !
drop table k_t4 !
drop table k_t5 !
drop table k_t6 !
drop table k_t7 !
drop table k_t8 !
drop table k_t9 !
drop table k_t10 !
drop table k_t11 !
drop table k_t12 !
drop table k_t13 !
drop table k_t14 !
drop table k_t15 !
drop table k_t16 !
commit work release !
