*   *ID* DMLPRIV4 DBCMD    changed on 1992-09-28-13.23.24 by ADMIN     *
 * user with DELETE privilege only
 * is only allowed to
 * 1) delete
 * 2) lock in exclusive mode ( table and rows ) !
 * 3) use every column in key spec and search condition
 *    ( as SYBASE and ORACLE do it ( check with V6 ) )!
file connect ( d8 w !
lock table kern.t1 in share mode !
update of kern.t1 set t1_c3=1.1 key t1_c1='d0',t1_c2=2 !
delete kern.t1 key t1_c1 = 'd0' , t1_c2 = 3 !
insert into kern.t1 values ( 'd0' , 7 , 1.0 ) !
select * from kern.t1 !
rollback !
lock table kern.t1 in exclusive mode !
delete kern.t1 where t1_c1 = 'd0' and t1_c2 = 3 !
delete kern.t1 where t1_c3 <> 4 !
rollback !
delete kern.t1 !
rollback !
 ***********************************************************!
lock row kern.t1  key t1_c1 = 'd0' , t1_c2 = 1
                              in exclusive mode !
delete kern.t1 key t1_c1 = 'd0', t1_c2 =2 !
delete kern.t1 key t1_c1 = 'd0' , t1_c2 =1 !
rollback work release !
