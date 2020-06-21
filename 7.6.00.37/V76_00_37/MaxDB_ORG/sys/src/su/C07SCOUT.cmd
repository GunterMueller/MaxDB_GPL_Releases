*   *ID* C07SCOUT DBCMD    changed on 1992-09-28-13.23.13 by ADMIN     *
*CS*
*
*  test mit einem selbst-referenzierenden link, ob auch wirklich alle
*  von einem DELETE betroffenen zeilen geloescht bzw. veraendert werden.
*  dabei werden die DELETE-operationen von verschiedenen 'ebenen' der
*  bestehenden tabelle aus ausgefuehrt.
*
*  ab 2.4.06 korrekt. vorher wurden bei ON DLETE CASCADE nur die direkt
*  abhaengigen zeilen geloescht.
*
file connect ( kern test !
create domain d_gnr   fixed(5)!
create domain d_gnr_d fixed(5) DEFAULT 99999!
CREATE TABLE g ( gnr    d_gnr key,
                 gna    CHAR(20) NOT NULL,
                 in_gnr d_gnr)!
ALTER TABLE G FOREIGN KEY GRUPPE (IN_GNR) REFERENCES G
            ON DELETE CASCADE !
CREATE TABLE gd ( gnr    d_gnr key,
                  gna    CHAR(20) NOT NULL,
                  in_gnr d_gnr_d )!
ALTER TABLE GD FOREIGN KEY GRUPPE (IN_GNR) REFERENCES GD
ON DELETE set default !
INSERT G VALUES (    0, 'RjB',                NULL)!
INSERT G VALUES (    1, 'R.d.P.',             NULL)!
INSERT G VALUES (   10, 'DPB',                   0)!
INSERT G VALUES (   11, 'BdP',                   1)!
INSERT G VALUES (  111, 'st Burgund',           11)!
INSERT G VALUES (  112, 'st Kimbern',           11)!
INSERT G VALUES ( 1111, 's Nelke von Semur',   111)!
INSERT G VALUES ( 1112, 's Loewe von Geldern', 111)!
INSERT G VALUES (   12, 'DPSG',                  1)!
INSERT G VALUES (   13, 'PS Grauer Reiter',      0)!
INSERT G VALUES (  131, 'st Midgard',           13)!
INSERT G VALUES (  100, 'Gau Wartburg',         10)!
INSERT G VALUES (  500, 'st Goldener Drache',   10)!
INSERT G VALUES ( 2000, 'st Alemannen',        200)!
INSERT G VALUES (  200, 'Ring Hohenstaufen',    10)!
INSERT G VALUES (  300, 'Stamm Gawan',          10)!
INSERT G VALUES ( 2000, 'st Alemannen',        200)!
INSERT G VALUES ( 1100, 'st O.v.Wolkenstein',  100)!
INSERT G VALUES ( 1000, 'js Parzival',         100)!
INSERT G VALUES ( 1010, 'st W.v.Eschenbach',  1000)!
INSERT G VALUES (10101, 't Lohengrin',        1010)!
INSERT G VALUES (10102, 't Gralsburg',        1010)!
INSERT G VALUES (10110, 'h Gralsritter',     10102)!
INSERT G VALUES (10111, 'h Gralsknappen',    10102)!
INSERT G VALUES (11010, 't Ivanhoe'   ,       1100)!
INSERT G VALUES (11011, 'h AH Wolkenstein',   1100)!
INSERT G VALUES (11012, 'h Hauenstein',      11010)!
INSERT G VALUES (11013, 'h Greiffenstein',   11010)!
COMMIT WORK !
*
select * from g !
fetch!
INSERT GD
   SELECT gnr, gna, in_gnr
     FROM G where gnr < (select max(gnr) from g) !
alter table gd column in_gnr drop default !
alter table gd column in_gnr default null !
INSERT GD
   SELECT gnr, gna, in_gnr
     FROM G where gnr < (select max(gnr) from g) !
select * from gd !
fetch!
rollback !
*
INSERT GD
   SELECT GNR, GNA, 99999
     FROM G WHERE IN_GNR IS NULL !
INSERT GD
   SELECT GNR, GNA, IN_GNR
     FROM G WHERE IN_GNR IS NOT NULL !
SELECT * FROM G !
FETCH!
SELECT * FROM GD !
FETCH!
COMMIT WORK!
*--------------------------------------------------
DELETE FROM G WHERE GNR = 0 !
SELECT * FROM G !
FETCH!
rollback work!
*
DELETE FROM G WHERE GNR < 10 !
SELECT * FROM G !
FETCH!
rollback work!
*
DELETE FROM G WHERE GNR between 10 and 30 !
SELECT * FROM G !
FETCH!
rollback work!
*
DELETE FROM G WHERE GNR = 10 !
SELECT * FROM G !
FETCH!
rollback work!
*
DELETE FROM G WHERE GNR = 100 !
SELECT * FROM G !
FETCH!
rollback work!
*
DELETE FROM G WHERE GNR between 100 and 300 !
SELECT * FROM G !
FETCH!
rollback work!
*
DELETE FROM G WHERE GNR = 1000 !
SELECT * FROM G !
FETCH!
rollback work!
*
DELETE FROM G WHERE GNR = 1100 !
SELECT * FROM G !
FETCH!
rollback work!
*
ALTER TABLE G DROP FOREIGN KEY GRUPPE !
ALTER TABLE G FOREIGN KEY GR (IN_GNR) REFERENCES G
ON DELETE SET NULL    !
DELETE FROM G WHERE GNR = 0!
SELECT * FROM G !
FETCH!
rollback work!
*
ALTER TABLE G DROP FOREIGN KEY GRUPPE !
ALTER TABLE G FOREIGN KEY GR (IN_GNR) REFERENCES G
ON DELETE SET NULL    !
DELETE FROM G WHERE GNR < 10!
SELECT * FROM G !
FETCH!
rollback work!
*
ALTER TABLE G DROP FOREIGN KEY GRUPPE !
ALTER TABLE G FOREIGN KEY GR (IN_GNR) REFERENCES G
ON DELETE SET NULL    !
DELETE FROM G WHERE GNR = 10!
SELECT * FROM G !
FETCH!
rollback work!
*
ALTER TABLE G DROP FOREIGN KEY GRUPPE !
ALTER TABLE G FOREIGN KEY GR (IN_GNR) REFERENCES G
ON DELETE SET NULL    !
DELETE FROM G WHERE GNR between 10 and 30 !
SELECT * FROM G !
FETCH!
rollback work!
*
ALTER TABLE G DROP FOREIGN KEY GRUPPE !
ALTER TABLE G FOREIGN KEY GR (IN_GNR) REFERENCES G
ON DELETE SET NULL    !
DELETE FROM G WHERE GNR = 100!
SELECT * FROM G !
FETCH!
rollback work!
*
ALTER TABLE G DROP FOREIGN KEY GRUPPE !
ALTER TABLE G FOREIGN KEY GR (IN_GNR) REFERENCES G
ON DELETE SET NULL    !
DELETE FROM G WHERE GNR between 100 and 300 !
SELECT * FROM G !
FETCH!
rollback work!
*
ALTER TABLE G DROP FOREIGN KEY GRUPPE !
ALTER TABLE G FOREIGN KEY GR (IN_GNR) REFERENCES G
ON DELETE SET NULL    !
DELETE FROM G WHERE GNR = 1000!
SELECT * FROM G !
FETCH!
rollback work!
*
ALTER TABLE G DROP FOREIGN KEY GRUPPE !
ALTER TABLE G FOREIGN KEY GR (IN_GNR) REFERENCES G
ON DELETE SET NULL    !
DELETE FROM G WHERE GNR = 1100!
SELECT * FROM G !
FETCH!
rollback work!
*  *  *  *  *  *  *  *  *  *  *  *  *  *
DELETE FROM GD WHERE GNR = 0 !
SELECT * FROM GD !
FETCH!
rollback work!
*
DELETE FROM GD WHERE GNR < 10 !
SELECT * FROM GD !
FETCH!
rollback work!
*
DELETE FROM GD WHERE GNR between 10 and 30 !
SELECT * FROM GD !
FETCH!
rollback work!
*
DELETE FROM GD WHERE GNR = 10 !
SELECT * FROM GD !
FETCH!
rollback work!
*
DELETE FROM GD WHERE GNR = 100 !
SELECT * FROM GD !
FETCH!
rollback work!
*
DELETE FROM GD WHERE GNR between 100 and 300 !
SELECT * FROM GD !
FETCH!
rollback work!
*
DELETE FROM GD WHERE GNR = 1000 !
SELECT * FROM GD !
FETCH!
rollback work!
*
DELETE FROM GD WHERE GNR = 1100 !
SELECT * FROM GD !
FETCH!
*
drop domain d_gnr !
drop domain d_gnr_d !
drop table g !
drop table gd !
commit work release!
