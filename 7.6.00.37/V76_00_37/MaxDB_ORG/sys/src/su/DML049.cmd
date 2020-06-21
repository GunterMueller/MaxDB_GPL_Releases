connect hu identified by hu sqlmode ansi !
     INSERT INTO TEMP_S
             SELECT EMPNUM,GRADE,CITY
             FROM STAFF
             WHERE GRADE > 11 !

     INSERT INTO STAFF1
             SELECT *
             FROM STAFF ! 

      INSERT INTO WORKS1
             SELECT *
             FROM WORKS !

     INSERT INTO STAFF4
             SELECT *
             FROM STAFF !

     INSERT INTO PROJ1
             SELECT *
             FROM PROJ !

     DECLARE TEN CURSOR
             FOR SELECT STAFF.EMPNUM,PROJ.PNUM,WORKS.HOURS,
                        STAFF3.GRADE,STAFF4.CITY,WORKS1.HOURS,
                        TEMP_S.GRADE,PROJ1.PNUM,STAFF1.GRADE,
                        UPUNIQ.COL2
                 FROM   STAFF,PROJ,WORKS,STAFF3,STAFF4,WORKS1,
                        TEMP_S,PROJ1,STAFF1,UPUNIQ
                 WHERE  STAFF.EMPNUM = WORKS.EMPNUM    AND
                        PROJ.PNUM = WORKS.PNUM         AND
                        STAFF3.EMPNUM = WORKS.EMPNUM   AND
                        STAFF4.EMPNUM = WORKS.EMPNUM   AND
                        WORKS1.EMPNUM = WORKS.EMPNUM   AND
                        WORKS1.PNUM = WORKS.PNUM       AND
                        TEMP_S.EMPNUM = WORKS.EMPNUM   AND
                        PROJ1.PNUM = WORKS.PNUM        AND
                        STAFF1.EMPNUM = WORKS.EMPNUM   AND
                        UPUNIQ.COL2 = 'A'
                 ORDER BY STAFF.EMPNUM, PROJ.PNUM !

     FETCH TEN INTO :EMPNO1,:PNO1,:HOURS1,:GRADE1,:CITY1,:HOURS2,
                             :GRADE2,:PNO2,:GRADE4,:COLUM2 !
     
commit work release !
