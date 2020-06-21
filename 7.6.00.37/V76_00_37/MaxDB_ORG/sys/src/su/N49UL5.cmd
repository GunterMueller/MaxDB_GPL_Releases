*   *ID* N49UL5   DBCMD    changed on 1992-09-28-13.26.46 by ADMIN     *
 ***************************************************
 *** Test von Referenz-Zyklen !
file connect ( kern test !
alter table t2 foreign key l12c1 ( t2xc3, t2xc4 )
   references t1 (t1xc1, t1xc2) on delete cascade !
alter table t1 foreign key l21c1 ( t1xc3, t1xc4 )
   references t2 (t2xc1, t2xc2) on delete cascade !
alter table t1 foreign key l21r1 ( t1xc3, t1xc4 )
   references t2 (t2xc1, t2xc2) on delete restrict !
alter table t1 foreign key l21n1 ( t1xc3, t1xc4 )
   references t2 (t2xc1, t2xc2) on delete set null !
alter table t1 foreign key l21n1 ( t1xc7, t1xc8 )
   references t2 (t2xc1, t2xc2) on delete set default !
alter table t2 drop foreign key l12c1 !
commit work !
 ***************************************************!
alter table t4 foreign key l34r1 ( t4xc3, t4xc4 )
   references t3 (t3xc1, t3xc2) on delete restrict !
alter table t3 foreign key l43c1 ( t3xc3, t3xc3 )
   references t4 (t4xc1, t4xc2) on delete cascade !
rollback work !
alter table t4 foreign key l34r1 ( t4xc3, t4xc4 )
   references t3 (t3xc1, t3xc2) !
alter table t3 foreign key l43c1 ( t3xc3, t3xc3 )
   references t4 (t4xc1, t4xc2) on delete cascade !
rollback work !
alter table t4 foreign key l34n1 ( t4xc3, t4xc4 )
   references t3 (t3xc1, t3xc2) on delete set null !
alter table t3 foreign key l43c1 ( t3xc3, t3xc3 )
   references t4 (t4xc1, t4xc2) on delete cascade !
rollback work !
alter table t4 foreign key l34d1 ( t4xc7, t4xc8 )
   references t3 (t3xc1, t3xc2) on delete set default !
alter table t3 foreign key l43c1 ( t3xc3, t3xc3 )
   references t4 (t4xc1, t4xc2) on delete cascade !
rollback work !
 ***************************************************!
alter table t4 foreign key l34r1 ( t4xc3, t4xc4 )
   references t3 (t3xc1, t3xc2) on delete restrict !
alter table t3 foreign key l43r1 ( t3xc3, t3xc4 )
   references t4 (t4xc1, t4xc2) on delete restrict !
insert t3 values ( 2, 'AAAA', 3, 'CCCC', 1, 'FFFF', 3, 'DDDD' ) !
insert t3 values ( 4, 'BBBB', 4, 'BBBB', 4, 'BBBB', 4, 'BBBB' ) !
insert t3 values ( 1, 'FFFF', 3, null, 4, 'AAAA', 2, null ) !
insert t3 values ( 2, 'AAAA', null, null, 5, 'BBBB', 1, 'CCCC' ) !
insert t4 values ( 2, 'DDDD', null, 'AAAA', 3, 'BBBB', 8, 'AAAA' ) !
insert t4 values ( 1, 'BBBB', 1, 'FFFF', 3, 'CCCC', 1, 'BBBB' ) !
insert t4 values ( 3, 'CCCC', 1, 'FFFF', 2, 'AAAA', 7, 'AAAA' ) !
insert t3 values ( 2, 'DDDD', 1, 'BBBB', 1, 'AAAA', 2, 'DDDD' ) !
insert t3 values ( 7, 'BBBB', 2, 'DDDD', 2, 'BBBB', 3, 'BBBB' ) !
select * from t3 !
fetch !
select * from t4 !
fetch !
delete t3 where t3xc1 = 1 !
delete t3 where t3xc1 < 3 !
delete t4 where t4xc1 = 2 !
delete t4 where t4xc1 = 3 !
rollback work !
 ***************************************************!
alter table t2 foreign key l12c1 ( t2xc3, t2xc4 )
   references t1 (t1xc1, t1xc2) on delete cascade !
alter table t3 foreign key l23c1 ( t3xc5, t3xc6 )
   references t2 (t2xc1, t2xc2) on delete cascade !
alter table t4 foreign key l34d1 ( t4xc7, t4xc8 )
   references t3 (t3xc1, t3xc2) on delete set default !
alter table t5 foreign key l45c1 ( t5xc3, t5xc4 )
   references t4 (t4xc1, t4xc2) on delete cascade !
alter table t1 foreign key l51n1 ( t1xc3, t1xc4 )
   references t5 (t5xc1, t5xc2) on delete set null !
commit work !
insert t1 values ( 2, 'AAAA', 3, 'BBBB', 4, 'CCCC', 5, 'DDDD' ) !
insert t1 values ( 1, 'DDDD', 4, null, 7, 'AAAA', 3, null ) !
insert t1 values ( 3, 'AAAA', null, 'CCCC', 4, 'DDDD', 2, 'AAAA' ) !
insert t2 values ( 5, 'AAAA', 4, 'BBBB', 2, 'FFFF', 1, null ) !
insert t2 values ( 3, 'CCCC', 4, null, 1, 'DDDD', 2, 'AAAA' ) !
insert t2 values ( 4, 'AAAA', 3, 'AAAA', 3, 'FFFF', 4, 'FFFF' ) !
insert t2 values ( 3, 'AAAA', 1, 'DDDD', 3, 'AAAA', 3, 'BBBB' ) !
insert t2 values ( 2, 'FFFF', null, 'DDDD', 4, 'AAAA', 1, 'CCCC' ) !
insert t3 values ( 4, 'DDDD', 2, 'BBBB', 3, 'CCCC', 7, null  ) !
insert t3 values ( 7, 'AAAA', 1, 'CCCC', 8, 'FFFF', 4, 'AAAA' ) !
insert t3 values ( 4, 'CCCC', 3, 'AAAA', 2, 'FFFF', 3, 'CCCC' ) !
insert t3 values ( 6, 'BBBB', 2, null, 2, 'FFFF', 4, 'AAAA' ) !
insert t3 values ( 1, 'AAAA', 5, 'BBBB', 4, 'AAAA', 5, 'BBBB' ) !
insert t3 values ( 5, 'CCCC', 2, 'CCCC', 4, 'AAAA', 5, 'BBBB' ) !
insert t4 values ( 3, 'CCCC', 2, null, 5, 'BBBB', 2, null ) !
insert t4 values ( 4, 'AAAA', 1, 'BBBB', 2, 'CCCC', 3, 'CCCC' ) !
insert t4 values ( 3, 'DDDD', 4, 'CCCC', 7, 'FFFF', 4, 'CCCC' ) !
insert t4 ( t4xc1, t4xc2, t4xc3, t4xc4, t4xc5, t4xc6 )
   values ( 8, 'AAAA', 2, 'AAAA', 5, 'BBBB' ) !
insert t4 ( t4xc1, t4xc2, t4xc3, t4xc4, t4xc5, t4xc6 )
   values ( 2, 'AAAA', 1, 'CCCC', 4, 'CCCC' ) !
insert t4 values ( 5, 'BBBB', 6, 'AAAA', 6, 'BBBB', 4, 'DDDD' ) !
insert t5 values ( 3, 'AAAA', 2, 'AAAA', 5, 'BBBB', 2, 'CCCC' ) !
insert t5 values ( 7, 'DDDD', 1, 'CCCC', 2, 'AAAA', 1, 'AAAA' ) !
insert t5 values ( 5, 'AAAA', null, 'DDDD', 4, 'FFFF', 5, null ) !
insert t5 values ( 3, 'FFFF', null, null, 4, 'DDDD', 2, 'AAAA' ) !
insert t5 values ( 3, 'DDDD', 2, 'CCCC', 3, 'AAAA', 1, 'FFFF' ) !
insert t5 values ( 2, 'DDDD', 2, 'AAAA', 2, 'DDDD', 3, 'DDDD' ) !
insert t5 values ( 7, 'CCCC', 8, 'AAAA', 1, 'CCCC', 8, 'FFFF' ) !
insert t1 values ( 2, 'DDDD', 4, null, 5, 'BBBB', 7, 'BBBB' ) !
insert t1 values ( 3, 'DDDD', 5, 'AAAA', 1, 'CCCC', 7, 'AAAA' ) !
insert t1 values ( 2, 'AAAA', 3, 'FFFF', 5, 'CCCC', 5, 'BBBB' ) !
insert t1 values ( 8, 'AAAA', 7, 'CCCC', 3, 'FFFF', 3, 'CCCC' ) !
select * from t1 !
fetch !
select * from t2 !
fetch !
select * from t3 !
fetch !
select * from t4 !
fetch !
select * from t5 !
fetch !
commit work !
delete t1 where t1xc1 = 3 and t1xc2 = 'AAAA' !
delete t4 where t4xc2 = 'DDDD' !
delete t3 where t3xc1 = 4 and t3xc2 = 'CCCC' !
delete t5 where t5xc1 >= 5 !
select * from t1 !
fetch !
select * from t2 !
fetch !
select * from t3 !
fetch !
select * from t4 !
fetch !
select * from t5 !
fetch !
commit work !
alter table t4 drop foreign key l34d1 !
alter table t4 foreign key l34c1 ( t4xc3, t4xc4 )
   references t3 (t3xc1, t3xc2) on delete cascade !
alter table t2 drop foreign key l12c1 !
alter table t3 drop foreign key l23c1 !
alter table t5 drop foreign key l45c1 !
alter table t1 drop foreign key l51n1 !
delete t1 !
delete t2 !
delete t3 !
delete t4 !
delete t5 !
commit work release !
