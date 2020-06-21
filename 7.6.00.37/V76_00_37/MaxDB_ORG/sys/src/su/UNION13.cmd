*   *ID* UNION13  DBCMD    changed on 1992-09-28-13.28.46 by ADMIN     *
file connect ( kern test !
create table t1 (s1 int,
                 s2 int,
                 sl int) !
insert t1 values (1, 2, 3)!
create table t2  (ak1 int key,
                 ak2 int,
                 akl int) !
insert t2 values (1, 3, 4) !
insert t2 values (2, 1, 4) !
insert t2 values (3, 3, 4) !

select s1*4 from t1
union
select akl/2 from t2!
fetch!

select s1*2 from t1
intersect
select akl/2 from t2!
fetch!

select s1*4 from t1
except
select akl/2 from t2!
fetch!

rollback release !
file connect ( kern test !
* till 22.6.94 length of 2. order-column was the same as the
* first one ==> error -2010 !
create table t ( a char (8), b char (8)) !
alter table t add ( c char (10))!
alter table t add ( d char (45))!
insert t values ('aaaaaaaa', 'bbbbbbbb', 'cccccccccc',
'ddddddddd1ddddddddd2ddddddddd3ddddddddd4ddddd')!
select c, d from t union select c, d from t !
fetch!
select c, d from t union select c, d from t order by 1, 2!
fetch!
rollback release !
file connect ( kern test !
create table t ( a char (40) , b char (8), c fixed(5))!
insert t values ('abcdefghijklmnopqrstuvw', 'aaaa', 4)!
select count(a), count(a) from t group by b !
fetch !
select avg(c) from t !
fetch!
select b, avg(c), b from t group by b !
fetch!
select count(a), 'aaa' from t 
union
select min(c)  , 'xxx' from t 
order by 1 desc !
fetch !
select count(A), count(a) from t 
union
select min(c)  , count(a) from t !
fetch !
select count(A), b, avg(c), count(a), avg(c) from t group by b !
fetch !
select count(A), b, count(a), 'xyz' from t group by b 
union
select 17      , b, 17      , a     from t 
order by 1 !
fetch!
select count(A), b, avg(c), count(a), avg(c), 'xyz' from t group by b 
union
select 17      , b, 17    , 17      , 17    , a     from t 
order by 1 !
fetch!
select count(A), b, avg(c), count(a) from t group by b 
union
select 17      , b, 17    , 17       from t !
fetch!
drop table t !
commit release !
