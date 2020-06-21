*   *ID* N39SUB1  DBCMD    changed on 1992-09-28-13.26.28 by ADMIN     *
file CONNECT ( KERN TEST !
create table t1 ( t1c1 fixed(7,2) key, t1c2 char(10) ) !
insert t1 values ( 1, 'Hans' ) !
insert t1 values ( 30, 'Otto' ) !
insert t1 values ( 200, 'Paul' ) !
insert t1 values (  10, 'Otto' ) !
insert t1 values ( -5, null ) !
insert t1 values ( 1000, null ) !
create table t2 ( t2c1 fixed(5), t2c2 char(10) ) !
insert t2 values ( 30, 'Karin' ) !
insert t2 values ( 200, 'Ilse' ) !
insert t2 values ( 3000, null ) !
insert t2 values ( 200, 'Marlis' ) !
insert t2 values ( -50 , 'Karin' ) !
select * from t1 !
fetch !
select * from t2 !
fetch !
commit work !
delete t1 where t1c1 <= all ( select t2c1 from t2 where t2.t2c1 > 0 ) !
select * from t1 !
fetch !
rollback work !
delete t1 where
      10*t1c1 =any ( select t2.t2c1 * 10 from t2 where t2c2 <> 'Ilse')
    or t1c2 <= all ( select t2c2 from t2 where t2c2 is not null ) !
select * from t1 !
fetch !
rollback work !
delete t1 key t1c1 = 30
          where t1c2 > ALL (select t2.t2c2 from t2 where t2c1 < 1000 ) !
select * from t1 !
fetch !
rollback work !
delete t1 where t1c2 <> all ( select t2c2 from t2 ) !
select * from t1 !
fetch !
rollback work !
delete t1 where t1c1 in ( select t2c1 from t2 ) !
select * from t1 !
fetch !
rollback work !
delete t1 where t1c1 <> any ( select t1c1 from t1 ) !
rollback work !
delete t1 where t1c1 <> all ( select t2c1 from t1,t2 ) !
select * from t1 !
fetch !
rollback work !
update t1 set t1c2 = substr ( t1c2, 3 )
     where t1.t1c2 < any ( select t2c2 from t2 ) !
select * from t1 !
fetch !
rollback work !
update t1 set t1c2 = t1c2 & t1c2
   where t1c1 = any ( select t2c1 from t2 where t2c1 >0 )
     and t1c2 is not null !
select * from t1 !
fetch !
rollback work !
update t1 set t1c2 = 'Bernd' key t1c1 = 200
   where t1c2 > all(select t2c2 from t2 where t2c2 is not null ) !
select * from t1 !
fetch !
rollback work !
update t1 set t1c2 = 'Bernd' key t1c1 = 200
   where t1c2 > all(select t2c2 from t2 ) !
select * from t1 !
fetch !
rollback work !
update t1 set t1c2 = 'Heinz' where t1c2 <= all ( select t1c2 from t1 ) !
update t1 set t1c2 = 'Heinz'
   where t1c2 <= all ( select t1c2 from t1 where t1c2 is not null ) !
select * from t1 !
fetch !
rollback work !
update t1 set t1c2 = 'Peter'
    where t1c1 < any ( select t2c1 from t2
                       where t2c2 < any ( select t1.t1c2 from t1 ) ) !
select * from t1 !
fetch !
rollback work release !
