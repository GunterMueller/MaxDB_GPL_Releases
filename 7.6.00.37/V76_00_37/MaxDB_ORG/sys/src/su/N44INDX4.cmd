*   *ID* N44INDX4 DBCMD    changed on 1992-09-28-13.26.34 by ADMIN     *
file connect ( kern test !
* fest langes Feld
create table t ( a char (5), b char(7), c char (6))!
create index i1 on t (a, b desc, c )!
insert t values ('a', NULL, 'b')!
insert t values ('aaaa', 'b', 'b')!
declare erg cursor for 
select * from t where a = 'a' for reuse !
fetch erg!
declare erg cursor for 
select * from t where a = 'a' and b is null for reuse!
declare erg cursor for 
select * from t where a = 'a' and b is null and c = 'b' for reuse!
rollback release !
file connect ( kern test !
* variable langes Feld
create table t ( a char (50), b char(7), c char (6))!
create index i1 on t (a, b desc, c )!
insert t values ('a', NULL, 'b')!
insert t values ('aaaa', 'b', 'b')!
declare erg cursor for 
select * from t where a = 'a' for reuse !
fetch erg !
declare erg cursor for 
select * from t where a = 'a' and b is null for reuse!
declare erg cursor for 
select * from t where a = 'a' and b is null and c = 'b' for reuse!
delete t!
insert t values ('a', NULL, null)!
declare erg cursor for 
select * from t where a = 'a' and b is null for reuse!
declare erg cursor for 
select * from t where a = 'a' and b is null and c is null for reuse!
delete t!
drop index i1 on t!
create index i1 on t (a, b , c desc)!
insert t values ('a', 'b' , null)!
declare erg cursor for 
select * from t where a = 'a' and b = 'b' for reuse!
declare erg cursor for 
select * from t where a = 'a' and b = 'b' and c is null for reuse!
rollback release !
file connect ( kern test !
create table t ( a fixed (3), b fixed (6,0))!
insert t set a = 3, b = 6!
create index i1 on t (a desc, b)!
create table t1 ( c fixed (10))!
insert t1 set c = 10!
select * from t where a = 3 and b = 6!
select * from t, t1 where a = 3 and b = 6!
pars_then_ex !
select * from t where a = 3 and b = 6!
data !
select *  from t where a = :a and b = :b!
n 3 3 n 6 6

select * from t, t1 where a = :a and b = :b!
n 3 3 n 6 6

nodata!
pars_execute!
rollback work release !
