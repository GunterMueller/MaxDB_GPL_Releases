
FILE connect (kern test SQLMODE ORACLE!

* Syntax errors!
ALTER!
ALTER SESSION!
ALTER SESSION x!
ALTER SESSION SET!
ALTER SESSION SET x!
ALTER SESSION SET nls_date_format!
ALTER SESSION SET nls_date_language!
ALTER SESSION SET nls_numeric_characters!
ALTER SESSION SET nls_currency!
ALTER SESSION SET nls_date_format x!
ALTER SESSION SET nls_date_language x!
ALTER SESSION SET nls_numeric_characters x!
ALTER SESSION SET nls_currency x!
ALTER SESSION SET nls_date_format =!
ALTER SESSION SET nls_date_language =!
ALTER SESSION SET nls_numeric_characters =!
ALTER SESSION SET nls_currency =!
ALTER SESSION SET nls_date_format = SELECT!
ALTER SESSION SET nls_date_language = SELECT!
ALTER SESSION SET nls_numeric_characters = SELECT!
ALTER SESSION SET nls_currency = SELECT!
ALTER SESSION SET nls_date_format = 'x'!
ALTER SESSION SET nls_date_language = 'x'!
ALTER SESSION SET nls_numeric_characters = 'x'!
ALTER SESSION SET nls_currency = 'x'!
ALTER SESSION SET nls_date_format = "y"!
ALTER SESSION SET nls_date_language = "y"!
ALTER SESSION SET nls_numeric_characters = "y"!
ALTER SESSION SET nls_currency = "y"!
ALTER SESSION SET nls_date_format = 'DD-MON-YY' blub!
ALTER SESSION SET nls_date_language = eng blub!
ALTER SESSION SET nls_numeric_characters = '.,' blub!
ALTER SESSION SET nls_currency = '$' blub!
SELECT * FROM v$nls_parameters ORDER BY parameter!
FETCH INTO :A,:B!
*Semantic errors!
ALTER SESSION SET nls_date_format =
   '123456789012345678901234567890123456789012345678901234567890'!
ALTER SESSION SET nls_date_format = 'xxxxxxx'!
ALTER SESSION SET nls_date_language = suaheli!
ALTER SESSION SET nls_date_language = d!
ALTER SESSION SET nls_date_language = de!
ALTER SESSION SET nls_numeric_characters = '.'!
ALTER SESSION SET nls_numeric_characters = '.,;'!
ALTER SESSION SET nls_numeric_characters = '..'!
ALTER SESSION SET nls_numeric_characters = '<>'!
ALTER SESSION SET nls_numeric_characters = '23'!
ALTER SESSION SET nls_currency = '<>'!
ALTER SESSION SET nls_currency = 'abcdef'!
ALTER SESSION SET nls_currency = '123'!
ALTER SESSION SET nls_currency = '<>'!
SELECT * FROM v$nls_parameters ORDER BY parameter!
FETCH INTO :A,:B!

* Empty format!
ALTER SESSION SET nls_date_format = ''!
SELECT SYSDATE INTO :a FROM dual!

* A kind of german settings!
ALTER SESSION SET nls_date_format        = 'dd. Month yyyy'!
ALTER SESSION SET nls_date_language      = german!
ALTER SESSION SET nls_numeric_characters = ', '!
ALTER SESSION SET nls_currency           = 'DM'!
SELECT * FROM v$nls_parameters ORDER BY parameter!
FETCH INTO :A,:B!
 
*Output!
ALTER SESSION SET nls_date_format = 'dd.mm.yyyy'!
SELECT "VALUE" INTO :a FROM v$nls_parameters 
   where parameter = 'NLS_DATE_FORMAT'!
SELECT SYSDATE INTO :A FROM dual!
SELECT TO_CHAR (SYSDATE) INTO :A FROM dual!

CREATE TABLE hallo (d date)!
INSERT INTO hallo VALUES (SYSDATE)!
SELECT * INTO :A FROM hallo!
INSERT INTO hallo VALUES ('14.10.1958')!

SELECT 'Text to make sed_sut happy', * FROM  hallo!
FETCH INTO :A, :b!
ALTER SESSION SET nls_date_format = 'yyyymmdd'!
SELECT * FROM hallo!
FETCH INTO :A!

DROP TABLE hallo!
COMMIT WORK RELEASE!

* Now testing the way in, too...
FILE connect (kern test SQLMODE ORACLE!
CREATE TABLE tt (name CHAR(20) PRIMARY KEY, geburt DATE)!
INSERT INTO tt VALUES ('Christian', '14-OCT-58')!
DELETE FROM TT WHERE name = 'Christian' !
ALTER SESSION SET NLS_DATE_FORMAT = 'dd.mm.yyyy'!
INSERT INTO tt VALUES ('Christian', '14.10.1958')!
INSERT INTO tt VALUES ('Werner',    '01-JUN-54')!
INSERT INTO tt VALUES ('Werner',    '01.06.1954')!
INSERT INTO tt VALUES ('Elke',      TO_DATE ('25-MAY-59'))!
INSERT INTO tt VALUES ('Elke',      TO_DATE ('25.05.1959'))!
COMMIT WORK!
PARS_THEN_EX!
DATA!
INSERT INTO tt VALUES ('Oliver', :geburt)!
c 9 '18-JUN-68'

INSERT INTO tt VALUES ('Oliver', :geburt)!
c 10 '18.06.1968'

INSERT INTO tt VALUES (:name, :geburt)!
c 20 'Carola'
c 9  '23-JAN-62'

INSERT INTO tt VALUES (:name, :geburt)!
c 20 'Carola'
c 9  '23.01.1962'

SELECT * FROM tt!
FETCH INTO :name, :geburt!

PARS_EXECUTE!
SELECT name FROM tt WHERE geburt = '01.06.1954'!
FETCH INTO :A!

PARS_THEN_EX!
SELECT name FROM tt WHERE geburt = '01.06.1954'!
FETCH INTO :A!

SELECT name FROM tt WHERE geburt = :geburt!
c 9 '01-JUN-54'

SELECT name FROM tt WHERE geburt = :geburt!
c 9 '01.06.1954'

FETCH INTO :A!

SELECT * FROM tt WHERE geburt = '01.06.1954'!
FETCH INTO :A, :B!
SELECT * FROM tt WHERE geburt = '01'||'.'||'06'||'.'||'1954'!
FETCH INTO :A, :B!
SELECT * FROM tt WHERE substr (geburt, 7) = '1958'!
FETCH INTO :A, :B!

NODATA!
PARS_EXECUTE!
COMMIT WORK RELEASE!

* Is this working in a view, too?
FILE connect (kern test SQLMODE ORACLE!

CREATE VIEW tva AS
  SELECT name || ' geboren am ' || geburt FROM tt!
CREATE VIEW tvb AS
  SELECT name || ' geboren am ' || TO_CHAR (geburt) FROM tt!
CREATE VIEW tvc AS
  SELECT name FROM tt WHERE SUBSTR (geburt, 7) = '1958'!

SELECT * FROM tva!
FETCH INTO :A!
SELECT * FROM tvb!
FETCH INTO :A!
SELECT * FROM tvc!

ALTER SESSION SET NLS_DATE_FORMAT = 'dd. fmMonth yy'!

SELECT * FROM tva!
FETCH INTO :A!
SELECT * FROM tvb!
FETCH INTO :A!
SELECT * FROM tvc!

ALTER SESSION SET NLS_LANGUAGE = GER!

SELECT * FROM tva!
FETCH INTO :A!
SELECT * FROM tvb!
FETCH INTO :A!
SELECT * FROM tvc!

ALTER SESSION SET NLS_LANGUAGE = ENG!

SELECT * FROM tvb!
FETCH INTO :A!
SELECT * FROM tvc!

ALTER SESSION SET NLS_DATE_LANGUAGE = GER!

SELECT * FROM tva!
FETCH INTO :A!
SELECT * FROM tvb!
FETCH INTO :A!
SELECT * FROM tvc!

ALTER SESSION SET NLS_DATE_FORMAT = 'dd.mm.yyyy'!

SELECT * FROM tva!
FETCH INTO :A!
SELECT * FROM tvb!
FETCH INTO :A!
SELECT * FROM tvc!
FETCH INTO :A!

DROP VIEW tva!
DROP VIEW tvb!
DROP VIEW tvc!
COMMIT WORK RELEASE!

* Funktioniert der ALTER SESSION Befehl auch parse/execute getrennt?
FILE connect (kern test SQLMODE ORACLE!
SELECT geburt FROM tt WHERE name = 'Christian'!
FETCH INTO :A!
ALTER SESSION SET NLS_DATE_FORMAT = 'dd.mm.yyyy'!
PARS_THEN_EX!
SELECT geburt FROM tt WHERE name = 'Christian'!
FETCH INTO :A!
PARSING!
ALTER SESSION SET NLS_DATE_FORMAT = 'dd.mm.yy'!
PARS_THEN_EX!
SELECT geburt FROM tt WHERE name = 'Christian'!
FETCH INTO :A!
EXECUTING!
ALTER!
1
PARS_THEN_EX!
SELECT geburt FROM tt WHERE name = 'Christian'!
FETCH INTO :A!
PARS_EXECUTE!

* Nun ein paar Tests, die eigentlich in ORADATE getestet werden sollten.
* Aber da wir sie spaeter mit NLS_DATE_FORMAT wiederholen, bleiben sie hier!
ALTER SESSION SET NLS_DATE_FORMAT = 'dd.mm.yyyy'!
SELECT TO_DATE ('12.12.1996',  'dd.mm.yyyy') INTO :a FROM dual!
SELECT TO_DATE ('1201201996',  'dd.mm.yyyy') INTO :a FROM dual!
SELECT TO_DATE ('12.12.1996','fxdd.mm.yyyy') INTO :a FROM dual!
SELECT TO_DATE ('1201201996','fxdd.mm.yyyy') INTO :a FROM dual!

SELECT TO_DATE ('12xx12xx1996',  'dd"xx"mm"xx"yyyy') INTO :a FROM dual!
SELECT TO_DATE ('120012001996',  'dd"xx"mm"xx"yyyy') INTO :a FROM dual!
SELECT TO_DATE ('12xx12xx1996','fxdd"xx"mm"xx"yyyy') INTO :a FROM dual!
SELECT TO_DATE ('120012001996','fxdd"xx"mm"xx"yyyy') INTO :a FROM dual!

* Bei TO_DATE Umwandlungen werden Punktuationszeichen im Formatstring
* ignoriert. Bei O7 wird dafuer allerdings jeweils ein Zeichen in der
* Eingabe ignoriert. Dadurch kann es mitunter zu voellig anderen
* Ergebnissen kommen!
*  EINGABE      FORMAT          ORACLE6         ORACLE7
*  ----------------------------------------------------------------
*  11011995     dd-mm-yyyy      11.01.1995      11.11.995
*  123123       --dd..yyyy..    12.??.3123      Input too short
*  122123123123 --dd..yyyy..    Input too long  21.??.1231
*  11121995     ddmmyyyy-       11.12.1995      Input too short
*  111219951    ddmmyyyy-       Input too long  11.12.1995
* Besser, wir verzichten auf die Ausgabe des aktuellen Monats,
* zur Vermeidung von Differenzen...

SELECT TO_DATE ('11011995',       'dd-mm-yyyy')     INTO :a FROM dual!
SELECT TO_DATE ('01123123',       'mm--dd..yyyy..') INTO :a FROM dual!
SELECT TO_DATE ('01122123123123', 'mm--dd..yyyy..') INTO :a FROM dual!
SELECT TO_DATE ('11121995',       'ddmmyyyy-')      INTO :a FROM dual!
SELECT TO_DATE ('111219951',      'ddmmyyyy-')      INTO :a FROM dual!

* Geht das ganze auch mit NLS_DATE_FORMAT?

ALTER SESSION SET NLS_DATE_FORMAT = 'dd.mm.yyyy'!
SELECT TO_CHAR (TO_DATE('12.12.1996'),  'dd.mm.yyyy') INTO :a FROM dual!
ALTER SESSION SET NLS_DATE_FORMAT = 'dd.mm.yyyy'!
SELECT TO_CHAR (TO_DATE('1201201996'),  'dd.mm.yyyy') INTO :a FROM dual!
ALTER SESSION SET NLS_DATE_FORMAT = 'fxdd.mm.yyyy'!
SELECT TO_CHAR (TO_DATE('12.12.1996'),'dd.mm.yyyy') INTO :a FROM dual!
ALTER SESSION SET NLS_DATE_FORMAT = 'fxdd.mm.yyyy'!
SELECT TO_CHAR (TO_DATE('1201201996'),  'dd.mm.yyyy') INTO :a FROM dual!

ALTER SESSION SET NLS_DATE_FORMAT = 'dd"xx"mm"xx"yyyy'!
SELECT TO_CHAR (TO_DATE('12xx12xx1996'),'dd.mm.yyyy') INTO :a FROM dual!
ALTER SESSION SET NLS_DATE_FORMAT = 'dd"xx"mm"xx"yyyy'!
SELECT TO_CHAR (TO_DATE('120012001996'),'dd.mm.yyyy') INTO :a FROM dual!
ALTER SESSION SET NLS_DATE_FORMAT = 'fxdd"xx"mm"xx"yyyy'!
SELECT TO_CHAR (TO_DATE('12xx12xx1996'),'dd.mm.yyyy') INTO :a FROM dual!
ALTER SESSION SET NLS_DATE_FORMAT = 'fxdd"xx"mm"xx"yyyy'!
SELECT TO_CHAR (TO_DATE('120012001996'),'dd.mm.yyyy') INTO :a FROM dual!

ALTER SESSION SET NLS_DATE_FORMAT = 'dd-mm-yyyy'!
SELECT TO_CHAR (TO_DATE('11011995'),     'dd.mm.yyyy') INTO :a FROM dual!
ALTER SESSION SET NLS_DATE_FORMAT = '--dd..yyyy..'!
SELECT TO_CHAR (TO_DATE('123123'),       'dd.yyyy') INTO :a FROM dual!
SELECT TO_CHAR (TO_DATE('122123123123'), 'dd.yyyy') INTO :a FROM dual!
ALTER SESSION SET NLS_DATE_FORMAT = 'ddmmyyyy-'!
SELECT TO_CHAR (TO_DATE('11121995'),     'dd.mm.yyyy') INTO :a FROM dual!
SELECT TO_CHAR (TO_DATE('111219951'),    'dd.mm.yyyy') INTO :a FROM dual!

DROP TABLE tt!
COMMIT WORK RELEASE!
