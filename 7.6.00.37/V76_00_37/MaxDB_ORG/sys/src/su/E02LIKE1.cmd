*   *ID* E02LIKE1 DBCMD    changed on 1992-09-28-13.23.36 by ADMIN     *
***********************************************************
**    checking modifications for like-strategy     ********
***********************************************************
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
declare erg cursor for select * from t1 where k like 'b22' for reuse!
declare erg cursor for select * from t1 where k like 'b2%' for reuse!
declare erg cursor for select * from t1 where k like 'b%2' for reuse!
declare erg cursor for select * from t1 where k like '' for reuse!
declare erg cursor for select * from t1 where k like '%' for reuse!
declare erg cursor for select * from t1 where k like 'b% ' for reuse!
declare erg cursor for select * from t1 where k like '%2 ' for reuse!
**
declare erg cursor for select * from t1 where i like 'b22' for reuse!
declare erg cursor for select * from t1 where i like 'b2%' for reuse!
declare erg cursor for select * from t1 where i like 'b%2' for reuse!
declare erg cursor for select * from t1 where i like '' for reuse!
declare erg cursor for select * from t1 where i like '%' for reuse!
declare erg cursor for select * from t1 where i like 'b% ' for reuse!
declare erg cursor for select * from t1 where i like '%2 ' for reuse!
**
explain declare erg cursor for select * from t1 where k like 'b22' for reuse!
explain declare erg cursor for select * from t1 where k like 'b2%' for reuse!
explain declare erg cursor for select * from t1 where k like 'b%2' for reuse!
explain declare erg cursor for select * from t1 where k like '%' for reuse!
explain declare erg cursor for select * from t1 where k like 'b% ' for reuse!
explain declare erg cursor for select * from t1 where k like '%2 ' for reuse!
**
explain declare erg cursor for select * from t1 where i like 'b22' for reuse!
explain declare erg cursor for select * from t1 where i like 'b2%' for reuse!
explain declare erg cursor for select * from t1 where i like 'b%2' for reuse!
explain declare erg cursor for select * from t1 where i like '%' for reuse!
explain declare erg cursor for select * from t1 where i like 'b% ' for reuse!
explain declare erg cursor for select * from t1 where i like '%2 ' for reuse!
**
*****************************************************
** joins / parse-execute                           **
*****************************************************
**
select * from t1,t2 where t1.k like 'b22'!
select * from t1,t2 where t1.k like 'b2%'!
select * from t1,t2 where t1.k like 'b%2'!
**
select * from t1,t2 where t1.i like 'b22'!
select * from t1,t2 where t1.i like 'b2%'!
select * from t1,t2 where t1.i like 'b%2'!
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
declare erg cursor for select * from t1 where k like :param for reuse!
c 3 'b22'

declare erg cursor for select * from t1 where k like :param for reuse!
c 3 'b2%'

declare erg cursor for select * from t1 where k like :param for reuse!
c 3 'b%2'

declare erg cursor for select * from t1 where k like :param for reuse!
c 3 'b% '

declare erg cursor for select * from t1 where k like :param for reuse!
c 3 '%2 '

declare erg cursor for select * from t1 where i like :param for reuse!
c 3 'b22'

declare erg cursor for select * from t1 where i like :param for reuse!
c 3 'b2%'

declare erg cursor for select * from t1 where i like :param for reuse!
c 3 'b%2'

declare erg cursor for select * from t1 where i like :param for reuse!
c 3 '%  '

declare erg cursor for select * from t1 where i like :param for reuse!
c 3 'b% '

declare erg cursor for select * from t1 where i like :param for reuse!
c 3 '%2 '

**
*****************************************************
** joins / parse-then-execute                      **
*****************************************************
**
select * from t1,t2 where t1.k like :param !
c 3 'b22'

select * from t1,t2 where t1.k like :param !
c 3 'b2%'

select * from t1,t2 where t1.k like :param !
c 3 'b%2'

select * from t1,t2 where t1.k like :param !
c 3 'b% '

select * from t1,t2 where t1.k like :param !
c 3 '%2 '

select * from t1,t2 where t1.i like :param !
c 3 'b22'

select * from t1,t2 where t1.i like :param !
c 3 'b2%'

select * from t1,t2 where t1.i like :param !
c 3 'b%2'

select * from t1,t2 where t1.i like :param !
c 3 'b% '

select * from t1,t2 where t1.i like :param !
c 3 '%2 '

nodata!
pars_execute!
drop table t1 !
drop table t2 !
commit work release!
