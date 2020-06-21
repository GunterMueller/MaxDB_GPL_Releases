file connect ( kern test sqlmode oracle !
create table t ( a char (8) primary key, b char (20), c char (12))!
insert into t values ('aaa', 'bbbaaa', 'cccaaa')!
insert into t values ('bbb', 'bbbbbb', 'cccbbb')!
insert into t values ('ccc', 'bbbccc', 'cccccc')!
insert into t values ('ddd', 'bbbddd', 'cccddd')!
insert into t values ('eee', 'bbbeee', 'ccceee')!
commit work release!
file connect ( kern test !
pars_then_ex !
data!
select b||'xx' into :b from t where a = :a !
c 8 'aaa'

nodata !
pars_execute!
commit work release!
file connect ( kern test sqlmode oracle !
pars_then_ex !
data !
mselect * into :a, :b, :c from t where a = :a!
32767
3
c 8 'ccc'
c 8 'bbb'
c 8 'eee'

mselect * into :a, :b, :c from t where a = :a!
32767
3
c 8 'ccc'
c 8 'iii'
c 8 'eee'

mselect * into :a, :b, :c from t where a = :a!
32767
3
c 8 'ccc'
c 8 'eee'
c 8 'iii'

mselect * into :a, :b, :c from t where a = :a!
32767
2
c 8 'eee'
c 8 'iii'

mselect * into :a, :b, :c from t where a = :a!
32767
2
c 8 'iii'
c 8 'eee'

mselect * into :a, :b, :c from t where a = :a!
32767
2
c 8 'iii'
c 8 'qqq'

mselect t.*, t1.a d into :a, :b, :c, :d
   from t, t t1 where t.a = :a and t.a = t1.a !
32767
4
c 8 'ccc'
c 8 'fff'
c 8 'eee'
c 8 'mmm'

mselect t.*, t1.a d into :a, :b, :c, :d
   from t, t t1 where t.a = :a and t.a = t1.a !
32767
2
c 8 'fff'
c 8 'mmm'

mselect t.a, t.b into :a, :b
   from t where t.a = :a and exists (select * from t t1 where t.a=t1.a)!
32767
2
c 8 'fff'
c 8 'mmm'

mselect t.a, t.b into :a, :b
   from t where t.a = :a and exists (select * from t t1 where t.a=t1.a)!
32767
4
c 8 'ccc'
c 8 'fff'
c 8 'eee'
c 8 'mmm'

nodata !
pars_execute !
drop table t !
commit work release !
