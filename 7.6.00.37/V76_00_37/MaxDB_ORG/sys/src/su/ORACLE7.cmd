file connect ( kern test sqlmode oracle !
create table t ( a char (8))!
insert into t values ('abcd')!
insert into t values ('ab_d')!
declare a cursor for select * from t where a like '%b_d' !
fetch a into :a !
declare a cursor for select * from t where a like '%b/_d' escape '/'!
fetch a into :a !
declare a cursor for select * from t where a like '%b/_d' escape '/'
   union all
                     select * from t !
fetch a into :a !
declare a cursor for select * from t where a = some 
                    ( select a from t )!
fetch a into :a !
drop table t !
create table t0 (a char(8)               check(a>'a')) !
create table t1 (a char(8) constraint c1 check(a>'a')) !
create table t2 (a char(8)               check(a>'a') constraint c1)!
drop table t0 !
drop table t1 !
drop table t2 !
create table t  ( a char (8) primary key)!
create table t1 (a1 char (8) references t (a) on delete cascade)!
drop table t !
drop table t1!
commit work release !
