file connect ( kern test sqlmode oracle !
create table t ( a char (8), b char (9), c char (10), d number(2),
primary key (a,b,c))!
create unique index i1 on t ( a,b,c)!
select * from t where a = 'aaa'  and b = 'bbb' and c = 'ccc'!
create table t1 ( a char (8), b char (9), c char (10), d number(2),
primary key (c,b,a))!
 * Pseudoindex was not checked correctly !
create unique index i1 on t1 (c,b,a )!
select * from t1 where a = 'aaa' and b = 'bbb' and c = 'ccc'!
fetch into :a, :b, :c, :d !
select t.a, t1.a from <t,t1> where t.a = t1.a and t.b = t1.b and
   t.c = t1.c !
fetch into :a, :b !
drop table t !
drop table t1 !
commit work release !
