*   *ID* DMLINTO  DBCMD    changed on 1992-09-28-13.23.21 by ADMIN     *
file connect ( kern test !
create table t ( a char (8) key )!
insert t set a = 'aaa'!
create table t1 (a char (8) key, b char (8) key, c char (8))!
insert t1 set a = 'aaa', b = 'bbb', c = 'ccc' !
create view v1 as select t.a x, t1.* from t, t1 where t.a = t1.a!
select * into :a from t where a = 'aaa'!
file sel_cols_tab ( T1 !
pars_then_ex !
select distinct * into :a from t where a = 'aaa'!
select max(a) into :a from t where a = 'aaa'!
select 'Max''s', * into :m, :a from t where a = 'aaa'!
select t.a, t1.a b into :a, :b from t, t t1 where t1.a = 'aaa'!
select * into :a from t where a <> 'bbb'!
select * into :a from t where a = 'aaa' or a = 'bbb' !
select * into :a from t where a = 'aaa'!
select * into :a, :b, :c from t1 where a = 'aaa' and a = 'vvv' !
select * into :a, :b, :c from t1 where a = 'aaa' !
select * into :a, :b, :c from t1 where c = 'aaa' !
select * into :a, :b, :c from v1 where a = 'aaa' !
select * into :a, :b, :c from t1 where a = 'aaa' and b = 'bbb' !
pars_execute !
drop table t !
drop table t1 !
commit release !
