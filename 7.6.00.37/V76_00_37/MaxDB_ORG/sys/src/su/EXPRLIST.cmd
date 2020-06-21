CONNECT kern IDENTIFIED BY test SQLMODE ORACLE!

* This is to see all the possible rows!
SELECT * FROM artikel!
FETCH INTO :a, :b, :c, :d!
SELECT * FROM st_liste!
FETCH INTO :a, :b, :c, :d!

* A little bit error checking!
* 1. Incomplete operation!
SELECT * FROM dual WHERE  (1,2)!
SELECT * FROM dual WHERE  (1,2) <>!
SELECT * FROM dual WHERE  (1,2) NOT!
SELECT * FROM dual WHERE  (1,2) IN!
SELECT * FROM dual WHERE  (1,2) IN (!
SELECT * FROM dual WHERE  (1,2) IN (SELECT!

* 2. Compare operation!
SELECT * FROM artikel WHERE  (nummer,  name)*3 = 27!
SELECT * FROM artikel WHERE  (nummer,  name)-5 = 27!
SELECT * FROM artikel WHERE  (nummer,  name)   = 27!
SELECT * FROM artikel WHERE  (nummer,  name)   = (nummer, name)!

* 3. IN operation with expression list!
SELECT * FROM artikel WHERE  (nummer,  name)    IN ((1))!
SELECT * FROM artikel WHERE  (nummer,  name)    IN ((1,2,3))!
SELECT * FROM artikel WHERE  (nummer,  name)    IN (1,2)!
SELECT * FROM artikel WHERE ((nummer), name)    IN (1,2)!
SELECT * FROM artikel WHERE  (nummer,  name)    IN ((1,2),(1,2,3),(1,2))!
SELECT * FROM artikel WHERE ((nummer = name),2) IN ((1,2))!

* 4. IN operation with subquery!
SELECT * FROM artikel WHERE  (nummer, 0, 0) 
  IN (SELECT nummer, 0 FROM artikel)!
SELECT * FROM artikel WHERE  (nummer, 0, 0) 
  IN (SELECT * FROM artikel)!
SELECT * FROM artikel WHERE  (nummer, name, zeichnung, wert, 0, 1) 
  IN (SELECT * FROM artikel)!
SELECT * FROM artikel WHERE  (nummer, 0) 
  IN (SELECT nummer, 0, name FROM artikel)!

* 5. Checking of expression trees with 'and' or 'or'!
SELECT * FROM dual WHERE 1 = 1 AND (1,1) IN ((1,2), (1,1))!
SELECT * FROM dual WHERE 1 = 2 AND (1,1) IN ((1,2), (1,1))!
SELECT * FROM dual WHERE 1 = 1 AND (1,1) IN ((1,2), (2,1))!
SELECT * FROM dual WHERE 1 = 2 OR  (1,1) IN ((1,2), (1,1))!
SELECT * FROM dual WHERE 1 = 1 OR  (1,1) IN ((1,2), (2,1))!

* 6. Lists must be on the same syntactic level as the relational operator!
SELECT * FROM dual WHERE  ((1,1)) IN ((1,2), (1,1))!
SELECT * FROM dual WHERE (((1,1)) IN ((1,2), (1,1)))!
SELECT * FROM dual WHERE  ((1,1)  IN ((1,2), (1,1)))!
SELECT * FROM dual WHERE (((((((7*8+56,SYSDATE,48)IN((1,2,3),(2,3,4))))))))!
SELECT * FROM dual WHERE (((((((7*8+56,SYSDATE,48)IN((1,2,3),(2,3,4,5))))))))!
SELECT * FROM dual WHERE (((((((7*8+56,SYSDATE,48))IN((1,2,3),(2,3,4)))))))!

* 7. The datatypes of the n'th values must be compatible!
SELECT * FROM artikel
  WHERE (nummer, 0, 'Kein NameKein Name')
          IN ((3, SYSDATE,         SYSDATE),
              (4, SYSDATE-SYSDATE, name||name))!
SELECT * FROM artikel
  WHERE (nummer, SYSDATE, 'Pos-Test1')
          IN (SELECT nummer, 0, name FROM artikel)!
SELECT * FROM artikel
  WHERE (nummer, name, zeichnung, SYSDATE)
          IN (SELECT * FROM artikel)!
* ...even in corelated subqueries!
SELECT name, position FROM artikel, st_liste
  WHERE (nummer, position)
           IN (SELECT bauk, SYSDATE FROM st_liste WHERE bauk = nummer)!

* 8. Comparing of lists is only allowed for <equal-or-not>!
SELECT * FROM dual WHERE (1,1) < ANY ((1,2), (2,1))!
SELECT * FROM dual WHERE (1,1) <>    ((1,2), (2,1))!
SELECT * FROM dual WHERE (1,1) >=    ((1,2), (2,1))!

* 9. There are limits in the length of all the compared columns!
SELECT * FROM st_liste
  WHERE (bauk, 'Hallo', 'Welt')
     IN (SELECT nummer, RPAD (name, 125), RPAD (name, 124) FROM artikel)!

* 10. At last we found that a list is treated like a subquery, not only
*     for IN or NOT IN, but also for = or <>. So here some more selects!
SELECT * FROM dual WHERE (1,'XYZ',3) IN     ((1,'XYZ',3),(3,'ABC',5))!
SELECT * FROM dual WHERE (1,'XYZ',3) NOT IN ((1,'XYZ',3),(3,'ABC',5))!
SELECT * FROM dual WHERE (1,'123',4) IN     ((1,'XYZ',3),(3,'ABC',5))!
SELECT * FROM dual WHERE (1,'123',4) NOT IN ((1,'XYZ',3),(3,'ABC',5))!

SELECT * FROM dual WHERE (1,'XYZ',3) =  ANY ((1,'XYZ',3),(3,'ABC',2))!
SELECT * FROM dual WHERE (1,'XYZ',3) =  ANY ((3,'ABC',2),(1,'XYZ',3))!
SELECT * FROM dual WHERE (1,'XYZ',3) =  ALL ((1,'XYZ',3),(3,'ABC',2))!
SELECT * FROM dual WHERE (1,'XYZ',3) <> ANY ((1,'XYZ',3),(3,'ABC',2))!
SELECT * FROM dual WHERE (1,'XYZ',3) <> ALL ((1,'XYZ',3),(3,'ABC',2))!
SELECT * FROM dual WHERE (1,'XYZ',3) <> ANY ((1,'XYZ',3),(1,'XYZ',3))!
SELECT * FROM dual WHERE (1,'XYZ',3) =  ALL ((1,'XYZ',3),(1,'XYZ',3))!

SELECT * FROM dual WHERE (1,'XYZ',3) =  ANY ((2,'123',4),(3,'ABC',2))!
SELECT * FROM dual WHERE (1,'XYZ',3) =  ALL ((2,'123',4),(3,'ABC',2))!
SELECT * FROM dual WHERE (1,'XYZ',3) <> ANY ((2,'123',4),(3,'ABC',2))!
SELECT * FROM dual WHERE (1,'XYZ',3) <> ALL ((2,'123',4),(3,'ABC',2))!

SELECT * FROM dual WHERE (1,'XYZ',3) =      ((1,'XYZ',3),(2,'123',1))!
SELECT * FROM dual WHERE (1,'XYZ',3) <>     ((1,'XYZ',3),(2,'123',1))!
SELECT * FROM dual WHERE (1,'XYZ',3) <      ((1,'XYZ',3))!
SELECT * FROM dual WHERE (1,'XYZ',3) < ANY  ((1,'XYZ',3))!
SELECT * FROM dual WHERE (1,'XYZ',3) < ALL  ((1,'XYZ',3))!

SELECT * FROM dual WHERE (1,'XYZ',3) =  ((1,'XYZ',3))!
SELECT * FROM dual WHERE (1,'XYZ',3) <> ((1,'XYZ',3))!
SELECT * FROM dual WHERE (1,'XYZ',3) =  ((2,'123',4))!
SELECT * FROM dual WHERE (1,'XYZ',3) <> ((2,'123',4))!
COMMIT!

*------------------------------------------!
* And now finally a few successful selects !
*------------------------------------------!

SELECT * FROM artikel
  WHERE (nummer, 0, 'Kein NameKein Name')
          IN ((3, nummer-3,
               'Kein Name' || SUBSTR ('Kein Name mehr frei', 1, 9)),
              (4, SYSDATE-SYSDATE,       name||name))!
FETCH INTO :a, :b, :c, :d!

SELECT * FROM artikel
  WHERE (nummer, 0, 'Kein NameKein Name')
          NOT IN ((3, nummer-3,
                  'Kein Name' || SUBSTR ('Kein Name mehr frei', 1, 9)),
                 (4, SYSDATE-SYSDATE,       name||name))!
FETCH INTO :a, :b, :c, :d!

* Here two IN statements with a few function calls within a join select!
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

* And now the same as view and as table as select definition!
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

SELECT * FROM artikel
  WHERE (nummer, 0, 'Pos-Test1')
          IN (SELECT nummer, 0, name FROM artikel)!
FETCH INTO :a, :b, :c, :d!

SELECT * FROM artikel
  WHERE (nummer, 0, 'Pos-Test2')
          <> ALL (SELECT nummer, 0, name FROM artikel)!
FETCH INTO :a, :b, :c, :d!

SELECT * FROM artikel
  WHERE (nummer, 1) IN (SELECT bauk, position FROM st_liste)!
FETCH INTO :a, :b, :c, :d!

SELECT * FROM artikel
  WHERE (nummer, 1) NOT IN (SELECT stueck, position FROM st_liste)!
FETCH INTO :a, :b, :c, :d!

* Das folgende Select hat zwar nicht mit Expressionlisten zu tun, hat aber
* mal zu Problemen (ROOT CHECK) gefuehrt und passt nirgendwo sonst hin!
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

SELECT nummer, name, zeichnung FROM artikel
  WHERE (name||name, nummer+nummer) IN (SELECT name, nummer FROM artikel)!
SELECT nummer, name, zeichnung FROM artikel
  WHERE (name, nummer) IN (SELECT name||name, nummer+nummer FROM artikel)!
SELECT nummer, name, zeichnung FROM artikel
  WHERE (nummer, name, zeichnung, wert*nummer) IN
     (SELECT 2, 'Posartikel 1', 'Ohne Zeichnung', 20 FROM dual)!
FETCH INTO :a, :b, :c!

* The treatment of null values in subqueries isn't yet consistent. But
* even in Oracle6.0 you can get surprised. There are 7 rows in table
* 'artikel', none of them with name or zeichnung 'Gibts nicht'.
* If there is a second number behind a slash, it's the result with the
* values on both sides switched. The results are the same, whether you
* compare with a subquery from dual or with a list of a two values list!
*
* Query                                                 SQL-DB  Oracle6.0
* -----------------------------------------------------------------------
* SELECT COUNT (*) FROM artikel                           7       7
*   WHERE (name, zeichnung) NOT IN
*      (SELECT 'Gibts nicht', 'Gibts nicht' FROM dual)
* -----------------------------------------------------------------------
* SELECT COUNT (*) FROM artikel                           0       7
*   WHERE (name, NULL) NOT IN
*      (SELECT 'Gibts nicht', 'Gibts nicht' FROM dual)
* -----------------------------------------------------------------------
* SELECT COUNT (*) FROM artikel                           7/0     7
*   WHERE (name, zeichnung) NOT IN
*      (SELECT 'Gibts nicht', NULL FROM dual)
* -----------------------------------------------------------------------
* SELECT COUNT (*) FROM artikel                           0       0
*   WHERE (name, NULL) IN
*      (SELECT 'Gibts nicht', NULL FROM dual)
* -----------------------------------------------------------------------
* SELECT COUNT (*) FROM artikel                           0       7/0
*   WHERE (name, NULL) NOT IN
*      (SELECT 'Gibts nicht', NULL FROM dual)
* -----------------------------------------------------------------------

SELECT COUNT (*) FROM artikel
  WHERE (name, NULL) NOT IN (SELECT 'Gibts Nicht', 'Gibts Nicht' FROM dual)!
FETCH INTO :a!
SELECT COUNT (*) FROM artikel
  WHERE (name, zeichnung) NOT IN (SELECT 'Gibts Nicht', NULL FROM dual)!
FETCH INTO :a!
SELECT COUNT (*) FROM artikel
  WHERE (name, NULL) IN (SELECT 'Gibts Nicht', NULL FROM dual)!
FETCH INTO :a!
SELECT COUNT (*) FROM artikel
  WHERE (name, NULL) NOT IN (SELECT 'Gibts Nicht', NULL FROM dual)!
FETCH INTO :a!
SELECT COUNT (*) FROM artikel
  WHERE (name, NULL) NOT IN (('Gibts Nicht', 'Gibts Nicht'))!
FETCH INTO :a!
SELECT COUNT (*) FROM artikel
  WHERE (name, zeichnung) NOT IN (('Gibts Nicht', NULL))!
FETCH INTO :a!
SELECT COUNT (*) FROM artikel
  WHERE (name, NULL)     IN (('Gibts Nicht', NULL))!
FETCH INTO :a!
SELECT COUNT (*) FROM artikel
  WHERE (name, NULL) NOT IN (('Gibts Nicht', NULL))!
FETCH INTO :a!
SELECT COUNT (*) FROM artikel
  WHERE (NULL, name) NOT IN (SELECT 'Gibts Nicht', 'Gibts Nicht' FROM dual)!
FETCH INTO :a!
SELECT COUNT (*) FROM artikel
  WHERE (zeichnung, name) NOT IN (SELECT NULL, 'Gibts Nicht' FROM dual)!
FETCH INTO :a!
SELECT COUNT (*) FROM artikel
  WHERE (NULL, name) IN (SELECT NULL, 'Gibts Nicht' FROM dual)!
FETCH INTO :a!
SELECT COUNT (*) FROM artikel
  WHERE (NULL, name) IN (SELECT NULL, 'Gibts Nicht' FROM dual)!
FETCH INTO :a!

* Oracle6.0 wouldn't return a row for the following selects, since there
* blanks at the end of a string are significant!
SELECT * FROM artikel
  WHERE (name, RPAD (zeichnung, 60)) IN
     (SELECT RPAD (name, 30), zeichnung FROM artikel)!
FETCH INTO :a, :b, :c, :d!

* If there is a value on the left side, that must be truncated to the size
* of the value to the right, these both values are unequal!
SELECT COUNT (*) FROM dual
  WHERE ('Hallo', 'Hello World') IN
     (SELECT 'Hallo', 'Hello' FROM dual)!
FETCH INTO :a!
SELECT COUNT (*) FROM dual
  WHERE ('Hallo', 'Hello') = ANY
     (SELECT 'Hallo Welt', 'Hello' FROM dual)!
FETCH INTO :a!
SELECT COUNT (*) FROM dual
  WHERE ('Hallo', 'Hello World') <> ALL
     (SELECT 'Hallo Welt', 'Hello' FROM dual)!
FETCH INTO :a!

* And now a few corelated subqueries!
SELECT name, position FROM artikel, st_liste
  WHERE (nummer, position)
           IN (SELECT bauk, 2 FROM st_liste WHERE bauk = nummer)!
FETCH INTO :a, :b!

SELECT nummer, name, zeichnung FROM artikel
  WHERE (nummer, 2)
          IN (SELECT bauk, position FROM st_liste WHERE bauk = nummer)!
FETCH INTO :a, :b, :c!

SELECT bauk, position FROM st_liste
  WHERE (bauk, position)
           IN (SELECT nummer, 2 FROM artikel WHERE bauk = nummer)!
FETCH INTO :a, :b!

SELECT nummer, name, zeichnung, position FROM artikel, st_liste
  WHERE bauk = nummer
    AND (bauk, position)
           IN (SELECT nummer, 2 FROM artikel WHERE bauk = nummer)!
FETCH INTO :a, :b, :c, :d!

SELECT nummer, name, zeichnung FROM artikel
  WHERE ('Test-String', nummer, 2)
          IN (SELECT 'Test-String', bauk, position FROM st_liste
                WHERE bauk = nummer)!
FETCH INTO :a, :b, :c!

SELECT nummer, name, zeichnung FROM artikel
  WHERE ('Test-String', nummer, 2)
          <> ALL (SELECT 'Test-String', bauk, position FROM st_liste
                    WHERE bauk = nummer)!
FETCH INTO :a, :b, :c!

SELECT nummer, name, zeichnung FROM artikel
  WHERE ('Test-String am Anfang', nummer, 2)
          IN (SELECT 'Test-String', bauk, position FROM st_liste
                WHERE bauk = nummer)!

SELECT nummer, name, zeichnung FROM artikel
  WHERE ('Test-String', nummer, 2)
          IN (SELECT 'Test-String am Anfang', bauk, position FROM st_liste
                WHERE bauk = nummer)!

* And now corelated (and normal) subqueries with selection of all columns!

SELECT * FROM artikel
  WHERE (4, 'Kein Name', zeichnung, wert)
    IN (SELECT * FROM artikel)!

* Since we will continue to find no row, if we don't assign a meaningful
* value to 'wert', we do this better right now!
UPDATE artikel SET wert = 24 WHERE nummer = 4!

SELECT * FROM artikel
  WHERE (4, 'Kein Name', zeichnung, wert)
    IN (SELECT * FROM artikel)!
FETCH INTO :a, :b, :c, :d!

SELECT * FROM artikel a
  WHERE (4, 'Kein Name', zeichnung, wert)
    IN (SELECT * FROM artikel WHERE name = a.name)!
FETCH INTO :a, :b, :c, :d!

SELECT * FROM artikel a
  WHERE (4, 'Kein Name', zeichnung, wert)
    IN (SELECT nummer, name, zeichnung, wert FROM artikel
          WHERE name = a.name)!
FETCH INTO :a, :b, :c, :d!

SELECT * FROM artikel a
  WHERE EXISTS (SELECT nummer, name, zeichnung, wert FROM artikel
                  WHERE name = a.name)!
FETCH INTO :a, :b, :c, :d!

* In update clauses there can't be a corelation, so we update here all rows!
UPDATE artikel SET wert = 27
  WHERE (4, 'Kein Name', zeichnung)
    IN (SELECT nummer, name, zeichnung FROM artikel)!

* The following few statements are a bunch of CREATE ... AS SELECT!
CREATE VIEW tv AS
SELECT * FROM artikel a
  WHERE (4, 'Kein Name', zeichnung, wert)
    IN (SELECT nummer, substr(' ' || name, 2), zeichnung, wert FROM artikel)!
SELECT * FROM tv!
FETCH INTO :a, :b, :c, :d!
DROP VIEW tv!

CREATE VIEW tv AS
SELECT * FROM artikel a
  WHERE (4, 'Kein Name', zeichnung, wert)
    IN (SELECT * FROM artikel WHERE name = a.name)!
SELECT * FROM tv!
FETCH INTO :a, :b, :c, :d!
DROP VIEW tv!

CREATE TABLE tt AS
SELECT * FROM artikel a
  WHERE (4, 'Kein Name', zeichnung, wert)
    IN (SELECT * FROM artikel)!
SELECT * FROM tt!
FETCH INTO :a, :b, :c, :d!
DELETE FROM tt !

INSERT INTO tt SELECT * FROM artikel a
                 WHERE (4, 'Kein Name', zeichnung, wert)
                   IN (SELECT * FROM artikel WHERE name = a.name)!
SELECT * FROM tt!
FETCH INTO :a, :b, :c, :d!
DROP TABLE tt!

* The following is to check, whether the setting of the date/time format
* has no bad influence on expression lists!
COMMIT WORK RELEASE!

CONNECT kern IDENTIFIED BY test!

CREATE TABLE testdatum (datum DATE, zeit TIME)!
INSERT INTO testdatum VALUES ('19940501', '00150000')!
INSERT INTO testdatum VALUES (DATE,       '00150000')!
UPDATE testdatum SET datum = ADDDATE (datum, -6) WHERE zeit = '00150000'!
COMMIT WORK!

SELECT zeit FROM testdatum
  WHERE datum IN (SELECT ADDDATE (DATE, -6) FROM dual)!
FETCH!
SELECT * FROM testdatum
  WHERE datum NOT IN (SELECT ADDDATE (DATE, -6) FROM dual)!
FETCH!
SELECT zeit FROM testdatum
  WHERE (datum, zeit)
    IN (SELECT ADDDATE (DATE, -6), MAKETIME (15, 0, 0) FROM dual)!
FETCH!
SELECT * FROM testdatum
  WHERE (datum, zeit)
    NOT IN (SELECT ADDDATE (DATE, -6), MAKETIME (15, 0, 0) FROM dual)!
FETCH!
SELECT zeit FROM testdatum WHERE (datum, zeit)
  IN (SELECT datum, zeit FROM testdatum)!
FETCH!
SELECT * FROM testdatum WHERE (datum, zeit)
  NOT IN (SELECT datum, zeit FROM testdatum)!
SELECT zeit FROM testdatum WHERE (datum, zeit)
  IN (SELECT * FROM testdatum)!
FETCH!
SELECT * FROM testdatum WHERE (datum, zeit)
  NOT IN (SELECT * FROM testdatum)!
SELECT zeit FROM testdatum
  WHERE (datum, zeit)
    IN (SELECT ADDDATE (DATE, -6), MAKETIME (15, 0, 0)
          FROM dual WHERE DATE <> datum)!
FETCH!
SELECT * FROM testdatum
  WHERE (datum, zeit)
    NOT IN (SELECT ADDDATE (DATE, -6), MAKETIME (15, 0, 0)
              FROM dual WHERE DATE <> datum)!
FETCH!
SELECT zeit FROM testdatum
  WHERE (zeit, datum)
    IN (SELECT MAKETIME (15, 0, 0), ADDDATE (DATE, -6)
          FROM dual WHERE DATE <> datum)!
FETCH!
SELECT * FROM testdatum
  WHERE (zeit, datum)
    NOT IN (SELECT MAKETIME (15, 0, 0), ADDDATE (DATE, -6)
              FROM dual WHERE DATE <> datum)!
FETCH!
COMMIT!

* And now the same with another date/time format!
SET FORMAT EUR!

SELECT zeit FROM testdatum
  WHERE datum IN (SELECT ADDDATE (DATE, -6) FROM dual)!
FETCH!
SELECT * FROM testdatum
  WHERE datum NOT IN (SELECT ADDDATE (DATE, -6) FROM dual)!
FETCH!
SELECT zeit FROM testdatum
  WHERE (datum, zeit)
    IN (SELECT ADDDATE (DATE, -6), MAKETIME (15, 0, 0) FROM dual)!
FETCH!
SELECT * FROM testdatum
  WHERE (datum, zeit)
    NOT IN (SELECT ADDDATE (DATE, -6), MAKETIME (15, 0, 0) FROM dual)!
FETCH!
SELECT zeit FROM testdatum WHERE (datum, zeit)
  IN (SELECT datum, zeit FROM testdatum)!
FETCH!
SELECT * FROM testdatum WHERE (datum, zeit)
  NOT IN (SELECT datum, zeit FROM testdatum)!
SELECT zeit FROM testdatum WHERE (datum, zeit)
  IN (SELECT * FROM testdatum)!
FETCH!
SELECT * FROM testdatum WHERE (datum, zeit)
  NOT IN (SELECT * FROM testdatum)!
SELECT zeit FROM testdatum
  WHERE (datum, zeit)
    IN (SELECT ADDDATE (DATE, -6), MAKETIME (15, 0, 0)
          FROM dual WHERE DATE <> datum)!
FETCH!
SELECT * FROM testdatum
  WHERE (datum, zeit)
    NOT IN (SELECT ADDDATE (DATE, -6), MAKETIME (15, 0, 0)
              FROM dual WHERE DATE <> datum)!
FETCH!
SELECT zeit FROM testdatum
  WHERE (zeit, datum)
    IN (SELECT MAKETIME (15, 0, 0), ADDDATE (DATE, -6)
          FROM dual WHERE DATE <> datum)!
FETCH!
SELECT * FROM testdatum
  WHERE (zeit, datum)
    NOT IN (SELECT MAKETIME (15, 0, 0), ADDDATE (DATE, -6)
              FROM dual WHERE DATE <> datum)!
FETCH!

DROP TABLE testdatum!
SET FORMAT INTERNAL!
COMMIT RELEASE!

* Some selects with the order and store tables!
FILE ords_d2l!

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

SELECT * FROM auftpos
  WHERE (auftnum, auftpos, artikel)
          IN (SELECT auftnum, auftpos, artikel FROM lagerfach)
  ORDER BY auftnum, auftpos!
FETCH INTO :a, :b, :c, :d!

SELECT auftnum, auftpos, artikel, name, menge FROM auftpos, artikel
  WHERE artikel = nummer
    AND (auftnum, auftpos, artikel)
          IN (SELECT auftnum, auftpos, artikel FROM lagerfach)
  ORDER BY auftnum, auftpos!
FETCH INTO :a, :b, :c, :d, :e!

SELECT auftnum, auftpos, artikel, menge FROM auftpos
  WHERE (auftnum, auftpos)
          IN (SELECT auftnum, auftpos FROM lagerfach
                WHERE lagerfach.artikel = auftpos.artikel)
  ORDER BY auftnum, auftpos!
FETCH INTO :a, :b, :c, :d!

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

DECLARE res CURSOR FOR 
SELECT * FROM dual WHERE 1 = 1 AND (1,1) = any ((1,2), (1,1))!
FETCH res INTO :a !
DECLARE res4 CURSOR FOR 
SELECT * FROM dual WHERE 1 = 1 AND 1 = any (3-2, 4 div 2) !
FETCH res4 INTO :a !
COMMIT WORK RELEASE!

CONNECT kern IDENTIFIED BY test SQLMODE internal !
DECLARE res CURSOR FOR 
SELECT * FROM dual WHERE 1 = 1 AND (1,1) IN ((1,2), (1,1))!
FETCH res INTO :a !
DECLARE res1 CURSOR FOR SELECT * FROM artikel
  WHERE (nummer, 0, 'Pos-Test1')
          IN (SELECT nummer, 0, name FROM artikel)!
FETCH res1 INTO :a, :b, :c, :d!
DECLARE res2 CURSOR FOR SELECT * FROM artikel
  WHERE (nummer, 0, 'Kein NameKein Name')
          IN ((3, nummer-3,
               'Kein Name' || SUBSTR ('Kein Name mehr frei', 1, 9)),
              (4, 3-(5-2),       name||name))!
FETCH res2 INTO :a, :b, :c, :d!
DECLARE res3 CURSOR FOR 
  SELECT * FROM dual WHERE 1 = 1 AND (1,1) = any ((1,2), (1,1))!
FETCH res3 INTO :a !
DECLARE res4 CURSOR FOR
  SELECT * FROM dual WHERE 1 = 1 AND 1 = any (3-2, 4 div 2) !
FETCH res4 INTO :a !
DECLARE res5 CURSOR FOR SELECT COUNT (*) FROM dual
  WHERE ('Hallo', 'Hello World') <> ALL
     (SELECT 'Hallo Welt', 'Hello' FROM dual)!
FETCH res5 INTO :a!
DECLARE res5a CURSOR FOR SELECT COUNT (*) FROM dual
  WHERE ('Hallo', 'Hello World') < ALL
     (SELECT 'Hallo Welt', 'Hello' FROM dual)!
FETCH res5a INTO :a!
COMMIT !
SELECT * INTO :a, :b, :c, :d FROM artikel WHERE nummer = 4 !
UPDATE artikel SET name = (SELECT MAX(name) FROM artikel)
    WHERE nummer = 4 !
SELECT * INTO :a, :b, :c, :d FROM artikel WHERE nummer = 4 !
ROLLBACK !
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
* Drei Versuche, denselben 3fach-Join auszudruecken!
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

connect kern identified by test sqlmode ansi !
declare res cursor for 
SELECT * FROM dual WHERE 1 = 1 AND (1,1) IN ((1,2), (1,1))!
fetch res into :a !
declare res1 cursor for SELECT * FROM artikel
  WHERE (nummer, 0, 'Pos-Test1')
          IN (SELECT nummer, 0, name FROM artikel)!
FETCH res1 INTO :a, :b, :c, :d!
declare res2 cursor for SELECT * FROM artikel
  WHERE (nummer, 0, 'Kein NameKein Name')
          IN ((3, nummer-3,
               'Kein Name' || SUBSTR ('Kein Name mehr frei', 1, 9)),
              (4, 3-(5-2),       name||name))!
FETCH res2 INTO :a, :b, :c, :d!
declare res3 cursor for 
SELECT * FROM dual WHERE 1 = 1 AND (1,1) = any ((1,2), (1,1))!
fetch res3 into :a !
declare res4 cursor for 
SELECT * FROM dual WHERE 1 = 1 AND 1 = any (3-2, 4 div 2) !
fetch res4 into :a !
declare res5 cursor for SELECT COUNT (*) FROM dual
  WHERE ('Hallo', 'Hello World') <> ALL
     (SELECT 'Hallo Welt', 'Hello' FROM dual)!
FETCH res5 into :a !
declare res5a cursor for SELECT COUNT (*) FROM dual
  WHERE ('Hallo', 'Hello World') < ALL
     (SELECT 'Hallo Welt', 'Hello' FROM dual)!
FETCH res5a into :a !
commit !
select * into :a, :b, :c, :d from artikel where nummer = 4 !
update artikel set name = (select max(name) from artikel)
    where nummer = 4 !
select * into :a, :b, :c, :d from artikel where nummer = 4 !
rollback work release !

connect kern identified by test sqlmode db2 !
declare res cursor for 
SELECT * FROM dual WHERE 1 = 1 AND (1,1) IN ((1,2), (1,1))!
fetch res into :a !
declare res1 cursor for SELECT * FROM artikel
  WHERE (nummer, 0, 'Pos-Test1')
          IN (SELECT nummer, 0, name FROM artikel)!
FETCH res1 INTO :a, :b, :c!
declare res2 cursor for SELECT * FROM artikel
  WHERE (nummer, 0, 'Kein NameKein Name')
          IN ((3, nummer-3,
               'Kein Name' || SUBSTR ('Kein Name mehr frei', 1, 9)),
              (4, 3-(5-2),       name||name))!
FETCH res2 INTO :a, :b, :c!
declare res3 cursor for 
SELECT * FROM dual WHERE 1 = 1 AND (1,1) = any ((1,2), (1,1))!
fetch res3 into :a !
declare res4 cursor for 
SELECT * FROM dual WHERE 1 = 1 AND 1 = any (3-2, 4 div 2) !
fetch res4 into :a !
declare res5 cursor for SELECT COUNT (*) FROM dual
  WHERE ('Hallo', 'Hello World') <> ALL
     (SELECT 'Hallo Welt', 'Hello' FROM dual)!
FETCH res5 into :a !
declare res5a cursor for SELECT COUNT (*) FROM dual
  WHERE ('Hallo', 'Hello World') < ALL
     (SELECT 'Hallo Welt', 'Hello' FROM dual)!
FETCH res5a into :a !
commit !
select * into :a, :b, :c, :d from artikel where nummer = 4 !
update artikel set name = (select max(name) from artikel)
    where nummer = 4 !
select * into :a, :b, :c, :d from artikel where nummer = 4 !
rollback release !

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

FILE connect (kern test!

CREATE TABLE tt_in (nummer FIXED,
 a FIXED,b  FIXED, c FIXED, d FIXED, e FIXED, f FIXED, g FIXED, h FIXED,
 i FIXED,a0 FIXED,aa FIXED,ab FIXED,ac FIXED,ad FIXED,ae FIXED,af FIXED,
ag FIXED,ah FIXED,ai FIXED,b0 FIXED,ba FIXED,bb FIXED,bc FIXED,bd FIXED,
be FIXED,bf FIXED,bg FIXED,bh FIXED,bi FIXED,c0 FIXED,ca FIXED,cb FIXED,
cc FIXED,cd FIXED,ce FIXED,cf FIXED,cg FIXED,ch FIXED,ci FIXED,d0 FIXED)!
* Man muss viel schreiben, um 2048 Stackentries vollzukriegen...
SELECT nummer,
 a+ b+ c+ d+ e+ f+ g+ h+ i+a0,  b+ c+ d+ e+ f+ g+ h+ i+a0+aa,
 c+ d+ e+ f+ g+ h+ i+a0+aa+ab,  d+ e+ f+ g+ h+ i+a0+aa+ab+ac,
 e+ f+ g+ h+ i+a0+aa+ab+ac+ad,  f+ g+ h+ i+a0+aa+ab+ac+ad+ae,
 g+ h+ i+a0+aa+ab+ac+ad+ae+af,  h+ i+a0+aa+ab+ac+ad+ae+af+ag,
 i+a0+aa+ab+ac+ad+ae+af+ag+ah, a0+aa+ab+ac+ad+ae+af+ag+ah+ai,
aa+ab+ac+ad+ae+af+ag+ah+ai+b0, ab+ac+ad+ae+af+ag+ah+ai+b0+ba,
ac+ad+ae+af+ag+ah+ai+b0+ba+bb, ad+ae+af+ag+ah+ai+b0+ba+bb+bc,
ae+af+ag+ah+ai+b0+ba+bb+bc+bd, af+ag+ah+ai+b0+ba+bb+bc+bd+be,
ag+ah+ai+b0+ba+bb+bc+bd+be+bf, ah+ai+b0+ba+bb+bc+bd+be+bf+bg,
ai+b0+ba+bb+bc+bd+be+bf+bg+bh, b0+ba+bb+bc+bd+be+bf+bg+bh+bi,
ba+ab+ac+ad+ae+af+ag+ah+ai+b0, bb+ac+ad+ae+af+ag+ah+ai+b0+ba,
bc+ad+ae+af+ag+ah+ai+b0+ba+bb, bd+ae+af+ag+ah+ai+b0+ba+bb+bc,
be+af+ag+ah+ai+b0+ba+bb+bc+bd, bf+ag+ah+ai+b0+ba+bb+bc+bd+be,
bg+ah+ai+b0+ba+bb+bc+bd+be+bf, bh+ai+b0+ba+bb+bc+bd+be+bf+bg,
bi+b0+ba+bb+bc+bd+be+bf+bg+bh, c0+ba+bb+bc+bd+be+bf+bg+bh+bi,
ca+bb+bc+bd+be+bf+bg+bh+bi+c0, cb+bc+bd+be+bf+bg+bh+bi+c0+ca,
cc+bd+be+bf+bg+bh+bi+c0+ca+cb, cd+be+bf+bg+bh+bi+c0+ca+cb+cc,
ce+bf+bg+bh+bi+c0+ca+cb+cc+cd, cf+bg+bh+bi+c0+ca+cb+cc+cd+ce,
cg+bh+bi+c0+ca+cb+cc+cd+ce+cf, ch+bi+c0+ca+cb+cc+cd+ce+cf+cg,
ci+c0+ca+cb+cc+cd+ce+cf+cg+ch, d0+ca+cb+cc+cd+ce+cf+cg+ch+ci,
 a+ b+ c+ d+ e+ f+ g+ h+ i+a0,  b+ c+ d+ e+ f+ g+ h+ i+a0+aa,
 c+ d+ e+ f+ g+ h+ i+a0+aa+ab,  d+ e+ f+ g+ h+ i+a0+aa+ab+ac,
 e+ f+ g+ h+ i+a0+aa+ab+ac+ad,  f+ g+ h+ i+a0+aa+ab+ac+ad+ae,
 g+ h+ i+a0+aa+ab+ac+ad+ae+af,  h+ i+a0+aa+ab+ac+ad+ae+af+ag,
 i+a0+aa+ab+ac+ad+ae+af+ag+ah, a0+aa+ab+ac+ad+ae+af+ag+ah+ai,
aa+ab+ac+ad+ae+af+ag+ah+ai+b0, ab+ac+ad+ae+af+ag+ah+ai+b0+ba,
ac+ad+ae+af+ag+ah+ai+b0+ba+bb, ad+ae+af+ag+ah+ai+b0+ba+bb+bc,
ae+af+ag+ah+ai+b0+ba+bb+bc+bd, af+ag+ah+ai+b0+ba+bb+bc+bd+be,
ag+ah+ai+b0+ba+bb+bc+bd+be+bf, ah+ai+b0+ba+bb+bc+bd+be+bf+bg,
ci+c0+ca+cb+cc+cd+ce+cf+cg+ch, d0+ca+cb+cc+cd+ce+cf+cg+ch+ci,
 a+ b+ c+ d+ e+ f+ g+ h+ i+a0,  b+ c+ d+ e+ f+ g+ h+ i+a0+aa,
 c+ d+ e+ f+ g+ h+ i+a0+aa+ab,  d+ e+ f+ g+ h+ i+a0+aa+ab+ac,
 e+ f+ g+ h+ i+a0+aa+ab+ac+ad,  f+ g+ h+ i+a0+aa+ab+ac+ad+ae,
 g+ h+ i+a0+aa+ab+ac+ad+ae+af,  h+ i+a0+aa+ab+ac+ad+ae+af+ag,
 i+a0+aa+ab+ac+ad+ae+af+ag+ah, a0+aa+ab+ac+ad+ae+af+ag+ah+ai,
aa+ab+ac+ad+ae+af+ag+ah+ai+b0, ab+ac+ad+ae+af+ag+ah+ai+b0+ba,
ac+ad+ae+af+ag+ah+ai+b0+ba+bb, ad+ae+af+ag+ah+ai+b0+ba+bb+bc,
ae+af+ag+ah+ai+b0+ba+bb+bc+bd, af+ag+ah+ai+b0+ba+bb+bc+bd+be,
ag+ah+ai+b0+ba+bb+bc+bd+be+bf, ah+ai+b0+ba+bb+bc+bd+be+bf+bg,
ai+b0+ba+bb+bc+bd+be+bf+bg+bh, b0+ba+bb+bc+bd+be+bf+bg+bh+bi,
ba+ab+ac+ad+ae+af+ag+ah+ai+b0, bb+ac+ad+ae+af+ag+ah+ai+b0+ba,
bc+ad+ae+af+ag+ah+ai+b0+ba+bb, bd+ae+af+ag+ah+ai+b0+ba+bb+bc,
be+af+ag+ah+ai+b0+ba+bb+bc+bd, bf+ag+ah+ai+b0+ba+bb+bc+bd+be,
bg+ah+ai+b0+ba+bb+bc+bd+be+bf, bh+ai+b0+ba+bb+bc+bd+be+bf+bg,
bi+b0+ba+bb+bc+bd+be+bf+bg+bh, c0+ba+bb+bc+bd+be+bf+bg+bh+bi,
ca+bb+bc+bd+be+bf+bg+bh+bi+c0, cb+bc+bd+be+bf+bg+bh+bi+c0+ca,
cc+bd+be+bf+bg+bh+bi+c0+ca+cb, cd+be+bf+bg+bh+bi+c0+ca+cb+cc,
ce+bf+bg+bh+bi+c0+ca+cb+cc+cd, cf+bg+bh+bi+c0+ca+cb+cc+cd+ce,
cg+bh+bi+c0+ca+cb+cc+cd+ce+cf, ch+bi+c0+ca+cb+cc+cd+ce+cf+cg,
ci+c0+ca+cb+cc+cd+ce+cf+cg+ch, d0+ca+cb+cc+cd+ce+cf+cg+ch+ci,
 c+ d+ e+ f+ g+ h+ i+a0+aa+ab,  d+ e+ f+ g+ h+ i+a0+aa+ab+ac,
 e+ f+ g+ h+ i+a0+aa+ab+ac+ad,  f+ g+ h+ i+a0+aa+ab+ac+ad+ae,
 g+ h+ i+a0+aa+ab+ac+ad+ae+af 
FROM tt_in WHERE nummer IN (
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,  2, 3, 4, 5, 6, 7, 8, 9,10,11,
 3, 4, 5, 6, 7, 8, 9,10,11,12,  4, 5, 6, 7, 8, 9,10,11,12,13,
 5, 6, 7, 8, 9,10,11,12,13,14,  6, 7, 8, 9,10,11,12,13,14,15,
 7, 8, 9,10,11,12,13,14,15,16,  8, 9,10,11,12,13,14,15,16,17,
 9,10,11,12,13,14,15,16,17,18, 10,11,12,13,14,15,16,17,18,19,
11,12,13,14,15,16,17,18,19,20, 12,13,14,15,16,17,18,19,20,21,
13,14,15,16,17,18,19,20,21,22, 14,15,16,17,18,19,20,21,22,23,
15,16,17,18,19,20,21,22,23,24, 16,17,18,19,20,21,22,23,24,25,
17,18,19,20,21,22,23,24,25,26, 18,19,20,21,22,23,24,25,26,27,
19,20,21,22,23,24,25,26,27,28, 20,21,22,23,24,25,26,27,28,29,
21,12,13,14,15,16,17,18,19,20, 22,13,14,15,16,17,18,19,20,21,
23,14,15,16,17,18,19,20,21,22, 24,15,16,17,18,19,20,21,22,23,
25,16,17,18,19,20,21,22,23,24, 26,17,18,19,20,21,22,23,24,25,
27,18,19,20,21,22,23,24,25,26, 28,19,20,21,22,23,24,25,26,27,
29,20,21,22,23,24,25,26,27,28, 30,21,22,23,24,25,26,27,28,29,
31,22,23,24,25,26,27,28,29,30, 32,23,24,25,26,27,28,29,30,31,
33,24,25,26,27,28,29,30,31,32, 34,25,26,27,28,29,30,31,32,33,
35,26,27,28,29,30,31,32,33,34, 36,27,28,29,30,31,32,33,34,35,
37,28,29,30,31,32,33,34,35,36, 38,29,30,31,32,33,34,35,36,37,
39,30,31,32,33,34,35,36,37,38, 40,31,32,33,34,35,36,37,38,39,
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,  2, 3, 4, 5, 6, 7, 8, 9,10,11,
 3, 4, 5, 6, 7, 8, 9,10,11,12,  4, 5, 6, 7, 8, 9,10,11,12,13,
 5, 6, 7, 8, 9,10,11,12,13,14,  6, 7, 8, 9,10,11,12,13,14,15,
 7, 8, 9,10,11,12,13,14,15,16,  8, 9,10,11,12,13,14,15,16,17,
 9,10,11,12,13,14,15,16,17,18, 10,11,12,13,14,15,16,17,18,19,
11,12,13,14,15,16,17,18,19,20, 12,13,14,15,16,17,18,19,20,21,
13,14,15,16,17,18,19,20,21,22, 14,15,16,17,18,19,20,21,22,23,
15,16,17,18,19,20,21,22,23,24, 16,17,18,19,20,21,22,23,24,25,
17,18,19,20,21,22,23,24,25,26, 18,19,20,21,22,23,24,25,26,27,
19,20,21,22,23,24,25,26,27,28, 20,21,22,23,24,25,26,27,28,29,
21,12,13,14,15,16,17,18,19,20, 22,13,14,15,16,17,18,19,20,21,
23,14,15,16,17,18,19,20,21,22, 24,15,16,17,18,19,20,21,22,23,
25,16,17,18,19,20,21,22,23,24, 26,17,18,19,20,21,22,23,24,25,
27,18,19,20,21,22,23,24,25,26, 28,19,20,21,22,23,24,25,26,27,
29,20,21,22,23,24,25,26,27,28, 30,21,22,23,24,25,26,27,28,29,
31,22,23,24,25,26,27,28,29,30, 32,23,24,25,26,27,28,29,30,31,
33,24,25,26,27,28,29,30,31,32, 34,25,26,27,28,29,30,31,32,33,
35,26,27,28,29,30,31,32,33,34, 36,27,28,29,30,31,32,33,34,35,
37,28,29,30,31,32,33,34,35,36, 38,29,30,31,32,33,34,35,36,37,
39,30,31
)!


ROLLBACK WORK RELEASE!
