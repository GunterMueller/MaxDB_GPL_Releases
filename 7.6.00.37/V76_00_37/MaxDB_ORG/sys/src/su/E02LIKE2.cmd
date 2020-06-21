*   *ID* E02LIKE2 DBCMD    changed on 1992-09-28-13.23.37 by ADMIN     *
***********************************************************
**    checking modifications for like-strategy     ********
***********************************************************
file cleark !
file connect ( kern test !
create table t1 ( k char(3) key,i char(3))!
create index "t1.i"
	on t1( i ) !
insert into t1 set k = 'b11', i = 'b11'!
insert into t1 set k = 'b12', i = 'b12'!
insert into t1 set k = 'b13', i = 'b13'!
insert into t1 set k = 'b21', i = 'b21'!
insert into t1 set k = 'b22', i = 'b22'!
insert into t1 set k = 'b23', i = 'b23'!
insert into t1 set k = 'b31', i = 'b31'!
insert into t1 set k = 'b32', i = 'b32'!
insert into t1 set k = 'b33', i = 'b33'!
create table t2 like t1!
insert into t2 set k = 'xxx'!
commit work!
*****************************************************
** single selects / parse-execute                  **
*****************************************************
select * from t1 where k like 'b22' !
fetch!
select * from t1 where k like 'b2%' !
fetch!
select * from t1 where k like 'b%2' !
fetch!
select * from t1 where k like '' !
fetch!
select * from t1 where k like '%  ' !
fetch!
select * from t1 where k like 'b% ' !
fetch!
select * from t1 where k like '%2 ' !
fetch!
**
select * from t1 where i like 'b22' !
fetch!
select * from t1 where i like 'b2%' !
fetch!
select * from t1 where i like 'b%2' !
fetch!
select * from t1 where i like '' !
fetch!
select * from t1 where i like '%  ' !
fetch!
select * from t1 where i like 'b% ' !
fetch!
select * from t1 where i like '%2 ' !
fetch!
**
explain select * from t1 where k like 'b22' !
explain select * from t1 where k like 'b2%' !
explain select * from t1 where k like 'b%2' !
explain select * from t1 where k like '%  ' !
explain select * from t1 where k like 'b% ' !
explain select * from t1 where k like '%2 ' !
**
explain select * from t1 where i like 'b22' !
explain select * from t1 where i like 'b2%' !
explain select * from t1 where i like 'b%2' !
explain select * from t1 where i like '%  ' !
explain select * from t1 where i like 'b% ' !
explain select * from t1 where i like '%2 ' !
**
*****************************************************
** joins / parse-execute                           **
*****************************************************
**
select * from t1,t2 where t1.k like 'b22'!
fetch!
select * from t1,t2 where t1.k like 'b2%'!
fetch!
select * from t1,t2 where t1.k like 'b%2'!
fetch!
**
select * from t1,t2 where t1.i like 'b22'!
fetch!
select * from t1,t2 where t1.i like 'b2%'!
fetch!
select * from t1,t2 where t1.i like 'b%2'!
fetch!
**
explain select * from t1,t2 where t1.k like 'b22' !
explain select * from t1,t2 where t1.k like 'b2%' !
explain select * from t1,t2 where t1.k like 'b%2' !
**
explain select * from t1,t2 where t1.i like 'b22' !
explain select * from t1,t2 where t1.i like 'b2%' !
explain select * from t1,t2 where t1.i like 'b%2' !
**
*****************************************************
** single-selects  / parse-then-execute            **
*****************************************************
**
pars_then_exec!
data!
select * from t1 where k like :param !
c 3 'b22'

nodata!
fetch into :a, :b !
data!
select * from t1 where k like :param !
c 3 'b2%'

nodata!
fetch into :a, :b !
data!
select * from t1 where k like :param !
c 3 'b%2'

nodata!
fetch into :a, :b !
data!
select * from t1 where k like :param !
c 3 'b% '

nodata!
fetch into :a, :b !
data!
select * from t1 where k like :param !
c 3 '%2 '

nodata!
fetch into :a, :b !
data!
select * from t1 where i like :param !
c 3 'b22'

nodata!
fetch into :a, :b !
data!
select * from t1 where i like :param !
c 3 'b2%'

nodata!
fetch into :a, :b !
data!
select * from t1 where i like :param !
c 3 'b%2'

nodata!
fetch into :a, :b !
data!
select * from t1 where i like :param !
c 3 'b% '

nodata!
fetch into :a, :b !
data!
select * from t1 where i like :param !
c 3 '%2 '

nodata!
fetch into :a, :b !
data!
**
*****************************************************
** joins / parse-then-execute                      **
*****************************************************
**
select * from t1,t2 where t1.k like :param !
c 3 'b22'

nodata!
fetch into :a, :b, :c, :d !
data!
select * from t1,t2 where t1.k like :param !
c 3 'b2%'

nodata!
fetch into :a, :b, :c, :d !
data!
select * from t1,t2 where t1.k like :param !
c 3 'b%2'

nodata!
fetch into :a, :b, :c, :d !
data!
select * from t1,t2 where t1.k like :param !
c 3 'b% '

nodata!
fetch into :a, :b, :c, :d !
data!
select * from t1,t2 where t1.k like :param !
c 3 '%2 '

nodata!
fetch into :a, :b, :c, :d !
data!
select * from t1,t2 where t1.i like :param !
c 3 'b22'

nodata!
fetch into :a, :b, :c, :d !
data!
select * from t1,t2 where t1.i like :param !
c 3 'b2%'

nodata!
fetch into :a, :b, :c, :d !
data!
select * from t1,t2 where t1.i like :param !
c 3 'b%2'

nodata!
fetch into :a, :b, :c, :d !
data!
select * from t1,t2 where t1.i like :param !
c 3 'b% '

nodata!
fetch into :a, :b, :c, :d !
data!
select * from t1,t2 where t1.i like :param !
c 3 '%2 '

nodata!
fetch into :a, :b, :c, :d !
pars_execute!
drop table t1 !
drop table t2 !
commit work release!
