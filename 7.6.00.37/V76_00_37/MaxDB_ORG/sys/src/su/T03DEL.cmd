*   *ID* T03DEL   DBCMD    changed on 1992-09-28-13.28.35 by ADMIN     *
 ***************************
 ***   T 0 3 D E L       ***
 ***************************!
COMMIT WORK !
CREATE TABLE personal
       ( nummer FIXED (4,0)  key,
         nachname CHAR (12) not null RANGE BETWEEN 'A' AND 'T',
         jahrgang FIXED (2) not null RANGE BETWEEN 17 AND 67,
         kinder FIXED (2,0) not null RANGE IN (0,1,2,3,4,5,6,12),
         abt_leiter_v   CHAR (12) 
                     range   IN ('TISCHLEREI','SETZEREI','BUCHHALTUNG')) !
CREATE VIEW kinderlos AS SELECT *
                         FROM personal
                         WHERE ( kinder = 0 )
                         with check option !
CREATE TABLE nur_key
       ( eins_nur CHAR (3) key,
         zwei_nur FIXED (7,7) key,
         drei_nur FIXED (1) key )    !
COMMIT WORK !
LOCK TABLE personal, kinderlos, nur_key IN EXCLUSIVE MODE !
INSERT INTO personal
   SET nummer = 50,
       nachname = 'MILSTER',
       kinder = 2,
       jahrgang = 30  !
INSERT INTO kinderlos
   SET nummer = 51,
       nachname = 'RICHTER',
       jahrgang = 28,
       kinder = 0,
       abt_leiter_v   = 'BUCHHALTUNG'  !
INSERT INTO personal
   SET nummer = 52,
       nachname = 'BILLMANN',
       kinder = 3,
       jahrgang = 34  !
INSERT INTO nur_key
   SET eins_nur = 'DEF',
       zwei_nur = .7,
       drei_nur = 4     !
COMMIT WORK !
DECLARE erg CURSOR FOR 
SELECT nummer, nachname, kinder 
   FROM personal  !
FETCH personal !
CLOSE personal !
LOCK TABLE personal IN EXCLUSIVE MODE !
DELETE FROM kinderlos
   KEY nummer = 50  !
DELETE FROM kinderlos
   KEY nummer = 51  !
COMMIT WORK !
LOCK TABLE personal IN EXCLUSIVE MODE !
DECLARE RES CURSOR FOR 
SELECT nummer, nachname, kinder
   FROM personal  !
FETCH res  !
COMMIT WORK !
LOCK TABLE nur_key IN EXCLUSIVE MODE !
DELETE FROM nur_key
   KEY eins_nur = 'DEF'  !
DELETE FROM nur_key
   KEY eins_nur = 'DEF',
                zwei_nur = 0.6,
                drei_nur = 4       !
DELETE FROM nur_key
   KEY eins_nur = 'DEF',
                zwei_nur = 0.7,
                drei_nur = 4      !
COMMIT WORK !
ROLLBACK WORK RELEASE !
