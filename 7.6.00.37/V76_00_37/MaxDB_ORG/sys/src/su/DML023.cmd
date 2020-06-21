connect hu identified by hu sqlmode ansi!
pars_then_ex !
* Test0103 !
DECLARE S1 CURSOR FOR
          SELECT PNUM
          FROM PROJ
          WHERE PROJ.CITY =
             (SELECT STAFF.CITY
              FROM STAFF
              WHERE EMPNUM = 'E1') !
FETCH S1 INTO :EMPNO2 !
CLOSE S1 !
* Test0104 !
DECLARE S2 CURSOR FOR
          SELECT PNUM
          FROM PROJ
          WHERE PROJ.CITY =
             (SELECT STAFF.CITY
              FROM STAFF
              WHERE EMPNUM > 'E1') !
FETCH S2 INTO :PNO1 !
* Test0105 !
SELECT COUNT(*)
          INTO :ii
          FROM   STAFF
          WHERE  STAFF.CITY =
          (SELECT PROJ.CITY
           FROM   PROJ
           WHERE  PNUM > 'P7') !
SELECT COUNT(*)
          INTO :ii
          FROM   STAFF
          WHERE  NOT (STAFF.CITY =
                    (SELECT PROJ.CITY
                     FROM   PROJ
                     WHERE  PNUM > 'P7')) !
* Test0106 !
DECLARE S4 CURSOR FOR
          SELECT PNUM
          FROM PROJ
          WHERE CITY <> 'Deale' !
FETCH S4 INTO :PNO1 !
CLOSE S4 !
* Test0107 !
SELECT COUNT(*)
          INTO :ii
          FROM WORKS
          WHERE EMPNUM = 'E1' !
SELECT COUNT(*)
          INTO :ii
          FROM WORKS
          WHERE EMPNUM = 'E1' AND EMPNUM = 'E1 ' !
* Test0180 !
UPDATE STAFF
         SET    GRADE = NULL
         WHERE  EMPNUM='E1' OR EMPNUM='E3' OR EMPNUM='E5' !
DECLARE XX CURSOR 
     FOR SELECT EMPNUM,GRADE
         FROM   STAFF
         ORDER  BY GRADE,EMPNUM !
FETCH XX INTO :EMPNO2,:GRADE1:indic1 !
ROLLBACK WORK !
* Test0181 !
UPDATE STAFF
         SET    GRADE = NULL
         WHERE  EMPNUM='E1' OR EMPNUM='E3' OR EMPNUM='E5' !
DECLARE XXX CURSOR 
     FOR SELECT DISTINCT GRADE
         FROM   STAFF
         ORDER  BY GRADE !
FETCH XXX INTO :GRADE1:indic1 !
pars_execute !
ROLLBACK WORK Release!
