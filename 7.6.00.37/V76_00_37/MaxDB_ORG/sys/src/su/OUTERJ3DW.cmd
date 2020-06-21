file connect (kern test !

create table T1 (K1 fixed (4) key, A1 char (2))!

insert into t1 values ( 1, 'A' ) !
insert into t1 values ( 2, 'B' ) !
insert into t1 values ( 3, 'C' ) !
insert into t1 values ( 4, 'D' ) !
insert into t1 values ( 5, 'E' ) !

create table T2 (K1 fixed (4) key, A2 char (2))!

insert into t2 values ( 1, 'F' ) !
insert into t2 values ( 3, 'G' ) !
insert into t2 values ( 4, 'H' ) !

create table T3 (K1 fixed (4) key, A3 char (2))!

insert into t3 values ( 5, 'I' ) !
insert into t3 values ( 2, 'J' ) !
insert into t3 values ( 4, 'K' ) !

create table T4 (K1 fixed (4) key, A4 char (2))!

insert into t4 values ( 1, 'L' ) !
insert into t4 values ( 2, 'M' ) !
insert into t4 values ( 3, 'N' ) !

commit release !

file connect (kern test !
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3
FROM < T1, T2, T3 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T2.K1=T3.K1 (+) !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3
FROM < T1, T2, T3 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T2.K1=T3.K1 (+) !
 
fetch !
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3
FROM < T1, T3, T2 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T2.K1=T3.K1 (+) !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3
FROM < T1, T3, T2 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T2.K1=T3.K1 (+) !
 
fetch !
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3
FROM < T2, T1, T3 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T2.K1=T3.K1 (+) !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3
FROM < T2, T1, T3 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T2.K1=T3.K1 (+) !
 
fetch !
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3
FROM < T2, T3, T1 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T2.K1=T3.K1 (+) !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3
FROM < T2, T3, T1 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T2.K1=T3.K1 (+) !
 
fetch !
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3
FROM < T3, T1, T2 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T2.K1=T3.K1 (+) !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3
FROM < T3, T1, T2 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T2.K1=T3.K1 (+) !
 
fetch !
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3
FROM < T3, T2, T1 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T2.K1=T3.K1 (+) !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3
FROM < T3, T2, T1 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T2.K1=T3.K1 (+) !
 
fetch !
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T1, T2, T3, T4 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T1, T2, T3, T4 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
fetch !
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T1, T2, T4, T3 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T1, T2, T4, T3 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
fetch !
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T1, T3, T2, T4 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T1, T3, T2, T4 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
fetch !
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T1, T3, T4, T2 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T1, T3, T4, T2 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
fetch !
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T1, T4, T2, T3 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T1, T4, T2, T3 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
fetch !
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T1, T4, T3, T2 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T1, T4, T3, T2 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
fetch !
 
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T2, T1, T3, T4 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T2, T1, T3, T4 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
fetch !
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T2, T1, T4, T3 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T2, T1, T4, T3 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
fetch !
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T2, T3, T1, T4 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T2, T3, T1, T4 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
fetch !
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T2, T3, T4, T1 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T2, T3, T4, T1 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
fetch !
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T2, T4, T1, T3 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T2, T4, T1, T3 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
fetch !
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T2, T4, T3, T1 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T2, T4, T3, T1 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
fetch !
 
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T3, T2, T1, T4 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T3, T2, T1, T4 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
fetch !
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T3, T2, T4, T1 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T3, T2, T4, T1 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
fetch !
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T3, T1, T2, T4 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T3, T1, T2, T4 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
fetch !
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T3, T1, T4, T2 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T3, T1, T4, T2 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
fetch !
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T3, T4, T2, T1 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T3, T4, T2, T1 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
fetch !
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T3, T4, T1, T2 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T3, T4, T1, T2 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
fetch !
 
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T4, T2, T3, T1 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T4, T2, T3, T1 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
fetch !
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T4, T2, T1, T3 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T4, T2, T1, T3 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
fetch !
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T4, T3, T2, T1 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T4, T3, T2, T1 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
fetch !
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T4, T3, T1, T2 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T4, T3, T1, T2 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
fetch !
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T4, T1, T2, T3 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T4, T1, T2, T3 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
fetch !
 
EXPLAIN SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T4, T1, T3, T2 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
SELECT T1.K1, T1.A1, T2.A2, T3.A3, T4.A4
FROM < T4, T1, T3, T2 >
WHERE T1.K1=T2.K1 (+) AND
      T1.K1=T3.K1 (+) AND
      T1.K1=T4.K1 !
 
fetch !

DROP TABLE T1 !
DROP TABLE T2 !
DROP TABLE T3 !
DROP TABLE T4 !
 
commit work release !
