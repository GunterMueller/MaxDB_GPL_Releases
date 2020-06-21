*   *ID* N00T0603 DBCMD    changed on 1992-09-28-13.24.52 by ADMIN     *
 *** 6.3  <lock statement> !
file connect ( kern test !
create table kern.t12 ( t12_c1 fixed(5) key,
                          t12_c2 fixed(5) key,
                          t12_c3 char(12),
                          t12_c4 float(5) ) !
insert kern.t12 values ( 88 , 0 , 'Karl' , null ) !
commit work !
lock table kern.t12 in share mode !
select * from kern.t12 !
create table kern.t13 ( t13_c1 char (10) ) !
insert kern.t13 values ( 'Elisabeth' ) !
lock table kern.t12, kern.t13 in exclusive mode !
update kern.t12 set t12_c4 = 72 !
delete kern.t13 !
create synonym  syn_t12 for kern.t12 !
lock ( nowait ) table syn_t12 in exclusive mode !
commit work !
lock ( nowait ) row kern.t12 key t12_c1 = 88, t12_c2 = 0
    in exclusive mode !
insert kern.t13 values ( 'Hanna' ) !
commit work !
lock row syn_t12 key t12_c1 = 88 , t12_c2 =0
     row kern.t12 key t12_c1 = 12, t12_c2 = 65
     in  exclusive  mode !
update kern.t12 set t12_c3 = 'Karin'
     key t12_c1=88,t12_c2 = 0 !
insert kern.t12 values (+12, 65, null,-3  ) !
commit work !
lock  table kern.t13   row t12 key t12_c1 =88, t12_c2=0
     in share mode !
lock ( nowait ) table kern.t13 row t12 key t12_c1 =88, t12_c2 = 0
    in exclusive mode !
rollback work !
delete kern.t12 key t12_c1  = 88 , t12_c2 =0 !
insert syn_t12 values (88,0, 'Karin', 72 ) !
insert kern.t12 values ( 12 , 64, null, -3 ) !
commit work !
lock ( wait ) table kern.t13 in share mode
     row kern.t12 key t12_c1 = 88, t12_c2 = 0
     in exclusive mode !
update kern.t12 set t12_c4 = 92
    key t12_c1= 88 , t12_c2 = 0 !
rollback work !
lock ( wait ) row kern.t12 key t12_c1 = 12, t12_c2 = 65 in share  mode
     table kern.t13 in  exclusive mode !
rollback !
lock row kern.t13 key syskey = X'0001' in share mode !
lock table kern.t12 in share mode
     row kern.t13 key syskey = x'0000000000000002' in exclusive mode !
commit work release !
