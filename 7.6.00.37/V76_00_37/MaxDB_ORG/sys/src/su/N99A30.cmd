file CONNECT ( kern test!

SELECT * from dual where dummy = ((('Hallo''Welt')))!
SELECT * from dual where dummy IN ('Hallo''Welt')!
SELECT 'Hallo''Welt' "Halli""Galli" FROM dual!
FETCH!

CREATE TABLE tt ("Hallo""Welt" CHAR (20))!
INSERT INTO tt ("Hallo""Welt") VALUES ('Halli''Hallo')!
SELECT "Hallo""Welt" "Halli""Galli" FROM tt!
FETCH!
ROLLBACK WORK!

* Since the procedures in vak01 use different code for the first paired
* quotes and the following ones, here we repeat the same statements
* with more quotes than ever!

SELECT * from dual where dummy = ((('Hallo''Du''Welt')))!
SELECT * from dual where dummy IN ('Hallo''Du''Welt')!
SELECT 'Hallo''Du''Welt' "Halli""Galli""Walli" FROM dual!
FETCH!

CREATE TABLE tt ("Hallo""Du""Welt" CHAR (20))!
INSERT INTO tt ("Hallo""Du""Welt") VALUES ('Halli''Hallo''Hollo')!
SELECT "Hallo""Du""Welt" "Halli""Galli""Walli" FROM tt!
FETCH!

ROLLBACK RELEASE!
