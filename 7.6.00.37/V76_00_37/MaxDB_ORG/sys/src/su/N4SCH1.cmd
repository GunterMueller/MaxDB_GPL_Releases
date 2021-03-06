* NIST SQL Test Suite, V4.0, Schema Definition, schema1.std !
* 59-byte ID !
* TEd Version # !
* date_time print !
* *************************************************************** !
* ****** THIS FILE SHOULD BE RUN UNDER AUTHORIZATION ID HU ****** !
* *************************************************************** !
* This file defines the base tables used in most of the tests. !
* This is a standard schema definition. !
  CREATE SCHEMA
    AUTHORIZATION HU !
  CREATE TABLE b_testrep
   (TESTNO   CHAR(4),
    RESULT   CHAR(4),
    TESTTYPE  CHAR(3)) !
  CREATE TABLE b_testind
    (TESTNO   CHAR(4) NOT NULL,
     PROGRAM  CHAR(6),
     AUTHID   CHAR(18),
     DESCRIPT CHAR(50),
     MCO      CHAR(3),
     MFO      CHAR(3),
     MC       CHAR(3),
     MPA      CHAR(3),
     MAD      CHAR(3),
     PCO      CHAR(3),
     PFO      CHAR(3),
     PC       CHAR(3),
     PPA      CHAR(3),
     PAD      CHAR(3),
     ISQL     CHAR(3),
     SYNVIO   CHAR(1),
     FLAGGER  CHAR(1),
     FIPMIN   CHAR(1),
     constraint c1_b_testind unique(testno)) !
  CREATE TABLE HU.ECCO (C1 CHAR(2)) !
  CREATE TABLE STAFF
   (EMPNUM   CHAR(3) NOT NULL,
    EMPNAME  CHAR(20),
    GRADE    DECIMAL(4),
    CITY     CHAR(15),
    constraint c1_staff unique(empnum)) !
  CREATE TABLE PROJ
   (PNUM     CHAR(3) NOT NULL,
    PNAME    CHAR(20),
    PTYPE    CHAR(6),
    BUDGET   DECIMAL(9),
    CITY     CHAR(15),
    constraint c1_proj unique(PNUM))  !
  CREATE TABLE WORKS
   (EMPNUM   CHAR(3) NOT NULL,
    PNUM     CHAR(3) NOT NULL,
    HOURS    DECIMAL(5),
    constraint c1_works UNIQUE(EMPNUM,PNUM)) !
  CREATE TABLE FIPS1
   (FIPS_TEST CHAR(20)) !
  CREATE TABLE STAFF1 (EMPNUM    CHAR(3) NOT NULL,
                EMPNAME  CHAR(20),
                GRADE DECIMAL(4),
                CITY   CHAR(15)) !
  CREATE TABLE PROJ1 (PNUM    CHAR(3) NOT NULL,
                PNAME  CHAR(20),
                PTYPE  CHAR(6),
                BUDGET DECIMAL(9),
                CITY   CHAR(15),
                constraint c1_proj1 unique(pnum)) !
  CREATE TABLE WORKS1(EMPNUM    CHAR(3) NOT NULL,
                PNUM    CHAR(3) NOT NULL,
                HOURS   DECIMAL(5),
                constraint c1_works1 UNIQUE(EMPNUM, PNUM)) !
  CREATE TABLE STAFF3 (EMPNUM    CHAR(3) NOT NULL,
                EMPNAME  CHAR(20),
                GRADE DECIMAL(4),
                CITY   CHAR(15)) !
  CREATE TABLE STAFF4 (EMPNUM    CHAR(3) NOT NULL,
                EMPNAME  CHAR(20),
                GRADE DECIMAL(4),
                CITY   CHAR(15)) !
  CREATE TABLE LONGINT (LONG_INT DECIMAL(15)) !
  CREATE TABLE TEMP_S
     (EMPNUM  CHAR(3),
      GRADE DECIMAL(4),
      CITY CHAR(15)) !
  CREATE TABLE TMP (T1 CHAR (10), T2 DECIMAL(2), T3 CHAR (10)) !
  CREATE TABLE AA (CHARTEST     CHAR(20)) !
  CREATE TABLE BB (CHARTEST     CHAR) !
  CREATE TABLE CC (CHARTEST     CHARACTER(20)) !
  CREATE TABLE DD (CHARTEST     CHARACTER) !
  CREATE TABLE EE (INTTEST     INTEGER) !
  CREATE TABLE FF (INTTEST     INT) !
  CREATE TABLE GG (REALTEST     REAL) !
  CREATE TABLE HH (SMALLTEST  SMALLINT) !
  CREATE TABLE II (DOUBLET  DOUBLE PRECISION) !
  CREATE TABLE JJ (FLOATTEST  FLOAT) !
  CREATE TABLE KK (FLOATTEST  FLOAT(32)) !
  CREATE TABLE LL (NUMTEST  NUMERIC(13,6)) !
  CREATE TABLE MM (NUMTEST  NUMERIC) !
  CREATE TABLE MM2 (NUMTEST NUMERIC(10)) !
  CREATE TABLE NN (NUMTEST  NUMERIC(9)) !
  CREATE TABLE OO (NUMTEST  NUMERIC(9)) !
  CREATE TABLE PP (NUMTEST  DECIMAL(13,6)) !
  CREATE TABLE QQ (NUMTEST  DECIMAL) !
  CREATE TABLE RR (NUMTEST  DECIMAL(8)) !
  CREATE TABLE SS (NUMTEST  DEC(13,6)) !
  CREATE TABLE P1 (NUMTEST  NUMERIC(1)) !
  CREATE TABLE P7 (NUMTEST  NUMERIC(7)) !
  CREATE TABLE P12 (NUMTEST  NUMERIC(12)) !
  CREATE TABLE P15 (NUMTEST  NUMERIC(15)) !
  CREATE TABLE VTABLE
         (COL1   INTEGER,
          COL2   INTEGER,
          COL3   INTEGER,
          COL4   INTEGER,
          COL5   DECIMAL(7,2)) !
  CREATE TABLE UPUNIQ (NUMKEY  DECIMAL(3) NOT NULL,
                           COL2    CHAR(2),
                           constraint c1_upuniq unique(NUMKEY)) !
  CREATE TABLE TEXT80  (TEXXT CHAR(80)) !
  CREATE TABLE TEXT132  (TEXXT CHAR(132)) !
  CREATE TABLE TEXT240  (TEXXT CHAR(240)) !
  CREATE TABLE TEXT256  (TEXXT CHAR(256)) !
  CREATE TABLE TEXT512  (TEXXT CHAR(512)) !
  CREATE TABLE TEXT1024  (TEXXT CHAR(1024)) !
 * The following tables are used to test the limitations (12-14-88) !
  CREATE TABLE T240(STR240 CHAR(240)) !
  CREATE TABLE DEC15(COL1 DECIMAL(15,7)) !
  CREATE TABLE FLO15(COL1 FLOAT(15)) !
  CREATE TABLE INT10(COL1 INTEGER, COL2 SMALLINT) !
  CREATE TABLE T100(C1 CHAR(2),C2 CHAR(2),C3 CHAR(2),C4 CHAR(2),
                C5 CHAR(2),C6 CHAR(2),C7 CHAR(2),C8 CHAR(2),
                C9 CHAR(2),C10 CHAR(2),C11 CHAR(2),C12 CHAR(2),
                C13 CHAR(2),C14 CHAR(2),C15 CHAR(2),C16 CHAR(2),
                C17 CHAR(2),C18 CHAR(2),C19 CHAR(2),C20 CHAR(2),
                C21 CHAR(2),C22 CHAR(2),C23 CHAR(2),C24 CHAR(2),
                C25 CHAR(2),C26 CHAR(2),C27 CHAR(2),C28 CHAR(2),
                C29 CHAR(2),C30 CHAR(2),C31 CHAR(2),C32 CHAR(2),
                C33 CHAR(2),C34 CHAR(2),C35 CHAR(2),C36 CHAR(2),
                C37 CHAR(2),C38 CHAR(2),C39 CHAR(2),C40 CHAR(2),
                C41 CHAR(2),C42 CHAR(2),C43 CHAR(2),C44 CHAR(2),
                C45 CHAR(2),C46 CHAR(2),C47 CHAR(2),C48 CHAR(2),
                C49 CHAR(2),C50 CHAR(2),C51 CHAR(2),C52 CHAR(2),
                C53 CHAR(2),C54 CHAR(2),C55 CHAR(2),C56 CHAR(2),
                C57 CHAR(2),C58 CHAR(2),C59 CHAR(2),C60 CHAR(2),
                C61 CHAR(2),C62 CHAR(2),C63 CHAR(2),C64 CHAR(2),
                C65 CHAR(2),C66 CHAR(2),C67 CHAR(2),C68 CHAR(2),
                C69 CHAR(2),C70 CHAR(2),C71 CHAR(2),C72 CHAR(2),
                C73 CHAR(2),C74 CHAR(2),C75 CHAR(2),C76 CHAR(2),
                C77 CHAR(2),C78 CHAR(2),C79 CHAR(2),C80 CHAR(2),
                C81 CHAR(2),C82 CHAR(2),C83 CHAR(2),C84 CHAR(2),
                C85 CHAR(2),C86 CHAR(2),C87 CHAR(2),C88 CHAR(2),
                C89 CHAR(2),C90 CHAR(2),C91 CHAR(2),C92 CHAR(2),
                C93 CHAR(2),C94 CHAR(2),C95 CHAR(2),C96 CHAR(2),
                C97 CHAR(2),C98 CHAR(2),C99 CHAR(2),C100 CHAR(2)) !
  CREATE TABLE T2000(STR110 CHAR(110),STR120 CHAR(120),
                   STR130 CHAR(130),STR140 CHAR(140),
                   STR150 CHAR(150),STR160 CHAR(160),
                   STR170 CHAR(170),STR180 CHAR(180),
                   STR190 CHAR(190),STR200 CHAR(200),
                   STR210 CHAR(210),STR216 CHAR(216)) !
  CREATE TABLE T8(COL1 CHAR(2) NOT NULL,COL2 CHAR(4) NOT NULL,
                COL3 CHAR(6) NOT NULL,COL4 CHAR(8) NOT NULL,
                COL5 CHAR(10) NOT NULL,COL6 CHAR(12) NOT NULL,
                COL7 CHAR(14),COL8 CHAR(16),
                constraint c1_t8 UNIQUE(COL1,COL2,COL3,COL4,COL5,COL6)) !
  CREATE TABLE T118(STR118 CHAR(118) NOT NULL,
    constraint c1_t118 unique(str118)) !
  CREATE TABLE T4(STR110 CHAR(110) NOT NULL,
                NUM6   NUMERIC(6) NOT NULL,
                COL3   CHAR(10),COL4 CHAR(20),
                constraint c1_t4 UNIQUE(STR110,NUM6)) !
  CREATE TABLE T12(COL1 CHAR(1), COL2 CHAR(2),
                 COL3 CHAR(4), COL4 CHAR(6),
                 COL5 CHAR(8), COL6 CHAR(10),
                 COL7 CHAR(20), COL8 CHAR(30),
                 COL9 CHAR(40), COL10 CHAR(50),
                 COL11 INTEGER, COL12 INTEGER) !
  CREATE TABLE NEXTKEY (KEYNUM INTEGER, AUTHOR CHAR(1),
                         DOLLARS INTEGER) !
  CREATE TABLE SV (NUMTEST NUMERIC(8,3)) !
  CREATE TABLE JJ_20 (FLOATTEST  FLOAT(20)) !
  CREATE TABLE PP_15 (NUMTEST  DECIMAL(15,15)) !
  CREATE TABLE PP_7  (NUMTEST  DECIMAL(15,7)) !
  CREATE TABLE P15_15 (NUMTEST  NUMERIC(15,15)) !
  CREATE TABLE P15_7 (NUMTEST  NUMERIC(15,7)) !
  CREATE TABLE t_observ
               (y_observ  NUMERIC(4),
                CITY         CHAR(10),
                MAX_TEMP     NUMERIC(5,2),
                MIN_TEMP     NUMERIC(5,2)) !
 * ************* create view statements follow ************* !
