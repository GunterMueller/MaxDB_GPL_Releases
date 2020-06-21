*   *ID* N50L4    DBCMD    changed on 1992-09-28-13.26.49 by ADMIN     *
file connect ( kern test !
create table t1 ( t1xc1 fixed(5) key,
                  t1xc2 char(5) key,
                  t1xc3 fixed(5),
                  t1xc4 char(5),
                  t1Xc5 fixed(5) not null,
                  t1xc6 char(5) not null,
                  t1xc7 fixed(5),
                  t1xc8 char(5) ) !
create table t2 ( t2xc1 fixed(5) key,
                  t2xc2 char(5) key,
                  t2xc3 fixed(5),
                  t2xc4 char(5),
                  t2Xc5 fixed(5) not null,
                  t2xc6 char(5) not null,
                  t2xc7 fixed(5),
                  t2xc8 char(5) ) !
commit work !
 ***************************************************!
insert t1 values
   ( 1, 'AAAA', 2, 'DDDD', 3, 'BBBB', 2, 'AAAA' ) !
insert t1 values
   ( 1, 'CCCC', 2, 'EEEE', 4, 'BBBB', 2, null ) !
insert t1 values
   ( 4, 'AAAA', 6, 'CCCC', 3, 'BBBB', 1, 'AAAA' ) !
insert t1 values
   ( 2, 'BBBB', 2, 'DDDD', 1, 'AAAA', 8, 'FFFF' ) !
insert t1 values
   ( 3, 'FFFF', null, 'CCCC', 3, 'BBBB', 2, 'AAAA' ) !
insert t1 values
   ( 5, 'AAAA', 2, 'FFFF', 3, 'BBBB', 2, 'CCCC' ) !
insert t1 values
   ( 1, 'FFFF', 6, 'AAAA', 3, 'FFFF', 2, 'AAAA' ) !
insert t1 values
   ( 8, 'DDDD', null, null, 8, 'BBBB', null, 'FFFF' ) !
insert t1 values
   ( 6, 'AAAA', 2, 'DDDD', 3, 'AAAA', 2, 'FFFF' ) !
insert t1 values
   ( 3, 'BBBB', 7, 'DDDD', 3, 'CCCC', 2, 'AAAA' ) !
select * from t1 !
fetch !
commit work !
 ***************************************************!
alter table t2 foreign key l12c1 ( t2xc3, t2xc4 )
   references t1 on delete cascade !
alter table t2 foreign key l22c1 ( t2xc7, t2xc8 )
   references t2 on delete cascade !
insert t2 values ( 8, 'AAAA', null, 'CCCC', 3, 'DDDD', null, 'BBBB' ) !
insert t2 values ( 7, 'BBBB', 1, 'CCCC', 5, 'FFFF', 7, null ) !
insert t2 values ( 3, 'DDDD', 3, null, 3, 'AAAA', 3, 'FFFF' ) !
insert t2 values ( 5, 'FFFF', null, 'CCCC', 4, 'FFFF', 5, 'FFFF' ) !
insert t2 values ( 1, 'CCCC', 6, 'AAAA', 5, 'AAAA', 7, 'BBBB' ) !
delete t1 where t1xc1 = 1 !
select * from t1 !
fetch !
select * from t2 !
fetch !
rollback work !
delete t1 !
commit work !
 ***************************************************!
alter table t1 foreign key l11c1 ( t1xc3, t1xc4 )
   references t1 on delete cascade !
alter table t2 foreign key l12c1 ( t2xc3, t2xc4 )
   references t1 on delete cascade !
commit work !
insert t1 values ( 3, 'AAAA', 2, null, 4, 'BBBB', 3, 'FFFF' ) !
insert t1 values ( 5, 'BBBB', 5, 'BBBB', 3, 'AAAA', 7, 'BBBB' ) !
insert t1 values ( 2, 'FFFF', 3, 'AAAA', 4, 'CCCC', 1, 'AAAA' ) !
insert t1 values ( 5, 'AAAA', null, 'AAAA', 2, 'AAAA', 2, null ) !
insert t1 values ( 3, 'CCCC', 3, 'AAAA', 5, 'CCCC', 3, 'FFFF' ) !
insert t2 values ( 4, 'CCCC', 2, null, 3, 'DDDD', 1, 'AAAA' ) !
insert t2 values ( 2, 'DDDD', 3, 'AAAA', 4, 'FFFF', 5, 'BBBB' ) !
insert t2 values ( 7, 'AAAA', 5, 'BBBB', 1, 'CCCC', 1, 'FFFF' ) !
commit work !
delete t1 where t1xc1 between 3 and 5 !
select * from t1 !
fetch !
select * from t2 !
fetch !
rollback work !
alter table t2 drop foreign key l12c1 !
alter table t2 foreign key l12r1 ( t2xc3, t2xc4 )
   references t1 on delete restrict !
delete t1 where t1xc1 between 3 and 3 !
select * from t1 !
fetch !
select * from t2 !
fetch !
drop table t1 !
drop table t2 !
commit work release !
