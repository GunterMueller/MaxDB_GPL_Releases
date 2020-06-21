*   *ID* N55VIEW4 DBCMD    changed on 1992-09-28-13.26.54 by ADMIN     *
file connect ( kern test !
create table t1 ( t1xc1 fixed(2) key, t1xc2 fixed(2),
                  t1xc3 fixed(2) ) !
create table t2 ( t2xc1 fixed(2) key, t2xc2 fixed(2), t2xc3 fixed(2) ) !
create table t3 ( t3xc1 fixed(2) key, t3xc2 fixed(2), t3xc3 fixed(2) ) !
alter table t1 foreign key t1lt2 (t1xc1) references t2 !
alter table t2 foreign key t2lt3 (t2xc2) references t3 !
create view v1 as select t1xc1, t1xc2, t1xc3, t2xc2, t2xc3, t3xc2, t3xc3
   from t1, t2, t3
   where t1xc1 = t2xc1 and t2xc2 = t3xc1 with check option !
insert t3 values ( 2, 3, 3 ) !
insert t3 values ( 1, 6, 6 ) !
insert t3 values ( 1, 2, 4 ) !
insert t3 values ( 5, 2, 3 ) !
insert t3 values ( 3, 1, 7 ) !
insert t2 values ( 8, 1, 5 ) !
insert t2 values ( 2, 5, 5 ) !
insert t2 values ( 4, 2, 6 ) !
insert t1 values ( 2, 3, 5 ) !
insert t1 values ( 4, 1, 3 ) !
insert t1 values ( 8, 3, 1 ) !
commit work !
select * from v1 !
fetch !
insert v1 values ( 5,2,2,6,4,6,1 ) !
insert v1 values ( 7,8,3,4,7,8,1 ) !
insert into v1 values ( 3,4,2,7,8,7,2 ) !
insert v1 values ( null, 4, null, 3,9,4,1 ) !
insert v1 values ( 2,5,7, null, null, 6,9 ) !
insert v1 values ( 9,2,3,9, null, 8,7 ) !
insert v1 ( t1xc1, t1xc2, t1xc3, t2xc2, t3xc1 )
   values (17, 3, 7, 4, 4 ) !
commit work !
select * from t1 !
fetch !
select * from t2 !
fetch !
select * from t3 !
fetch !
select * from v1 !
fetch !
update of v1 set t2xc2=3 key t1xc1=7 !
commit work !
update v1 set t2xc2 = 3 !
select * from v1 !
fetch !
rollback work !
update of v1 set t1xc1=4, t1xc2=1, t2xc3=5 where t1xc1=4 !
update v1 set t1xc1=2, t1xc2=4 where t1xc1=2 and t2xc1=2 !
update v1 set t3xc3=3 where t2xc1=8 and t3xc2=2 !
rollback work !
delete v1 key t1xc1=8 !
delete from v1 where t2xc2=5 !
select * from v1 !
fetch !
delete v1 !
commit work !
select * from t1 !
fetch !
select * from t2 !
fetch !
select * from t3 !
fetch !
select * from v1 !
fetch !
drop table t1 !
drop table t2 !
drop table t3 !
commit work release !
