*   *ID* ROUND    DBCMD    changed on 1992-09-28-13.27.58 by ADMIN     *
file connect ( kern test !
create table t1   (a fixed ( 1, 0) )!
create table t5   (a fixed ( 5, 0) )!
create table t5_2 (a fixed ( 5, 2) )!
create table t5_5 (a fixed ( 5, 5) )!
create table f1   (a float ( 1   ) )!
create table f5   (a float ( 5   ) )!
commit !
*-------------------------!
insert t1   set a =     1 !
insert t5   set a =     1 !
insert t5_2 set a =     1 !
insert t5_5 set a =     1 !
insert f1   set a =     1 !
insert f5   set a =     1 !
*-------------------------!
insert t1   set a =    10 !
insert t5   set a =    10 !
insert t5_2 set a =    10 !
insert t5_5 set a =    10 !
insert f1   set a =    10 !
insert f5   set a =    10 !
*-------------------------!
insert t1   set a =    12 !
insert t5   set a =    12 !
insert t5_2 set a =    12 !
insert t5_5 set a =    12 !
insert f1   set a =    12 !
insert f5   set a =    12 !
*-------------------------!
insert t1   set a =  1234 !
insert t5   set a =  1234 !
insert t5_2 set a =  1234 !
insert t5_5 set a =  1234 !
insert f1   set a =  1234 !
insert f5   set a =  1234 !
*-------------------------!
insert t1   set a =123456 !
insert t5   set a =123456 !
insert t5_2 set a =123456 !
insert t5_5 set a =123456 !
insert f1   set a =123456 !
insert f5   set a =123456 !
*-------------------------!
insert t1   set a =  1.19 !
insert t5   set a =  1.19 !
insert t5_2 set a =  1.19 !
insert t5_5 set a =  1.19 !
insert f1   set a =  1.19 !
insert f5   set a =  1.19 !
*-------------------------!
insert t1   set a = 0.123456 !
insert t5   set a = 0.123456 !
insert t5_2 set a = 0.123456 !
insert t5_5 set a = 0.123456 !
insert f1   set a = 0.123456 !
insert f5   set a = 0.123456 !
*-------------------------!
select * from t1   !
fetch!
select * from t5   !
fetch!
select * from t5_2 !
fetch!
select * from t5_5 !
fetch!
select * from f1   !
fetch!
select * from f5   !
fetch!
drop table t1   !
drop table t5   !
drop table t5_2 !
drop table t5_5 !
drop table f1   !
drop table f5   !
create table t ( a fixed (18,16))!
insert t set a = 12.3456789 !
select round (a,-1) e1, a from t !
fetch !
commit release !
