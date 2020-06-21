* NIST SQL Test Suite, V4.0, Schema Definition, schema8.std !
* 59-byte ID !
* TEd Version # !
* date_time print !
* *************************************************************** !
* ****** THIS FILE SHOULD BE RUN UNDER AUTHORIZATION ID SUN ***** !
* *************************************************************** !
* This file defines the base tables used in most of the CDR tests. !
* This is a standard schema definition. !
  CREATE SCHEMA 
      AUTHORIZATION SUN !
  CREATE TABLE SUN.ECCO (C1 CHAR(2)) !
  CREATE TABLE STAFF 
   (EMPNUM   CHAR(3) NOT NULL,
    EMPNAME  CHAR(20),
    GRADE    DECIMAL(4),
    CITY     CHAR(15)) !
  CREATE TABLE PROJ 
   (PNUM     CHAR(3) NOT NULL,
    PNAME    CHAR(20),
    PTYPE    CHAR(6),
    BUDGET   DECIMAL(9),
    CITY     CHAR(15))  !
  CREATE TABLE WORKS 
   (EMPNUM   CHAR(3) NOT NULL,
    PNUM     CHAR(3) NOT NULL,
    HOURS    DECIMAL(5)) !
  CREATE TABLE STAFF3 
   (EMPNUM   CHAR(3) NOT NULL,
    EMPNAME  CHAR(20),
    GRADE    DECIMAL(4),
    CITY     CHAR(15),
    constraint c1_staff3 UNIQUE (EMPNUM)) !
  CREATE TABLE PROJ3 
   (PNUM     CHAR(3) NOT NULL,
    PNAME    CHAR(20),
    PTYPE    CHAR(6),
    BUDGET   DECIMAL(9),
    CITY     CHAR(15),
    constraint c1_proj3 UNIQUE (PNUM))  !
  CREATE TABLE WORKS3 
   (EMPNUM   CHAR(3) NOT NULL,
    PNUM     CHAR(3) NOT NULL,
    HOURS    DECIMAL(5),
    FOREIGN KEY (EMPNUM) REFERENCES STAFF3(EMPNUM),
    FOREIGN KEY (PNUM) REFERENCES PROJ3(PNUM)) !
        CREATE TABLE STAFF4 (EMPNUM    CHAR(3) NOT NULL,
                EMPNAME  CHAR(20) DEFAULT NULL,
                GRADE DECIMAL(4) DEFAULT 0,
                CITY   CHAR(15) DEFAULT '               ') !
 * EMPNAME CHAR precision may be changed to implementation-defined 
 *              precision for value of USER !
        CREATE TABLE STAFF14 (EMPNUM    CHAR(3) NOT NULL,
                EMPNAME  CHAR(20) DEFAULT USER,
                GRADE DECIMAL(4),
                CITY   CHAR(15)) !
        CREATE TABLE STAFF5 (EMPNUM    CHAR(3) NOT NULL,
                EMPNAME  CHAR(20),
                GRADE DECIMAL(4),
                CITY   CHAR(15),
                PRIMARY KEY (EMPNUM),
                CHECK (GRADE > 0 AND GRADE < 20)) !
        CREATE TABLE STAFF6 (EMPNUM    CHAR(3) NOT NULL,
                EMPNAME  CHAR(20),
                GRADE DECIMAL(4) CHECK (GRADE > 0 AND GRADE < 20),
                CITY   CHAR(15)) !
        CREATE TABLE STAFF7 (EMPNUM    CHAR(3) NOT NULL,
                EMPNAME  CHAR(20),
                GRADE DECIMAL(4),
                CITY   CHAR(15),
                PRIMARY KEY (EMPNUM),
                CHECK (GRADE BETWEEN 1 AND 20)) !
        CREATE TABLE STAFF8 (EMPNUM    CHAR(3) NOT NULL,
                EMPNAME  CHAR(20),
                GRADE DECIMAL(4),
                CITY   CHAR(15),
                PRIMARY KEY (EMPNUM),
                CHECK (EMPNAME IS NOT NULL)) !
        CREATE TABLE STAFF9 (EMPNUM    CHAR(3) NOT NULL PRIMARY KEY,
                EMPNAME  CHAR(20),
                GRADE DECIMAL(4),
                CITY   CHAR(15),
                CHECK (EMPNAME NOT LIKE 'T%')) !
        CREATE TABLE STAFF10 (EMPNUM    CHAR(3) NOT NULL,
                EMPNAME  CHAR(20),
                GRADE DECIMAL(4),
                CITY   CHAR(15),
                PRIMARY KEY (EMPNUM),
                CHECK (GRADE NOT IN (5,22))) !
        CREATE TABLE STAFF11 (EMPNUM    CHAR(3) NOT NULL PRIMARY KEY,
                EMPNAME  CHAR(20),
                GRADE DECIMAL(4),
                CITY   CHAR(15),
                CHECK (GRADE NOT IN (5,22)  
                            AND EMPNAME NOT LIKE 'T%')) !
        CREATE TABLE STAFF12 (EMPNUM    CHAR(3) NOT NULL,
                EMPNAME  CHAR(20),
                GRADE DECIMAL(4),
                CITY   CHAR(15),
                PRIMARY KEY (EMPNUM),
                CHECK (NOT GRADE IN (5,22)  
                            AND NOT EMPNAME LIKE 'T%')) !
        CREATE TABLE STAFF13 (EMPNUM    CHAR(3) NOT NULL,
                EMPNAME  CHAR(20),
                GRADE DECIMAL(4),
                CITY   CHAR(15),
                PRIMARY KEY (EMPNUM),
                CHECK (NOT EMPNAME IS NULL)) !
        CREATE TABLE STAFF15 (EMPNUM    CHAR(3),
                EMPNAME  CHAR(20) NOT NULL,
                GRADE DECIMAL(4),
                CITY   CHAR(15)) !
        CREATE TABLE STAFF16 (EMPNUM    CHAR(3) NOT NULL,
                EMPNAME  CHAR(20) DEFAULT NULL,
                GRADE DECIMAL(4) NOT NULL CHECK (GRADE IN (100,150,200)),
                CITY   CHAR(15), PRIMARY KEY (GRADE,EMPNUM)) !
        CREATE TABLE SIZ1_P 
           (S1   CHAR(3) NOT NULL,
            S2   CHAR(3) NOT NULL,
            S3   DECIMAL(4) NOT NULL,
            S4   CHAR(3) NOT NULL,
            S5   DECIMAL(4) NOT NULL,
            S6   CHAR(3) NOT NULL,
            R1   CHAR(3),
            R2   CHAR(3),
            R3   DECIMAL(4),
            constraint c1_siz1_p UNIQUE   (S1,S2,S3,S4,S5,S6)) !
        CREATE TABLE SIZ1_F 
           (F1   CHAR(3) NOT NULL,
            F2   CHAR(3),
            F3   DECIMAL(4),
            F4   CHAR(3),
            F5   DECIMAL(4),
            F6   CHAR(3),
            R1   CHAR(3),
            R2   DECIMAL(5),
            R3   DECIMAL(4),
            FOREIGN KEY   (F1,F2,F3,F4,F5,F6) 
            REFERENCES SIZ1_P(S1,S2,S3,S4,S5,S6)) !
        CREATE TABLE SIZ2_P 
           (P1   CHAR(3) NOT NULL,
            P2   CHAR(3) NOT NULL,
            P3   DECIMAL(4) NOT NULL,
            P4   CHAR(3) NOT NULL,
            P5   DECIMAL(4) NOT NULL,
            P6   CHAR(3) NOT NULL,
            P7   CHAR(3) NOT NULL,
            P8   DECIMAL(4) NOT NULL,
            P9   DECIMAL(4) NOT NULL,
            P10   DECIMAL(4) NOT NULL,
            P11   CHAR(4),
            constraint c01_siz2_p UNIQUE (P1),
            constraint c02_siz2_p UNIQUE (P2),
            constraint c03_siz2_p UNIQUE (P3),
            constraint c04_siz2_p UNIQUE (P4),
            constraint c05_siz2_p UNIQUE (P5),
            constraint c06_siz2_p UNIQUE (P6),
            constraint c07_siz2_p UNIQUE (P7),
            constraint c08_siz2_p UNIQUE (P8),
            constraint c09_siz2_p UNIQUE (P9),
            constraint c10_siz2_p UNIQUE (P10)) !
        CREATE TABLE SIZ2_F1 
           (F1   CHAR(3) NOT NULL,
            F2   CHAR(8),
            FOREIGN KEY (F1)  
            REFERENCES SIZ2_P(P1)) !
        CREATE TABLE SIZ2_F2 
           (F1   CHAR(3) NOT NULL,
            F2   CHAR(8),
            FOREIGN KEY (F1)  
            REFERENCES SIZ2_P(P2)) !
        CREATE TABLE SIZ2_F3 
           (F1   DECIMAL(4) NOT NULL,
            F2   CHAR(8),
            FOREIGN KEY (F1)  
            REFERENCES SIZ2_P(P3)) !
        CREATE TABLE SIZ2_F4 
           (F1   CHAR(3) NOT NULL,
            F2   CHAR(8),
            FOREIGN KEY (F1)  
            REFERENCES SIZ2_P(P4)) !
        CREATE TABLE SIZ2_F5 
           (F1   DECIMAL(4) NOT NULL,
            F2   CHAR(8),
            FOREIGN KEY (F1)  
            REFERENCES SIZ2_P(P5)) !
        CREATE TABLE SIZ2_F6 
           (F1   CHAR(3) NOT NULL,
            F2   CHAR(8),
            FOREIGN KEY (F1)  
            REFERENCES SIZ2_P(P6)) !
        CREATE TABLE SIZ2_F7 
           (F1   CHAR(3) NOT NULL,
            F2   CHAR(8),
            FOREIGN KEY (F1)  
            REFERENCES SIZ2_P(P7)) !
        CREATE TABLE SIZ2_F8 
           (F1   DECIMAL(4) NOT NULL,
            F2   CHAR(8),
            FOREIGN KEY (F1)  
            REFERENCES SIZ2_P(P8)) !
        CREATE TABLE SIZ2_F9 
           (F1   DECIMAL(4) NOT NULL,
            F2   CHAR(8),
            FOREIGN KEY (F1)  
            REFERENCES SIZ2_P(P9)) !
        CREATE TABLE SIZ2_F10 
           (F1   DECIMAL(4) NOT NULL,
            F2   CHAR(8),
            FOREIGN KEY (F1)  
            REFERENCES SIZ2_P(P10)) !
        CREATE TABLE SIZ3_P1 
           (F1   CHAR(3) NOT NULL,
            F2   CHAR(8),
            constraint c1_siz3_p1 UNIQUE (F1)) !
        CREATE TABLE SIZ3_P2 
           (F1   CHAR(3) NOT NULL,
            F2   CHAR(8),
            constraint c1_siz3_p2 UNIQUE (F1)) !
        CREATE TABLE SIZ3_P3 
           (F1   DECIMAL(4) NOT NULL,
            F2   CHAR(8),
            constraint c1_siz3_p3 UNIQUE (F1)) !
        CREATE TABLE SIZ3_P4 
           (F1   CHAR(3) NOT NULL,
            F2   CHAR(8),
            constraint c1_siz3_p4 UNIQUE (F1)) !
        CREATE TABLE SIZ3_P5 
           (F1   DECIMAL(4) NOT NULL,
            F2   CHAR(8),
            constraint c1_siz3_p5 UNIQUE (F1)) !
        CREATE TABLE SIZ3_P6 
           (F1   CHAR(3) NOT NULL,
            F2   CHAR(8),
            constraint c1_siz3_p6 UNIQUE (F1)) !
        CREATE TABLE SIZ3_P7 
           (F1   CHAR(3) NOT NULL,
            F2   CHAR(8),
            constraint c1_siz3_p7 UNIQUE (F1)) !
        CREATE TABLE SIZ3_P8 
           (F1   DECIMAL(4) NOT NULL,
            F2   CHAR(8),
            constraint c1_siz3_p8 UNIQUE (F1)) !
        CREATE TABLE SIZ3_P9 
           (F1   DECIMAL(4) NOT NULL,
            F2   CHAR(8),
            constraint c1_siz3_p9 UNIQUE (F1)) !
        CREATE TABLE SIZ3_P10 
           (F1   DECIMAL(4) NOT NULL,
            F2   CHAR(8),
            constraint c1_siz3_p10 UNIQUE (F1)) !
        CREATE TABLE SIZ3_F 
           (P1   CHAR(3) NOT NULL,
            P2   CHAR(3),
            P3   DECIMAL(4),
            P4   CHAR(3),
            P5   DECIMAL(4),
            P6   CHAR(3),
            P7   CHAR(3),
            P8   DECIMAL(4),
            P9   DECIMAL(4),
            P10   DECIMAL(4),
            P11   CHAR(4),
            FOREIGN KEY (P1) 
            REFERENCES SIZ3_P1(F1),
            FOREIGN KEY (P2) 
            REFERENCES SIZ3_P2(F1),
            FOREIGN KEY (P3) 
            REFERENCES SIZ3_P3(F1),
            FOREIGN KEY (P4) 
            REFERENCES SIZ3_P4(F1),
            FOREIGN KEY (P5) 
            REFERENCES SIZ3_P5(F1),
            FOREIGN KEY (P6) 
            REFERENCES SIZ3_P6(F1),
            FOREIGN KEY (P7) 
            REFERENCES SIZ3_P7(F1),
            FOREIGN KEY (P8) 
            REFERENCES SIZ3_P8(F1),
            FOREIGN KEY (P9) 
            REFERENCES SIZ3_P9(F1),
            FOREIGN KEY (P10) 
            REFERENCES SIZ3_P10(F1)) !
        CREATE TABLE DEPT 
                (DNO DECIMAL(4) NOT NULL,
                 DNAME CHAR(20) NOT NULL,
                 DEAN CHAR(30),
                 PRIMARY KEY (DNO),
                 constraint c1_dept UNIQUE (DNAME)) !
        CREATE TABLE EMP 
                (ENO DECIMAL(4) NOT NULL,
                 ENAME CHAR(20) NOT NULL,
                 EDESC CHAR(30),
                 DNO DECIMAL(4) NOT NULL,
                 DNAME CHAR(20),
                 BTH_DATE  DECIMAL(6) NOT NULL,
                 PRIMARY KEY (ENO),
                 constraint c1_emp UNIQUE (ENAME,BTH_DATE),
                 FOREIGN KEY (DNO) REFERENCES 
                 DEPT(DNO),
                 FOREIGN KEY (DNAME) REFERENCES 
                 DEPT(DNAME)) !
        CREATE TABLE experienc 
                (EXP_NAME CHAR(20),
                 BTH_DATE DECIMAL(6),
                 WK_DATE  DECIMAL(6),
                 DESCR CHAR(40),
                 FOREIGN KEY (EXP_NAME,BTH_DATE) REFERENCES 
                 EMP(ENAME,BTH_DATE)) !
 * The following tables, STAFF_M and PROJ_M reference each other.
 * Table STAFF_M has a "forward reference" to PROJ_M. !
        CREATE TABLE STAFF_M 
           (EMPNUM   CHAR(3) NOT NULL,
            EMPNAME  CHAR(20),
            GRADE    DECIMAL(4),
            CITY     CHAR(15),
            PRI_WK   CHAR(3),
            constraint c1_staff_m UNIQUE   (EMPNUM),
            FOREIGN KEY (PRI_WK) 
            REFERENCES PROJ_M(PNUM)) !
        CREATE TABLE PROJ_M 
           (PNUM     CHAR(3) NOT NULL,
            PNAME    CHAR(20),
            PTYPE    CHAR(6),
            BUDGET   DECIMAL(9),
            CITY     CHAR(15),
            MGR   CHAR(3),
            constraint c1_proj_p UNIQUE (PNUM),
            FOREIGN KEY (MGR) 
            REFERENCES STAFF_M(EMPNUM)) !
 *     The following table is self-referencing. !
        CREATE TABLE STAFF_C 
           (EMPNUM   CHAR(3) NOT NULL,
            EMPNAME  CHAR(20),
            GRADE    DECIMAL(4),
            CITY     CHAR(15),
            MGR   CHAR(3),
            constraint c1_staff_c UNIQUE   (EMPNUM),
            FOREIGN KEY (MGR) 
            REFERENCES STAFF_C(EMPNUM)) !
        CREATE TABLE STAFF_P 
           (EMPNUM   CHAR(3) NOT NULL,
            EMPNAME  CHAR(20),
            GRADE    DECIMAL(4),
            CITY     CHAR(15),
            constraint c1_staff_p UNIQUE  (EMPNUM)) !
        CREATE TABLE PROJ_P 
           (PNUM     CHAR(3) NOT NULL,
            PNAME    CHAR(20),
            PTYPE    CHAR(6),
            BUDGET   DECIMAL(9),
            CITY     CHAR(15),
            constraint c1_proj_p UNIQUE   (PNUM)) !
        CREATE TABLE MID1 (P_KEY DECIMAL(4) NOT NULL,
                         constraint c1_mid1 unique(p_key),
                         F_KEY DECIMAL(4) REFERENCES MID1(P_KEY)) !
        CREATE TABLE ACR_SCH_P(P1 DECIMAL(4) NOT NULL,
                               constraint c1_ACR_SCH_P unique(p1), 
                               P2 CHAR(4)) !
   CREATE TABLE char_def 
               (SEX_CODE  CHAR(1)  DEFAULT 'F',
                NICKNAME  CHAR(20) DEFAULT 'No nickname given',
                insuranc1 CHAR(5)  DEFAULT 'basic') !
   CREATE TABLE EXACT_DEF 
               (BODY_TEMP NUMERIC(4,1) DEFAULT 98.6,
                MAX_NUM   NUMERIC(5)   DEFAULT -55555,
                MIN_NUM   DEC(6,6)     DEFAULT .000001) !
   CREATE TABLE appr_def 
               (X_COUNT   REAL DEFAULT 1.78E12,
                Y_COUNT   REAL DEFAULT -9.99E10,
                Z_COUNT   REAL DEFAULT 3.45E-11,
                ZZ_COUNT  REAL DEFAULT -7.6777E-7) !
   CREATE TABLE SIZE_TAB 
               (COL1 CHAR(75)  DEFAULT  
'ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz0123456789012',
                COL2 INTEGER   DEFAULT -999888777,
                COL3 DEC(15,6) DEFAULT 987654321.123456,
                COL4 REAL      DEFAULT -1.048576E22)    !
   CREATE TABLE COMMODITY 
         (C_NUM INTEGER NOT NULL,
          C_NAME CHAR(7) NOT NULL,
          constraint c1_commodity unique(C_NAME),
          PRIMARY KEY (C_NUM)) !
   CREATE TABLE curr_tab 
         (CURRENCY CHAR(10) NOT NULL,
          dol_equiv NUMERIC(5, 2),
          PRIMARY KEY (CURRENCY)) !
   CREATE TABLE measure_t 
         (MEASURE CHAR(8) NOT NULL,
          pou_equiv NUMERIC(8,2),
          PRIMARY KEY (MEASURE)) !
   CREATE TABLE c_transac  
         (COMMOD_NO INTEGER,
          TOT_PRICE     DECIMAL(12,2),
          CURRENCY  CHAR(10),
          UNITS     INTEGER,
          MEASURE   CHAR(8),
          T_DATE    INTEGER,
          FOREIGN KEY (COMMOD_NO) 
           REFERENCES COMMODITY,
          FOREIGN KEY (CURRENCY) 
           REFERENCES curr_tab,
          FOREIGN KEY (MEASURE) 
           REFERENCES measure_t) !
 * ********************* create view statements ******************** !
