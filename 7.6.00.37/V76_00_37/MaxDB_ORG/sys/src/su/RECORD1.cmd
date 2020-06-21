file connect ( kern test !
create table t like org !
alter table t add a boolean!
commit release !
file connect ( kern test sqlmode sapr3 !
alter table t modify varcol varchar (&1) ascii default
'abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz0123456789&2'!
commit release !
file connect ( kern test !
insert into t (k1,k2,a) values ('abcde','0', null)! 
insert into t (k1,k2,a) values ('abcde','a', true)! 
insert into t (k1,k2,a) values ('abcde','b', false)! 
select k2,a from t !
fetch into :a, :b!
rollback !
drop table t !
commit !
create table t like org !
create sequence seq1 minvalue 1 !
alter table t add a fixed (6)!
commit release !
file connect ( kern test sqlmode sapr3 !
alter table t modify varcol varchar (&1) ascii default
'abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz0123456789&2'!
commit release !
file connect ( kern test !
insert into t (k1,k2,a) values ('abcde','0', null)! 
insert into t (k1,k2,a) values ('abcde','a', timezone)! 
insert into t (k1,k2,a) values ('abcde','b', seq1.nextval)! 
insert into t (k1,k2,a) values ('abcde','c', 0)! 
insert into t (k1,k2,a) values ('abcde','d', 12345)! 
select k2,a from t !
fetch into :a, :b!
rollback !
drop table t !
drop sequence seq1 !
commit !
create table t like org !
alter table t add a fixed (8)!
commit release !
file connect ( kern test sqlmode sapr3 !
alter table t modify varcol varchar (&1) ascii default
'abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz0123456789&2'!
insert into t (k1,k2,a) values ('abcde','a', uid)! 
insert into t (k1,k2,a) values ('abcde','0', '-123.456')! 
select k2,a from t !
fetch into :a, :b!
drop table t!
commit work release !
file connect ( kern test !
create table t like org !
alter table t add a float (6)!
commit release !
file connect ( kern test sqlmode sapr3 !
alter table t modify varcol varchar (&1) ascii default
'abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz0123456789&2'!
commit release !
file connect ( kern test !
insert into t (k1,k2,a) values ('abcde','0', null)! 
insert into t (k1,k2,a) values ('abcde','a', pi)! 
insert into t (k1,k2,a) values ('abcde','b', 0)! 
insert into t (k1,k2,a) values ('abcde','c', 12345)! 
select k2,a from t !
fetch into :a, :b!
rollback !
drop table t !
commit !
create table t like org !
alter table t add a char (62) ascii!
commit release !
file connect ( kern test sqlmode sapr3 !
alter table t modify varcol varchar (&1) ascii default
'abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz0123456789&2'!
commit release !
file connect ( kern test !
insert into t (k1,k2,a) values ('abcde','0', null)! 
insert into t (k1,k2,a) values ('abcde','a', user)! 
insert into t (k1,k2,a) values ('abcde','b', usergroup)! 
insert into t (k1,k2,a) values ('abcde','c', database)! 
insert into t (k1,k2,a) values ('abcde','d', sysdba)! 
insert into t (k1,k2,a) values ('abcde','g', 'a')! 
insert into t (k1,k2,a) values ('abcde','h', 'abcdefcg')! 
select k2,a from t !
fetch into :a, :b!
rollback !
drop table t !
commit !
create table t like org !
alter table t add a char (8) ascii!
commit release !
file connect ( kern test sqlmode sapr3 !
alter table t modify varcol varchar (&1) ascii default
'abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz0123456789&2'!
insert into t (k1,k2,a) values ('abcde','0', -123.456)! 
select k2,a from t !
fetch into :a, :b!
drop table t !
commit release !
file connect ( kern test !
create table t like org !
alter table t add a char(10) byte !
commit release !
file connect ( kern test sqlmode sapr3 !
alter table t modify varcol varchar (&1) ascii default
'abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz0123456789&2'!
commit release !
file connect ( kern test !
insert into t (k1,k2,a) values ('abcde','0', null)! 
insert into t (k1,k2,a) values ('abcde','a', stamp)! 
insert into t (k1,k2,a) values ('abcde','b', transaction)! 
insert into t (k1,k2,a) values ('abcde','c', x'aa')! 
insert into t (k1,k2,a) values ('abcde','d', x'aabbcc')! 
select k2,a from t !
fetch into :a, :b!
rollback !
drop table t !
commit !
create table t like org !
alter table t add a date !
commit release !
file connect ( kern test sqlmode sapr3 !
alter table t modify varcol varchar (&1) ascii default
'abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz0123456789&2'!
commit release !
file connect ( kern test !
insert into t (k1,k2,a) values ('abcde','0', null)! 
insert into t (k1,k2,a) values ('abcde','a', date)! 
insert into t (k1,k2,a) values ('abcde','b', '19800324')! 
select k2,a from t !
fetch into :a, :b!
rollback !
drop table t !
commit !
create table t like org !
alter table t add a time !
commit release !
file connect ( kern test sqlmode sapr3 !
alter table t modify varcol varchar (&1) ascii default
'abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz0123456789&2'!
commit release !
file connect ( kern test !
insert into t (k1,k2,a) values ('abcde','0', null)! 
insert into t (k1,k2,a) values ('abcde','a', time)! 
insert into t (k1,k2,a) values ('abcde','b', '00153647')! 
select k2,a from t !
fetch into :a, :b!
rollback !
drop table t !
commit !
create table t like org !
alter table t add a timestamp !
commit release !
file connect ( kern test sqlmode sapr3 !
alter table t modify varcol varchar (&1) ascii default
'abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz0123456789&2'!
commit release !
file connect ( kern test !
insert into t (k1,k2,a) values ('abcde','0', null)! 
insert into t (k1,k2,a) values ('abcde','a', timestamp)! 
insert into t (k1,k2,a) values ('abcde','b', '19800324153647654321')! 
select k2,a from t !
fetch into :a, :b!
rollback !
drop table t !
commit release !
