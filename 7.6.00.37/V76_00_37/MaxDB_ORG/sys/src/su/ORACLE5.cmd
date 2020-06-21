file CONNECT ( kern test SQLMODE ORACLE !

CREATE TABLE privtest (a CHAR(5), b CHAR (4)) !
GRANT  ALL PRIVILEGES ON privtest TO   public !
REVOKE ALL PRIVILEGES ON privtest FROM public !
GRANT  ALL PRIV       ON privtest TO   public !
REVOKE ALL PRIV       ON privtest FROM public !
GRANT  ALL            ON privtest TO   public !
REVOKE ALL            ON privtest FROM public !
GRANT                 ON privtest TO   public !
REVOKE                ON privtest FROM public !
DROP TABLE privtest !

CREATE TABLE commtest (a CHAR(5), b CHAR (4)) !
COMMENT ON TABLE  commtest   IS 'Comment table  commtest'   !
COMMENT ON COLUMN commtest.a IS 'Comment column commtest.a' !
COMMENT ON commtest (a IS 'Comment commtest.a', b IS 'Comment commtest.b') !
COMMENT ON commtest !
DROP TABLE commtest !


* Test der Moeglichkeit, im OracleMode, die HAVING und die !
* GROUP BY Klausel zu vertauschen !

CREATE TABLE havingtab  
 (
 spalte1 NUMBER(3),
 spalte2 CHAR(30),
 spalte3 NUMBER(2,1) 
 ) !

INSERT INTO havingtab VALUES (1, 'Das ist der 1. Datensatz.', 3.5)!
INSERT INTO havingtab VALUES (2, 'Das ist der 2. Datensatz.', 3.6)!
INSERT INTO havingtab VALUES (3, 'Das ist der 3. Datensatz.', 3.6)!
INSERT INTO havingtab VALUES (4, 'Das ist der 4. Datensatz.', 3.6)!
INSERT INTO havingtab VALUES (5, 'Das ist der 5. Datensatz.', 3.7)!
INSERT INTO havingtab VALUES (6, 'Das ist der 6. Datensatz.', 3.7)!
INSERT INTO havingtab VALUES (7, 'Das ist der 7. Datensatz.', 3.8)!
INSERT INTO havingtab VALUES (8, 'Das ist der 8. Datensatz.', 3.9)!
INSERT INTO havingtab VALUES (9, 'Das ist der 9. Datensatz.', 3.9)!
INSERT INTO havingtab VALUES (10, 'Das ist der 10. Datensatz.', 7.5)!
INSERT INTO havingtab VALUES (11, 'Das ist der 11. Datensatz.', 7.5)!
INSERT INTO havingtab VALUES (12, 'Das ist der 12. Datensatz.', 7.5)!

* This Select Statement must be run in ORACLE mode !
SELECT spalte3
FROM havingtab
GROUP BY spalte3
HAVING  SUM (spalte1) > 2 !
 
FETCH INTO :a !
 
* This Select Statement must be run in ORACLE mode !
SELECT spalte3
FROM havingtab
HAVING  SUM (spalte1) > 2
GROUP BY spalte3 !
 
FETCH INTO :a ! 

COMMIT WORK RELEASE!

file CONNECT ( KERN TEST SQLMODE internal!

* This Select Statement must be run in the internal mode!
SELECT spalte3
FROM havingtab
GROUP BY spalte3
HAVING  SUM (spalte1) > 2 !
 
FETCH !
 
* This Select Statement can not be run in the internal mode!
SELECT spalte3
FROM havingtab
HAVING  SUM (spalte1) > 2
GROUP BY spalte3 !
 
DROP TABLE havingtab ! 

COMMIT WORK RELEASE !
