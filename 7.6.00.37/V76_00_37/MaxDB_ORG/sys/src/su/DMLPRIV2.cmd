*   *ID* DMLPRIV2 DBCMD    changed on 1992-09-28-13.23.23 by ADMIN     *
 * user with INSERT privilege only
 * is only allowed to
 * 1) insert records
 * 2) to lock in exclusive mode ( table and rows ) !
file connect ( d5 w !
lock table kern.t1 in share mode !
update of kern.t1 set t1_c3=1.1 key t1_c1='d0',t1_c2=2 !
delete kern.t1 key t1_c1 = 'd0' , t1_c2 = 3 !
insert into kern.t1 values ( 'd0' , 7 , 1.0 ) !
insert into kern.t1 values ( 'd0' , 8 , 1.0 ) !
insert into kern.t1 values ( 'd0' , 9 , 1.0 ) !
select * from kern.t1 !
lock table kern.t1 in exclusive mode !
rollback !
 ***********************************************************!
lock row kern.t1 key t1_c1='d0',t1_c2=1 in share mode !
lock row kern.t1 key t1_c1='d0',t1_c2=2 in exclusive mode !
rollback work !
insert kern.t1 values ( 'd5', 1 , 1 ) !
lock row kern.t1 key t1_c1 = 'd5' , t1_c2 =1 in share mode !
lock row kern.t1 key t1_c1 = 'd5' , t1_c2 =1
                                          in exclusive mode !
rollback work release  !
 ***********************************************************!
