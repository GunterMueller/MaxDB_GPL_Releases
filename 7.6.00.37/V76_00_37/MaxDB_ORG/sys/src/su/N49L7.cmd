*   *ID* N49L7    DBCMD    changed on 1992-09-28-13.26.44 by ADMIN     *
file CONNECT ( KERN TEST !
alter table t3 foreign key l13c1 ( t3xc3, t3xc4 )
   references t1 on delete cascade !
alter table t3 foreign key l23r1 ( t3xc5, t3xc6 )
   references t2 on delete restrict !
insert t3 values ( 2, 'AAAA', 3, 'CCCC', 1, 'DDDD', 7, 'AAAA' ) !
insert t1 values ( 5, 'BBBB', 6, 'AAAA', 3, 'FFFF', 2, 'BBBB' ) !
insert t2 values ( 5, 'BBBB', 3, null, 4, 'AAAA', 1, 'BBBB' ) !
insert t3 values ( 3, 'CCCC', 5, 'BBBB', 3, 'FFFF', 4, null ) !
insert t3 values ( 3, 'BBBB', 1, 'FFFF', 5, 'BBBB', 6, 'AAAA' ) !
insert t3 values ( 5, 'BBBB', 5, 'BBBB', 5, 'BBBB', 5, 'BBBB' ) !
delete t2 !
delete t2 where t2xc1 = 5 and t2xc2 = 'BBBB' !
delete t3 !
delete t1 !
alter table t3 drop foreign key l13c1 !
alter table t3 drop foreign key l23r1 !
commit work release !
