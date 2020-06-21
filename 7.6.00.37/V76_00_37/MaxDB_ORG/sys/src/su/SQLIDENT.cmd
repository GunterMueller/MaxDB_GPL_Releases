file connect ( kern test sqlmode &1 !
create table t23456789012345is19 ( a char (30), b char (8))!
create table t2345678901234is18  ( a char (30), b char (8))!
create table t19 (c23456789012345is19 char (8), a char (8))!
create table t (c2345678901234is18  char (8), a char (8))!
create index i23456789012345is19 on t (a) !
declare c23456789012345is19 cursor for select * from t !
declare c2345678901234is18  cursor for select * from t !
declare c1 cursor for select a r23456789012345is19 from t !
declare c2 cursor for select a r2345678901234is18  from t !
commit work release !
file connect (kern test !
drop table t23456789012345is19 !
drop table t2345678901234is18  !
drop table t19 !
drop table t !
commit work release !
