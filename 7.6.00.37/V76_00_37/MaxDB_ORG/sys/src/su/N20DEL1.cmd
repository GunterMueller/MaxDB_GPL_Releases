*   *ID* N20DEL1  DBCMD    changed on 1992-09-28-13.25.29 by ADMIN     *
 *** DELETE wird getestet.
 *** DELETE /FROM/ <table name>   wird getestet.
 ***********************************************************!
file connect ( kern test !
 ***********************************************************!
create table t1 ( t1_c1 char(10) ) !
delete t1 !
create table t1 ( t1_c1 char(10) ) !
delete from t1 !
commit work !
delete t1 !
delete from t1 !
insert t1 values ( 'Hans' ) !
delete t1 !
select * from t1 !
fetch !
insert t1 values ( 'Hans' ) !
insert t1 values ( 'Otto' ) !
delete from t1 !
select * from t1 !
fetch !
delete t1 !
 ***********************************************************!
create table t2 ( t2_c1 char(10) range between
                          'a         ' and 'fzzzzzzzzz' ) !
delete  from t2 !
commit work !
delete t2 !
delete  from t2 !
insert t2  values ( 'aaaa' ) !
delete  t2 !
select * from t2 !
fetch !
insert t2 values ( 'aaaaaaaaaa' ) !
insert t2 values ( 'bbbbbbbb' ) !
insert t2 values ( 'cccccccccc') !
delete from t2 !
select * from t2 !
fetch !
delete t2 !
 ***********************************************************!
create table t3 ( t3_c1 fixed(5) not null ) !
delete  t3 !
create table t3 ( t3_c1 fixed(5) not null ) !
delete      t3 !
commit work !
delete    from  t3 !
delete      t3 !
insert  t3 values ( 1111 ) !
delete t3 !
select * from t3 !
fetch !
insert t3 values ( 2      ) !
insert t3 values ( 22222  ) !
delete     t3 !
select * from t3 !
fetch !
delete from  t3 !
 ***********************************************************!
create table t4 ( t4_c1 fixed(5,3) key ) !
delete t4 !
commit work !
delete from t4 !
insert  t4  values  ( 55.123 ) !
delete t4 !
select * from t4 !
fetch !
insert t4 values ( - .0   ) !
insert t4 values ( +33.   ) !
insert t4 values ( + .110 ) !
insert t4 values ( .01 )    !
delete t4 !
select * from t4 !
fetch !
delete t4 !
 ***********************************************************!
create table t5 ( t5_c1 fixed(7) , t5_c2 float (7) ) !
delete t5 !
create table t5 ( t5_c1 fixed(7) , t5_c2 float (7) ) !
delete   from    t5 !
commit work !
delete   t5   !
delete from t5 !
insert t5 values ( 1234567, 1234567) !
delete  t5 !
select * from t5 !
fetch !
insert t5 values ( - 123.0 , +22E-49 ) !
insert t5 values ( - .0 , - 33.33E12 ) !
delete from t5 !
select * from t5 !
fetch !
delete t5 !
 ***********************************************************!
create table t6 ( t6_c1 float(3) key range in (1,2,3,4,5,6)
                   , t6_c2 char(10) not null ) !
delete t6 !
create table t6 ( t6_c1 float(3) key range in (1,2,3,4,5,6)
                   , t6_c2 char(10) not null ) !
delete from t6 !
commit work !
delete t6 !
insert t6  values ( 1, 'Hans' ) !
delete  from t6 !
select * from t6 !
fetch !
insert t6 values ( 2 , 'Ottokar'  ) !
insert t6 values ( 3, 'Paul' ) !
delete  t6 !
select * from t6 !
fetch !
commit work !
 ***********************************************************!
create table t7 ( t7_c1 char(10) key, t7_c2 fixed (3, 2)
    not null range between -1.1 and +1.1, t7_c3 float(8) ) !
delete from  t7 !
commit work !
delete t7 !
insert t7   values ( 'Hans' , -1.1 , 1212 ) !
delete t7 !
select * from t7 !
fetch !
insert t7 ( t7_c1,t7_c2 ) values ( 'Paul', + 1.1 ) !
insert t7 values ( 'Otto' , 00 , null ) !
commit work !
delete from t7 !
select * from t7 !
fetch !
 ***********************************************************!
create table t8 ( t8_c1 fixed(18) key , t8_c2 float(9) key ) !
delete t8 !
create table t8 ( t8_c1 fixed(18) key , t8_c2 float(9) key ) !
delete t8 !
delete from t8 !
insert         t8 values ( 555555 ,- 8.3 ) !
commit work !
delete from t8 !
select * from t8 !
fetch !
insert t8 values ( 1223  , + 0.0000006 ) !
insert t8 values ( 1, 0e00) !
insert t8 values ( 2 , + 0.e5 ) !
insert t8 values ( 3 , - 123.2 ) !
insert t8 values ( 4 , 1e49 ) !
delete   t8 !
select * from t8 !
fetch !
rollback work !
 ***********************************************************!
create table t9 ( t9_c1 char(10) key range in ('1','2','3',
    '4','5','6','7','8','9') , t9_c2 fixed ( 5 ) key,
    t9_c3 fixed(5  ) range between -0 and 11111 ) !
delete from t9 !
commit work !
delete t9 !
insert  t9 ( t9_c1 , t9_c2 ) values ( '1', 1 ) !
delete t9 !
select * from t9 !
fetch !
insert t9  ( t9_c1 , t9_c2 ) values ( '1', 1 ) !
insert t9 values ( '1', 12345, 0 ) !
insert t9 values ( '2' , 12345, 11111. ) !
commit work !
delete from t9 !
select * from t9 !
fetch !
delete t9 !
 ***********************************************************!
create table t10 ( t10_c1 fixed(5,3) key, t10_c2 float(5) key,
    t10_c3 float (18) not null, t10_c4 char(14 ) ) !
delete t10 !
create table t10 ( t10_c1 fixed(5,3) key, t10_c2 float(5) key,
    t10_c3 float (18) not null, t10_c4 char(14 ) ) !
delete from t10 !
commit work !
delete t10 !
delete from t10 !
insert t10 values ( 5.3 , 0.0e44 , -.3e-35 , 'Paula' ) !
delete t10 !
select * from t10 !
fetch !
insert  t10  values ( 5.3 , 0.0e44 , -.3e-35 , 'Paula' ) !
insert t10 set t10_c1 =00.01 ,t10_c2 = 88,t10_c3 = +6e-7 !
insert t10 set t10_c1 = 00.01, t10_c2= 99, t10_c3 =+6e-7 !
insert t10 values ( -  2.8, 2223, 88, '12345' ) !
delete   from            t10  !
select * from t10 !
fetch !
 ***********************************************************!
commit work release !
