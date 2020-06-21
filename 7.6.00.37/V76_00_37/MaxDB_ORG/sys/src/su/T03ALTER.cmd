*   *ID* T03ALTER DBCMD    changed on 1992-09-28-13.28.34 by ADMIN     *
 ***************************
 ***   T 0 3 A L T E R   ***
 ***************************!
COMMIT WORK !
CREATE TABLE kern.personal
  (nummer FIXED (4,0) key RANGE BETWEEN 10 AND 4000,
    nachname CHAR (12) not null RANGE BETWEEN 'A' AND 'T',
   jahrgang FIXED (2) not null RANGE BETWEEN 17 AND 67,
   kinder FIXED (2,0) not null RANGE IN (0,1,2,3,4,5,6,12),
   abtleiter CHAR (12) RANGE
              IN ('TISCHLEREI','SETZEREI','BUCHHALTUNG')) !
COMMIT WORK !
ALTER TABLE kern.personal
   DROP CONSTRAINT nachname !
ALTER TABLE kern.personal
   ADD CONSTRAINT nachname CHECK nachname IN ( 'ADAM', 'MUELLER' ) !
ALTER TABLE kern.personal
   ALTER CONSTRAINT kinder CHECK kinder BETWEEN 0 AND 12 !
COMMIT WORK !
LOCK TABLE kern.personal IN EXCLUSIVE MODE !
INSERT INTO kern.personal
   SET nummer = 50,
       nachname = 'Milster',
       kinder = 2,
       jahrgang = 30  !
INSERT INTO kern.personal
   SET nummer = 51,
       nachname = 'Richter',
       kinder = 0,
       jahrgang = 28,
       abtleiter = 'Buchhandlung' !
INSERT INTO kern.personal
   SET nummer = 52,
       nachname = 'Billmann',
       kinder  TO NULL,
       jahrgang = 41  !
INSERT INTO kern.personal
   SET nummer = 53,
       nachname TO NULL,
       kinder = 6,
       jahrgang = 53  !
