*   *ID* N49L4    DBCMD    changed on 1992-09-28-13.26.42 by ADMIN     *
file connect ( kern test !
alter table t1 foreign key l11n1 ( t1xc3, t1xc4 )
   references t1 on delete set null !
alter table t1 foreign key l11r1 ( t1xc5, t1xc6 )
   references t1 !
insert t1 values ( 2, 'AAAA', 1, 'BBBB', 8, 'FFFF', 3, null ) !
insert t1 values ( 1, 'BBBB', null, 'BBBB', 2, 'DDDD', 1, 'AAAA' ) !
insert t1 values ( 4, 'AAAA', 1, 'BBBB', 3, 'FFFF', 2, 'FFFF' ) !
insert t1 values ( 4, 'FFFF', 1, 'BBBB', 3, 'DDDD', 1, 'CCCC' ) !
insert t1 values ( 3, 'CCCC', 5, null, 4, 'CCCC', 5, null ) !
delete t1 where t1xc1 = 1 !
select * from t1 !
fetch !
rollback work !
 ***************************************************!
alter table t1 foreign key l11d1 ( t1xc7, t1xc8 )
   references t1 on delete set default !
insert t1 values ( 2, 'CCCC', 4, 'BBBB', 1, 'AAAA', null, 'FFFF' ) !
insert t1 values ( 5, 'FFFF', 2, 'CCCC', 2, 'AAAA', 5, 'FFFF' ) !
insert t1 values ( 7, 'DDDD', 1, null, 4, 'DDDD', 5, 'FFFF' ) !
insert t1 values ( 4, 'AAAA', 4, null, 7, 'BBBB', 2, 'CCCC' ) !
insert t1 values ( 2, 'DDDD', 5, 'AAAA', 2, 'DDDD', 2, 'AAAA' ) !
insert t1 ( t1xc1, t1xc2, t1xc3, t1xc4, t1xc5, t1xc6 )
   values ( 1, 'AAAA', 3, 'FFFF', 2, 'BBBB' ) !
insert t1 set t1xc1 = 4, t1xc2 = 'BBBB', t1xc4 = 'AAAA', t1xc5 = 3,
              t1xc6 = 'AAAA' !
insert t1 values ( 8, 'CCCC', 7, 'BBBB', 5, 'CCCC', 7, 'DDDD' ) !
insert t1 values ( 3, 'FFFF', 3, 'AAAA', 1, 'CCCC', 4, 'AAAA' ) !
delete t1 where t1Xc1 in ( 2, 5 ) !
select * from t1 !
fetch !
delete t1 where t1xc7 = 1 and t1xc8 = 'AAAA' !
select * from t1 !
fetch !
delete t1 !
alter table t1 drop foreign key l11d1 !
commit work !
 ***************************************************!
alter table t1 foreign key l11n1 ( t1xc1, t1xc2 )
   references t1 on delete set null !
alter table t1 foreign key l11n2 ( t1xc5, t1xc6 )
   references t1 on delete set null !
alter table t1 foreign key l11d1 ( t1xc1, t1xc2 )
   references t1 on delete set default !
alter table t1 foreign key l11d2 ( t1xc3, t1xc4 )
   references t1 on delete set default !
alter table t1 foreign key l11r1 ( t1xc3, t1xc4 )
   references t1 !
insert t1 values ( 2, 'AAAA', 1, 'BBBB', 4, 'AAAA', 2, null ) !
insert t1 values ( 4, 'BBBB', 4, 'BBBB', 2, 'AAAA', 4, 'CCCC' ) !
insert t1 values ( 6, 'DDDD', 7, null, 1, 'CCCC', 1, 'FFFF' ) !
insert t1 values ( 3, 'CCCC', 6, 'DDDD', 3, 'FFFF', 2, 'AAAA' ) !
insert t1 values ( 5, 'AAAA', null, 'DDDD', 2, 'AAAA', 7, 'BBBB' ) !
delete t1 where t1xc1 in ( 4, 6 ) !
delete t1 where t1xc1 = 4 !
delete t1 where t1xc1 = 6 !
delete t1 where t1xc1 = 5 !
select * from t1 !
fetch !
rollback work !
 ***************************************************!
alter table t1 foreign key l11r1 ( t1xc1, t1xc2 )
   references t1 on delete restrict !
insert t1 values ( 2, 'AAAA', 1, 'BBBB', 2, 'AAAA', 9, null ) !
insert t1 values ( 1, 'BBBB', 4, null, 1, 'DDDD', 2, 'CCCC' ) !
insert t1 values ( 2, 'AAAA', 1, 'CCCC', 2, 'FFFF', null, 'AAAA' ) !
delete t1 where t1xc1 = 2 !
select * from t1 !
fetch !
rollback work !
 ***************************************************!
alter table t1 foreign key l11r1 ( t1xc3, t1xc4 )
   references t1 on delete restrict !
insert t1 values ( 2, 'AAAA', 2, 'AAAA', 2, 'AAAA', 2, 'AAAA' ) !
delete t1 where t1xc1 = 2 !
rollback work !
 ***************************************************!
alter table t1 foreign key l11r1 ( t1xc1, t1xc2 )
   references t1 on delete cascade !
insert t1 values ( 3, 'AAAA', 2, null, 4, 'FFFF', 3, 'BBBB' ) !
insert t1 values ( 3, 'BBBB', 1 , 'AAAA', 3, 'CCCC', 2, 'AAAA' ) !
insert t1 values ( 1, 'FFFF', null, null, 2, 'CCCC', 5, 'DDDD' ) !
delete t1 where t1xc1 > 2 !
select * from t1 !
fetch !
rollback work !
 ***************************************************!
alter table t1 foreign key l11c1 ( t1xc3, t1xc4 )
   references t1 on delete cascade !
insert t1 values ( 5, 'AAAA', 2, null, 4, 'BBBB', 1, null ) !
insert t1 values ( 1, 'BBBB', 5, 'AAAA', 1, 'CCCC', 6, 'FFFF' ) !
insert t1 values ( 4, 'CCCC', 1, 'BBBB', 3, 'CCCC', 3, 'BBBB' ) !
delete t1 where t1xc1 = 5 !
select * from t1 !
fetch !
alter table t1 drop foreign key l11c1 !
commit work release !
