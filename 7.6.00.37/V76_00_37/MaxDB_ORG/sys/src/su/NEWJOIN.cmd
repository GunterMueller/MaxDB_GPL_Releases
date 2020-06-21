FILE CONNECT ( KERN TEST SQLMODE &1 !

CREATE TABLE J1 (J1 INT, C1 CHAR)!
CREATE TABLE J2 (J2 INT, C2 CHAR)!
CREATE TABLE J3 (J3 INT, C3 CHAR)!
INSERT INTO J1 VALUES (0, 'A')!

select * from (
select * from (
select * from (select t1.c1 from j1 t1, j1 t2)))!

select * from (
select * from (
select * from (select t1.c1 from j1 t1, j1 t2)))
union 
select * from (
select * from (
select t1.c1 from j1 t1 left join 
(j1 t2 join (select * from j3) t3) on t1.j1 = t2.j1))  !
fetch !

(((select * from (
select * from (
select * from (select t1.c1 from j1 t1, j1 t2))))))
union ALL 
select * from (
((select * from (
select t1.c1 from j1 t1 left join 
(j1 t2 join (select * from j3) t3) on t1.j1 = t2.j1))))  !
fetch !

select * from (
select * from (
select t1.j1 from j1 t1 left join 
(j1 t2 join (select * from j3) t3) on t1.j1 = t2.j1))  !
fetch !

select * from (
select * from (
select t1.j1 from j1 t1 right join 
(j1 t2 join (select * from j3) t3) on t1.j1 = t2.j1))  !
fetch !

select t1.j1 from j1 t1 ,j3 t3 where t1.j1 = t3.j3 (+)!
fetch !

select t1.j1 from j1 t1 ,j3 t3 where t1.j1 (+) = t3.j3 !
fetch !

select * from j1 t1 left outer join 
j1 t2 join
j1 t3 join
j1 t4 join
j1 t5 join
j1 t6 join
j1 t7 join
j1 t8 join
j1 t9 join
j1 t10 join
j1 t11 join
j1 t12 join
j1 t13 join
j1 t14 join
j1 t15 join
j1 t16  !


INSERT INTO J1 VALUES (1, 'A')!
INSERT INTO J1 VALUES (2, 'A')!
INSERT INTO J1 VALUES (3, 'A')!
INSERT INTO J2 VALUES (3, 'B')!
INSERT INTO J2 VALUES (4, 'B')!
INSERT INTO J2 VALUES (5, 'B')!
INSERT INTO J2 VALUES (0, 'A')!
INSERT INTO J3 VALUES (0, 'C')!
INSERT INTO J3 VALUES (5, 'C')!
INSERT INTO J3 VALUES (6, 'C')!
INSERT INTO J3 VALUES (7, 'C')!
SELECT * FROM J1 CROSS JOIN J2 !
fetch !
SELECT * FROM J3, (J1 JOIN J2) !
fetch !
SELECT * FROM (J1 T1 CROSS JOIN (J2 T2 JOIN J3 T3)) !
fetch !
SELECT * FROM (J1 T1 JOIN (J2 T2 JOIN J3 T3)) !
fetch !
SELECT * FROM J1 JOIN J2 !
fetch !
SELECT * FROM J1 NATURAL JOIN J2 !
fetch !
SELECT * FROM J1 FULL OUTER JOIN J2 ON J1 = J2 !
fetch !
SELECT * FROM J1 LEFT OUTER JOIN J2 ON J1 (+) = J2 !
fetch !
SELECT * FROM J1 LEFT OUTER JOIN J2 ON J1 = J2 !
fetch !
SELECT * FROM J1 RIGHT OUTER JOIN J2 ON J1 = J2 !
fetch !
SELECT * FROM J1 LEFT OUTER JOIN J2 ON J1 = J2 (+) !
fetch !
SELECT * FROM J1 FULL OUTER JOIN J2 ON J1 (+) = J2 (+) !
fetch !

CREATE VIEW v1 AS 
SELECT * FROM J1 CROSS JOIN J2 !
SELECT * from v1 !
fetch !

ROLLBACK !

CREATE TABLE HOTEL (
HNR INT, 
NAME CHAR (30), 
ORT CHAR (10),
PRIMARY KEY (HNR))!
INSERT INTO HOTEL VALUES (1, 'Schweizer Hof', 'Berlin')!
INSERT INTO HOTEL VALUES (2, 'InterConti', 'Berlin')!
INSERT INTO HOTEL VALUES (3, 'Garni', 'Zossen')!
INSERT INTO HOTEL VALUES (4, 'Palace', 'Paris')!
INSERT INTO HOTEL VALUES (5, 'Ritz', 'Paris')!
INSERT INTO HOTEL VALUES (6, 'Kempinski', 'Berlin')!

CREATE TABLE RAUM  (HNR INT,
PREIS INT,
PRIMARY KEY (HNR))!
INSERT INTO RAUM VALUES (1, 100)!
INSERT INTO RAUM VALUES (2, 150)!
INSERT INTO RAUM VALUES (3, 75)!
INSERT INTO RAUM VALUES (4, 200)!
INSERT INTO RAUM VALUES (5, 1000)!
INSERT INTO RAUM VALUES (6, 105)!

SELECT  ORT, AVG (PREIS) FROM HOTEL, RAUM
WHERE HOTEL.HNR = RAUM.HNR 
AND RAUM.PREIS > 10
GROUP BY (ORT) !
fetch !

SELECT  ORT, AVG (PREIS) FROM HOTEL JOIN  RAUM
ON HOTEL.HNR = RAUM.HNR 
WHERE RAUM.PREIS > 10
GROUP BY (ORT) !
fetch !

SELECT  ORT, AVG (PREIS) FROM HOTEL JOIN  RAUM
ON HOTEL.HNR = RAUM.HNR 
GROUP BY (ORT) !
fetch !

SELECT NAME, ORT 
FROM HOTEL X, RAUM
WHERE X.HNR = RAUM.HNR
AND RAUM.PREIS <= (SELECT AVG(RAUM.PREIS) FROM HOTEL, RAUM 
                   WHERE HOTEL.HNR = RAUM.HNR 
                   AND HOTEL.ORT = X.ORT)!
fetch !
SELECT NAME, ORT 
 FROM HOTEL  X INNER JOIN RAUM ON  X.HNR = RAUM.HNR
 WHERE RAUM.PREIS <= ( SELECT AVG(RAUM.PREIS) 
                       FROM HOTEL INNER JOIN RAUM ON HOTEL.HNR = RAUM.HNR
                       WHERE HOTEL.ORT = X.ORT)! 
fetch !
SELECT NAME, ORT 
 FROM HOTEL  X INNER JOIN RAUM ON  X.HNR = RAUM.HNR
 WHERE RAUM.PREIS <= ( SELECT AVG(RAUM.PREIS) 
                       FROM HOTEL, RAUM 
                       WHERE HOTEL.HNR = RAUM.HNR 
                       AND HOTEL.ORT = X.ORT)! 
fetch !
SELECT NAME, ORT 
 FROM HOTEL  X, RAUM 
 WHERE X.HNR = RAUM.HNR
 AND RAUM.PREIS <= ( SELECT AVG(RAUM.PREIS) 
                       FROM HOTEL INNER JOIN RAUM ON HOTEL.HNR = RAUM.HNR
                       WHERE HOTEL.ORT = X.ORT)! 
fetch !
ROLLBACK !

CREATE TABLE TAB1 ( A INT, B INT, C INT)!
CREATE TABLE TAB2 ( A INT, B INT, C INT)!
CREATE TABLE TAB3 ( A INT, B INT, C INT)!
INSERT INTO TAB1 VALUES (    1,   1, 1) !
INSERT INTO TAB1 VALUES (    1,   1, 2) !
INSERT INTO TAB1 VALUES (    1,   1, 3) !
INSERT INTO TAB1 VALUES (    1,   2, 4) !
INSERT INTO TAB2 VALUES (    1,   1, 1) !
INSERT INTO TAB2 VALUES (    1,   1, 2) !
INSERT INTO TAB2 VALUES (    1,   1, 3) !
INSERT INTO TAB2 VALUES (    1,   3, 4) !
COMMIT WORK !
SELECT * FROM TAB1 CROSS JOIN TAB2 !
fetch !
SELECT * FROM TAB1 CROSS JOIN TAB2
WHERE TAB2.C < TAB1.C  !
fetch !
SELECT * FROM TAB1 NATURAL JOIN TAB2 ON
                   TAB1.A = TAB2.A AND
                   TAB1.B = TAB2.B AND
                   TAB2.B =    1
WHERE TAB2.C < TAB1.C  !
fetch !
SELECT * FROM TAB1  JOIN TAB2 ON
                   TAB1.A = TAB2.A AND
                   TAB1.B = TAB2.B AND
                   TAB2.B =    1
WHERE TAB2.C < TAB1.C  !
fetch !
SELECT * FROM TAB1 NATURAL JOIN TAB2 ON
        USING (A, B)
WHERE TAB2.C < TAB1.C  !
fetch !
SELECT * FROM TAB1 NATURAL INNER JOIN TAB2 ON
        USING (A, B)
WHERE TAB2.C < TAB1.C  !
fetch !
SELECT * FROM TAB1 NATURAL LEFT JOIN TAB2 ON
        USING (A, B)
WHERE TAB2.C < TAB1.C  !
fetch !
SELECT * FROM TAB1 NATURAL LEFT OUTER JOIN TAB2 ON
        USING (A, B)
WHERE TAB2.C < TAB1.C  !
fetch !
SELECT * FROM TAB1 NATURAL RIGHT OUTER JOIN TAB2 ON
        USING (A, B)
WHERE TAB2.C < TAB1.C  !
fetch !
SELECT * FROM TAB1 NATURAL FULL OUTER JOIN TAB2 ON
        USING (A, B)
WHERE TAB2.C < TAB1.C  !
fetch !
SELECT * FROM TAB1 NATURAL FULL JOIN TAB2 ON
        USING (A, B)
WHERE TAB2.C < TAB1.C  !
fetch !
SELECT * FROM TAB1 NATURAL FULL OUTER TAB2 ON
        USING (A, B)
WHERE TAB2.C < TAB1.C  !
fetch !
SELECT * FROM TAB1, TAB2 !
fetch !
SELECT * FROM TAB1, TAB2
WHERE TAB1.A     = TAB2.A AND
      TAB1.B     = TAB2.B AND
      TAB2.B     =    1   AND
      TAB2.C < TAB1.C !
fetch !
SELECT * FROM TAB1, TAB2
WHERE TAB1.A     = TAB2.A (+) AND
      TAB1.B     = TAB2.B (+) AND
      TAB2.B (+) =    1       AND
      TAB2.C < TAB1.C !
fetch !
SELECT * FROM TAB1 LEFT OUTER JOIN TAB2 ON
                   TAB1.A = TAB2.A AND
                   TAB1.B = TAB2.B AND
                   TAB2.B =    1
WHERE TAB2.C < TAB1.C  !
fetch !
SELECT * FROM ( TAB1 LEFT OUTER JOIN TAB2 ON
                   TAB1.A = TAB2.A AND
                   TAB1.B = TAB2.B AND
                   TAB2.B =    1)
WHERE TAB2.C < TAB1.C  !
fetch !
SELECT * FROM (( TAB1 LEFT OUTER JOIN TAB2 ON
                   TAB1.A = TAB2.A AND
                   TAB1.B = TAB2.B AND
                   TAB2.B =    1))
WHERE TAB2.C < TAB1.C  !
fetch !
SELECT * FROM TAB1 LEFT OUTER JOIN TAB2 ON
        USING (A, B)
WHERE TAB2.C< TAB1.C !
fetch !

SELECT * FROM TAB1 TAB1 UNION JOIN TAB2 !
fetch !       
SELECT * FROM TAB1 TAB1 UNION SELECT * FROM TAB2
WHERE TAB2.C < TAB1.C  !
fetch !       
SELECT * FROM TAB1 TAB1 CROSS JOIN TAB2 !
fetch !       
*! ES DARF KEINE SEARCHCONDITION KOMMEN
SELECT * FROM TAB1 TAB1 CROSS JOIN TAB2
WHERE TAB2.C < TAB1.C  !
fetch !       
SELECT * FROM TAB1 NATURAL JOIN TAB2 ON
                   TAB1.A = TAB2.A AND
                   TAB1.B = TAB2.B AND
                   TAB2.B =    1
WHERE TAB2.C < TAB1.C  !
fetch !       
SELECT * FROM TAB1  JOIN TAB2 ON
                   TAB1.A = TAB2.A AND
                   TAB1.B = TAB2.B AND
                   TAB2.B =    1
WHERE TAB2.C < TAB1.C  !
fetch !       
SELECT * FROM TAB1 NATURAL JOIN TAB2 
        USING (A, B)
WHERE TAB2.C < TAB1.C  !
fetch !       
SELECT * FROM TAB1 NATURAL JOIN TAB2 ON
        USING (A, B)
WHERE TAB2.C < TAB1.C  !
fetch !       
SELECT * FROM TAB1 NATURAL INNER JOIN TAB2 
        USING (A, B)
WHERE TAB2.C < TAB1.C  !
fetch !       
SELECT * FROM TAB1 NATURAL LEFT JOIN TAB2 
        USING (A, B)
WHERE TAB2.C < TAB1.C  !
fetch !       
SELECT * FROM TAB1 NATURAL LEFT OUTER JOIN TAB2
        USING (A, B)
WHERE TAB2.C < TAB1.C  !
fetch !       
SELECT * FROM TAB1 NATURAL RIGHT OUTER JOIN TAB2
        USING (A, B)
WHERE TAB2.C < TAB1.C  !
fetch !       
SELECT * FROM TAB1 NATURAL FULL OUTER JOIN TAB2
        USING (A, B)
WHERE TAB2.C < TAB1.C  !
fetch !       
SELECT * FROM TAB1 NATURAL FULL JOIN TAB2
        USING (A, B)
WHERE TAB2.C < TAB1.C  !
fetch !       
SELECT * FROM TAB1 NATURAL FULL OUTER TAB2
        USING (A, B)
WHERE TAB2.C < TAB1.C  !
fetch !       
SELECT * FROM TAB1, TAB2
WHERE TAB1.A     = TAB2.A (+) AND
      TAB1.B     = TAB2.B (+) AND
      TAB2.B (+) =    1       AND
      TAB2.C < TAB1.C !
fetch !       
SELECT * FROM TAB1 LEFT OUTER JOIN TAB2 ON
                   TAB1.A = TAB2.A AND
                   TAB1.B = TAB2.B AND
                   TAB2.B =    1
WHERE TAB2.C < TAB1.C  !
fetch !       
SELECT * FROM ( TAB1 LEFT OUTER JOIN TAB2 ON
                   TAB1.A = TAB2.A AND
                   TAB1.B = TAB2.B AND
                   TAB2.B =    1)
WHERE TAB2.C < TAB1.C  !
fetch !       
SELECT * FROM (( TAB1 LEFT OUTER JOIN TAB2 ON
                   TAB1.A = TAB2.A AND
                   TAB1.B = TAB2.B AND
                   TAB2.B =    1))
WHERE TAB2.C < TAB1.C  !
fetch !       
SELECT * FROM TAB1 LEFT OUTER JOIN TAB2 
        USING (A, B)
WHERE TAB2.C< TAB1.C !
fetch !       
CREATE VIEW V AS SELECT TAB1.A, TAB1.C, TAB1.B, TAB2.A A2 
FROM TAB1, TAB2 WHERE 
TAB1.A = TAB2.B !
SELECT * FROM V TAB1 LEFT OUTER JOIN TAB2 
        USING (A, B)
WHERE TAB2.C< TAB1.C !
fetch !
SELECT TAB1.A, TAB1.C, TAB1.B, TAB2.A A2, T3.* FROM TAB1 INNER JOIN TAB2 
LEFT OUTER JOIN TAB2 T3
        USING (A, B)
WHERE TAB1.A = TAB2.B AND T3.C< TAB1.C !
fetch !
ROLLBACK WORK RELEASE !
file connect ( kern test !
drop table tab1!
drop table tab2!
drop table tab3!
commit release!
