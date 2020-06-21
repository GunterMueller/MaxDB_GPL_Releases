file connect ( kern test !
create table t ( a char (8)) !
insert t set a = 'aaa'!
insert t set a = 'xxx'!
create table t1 ( b char (8))!
insert t1 set b = 'aaa'!
insert t1 set b = 'xxx'!
create table t3 (c char (8))!
insert t3 set c = 'bbb'!
insert t3 set c = 'ccc'!
select * from t,t1
  where rowno <= 3!
select * from t,t1
  where not exists (select * from t3)!
select * from t, t1
  where not exists (select * from t3 where c = t.a )!
insert t3 set c = 'aaa'!
insert t3 set c = 'aaa'!
insert t3 set c = 'bbb'!
select * from t, t1
     where exists (select distinct * from t3
                             where c = t.a) !
select * from t, t1
  where not exists 
         (select distinct * from t3 x where
             exists (select * from t3 where c = t.a
                             and c = x.c ) )!
select * from t, t1
  where not exists 
         (select distinct c from t3 x where
             exists (select * from t3 where c = t.a
                             and c = x.c ) )!
create index "t3.c" on t3( c ) ! 
select * from t, t1
  where not exists 
         (select distinct c from t3 x where
             exists (select * from t3 where c = t.a
                             and c = x.c ) )!
select * from t, t1
  where not exists 
         (select distinct * from t3 x where
             exists (select * from t3 where c = t.a
                             and c = x.c ) )!
create view v as select distinct * from t3 !
select * from t
     where exists (select distinct * from t3
                             where c = t.a) !
select * from t
     where exists (select * from v 
                             where c = t.a) !
pars_then_ex !
select * from t
     where exists (select * from v 
                             where c = t.a) !
select * from t, t1
  where not exists 
         (select * from v x where
             exists (select * from t3 where c = t.a
                             and c = x.c ) )!
select * from t, t1
  where not exists 
         (select * from v x where
             exists (select * from t3 where c = 'bbb') )!
select * from t, t1
  where not exists 
         (select * from v x where x.c = t.a and
             exists (select * from t3 where c = 'bbb') )!
select * from t, t1
  where not exists 
         (select * from t3 x where x.c = 'bbb' and
             exists (select * from v where v.c = 'bbb') )!
select * from t, t1
  where not exists 
         (select c from v x where
             exists (select * from t3 where c = t.a
                             and c = x.c ) )!
create view v1 as select distinct c from t3 !
select * from t, t1
  where not exists 
         (select * from v1 x where
             exists (select * from t3 where c = t.a
                             and c = x.c ) )!
select * from t, t1
  where not exists 
         (select c from v1 x where
             exists (select * from t3 where c = t.a
                             and c = x.c ) )!
pars_execute!
rollback release !
file connect ( kern test !
* correlated subqueries mit UNION; Crash bis incl. 10.01.94 !
create table liwi (
SCHLUESSEL         FIXED (10) key,
NAME               CHAR (25),
ART                CHAR (4),
WERT               FIXED (10))!
insert liwi values (1, 'Liane', 'R3', 1)!
insert liwi values (2, 'Liane', 'R3', 7)!
insert liwi values (3, 'Rocco', 'R3', 1)!
insert liwi values (4, 'Elke ', 'Kern', 1)!
insert liwi values (5, 'Rocco', 'R3', 1)!
select * from liwi l
where schluessel> (select min(schluessel) from liwi
                   where name in (select name from liwi
                   except all select distinct name from liwi)
                   and name=l.name and art=l.art
                   and art in (select art from liwi
                   except all select distinct art from liwi))!
fetch !
rollback release!
file connect ( kern test !
create table t ( a date, b char (8))!
insert t values ('18940512', 'aaaa')!
insert t values ('18940131', 'aaaa')!
insert t values ('18940512', 'bbbb')!
insert t values ('18940131', 'bbbb')!
create table t1 ( c char (8))!
insert t1 set c = 'ccc'!
set format eur !
select * from t where a = (select max (a) from t )!
fetch !
select * from t where a = (select max (a) from t,t1 )!
fetch!
select * from t x 
 where a = (select max (a) from t where x.b = t.b)!
fetch!
set format internal !
select * from t where a = (select max (a) from t )!
fetch!
select * from t x 
 where a = (select max (a) from t where x.b = t.b)!
fetch!
rollback release !
file connect ( kern test !
CREATE TABLE KOM
("SP1               " FIXED (4,0)        ,
 "SP2               " CHAR (1) ASCII  ,
 "GREIF_KNZ         " CHAR (1) ASCII  ) !
insert kom values (
   100 , '1' , '1') !
insert kom values (
   200 , '2' , '2') !
insert kom values (
   300 , '3' , '1') !
CREATE TABLE KOP
("TEILE_NR          " FIXED (4,0)        ,
 "GREIF_KNZ         " CHAR (1) ASCII  ) !
insert kop values (     1 , '1' )!
insert kop values (     1 , '1' )!
insert kop values (     1 , '1' )!
insert kop values (     2 , '1' )!
insert kop values (     2 , '1' )!
insert kop values (     2 , '2' )!
insert kop values (     2 , '2' )!
select  km.greif_knz, teile_nr
from kop kp, kom km
where kp.greif_knz = '1' !
fetch !
select teile_nr, count(teile_nr)
          from kop kp
          where kp.greif_knz = '1'
          group by teile_nr !
fetch !
select teile_nr, count(teile_nr)
          from kop kp
          where kp.greif_knz = '1'
          group by teile_nr 
          having count (kp.teile_nr) >= 2 !
fetch !
select teile_nr, count(teile_nr)
          from kop kp
          where kp.greif_knz = '2'
          group by teile_nr !
fetch !
select teile_nr, count(teile_nr)
          from kop kp
          where kp.greif_knz = '2'
          group by teile_nr 
          having count (kp.teile_nr) >= 2 !
fetch !
select  km.greif_knz
from kop kp, kom km
where kp.greif_knz = '1'
and teile_nr in
  (select teile_nr
          from kop kp
          where kp.greif_knz = km.greif_knz
          group by teile_nr
          having count (kp.teile_nr) >= 2) !
fetch !
pars_then_ex !
data !
select  km.greif_knz
from kop kp, kom km
where kp.greif_knz = '1'
and teile_nr in
  (select teile_nr
          from kop kp
          where kp.greif_knz = km.greif_knz
          group by teile_nr
          having count (kp.teile_nr) >= :tn) !
n 38 2

nodata !
fetch into :a !
pars_execute !
rollback release !

* Meldung Michael 509011: Datumsfunktionen mit Corelated Columns liefern
* einen Fehler bei der semantischen Analyse!
file CONNECT ( kern test!
CREATE TABLE tt1 (t1f CHAR(10), t1date DATE)!
INSERT INTO tt1 VALUES ('Hallo', '19950830')!
INSERT INTO tt1 VALUES ('Hallo', '19950728')!
CREATE TABLE tt2 (t2f CHAR(10), t2date DATE)!
INSERT INTO tt2 VALUES ('Hallo', '19950830')!
INSERT INTO tt2 VALUES ('Hallo', '19950625')!

SELECT * FROM tt1 WHERE t1f IN
  (SELECT t2f FROM tt2 WHERE CHAR (t1date, USA) = CHAR (t2date, USA))!
SELECT * FROM tt1 WHERE t1f IN
  (SELECT t2f FROM tt2 WHERE ADDDATE (t1date, 1) = ADDDATE (t2date, 1))!

SELECT * FROM tt1
  WHERE t1f IN (SELECT t2f FROM tt2
      WHERE CHAR (t1date, INTERNAL) = CHAR (t2date, INTERNAL))!
SET FORMAT ISO!
SELECT * FROM tt1
  WHERE t1f IN (SELECT t2f FROM tt2
      WHERE CHAR (t1date, INTERNAL) = CHAR (t2date, INTERNAL))!
SET FORMAT INTERNAL!
SELECT * FROM tt1
  WHERE t1f IN (SELECT t2f FROM tt2
                      WHERE CHAR (t1date, ISO) = CHAR (t2date, ISO))!
SET FORMAT ISO!
SELECT * FROM tt1
  WHERE t1f IN (SELECT t2f FROM tt2
                      WHERE CHAR (t1date, ISO) = CHAR (t2date, ISO))!

ROLLBACK WORK RELEASE!
file CONNECT  ( kern test!
 
CREATE TABLE TABX
( K FIXED(18),
  N FIXED(18),
  PRIMARY KEY( K,N ))!
INSERT INTO TABX VALUES( 1, 1)!
CREATE SYNONYM TVA0 FOR TABX!
 
COMMIT!
 
CREATE VIEW TVA1 AS SELECT K,N FROM TVA0 T0
WHERE K = ( SELECT MAX(N) FROM TVA0 WHERE K = T0.N)!
COMMIT!
SELECT T0.N FROM TVA1 T0
WHERE T0.N IN (SELECT T1.N FROM  TVA1 T1
             WHERE T0.K = 1)!
fetch !
CREATE VIEW BVDL1 AS SELECT T0.N FROM TVA1 T0
WHERE T0.N IN (SELECT T1.N FROM  TVA1 T1
             WHERE T0.K = 1)!
select * from bvdl1 !
fetch !
 
CREATE VIEW TVA2 AS SELECT K,N FROM TVA1 T0
WHERE K = ( SELECT MAX(N) FROM TVA1 WHERE K = T0.N)!
COMMIT!
SELECT T0.N FROM TVA2 T0
WHERE T0.N IN (SELECT T1.N FROM  TVA2 T1
               WHERE T0.K = 1)!
fetch !
CREATE VIEW BVDL2 AS SELECT T0.N FROM TVA2 T0
WHERE T0.N IN (SELECT T1.N FROM  TVA2 T1
               WHERE T0.K = 1)!
select * from bvdl2 !
fetch !
CREATE VIEW TVA3 AS SELECT K,N FROM TVA2 T0
WHERE K = ( SELECT MAX(N) FROM TVA2 WHERE K = T0.N)!
COMMIT!
      SELECT ((((N)))) "N" 
        FROM KERN.TABX T0 
       WHERE K = (SELECT MAX(N) 
                    FROM KERN.TABX TVA0 
                    WHERE K = T0.N
                 ) AND 
             ( K = (SELECT MAX((N)) 
                     FROM KERN.TABX T0 
                    WHERE K = (SELECT MAX(N) 
                                 FROM KERN.TABX TVA0 
                                WHERE K = T0.N
                              ) AND 
                          ( K = T0.N)
                   ) AND 
               ( K = (SELECT MAX((N)) 
                        FROM TVA1 T0 
                       WHERE K = (SELECT MAX(N)  
                                    FROM TVA1 
                                   WHERE K = T0.N
                                 ) AND 
                             ( K = T0.N)
                     ) AND 
                 ( T0.N IN (SELECT T1.N 
                              FROM TVA3 T1 
                             WHERE T0.K = 1
                           ) 
                 )
                )
               )
      !
fetch!
SELECT T0.N FROM TVA3 T0
WHERE T0.N IN (SELECT T1.N FROM  TVA3 T1
               WHERE T0.K = 1)!
fetch !
CREATE VIEW BVDL3 AS SELECT T0.N FROM TVA3 T0
WHERE T0.N IN (SELECT T1.N FROM  TVA3 T1
               WHERE T0.K = 1)!
 
SELECT * FROM BVDL3!
fetch!
DROP TABLE TABX!
COMMIT WORK RELEASE!
