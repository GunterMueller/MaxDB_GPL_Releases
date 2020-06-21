*   *ID* DMLPRIV1 DBCMD    changed on 1992-09-28-13.23.22 by ADMIN     *
 ***********************************************************
 * user with all privilege may do everything !
connect d0 identified by w isolation level  0 !
lock table kern.t1 in share mode !
lock table kern.t1 in exclusive mode !
rollback work !
lock row kern.t1 key t1_c1 = 'd0' , t1_c2 = 1
     row kern.t1 key t1_c1 = 'd0' , t1_c2=5 in exclusive mode!
lock row kern.t1 key t1_c1 = 'd0' , t1_c2 = 1
     row kern.t1 key t1_c1 = 'd0' , t1_c2=5 in share mode!
rollback !
update of kern.t1 set t1_c2 = 2 key t1_c1='d0',t1_c2=2 !
delete kern.t1 key t1_c1 = 'd0' , t1_c2 = 3 !
insert into kern.t1 values ( 'd0' , 7 , 1.0 ) !
select * from kern.t1 !
fetch !
select t1_c1, t1_c2 from kern.t1 !
fetch !
select t1_c1, t1_c2 from kern.t1 where t1_c1 <> 'q' and
       t1_c2 < 7 and t1_c3 <> 4 !
fetch !
rollback work release !
 ***********************************************************
 * user with SELECT privilege only
 * is only allowed to
 * 1) use all columns in select list
 *    and search condition of a select
 * 2) to lock in share mode ( table and rows ) !
connect d1 identified by w isolation level  0 !
lock table kern.t1 in share mode !
lock table kern.t1 in exclusive mode !
rollback work !
lock row kern.t1 key t1_c1 = 'd0' , t1_c2 = 1
     row kern.t1 key t1_c1 = 'd0' , t1_c2=5 in exclusive mode!
lock row kern.t1 key t1_c1 = 'd0' , t1_c2 = 1
     row kern.t1 key t1_c1 = 'd0' , t1_c2=5 in share mode!
rollback !
update of kern.t1 set t1_c2 = 2 key t1_c1='d0',t1_c2=2 !
delete kern.t1 key t1_c1 = 'd0' , t1_c2 = 3 !
insert into kern.t1 values ( 'd0' , 7 , 1.0 ) !
select * from kern.t1 !
fetch !
select t1_c1, t1_c2 from kern.t1 !
fetch !
select t1_c1, t1_c2 from kern.t1 where t1_c1 <> 'q' and
       t1_c2 < 7 and t1_c3 <> 4 !
fetch !
rollback work release !
 ***********************************************************
 * user with SELECT privilege on column t1_c1
 * is only allowed to
 * 1) use t1_c1 in select list and search condition of a select
 * 2) to lock in share mode ( table only ) !
connect d2 identified by w  isolation level  0 !
lock table kern.t1 in share mode !
lock table kern.t1 in exclusive mode !
rollback !
lock row kern.t1 key t1_c1 = 'd0', t1_c2 = 1
              in exclusive mode !
lock row kern.t1 key t1_c1 = 'd0', t1_c2 = 1
              in share mode !
rollback work !
update of kern.t1 set t1_c3=1.1 key t1_c1='d0',t1_c2=2 !
delete kern.t1 key t1_c1 = 'd0' , t1_c2 = 3 !
insert into kern.t1 values ( 'd0' , 7 , 1.0 ) !
select * from kern.t1 !
fetch !
select t1_c1 from kern.t1 !
fetch !
select t1_c1, t1_c2 from kern.t1 !
select t1_c1 from kern.t1 where t1_c1 <> 'q' and
       t1_c2 < 7 !
rollback work release !
 ***********************************************************
 * user with SELECT privilege on column t1_c1 and t1_c2
 * is only allowed to
 * 1) use t1_c1 and t1_c2 in select list and
 *    search condition of a select
 * 2) to lock in share mode ( table and rows ) !
connect d3 identified by w isolation level  0 !
lock row kern.t1 key t1_c1 = 'd0' , t1_c2 = 1
     row kern.t1 key t1_c1 = 'd0' , t1_c2=5 in exclusive mode!
lock table kern.t1 in exclusive mode !
lock row kern.t1 key t1_c1 = 'd0' , t1_c2 = 1
     row kern.t1 key t1_c1 = 'd0' , t1_c2=5 in share mode!
rollback !
lock table kern.t1 in share mode !
update of kern.t1 set t1_c2 = 2 key t1_c1='d0',t1_c2=2 !
delete kern.t1 key t1_c1 = 'd0' , t1_c2 = 3 !
insert into kern.t1 values ( 'd0' , 7 , 1.0 ) !
select * from kern.t1 !
fetch !
select t1_c1, t1_c2 from kern.t1 !
fetch !
select t1_c1, t1_c2 from kern.t1 where t1_c1 <> 'q' and
       t1_c2 < 7 !
fetch !
select t1_c3 from kern.t1 !
select t1_c1, t1_c2 from kern.t1 where t1_c1 <> 'q' and
       t1_c2 < 7 and t1_c3 <> 4 !
rollback work release !
 ***********************************************************
 * user with SELECT privilege on column t1_c3
 * is only allowed to
 * 1) use t1_c3 in select list and search condition of a select
 * 2) to lock in share mode ( table only ) !
connect d4 identified by w isolation level  0 !
lock table kern.t1 in share mode !
lock table kern.t1 in exclusive mode !
rollback work !
lock row kern.t1 key t1_c1 = 'd0' , t1_c2 = 1
     row kern.t1 key t1_c1 = 'd0' , t1_c2 = 2
     row kern.t1 key t1_c1 ='d0',t1_c2= 3 in share mode !
update of kern.t1 set t1_c2 = 2 key t1_c1='d0',t1_c2=2 !
delete kern.t1 key t1_c1 = 'd0' , t1_c2 = 3 !
insert into kern.t1 values ( 'd0' , 7 , 1.0 ) !
select t1_c2 from kern.t1 !
select * from kern.t1 !
fetch !
select t1_c3 from kern.t1 !
fetch !
select t1_c3 from kern.t1 where t1_c3 <> 4 !
fetch !
rollback work release !
 ***********************************************************!
