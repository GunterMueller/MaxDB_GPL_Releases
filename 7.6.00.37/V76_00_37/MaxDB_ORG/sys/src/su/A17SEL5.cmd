*   *ID* A17SEL5  DBCMD    changed on 1992-09-28-13.22.47 by ADMIN     *
file connect ( kern test !
CREATE TABLE T5 ( A CHAR(4) KEY, B FIXED(5,2), c char(4)) !
INSERT T5 VALUES ('AAA', 11, 'zzz')!
INSERT T5 VALUES ('BAA', 11, 'z')!
INSERT T5 VALUES ('CAA', 13, 'zz')!
INSERT T5 VALUES ('DAA', 13, 'a')!
INSERT T5 VALUES ('EAA', 15, 'a')!
INSERT T5 VALUES ('FAA', 15, 'a')!
SELECT * FROM T5 !
FETCH !
select * from t5 order by a !
fetch !
select a from t5 order by b !
fetch !
select a from t5 order by a desc !
fetch !
select * from t5 order by c, b desc !
fetch !
select distinct b,c from t5 order by c, b desc !
fetch !
select * from t5 order by 4 !
fetch !
select * from t5 order by 1 desc !
fetch !
select * from t5 order by 3, b desc !
fetch !
select * from t5 order by 3, c desc !
fetch !
* ORDER BY - Strategie fuer Index, der auch NULL - Werte enhalten kann *
create table t51 (
k1  char (30) key,
k2  char (30) key,
k6  char (30) ,
k7  char (30) ) !
insert t51 values ('aa', 'bb', 'GG', 'GG') !
insert t51 values ('bb', 'cc', 'MM', 'MM') !
insert t51 values ('cc', 'dd', 'AA', 'AA') !
insert t51 values ('dd', 'ee', 'AB', 'AB') !
insert t51 values ('ee', 'ff', null, null) !
insert t51 values ('ff', 'gg', 'MA', null) !
create index "t51.k6"
	on t51( k6 ) !
create index "t51.k7"
	on t51( k7 ) !
explain select * from t51 where k6 < 'ZZ' order by k6 !
explain select * from t51 where k6 < 'ZZ' order by k7 !
explain select * from t51 where k7 < 'ZZ' order by k6 !
select * from t51 where k6 < 'ZZ' order by k6 !
fetch!
select * from t51 where k6 < 'ZZ' order by k7 !
fetch!
select * from t51 where k7 < 'ZZ' order by k6 !
fetch!
select * from t51 where k6 > '  ' order by k6 !
fetch!
explain select * from t51 where k6 > '  ' !
select * from t51 where k6 > '  ' !
fetch!
select * from t51 where k6 = 'MM' order by k6 !
fetch!
select * from t51 where k6 like 'A%' order by k6 !
fetch!
select * from t51 where k6 like '%A' order by k6 !
fetch!
select * from t51 where k6 between 'AB' and 'LL' order by k6 !
fetch!
select * from t51 where k6 in ('MM', 'AB') order by k6 !
fetch!
select * from t51 where k6 is null order by k6 !
fetch!
select * from t51 where k6 is not null order by k6 !
fetch!
select * from t51 where k6 > '  ' and k7 > 'AA' order by k6 !
fetch!
select * from t51 where k6 > 'AA' or k7 < 'MM' order by k6 !
fetch!
select * from t51 where k6 > '  ' and k7 is null order by k6 !
fetch!
select * from t51 where k6 > '  ' and (k7 > '  ' or k7 < 'ZZ')
order by k6 !
fetch!
select * from t51 where k7 > '  ' and (k6 > '  ' or k6 < 'ZZ')
order by k6 !
fetch!
drop table t5 !
drop table t51 !
COMMIT WORK RELEASE !
file connect ( kern test !
create table t1 ( a char (8), b1 char (7))!
insert t1 set a = 'aaa', b1 = 'bbb'!
create table t2 (a fixed (3), b2  fixed (5))!
insert t2 set a = 234, b2 = 12345 !
 * das geht sowieso !
select * from t1 order by a!
fetch into :a, :b !
 * das geht auch in echt Oracle 8 nicht !
select * from t1, t2 order by a !
fetch into :a, :b, :c, :d !
 * das geht komischerweise in echt Oracle 8 nicht !
select t1.*, t2.b2 from t1, t2 order by a !
fetch into :a, :b, :c!
 * das geht auch in echt Oracle 8 nicht !
select t1.a, t2.* from t1, t2 order by a !
fetch into :a, :b, :c!
 * das geht auch in echt Oracle 8 nicht !
select substr(t1.a, 1, 2), t2.b2 from t1, t2 order by a !
fetch into :a, :b!
 * das muß ja nun gehen !
select t1.a, t2.b2 from t1,t2 order by a !
fetch into :a, :b !  
 * das sollte ja nun wirklich gehen !
select * from t1, t2 order by t1.a !
fetch into :a, :b, :c, :d !
 * mit GROUP BY geht das ganze Verfahren auch in echt Oracle 8 nicht!
select t1.a, min(t2.b2) from t1,t2 group by a !
fetch into :a, :b !  
rollback release !