CREATE VIEW testrep AS 
    SELECT TESTNO, RESULT, TESTTYPE 
    FROM HU.testrep !
   CREATE VIEW dol_per_p (COMMODITY, u_price, FROM_DATE, TO_DATE) 
      AS SELECT COMMODITY.C_NAME,  
                SUM(TOT_PRICE * dol_equiv) / SUM(UNITS * pou_equiv),
                MIN(T_DATE), MAX(T_DATE) 
         FROM c_transac, COMMODITY, curr_tab, measure_t 
         WHERE c_transac.COMMOD_NO = COMMODITY.C_NUM 
            AND c_transac.CURRENCY = curr_tab.CURRENCY 
            AND c_transac.MEASURE  = measure_t.MEASURE 
         GROUP BY COMMODITY.C_NAME 
         HAVING SUM(TOT_PRICE * dol_equiv) > 10000 !
 * View cost_p_u for OPTIONAL test 0403
 * Remove view from schema if it causes errors. !
   CREATE VIEW cost_p_u 
     (COMMODITY, u_price, CURRENCY, MEASURE) 
      AS SELECT COMMODITY, u_price * pou_equiv / dol_equiv,
                CURRENCY, MEASURE 
            FROM dol_per_p, curr_tab, measure_t !
        CREATE VIEW st6_w_gra AS 
                          SELECT EMPNUM,EMPNAME,GRADE,CITY 
                          FROM STAFF6 
                          WHERE GRADE > 0 AND GRADE < 20 
                          WITH CHECK OPTION !
 * ************** grant statements follow ************* !
        GRANT SELECT ON SUN.ECCO TO PUBLIC !
        GRANT INSERT ON testrep 
             TO PUBLIC !
        GRANT REFERENCES ON ACR_SCH_P TO SULLIVAN 
              WITH GRANT OPTION !
        GRANT ALL PRIVILEGES ON PROJ_P 
             TO SULLIVAN !
 *  Test GRANT without grant permission below. 
 *  "WITH GRANT OPTION" purposefully omitted from SUN''s GRANT.  
 *  Do not insert text "WITH GRANT OPTION" !
        GRANT REFERENCES ON STAFF_P 
             TO SULLIVAN !
        GRANT REFERENCES (C_NUM) ON COMMODITY TO SCHANZLE !
 * ************* End of Schema ************* !
