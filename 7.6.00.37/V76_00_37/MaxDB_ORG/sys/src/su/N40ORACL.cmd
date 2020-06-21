file connect ( kern test !
create table t ( a date, b time, c char (8))!
insert into t values (date, time, 'abc')!
commit release !
file connect ( kern test sqlmode db2 !
create table t1 (a1 timestamp, c1 char (8))!
insert into t1 values (timestamp, 'xyz')!
commit release !
file connect ( kern test sqlmode oracle !
declare c cursor for select * from t!
fetch c into :a, :b, :c !
close c!
declare c cursor for select * from t1 !
fetch c into :a, :b !
close c!
create table t2 (a2 date, c2 char (8))!
insert into t2 values ('1jan92                    ', 'abc' ) !
insert into t2 values ('01JUN3                    ', 'abc' ) !
insert into t2 values ('1            dec 45       ', 'abc' ) !
insert into t2 values ('23!;!apr--------92        ', 'abc' ) !
insert into t2 values ('14+sep+2                  ', 'abc' ) !
insert into t2 values ('20-MAY-72                 ', 'abc' ) !
insert into t2 values (sysdate                     , 'abc' ) !
insert into t2 values ('1jam92                    ', 'abc' ) !
insert into t2 values ('001jun92                  ', 'abc' ) !
insert into t2 values ('1JAN92;                   ', 'abc' ) !
insert into t2 values ('22ejane92                 ', 'abc' ) !
declare c cursor for select * from t2 !
fetch c into :a, :b !
commit release !
file connect ( kern test !
drop table t !
drop table t1 !
drop table t2 !
commit release !