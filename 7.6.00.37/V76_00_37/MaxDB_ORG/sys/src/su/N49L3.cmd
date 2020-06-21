*   *ID* N49L3    DBCMD    changed on 1992-09-28-13.26.42 by ADMIN     *
file CONNECT ( KERN TEST !
alter table t0 foreign key l10n1 ( t0xc3, t0xc4 )
   references t1 on delete set null !
alter table t0 foreign key l10d1 ( t0xc7, t0xc8 )
   references t1 on delete set default !
alter table t0 foreign key l10c1 ( t0xc5, t0xc6 )
   references t1 on delete cascade !
alter table t0 foreign key l10r1 ( t0xc7, t0xc8 )
   references t1 on delete restrict !
alter table t0 foreign key l10r1 ( t0xc7, t0xc8 )
   references t1 !
alter table t0 drop foreign key l10n1 !
commit work !
 ***************************************************!
alter table t0 foreign key l10d1 ( t0xc7, t0xc8 )
   references t1 on delete set default !
alter table t0 foreign key l10n1 ( t0xc3, t0xc4 )
   references t1 on delete set null !
alter table t0 foreign key l10c1 ( t0xc5, t0xc6 )
   references t1 on delete cascade !
alter table t0 foreign key l10r1 ( t0xc7, t0xc8 )
   references t1 on delete restrict !
alter table t0 foreign key l10r1 ( t0xc7, t0xc8 )
   references t1 !
alter table t0 drop foreign key l10d1 !
commit work !
 ***************************************************!
alter table t0 foreign key l10r1 ( t0xc7, t0xc8 )
   references t1 on delete restrict !
alter table t0 foreign key l10d1 ( t0xc7, t0xc8 )
   references t1 on delete set default !
alter table t0 foreign key l10n1 ( t0xc3, t0xc4 )
   references t1 on delete set null !
alter table t0 foreign key l10c1 ( t0xc5, t0xc6 )
   references t1 on delete cascade !
alter table t0 drop foreign key l10r1 !
commit work !
 ***************************************************!
alter table t0 foreign key l10c1 ( t0xc5, t0xc6 )
   references t1 on delete cascade !
alter table t0 foreign key l10r1 ( t0xc7, t0xc8 )
   references t1 on delete restrict !
alter table t0 foreign key l10d1 ( t0xc7, t0xc8 )
   references t1 on delete set default !
alter table t0 foreign key l10n1 ( t0xc3, t0xc4 )
   references t1 on delete set null !
alter table t0 drop foreign key l10r1 !
alter table t0 drop foreign key l10c1 !
commit work !
 ***************************************************!
alter table t0 foreign key l10r1 ( t0xc7, t0xc8 )
   references t1 on delete restrict !
alter table t0 foreign key l10r2 ( t0xc5, t0xc6 )
   references t1 on delete restrict !
alter table t0 foreign key l10r3 ( t0xc3, t0xc4 )
   references t1 !
alter table t0 foreign key l10r4 ( t0xc3, t0xc4 )
   references t1 !
insert t1 values ( 2, 'AAAA', 8, 'CCCC', 3, 'DDDD', 3, null ) !
insert t0 values ( 3, 'CCCC', 2, 'AAAA', 3, 'AAAA', 1, null ) !
insert t0 values ( 1, 'FFFF', null, 'DDDD', 2, 'AAAA', null, null ) !
insert t0 values ( 2, 'AAAA', 2, 'AAAA', 2, 'AAAA', 2, 'AAAA' ) !
delete t1 where t1xc1 = 2 !
rollback work !
 ***************************************************!
alter table t0 foreign key l10c1 ( t0xc7, t0xc8 )
   references t1 on delete cascade !
alter table t0 foreign key l10c2 ( t0xc5, t0xc6 )
   references t1 on delete cascade !
alter table t0 foreign key l10c3 ( t0xc3, t0xc4 )
   references t1 on delete cascade !
insert t1 values ( 2, 'AAAA', 8, 'CCCC', 3, 'DDDD', 3, null ) !
insert t1 values ( 1, 'FFFF', 2, null, 3, 'BBBB', 1, 'CCCC' ) !
insert t0 values ( 3, 'CCCC', 2, 'AAAA', 3, 'AAAA', 1, null ) !
insert t0 values ( 1, 'BBBB', null, 'DDDD', 2, 'AAAA', null, null ) !
insert t0 values ( 2, 'BBBB', null, 'DDDD', 1, 'FFFF', null, null ) !
insert t0 values ( 2, 'AAAA', 2, 'AAAA', 2, 'AAAA', 2, 'AAAA' ) !
delete t1 where t1xc1 = 2 !
select * from t1 !
fetch !
select * from t0 !
fetch !
rollback work release !
