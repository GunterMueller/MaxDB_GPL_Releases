file connect ( kern test !
create table t like org !
alter table t add a boolean!
commit release !
file connect ( kern test sqlmode sapr3 !
alter table t modify varcol varchar (&1) ascii default
'abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz0123456789&2'!
commit release !
file connect ( kern test !
pars_then_ex !
data !
insert into t (k1,k2,a) values ('abcde','0', :a)! 
b 1 HI

insert into t (k1,k2,a) values ('abcde','a', :a)! 
b 1 01

insert into t (k1,k2,a) values ('abcde','b', :a)! 
b 1 00

pars_execute !
nodata !
select k2,a from t !
fetch into :a, :b !
rollback !
drop table t !
diagnose share parse drop !
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
pars_then_ex !
data !
insert into t (k1,k2,a) values ('abcde','0', :a)! 
b 1 HI

insert into t (k1,k2,a) values ('abcde','a', timezone)! 

insert into t (k1,k2,a) values ('abcde','b', seq1.nextval)! 

insert into t (k1,k2,a) values ('abcde','c', :a)! 
n 6 0

insert into t (k1,k2,a) values ('abcde','d', :a)! 
n 6 12345

pars_execute !
nodata !
select k2,a from t !
fetch into :a, :b !
rollback !
drop table t !
diagnose share parse drop !
commit !
create table t like org !
alter table t add a float (6)!
commit release !
file connect ( kern test sqlmode sapr3 !
alter table t modify varcol varchar (&1) ascii default
'abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz0123456789&2'!
commit release !
file connect ( kern test !
pars_then_ex !
data !
insert into t (k1,k2,a) values ('abcde','0', :a)! 
b 1 HI

insert into t (k1,k2,a) values ('abcde','a', pi)! 

insert into t (k1,k2,a) values ('abcde','b', :a)! 
n 6 0

insert into t (k1,k2,a) values ('abcde','c', :a)! 
n 6 12345

pars_execute !
nodata !
select k2,a from t !
fetch into :a, :b !
rollback !
drop table t !
diagnose share parse drop !
commit !
create table t like org !
alter table t add a char (62) ascii!
commit release !
file connect ( kern test sqlmode sapr3 !
alter table t modify varcol varchar (&1) ascii default
'abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz0123456789&2'!
commit release !
file connect ( kern test !
pars_then_ex !
data !
insert into t (k1,k2,a) values ('abcde','0', :a)! 
b 1 HI

insert into t (k1,k2,a) values ('abcde','a', user)! 

insert into t (k1,k2,a) values ('abcde','b', usergroup)! 

insert into t (k1,k2,a) values ('abcde','c', database)! 

insert into t (k1,k2,a) values ('abcde','d', sysdba)! 

insert into t (k1,k2,a) values ('abcde','g', :a)! 
c 64 'a'

insert into t (k1,k2,a) values ('abcde','h', :a)! 
c 64 'abcdefgh'

pars_execute !
nodata !
select k2,a from t !
fetch into :a, :b !
rollback !
drop table t !
diagnose share parse drop !
commit !
create table t like org !
alter table t add a char(10) byte !
commit release !
file connect ( kern test sqlmode sapr3 !
alter table t modify varcol varchar (&1) ascii default
'abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz0123456789&2'!
commit release !
file connect ( kern test !
pars_then_ex !
data !
insert into t (k1,k2,a) values ('abcde','0', :a)! 
b 1 HI

insert into t (k1,k2,a) values ('abcde','a', stamp)! 

insert into t (k1,k2,a) values ('abcde','b', transaction)! 

insert into t (k1,k2,a) values ('abcde','c', :a)! 
b 10 AA

insert into t (k1,k2,a) values ('abcde','d', :a)! 
b 10 AABBCC

pars_execute !
nodata !
select k2,a from t !
fetch into :a, :b !
rollback !
drop table t !
diagnose share parse drop !
commit !
create table t like org !
alter table t add a date !
commit release !
file connect ( kern test sqlmode sapr3 !
alter table t modify varcol varchar (&1) ascii default
'abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz0123456789&2'!
commit release !
file connect ( kern test !
pars_then_ex !
data !
insert into t (k1,k2,a) values ('abcde','0', :a)! 
b 1 HI

insert into t (k1,k2,a) values ('abcde','a', date)! 

insert into t (k1,k2,a) values ('abcde','b', :a)! 
c 8 '19800324'

pars_execute !
nodata !
select k2,a from t !
fetch into :a, :b !
rollback !
drop table t !
diagnose share parse drop !
commit !
create table t like org !
alter table t add a time !
commit release !
file connect ( kern test sqlmode sapr3 !
alter table t modify varcol varchar (&1) ascii default
'abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz0123456789&2'!
commit release !
file connect ( kern test !
pars_then_ex !
data !
insert into t (k1,k2,a) values ('abcde','0', :a)! 
b 1 HI

insert into t (k1,k2,a) values ('abcde','a', time)! 

insert into t (k1,k2,a) values ('abcde','b', :a)! 
c 8 '00153647'

pars_execute !
nodata !
select k2,a from t !
fetch into :a, :b !
rollback !
drop table t !
diagnose share parse drop !
commit !
create table t like org !
alter table t add a timestamp !
commit release !
file connect ( kern test sqlmode sapr3 !
alter table t modify varcol varchar (&1) ascii default
'abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz0123456789&2'!
commit release !
file connect ( kern test !
pars_then_ex !
data !
insert into t (k1,k2,a) values ('abcde','0', :a)! 
b 1 HI

insert into t (k1,k2,a) values ('abcde','a', timestamp)! 

insert into t (k1,k2,a) values ('abcde','b', :a)!
c 20 '19800324153647654321' 

select k2,a from t !
fetch into :a, :b !
pars_execute !
nodata !
rollback !
diagnose share parse drop !
drop table t !
commit work release !
