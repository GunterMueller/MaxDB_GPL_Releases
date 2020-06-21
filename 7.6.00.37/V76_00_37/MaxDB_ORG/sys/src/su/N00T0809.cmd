*   *ID* N00T0809 DBCMD    changed on 1992-09-28-13.24.57 by ADMIN     *
 *** 8.13  <create view statement> !
file connect ( kern test !
create table t1 ( t1xc1 fixed(3) key, t1xc2 fixed(3) ) !
create table t2 ( t2xc1 fixed(3) key, t2xc2 fixed(3),
                  t2xc3 fixed(3) ) !
alter table t2 foreign key t2lt1 (t2xc2) references t1 !
create view v1 as select t1xc1, t1xc2, t2xc1, t2xc3
   from t1, t2 where t1xc1 = t2xc2 with check option !
commit work !
insert t1 values ( 5, 3 ) !
insert t1 values ( 8, 3 ) !
insert t1 values ( 4, 1 ) !
insert t1 values ( 3, 1 ) !
insert t1 values ( 2, 3 ) !
insert t2 values ( 7, 8, 3 ) !
insert t2 values ( 3, 5, 4 ) !
insert t2 values ( 5, 3, 4 ) !
insert t2 values ( 4, 2, 3 ) !
insert t2 values ( 9, 4, 2 ) !
insert t2 values ( 1, 3, 8 ) !
commit work !
select * from v1 where t1xc2 < t2xc1 !
fetch !
insert v1 values ( 1, 2, 6, 3 ) !
insert into v1 set t1xc1 = 6, t2xc1 = 8, t2xc3 = 2 !
select * from v1 !
fetch !
file sel_priv_on_tab ( V1 !
update v1 set t1xc2 = 3, t2xc3 = 4 key t2xc1 = 6 !
update v1 set t1xc2 = 3, t1xc1 = 5, t2xc3 = 4 key t2xc1 = 6 !
delete v1 key t2xc1 = 7 !
select * from v1 !
fetch !
commit work !
drop table t1 !
drop table t2 !
commit work !
create table tab ( s fixed(5) ) !
create view v1 as select s from tab where s<1000 with check option !
create view v2 as select s from v1  where s<100 !
create view v3 as select s from v2  where s>10 with check option !
commit work !
insert v1 set s=5 !
insert v2 set s=500 !
insert v3 set s=700 !
update v1 set s=80  where s=500 !
update v2 set s=800 where s=5 !
update v3 set s=200 where s=80 !
commit work !
drop table tab !
commit work release !