CREATE VIEW TESTINDEX AS 
    SELECT TESTNO, PROGRAM, AUTHID, DESCRIPT,
           MCO, MFO, MC, MPA, MAD, PCO, PFO, PC, PPA, PAD, ISQL,
           SYNVIO, FLAGGER, FIPMIN
    FROM b_testind !
CREATE VIEW testrep AS 
    SELECT TESTNO, RESULT, TESTTYPE 
    FROM b_testrep !
   CREATE VIEW c_observ (CITY, y_observ, MIN_C, MAX_C) 
      AS SELECT CITY, y_observ, (MIN_TEMP - 32) * 5 / 9,
                (MAX_TEMP - 32) * 5 / 9 
         FROM t_observ !
   CREATE VIEW my_observ (CITY, HIGH, LOW) 
      AS SELECT CITY, AVG(MAX_TEMP), AVG(MIN_TEMP) 
            FROM t_observ 
            GROUP BY CITY !
   CREATE VIEW extr_temp (y_observ, HIGH, LOW) 
      AS SELECT y_observ, MAX(MAX_TEMP), MIN(MIN_TEMP) 
            FROM t_observ 
            GROUP BY y_observ !
       CREATE VIEW SET_TEST (EMP1, EMP_AVG, EMP_MAX) AS 
           SELECT STAFF.EMPNUM, AVG(HOURS), MAX(HOURS) 
           FROM STAFF, WORKS 
           GROUP BY STAFF.EMPNUM !
       CREATE VIEW DUP_COL (EMP1, PNO, HOURS, HOURS_2) AS 
           SELECT EMPNUM, PNUM, HOURS, HOURS * 2 
           FROM WORKS !
  CREATE VIEW STAFFV1 
           AS SELECT * FROM STAFF 
              WHERE  GRADE >= 12 !
  CREATE VIEW STAFFV2 
           AS SELECT * FROM STAFF 
              WHERE  GRADE >= 12 
           WITH CHECK OPTION !
  CREATE VIEW STAFFV2_v 
           AS SELECT *  
              FROM   STAFFV2 
              WHERE  CITY = 'Vienna' !
  CREATE VIEW DOMAIN_v 
           AS SELECT * 
              FROM   WORKS 
              WHERE  EMPNUM = 'E1' AND HOURS = 80 
                  OR EMPNUM = 'E2' AND HOURS = 40 
                  OR EMPNUM = 'E4' AND HOURS = 20 
              WITH CHECK OPTION !
  CREATE VIEW STAFF2 
           AS SELECT *  
              FROM   STAFF 
           WITH CHECK OPTION !
  CREATE VIEW st_wo_des (NAME,COST,PROJECT) 
    AS SELECT EMPNAME,HOURS*2*GRADE,PNAME 
     FROM   PROJ,STAFF,WORKS 
     WHERE  STAFF.EMPNUM=WORKS.EMPNUM 
        AND WORKS.PNUM=PROJ.PNUM 
        AND PTYPE='Design' !
  CREATE VIEW SUBSP (EMPNUM,PNUM,HOURS) 
     AS SELECT EMPNUM,PNUM,HOURS 
        FROM   WORKS 
        WHERE  EMPNUM='E3' 
     WITH CHECK OPTION !
  CREATE VIEW TEMP_SS(EMPNUM,GRADE,CITY) 
     AS SELECT EMPNUM,GRADE,CITY 
        FROM   STAFF 
        WHERE  GRADE > 12 
     WITH CHECK OPTION !
  CREATE VIEW V_WORKS1                 
           AS SELECT * FROM WORKS        
              WHERE HOURS > 15           
           WITH CHECK OPTION !
  CREATE VIEW V_WORKS2                 
           AS SELECT * FROM V_WORKS1     
              WHERE EMPNUM = 'E1'        
                 OR EMPNUM = 'E6' !
  CREATE VIEW V_WORKS3                 
           AS SELECT * FROM V_WORKS2     
              WHERE PNUM = 'P2'          
                 OR PNUM = 'P7'          
           WITH CHECK OPTION        !
  CREATE VIEW UPDATE_v1    
            AS SELECT ALL CITY 
                     FROM PROJ !
  CREATE VIEW UPDATE_v2  
            AS SELECT HOURS, EMPNUM, PNUM 
                     FROM WORKS 
                     WHERE HOURS IN (10, 20, 40) !
  CREATE VIEW UPDATE_v3   
            AS SELECT * 
                     FROM WORKS 
                     WHERE PNUM BETWEEN 'P2' AND 'P4' 
                     AND EMPNUM NOT BETWEEN 'E2' AND 'E3' !
  CREATE VIEW UPDATE_v4   
            AS SELECT PNUM, EMPNUM  
                     FROM WORKS 
                     WHERE PNUM LIKE '_2%' !
  CREATE VIEW UPDATE_v5  
            AS SELECT * 
                     FROM STAFF 
                     WHERE EMPNAME IS NOT NULL AND CITY IS NULL !
  CREATE VIEW UPDATE_v6  
            AS SELECT EMPNAME, CITY, GRADE 
                     FROM STAFF 
                     WHERE EMPNAME >= 'Betty' AND EMPNUM < 'E35' 
                       OR CITY <= 'Deale' AND GRADE > 12 
                       OR GRADE = 13 AND CITY <> 'Akron' !
  CREATE VIEW UPDATE_v7  
            AS SELECT EMPNAME, CITY, GRADE 
                     FROM STAFFV2 
                     WHERE EMPNAME >= 'Betty' AND EMPNUM < 'E35' 
                       OR CITY <= 'Deale' AND GRADE > 12 
                       OR GRADE = 13 AND CITY <> 'Akron' !
  CREATE VIEW UPDATE_v8  
            AS SELECT MYTABLE.EMPNUM, MYTABLE.EMPNAME 
                     FROM STAFF MYTABLE 
                     WHERE MYTABLE.GRADE = 12 !
  CREATE VIEW UPDATE_v9  
            AS SELECT EMPNAME, CITY, GRADE 
                     FROM STAFF 
                     WHERE NOT EMPNAME >= 'Betty' AND EMPNUM <= 'E35' 
                       OR NOT (CITY <= 'Deale') AND GRADE > 9 
                       AND NOT (GRADE = 13 AND CITY <> 'Akron') 
                       OR NOT CITY IN ('Vienna','New York','Deale') !
   CREATE VIEW VSTAFF3 AS SELECT * FROM STAFF3 !
 * ************* grant statements follow ************* !
   GRANT SELECT ON HU.ECCO TO PUBLIC !
   GRANT INSERT ON testrep TO PUBLIC WITH GRANT OPTION !
   GRANT ALL PRIVILEGES ON testrep TO SUN WITH GRANT OPTION !
   GRANT SELECT ON STAFF TO SULLIVAN1 WITH GRANT OPTION !
   GRANT ALL PRIVILEGES ON STAFF 
           TO PUBLIC !
   GRANT SELECT ON WORKS 
           TO PUBLIC !
   GRANT SELECT ON PROJ 
           TO PUBLIC !
   GRANT SELECT,UPDATE ON WORKS 
           TO CUGINI 
           WITH GRANT OPTION !
   GRANT SELECT,UPDATE ON STAFF2 
           TO SULLIVAN !
   GRANT SELECT,UPDATE(EMPNUM,EMPNAME) ON STAFF3 
           TO SULLIVAN, CUGINI, FLATER, SULLIVAN1 
           WITH GRANT OPTION !
   GRANT SELECT,INSERT, DELETE ON STAFF4 
           TO SULLIVAN, CUGINI, FLATER, SULLIVAN1 !
   GRANT INSERT ON UPUNIQ TO SULLIVAN !
   GRANT SELECT,UPDATE(EMPNUM,EMPNAME) ON VSTAFF3 
    TO FLATER WITH GRANT OPTION !
   GRANT ALL PRIVILEGES ON HH TO SCHANZLE !
   GRANT ALL PRIVILEGES ON FF TO SCHANZLE !
   GRANT ALL PRIVILEGES ON P1 TO SCHANZLE !
 * ************* End of Schema ************* !
