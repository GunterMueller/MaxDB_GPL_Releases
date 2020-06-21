*   *ID* N00T0601 DBCMD    changed on 1992-09-28-13.24.51 by ADMIN     *
 *** 6.1  <commit statement> !
file connect ( sut sut !
create user d5 password d5xw dba !
commit work !
rollback work release !
file connect ( d5 d5xw !
create table d5xt1 ( d5xt1xc1 char(10) ) !
insert d5xt1 values ( 'Friedrich' ) !
update d5xt1 set d5xt1xc1 = 'Fred'
   where d5xt1xc1  = 'Friedrich' !
commit !
create table d5xt2 ( d5xt2xc1 char(5) key, d5xt2xc2 fixed(5) ) !
insert d5xt2 values ( 'bbb', 1 ) !
insert d5xt2 values ( 'aaa', 7 ) !
commit !
create table d5xt3 ( d5xt3xc1 fixed(5) key,
                     d5xt3xc2 char(8) ascii key,
                     d5xt3xc3 char(8) ascii ) !
create table d5xt4 ( d5xt4xc1 char(5) ascii ) !
insert d5xt3 values ( 1, 'aaa', 'cc' ) !
insert d5Xt3 values ( 5, 'fff', 'aa' ) !
commit !
insert d5xt1 values ( 'Hans' ) !
select * from d5xt1 where d5xt1xc1 >= 'F' !
commit work release !