COMMIT WORK !
file sel_cols_1tab ( KERN PERSONAL !
ALTER TABLE kern.personal
   COLUMN nachname RANGE BETWEEN 'C' AND 'T' !
COMMIT WORK !
LOCK TABLE kern.personal IN EXCLUSIVE MODE !
UPDATE OF personal
   SET nachname = 'Cillmann'
   KEY nummer = 52 !
COMMIT WORK !
ALTER TABLE kern.personal
   COLUMN nachname RANGE BETWEEN  'C' AND 'T'  !
ALTER TABLE kern.personal
   COLUMN jahrgang RANGE BETWEEN 28.0 AND 67   !
ALTER TABLE kern.personal
   COLUMN jahrgang RANGE BETWEEN 27.9 AND 67   ! 
ALTER TABLE kern.personal
   COLUMN kinder   RANGE IN ( 0,1,2,3,4,5,6,7,8,9,10,11,12 ) !
COMMIT WORK !
INSERT INTO kern.personal
   SET nummer = 54,
       nachname = 'Dittmer',
       kinder = 10,
       jahrgang = 32  !
COMMIT WORK !
ALTER TABLE kern.personal
   COLUMN abtleiter RANGE IN ( 'Tischlerei', 'Setzerei' ) !
ALTER TABLE kern.personal
   COLUMN abtleiter RANGE IN ( 'Tischlerei', 'Buchhaltung' ) !
COMMIT WORK !
DECLARE erg CURSOR FOR 
SELECT nummer, nachname, jahrgang, kinder, abtleiter
   FROM kern.personal  !
FETCH erg !
CLOSE erg !
file sel_cols_1tab ( KERN PERSONAL !
ROLLBACK WORK RELEASE !
file connect ( kern test !
CREATE TABLE kern.abt
   ( name CHAR (6) KEY,
     nummer FIXED (6),
     arbeiter FIXED (3) RANGE BETWEEN 10 AND 999,
     num2 FIXED (4) NOT NULL RANGE IN ( 1000, 8000, 5300 ),
     ort CHAR (12) RANGE IN ( 'BERLIN', 'MUENCHEN', 'BONN' )) !
COMMIT WORK !
file sel_cols_1tab ( KERN ABT !
LOCK TABLE kern.abt IN EXCLUSIVE MODE !
INSERT INTO kern.abt
   SET name = 'PERSON',
       num2 = 5300,
       nummer = 3500,
       ort = 'BONN'  !
INSERT INTO kern.abt
   SET name = 'VERWAL',
       num2 = 5300,
       arbeiter = 10,
       ort = 'BONN'  !
INSERT INTO kern.abt
   SET name = 'WERKST',
       num2 = 1000,
       ort = 'BERLIN' !
COMMIT WORK !
ALTER TABLE t
   COLUMN a RANGE IN ( 'AA', 'BB' ) !
ALTER TABLE kern.abt
   COLUMN name RANGE !
ALTER TABLE kern.abt
   COLUMN num1 RANGE IN ( 1000, 8000, 5300, 7000 ) !
ALTER TABLE kern.abt
   COLUMN ort  RANGE BETWEEN 'BERLIN' AND 'BONN' ! 
ALTER TABLE kern.abt
   COLUMN num2 RANGE IN ( 1000, 8000, 9000, 5300 )  !
ALTER TABLE kern.abt
   COLUMN arbeiter RANGE BETWEEN 20 AND 990 ! 
ALTER TABLE kern.abt
   COLUMN ort  RANGE IN ('BERLIN', 'MUENCHEN') ! 
ALTER TABLE kern.abt
   COLUMN arbeiter RANGE BETWEEN NULL AND 999 !
COMMIT WORK !
 *** mehrere Felder duerfen nicht in einem Befehl geaendert werden !
ALTER TABLE kern.abt
   COLUMN arbeiter RANGE BETWEEN NULL AND 900,
   COLUMN nummer RANGE IN (3500, 5400, 7200 )  ! 
COMMIT WORK !
 *** Grenzen der Enum-Werte=erweiterbarkeit abpruefen !
CREATE TABLE kern.kurz
 ( k1 CHAR (4) KEY ,
 einziges_feld FIXED (3) NOT NULL RANGE IN (8,100,110,120,130,200,220) )!
COMMIT WORK !
 *** muesste klappen, da er 100 verchieben kann !
ALTER TABLE kern.kurz
   COLUMN einziges_feld RANGE BETWEEN (8,10,15,20,25,30,35,40,45,50,55,60,
                                         65,70,75,80,85,100,110,120,130,200,
                                         220 )                    !
ALTER TABLE kern.kurz
   COLUMN einziges_feld RANGE IN (8,100,110,120,130,200,220 ) !
COMMIT WORK !
INSERT INTO kern.kurz
   SET k1 = 'A',
       einziges_feld = 100  !
 *** duerfte nicht klappen, weil er 100 nicht verschieben kann !
COMMIT WORK !
ALTER TABLE kern.kurz
   COLUMN einziges_feld RANGE IN (8,10,15,20,25,30,35,40,45,50,55,60,65,70,75,
                                    80,85,100,110,120,130,200,220 )     !
COMMIT WORK !
 *** muesste gerade noch passen !
ALTER TABLE kern.kurz
   COLUMN einziges_feld RANGE IN (8,10,15,20,25,30,35,40,45,50,55,60,65,70,75,
                                    80,100,110,120,130,200,220 ) !
COMMIT WORK!
 *** Ueberpruefung,ob neue kleinste und groesste Werte eingefuegt
 *** werden koennen!
INSERT INTO kern.kurz
   SET k1 = 'B',
       einziges_feld = 8  !
INSERT INTO kern.kurz
   SET k1 = 'C',
       einziges_feld = 220 !
COMMIT WORK !
ALTER TABLE kern.kurz COLUMN einziges_feld
   RANGE IN ( 1,2,3,4,5,6,7,8,9,100,110,120,130,200,220) !
ALTER TABLE kern.kurz
   COLUMN einziges_feld RANGE IN (8,100,110,120,130,200,220,230,240,250)!
COMMIT WORK !
file sel_tab_owner ( KERN !
ROLLBACK WORK RELEASE !
