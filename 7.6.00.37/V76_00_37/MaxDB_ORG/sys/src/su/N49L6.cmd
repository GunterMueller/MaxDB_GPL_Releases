*   *ID* N49L6    DBCMD    changed on 1992-09-28-13.26.43 by ADMIN     *
file connect ( kern test !
alter table t4 foreign key l24c1 ( t4xc3, t4xc4 )
   references t2 on delete cascade !
alter table t4 foreign key l34c1 ( t4xc5, t4xc6 )
   references t3 on delete cascade !
rollback work !
alter table t4 foreign key l24c1 ( t4xc3, t4xc4 )
   references t2 on delete cascade !
alter table t4 foreign key l34r1 ( t4xc5, t4xc6 )
   references t3 on delete restrict !
rollback work !
alter table t4 foreign key l24n1 ( t4xc3, t4xc4 )
   references t2 on delete set null !
alter table t4 foreign key l34d1 ( t4xc7, t4xc8 )
   references t3 on delete set default !
rollback work !
 ***************************************************!
alter table t2 foreign key l12c1 ( t2xc3, t2xc4 )
   references t1 on delete cascade !
alter table t3 foreign key l13c1 ( t3xc5, t3xc6 )
   references t1 on delete cascade !
alter table t3 foreign key l23r1 ( t3xc7, t3xc8 )
   references t2 on delete restrict !
rollback work !
 ***************************************************!
alter table t3 foreign key l23d1 ( t3xc7, t3xc8 )
   references t2 on delete set default !
alter table t3 foreign key l13d1 ( t3xc7, t3xc8 )
   references t1 on delete set default !
alter table t2 foreign key l12c1 ( t2xc3, t2xc4 )
   references t1 on delete cascade !
rollback work !
 ***************************************************!
alter table t3 foreign key l13c1 ( t3xc3, t3xc4 )
   references t1 on delete cascade !
alter table t3 foreign key l23n1 ( t3xc3, t3xc4 )
   references t2 on delete set null !
alter table t2 foreign key l12c1 ( t2xc3, t2xc4 )
   references t1 on delete cascade !
rollback work !
 ***************************************************!
alter table t2 foreign key l12c1 ( t2xc3, t2xc4 )
   references t1 on delete cascade !
alter table t3 foreign key l23d1 ( t3xc7, t3xc8 )
   references t2 on delete set default !
alter table t3 foreign key l13r1 ( t3xc3, t3xc4 )
   references t1 on delete restrict !
rollback work !
 ***************************************************!
alter table t3 foreign key l13n1 ( t3xc3, t3xc4 )
   references t1 on delete set null !
alter table t2 foreign key l12c1 ( t2xc5, t2xc6 )
   references t1 on delete cAScade !
alter table t3 foreign key l23n1 ( t3xc3, t3xc4 )
   references t2 on delete set null !
rollback work !
 ***************************************************!
alter table t3 foreign key l23c1 ( t3xc3, t3xc4 )
   references t2 on delete cascade !
alter table t3 foreign key l13c1 ( t3xc1, t3xc2 )
   references t1 on delete cascade !
alter table t2 foreign key l12c1 ( t2xc3, t2xc4 )
   references t1 on delete cascade !
rollback work !
 ***************************************************!
alter table t2 foreign key l12c1 ( t2xc3, t2xc4 )
   references t1 on delete cascade !
alter table t3 foreign key l13r1 ( t3xc3, t3xc4 )
   references t1 on delete restrict !
alter table t3 foreign key l23r1 ( t3xc3, t3xc4 )
   references t2 on delete restrict !
rollback work !
 ***************************************************!
alter table t2 foreign key l12c1 ( t2xc3, t2xc4 )
   references t1 on delete cascade !
alter table t4 foreign key l24c1 ( t4xc3, t4xc4 )
   references t2 on delete cascade !
alter table t3 foreign key l13d1 ( t3xc7, t3xc8 )
   references t1 on delete set default !
alter table t4 foreign key l34c1 ( t4xc7, t4xc8 )
   references t3 on delete cascade !
rollback work !
 ***************************************************!
alter table t2 foreign key l12c1 ( t2xc5, t2xc6 )
   references t1 on delete cascade !
alter table t3 foreign key l13n1 ( t3xc3, t3xc4 )
   references t1 on delete set null !
alter table t4 foreign key l24r1 ( t4xc1, t4xc2 )
   references t2 on delete restrict !
alter table t4 foreign key l34r1 ( t4xc3, t4xc4 )
   references t3 on delete restrict !
rollback work !
 ***************************************************!
alter table t2 foreign key l12c1 ( t2xc3, t2xc4 )
   references t1 on delete cascade !
alter table t4 foreign key l34c1 ( t4xc3, t4xc4 )
   references t3 on delete cascade !
alter table t3 foreign key l13c1 ( t3xc5, t3xc6 )
   references t1 on delete cascade !
alter table t4 foreign key l24d1 ( t4xc7, t4xc8 )
   references t2 on delete set default !
rollback work !
 ***************************************************!
alter table t4 foreign key l34n1 ( t4xc3, t4xc4 )
   references t3 on delete set null !
alter table t4 foreign key l24n1 ( t4xc3, t4xc4 )
   references t2 on delete set null !
alter table t3 foreign key l13c1 ( t3xc5, t3xc6 )
   references t1 on delete cascade !
alter table t2 foreign key l12c1 ( t2xc7, t2xc8 )
   references t1 on delete cascade !
rollback work !
 ***************************************************!
alter table t2 foreign key l12c1 ( t2xc3, t2xc4 )
   references t1 on delete cascade !
alter table t4 foreign key l24r1 ( t4xc1, t4xc2 )
   references t2 on delete restrict !
alter table t4 foreign key l34c1 ( t4xc3, t4xc4 )
   references t3 on delete cascade !
alter table t3 foreign key l13c1 ( t3xc5, t3xc6 )
   references t1 on delete cascade !
rollback work !
 ***************************************************!
alter table t3 foreign key l13c1 ( t3xc3, t3xc4 )
   references t1 on delete cascade !
alter table t4 foreign key l24r1 ( t4xc3, t4xc4 )
   references t2 on delete restrict !
alter table t4 foreign key l34r1 ( t4xc3, t4xc4 )
   references t3 on delete restrict !
alter table t2 foreign key l12c1 ( t2xc3, t2xc4 )
   references t1 on delete cascade !
rollback work !
 ***************************************************!
alter table t2 foreign key l12c1 ( t2xc5, t2xc6 )
   references t1 on delete cascade !
alter table t3 foreign key l13c1 ( t3xc3, t3xc4 )
   references t1 on delete cascade !
alter table t4 foreign key l24c1 ( t4xc7, t4xc8 )
   references t2 on delete cascade !
alter table t4 foreign key l34c1 ( t4xc3, t4xc4 )
   references t3 on delete cascade !
rollback work release !
