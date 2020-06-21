*   *ID* E02LIKE4 DBCMD    changed on 1992-09-28-13.23.38 by ADMIN     *
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
select * from t1 where k like 'b2_' !
fetch!
select * from t1 where k like 'b_2' !
fetch!
select * from t1 where k like '' !
fetch!
select * from t1 where k like '___' !
fetch!
**
select * from t1 where i like 'b22' !
fetch!
select * from t1 where i like 'b2_' !
fetch!
select * from t1 where i like 'b_2' !
fetch!
select * from t1 where i like '' !
fetch!
select * from t1 where i like '___' !
fetch!
**
explain select * from t1 where k like 'b22' !
explain select * from t1 where k like 'b2_' !
explain select * from t1 where k like 'b_2' !
**
explain select * from t1 where i like 'b22' !
explain select * from t1 where i like 'b2_' !
explain select * from t1 where i like 'b_2' !
**
*****************************************************
** joins / parse-execute                           **
*****************************************************
**
select * from t1,t2 where t1.k like 'b22'!
fetch!
select * from t1,t2 where t1.k like 'b2_'!
fetch!
select * from t1,t2 where t1.k like 'b_2'!
fetch!
**
select * from t1,t2 where t1.i like 'b22'!
fetch!
select * from t1,t2 where t1.i like 'b2_'!
fetch!
select * from t1,t2 where t1.i like 'b_2'!
fetch!
**
explain select * from t1,t2 where t1.k like 'b22' !
explain select * from t1,t2 where t1.k like 'b2_' !
explain select * from t1,t2 where t1.k like 'b_2' !
**
explain select * from t1,t2 where t1.i like 'b22' !
explain select * from t1,t2 where t1.i like 'b2_' !
explain select * from t1,t2 where t1.i like 'b_2' !
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
c 3 'b2_'

nodata!
fetch into :a, :b !
data!
select * from t1 where k like :param !
c 3 'b_2'

nodata!
fetch into :a, :b !
data!
select * from t1 where k like :param !
c 3 '___'

nodata!
fetch into :a, :b !
data!
select * from t1 where i like :param !
c 3 'b22'

nodata!
fetch into :a, :b !
data!
select * from t1 where i like :param !
c 3 'b2_'

nodata!
fetch into :a, :b !
data!
select * from t1 where i like :param !
c 3 'b_2'

nodata!
fetch into :a, :b !
data!
select * from t1 where i like :param !
c 3 '___'

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
fetch into :a, :b !
data!
select * from t1,t2 where t1.k like :param !
c 3 'b2_'

nodata!
fetch into :a, :b !
data!
select * from t1,t2 where t1.k like :param !
c 3 'b_2'

nodata!
fetch into :a, :b !
data!
select * from t1,t2 where t1.i like :param !
c 3 'b22'

nodata!
fetch into :a, :b !
data!
select * from t1,t2 where t1.i like :param !
c 3 'b2_'

nodata!
fetch into :a, :b !
data!
select * from t1,t2 where t1.i like :param !
c 3 'b_2'

nodata!
fetch into :a, :b !
pars_execute !
drop table t1 !
drop table t2 !
commit work release!
* PTS 3443!
file connect ( kern test !
create table t ( a char (8), b char (7)primary key)!
insert t set a = 'aaa', b  = 'xxx'!
insert t set a = 'bbb', b  = 'www'!
insert t set a = 'ccc', b  = 'b1'!
insert t set a = 'ddd', b  = 'x'!
create table t1 ( b char (8)key)!
insert t1 set b = 'x'!
pars_then_ex !
data !
select * from t1, t
where 
t.b like :a and
t.a = t1.b !
c 254 'x%'

nodata !
fetch into :a, :b, :c !
data !
select * from t1, t
where 
t.b like :a and
t.b = t1.b !
c 254 'x%'
 
nodata !
fetch into :a, :b, :c !
data !
select * from t1, t
where 
t.b in (:a,:b,:c,:d, :a) and
t.b = t1.b !
c 7 'aaaaaaa'
c 7 'eeeeeee'
c 7 'aaaaaaa'
c 7 'ddddddd'
c 7 'aaaaaaa'
 
nodata !
fetch into :a, :b, :c !
pars_execute!
nodata !
drop table t !
drop table t1 !
commit release !
