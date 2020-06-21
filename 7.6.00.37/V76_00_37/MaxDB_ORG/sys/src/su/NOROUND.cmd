*   *ID* NOROUND  DBCMD    changed on 1992-09-28-13.24.41 by ADMIN     *
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
insert t5_5 set a =   0.1 !
insert f1   set a =     1 !
insert f5   set a =     1 !
*-------------------------!
update t1   set a = noround(     1) !
update t5   set a = noround(     1) !
update t5_2 set a = noround(     1) !
update t5_5 set a = noround(     1) !
update f1   set a = noround(     1) !
update f5   set a = noround(     1) !
*-------------------------!
update t1   set a = noround(    10) !
update t5   set a = noround(    10) !
update t5_2 set a = noround(    10) !
update t5_5 set a = noround(    10) !
update f1   set a = noround(    10) !
update f5   set a = noround(    10) !
*-------------------------!
update t1   set a = noround(    12) !
update t5   set a = noround(    12) !
update t5_2 set a = noround(    12) !
update t5_5 set a = noround(    12) !
update f1   set a = noround(    12) !
update f5   set a = noround(    12) !
*-------------------------!
update t1   set a = noround(  1234) !
update t5   set a = noround(  1234) !
update t5_2 set a = noround(  1234) !
update t5_5 set a = noround(  1234) !
update f1   set a = noround(  1234) !
update f5   set a = noround(  1234) !
*-------------------------!
update t1   set a = noround(123456) !
update t5   set a = noround(123456) !
update t5_2 set a = noround(123456) !
update t5_5 set a = noround(123456) !
update f1   set a = noround(123456) !
update f5   set a = noround(123456) !
*-------------------------!
update t1   set a = noround(  1.19) !
update t5   set a = noround(  1.19) !
update t5_2 set a = noround(  1.19) !
update t5_5 set a = noround(  1.19) !
update f1   set a = noround(  1.19) !
update f5   set a = noround(  1.19) !
*-------------------------!
update t1   set a = noround( 0.123456) !
update t5   set a = noround( 0.123456) !
update t5_2 set a = noround( 0.123456) !
update t5_5 set a = noround( 0.123456) !
update f1   set a = noround( 0.123456) !
update f5   set a = noround( 0.123456) !
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
commit release !
