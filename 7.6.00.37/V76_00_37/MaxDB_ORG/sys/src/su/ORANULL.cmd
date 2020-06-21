file connect ( kern test sqlmode oracle !
create table t ( a char (8), b date )!
insert into t values ('aaa', null)!
select null, a, b into :a, :b, :c from t !
select * into :a, :b from t where b = null !
update t set b = timestamp(null)!
select * into :a, :b from t where null = timestamp(null)!
select * into :a, :b from t where timestamp(null) is null!
select * into :a, :b from t !
update t set a = null !
select * into :a, :b from t where a between 'x' and null !
select * into :a, :b from t where a in ('a', null, 'b') !
drop table t !
create table t (a char (8) primary key, b char (8))!
insert into t values ('', 'aaa')!
insert into t values ('aaa', '')!
update t set a = ''!
select * into :a, :b from t where b = ''!
select * into :a, :b from t where b is null !
select a || b || a into :a from t !
select lpad (a,8,'s') into :a from t !
select lpad (a,8,'') into :a from t !
select lpad (a,8,null) into :a from t !
select to_number(b) into :a from t !
drop table t !
commit work release !
file connect ( kern test !
create table t (a char (8) primary key, b char (8))!
insert into t values ('', 'aaa')!
delete t where a = '' !
insert into t values ('aaa', '')!
update t set a = ''!
select * into :a, :b from t where b = ''!
select * into :a, :b from t where b is null !
select a || b || a into :a from t !
select lpad (a,3,'s') into :a from t !
select lpad (a,3,'') into :a from t !
select lpad (a,3,null) into :a from t !
select num(b) into :a from t !
drop table t !
commit work release !
