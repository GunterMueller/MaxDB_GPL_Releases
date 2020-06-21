*   *ID* DMLPRIV5 DBCMD    changed on 1992-09-28-13.23.24 by ADMIN     *
 ***********************************************************!
 * user with UPDATE privilege for t1_c3 and SELECT for t1_c2
 * is only allowed to
 * 1) update t1_c3
 * 2) lock in exclusive mode ( table and rows )
 * 3) use every column of the table in key spec and
 *    search condition
 *    and perhaps in set_clause on the right side
 *    ( as SYBASE and ORACLE do it (checked with V6 ) )!
 * 4) to use t1_c2 in every select list, search condition and
 *    key spec !
connect d9 identified by w isolation level  0 !
update of kern.t1 set t1_c3 = 1.1
                 key t1_c1 = 'd0', t1_c2 = 2 !
update of kern.t1 set t1_c3 = 1.1 where t1_c2 = 2 !
update of kern.t1 set t1_c3 = 1.4 where t1_c3 >= 1.0 !
update of kern.t1 set t1_c3 = t1_c2 !
update of kern.t1 set t1_c3 = num(t1_c1) !
update of kern.t1 set t1_c3 = 1.2 !
delete kern.t1 key t1_c1 = 'd0' , t1_c2 = 3 !
insert into kern.t1 values ( 'd0' , 7 , 1.0 ) !
select * from kern.t1 !
lock row kern.t1 key t1_c1 = 'd0', t1_c2 =3 in share mode !
lock row kern.t1 key t1_c1='d0',t1_c2=3 in exclusive mode !
lock table kern.t1 in exclusive mode !
rollback work release !
 ***********************************************************!
