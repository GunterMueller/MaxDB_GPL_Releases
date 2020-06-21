senderid DBM !
file connect ( sut sut ! 
set parameter OPTIMIZE_OPERATOR_JOIN = '&1' !
commit release !
senderid XCI !

*****************************************************************************
* E02LIKE.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
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
select * from t1,t2 where t1.k like 'b22'!
select * from t1,t2 where t1.k like 'b2%'!
select * from t1,t2 where t1.k like 'b%2'!
select * from t1,t2 where t1.i like 'b22'!
select * from t1,t2 where t1.i like 'b2%'!
select * from t1,t2 where t1.i like 'b%2'!
explain select * from t1,t2 where t1.k like 'b22' !
explain select * from t1,t2 where t1.k like 'b2%' !
explain select * from t1,t2 where t1.k like 'b%2' !
explain select * from t1,t2 where t1.i like 'b22' !
explain select * from t1,t2 where t1.i like 'b2%' !
explain select * from t1,t2 where t1.i like 'b%2' !

pars_then_ex!
data!
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

file cleark !
file connect ( kern test !
create table t1 ( k char(3) key,i char(3))!
create index "t1.i" on t1( i ) !
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
pars_then_ex!
data!
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

file cleark !
file connect ( kern test !
create table t1 ( k char(3) key,i char(3))!
create index "t1.i" on t1( i ) !
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
select * from t1,t2 where t1.k like 'b22'!
select * from t1,t2 where t1.k like 'b2_'!
select * from t1,t2 where t1.k like 'b_2'!
**
select * from t1,t2 where t1.i like 'b22'!
select * from t1,t2 where t1.i like 'b2_'!
select * from t1,t2 where t1.i like 'b_2'!
**
explain select * from t1,t2 where t1.k like 'b22' !
explain select * from t1,t2 where t1.k like 'b2_' !
explain select * from t1,t2 where t1.k like 'b_2' !
**
explain select * from t1,t2 where t1.i like 'b22' !
explain select * from t1,t2 where t1.i like 'b2_' !
explain select * from t1,t2 where t1.i like 'b_2' !
pars_then_ex!
data!
select * from t1,t2 where t1.k like :param !
c 3 'b22'

select * from t1,t2 where t1.k like :param !
c 3 'b2_'

select * from t1,t2 where t1.k like :param !
c 3 'b_2'

select * from t1,t2 where t1.i like :param !
c 3 'b22'

select * from t1,t2 where t1.i like :param !
c 3 'b2_'

select * from t1,t2 where t1.i like :param !
c 3 'b_2'

nodata!
pars_execute!
drop table t1 !
drop table t2 !
commit work release!

file cleark !
file connect ( kern test !
create table t1 ( k char(3) key,i char(3))!
create index "t1.i" on t1( i ) !
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
pars_then_ex!
data!
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
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* E03MI1.vdnts
*   *ID* E03MI1   VDNTEST  changed on 1992-09-28-13.23.40 by ADMIN     *
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table t (k char(20) key, mi1 char(20),
                mi2 char(20), mi3 char(20)) !
insert into t set k = 'x1',mi1 = 'a', mi2 = 'a ', mi3 = '1'!
insert into t set k = 'x2',mi1 = 'a', mi2 = 'b ', mi3 = '2'!
insert into t set k = 'x3',mi1 = 'a', mi2 = 'c ', mi3 = '3'!
insert into t set k = 'y1',mi1 = 'b', mi2 = 'a ', mi3 = '4'!
insert into t set k = 'y2',mi1 = 'b', mi2 = 'b ', mi3 = '5'!
insert into t set k = 'y3',mi1 = 'b', mi2 = 'c ', mi3 = '6'!
insert into t set k = 'z1',mi1 = 'c', mi2 = 'a ', mi3 = '7'!
insert into t set k = 'z2',mi1 = 'c', mi2 = 'b ', mi3 = '8'!
insert into t set k = 'z3',mi1 = 'c', mi2 = 'c ', mi3 = '9'!
alter table t add (mi4 char(20))!
update t set mi4 = mi2 !
alter table t drop (mi2) !
rename column t.mi4 to mi2!
alter table t add (mi99 char(20))!
alter table t add (mi4 char(20))!
update t set mi4 = mi1 !
alter table t drop (mi1) !
rename column t.mi4 to mi1!
create index mia on t (mi1, mi2) !
create table t1 (dummy char(20)) !
insert into t1 set dummy = 'dummy' !
explain select k,mi1,mi2,mi3 from t,t1 where mi1 = 'b' and mi2 = 'b'!
explain select k,mi1,mi2,mi3 from t,t1 where mi1 = 'b'!
explain select k,mi1,mi2,mi3 from t,t1 where mi2 = 'b'!
explain select k,mi1,mi2,mi3 from t,t1 where mi1 > 'a'!
explain select k,mi1,mi2,mi3 from t,t1 where mi1 > 'b' and mi2 > 'b'!
explain select k,mi1,mi2,mi3 from t,t1 where mi1 < 'b' and mi2 < 'b'!
explain select k,mi1,mi2,mi3 from t,t1 where mi1 between 'b' and 'b'!
explain select k,mi1,mi2,mi3 from t,t1 where mi1 = 'b' and
    mi2 between 'b' and 'b'!
explain select k,mi1,mi2,mi3 from t,t1 where mi1 like 'b%'
                     and mi2 between 'b' and 'c'!
select k,mi1,mi2,mi3 from t,t1 where mi1 = 'b' and mi2 = 'b'
fetch !
select k,mi1,mi2,mi3 from t,t1 where mi1 = 'b'!
fetch !
select k,mi1,mi2,mi3 from t,t1 where mi2 = 'b'!
fetch !
select k,mi1,mi2,mi3 from t,t1 where mi1 > 'a'!
fetch !
select k,mi1,mi2,mi3 from t,t1 where mi1 > 'b' and mi2 > 'b'!
fetch !
select k,mi1,mi2,mi3 from t,t1 where mi1 < 'b' and mi2 < 'b'!
fetch !
select k,mi1,mi2,mi3 from t,t1 where mi1 between 'b' and 'b'!
fetch !
select k,mi1,mi2,mi3 from t,t1 where mi1 = 'b' and
    mi2 between 'b' and 'b'!
fetch !
select k,mi1,mi2,mi3 from t,t1 where mi1 like 'b%'
                     and mi2 between 'b' and 'c'!
fetch !
rollback release !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* E03MI2.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table t (k char(20) key, mi1 char(20),
                mi2 char(20), mi3 char(20)) !
insert into t set k = 'x1',mi1 = 'a', mi2 = 'a ', mi3 = '1'!
insert into t set k = 'x2',mi1 = 'a', mi2 = 'b ', mi3 = '2'!
insert into t set k = 'x3',mi1 = 'a', mi2 = 'c ', mi3 = '3'!
insert into t set k = 'y1',mi1 = 'b', mi2 = 'a ', mi3 = '4'!
insert into t set k = 'y2',mi1 = 'b', mi2 = 'b ', mi3 = '5'!
insert into t set k = 'y3',mi1 = 'b', mi2 = 'c ', mi3 = '6'!
insert into t set k = 'z1',mi1 = 'c', mi2 = 'a ', mi3 = '7'!
insert into t set k = 'z2',mi1 = 'c', mi2 = 'b ', mi3 = '8'!
insert into t set k = 'z3',mi1 = 'c', mi2 = 'c ', mi3 = '9'!
alter table t add (mi4 char(20))!
update t set mi4 = mi2 !
alter table t drop (mi2) !
rename column t.mi4 to mi2!
alter table t add (mi99 char(20))!
alter table t add (mi4 char(20))!
update t set mi4 = mi1 !
alter table t drop (mi1) !
rename column t.mi4 to mi1!
create index mia on t (mi1, mi2) !
create table t1 (dummy char(20)) !
insert into t1 set dummy = 'dummy' !
diagnose share parse drop !
commit release!
file connect ( kern test !
data!
pars_then_ex!
select k,mi1,mi2,mi3 from t,t1 where mi1 = :p1 and mi2 = :p2!
c 20 'b'
c 20 'b'

nodata!
fetch into :a,:b,:c,:d,:e !
data!
select k,mi1,mi2,mi3 from t,t1 where mi1 = :p!
c 20 'b'

nodata!
fetch into :a,:b,:c,:d,:e !
data!
select k,mi1,mi2,mi3 from t,t1 where mi2 = :p!
c 20 'b'

nodata!
fetch into :a,:b,:c,:d,:e !
data!
select k,mi1,mi2,mi3 from t,t1 where mi1 > :p!
c 20 'a'

nodata!
fetch into :a,:b,:c,:d,:e !
data!
select k,mi1,mi2,mi3 from t,t1 where mi1 > :p1 and mi2 > :p2!
c 20 'b'
c 20 'b'

nodata!
fetch into :a,:b,:c,:d,:e !
data!
select k,mi1,mi2,mi3 from t,t1 where mi1 < :p1 and mi2 < :p2!
c 20 'b'
c 20 'b'

nodata!
fetch into :a,:b,:c,:d,:e !
data!
select k,mi1,mi2,mi3 from t,t1 where mi1 between :p1 and :p2!
c 20 'b'
c 20 'b'

nodata!
fetch into :a,:b,:c,:d,:e !
data!
select k,mi1,mi2,mi3 from t,t1 where mi1 like :p1
                     and mi2 between :p2 and :p3!
c 20 'b%'
c 20 'b'
c 20 'c'

nodata!
fetch into :a,:b,:c,:d,:e !
pars_execute!
commit release!
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* E03MI3.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table t (k char(20) key, mi1 char(20),
                mi2 char(20), mi3 char(20)) !
create index mia on t (mi1, mi2)!
insert into t set k = 'x1',mi1 = 'a', mi2 = 'a ', mi3 = '1'!
insert into t set k = 'x2',mi1 = 'a', mi2 = 'b ', mi3 = '2'!
insert into t set k = 'x3',mi1 = 'a', mi2 = 'c ', mi3 = '3'!
insert into t set k = 'y1',mi1 = 'b', mi2 = 'a ', mi3 = '4'!
insert into t set k = 'y2',mi1 = 'b', mi2 = 'b ', mi3 = '5'!
insert into t set k = 'y3',mi1 = 'b', mi2 = 'c ', mi3 = '6'!
insert into t set k = 'z1',mi1 = 'c', mi2 = 'a ', mi3 = '7'!
insert into t set k = 'z2',mi1 = 'c', mi2 = 'b ', mi3 = '8'!
insert into t set k = 'z3',mi1 = 'c', mi2 = 'c ', mi3 = '9'!
alter table t add (mi4 char(20))!
update t set mi4 = mi2 !
alter table t drop (mi2) !
rename column t.mi4 to mi2!
alter table t add (mi99 char(20))!
alter table t add (mi4 char(20))!
update t set mi4 = mi1 !
alter table t drop (mi1) !
rename column t.mi4 to mi1!
create index mia on t(mi1 desc, k asc, mi2 desc) !
create table t1 (dummy char(20)) !
insert into t1 set dummy = 'dummy' !
commit work!
explain select k,mi1,mi2,mi3 from t1,t where mi1 = 'b' and k like '%2' !
select k,mi1,mi2,mi3 from t1,t where mi1 = 'b' and k like '%2' !
fetch into :a,:b,:c,:d!
explain select k,mi1,mi2,mi3 from t1,t where mi1 < 'b' and mi2 = 'a'!
select k,mi1,mi2,mi3 from t1,t where mi1 < 'b' and mi2 = 'a'!
fetch into :a,:b,:c,:d!
drop index mia!
create index mia on t(mi2 asc, mi1 desc) !
parsing!
select k,mi1,mi2,mi3 from t1,t where mi2 > 'a' and  mi2 like :a !
data!
executing!
****************
select!
1
c 20 'b%'

nodata!
pars_then_exec!
fetch into :a,:b,:c,:d!
****************
data!
executing!
select!
1
c 20 '%b'

nodata!
pars_then_exec!
fetch into :a,:b,:c,:d!
****************
data!
executing!
select!
1
c 20 'b%'

nodata!
pars_then_exec!
fetch into :a,:b,:c,:d!
****************
data!
executing!
select!
1
c 20 '%'

nodata!
pars_then_exec!
fetch into :a,:b,:c,:d!
 ******************************************************* !
data!
select k,mi1,mi2,mi3 from t1,t where mi2 > :p1 and mi1 < :p2 !
c 20 'a'
c 20 'c'

nodata!
fetch into :a,:b,:c,:d!
data!
select k,mi1,mi2,mi3 from t1,t where mi2 < :p1 and mi1 > :p2 !
c 20 'c'
c 20 'a'

nodata!
fetch into :a,:b,:c,:d!
 ******************************************************* !
pars_execute!
create index mib on t (mi1 desc, mi3 asc) !
 ******************************************************* !
pars_then_exec!
data!
select k,mi1,mi2,mi3 from t1,t where mi1 = :p1 and mi2 = :p2
                                   and mi3 = :p3 !
c 20 'a'
c 20 'b'
c 20 '2'

nodata!
fetch into :a,:b,:c,:d!
data!
select k,mi1,mi2,mi3 from t1,t where mi1 = :p1 and mi2 like :p2
                                     and mi3 = :p3 !
c 20 'a'
c 20 'b%'
c 20 '2'

pars_execute!
nodata!
rollback release !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* EXPL.vdnts
file connect ( sut sut !
CREATE TABLE T (A CHAR (8) KEY, B CHAR (8), C CHAR (8), D CHAR (8),
           E CHAR (8))!
INSERT INTO T SET A = 'aaaaaaaa' !
CREATE INDEX "T.B" on T( B ) !
CREATE INDEX I ON T (B,D)!
CREATE INDEX "T.C" on T( C ) !
CREATE TABLE T1 (A1 CHAR (8)) !
INSERT INTO T1 SET A1 = 'a1a1a1a1' !
EXPLAIN SELECT * FROM T, T1 !
EXPLAIN SELECT * FROM <T1, T> WHERE T1.A1 = T.C!
EXPLAIN SELECT * FROM <T1, T> WHERE T1.A1 = T.A!
EXPLAIN SELECT * FROM <T1, T> WHERE T1.A1 = T.B!
EXPLAIN SELECT /*+ordered*/ * INTO :A1, :A2, :A3, :A4, :A5, :A6
                 FROM T1, T WHERE T1.A1 = T.B!
EXPLAIN SELECT * FROM <T1, T, T X> WHERE T1.A1 = T.B AND T1.A1 = X.A!
EXPLAIN SELECT * FROM <T1, T, T X> WHERE T1.A1 = T.B AND T1.A1 = X.A AND
                                               T.B = X.A !
CREATE VIEW V AS SELECT * FROM T WHERE B = 'SSS'!
EXPLAIN SELECT * FROM <T1, V, T X> WHERE T1.A1 = V.B AND T1.A1 = X.A!
CREATE VIEW JOINVIEW AS
     SELECT V.*, T1.A1, X.A XA 
       FROM V, T1, T X WHERE T1.A1 = V.B AND T1.A1 = X.A!
EXPLAIN SELECT * FROM JOINVIEW !
EXPLAIN JOIN SELECT * FROM JOINVIEW !
EXPLAIN SEQUENCE SELECT * FROM JOINVIEW !
EXPLAIN SELECT * FROM JOINVIEW !
drop table t!
drop table t1!
commit work release !

*****************************************************************************
* EXPRLIST.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file CONNECT ( kern test SQLMODE ORACLE!

CREATE TABLE artikel
(
  nummer    NUMBER(6) PRIMARY KEY,
  name      CHAR(20)  DEFAULT 'Kein Name' CHECK (name <> 'Leerteil'),
  zeichnung CHAR(20)  DEFAULT 'Ohne Zeichnung'
)!
ALTER TABLE artikel ADD wert NUMBER (5)!
CREATE INDEX zeichnung ON artikel (zeichnung, nummer)!

CREATE TABLE st_liste
(
  bauk     NUMBER(6),
  position INTEGER,
  stueck   NUMBER(6) CONSTRAINT st_n_null NOT NULL,
  CONSTRAINT st_n_bauk CHECK (stueck <> bauk),
  CHECK (position < 999),
  PRIMARY KEY (bauk, position),
  FOREIGN KEY (bauk)   REFERENCES artikel (nummer),
  FOREIGN KEY (stueck) REFERENCES artikel (nummer)
)!
ALTER TABLE st_liste ADD anzahl NUMBER (5)!

CREATE UNIQUE INDEX verwendung ON st_liste (stueck, bauk, position)!

CREATE VIEW bauk (bauknr, baukname,
                       posnr, posinr, posiname)
  AS SELECT kopf.nummer, kopf.name,
          position, posi.nummer, posi.name
       FROM artikel kopf, st_liste, artikel posi
       WHERE st_liste.bauk   = kopf.nummer
         AND st_liste.stueck = posi.nummer
  WITH CHECK OPTION!

*COMMENT ON TABLE  artikel                   IS 'Bauteile'!
*COMMENT ON COLUMN artikel.nummer            IS 'Eindeutige Artikelnummer'!

*GRANT UPDATE (nummer, name) ON artikel     TO PUBLIC!
*GRANT UPDATE (nummer, name) ON artikel     TO PUBLIC!
*GRANT SELECT                ON st_liste    TO PUBLIC!


COMMIT WORK RELEASE!

file CONNECT ( kern test SQLMODE ORACLE!
INSERT INTO artikel (nummer, name) VALUES (1, 'Kopfartikel')!
INSERT INTO artikel (nummer, name) VALUES (2, 'Posartikel 1')!
INSERT INTO artikel (nummer, name) VALUES (3, 'Posartikel 2')!
INSERT INTO artikel (nummer)       VALUES (4)!
UPDATE artikel SET wert = 10 WHERE nummer=2!
UPDATE artikel SET wert = 20 WHERE nummer=3!
INSERT INTO st_liste (bauk, position, stueck) VALUES (5, 1, 1)!
INSERT INTO st_liste (bauk, position, stueck) VALUES (1, 1, 1)!
INSERT INTO st_liste (bauk, position, stueck) VALUES (1, 1, 2)!
INSERT INTO st_liste (bauk, position, stueck) VALUES (1, 1, 3)!
INSERT INTO st_liste (bauk, position, stueck) VALUES (1, 2, 3)!
UPDATE st_liste SET anzahl=1 WHERE bauk=1 AND position=1!
UPDATE st_liste SET anzahl=2 WHERE bauk=1 AND position=2!
INSERT INTO artikel (nummer, name) VALUES (10, 'Kopf-Test1')!
INSERT INTO artikel (nummer, name) VALUES (11, 'Pos-Test1')!
INSERT INTO artikel (nummer, name) VALUES (12, 'Pos-Test2')!
INSERT INTO st_liste (bauk, position, stueck) VALUES (10, 1, 11)!
INSERT INTO st_liste (bauk, position, stueck) VALUES (10, 2, 12)!

UPDATE st_liste SET anzahl=2 WHERE bauk=10 AND position=1!
COMMIT WORK!
DELETE FROM artikel     WHERE nummer = 2!
DELETE FROM st_liste WHERE stueck = 2!
DELETE FROM artikel     WHERE nummer = 2!
ROLLBACK WORK RELEASE!

CONNECT kern IDENTIFIED BY test SQLMODE ORACLE!
SELECT * FROM artikel, st_liste
  WHERE stueck = nummer
    AND (nummer, 0, 'Kein NameKein Name')
          IN ((3, DECODE (stueck, 2, bauk+1, bauk-1),
               'Kein Name' || SUBSTR ('Kein Name mehr frei', 1, 9)),
              (4, SYSDATE-SYSDATE,       name||name))
    AND position > 1!
FETCH INTO :a, :b, :c, :d, e, :f, :g, :h!

SELECT * FROM artikel, st_liste
  WHERE stueck = nummer
    AND (nummer, 0, 'Kein NameKein Name')
          NOT IN ((3, DECODE (stueck, 2, bauk+1, bauk-1),
                   'Kein Name' || SUBSTR ('Kein Name mehr frei', 1, 9)),
                  (4, SYSDATE-SYSDATE,       name||name))
    AND position > 1!
FETCH INTO :a, :b, :c, :d, e, :f, :g, :h!

CREATE VIEW tv AS
SELECT * FROM artikel, st_liste
  WHERE stueck = nummer
    AND (nummer, 0, 'Kein NameKein Name')
          IN ((3, DECODE (stueck, 2, bauk+1, bauk-1),
               'Kein Name' || SUBSTR ('Kein Name mehr frei', 1, 9)),
              (4, SYSDATE-SYSDATE,       name||name))
    AND position > 1!

SELECT * FROM tv!
FETCH INTO :a, :b, :c, :d, e, :f, :g, :h!
DROP VIEW tv!

CREATE TABLE tt AS
SELECT * FROM artikel, st_liste
  WHERE stueck = nummer
    AND (nummer, 0, 'Kein NameKein Name')
          NOT IN ((3, DECODE (stueck, 2, bauk+1, bauk-1),
                   'Kein Name' || SUBSTR ('Kein Name mehr frei', 1, 9)),
                  (4, SYSDATE-SYSDATE,       name||name))
    AND position > 1!

SELECT * FROM tt!
FETCH INTO :a, :b, :c, :d, e, :f, :g, :h!
DROP TABLE tt!

SELECT position FROM artikel, st_liste
WHERE nummer IN (SELECT bauk FROM st_liste)
  AND bauk=nummer!
FETCH INTO :A!

SELECT nummer, name, zeichnung, position
  FROM artikel, st_liste
  WHERE (nummer, position)
           IN (SELECT bauk, MAX (position) FROM st_liste GROUP BY bauk)
    AND bauk = nummer!
FETCH INTO :a, :b, :c, :d!

SELECT name, position FROM artikel, st_liste
  WHERE (nummer, position)
           IN (SELECT bauk, 2 FROM st_liste WHERE bauk = nummer)!
FETCH INTO :a, :b!

SELECT nummer, name, zeichnung, position FROM artikel, st_liste
  WHERE bauk = nummer
    AND (bauk, position)
           IN (SELECT nummer, 2 FROM artikel WHERE bauk = nummer)!
FETCH INTO :a, :b, :c, :d!

UPDATE artikel SET wert = 24 WHERE nummer = 4!

UPDATE artikel SET wert = 27
  WHERE (4, 'Kein Name', zeichnung)
    IN (SELECT nummer, name, zeichnung FROM artikel)!

COMMIT WORK RELEASE!

file CONNECT ( kern test SQLMODE ORACLE!

* Here come some more tables for the articles: orders and stores!

UPDATE artikel SET wert = nummer+20 WHERE wert IS NULL!

CREATE TABLE auftrag
(
  auftnum   NUMBER (6) PRIMARY KEY,
  kunde     CHAR (20),
  termin    DATE
)!
INSERT INTO auftrag VALUES (940001, 'Mueller', '16-MAY-94')!
INSERT INTO auftrag VALUES (940002, 'Mayer',   '16-JUN-94')!
INSERT INTO auftrag VALUES (940003, 'Schulze', '27-MAY-94')!

CREATE TABLE auftpos
(
  auftnum   NUMBER(6),
  auftpos   NUMBER(4),
  artikel   NUMBER(6) NOT NULL,
  menge     NUMBER(5) DEFAULT 1,
  PRIMARY KEY (auftnum, auftpos),
  FOREIGN KEY (auftnum) REFERENCES auftrag (auftnum),
  FOREIGN KEY (artikel) REFERENCES artikel (nummer)
)!
* Order 940001 by Mueller!
INSERT INTO auftpos VALUES (940001, 0001, 000001, 10)!
INSERT INTO auftpos VALUES (940001, 0002, 000002, 11)!
INSERT INTO auftpos VALUES (940001, 0003, 000003, 12)!
INSERT INTO auftpos VALUES (940001, 0004, 000004, 13)!
* Order 940002 by Mayer!
INSERT INTO auftpos VALUES (940002, 0001, 000010, 1)!
INSERT INTO auftpos VALUES (940002, 0002, 000011, 2)!
INSERT INTO auftpos VALUES (940002, 0003, 000012, 3)!
* Order 940003 by Schulze!
INSERT INTO auftpos VALUES (940003, 0001, 000001, 20)!

CREATE TABLE lagerfach
(
  lager   NUMBER(4),
  auftnum NUMBER(6),
  auftpos NUMBER(4),
  artikel NUMBER(6) NOT NULL,
  menge   NUMBER(5) NOT NULL,
  PRIMARY KEY (lager, artikel),
  FOREIGN KEY (auftnum, auftpos) REFERENCES auftpos (auftnum, auftpos),
  FOREIGN KEY (artikel)          REFERENCES artikel (nummer)
)!
* Auftragsneutrale Lagerrung!
INSERT INTO lagerfach VALUES (0005, NULL,   NULL, 000010,  15)!
INSERT INTO lagerfach VALUES (0006, NULL,   NULL, 000011, 200)!
INSERT INTO lagerfach VALUES (0007, NULL,   NULL, 000012,  11)!
* Auftragsbezogene Lagerung!
INSERT INTO lagerfach VALUES (0011, 940001, 0001, 000001,  10)!
INSERT INTO lagerfach VALUES (0021, 940003, 0001, 000001,  20)!
INSERT INTO lagerfach VALUES (0002, 940001, 0002, 000002,   5)!
INSERT INTO lagerfach VALUES (0003, 940001, 0003, 000003,   6)!
INSERT INTO lagerfach VALUES (0004, 940001, 0004, 000004,   7)!

COMMIT WORK RELEASE!

CONNECT kern IDENTIFIED BY test SQLMODE ORACLE!

SELECT auftrag.auftnum, nummer, menge, wert
  FROM auftrag, auftpos, artikel
  WHERE auftrag.auftnum = auftpos.auftnum
    AND artikel.nummer  = auftpos.artikel!
FETCH INTO :a, :b, :c, :d!

SELECT auftrag.auftnum, SUM (menge*wert)
  FROM auftrag, auftpos, artikel
  WHERE auftrag.auftnum = auftpos.auftnum
    AND artikel.nummer  = auftpos.artikel
  GROUP BY auftrag.auftnum!
FETCH INTO :a, :b!

SELECT auftnum, auftpos, artikel, name, menge FROM auftpos, artikel
  WHERE artikel = nummer
    AND (auftnum, auftpos, artikel)
          IN (SELECT auftnum, auftpos, artikel FROM lagerfach)
  ORDER BY auftnum, auftpos!
FETCH INTO :a, :b, :c, :d, :e!

SELECT auftnum, auftpos, artikel, name, menge FROM auftpos, artikel
  WHERE artikel = nummer
    AND (auftnum, auftpos)
          IN (SELECT auftnum, auftpos FROM lagerfach
                WHERE artikel = nummer)
  ORDER BY auftnum, auftpos!
FETCH INTO :a, :b, :c, :d, :e!

SELECT auftnum, auftpos, artikel, name, menge FROM auftpos, artikel
  WHERE artikel = nummer
    AND auftnum IN (SELECT auftnum FROM lagerfach
                      WHERE artikel = nummer
                        AND lagerfach.auftpos = auftpos.auftpos)
  ORDER BY auftnum, auftpos!
FETCH INTO :a, :b, :c, :d, :e!
COMMIT WORK RELEASE!

CONNECT kern IDENTIFIED BY test SQLMODE internal !
COMMIT !
DECLARE res5b CURSOR FOR
  SELECT * FROM auftrag ak, artikel a
    WHERE (ak.auftnum, a.nummer) = ANY (SELECT auftnum, artikel FROM auftpos)!
FETCH res5b INTO :a, :b, :c, :d, :e, :f, :g !
SELECT * FROM auftrag ak, artikel a, auftpos ap
  WHERE (ak.auftnum, a.nummer) = ANY ((ap.auftnum, ap.artikel))!
FETCH !
SELECT * FROM auftrag ak, auftpos ap, artikel a
  WHERE (ak.auftnum, a.nummer) = ((ap.auftnum, ap.artikel))!
FETCH INTO :a, :b, :c, :d, :e, :f, :g, :h, :i, :j, :k !
SELECT * FROM auftrag ak, artikel a
  WHERE (ak.auftnum, a.nummer) = ANY (SELECT auftnum, artikel FROM auftpos)!
FETCH !
SELECT ak.*, a.* FROM auftrag ak, artikel a, auftpos ap
  WHERE ak.auftnum = ap.auftnum
    AND a.nummer   = ap.artikel!
FETCH !
SELECT * FROM auftrag ak, artikel a
  WHERE ak.auftnum IN (SELECT ap.auftnum FROM auftpos ap
                         WHERE a.nummer = ap.artikel)!
FETCH !
COMMIT WORK RELEASE!

CONNECT kern IDENTIFIED BY test!
CREATE TABLE tt1 (a1 CHAR(5), b1 CHAR(5))!
CREATE TABLE tt2 (a2 CHAR(5), b2 CHAR(5))!
INSERT INTO tt1 VALUES ('Hallo','Welt')!
INSERT INTO tt1 VALUES ('Wie','gehts')!
INSERT INTO tt2 VALUES ('Hallo','Du')!
SELECT * FROM tt1, tt2 WHERE (a1, b1) =  ((a2, 'Welt'))!
SELECT * FROM tt1, tt2 WHERE a1='Hallo' AND (a1, b1) =  ((a2, 'Welt'))!
SELECT * FROM tt1, tt2 WHERE (a1, b1) =  ((a2, 'Welt')) AND a1='Hallo'!
SELECT * FROM tt1, tt2 WHERE (a1, b1) IN ((a2, 'Welt'))!
FETCH !
ROLLBACK WORK RELEASE!
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
