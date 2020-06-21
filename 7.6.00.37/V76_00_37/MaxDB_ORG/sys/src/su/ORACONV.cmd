file connect ( kern test sqlmode oracle !
drop table t1 !
drop table t2 !
drop table t3 !
drop table t4 !
drop table t5 !
create table t1 (c char (18))!
insert into t1 values ('0')!
insert into t1 values ('1.234')!
insert into t1 values ('0.1234') !
insert into t1 values ('.1234') !
insert into t1 values ('-.1234') !
insert into t1 values ('- .1234') !
insert into t1 values ('123456')!
insert into t1 values ('0.00007')!
insert into t1 values ('.00007')!
insert into t1 values ('-3.4')!
insert into t1 values ('-0.45')!
insert into t1 values ('-.45')!
declare d cursor for select * from t1 !
fetch d into :a !
declare d cursor for select * from t1 where c = 0!
fetch d into :a !
declare d cursor for select * from t1 where c = 1.234!
fetch d into :a !
declare d cursor for select * from t1 where c = 0.1234!
fetch d into :a !
declare d cursor for select * from t1 where c = 1.234e0!
fetch d into :a !
declare d cursor for select * from t1 where c = 1.234e-1!
fetch d into :a !
declare d cursor for select * from t1 where c = -0.1234!
fetch d into :a !
declare d cursor for select * from t1 where c = 123456!
fetch d into :a !
declare d cursor for select * from t1 where c = 1.23456e5!
fetch d into :a !
declare d cursor for select * from t1 where c = 0.00007!
fetch d into :a !
declare d cursor for select * from t1 where c = 7e-5!
fetch d into :a !
declare d cursor for select * from t1 where c = -3.4 !
fetch d into :a !
declare d cursor for select * from t1 where c = -.45 !
fetch d into :a !
create table t2 (c char (18))!
insert into t2 values (0)!
insert into t2 values (1.234)!
insert into t2 values (0.1234) !
insert into t2 values (.1234) !
insert into t2 values (-.1234) !
insert into t2 values (- .1234) !
insert into t2 values (123456)!
insert into t2 values (0.00007)!
insert into t2 values (.00007)!
insert into t2 values (-3.4)!
insert into t2 values (-0.45)!
insert into t2 values (-.45)!
declare d cursor for select * from t2 !
fetch d into :a !
declare d cursor for select * from t2 where c = 0!
fetch d into :a !
declare d cursor for select * from t2 where c = 1.234!
fetch d into :a !
declare d cursor for select * from t2 where c = 0.1234!
fetch d into :a !
declare d cursor for select * from t2 where c = 1.234e0!
fetch d into :a !
declare d cursor for select * from t2 where c = 1.234e-1!
fetch d into :a !
declare d cursor for select * from t2 where c = -0.1234!
fetch d into :a !
declare d cursor for select * from t2 where c = 123456!
fetch d into :a !
declare d cursor for select * from t2 where c = 1.23456e5!
fetch d into :a !
declare d cursor for select * from t2 where c = 0.00007!
fetch d into :a !
declare d cursor for select * from t2 where c = 7e-5!
fetch d into :a !
declare d cursor for select * from t2 where c = -3.4 !
fetch d into :a !
declare d cursor for select * from t2 where c = -.45 !
fetch d into :a !
create table t3 (c float (18))!
insert into t3 values ('0')!
insert into t3 values ('1.234')!
insert into t3 values ('0.1234') !
insert into t3 values ('.1234') !
insert into t3 values ('-.1234') !
insert into t3 values ('- .1234') !
insert into t3 values ('123456')!
insert into t3 values ('0.00007')!
insert into t3 values ('.00007')!
insert into t3 values ('-3.4')!
insert into t3 values ('-0.45')!
insert into t3 values ('-.45')!
declare d cursor for select * from t3 !
fetch d into :a !
declare d cursor for select * from t3 where c = '0'!
fetch d into :a !
declare d cursor for select * from t3 where c = '1.234'!
fetch d into :a !
declare d cursor for select * from t3 where c = '0.1234'!
fetch d into :a !
declare d cursor for select * from t3 where c = '1.234e0'!
fetch d into :a !
declare d cursor for select * from t3 where c = '1.234e-1'!
fetch d into :a !
declare d cursor for select * from t3 where c = '-0.1234'!
fetch d into :a !
declare d cursor for select * from t3 where c = '123456'!
fetch d into :a !
declare d cursor for select * from t3 where c = '1.23456e5'!
fetch d into :a !
declare d cursor for select * from t3 where c = '0.00007'!
fetch d into :a !
declare d cursor for select * from t3 where c = '7e-5'!
fetch d into :a !
declare d cursor for select * from t3 where c = '-3.4'!
fetch d into :a !
declare d cursor for select * from t3 where c = '-.45'!
fetch d into :a !
create table t4 (c float (18))!
insert into t4 values (0)!
insert into t4 values (1.234)!
insert into t4 values (0.1234) !
insert into t4 values (.1234) !
insert into t4 values (-.1234) !
insert into t4 values (- .1234) !
insert into t4 values (123456)!
insert into t4 values (0.00007)!
insert into t4 values (.00007)!
insert into t4 values (-3.4)!
insert into t4 values (-0.45)!
insert into t4 values (-.45)!
declare d cursor for select * from t4 !
fetch d into :a !
declare d cursor for select * from t4 where c = '0'!
fetch d into :a !
declare d cursor for select * from t4 where c = '1.234'!
fetch d into :a !
declare d cursor for select * from t4 where c = '0.1234'!
fetch d into :a !
declare d cursor for select * from t4 where c = '1.234e0'!
fetch d into :a !
declare d cursor for select * from t4 where c = '1.234e-1'!
fetch d into :a !
declare d cursor for select * from t4 where c = '-0.1234'!
fetch d into :a !
declare d cursor for select * from t4 where c = '123456'!
fetch d into :a !
declare d cursor for select * from t4 where c = '1.23456e5'!
fetch d into :a !
declare d cursor for select * from t4 where c = '0.00007'!
fetch d into :a !
declare d cursor for select * from t4 where c = '7e-5'!
fetch d into :a !
declare d cursor for select * from t4 where c = '-3.4'!
fetch d into :a !
declare d cursor for select * from t4 where c = '-.45'!
fetch d into :a !
commit release !
file connect ( kern test !
create table t5 (c char (18))!
commit release !
file connect ( kern test sqlmode oracle !
insert into t5 values ('1.234')!
declare d cursor for select * from t5 where c = 1.234!
fetch d into :a !
delete from t5 !
insert into t5 values (1.234)!
declare d cursor for select * from t5 where c = 1.234!
fetch d into :a !
drop table t1 !
drop table t2 !
drop table t3 !
drop table t4 !
drop table t5 !
commit release !
